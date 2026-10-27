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

#ifndef WFFMPEGPLAYER_P_H
#define WFFMPEGPLAYER_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

// Qt includes
#include <QMutex>
#include <QWaitCondition>
#include <QQueue>
#include <QElapsedTimer>

// Private includes
#include <private/Sk_p>

#ifndef SK_NO_FFMPEGPLAYER

// Forward declarations
class QThread;
struct AVFormatContext;
struct AVIOContext;
struct AVStream;
struct AVCodecParameters;
struct AVCodecContext;
struct AVBufferRef;
struct AVPacket;
struct AVFrame;
struct AVFilterGraph;
struct AVFilterContext;
struct SwsContext;
struct SwrContext;
class  WFfmpegCache;

//-------------------------------------------------------------------------------------------------
// WFfmpegPacket
//-------------------------------------------------------------------------------------------------

struct WFfmpegPacket
{
    AVPacket * packet;

    int serial;

    int duration;
};

//-------------------------------------------------------------------------------------------------
// WFfmpegQueue
//-------------------------------------------------------------------------------------------------

// NOTE: The packets of a stream, with their duration in milliseconds.
struct WFfmpegQueue
{
    QQueue<WFfmpegPacket> packets;

    int duration;

    bool eof;
};

//-------------------------------------------------------------------------------------------------
// WFfmpegFrame
//-------------------------------------------------------------------------------------------------

// NOTE: A decoded frame waiting to be shown, the first one of a seek goes at once.
struct WFfmpegFrame
{
    AVFrame * frame;

    int serial;

    int time;

    bool seek;
};

//-------------------------------------------------------------------------------------------------
// WFfmpegBase
//-------------------------------------------------------------------------------------------------

// NOTE: The time base of a stream, kept apart so the other threads never touch its input.
struct WFfmpegBase
{
    int num;
    int den;

    qint64 start;
};

//-------------------------------------------------------------------------------------------------
// WFfmpegInput
//-------------------------------------------------------------------------------------------------

// NOTE: A source read by its own thread: the video, the audio, or both from one file. A source
//       that can't seek is kept in memory as it's read (see WFfmpegCache).
struct WFfmpegInput
{
    QString url;

    bool video;
    bool audio;

    AVFormatContext * format;

    AVIOContext * io;

    WFfmpegCache * cache;

    int indexVideo;
    int indexAudio;

    int seek;

    QThread * thread;
};

//-------------------------------------------------------------------------------------------------
// WFfmpegPlayerPrivate
//-------------------------------------------------------------------------------------------------

class SK_MEDIA_EXPORT WFfmpegPlayerPrivate : public WPrivate
{
public:
    WFfmpegPlayerPrivate(WFfmpegPlayer * p);

    /* virtual */ ~WFfmpegPlayerPrivate();

    void init();

public: // Functions
    void start(int at);

    void clear();

    void runRead  (WFfmpegInput * input);
    void runDecode();
    void runVideo ();
    void runAudio ();

    bool open      (WFfmpegInput * input);
    bool openFormat(WFfmpegInput * input);

    bool openVideo(bool hardware);
    bool openAudio(AVStream * stream);

    void closeInput(WFfmpegInput * input);

    void applyReady();

    AVFrame * decodeFrame();

    bool applySoftware();

    bool applyFrame(AVFrame * frame);

    AVFrame * applyDepth(AVFrame * frame);

    void createDepth(AVFrame * frame);

    void applySeek(int msec);

    void applyEnd();

    void applyAudio();

    void clearQueue(WFfmpegQueue & queue);

    void clearFrames();

    void post(WFfmpegPlayer::EventType type, const QVariant & value = QVariant());

    int getTimeNext(AVFrame * frame, qint64 timestamp) const;

    qint64 getWait(int time, qint64 * slot) const;

public: // Static functions
    static int interrupt(void * data);

    static WFfmpegBase getBase(AVStream * stream);

    static int getTime(const WFfmpegBase & base, qint64 timestamp);

    static qint64 getTimestamp(const WFfmpegBase & base, int msec);

    static int getStream(AVFormatContext * format, int type, int id, int related, int quality);

    static QString getName(AVStream * stream, int index);

#ifdef Q_OS_LINUX
    static QByteArray getCertificates();
#endif

public: // Variables
    WAbstractBackend * backend;

    WFfmpegSetup  setup;
    WFfmpegLock   lock;
    WFfmpegUnlock unlock;

    QString url;
    QString urlAudio;

    bool loop;

    WAbstractBackend::Output output;

    int session;

    QList<WFfmpegInput *> inputs;

    QThread * threadDecode;
    QThread * threadVideo;
    QThread * threadAudio;

    // NOTE: The mutex guards the variables shared between the threads, from here to 'volume'.
    QMutex         mutex;
    QWaitCondition condition;

    QAtomicInt abort;

    int opened;

    bool ready;

    WFfmpegQueue queueVideo;
    WFfmpegQueue queueAudio;

    QQueue<WFfmpegFrame> frames;

    // NOTE: The serial whose video reached its end, -1 otherwise.
    int serialEnd;

    WFfmpegQueue * starving;

    int serial;
    int serialVideo;
    int serialAudio;

    int seekVideo;
    int seekAudio;

    bool playing;
    bool step;

    qreal speed;

    // NOTE: The audio leads the clock until it ends.
    bool audio;
    bool audioEnded;

    QElapsedTimer clock;

    int  clockTime;
    bool clockValid;

    int timeVideo;

    // NOTE: The last time sent to the backend, -1 sends the next one.
    int timePosted;

    qint64 swapTime;
    qint64 swapPeriod;

    // NOTE: The swap slot of the last frame shown and its time (see getWait).
    qint64 slotTime;
    int    slotFrame;

    QString proxy;

    int quality;

    // NOTE: The last source opened is a variant list.
    bool hasVariants;

    int idVideo;
    int idAudio;

    int volume;

    WFfmpegBase baseVideo;
    WFfmpegBase baseAudio;

    AVCodecParameters * parameters;

    AVCodecContext * codecVideo;
    AVCodecContext * codecAudio;

    AVBufferRef * device;

    int formatHardware;

    bool decoded;

    // NOTE: The video decodes on the hardware, it falls back to software on its first frame.
    bool accelerated;

    AVFrame * frameDecoded;
    AVFrame * frameSoftware;
    AVFrame * frameDepth;

    // NOTE: The GPU filter of the frames of more than 8 bits, for one frames context.
    AVFilterGraph   * graph;
    AVFilterContext * filterIn;
    AVFilterContext * filterOut;

    void * graphFrames;

    SwsContext * scale;
    SwrContext * resample;

    int width;
    int height;

    int pitches[3];

    uchar * planes[3];

    bool locked;

    qreal frameRate;

protected:
    W_DECLARE_PUBLIC(WFfmpegPlayer)
};

#endif // SK_NO_FFMPEGPLAYER
#endif // WFFMPEGPLAYER_P_H
