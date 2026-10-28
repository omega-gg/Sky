//=================================================================================================
/*
    Copyright (C) 2015-2026 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkMedia.

    - GNU Lesser General Public License Usage:
    This file may be used under the terms of the GNU Lesser General Public License version 3 as
    published by the Free Software Foundation and appearing in the LICENSE.md file included in the
    packaging of this file. Please review the following information to ensure the GNU Lesser
    General Public License requirements will be met: https://www.gnu.org/licenses/lgpl.html.

    - Private License Usage:
    Sky kit licensees holding valid private licenses may use this file in accordance with the
    private license agreement provided with the Software or, alternatively, in accordance with the
    terms contained in written agreement between you and Sky kit authors. For further information
    contact us at contact@omega.gg.
*/
//=================================================================================================

#include "WFfmpegPlayer.h"

#ifndef SK_NO_FFMPEGPLAYER

// Qt includes
#include <QCoreApplication>
#include <QThread>
#include <QTimer>
#include <QEventLoop>
#include <QUrl>
#include <QMediaDevices>
#include <QAudioDevice>
#include <QAudioSink>
#include <QDeadlineTimer>
#ifndef SK_NO_QML
#include <QQuickWindow>
#endif
#ifdef Q_OS_ANDROID
#include <QJniEnvironment>
#endif
#ifdef Q_OS_LINUX
#include <QFile>
#endif
#ifdef Q_OS_ANDROID
#include <QDir>
#include <QStandardPaths>
#endif

// FFmpeg includes
extern "C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/hwcontext.h>
#include <libavutil/pixdesc.h>
#include <libavutil/channel_layout.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavfilter/avfilter.h>
#include <libavfilter/buffersrc.h>
#ifdef Q_OS_ANDROID
#include <libavcodec/jni.h>
#endif
#include <libavfilter/buffersink.h>
}

//-------------------------------------------------------------------------------------------------
// Static variables

static const int FFMPEGPLAYER_MAX_WIDTH  = 5760;
static const int FFMPEGPLAYER_MAX_HEIGHT = 3240;

// NOTE: An input reads up to 4 seconds of packets ahead, and a playback that waits for packets
//       goes on with one second of them.
static const int FFMPEGPLAYER_QUEUE_MAXIMUM = 4000;
static const int FFMPEGPLAYER_QUEUE_READY   = 1000;

// NOTE: Decoding toward a seek, we skip the frames nothing refers to until this close to it.
static const int FFMPEGPLAYER_SKIP = 500;

// NOTE: The frames decoded ahead, so a slow one, as a copy from the GPU, never comes late.
static const int FFMPEGPLAYER_FRAMES = 3;

// NOTE: Behind the clock we drop the frames late by two, but never more than five in a row.
static const int FFMPEGPLAYER_DROP_LATE  = 2;
static const int FFMPEGPLAYER_DROP_COUNT = 5;

// NOTE: The audio device holds 100 milliseconds, filled every 10. The clock follows the audio
//       once they drift apart by more than 20 milliseconds.
static const int FFMPEGPLAYER_AUDIO_BUFFER = 100;
static const int FFMPEGPLAYER_AUDIO_TICK   =  10;
static const int FFMPEGPLAYER_AUDIO_DRIFT  =  20;

// NOTE: Without video, the time goes out every 100 milliseconds of audio.
static const int FFMPEGPLAYER_AUDIO_TIME = 100;

// NOTE: Android decodes on the hardware through the MediaCodec decoders (see openVideo).
#ifdef Q_OS_WIN
static const AVHWDeviceType FFMPEGPLAYER_DEVICE = AV_HWDEVICE_TYPE_D3D11VA;
#elif defined(Q_OS_MACOS) || defined(Q_OS_IOS)
static const AVHWDeviceType FFMPEGPLAYER_DEVICE = AV_HWDEVICE_TYPE_VIDEOTOOLBOX;
#elif defined(Q_OS_ANDROID)
static const AVHWDeviceType FFMPEGPLAYER_DEVICE = AV_HWDEVICE_TYPE_NONE;
#else
static const AVHWDeviceType FFMPEGPLAYER_DEVICE = AV_HWDEVICE_TYPE_VAAPI;
#endif

// NOTE: The GPU filter bringing a frame of more than 8 bits to 8 before its copy to memory
//       (see applyDepth).
#ifdef Q_OS_WIN
static const char * FFMPEGPLAYER_DEPTH = "scale_d3d11=format=nv12";
#elif defined(Q_OS_LINUX) && defined(Q_OS_ANDROID) == false
static const char * FFMPEGPLAYER_DEPTH = "scale_vaapi=format=nv12";
#else
static const char * FFMPEGPLAYER_DEPTH = NULL;
#endif

static const AVRational FFMPEGPLAYER_MSEC = { 1, 1000 };

// NOTE: The chunk a source in memory is read with.
static const int FFMPEGPLAYER_CACHE = 65536;

// NOTE: During playback the time goes out every 250 milliseconds, as VLC's: what shows it does
//       not need every frame.
static const int FFMPEGPLAYER_TIME = 250;

// NOTE: The frames are timed on the swaps of the window up to 100 milliseconds after the last
//       one (in nanoseconds).
static const qint64 FFMPEGPLAYER_SWAP = 100000000;

//-------------------------------------------------------------------------------------------------
// Static functions

// NOTE: The hardware format when the decoder offers it, otherwise the first software one. A
//       decoder with no software format fails, and we decode in software (see applySoftware).
static AVPixelFormat getFormat(AVCodecContext * context, const AVPixelFormat * formats)
{
    WFfmpegPlayerPrivate * d = static_cast<WFfmpegPlayerPrivate *> (context->opaque);

    for (const AVPixelFormat * format = formats; *format != AV_PIX_FMT_NONE; format++)
    {
        if (*format == d->formatHardware) return *format;
    }

    for (const AVPixelFormat * format = formats; *format != AV_PIX_FMT_NONE; format++)
    {
        const AVPixFmtDescriptor * descriptor = av_pix_fmt_desc_get(*format);

        if ((descriptor->flags & AV_PIX_FMT_FLAG_HWACCEL) == 0) return *format;
    }

    return AV_PIX_FMT_NONE;
}

#ifndef Q_OS_ANDROID

// NOTE: The decoding device of the graphics API Qt renders with, none when it has none: with two
//       APIs on the same GPU, the copies to memory wait about twice as long.
static AVHWDeviceType getDevice()
{
#if defined(SK_NO_QML) == false && (defined(Q_OS_WIN) || defined(Q_OS_LINUX))
    if (QQuickWindow::graphicsApi() == QSGRendererInterface::Vulkan)
    {
        return AV_HWDEVICE_TYPE_VULKAN;
    }
#endif

    return AV_HWDEVICE_TYPE_NONE;
}

// NOTE: The hardware format of a decoder on a device type, none without such a device.
static AVPixelFormat getHardware(const AVCodec * decoder, AVHWDeviceType type)
{
    for (int i = 0; decoder; i++)
    {
        const AVCodecHWConfig * config = avcodec_get_hw_config(decoder, i);

        if (config == NULL) break;

        if ((config->methods & AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX)
            &&
            config->device_type == type) return config->pix_fmt;
    }

    return AV_PIX_FMT_NONE;
}

#endif

static AVSampleFormat getSampleFormat(QAudioFormat::SampleFormat format)
{
    if      (format == QAudioFormat::UInt8) return AV_SAMPLE_FMT_U8;
    else if (format == QAudioFormat::Int16) return AV_SAMPLE_FMT_S16;
    else if (format == QAudioFormat::Int32) return AV_SAMPLE_FMT_S32;
    else                                    return AV_SAMPLE_FMT_FLT;
}

static WFfmpegInput * createInput(const QString & url, bool video, bool audio)
{
    WFfmpegInput * input = new WFfmpegInput;

    input->url = url;

    input->video = video;
    input->audio = audio;

    input->format = NULL;

    input->io = NULL;

    input->cache = NULL;

    input->indexVideo = -1;
    input->indexAudio = -1;

    input->seek = -1;

    input->thread = NULL;

    return input;
}

//=================================================================================================
// WFfmpegCache
//=================================================================================================

// NOTE: A source that can't seek, from a server without byte ranges, is kept in memory as it's
//       read: seeks land in what was read, and a file with its index at its end plays too.
class WFfmpegCache
{
public:
    WFfmpegCache(AVIOContext * source);

    ~WFfmpegCache();

public: // Functions
    void fill(qint64 end);

public: // Static functions
    static int read(void * opaque, uint8_t * buffer, int size);

    static int64_t seek(void * opaque, int64_t offset, int whence);

public: // Variables
    AVIOContext * source;
    AVIOContext * io;

    QByteArray data;

    qint64 position;
};

//-------------------------------------------------------------------------------------------------

WFfmpegCache::WFfmpegCache(AVIOContext * source)
{
    this->source = source;

    uchar * buffer = static_cast<uchar *> (av_malloc(FFMPEGPLAYER_CACHE));

    io = avio_alloc_context(buffer, FFMPEGPLAYER_CACHE, 0, this, read, NULL, seek);

    position = 0;
}

