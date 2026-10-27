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

#ifndef WFFMPEGPLAYER_H
#define WFFMPEGPLAYER_H

// Qt includes
#include <QObject>
#include <QEvent>
#include <QVariant>

// Sk includes
#include <WAbstractBackend>

#ifndef SK_NO_FFMPEGPLAYER

// NOTE: The frame callbacks, as libVLC's: the setup takes the frame size and gives the pitches
//       and lines of the 3 I420 planes, lock gives the planes to write, unlock says it's written.
typedef bool (*WFfmpegSetup)(void * data, int * width, int * height, int * pitches, int * lines);

typedef void (*WFfmpegLock)  (void * data, uchar ** planes);
typedef void (*WFfmpegUnlock)(void * data);

class WFfmpegPlayerPrivate;

class SK_MEDIA_EXPORT WFfmpegPlayer : public QObject, public WPrivatable
{
    Q_OBJECT

public: // Enums
    enum EventType
    {
        EventPlaying = QEvent::User,
        EventBuffering,     // WFfmpegPlayerEvent int
        EventLengthChanged, // WFfmpegPlayerEvent int
        EventTimeChanged,   // WFfmpegPlayerEvent int
        EventEndReached,
        EventTracks,        // WFfmpegTracksEvent
        EventError,
        EventDelete
    };

public:
    WFfmpegPlayer(QObject * parent = NULL);

public: // Interface
    void setBackend(WAbstractBackend * backend, WFfmpegSetup  setup,
                                                WFfmpegLock   lock,
                                                WFfmpegUnlock unlock);

    void setSource(const QString     & url,
                   const QString     & audio   = QString(),
                   const QStringList & options = QStringList(), bool loop = false);

    void play(int at = 0);

    void pause();
    void stop ();

    void seek(int msec);

    void nextFrame();

    void setSpeed(qreal speed);

    void setVolume(int percent);

    void setOutput(WAbstractBackend::Output output);

    void setProxy(const QString & host, const QString & password);
    void clearProxy();

    // NOTE: Returns false when these tracks are picked already.
    bool setTracks(int idVideo, int idAudio);

    // NOTE: The last regular swap of the window and its period in nanoseconds (see getWait).
    void setSwap(qint64 time, qint64 period);

    void deletePlayer();

protected: // Events
    /* virtual */ bool event(QEvent * event);

private:
    W_DECLARE_PRIVATE(WFfmpegPlayer)
};

//-------------------------------------------------------------------------------------------------
// WFfmpegPlayerEvent
//-------------------------------------------------------------------------------------------------

class WFfmpegPlayerEvent : public QEvent
{
public:
    WFfmpegPlayerEvent(WFfmpegPlayer::EventType type, int session,
                       const QVariant & value = QVariant())
        : QEvent(static_cast<QEvent::Type> (type))
    {
        this->session = session;

        this->value = value;
    }

public: // Variables
    int session;

    QVariant value;
};

//-------------------------------------------------------------------------------------------------
// WFfmpegTracksEvent
//-------------------------------------------------------------------------------------------------

class WFfmpegTracksEvent : public WFfmpegPlayerEvent
{
public:
    WFfmpegTracksEvent(int session, const QList<WBackendTrack> & tracks, int trackVideo,
                       int trackAudio, qreal frameRate)
        : WFfmpegPlayerEvent(WFfmpegPlayer::EventTracks, session)
    {
        this->tracks = tracks;

        this->trackVideo = trackVideo;
        this->trackAudio = trackAudio;

        this->frameRate = frameRate;
    }

public: // Variables
    QList<WBackendTrack> tracks;

    int trackVideo;
    int trackAudio;

    qreal frameRate;
};

#include <private/WFfmpegPlayer_p>

#endif // SK_NO_FFMPEGPLAYER
#endif // WFFMPEGPLAYER_H
