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

#ifndef WVLCAUDIO_P_H
#define WVLCAUDIO_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

// Qt includes
#include <QMetaMethod>
#include <QEvent>
#include <QTimer>
#include <QVariant>
#include <QMutex>
#ifdef QT_4
#include <QStringList>
#endif

// Private includes
#include <private/Sk_p>

#ifndef SK_NO_VLCAUDIO

//-------------------------------------------------------------------------------------------------
// WVlcAudioPrivate
//-------------------------------------------------------------------------------------------------

class SK_MEDIA_EXPORT WVlcAudioPrivate : public WPrivate
{
public: // Enums
    enum EventType
    {
        EventCreate = QEvent::User,
        EventSource,
        EventBuffering,
        EventSynchronize,
        EventPause,
        EventStop,
        EventSpeed,
        EventVolume,
        EventDelete
    };

public:
    WVlcAudioPrivate(WVlcAudio * p);

    void init(WVlcEngine * engine, QThread * thread);

public: // Functions
    void create();

    void setSource(const QString & url, int loop);

    void synchronize(int time);

    void pause();
    void stop ();

    void applyBuffering(float progress);

    void setSpeed(qreal speed);

    void setVolume(int percent);

    void deletePlayer();

    void applyPlay();

    void applyTime(int time);

    void clearDelay();

    void setWait(bool enabled);

public: // Static events
    static void onPlaying(const struct libvlc_event_t * event, void * data);
    static void onPaused (const struct libvlc_event_t * event, void * data);
    static void onStopped(const struct libvlc_event_t * event, void * data);

    static void onBuffering(const struct libvlc_event_t * event, void * data);

    static void onTime(const struct libvlc_event_t * event, void * data);

public: // Variables
    QThread    * thread;
    WVlcEngine * engine;

    libvlc_media_player_t * player;

    bool playing;
    bool buffering;

    bool playerBuffering;

    bool wait;

    qint64 delay;

    int count;

    QStringList options;

    int networkCache;

    QString proxyHost;
    QString proxyPassword;

    QMutex mutex;

protected:
    W_DECLARE_PUBLIC(WVlcAudio)
};

//-------------------------------------------------------------------------------------------------
// WVlcAudioPrivateEvent
//-------------------------------------------------------------------------------------------------

class WVlcAudioPrivateEvent : public QEvent
{
public:
    WVlcAudioPrivateEvent(WVlcAudioPrivate::EventType type, const QVariant & value)
        : QEvent(static_cast<QEvent::Type> (type))
    {
        this->value = value;
    }

public: // Variables
    QVariant value;
};

//-------------------------------------------------------------------------------------------------
// WVlcAudioEventSource
//-------------------------------------------------------------------------------------------------

class WVlcAudioEventSource : public QEvent
{
public:
    WVlcAudioEventSource(const QString & url, int loop)
        : QEvent(static_cast<QEvent::Type> (WVlcAudioPrivate::EventSource))
    {
        this->url = url;

        this->loop = loop;
    }

public: // Variables
    QString url;

    bool loop;
};

#endif // SK_NO_VLCAUDIO
#endif // WVLCAUDIO_P_H
