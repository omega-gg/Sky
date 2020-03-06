//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkMedia module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
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

#include <private/Sk_p>

#ifndef SK_NO_VLCPLAYER

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
        EventDelete
    };

public:
    WVlcPlayerPrivate(WVlcPlayer * p);

    void init(WVlcEngine * engine, QThread * thread);

public: // Functions
    QString encodeUrl(const QString & url) const;

public: // Static functions
    static void eventPlaying(const struct libvlc_event_t * event, void * data);
    static void eventPaused (const struct libvlc_event_t * event, void * data);
    static void eventStopped(const struct libvlc_event_t * event, void * data);

    static void eventBuffering(const struct libvlc_event_t * event, void * data);

    static void eventLengthChanged(const struct libvlc_event_t * event, void * data);
    static void eventTimeChanged  (const struct libvlc_event_t * event, void * data);

    static void eventEndReached(const struct libvlc_event_t * event, void * data);

    static void eventEncounteredError(const struct libvlc_event_t * event, void * data);

public: // Variables
    QMutex mutex;

    WVlcEngine * engine;

    libvlc_media_player_t * player;

    QObject * backend;

    QStringList options;

    WAbstractBackend::Output output;

    int networkCache;

    QString proxyHost;
    QString proxyPassword;

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
    WVlcPlayerEventBackend(QObject * backend,
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
    QObject * backend;

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
    WVlcPlayerEventSource(const QString & media, const QString & audio)
        : QEvent(static_cast<QEvent::Type> (WVlcPlayerPrivate::EventSource))
    {
        this->media = media;
        this->audio = audio;
    }

public: // Variables
    QString media;
    QString audio;
};

#endif // SK_NO_VLCPLAYER
#endif // WVLCPLAYER_P_H
