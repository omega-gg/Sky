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

#include "WFfmpegDownload.h"

#ifndef SK_NO_FFMPEGDOWNLOAD

// Qt includes
#include <QFile>
#include <QUrl>

// Sk includes
#include <WControllerNetwork>
#include <WControllerPlaylist>
#include <WFfmpegPlayer>

// FFmpeg includes
extern "C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}

// Private includes
#include <private/WFfmpegPlayer_p>

//-------------------------------------------------------------------------------------------------
// Static variables

static const int FFMPEGDOWNLOAD_BUFFER = 262144;

// NOTE: The part of the progress we leave for the mux.
static const int FFMPEGDOWNLOAD_MUX = 99;

// NOTE: The received bytes go out at least with every megabyte.
static const int FFMPEGDOWNLOAD_STEP = 1048576;

// NOTE: The header can't be written.
static const int FFMPEGDOWNLOAD_REFUSED = 1;

//=================================================================================================
// WFfmpegDownloadPrivate
//=================================================================================================

WFfmpegDownloadPrivate::WFfmpegDownloadPrivate(WFfmpegDownload * p) : WPrivate(p) {}

void WFfmpegDownloadPrivate::init(const QStringList & urls,
                                  const QString     & path, const QStringList & options)
{
    this->urls = urls;
    this->path = path;

    header = WFfmpegPlayerPrivate::getHeader(options);

    progress = -1;

    received =  0;
    done     =  0;
    total    = -1;
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

AVIOContext * WFfmpegDownloadPrivate::openIo(const QString & url)
{
    AVDictionary * options = WFfmpegPlayerPrivate::getOptions(proxy, header);

    AVIOInterruptCB callback = { interrupt, this };

    AVIOContext * io = NULL;

    avio_open2(&io, getUrl(url).constData(), AVIO_FLAG_READ, &callback, &options);

    av_dict_free(&options);

    return io;
}

qint64 WFfmpegDownloadPrivate::getTotal()
{
    qint64 sum = 0;

    foreach (const QString & url, urls)
    {
        AVIOContext * io = openIo(url);

        if (io == NULL) return -1;

        qint64 size = avio_size(io);

        avio_closep(&io);

        if (size <= 0) return -1;

        sum += size;
    }

    return sum;
}

bool WFfmpegDownloadPrivate::copy(const QString & url, const QString & part)
{
    QFile file(part);

    if (file.open(QIODevice::WriteOnly) == false) return false;

    AVIOContext * io = openIo(url);

    if (io == NULL) return false;

    qint64 size = avio_size(io);

    if (urls.count() == 1 && size > 0) total = size;

    qint64 bytes = 0;

    QByteArray buffer(FFMPEGDOWNLOAD_BUFFER, Qt::Uninitialized);

    bool ok = true;

    unsigned char * data = reinterpret_cast<unsigned char *> (buffer.data());

    while (abort.loadRelaxed() == 0)
    {
        int result = avio_read(io, data, buffer.size());

        if (result < 0)
        {
            ok = (result == AVERROR_EOF);

            break;
        }

        if (file.write(buffer.constData(), result) != result)
        {
            ok = false;

            break;
        }

        bytes += result;

        qint64 current = done + bytes;

        if (total > 0)
        {
             setProgress(current * FFMPEGDOWNLOAD_MUX / total, current);
        }
        else setProgress(0, current);
    }

    done += bytes;

    avio_closep(&io);

    if (abort.loadRelaxed()) return false;

    // NOTE: A connection cut before the end must not pass for a complete file.
    return (ok && (size <= 0 || bytes == size));
}

bool WFfmpegDownloadPrivate::mux(const QStringList & sources, bool remote)
{
    QList<WFfmpegDownloadInput> inputs;

    foreach (const QString & source, sources)
    {
        if (openInput(&inputs, source, remote)) continue;

        closeInputs(&inputs);

        return false;
    }

    WFfmpegDownloadInput & first = inputs.first();
    WFfmpegDownloadInput & last  = inputs.last();

    if (inputs.count() > 1)
    {
        // NOTE: The video comes from the video part, and the audio from the audio part.
        if (first.video != -1) last.video = -1;

        if (last.audio != -1) first.audio = -1;
    }

    bool hasVideo = (first.video != -1 || last.video != -1);

    if (hasVideo == false && first.audio == -1 && last.audio == -1)
    {
        closeInputs(&inputs);

        return false;
    }

    QString part = path + ".part";

    QString extension = (hasVideo) ? "mp4" : "m4a";

    int result = write(&inputs, "mp4", part, remote);

    if (result == FFMPEGDOWNLOAD_REFUSED)
    {
        extension = (hasVideo) ? "mkv" : "mka";

        result = write(&inputs, "matroska", part, remote);
    }

    closeInputs(&inputs);

    if (result == 0 && apply(part, extension)) return true;

    QFile::remove(part);

    return false;
}

bool WFfmpegDownloadPrivate::openInput(QList<WFfmpegDownloadInput> * inputs, const QString & url,
                                                                             bool            remote)
{
    AVFormatContext * format = avformat_alloc_context();

    format->interrupt_callback.callback = interrupt;
    format->interrupt_callback.opaque   = this;

    AVDictionary * options = NULL;

    if (remote) options = WFfmpegPlayerPrivate::getOptions(proxy, header);

    // NOTE: This frees the context when it fails.
    int result = avformat_open_input(&format, getUrl(url).constData(), NULL, &options);

    av_dict_free(&options);

    if (result < 0) return false;

    WFfmpegDownloadInput input;

    input.format = format;

    input.packet = av_packet_alloc();

    input.video = -1;
    input.audio = -1;

    input.outputVideo = NULL;
    input.outputAudio = NULL;

    input.ready = false;

    inputs->append(input);

    if (avformat_find_stream_info(format, NULL) < 0) return false;

    WFfmpegDownloadInput & item = inputs->last();

    item.video = qMax(-1, av_find_best_stream(format, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0));
    item.audio = qMax(-1, av_find_best_stream(format, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0));

    // NOTE: A cover is a video stream to FFmpeg.
    if (item.video != -1
        &&
        (format->streams[item.video]->disposition & AV_DISPOSITION_ATTACHED_PIC))
    {
        item.video = -1;
    }

    return true;
}

// NOTE: This returns 0 when the file is written, FFMPEGDOWNLOAD_REFUSED when the format can't take
//       the streams and it's worth trying another one, and an error otherwise.
int WFfmpegDownloadPrivate::write(QList<WFfmpegDownloadInput> * inputs, const char    * format,
                                                                        const QString & part,
                                                                        bool            remote)
{
    AVFormatContext * output = NULL;

    QByteArray name = part.toUtf8();

    if (avformat_alloc_output_context2(&output, NULL, format, name.constData()) < 0)
    {
        return AVERROR_UNKNOWN;
    }

    output->interrupt_callback.callback = interrupt;
    output->interrupt_callback.opaque   = this;

    for (int i = 0; i < inputs->count(); i++)
    {
        WFfmpegDownloadInput & input = (*inputs)[i];

        for (int j = 0; j < 2; j++)
        {
            int index = (j) ? input.audio : input.video;

            if (index < 0) continue;

            const AVCodecParameters * parameters = input.format->streams[index]->codecpar;

            AVStream * stream = avformat_new_stream(output, NULL);

            if (stream == NULL || avcodec_parameters_copy(stream->codecpar, parameters) < 0)
            {
                avformat_free_context(output);

                return AVERROR_UNKNOWN;
            }

            // NOTE: A tag from the source may mean nothing in the new container.
            stream->codecpar->codec_tag = 0;

            if (j) input.outputAudio = stream;
            else   input.outputVideo = stream;
        }
    }

    AVIOInterruptCB callback = { interrupt, this };

    int result = avio_open2(&(output->pb), name.constData(), AVIO_FLAG_WRITE, &callback, NULL);

    if (result < 0)
    {
        avformat_free_context(output);

        return result;
    }

    result = avformat_write_header(output, NULL);

    if (result < 0)
    {
        avio_closep(&(output->pb));

        avformat_free_context(output);

        return FFMPEGDOWNLOAD_REFUSED;
    }

    int count = inputs->count();

    for (int i = 0; i < count && result >= 0; i++)
    {
        result = readInput(&((*inputs)[i]));

        if (result == AVERROR_EOF) result = 0;
    }

    qint64 duration = (*inputs)[0].format->duration;

    while (result >= 0)
    {
        WFfmpegDownloadInput * input = NULL;

        // NOTE: We take the earliest packet to keep the interleaving buffer small.
        for (int i = 0; i < count; i++)
        {
            WFfmpegDownloadInput * item = &((*inputs)[i]);

            if (item->ready == false) continue;

            if (input == NULL || getPacketTime(*item) < getPacketTime(*input))
            {
                input = item;
            }
        }

        if (input == NULL) break;

        AVPacket * packet = input->packet;

        const AVStream * source = input->format->streams[packet->stream_index];

        AVStream * stream = (packet->stream_index == input->video) ? input->outputVideo
                                                                   : input->outputAudio;

        if (remote)
        {
            // NOTE: A playlist has a duration but no size, the file is what we received so far.
            int percent = 0;

            if (duration > 0) percent = getPacketTime(*input) * FFMPEGDOWNLOAD_MUX / duration;

            setProgress(percent, avio_tell(output->pb));
        }

        av_packet_rescale_ts(packet, source->time_base, stream->time_base);

        packet->stream_index = stream->index;

        packet->pos = -1;

        // NOTE: This takes the packet over.
        result = av_interleaved_write_frame(output, packet);

        if (result < 0) break;

        result = readInput(input);

        if (result == AVERROR_EOF) result = 0;

        if (abort.loadRelaxed()) result = AVERROR_EXIT;
    }

    if (result >= 0)
    {
        result = av_write_trailer(output);
    }

    avio_closep(&(output->pb));

    avformat_free_context(output);

    return qMin(result, 0);
}

bool WFfmpegDownloadPrivate::apply(const QString & part, const QString & extension)
{
    QString name = path + '.' + extension;

    int index = 2;

    // NOTE: The name may have been taken since we started.
    while (QFile::exists(name))
    {
        name = path + " (" + QString::number(index) + ")." + extension;

        index++;
    }

    if (QFile::rename(part, name) == false) return false;

    fileName = name;

    setProgress(100, received);

    return true;
}

void WFfmpegDownloadPrivate::setProgress(int percent, qint64 bytes)
{
    if (progress == percent && bytes - received < FFMPEGDOWNLOAD_STEP) return;

    Q_Q(WFfmpegDownload);

    progress = percent;
    received = bytes;

    emit q->progress(percent, bytes, total);
}

//-------------------------------------------------------------------------------------------------
// Private static functions
//-------------------------------------------------------------------------------------------------

/* static */ int WFfmpegDownloadPrivate::interrupt(void * data)
{
    return static_cast<WFfmpegDownloadPrivate *> (data)->abort.loadRelaxed();
}

/* static */ QByteArray WFfmpegDownloadPrivate::getUrl(const QString & url)
{
    if (url.startsWith("file:", Qt::CaseInsensitive))
    {
         return QUrl(url).toLocalFile().toUtf8();
    }
    else return url.toUtf8();
}

/* static */ void WFfmpegDownloadPrivate::closeInputs(QList<WFfmpegDownloadInput> * inputs)
{
    for (int i = 0; i < inputs->count(); i++)
    {
        WFfmpegDownloadInput & input = (*inputs)[i];

        av_packet_free(&(input.packet));

        avformat_close_input(&(input.format));
    }
}

// NOTE: We read up to a packet of the streams we keep.
/* static */ int WFfmpegDownloadPrivate::readInput(WFfmpegDownloadInput * input)
{
    AVPacket * packet = input->packet;

    while (true)
    {
        int result = av_read_frame(input->format, packet);

        if (result < 0)
        {
            input->ready = false;

            return result;
        }

        if (packet->stream_index == input->video || packet->stream_index == input->audio)
        {
            input->ready = true;

            return 0;
        }

        av_packet_unref(packet);
    }
}

/* static */ qint64 WFfmpegDownloadPrivate::getPacketTime(const WFfmpegDownloadInput & input)
{
    const AVPacket * packet = input.packet;

    qint64 time = (packet->dts == AV_NOPTS_VALUE) ? packet->pts : packet->dts;

    if (time == AV_NOPTS_VALUE) return 0;

    return av_rescale_q(time, input.format->streams[packet->stream_index]->time_base,
                        AV_TIME_BASE_Q);
}

//=================================================================================================
// WFfmpegDownload
//=================================================================================================

WFfmpegDownload::WFfmpegDownload(const QStringList & urls,
                                 const QString     & path,
                                 const QStringList & options, QObject * parent)
    : QThread(parent), WPrivatable(new WFfmpegDownloadPrivate(this))
{
    Q_D(WFfmpegDownload); d->init(urls, path, options);
}

/* virtual */ WFfmpegDownload::~WFfmpegDownload()
{
    abort();

    wait();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

void WFfmpegDownload::setProxy(const QString & host, const QString & password)
{
    Q_D(WFfmpegDownload);

    d->proxy = WFfmpegPlayerPrivate::getProxy(host, password).toUtf8();
}

void WFfmpegDownload::abort()
{
    Q_D(WFfmpegDownload);

    d->abort.storeRelaxed(1);
}

//-------------------------------------------------------------------------------------------------
// Protected QThread reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WFfmpegDownload::run()
{
    Q_D(WFfmpegDownload);

    QString url = d->urls.first();

    QString extension = WControllerNetwork::extractUrlExtension(url).toLower();

    int count = d->urls.count();

    // NOTE: A playlist is made of segments, we can only read it as a stream.
    if (count == 1 && WControllerPlaylist::extensionIsM3u(extension))
    {
        d->mux(d->urls, true);

        return;
    }

    // NOTE: We want the total from the start, not to see it grow with each part.
    if (count > 1) d->total = d->getTotal();

    QStringList parts;

    bool ok = true;

    for (int i = 0; i < count && ok; i++)
    {
        QString part = d->path + '.' + QString::number(i) + ".part";

        parts.append(part);

        ok = d->copy(d->urls.at(i), part);
    }

    if (ok)
    {
        // NOTE: A single file we can identify is the result, we don't touch it.
        if (count == 1 && WControllerPlaylist::extensionIsMedia(extension))
        {
            d->apply(parts.first(), extension);
        }
        else d->mux(parts, false);
    }

    foreach (const QString & part, parts)
    {
        QFile::remove(part);
    }
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

QString WFfmpegDownload::fileName() const
{
    Q_D(const WFfmpegDownload); return d->fileName;
}

#endif // SK_NO_FFMPEGDOWNLOAD
