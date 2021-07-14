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

#ifndef WVLCPLAYER_H
#define WVLCPLAYER_H

// Qt includes
#include <QEvent>
#include <QVariant>
#include <QStringList>

// FIXME MSVC: ssize_t is required by vlc headers.
#ifdef _MSC_VER
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif

// VLC includes
#include <vlc/vlc.h>

// Sk includes
#include <WVlcEngine>
#include <WAbstractBackend>

#ifndef SK_NO_VLCPLAYER

class WVlcPlayerPrivate;

//-------------------------------------------------------------------------------------------------
// WVlcPlayer
//-------------------------------------------------------------------------------------------------

class SK_MEDIA_EXPORT WVlcPlayer : public QObject, public WPrivatable
{
    Q_OBJECT

    Q_PROPERTY(QStringList options READ options WRITE setOptions NOTIFY optionsChanged)

    Q_PROPERTY(WAbstractBackend::Output output READ output WRITE setOutput NOTIFY outputChanged)

    Q_PROPERTY(int networkCache READ networkCache WRITE setNetworkCache NOTIFY networkCacheChanged)

public: // Enums
    enum EventType
    {
        EventPlaying = QEvent::User,
        EventPaused,
        EventStopped,
        EventBuffering,
        EventLengthChanged, // WVlcPlayerEvent int
        EventTimeChanged,   // WVlcPlayerEvent int
        EventEndReached,
        EventError,
        EventUser,
        EventDelete
    };

public:
    WVlcPlayer(WVlcEngine * engine, QThread * thread = NULL, QObject * parent = NULL);

public: // Interface
    Q_INVOKABLE void setBackend(QObject * backend, libvlc_video_format_cb  setup   = NULL,
                                                   libvlc_video_cleanup_cb cleanup = NULL,
                                                   libvlc_video_lock_cb    lock    = NULL,
                                                   libvlc_video_unlock_cb  unlock  = NULL,
                                                   libvlc_video_display_cb display = NULL);

    Q_INVOKABLE void setSource(const QString & media, const QString & audio = QString());

    Q_INVOKABLE void play(int at = 0);

    Q_INVOKABLE void pause();
    Q_INVOKABLE void stop ();

    Q_INVOKABLE void seek(int msec);

    Q_INVOKABLE void setSpeed(qreal speed);

    Q_INVOKABLE void setVolume(int percent);

    Q_INVOKABLE void setScanOutput(bool enabled);

    Q_INVOKABLE void setProxy(const QString & host,
                              int             port, const QString & password = QString());

    Q_INVOKABLE void clearProxy();

    Q_INVOKABLE void deletePlayer();

protected: // Events
    /* virtual */ bool event(QEvent * event);

signals:
    void optionsChanged();

    void outputChanged();

    void networkCacheChanged();

public: // Properties
    QStringList options();
    void        setOptions(const QStringList & options);

    WAbstractBackend::Output output();
    void                     setOutput(WAbstractBackend::Output output);

    int  networkCache();
    void setNetworkCache(int msec);

private:
    W_DECLARE_PRIVATE(WVlcPlayer)

    friend class WBackendVlc;
    friend class WBackendVlcPrivate;
};

//-------------------------------------------------------------------------------------------------
// WVlcPlayerEvent
//-------------------------------------------------------------------------------------------------

class WVlcPlayerEvent : public QEvent
{
public:
    WVlcPlayerEvent(WVlcPlayer::EventType type, const QVariant & value)
        : QEvent(static_cast<QEvent::Type> (type))
    {
        this->value = value;
    }

public: // Variables
    QVariant value;
};

#endif // SK_NO_VLCPLAYER
#endif // WVLCPLAYER_H