WFfmpegCache::~WFfmpegCache()
{
    av_freep(&(io->buffer));

    avio_context_free(&io);

    avio_closep(&source);
}

//-------------------------------------------------------------------------------------------------

// NOTE: Reads the source on, up to this offset or its end.
void WFfmpegCache::fill(qint64 end)
{
    uchar buffer[FFMPEGPLAYER_CACHE];

    while (data.size() < end)
    {
        int count = avio_read_partial(source, buffer, FFMPEGPLAYER_CACHE);

        if (count <= 0) return;

        data.append(reinterpret_cast<const char *> (buffer), count);
    }
}

//-------------------------------------------------------------------------------------------------

/* static */ int WFfmpegCache::read(void * opaque, uint8_t * buffer, int size)
{
    WFfmpegCache * cache = static_cast<WFfmpegCache *> (opaque);

    cache->fill(cache->position + size);

    int count = qMin(qint64(size), cache->data.size() - cache->position);

    if (count <= 0) return AVERROR_EOF;

    memcpy(buffer, cache->data.constData() + cache->position, count);

    cache->position += count;

    return count;
}

/* static */ int64_t WFfmpegCache::seek(void * opaque, int64_t offset, int whence)
{
    WFfmpegCache * cache = static_cast<WFfmpegCache *> (opaque);

    if (whence == AVSEEK_SIZE) return avio_size(cache->source);

    whence &= ~AVSEEK_FORCE;

    if (whence == SEEK_CUR)
    {
        offset += cache->position;
    }
    else if (whence == SEEK_END)
    {
        qint64 size = avio_size(cache->source);

        // NOTE: Without a size we don't read to the end: a radio has none.
        if (size < 0) return size;

        offset += size;
    }
    else if (whence != SEEK_SET) return AVERROR(EINVAL);

    if (offset < 0) return AVERROR(EINVAL);

    cache->fill(offset);

    cache->position = offset;

    return offset;
}

//=================================================================================================
// WFfmpegAudio
//=================================================================================================

// NOTE: The audio thread: every tick it decodes what the device can take and writes it, then
//       moves the clock to what the device plays.
class WFfmpegAudio
{
public:
    WFfmpegAudio(WFfmpegPlayerPrivate * d, const QAudioDevice & device,
                                           const QAudioFormat & format);

    ~WFfmpegAudio();

public: // Functions
    void tick();

    bool decode ();
    bool convert();

    void clear();

    void applySpeed();

    void createGraph();

public: // Variables
    WFfmpegPlayerPrivate * d;

    QAudioSink sink;

    QIODevice * io;

    QAudioFormat format;

    AVSampleFormat sampleFormat;

    AVChannelLayout layout;

    int rate;
    int bytes;

    AVFrame * frame;
    AVFrame * filtered;

    // NOTE: At another speed the audio goes through atempo.
    AVFilterGraph   * graph;
    AVFilterContext * filterIn;
    AVFilterContext * filterOut;

    qreal speed;

    QByteArray pending;

    int offset;

    int end;
    int target;
    int time;

    // NOTE: The media time the filter started from, and the samples it gave since.
    int    base;
    qint64 samples;

    int volume;

    bool active;
    bool draining;
    bool flushed;
};

//-------------------------------------------------------------------------------------------------

WFfmpegAudio::WFfmpegAudio(WFfmpegPlayerPrivate * d, const QAudioDevice & device,
                                                     const QAudioFormat & format)
    : sink(device, format)
{
    this->d = d;

    this->format = format;

    io = NULL;

    sampleFormat = getSampleFormat(format.sampleFormat());

    av_channel_layout_default(&layout, format.channelCount());

    rate  = format.sampleRate();
    bytes = format.bytesPerFrame();

    frame    = av_frame_alloc();
    filtered = av_frame_alloc();

    graph     = NULL;
    filterIn  = NULL;
    filterOut = NULL;

    speed = 1.0;

    offset = 0;

    end    = -1;
    target = -1;
    time   = -1;

    base    = -1;
    samples = 0;

    volume = -1;

    active   = false;
    draining = false;
    flushed  = false;

    sink.setBufferSize(format.bytesForDuration(FFMPEGPLAYER_AUDIO_BUFFER * 1000));
}

// NOTE Qt6: stop() lets the sink drain in the background, and keeps some memory each time.
//           reset() drops what it holds and frees it.
WFfmpegAudio::~WFfmpegAudio()
{
    sink.reset();

    avfilter_graph_free(&graph);

    av_frame_free(&frame);
    av_frame_free(&filtered);
}

//-------------------------------------------------------------------------------------------------

void WFfmpegAudio::tick()
{
    QMutexLocker locker(&d->mutex);

    // NOTE: After a seek the audio starts on its time at its speed, and stops when it's no
    //       longer active.
    if (d->serialAudio != d->serial || active != d->audio)
    {
        if (d->serialAudio != d->serial)
        {
            d->serialAudio = d->serial;

            target = d->seekAudio;
        }

        active = d->audio;

        speed = d->speed;

        locker.unlock();

        clear();

        if (d->codecVideo == NULL && target != -1)
        {
            time = target;

            d->post(WFfmpegPlayer::EventTimeChanged, time);
        }

        locker.relock();
    }
    // NOTE: Another speed goes on from where we decode, through atempo.
    else if (speed != d->speed)
    {
        speed = d->speed;

        locker.unlock();

        applySpeed();

        locker.relock();
    }

    // NOTE: After a seek the audio waits for the frame of the video, so both start together.
    if (active == false || d->playing == false || (d->codecVideo && d->seekVideo != -1))
    {
        // NOTE: The audio is over, the packets left are dropped.
        if (active == false)
        {
            d->clearQueue(d->queueAudio);

            d->condition.wakeAll();
        }

        locker.unlock();

        if (io) sink.suspend();

        return;
    }

    int percent = d->volume;

    locker.unlock();

    if (io == NULL)
    {
         io = sink.start();
    }
    else if (sink.state() == QAudio::SuspendedState) sink.resume();

    if (volume != percent)
    {
        volume = percent;

        sink.setVolume(percent / 100.0);
    }

    int free = sink.bytesFree();

    while (free >= bytes)
    {
        if (offset == pending.size())
        {
            if (decode() == false) break;

            continue;
        }

        int count = qMin(free, pending.size() - offset);

        count -= count % bytes;

        io->write(pending.constData() + offset, count);

        offset += count;

        free -= count;
    }

    int buffered = sink.bufferSize() - sink.bytesFree() + pending.size() - offset;

    locker.relock();

    if (d->serialAudio != d->serial || d->audio == false) return;

    if (draining && buffered == 0)
    {
        // NOTE: The audio is over, the clock goes on alone.
        d->audioEnded = true;

        d->applyAudio();

        if (d->codecVideo == NULL) d->applyEnd();

        return;
    }

    if (end == -1) return;

    // NOTE: What we hear is the end of what we decoded, minus what waits in the device and here,
    //       each millisecond of it being 'speed' milliseconds of media.
    int heard = end - qRound(format.durationForBytes(buffered) / 1000 * speed);

    if (d->clockValid == false
        ||
        qAbs(heard - d->clockTime - d->clock.elapsed() * speed) > FFMPEGPLAYER_AUDIO_DRIFT)
    {
        if (d->clockValid == false)
        {
            d->clockValid = true;

            d->condition.wakeAll();
        }

        d->clock.start();

        d->clockTime = heard;
    }

    if (d->codecVideo == NULL && qAbs(heard - time) >= FFMPEGPLAYER_AUDIO_TIME)
    {
        time = heard;

        d->post(WFfmpegPlayer::EventTimeChanged, time);
    }
}

// NOTE: Returns false when there's nothing to decode right now.
bool WFfmpegAudio::decode()
{
    AVCodecContext * codec = d->codecAudio;

    while (true)
    {
        // NOTE: At another speed the samples come out of atempo.
        if (graph)
        {
            int result = av_buffersink_get_frame(filterOut, filtered);

            if (result >= 0)
            {
                int count = filtered->nb_samples;

                pending = QByteArray(reinterpret_cast<const char *> (filtered->data[0]),
                                     count * bytes);

                av_frame_unref(filtered);

                offset = 0;

                samples += count;

                end = base + qRound(samples * 1000 * speed / rate);

                return true;
            }

            if (result == AVERROR_EOF) return false;
        }

        int result = avcodec_receive_frame(codec, frame);

        if (result == 0)
        {
            if (convert() && graph == NULL) return true;

            continue;
        }

        if (result == AVERROR_EOF)
        {
            // NOTE: The samples atempo holds come out at the end.
            if (graph == NULL || flushed) return false;

            flushed = true;

            if (av_buffersrc_add_frame(filterIn, NULL) < 0) return false;

            continue;
        }

        if (draining) return false;

        QMutexLocker locker(&d->mutex);

        if (d->serialAudio != d->serial) return false;

        WFfmpegQueue & queue = d->queueAudio;

        if (queue.packets.isEmpty())
        {
            if (queue.eof == false)
            {
                if (d->starving == NULL)
                {
                    d->starving = &queue;

                    d->post(WFfmpegPlayer::EventBuffering, 0);
                }

                return false;
            }

            locker.unlock();

            draining = true;

            avcodec_send_packet(codec, NULL);

            continue;
        }

        WFfmpegPacket item = queue.packets.dequeue();

        queue.duration -= item.duration;

        d->condition.wakeAll();

        locker.unlock();

        if (item.serial == d->serialAudio) avcodec_send_packet(codec, item.packet);

        av_packet_free(&item.packet);
    }
}

