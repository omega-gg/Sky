//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

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

#ifndef WVLCPLAYER_P_H
#define WVLCPLAYER_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

// Qt includes
#include <QMutex>
#ifdef VLCPLAYER_AUDIO
#include <QTimer>
#endif

// Private includes
#include <private/Sk_p>

#ifndef SK_NO_VLCPLAYER

// Forward declarations
class WVlcPlayerPrivateAudio;
#ifdef VLCPLAYER_AUDIO
class WVlcAudio;
#endif

//-------------------------------------------------------------------------------------------------
// WVlcPlayerPrivate
//-------------------------------------------------------------------------------------------------

class SK_MEDIA_EXPORT WVlcPlayerPrivate : public WPrivate
{
public: // Enums
    enum EventType
    {
        EventCreate = QEvent::User,
        EventBackend, // WVlcPlayerEventBackend
        EventSource,  // WVlcPlayerEvent QString
        EventPlay,
        EventPause,
        EventStop,
        EventSeek,   // WVlcPlayerEvent int
        EventSpeed,  // WVlcPlayerEvent qreal
        EventVolume, // WVlcPlayerEvent int
        EventVideo,  // WVlcPlayerEvent int
        EventAudio,  // WVlcPlayerEvent int
        EventScan,   // WVlcPlayerEvent bool
        EventOutput, // WVlcPlayerEvent int
        EventDelete
    };

public:
    WVlcPlayerPrivate(WVlcPlayer * p);

    void init(WVlcEngine * engine, QThread * thread);

public: // Functions
    libvlc_media_t * createMedia(const QString & url) const;

    QString encodeUrl(const QString & url) const;

    void play(int time);

    void pause();
    void stop ();

    void seek(int time);

    void setSpeed(float speed);

    void setVolume(int volume);

    void setScanOutput(bool enabled);

#if LIBVLC_VERSION_MAJOR > 3
    void applyOpen();
#endif
    void applyPlay();
    void applyEnd ();

    bool checkTime(int at);

#if LIBVLC_VERSION_MAJOR > 3
    libvlc_media_track_t * getTrack(int id, libvlc_track_type_t type) const;
#endif

public: // Static events
#if LIBVLC_VERSION_MAJOR > 3
    static void onChanged(const struct libvlc_event_t * event, void * data);
    static void onOpening(const struct libvlc_event_t * event, void * data);
#endif

    static void onPlaying(const struct libvlc_event_t * event, void * data);
    static void onPaused (const struct libvlc_event_t * event, void * data);
    static void onStopped(const struct libvlc_event_t * event, void * data);

    static void onBuffering(const struct libvlc_event_t * event, void * data);

    static void onLength(const struct libvlc_event_t * event, void * data);
    static void onTime  (const struct libvlc_event_t * event, void * data);

#if LIBVLC_VERSION_MAJOR < 4
    static void onEndReached(const struct libvlc_event_t * event, void * data);
#endif

    static void onEncounteredError(const struct libvlc_event_t * event, void * data);

public: // Slots
    void onPlay ();
    void onPause();

    void onOutputAdded(const WBackendOutput & output);

    void onOutputRemoved(int index);

    void onOutputCleared();

public: // Variables
    WVlcEngine * engine;

    libvlc_media_player_t * player;

#ifdef VLCPLAYER_AUDIO
    WVlcAudio * audio;
#endif

    WAbstractBackend * backend;

    QStringList options;

#ifdef VLCPLAYER_AUDIO
    bool hasAudio;
#endif
    bool hasOutput;

#if LIBVLC_VERSION_MAJOR > 3
    bool opening;
#endif
    bool playing;

    int retry;

    int trackId;

#if LIBVLC_VERSION_MAJOR > 3
    int currentTime;
#endif

    WAbstractBackend::Output output;
    QString                  quality;

    bool scanOutput;

    int networkCache;

    QString proxyHost;
    QString proxyPassword;

    QMutex mutex;

protected:
    W_DECLARE_PUBLIC(WVlcPlayer)
};

//-------------------------------------------------------------------------------------------------
// WVlcPlayerPrivateEvent
//-------------------------------------------------------------------------------------------------

class WVlcPlayerPrivateEvent : public QEvent
{
public:
    WVlcPlayerPrivateEvent(WVlcPlayerPrivate::EventType type, const QVariant & value)
        : QEvent(static_cast<QEvent::Type> (type))
    {
        this->value = value;
    }

public: // Variables
    QVariant value;
};

//-------------------------------------------------------------------------------------------------
// WVlcPlayerEventBackend
//-------------------------------------------------------------------------------------------------

class WVlcPlayerEventBackend : public QEvent
{
public:
    WVlcPlayerEventBackend(WAbstractBackend      * backend,
                           libvlc_video_format_cb  setup,
                           libvlc_video_cleanup_cb cleanup,
                           libvlc_video_lock_cb    lock,
                           libvlc_video_unlock_cb  unlock,
                           libvlc_video_display_cb display)
        : QEvent(static_cast<QEvent::Type> (WVlcPlayerPrivate::EventBackend))
    {
        this->backend = backend;

        this->setup   = setup;
        this->cleanup = cleanup;

        this->lock    = lock;
        this->unlock  = unlock;
        this->display = display;
    }

public: // Variables
    WAbstractBackend * backend;

    libvlc_video_format_cb  setup;
    libvlc_video_cleanup_cb cleanup;

    libvlc_video_lock_cb    lock;
    libvlc_video_unlock_cb  unlock;
    libvlc_video_display_cb display;
};

//-------------------------------------------------------------------------------------------------
// WVlcPlayerEventSource
//-------------------------------------------------------------------------------------------------

class WVlcPlayerEventSource : public QEvent
{
public:
    WVlcPlayerEventSource(const QString & media, const QString & audio, int loop)
        : QEvent(static_cast<QEvent::Type> (WVlcPlayerPrivate::EventSource))
    {
        this->media = media;
        this->audio = audio;

        this->loop = loop;
    }

public: // Variables
    QString media;
    QString audio;

    bool loop;
};

#endif // SK_NO_VLCPLAYER
#endif // WVLCPLAYER_P_H