// NOTE: The decoded samples go to the device format from the seek's time, then to the device,
//       or to atempo at another speed.
bool WFfmpegAudio::convert()
{
    int count = swr_get_out_samples(d->resample, frame->nb_samples);

    QByteArray data(count * bytes, Qt::Uninitialized);

    uint8_t * buffer = reinterpret_cast<uint8_t *> (data.data());

    count = swr_convert(d->resample, &buffer, count,
                        const_cast<const uint8_t **> (frame->extended_data), frame->nb_samples);

    int start = d->getTime(d->baseAudio, frame->best_effort_timestamp);

    av_frame_unref(frame);

    if (count <= 0) return false;

    // NOTE: We keep the samples converted, the buffer was sized for the most we could get.
    data.truncate(count * bytes);

    int skip = 0;

    // NOTE: The audio of a seek starts on its time, to the sample.
    if (target != -1)
    {
        if (start + count * 1000 / rate <= target) return false;

        if (target > start)
        {
            skip = qMin(count, (target - start) * rate / 1000);

            start = target;
        }

        target = -1;
    }

    count -= skip;

    if (graph == NULL)
    {
        pending = data;

        offset = skip * bytes;

        end = start + count * 1000 / rate;

        return true;
    }

    if (base == -1) base = start;

    AVFrame * input = av_frame_alloc();

    input->format      = sampleFormat;
    input->sample_rate = rate;
    input->nb_samples  = count;

    av_channel_layout_copy(&(input->ch_layout), &layout);

    av_frame_get_buffer(input, 0);

    memcpy(input->data[0], data.constData() + skip * bytes, count * bytes);

    int result = av_buffersrc_add_frame(filterIn, input);

    av_frame_free(&input);

    return (result >= 0);
}

void WFfmpegAudio::clear()
{
    avcodec_flush_buffers(d->codecAudio);

    swr_init(d->resample);

    pending.clear();

    offset = 0;

    end = -1;

    draining = false;

    applySpeed();

    if (io == NULL) return;

    sink.reset();

    io = NULL;
}

// NOTE: At 1x the samples go straight to the device.
void WFfmpegAudio::applySpeed()
{
    avfilter_graph_free(&graph);

    base    = -1;
    samples = 0;

    flushed = false;

    if (speed != 1.0) createGraph();
}

// NOTE: atempo goes from 0.5 to 100, slower speeds chain it.
void WFfmpegAudio::createGraph()
{
    char name[64];

    av_channel_layout_describe(&layout, name, sizeof(name));

    QByteArray arguments = QString("sample_rate=%1:sample_fmt=%2:channel_layout=%3:"
                                   "time_base=1/%1").arg(rate)
                                                    .arg(av_get_sample_fmt_name(sampleFormat))
                                                    .arg(name).toLatin1();

    QString filters;

    qreal tempo = speed;

    while (tempo < 0.5)
    {
        filters.append("atempo=0.5,");

        tempo *= 2;
    }

    filters.append("atempo=" + QString::number(tempo, 'f', 6));

    graph = avfilter_graph_alloc();

    AVFilterInOut * inputs  = avfilter_inout_alloc();
    AVFilterInOut * outputs = avfilter_inout_alloc();

    if (avfilter_graph_create_filter(&filterIn, avfilter_get_by_name("abuffer"), "in",
                                     arguments.constData(), NULL, graph) >= 0
        &&
        avfilter_graph_create_filter(&filterOut, avfilter_get_by_name("abuffersink"), "out",
                                     NULL, NULL, graph) >= 0)
    {
        outputs->name       = av_strdup("in");
        outputs->filter_ctx = filterIn;

        inputs->name       = av_strdup("out");
        inputs->filter_ctx = filterOut;

        if (avfilter_graph_parse_ptr(graph, filters.toLatin1().constData(), &inputs, &outputs,
                                     NULL) >= 0
            &&
            avfilter_graph_config(graph, NULL) >= 0)
        {
            avfilter_inout_free(&inputs);
            avfilter_inout_free(&outputs);

            return;
        }
    }

    avfilter_inout_free(&inputs);
    avfilter_inout_free(&outputs);

    avfilter_graph_free(&graph);

    qWarning("WFfmpegAudio::createGraph: Cannot play the audio at this speed.");
}

//=================================================================================================
// WFfmpegPlayerPrivate
//=================================================================================================

WFfmpegPlayerPrivate::WFfmpegPlayerPrivate(WFfmpegPlayer * p) : WPrivate(p) {}

/* virtual */ WFfmpegPlayerPrivate::~WFfmpegPlayerPrivate()
{
    clear();

    av_buffer_unref(&device);

    av_frame_free(&frameDecoded);
    av_frame_free(&frameSoftware);
    av_frame_free(&frameDepth);
}

//-------------------------------------------------------------------------------------------------

void WFfmpegPlayerPrivate::init()
{
#ifdef Q_OS_ANDROID
    // NOTE: MediaCodec runs through Java, FFmpeg needs its VM once.
    static bool jni = (av_jni_set_java_vm(QJniEnvironment::javaVM(), NULL) >= 0);

    Q_UNUSED(jni);
#endif

    backend = NULL;

    setup  = NULL;
    lock   = NULL;
    unlock = NULL;

    loop = false;

    output = WAbstractBackend::OutputMedia;

    session = 0;

    threadDecode = NULL;
    threadVideo  = NULL;
    threadAudio  = NULL;

    opened = 0;

    ready = false;

    queueVideo.duration = 0;
    queueAudio.duration = 0;

    queueVideo.eof = false;
    queueAudio.eof = false;

    serialEnd = -1;

    starving = NULL;

    serial      = 0;
    serialVideo = 0;
    serialAudio = 0;

    seekVideo = -1;
    seekAudio = -1;

    playing = false;
    step    = false;

    speed = 1.0;

    audio      = false;
    audioEnded = false;

    clockTime  = 0;
    clockValid = false;

    timeVideo  =  0;
    timePosted = -1;

    swapTime   = 0;
    swapPeriod = 0;

    slotTime  = 0;
    slotFrame = 0;

    quality = 0;

    hasVariants = false;

    idVideo = -1;
    idAudio = -1;

    volume = 100;

    parameters = NULL;

    codecVideo = NULL;
    codecAudio = NULL;

    device = NULL;

    formatHardware = AV_PIX_FMT_NONE;

    decoded = false;

    accelerated = false;

    frameDecoded  = av_frame_alloc();
    frameSoftware = av_frame_alloc();
    frameDepth    = av_frame_alloc();

    graph     = NULL;
    filterIn  = NULL;
    filterOut = NULL;

    graphFrames = NULL;

    scale    = NULL;
    resample = NULL;

    width  = -1;
    height = -1;

    locked = false;

    frameRate = 0.0;
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

// NOTE: The audio comes from its own source when it has one, otherwise from the video's. The
//       first frame shown is the one at this time, even when the player is paused right away.
void WFfmpegPlayerPrivate::start(int at)
{
    if (output == WAbstractBackend::OutputAudio)
    {
        if (urlAudio.isEmpty())
        {
             inputs.append(createInput(url, false, true));
        }
        else inputs.append(createInput(urlAudio, false, true));
    }
    else if (output == WAbstractBackend::OutputVideo)
    {
        inputs.append(createInput(url, true, false));
    }
    else if (urlAudio.isEmpty())
    {
        inputs.append(createInput(url, true, true));
    }
    else
    {
        inputs.append(createInput(url,      true,  false));
        inputs.append(createInput(urlAudio, false, true));
    }

    seekVideo = qMax(0, at);
    seekAudio = seekVideo;

    // NOTE: The audio starts on its seek, as after any other.
    serialAudio = -1;

    playing = true;

    foreach (WFfmpegInput * input, inputs)
    {
        if (at > 0) input->seek = at;

        input->thread = QThread::create([this, input] { runRead(input); });

        input->thread->start();
    }

    threadDecode = QThread::create([this] { runDecode(); });
    threadVideo  = QThread::create([this] { runVideo (); });
    threadAudio  = QThread::create([this] { runAudio (); });

    threadDecode->start();
    threadVideo ->start();
    threadAudio ->start();
}

// NOTE: The interrupt callback makes FFmpeg give up any read at once, so the threads end quickly.
void WFfmpegPlayerPrivate::clear()
{
    if (inputs.isEmpty()) return;

    mutex.lock();

    abort.storeRelaxed(1);

    condition.wakeAll();

    mutex.unlock();

    threadAudio->quit();

    foreach (WFfmpegInput * input, inputs)
    {
        input->thread->wait();

        delete input->thread;
    }

    threadDecode->wait();
    threadVideo ->wait();
    threadAudio ->wait();

    delete threadDecode;
    delete threadVideo;
    delete threadAudio;

    threadDecode = NULL;
    threadVideo  = NULL;
    threadAudio  = NULL;

    clearQueue(queueVideo);
    clearQueue(queueAudio);

    clearFrames();

    avcodec_parameters_free(&parameters);

    avcodec_free_context(&codecVideo);
    avcodec_free_context(&codecAudio);

    sws_freeContext(scale);

    scale = NULL;

    avfilter_graph_free(&graph);

    graphFrames = NULL;

    swr_free(&resample);

    foreach (WFfmpegInput * input, inputs)
    {
        closeInput(input);

        delete input;
    }

    inputs.clear();

    av_frame_unref(frameDecoded);
    av_frame_unref(frameSoftware);
    av_frame_unref(frameDepth);

    opened = 0;

    ready = false;

    queueVideo.eof = false;
    queueAudio.eof = false;

    serialEnd = -1;

    starving = NULL;

    serial      = 0;
    serialVideo = 0;
    serialAudio = 0;

    seekVideo = -1;
    seekAudio = -1;

    playing = false;
    step    = false;

    audio      = false;
    audioEnded = false;

    clockValid = false;

    timeVideo  =  0;
    timePosted = -1;

    width  = -1;
    height = -1;

    locked = false;

    frameRate = 0.0;

    abort.storeRelaxed(0);
}

//-------------------------------------------------------------------------------------------------

void WFfmpegPlayerPrivate::runRead(WFfmpegInput * input)
{
    bool result = open(input);

    mutex.lock();

    if (result == false)
    {
        mutex.unlock();

        // NOTE: Without its audio source, the video plays alone.
        if (input == inputs.first())
        {
            if (abort.loadRelaxed() == 0) post(WFfmpegPlayer::EventError);

            return;
        }

        if (abort.loadRelaxed() == 0)
        {
            qWarning("WFfmpegPlayerPrivate::runRead: Cannot open the audio.");
        }

        // NOTE: An audio source that failed has no format, whatever step it failed at.
        closeInput(input);

        mutex.lock();
    }

    opened++;

    if (opened == inputs.count()) applyReady();

    mutex.unlock();

    if (result == false) return;

    bool video = (input->indexVideo != -1);
    bool audio = (input->indexAudio != -1);

    AVPacket * packet = av_packet_alloc();

    while (true)
    {
        mutex.lock();

        while (abort.loadRelaxed() == 0 && input->seek == -1
               &&
               (video == false || queueVideo.eof
                ||
                queueVideo.duration >= FFMPEGPLAYER_QUEUE_MAXIMUM)
               &&
               (audio == false || queueAudio.eof
                ||
                queueAudio.duration >= FFMPEGPLAYER_QUEUE_MAXIMUM))
        {
            condition.wait(&mutex);
        }

        if (abort.loadRelaxed())
        {
            mutex.unlock();

            break;
        }

        if (input->seek != -1)
        {
            int msec = input->seek;

            input->seek = -1;

            if (video)
            {
                clearQueue(queueVideo);

                queueVideo.eof = false;
            }

            if (audio)
            {
                clearQueue(queueAudio);

                queueAudio.eof = false;
            }

            mutex.unlock();

            int index = (video) ? input->indexVideo : input->indexAudio;

            qint64 timestamp = getTimestamp(getBase(input->format->streams[index]), msec);

            av_seek_frame(input->format, index, timestamp, AVSEEK_FLAG_BACKWARD);

            continue;
        }

        mutex.unlock();

        int result = av_read_frame(input->format, packet);

        if (result < 0)
        {
            if (abort.loadRelaxed()) break;

            // NOTE: An error past FFmpeg's reconnects ends the stream as its end does.
            if (result != AVERROR_EOF)
            {
                qWarning("WFfmpegPlayerPrivate::runRead: Read error %d.", result);
            }

            mutex.lock();

            if (video) queueVideo.eof = true;
            if (audio) queueAudio.eof = true;

            if (starving && starving->eof)
            {
                starving = NULL;

                post(WFfmpegPlayer::EventBuffering, 100);
            }

            condition.wakeAll();

            mutex.unlock();

            continue;
        }

        WFfmpegQueue * queue;

        if (packet->stream_index == input->indexVideo)
        {
            queue = &queueVideo;
        }
        else if (packet->stream_index == input->indexAudio)
        {
            queue = &queueAudio;
        }
        else
        {
            av_packet_unref(packet);

            continue;
        }

        WFfmpegBase base = getBase(input->format->streams[packet->stream_index]);

        mutex.lock();

        // NOTE: A seek asked meanwhile makes this packet useless.
        if (input->seek == -1)
        {
            WFfmpegPacket item;

            item.packet = av_packet_alloc();

            av_packet_move_ref(item.packet, packet);

            item.serial = serial;

            item.duration = av_rescale(item.packet->duration, (qint64) base.num * 1000, base.den);

            queue->packets.enqueue(item);

            queue->duration += item.duration;

            if (starving == queue && queue->duration >= FFMPEGPLAYER_QUEUE_READY)
            {
                starving = NULL;

                post(WFfmpegPlayer::EventBuffering, 100);
            }

            condition.wakeAll();
        }
        else av_packet_unref(packet);

        mutex.unlock();
    }

    av_packet_free(&packet);
}

// NOTE: Decodes a few frames ahead of the video thread.
void WFfmpegPlayerPrivate::runDecode()
{
    mutex.lock();

    while (abort.loadRelaxed() == 0 && ready == false)
    {
        condition.wait(&mutex);
    }

    mutex.unlock();

    if (codecVideo == NULL) return;

    // NOTE: The serial whose seek has its frame.
    int serialSeek = -1;

    int dropped = 0;

    while (true)
    {
        AVFrame * frame = decodeFrame();

        QMutexLocker locker(&mutex);

        if (abort.loadRelaxed())
        {
            if (frame) av_frame_unref(frame);

            break;
        }

        // NOTE: A seek asked meanwhile, we decode toward it.
        if (serialVideo != serial)
        {
            if (frame) av_frame_unref(frame);

            continue;
        }

        // NOTE: That's the end of the stream, the frames before it go out first.
        if (frame == NULL)
        {
            serialEnd = serial;

            condition.wakeAll();

            while (abort.loadRelaxed() == 0 && serialVideo == serial)
            {
                condition.wait(&mutex);
            }

            continue;
        }

        qint64 timestamp = frame->best_effort_timestamp;

        if (timestamp == AV_NOPTS_VALUE) timestamp = frame->pts;

        bool seek = (seekVideo != -1 && serialSeek != serial);

        // NOTE: The frame shown for a seek is the one its time falls in. Times are whole
        //       milliseconds, so a frame starting within a millisecond after it counts too:
        //       a step of a frame duration lands on the next frame either way.
        if (seek && getTimeNext(frame, timestamp) - 1 <= seekVideo)
        {
            av_frame_unref(frame);

            continue;
        }

        while (abort.loadRelaxed() == 0 && serialVideo == serial
               &&
               frames.count() >= FFMPEGPLAYER_FRAMES)
        {
            condition.wait(&mutex);
        }

        if (abort.loadRelaxed() || serialVideo != serial)
        {
            av_frame_unref(frame);

            continue;
        }

        int time = getTime(baseVideo, timestamp);

        // NOTE: Behind the clock, a late frame is dropped before its copy from the GPU.
        if (seek == false && playing && step == false && clockValid
            &&
            clockTime + clock.elapsed() * speed - time > FFMPEGPLAYER_DROP_LATE * 1000 / frameRate
            &&
            dropped < FFMPEGPLAYER_DROP_COUNT)
        {
            av_frame_unref(frame);

            dropped++;

            continue;
        }

        dropped = 0;

        if (seek) serialSeek = serial;

        locker.unlock();

        if (frame->hw_frames_ctx)
        {
            AVFrame * source = applyDepth(frame);

            int result = av_hwframe_transfer_data(frameSoftware, source, 0);

            if (result >= 0) av_frame_copy_props(frameSoftware, frame);

            if (source != frame) av_frame_unref(source);

            av_frame_unref(frame);

            frame = frameSoftware;

            if (result < 0)
            {
                av_frame_unref(frame);

                locker.relock();

                continue;
            }
        }

        locker.relock();

        if (serialVideo != serial)
        {
            av_frame_unref(frame);

            continue;
        }

        WFfmpegFrame item;

        item.frame = av_frame_alloc();

        av_frame_move_ref(item.frame, frame);

        item.serial = serial;

        item.time = time;

        item.seek = seek;

        frames.enqueue(item);

        condition.wakeAll();
    }
}

// NOTE: Shows the frames on time, the next one is converted ahead into the backend's buffer.
void WFfmpegPlayerPrivate::runVideo()
{
    QMutexLocker locker(&mutex);

    while (abort.loadRelaxed() == 0 && ready == false)
    {
        condition.wait(&mutex);
    }

    if (codecVideo == NULL) return;

    bool prepared = false;

    while (abort.loadRelaxed() == 0)
    {
        // NOTE: The frames from before a seek are dropped, the buffer they took goes to the next.
        while (frames.isEmpty() == false && frames.head().serial != serial)
        {
            AVFrame * frame = frames.dequeue().frame;

            av_frame_free(&frame);

            prepared = false;

            condition.wakeAll();
        }

        if (frames.isEmpty())
        {
            if (serialEnd == serial)
            {
                serialEnd = -1;

                // NOTE: A seek past the last frame stops at the end.
                seekVideo = -1;

                applyEnd();
            }

            condition.wait(&mutex);

            continue;
        }

        WFfmpegFrame item = frames.head();

        // NOTE: The swap slot of the frame, 0 when it goes on its time.
        qint64 slot = 0;

        // NOTE: Behind the clock, a frame whose next one is due already is dropped.
        if (item.seek == false && playing && step == false && clockValid && frames.count() > 1
            &&
            frames.at(1).time <= clockTime + clock.elapsed() * speed)
        {
            frames.dequeue();

            av_frame_free(&(item.frame));

            prepared = false;

            condition.wakeAll();

            continue;
        }

        if (prepared == false)
        {
            locker.unlock();

            prepared = applyFrame(item.frame);

            locker.relock();

            if (prepared == false
                &&
                frames.isEmpty() == false && frames.head().frame == item.frame)
            {
                frames.dequeue();

                av_frame_free(&(item.frame));
            }

            continue;
        }

        // NOTE: The time of a seek, a step or a first frame goes out at once, then every 250 ms.
        bool posted = (item.seek || step || timePosted == -1);

        if (item.seek)
        {
            seekVideo = -1;
        }
        else if (step)
        {
            step = false;
        }
        else if (playing == false)
        {
            condition.wait(&mutex);

            continue;
        }
        else if (clockValid)
        {
            qint64 wait = getWait(item.time, &slot);

            if (wait > 0)
            {
                condition.wait(&mutex, wait);

                continue;
            }
        }
        // NOTE: The audio starts the clock.
        else if (audio)
        {
            condition.wait(&mutex);

            continue;
        }

        // NOTE: Without audio, the clock starts from the first frame shown after a play or a
        //       seek.
        if (playing && clockValid == false && audio == false)
        {
            clock.start();

            clockTime  = item.time;
            clockValid = true;
        }

        timeVideo = item.time;

        if (posted || qAbs(item.time - timePosted) >= FFMPEGPLAYER_TIME)
        {
            posted = true;

            timePosted = item.time;
        }

        slotTime  = slot;
        slotFrame = item.time;

        frames.dequeue();

        prepared = false;

        condition.wakeAll();

        locker.unlock();

        // NOTE: The time goes first, so the frame comes to a player done loading.
        if (posted) post(WFfmpegPlayer::EventTimeChanged, item.time);

        unlock(backend);

        locked = false;

        av_frame_free(&(item.frame));

        locker.relock();
    }
}

void WFfmpegPlayerPrivate::runAudio()
{
    mutex.lock();

    while (abort.loadRelaxed() == 0 && ready == false)
    {
        condition.wait(&mutex);
    }

    mutex.unlock();

    if (codecAudio == NULL) return;

    QAudioDevice device = QMediaDevices::defaultAudioOutput();

    QAudioFormat format = device.preferredFormat();

    format.setSampleFormat(QAudioFormat::Float);

    if (device.isFormatSupported(format) == false) format = device.preferredFormat();

    AVChannelLayout layout;

    av_channel_layout_default(&layout, format.channelCount());

    swr_alloc_set_opts2(&resample, &layout, getSampleFormat(format.sampleFormat()),
                        format.sampleRate(), &(codecAudio->ch_layout), codecAudio->sample_fmt,
                        codecAudio->sample_rate, 0, NULL);

    // NOTE: Without an audio device, the video plays alone.
    if (resample == NULL || swr_init(resample) < 0)
    {
        qWarning("WFfmpegPlayerPrivate::runAudio: Cannot play the audio.");

        swr_free(&resample);

        return;
    }

    WFfmpegAudio audio(this, device, format);

    mutex.lock();

    applyAudio();

    mutex.unlock();

    QTimer timer;

    timer.setTimerType(Qt::PreciseTimer);

    QObject::connect(&timer, &QTimer::timeout, [&audio] { audio.tick(); });

    timer.start(FFMPEGPLAYER_AUDIO_TICK);

    // NOTE: The loop runs until the thread quits.
    QEventLoop loop;

    loop.exec();
}

//-------------------------------------------------------------------------------------------------

bool WFfmpegPlayerPrivate::open(WFfmpegInput * input)
{
    if (openFormat(input) == false) return false;

    AVFormatContext * format = input->format;

    int indexVideo = -1;
    int indexAudio = -1;

    mutex.lock();

    // NOTE: A variant list (HLS) has a program per variant.
    if (input->video) hasVariants = (format->nb_programs > 1);

    int idVideo = this->idVideo;
    int idAudio = this->idAudio;

    int quality = this->quality;

    mutex.unlock();

    if (input->video)
    {
        indexVideo = getStream(format, AVMEDIA_TYPE_VIDEO, idVideo, -1, quality);
    }

    if (input->audio)
    {
        indexAudio = getStream(format, AVMEDIA_TYPE_AUDIO, idAudio, indexVideo, 0);
    }

    if (indexVideo < 0 && indexAudio < 0) return false;

    // NOTE: Each stream belongs to one input, its thread sets it up before the others use it.
    if (indexVideo >= 0)
    {
        AVStream * stream = format->streams[indexVideo];

        baseVideo = getBase(stream);

        frameRate = av_q2d(av_guess_frame_rate(format, stream, NULL));

        parameters = avcodec_parameters_alloc();

        avcodec_parameters_copy(parameters, stream->codecpar);

        if (openVideo(true) == false) return false;

        input->indexVideo = indexVideo;
    }

    if (indexAudio >= 0)
    {
        AVStream * stream = format->streams[indexAudio];

        baseAudio = getBase(stream);

        if (openAudio(stream)) input->indexAudio = indexAudio;
    }

    for (unsigned int i = 0; i < format->nb_streams; i++)
    {
        if ((int) i != input->indexVideo && (int) i != input->indexAudio)
        {
            format->streams[i]->discard = AVDISCARD_ALL;
        }
    }

    return true;
}

bool WFfmpegPlayerPrivate::openFormat(WFfmpegInput * input)
{
    input->format = avformat_alloc_context();

    input->format->interrupt_callback.callback = interrupt;
    input->format->interrupt_callback.opaque   = this;

    AVDictionary * options = NULL;

    // NOTE: A source without byte ranges can't resume, so it does not reconnect: at the end of
    //       a stream without length FFmpeg would take it for a cut.
    av_dict_set(&options, "reconnect",                  "1", 0);
    av_dict_set(&options, "reconnect_on_network_error", "1", 0);
    av_dict_set(&options, "reconnect_delay_max",        "8", 0);

    // NOTE: Seeks reuse the connection instead of opening a new one.
    av_dict_set(&options, "multiple_requests", "1", 0);

    // NOTE: A server as YouTube's serves a long request at about twice the playback speed, and
    //       requests of 10 MB at full speed.
    av_dict_set(&options, "request_size", "10485760", 0);

    av_dict_set(&options, "rw_timeout", "20000000", 0);

    mutex.lock();

    QByteArray proxy = this->proxy.toUtf8();

    mutex.unlock();

    if (proxy.isEmpty() == false)
    {
        av_dict_set(&options, "http_proxy", proxy.constData(), 0);
    }

#ifdef Q_OS_LINUX
    // NOTE: FFmpeg verifies the certificates, mbedTLS takes the system's ones in a file.
    static const QByteArray certificates = getCertificates();

    if (certificates.isEmpty() == false)
    {
        av_dict_set(&options, "ca_file", certificates.constData(), 0);
    }
#endif

    QByteArray url;

    if (input->url.startsWith("file:", Qt::CaseInsensitive))
    {
         url = QUrl(input->url).toLocalFile().toUtf8();
    }
    else url = input->url.toUtf8();

    // NOTE: We open the source ourselves, to keep in memory one that can't seek. A source that
    //       only its demuxer opens, as RTSP, has no protocol here and goes the usual way.
    AVIOInterruptCB callback = { interrupt, this };

    int result = avio_open2(&(input->io), url.constData(), AVIO_FLAG_READ, &callback, &options);

    if (result >= 0)
    {
        if ((input->io->seekable & AVIO_SEEKABLE_NORMAL) == 0)
        {
            input->cache = new WFfmpegCache(input->io);

            input->io = input->cache->io;
        }

        input->format->pb = input->io;

        input->format->flags |= AVFMT_FLAG_CUSTOM_IO;
    }
    // NOTE: Any other failure is the source's own: opening it again would go without the options
    //       avio_open2 took, as the proxy.
    else if (result != AVERROR_PROTOCOL_NOT_FOUND)
    {
        av_dict_free(&options);

        return false;
    }

    result = avformat_open_input(&(input->format), url.constData(), NULL, &options);

    av_dict_free(&options);

    if (result < 0) return false;

    return (avformat_find_stream_info(input->format, NULL) >= 0);
}

bool WFfmpegPlayerPrivate::openVideo(bool hardware)
{
    avcodec_free_context(&codecVideo);

    formatHardware = AV_PIX_FMT_NONE;

    decoded = false;

    accelerated = false;

    AVCodecID id = parameters->codec_id;

    const AVCodec * decoder = avcodec_find_decoder(id);

#ifdef Q_OS_ANDROID
    // NOTE: MediaCodec has decoders of its own, as h264_mediacodec, its frames come to memory.
    if (hardware)
    {
        QByteArray name = QByteArray(avcodec_get_name(id)) + "_mediacodec";

        const AVCodec * decoderHardware = avcodec_find_decoder_by_name(name.constData());

        if (decoderHardware)
        {
            decoder = decoderHardware;

            accelerated = true;
        }
    }
#else
    if (hardware && FFMPEGPLAYER_DEVICE != AV_HWDEVICE_TYPE_NONE)
    {
        // NOTE: FFmpeg's own AV1 decoder decodes on the GPU, dav1d is software only.
        const AVCodec * decoderHardware;

        if (id == AV_CODEC_ID_AV1)
        {
             decoderHardware = avcodec_find_decoder_by_name("av1");
        }
        else decoderHardware = decoder;

        // NOTE: The renderer's device first, then the platform's.
        AVHWDeviceType types[2] = { getDevice(), FFMPEGPLAYER_DEVICE };

        for (int i = 0; i < 2; i++)
        {
            AVHWDeviceType type = types[i];

            AVPixelFormat format = getHardware(decoderHardware, type);

            if (format == AV_PIX_FMT_NONE) continue;

            // NOTE: The device stays from a source to the next one, creating and releasing it is
            //       slow.
            if (device && ((AVHWDeviceContext *) device->data)->type != type)
            {
                av_buffer_unref(&device);
            }

            if (device == NULL && av_hwdevice_ctx_create(&device, type, NULL, NULL, 0) < 0)
            {
                continue;
            }

            formatHardware = format;

            decoder = decoderHardware;

            accelerated = true;

            break;
        }
    }
#endif

    if (decoder == NULL) return false;

    codecVideo = avcodec_alloc_context3(decoder);

    avcodec_parameters_to_context(codecVideo, parameters);

    codecVideo->pkt_timebase = av_make_q(baseVideo.num, baseVideo.den);

    codecVideo->opaque = this;

    if (formatHardware != AV_PIX_FMT_NONE)
    {
        codecVideo->hw_device_ctx = av_buffer_ref(device);

        codecVideo->get_format = getFormat;
    }
    // NOTE: As many threads as cores.
    else codecVideo->thread_count = 0;

    if (avcodec_open2(codecVideo, decoder, NULL) >= 0) return true;

    avcodec_free_context(&codecVideo);

    return false;
}

bool WFfmpegPlayerPrivate::openAudio(AVStream * stream)
{
    const AVCodec * decoder = avcodec_find_decoder(stream->codecpar->codec_id);

    if (decoder == NULL) return false;

    codecAudio = avcodec_alloc_context3(decoder);

    avcodec_parameters_to_context(codecAudio, stream->codecpar);

    codecAudio->pkt_timebase = stream->time_base;

    if (avcodec_open2(codecAudio, decoder, NULL) >= 0) return true;

    avcodec_free_context(&codecAudio);

    return false;
}

//-------------------------------------------------------------------------------------------------

void WFfmpegPlayerPrivate::closeInput(WFfmpegInput * input)
{
    avformat_close_input(&(input->format));

    if (input->cache)
    {
        delete input->cache;

        input->cache = NULL;

        input->io = NULL;
    }
    else avio_closep(&(input->io));
}

//-------------------------------------------------------------------------------------------------

// NOTE: Every input is open: the tracks and the length of the first one go to the backend.
void WFfmpegPlayerPrivate::applyReady()
{
    ready = true;

    condition.wakeAll();

    // NOTE: A track is a stream of its input, its id is the stream index.
    QList<WBackendTrack> tracks;

    int trackVideo = -1;
    int trackAudio = -1;

    int countVideo = 0;
    int countAudio = 0;

    foreach (WFfmpegInput * input, inputs)
    {
        AVFormatContext * format = input->format;

        if (format == NULL) continue;

        for (unsigned int i = 0; i < format->nb_streams; i++)
        {
            AVStream * stream = format->streams[i];

            AVMediaType type = stream->codecpar->codec_type;

            if (input->video
                &&
                type == AVMEDIA_TYPE_VIDEO
                &&
                (stream->disposition & AV_DISPOSITION_ATTACHED_PIC) == 0)
            {
                countVideo++;

                tracks.append(WBackendTrack(i, WAbstractBackend::TrackVideo,
                                            getName(stream, countVideo)));
            }
            else if (input->audio && type == AVMEDIA_TYPE_AUDIO)
            {
                countAudio++;

                tracks.append(WBackendTrack(i, WAbstractBackend::TrackAudio,
                                            getName(stream, countAudio)));
            }
        }

        if (input->indexVideo != -1) trackVideo = input->indexVideo;
        if (input->indexAudio != -1) trackAudio = input->indexAudio;
    }

    QCoreApplication::postEvent(q_func(), new WFfmpegTracksEvent(session, tracks, trackVideo,
                                                                 trackAudio, frameRate));

    qint64 duration = inputs.first()->format->duration;

    if (duration != AV_NOPTS_VALUE)
    {
        post(WFfmpegPlayer::EventLengthChanged, (int) (duration / 1000));
    }

    post(WFfmpegPlayer::EventPlaying);
}

//-------------------------------------------------------------------------------------------------

// NOTE: Returns the next frame of the last seek, NULL when aborted or at the end of the stream.
AVFrame * WFfmpegPlayerPrivate::decodeFrame()
{
    bool draining = false;

    while (true)
    {
        mutex.lock();

        if (serialVideo != serial)
        {
            serialVideo = serial;

            mutex.unlock();

            avcodec_flush_buffers(codecVideo);

            draining = false;
        }
        else mutex.unlock();

        int result = avcodec_receive_frame(codecVideo, frameDecoded);

        if (result == 0)
        {
            decoded = true;

            return frameDecoded;
        }

        if (result == AVERROR_EOF) return NULL;

        if (result != AVERROR(EAGAIN))
        {
            if (applySoftware() == false) return NULL;

            continue;
        }

        if (draining) return NULL;

        QMutexLocker locker(&mutex);

        while (abort.loadRelaxed() == 0 && queueVideo.packets.isEmpty() && queueVideo.eof == false
               &&
               serialVideo == serial)
        {
            if (playing && starving == NULL)
            {
                starving = &queueVideo;

                post(WFfmpegPlayer::EventBuffering, 0);
            }

            condition.wait(&mutex);
        }

        if (abort.loadRelaxed()) return NULL;

        if (serialVideo != serial) continue;

        if (queueVideo.packets.isEmpty())
        {
            locker.unlock();

            draining = true;

            avcodec_send_packet(codecVideo, NULL);

            continue;
        }

        WFfmpegPacket item = queueVideo.packets.dequeue();

        queueVideo.duration -= item.duration;

        condition.wakeAll();

        if (item.serial != serial)
        {
            av_packet_free(&item.packet);

            continue;
        }

        if (seekVideo != -1
            &&
            getTime(baseVideo, item.packet->pts) < seekVideo - FFMPEGPLAYER_SKIP)
        {
             codecVideo->skip_frame = AVDISCARD_NONREF;
        }
        else codecVideo->skip_frame = AVDISCARD_DEFAULT;

        locker.unlock();

        result = avcodec_send_packet(codecVideo, item.packet);

        av_packet_free(&item.packet);

        if (result < 0 && result != AVERROR(EAGAIN) && applySoftware() == false) return NULL;
    }
}

// NOTE: When the GPU can't decode a stream, its first frame fails, and we decode it in software
//       from the last seek. Later errors are corrupted data, the decoder goes past them.
bool WFfmpegPlayerPrivate::applySoftware()
{
    if (accelerated == false || decoded) return true;

    // NOTE: The device may be the one failing, after a GPU reset, the next source gets a new one.
    av_buffer_unref(&device);

    if (openVideo(false) == false) return false;

    QMutexLocker locker(&mutex);

    applySeek((seekVideo == -1) ? timeVideo : seekVideo);

    return true;
}

// NOTE: The frame goes to the backend's buffer as I420, also from a hardware or a 10 bits frame.
//       The buffer stays ours until the frame goes out, a frame dropped leaves it to the next.
bool WFfmpegPlayerPrivate::applyFrame(AVFrame * frame)
{
    int frameWidth  = frame->width;
    int frameHeight = frame->height;

    if (frameWidth > FFMPEGPLAYER_MAX_WIDTH || frameHeight > FFMPEGPLAYER_MAX_HEIGHT)
    {
        QSize size(frameWidth, frameHeight);

        size.scale(FFMPEGPLAYER_MAX_WIDTH, FFMPEGPLAYER_MAX_HEIGHT, Qt::KeepAspectRatio);

        frameWidth  = size.width ();
        frameHeight = size.height();
    }

    if (width != frameWidth || height != frameHeight)
    {
        int lines[3];

        if (setup(backend, &frameWidth, &frameHeight, pitches, lines) == false) return false;

        width  = frameWidth;
        height = frameHeight;

        locked = false;
    }

    SwsContext * context = sws_getCachedContext(scale, frame->width, frame->height,
                                                static_cast<AVPixelFormat> (frame->format),
                                                width, height, AV_PIX_FMT_YUV420P, SWS_BICUBIC,
                                                NULL, NULL, NULL);

    if (context == NULL) return false;

    // NOTE: The video shader expects the video range, a full range frame is brought to it.
    if (context != scale && frame->color_range == AVCOL_RANGE_JPEG)
    {
        const int * coefficients = sws_getCoefficients(SWS_CS_ITU709);

        sws_setColorspaceDetails(context, coefficients, 1, coefficients, 0, 0, 1 << 16, 1 << 16);
    }

    scale = context;

    if (locked == false)
    {
        lock(backend, planes);

        locked = true;
    }

    sws_scale(scale, frame->data, frame->linesize, 0, frame->height, planes, pitches);

    return true;
}

//-------------------------------------------------------------------------------------------------

// NOTE: Only the last seek counts: the threads drop what came before it.
void WFfmpegPlayerPrivate::applySeek(int msec)
{
    serial++;

    seekVideo = msec;
    seekAudio = msec;

    foreach (WFfmpegInput * input, inputs)
    {
        input->seek = msec;
    }

    step = false;

    clockValid = false;

    timePosted = -1;

    audioEnded = false;

    applyAudio();

    condition.wakeAll();
}

// NOTE: The end of the stream that leads: the video, or the audio alone.
void WFfmpegPlayerPrivate::applyEnd()
{
    if (playing == false) return;

    if (loop)
    {
        applySeek(0);

        return;
    }

    playing = false;

    post(WFfmpegPlayer::EventEndReached);
}

void WFfmpegPlayerPrivate::applyAudio()
{
    audio = (resample && audioEnded == false);
}

//-------------------------------------------------------------------------------------------------

void WFfmpegPlayerPrivate::clearFrames()
{
    foreach (WFfmpegFrame item, frames)
    {
        av_frame_free(&(item.frame));
    }

    frames.clear();
}

void WFfmpegPlayerPrivate::clearQueue(WFfmpegQueue & queue)
{
    foreach (WFfmpegPacket item, queue.packets)
    {
        av_packet_free(&item.packet);
    }

    queue.packets.clear();

    queue.duration = 0;
}

//-------------------------------------------------------------------------------------------------

void WFfmpegPlayerPrivate::post(WFfmpegPlayer::EventType type, const QVariant & value)
{
    QCoreApplication::postEvent(q_func(), new WFfmpegPlayerEvent(type, session, value));
}

//-------------------------------------------------------------------------------------------------

// NOTE: The time of the next frame, rounded as the time of this one.
int WFfmpegPlayerPrivate::getTimeNext(AVFrame * frame, qint64 timestamp) const
{
    if (frame->duration > 0) return getTime(baseVideo, timestamp + frame->duration);

    if (frameRate > 0.0)
    {
         return getTime(baseVideo, timestamp) + qRound(1000 / frameRate);
    }
    else return getTime(baseVideo, timestamp);
}

// NOTE: A hardware frame of more than 8 bits goes to 8 on the GPU before its copy to memory: a
//       4K 10 bits frame takes longer to copy than it lasts at 60 fps, and the shader shows 8
//       bits anyway. Without the filter, the frame is copied as it is.
AVFrame * WFfmpegPlayerPrivate::applyDepth(AVFrame * frame)
{
    if (FFMPEGPLAYER_DEPTH == NULL) return frame;

    AVHWFramesContext * context
        = reinterpret_cast<AVHWFramesContext *> (frame->hw_frames_ctx->data);

    // NOTE: The filter is the platform device's, a Vulkan frame is copied as it is.
    if (context->device_ctx->type != FFMPEGPLAYER_DEVICE
        ||
        av_pix_fmt_desc_get(context->sw_format)->comp[0].depth <= 8) return frame;

    // NOTE: A graph per frames context, made once: a failed one is not tried again.
    if (graphFrames != context)
    {
        graphFrames = context;

        createDepth(frame);
    }

    if (graph == NULL
        ||
        av_buffersrc_add_frame_flags(filterIn, frame, AV_BUFFERSRC_FLAG_KEEP_REF) < 0
        ||
        av_buffersink_get_frame(filterOut, frameDepth) < 0) return frame;

    return frameDepth;
}

void WFfmpegPlayerPrivate::createDepth(AVFrame * frame)
{
    avfilter_graph_free(&graph);

    graph = avfilter_graph_alloc();

    filterIn = avfilter_graph_alloc_filter(graph, avfilter_get_by_name("buffer"), "in");

    AVBufferSrcParameters * source = av_buffersrc_parameters_alloc();

    source->format = frame->format;

    source->width  = frame->width;
    source->height = frame->height;

    source->time_base = av_make_q(baseVideo.num, baseVideo.den);

    source->hw_frames_ctx = frame->hw_frames_ctx;

    int result = av_buffersrc_parameters_set(filterIn, source);

    av_free(source);

    AVFilterInOut * inputs  = avfilter_inout_alloc();
    AVFilterInOut * outputs = avfilter_inout_alloc();

    if (result >= 0
        &&
        avfilter_init_dict(filterIn, NULL) >= 0
        &&
        avfilter_graph_create_filter(&filterOut, avfilter_get_by_name("buffersink"), "out",
                                     NULL, NULL, graph) >= 0)
    {
        outputs->name       = av_strdup("in");
        outputs->filter_ctx = filterIn;

        inputs->name       = av_strdup("out");
        inputs->filter_ctx = filterOut;

        if (avfilter_graph_parse_ptr(graph, FFMPEGPLAYER_DEPTH, &inputs, &outputs, NULL) >= 0
            &&
            avfilter_graph_config(graph, NULL) >= 0)
        {
            avfilter_inout_free(&inputs);
            avfilter_inout_free(&outputs);

            return;
        }
    }

    avfilter_inout_free(&inputs);
    avfilter_inout_free(&outputs);

    avfilter_graph_free(&graph);

    qWarning("WFfmpegPlayerPrivate::createDepth: Cannot bring the frames to 8 bits.");
}

//-------------------------------------------------------------------------------------------------

// NOTE: The milliseconds until a frame is due. A frame goes in a slot, halfway between two swaps
//       of the window: a frame due close to a swap would land before it or after it with the
//       jitter, giving a swap two frames and the next none. A frame follows the slot of the
//       previous one while it's within 3/4 of a period of its time, so when the frame rate and
//       the refresh drift apart, it moves to the nearest slot once.
qint64 WFfmpegPlayerPrivate::getWait(int time, qint64 * slot) const
{
    qint64 wait = qint64((time - clockTime) / speed * 1000000) - clock.nsecsElapsed();

    if (swapPeriod == 0) return wait / 1000000;

    qint64 now = QDeadlineTimer::current(Qt::PreciseTimer).deadlineNSecs();

    // NOTE: A window that does not swap does not show, the frames go on their time.
    if (now - swapTime >= FFMPEGPLAYER_SWAP) return wait / 1000000;

    qint64 due = now + wait;

    qint64 base = swapTime + swapPeriod / 2;

    qint64 at;

    if (slotTime)
    {
        at = slotTime + qint64((time - slotFrame) / speed * 1000000);
    }
    else at = due;

    at = base + qRound64(double(at - base) / swapPeriod) * swapPeriod;

    if (qAbs(due - at) > swapPeriod * 3 / 4)
    {
        at = base + qRound64(double(due - base) / swapPeriod) * swapPeriod;
    }

    *slot = at;

    return (at - now) / 1000000;
}

//-------------------------------------------------------------------------------------------------
// Private static functions
//-------------------------------------------------------------------------------------------------

/* static */ int WFfmpegPlayerPrivate::interrupt(void * data)
{
    return static_cast<WFfmpegPlayerPrivate *> (data)->abort.loadRelaxed();
}

//-------------------------------------------------------------------------------------------------

/* static */ WFfmpegBase WFfmpegPlayerPrivate::getBase(AVStream * stream)
{
    WFfmpegBase base;

    base.num = stream->time_base.num;
    base.den = stream->time_base.den;

    if (stream->start_time != AV_NOPTS_VALUE)
    {
         base.start = stream->start_time;
    }
    else base.start = 0;

    return base;
}

/* static */ int WFfmpegPlayerPrivate::getTime(const WFfmpegBase & base, qint64 timestamp)
{
    if (timestamp == AV_NOPTS_VALUE) return -1;

    return av_rescale(timestamp - base.start, (qint64) base.num * 1000, base.den);
}

/* static */ qint64 WFfmpegPlayerPrivate::getTimestamp(const WFfmpegBase & base, int msec)
{
    return av_rescale(msec, base.den, (qint64) base.num * 1000) + base.start;
}

// NOTE: The stream of the track picked when it is one of that type, otherwise the best one. A
//       cover picture is not a video track.
/* static */ int WFfmpegPlayerPrivate::getStream(AVFormatContext * format, int type, int id,
                                                 int related, int quality)
{
    if (id >= 0 && id < (int) format->nb_streams)
    {
        AVStream * stream = format->streams[id];

        if (stream->codecpar->codec_type == type
            &&
            (stream->disposition & AV_DISPOSITION_ATTACHED_PIC) == 0)
        {
            return id;
        }
    }

    if (type != AVMEDIA_TYPE_VIDEO || format->nb_programs <= 1)
    {
        return av_find_best_stream(format, (AVMediaType) type, -1, related, NULL, 0);
    }

    // NOTE: The video of a variant list is the one closest to the quality, as VLC's
    //       preferred-resolution.
    int index = -1;
    int best  =  0;

    for (unsigned int i = 0; i < format->nb_streams; i++)
    {
        AVCodecParameters * parameters = format->streams[i]->codecpar;

        if (parameters->codec_type != AVMEDIA_TYPE_VIDEO) continue;

        int height = parameters->height;

        int score;

        // NOTE: Under the quality the highest wins, above it the lowest.
        if (quality == 0 || height <= quality)
        {
            score = height;
        }
        else score = -height;

        if (index != -1 && score <= best) continue;

        index = i;
        best  = score;
    }

    return index;
}

// NOTE: The name of a track is its title, otherwise its language, as VLC's.
/* static */ QString WFfmpegPlayerPrivate::getName(AVStream * stream, int index)
{
    AVDictionaryEntry * entry = av_dict_get(stream->metadata, "title", NULL, 0);

    if (entry) return QString::fromUtf8(entry->value);

    entry = av_dict_get(stream->metadata, "language", NULL, 0);

    if (entry) return QString::fromUtf8(entry->value);

    return "Track " + QString::number(index);
}

#ifdef Q_OS_LINUX

// NOTE: The certificates of the system in one file: the bundle of the distribution on Linux, the
//       certificates folder on Android, gathered once in the cache.
/* static */ QByteArray WFfmpegPlayerPrivate::getCertificates()
{
#ifdef Q_OS_ANDROID
    QString path = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);

    QDir().mkpath(path);

    path.append("/certificates.pem");

    QFile file(path);

    if (file.open(QIODevice::WriteOnly) == false) return QByteArray();

    QDir folder("/system/etc/security/cacerts");

    foreach (const QString & name, folder.entryList(QDir::Files))
    {
        QFile certificate(folder.filePath(name));

        if (certificate.open(QIODevice::ReadOnly)) file.write(certificate.readAll());
    }

    return path.toUtf8();
#else
    static const char * paths[] =
    {
        "/etc/ssl/certs/ca-certificates.crt", // Debian, Ubuntu, Arch
        "/etc/pki/tls/certs/ca-bundle.crt",   // Fedora, RHEL
        "/etc/ssl/ca-bundle.pem",             // openSUSE
        "/etc/ssl/cert.pem",                  // Alpine
        NULL
    };

    for (int i = 0; paths[i]; i++)
    {
        if (QFile::exists(paths[i])) return paths[i];
    }

    return QByteArray();
#endif
}

#endif

//=================================================================================================
// WFfmpegPlayer
//=================================================================================================

WFfmpegPlayer::WFfmpegPlayer(QObject * parent)
    : QObject(parent), WPrivatable(new WFfmpegPlayerPrivate(this))
{
    Q_D(WFfmpegPlayer); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

void WFfmpegPlayer::setBackend(WAbstractBackend * backend, WFfmpegSetup  setup,
                                                           WFfmpegLock   lock,
                                                           WFfmpegUnlock unlock)
{
    Q_D(WFfmpegPlayer);

    d->backend = backend;

    d->setup  = setup;
    d->lock   = lock;
    d->unlock = unlock;
}

// NOTE: The options are VLC's, we don't use them.
void WFfmpegPlayer::setSource(const QString & url, const QString & audio, const QStringList &,
                              bool loop)
{
    Q_D(WFfmpegPlayer);

    d->clear();

    d->session++;

    // NOTE: The tracks picked are the ones of a source, they stay when it plays again.
    if (d->url != url || d->urlAudio != audio)
    {
        QMutexLocker locker(&d->mutex);

        d->hasVariants = false;

        d->idVideo = -1;
        d->idAudio = -1;
    }

    d->url      = url;
    d->urlAudio = audio;

    d->loop = loop;
}

void WFfmpegPlayer::play(int at)
{
    Q_D(WFfmpegPlayer);

    if (d->inputs.isEmpty())
    {
        d->start(at);

        return;
    }

    QMutexLocker locker(&d->mutex);

    if (at > 0) d->applySeek(at);

    d->playing = true;

    d->clockValid = false;

    d->condition.wakeAll();
}

void WFfmpegPlayer::pause()
{
    Q_D(WFfmpegPlayer);

    QMutexLocker locker(&d->mutex);

    d->playing = false;

    // NOTE: A paused player says the time of the frame it shows.
    if (d->timePosted != -1 && d->timePosted != d->timeVideo)
    {
        d->timePosted = d->timeVideo;

        d->post(EventTimeChanged, d->timeVideo);
    }

    d->condition.wakeAll();
}

void WFfmpegPlayer::stop()
{
    Q_D(WFfmpegPlayer);

    d->clear();

    d->session++;
}

void WFfmpegPlayer::seek(int msec)
{
    Q_D(WFfmpegPlayer);

    if (d->inputs.isEmpty()) return;

    QMutexLocker locker(&d->mutex);

    d->applySeek(msec);
}

void WFfmpegPlayer::nextFrame()
{
    Q_D(WFfmpegPlayer);

    QMutexLocker locker(&d->mutex);

    d->step = true;

    d->condition.wakeAll();
}

void WFfmpegPlayer::setSpeed(qreal speed)
{
    Q_D(WFfmpegPlayer);

    QMutexLocker locker(&d->mutex);

    d->speed = speed;

    d->clockValid = false;

    d->condition.wakeAll();
}

void WFfmpegPlayer::setVolume(int percent)
{
    Q_D(WFfmpegPlayer);

    QMutexLocker locker(&d->mutex);

    d->volume = percent;
}

// NOTE: It applies from the next source on, as VLC's.
void WFfmpegPlayer::setOutput(WAbstractBackend::Output output)
{
    Q_D(WFfmpegPlayer);

    d->output = output;
}

// NOTE: It applies from the next source on, as VLC's. The host is 'address:port'.
void WFfmpegPlayer::setProxy(const QString & host, const QString & password)
{
    Q_D(WFfmpegPlayer);

    QMutexLocker locker(&d->mutex);

    if (password.isEmpty())
    {
         d->proxy = "http://" + host;
    }
    else d->proxy = "http://:" + password + '@' + host;
}

void WFfmpegPlayer::clearProxy()
{
    Q_D(WFfmpegPlayer);

    QMutexLocker locker(&d->mutex);

    d->proxy = QString();
}

// NOTE: It applies from the next start on. The tracks of a variant list follow the quality, so
//       they are cleared and it returns true.
bool WFfmpegPlayer::setQuality(int height)
{
    Q_D(WFfmpegPlayer);

    QMutexLocker locker(&d->mutex);

    d->quality = height;

    if (d->hasVariants == false) return false;

    d->idVideo = -1;
    d->idAudio = -1;

    return true;
}

// NOTE: The ids are stream indexes, -1 for the best stream. It applies from the next start on.
bool WFfmpegPlayer::setTracks(int idVideo, int idAudio)
{
    Q_D(WFfmpegPlayer);

    QMutexLocker locker(&d->mutex);

    if (d->idVideo == idVideo && d->idAudio == idAudio) return false;

    d->idVideo = idVideo;
    d->idAudio = idAudio;

    return true;
}

void WFfmpegPlayer::setSwap(qint64 time, qint64 period)
{
    Q_D(WFfmpegPlayer);

    QMutexLocker locker(&d->mutex);

    d->swapTime   = time;
    d->swapPeriod = period;
}

void WFfmpegPlayer::deletePlayer()
{
    Q_D(WFfmpegPlayer);

    d->clear();

    d->session++;

    QCoreApplication::postEvent(d->backend, new WFfmpegPlayerEvent(EventDelete, d->session));
}

//-------------------------------------------------------------------------------------------------
// Events
//-------------------------------------------------------------------------------------------------

// NOTE: The events of the threads go to the backend, unless they come from a previous source.
/* virtual */ bool WFfmpegPlayer::event(QEvent * event)
{
    QEvent::Type type = event->type();

    if (type < static_cast<QEvent::Type> (EventPlaying)
        ||
        type > static_cast<QEvent::Type> (EventDelete))
    {
        return QObject::event(event);
    }

    Q_D(WFfmpegPlayer);

    if (static_cast<WFfmpegPlayerEvent *> (event)->session == d->session && d->backend)
    {
        QCoreApplication::sendEvent(d->backend, event);
    }

    return true;
}

#endif // SK_NO_FFMPEGPLAYER
