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

#ifndef WVLCAUDIO_H
#define WVLCAUDIO_H

// Qt includes
#include <QObject>

// FIXME MSVC: ssize_t is required by vlc headers.
#ifdef _MSC_VER
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif

// VLC includes
#include <vlc/vlc.h>

// Sk includes
#include <Sk>

#ifndef SK_NO_VLCAUDIO

// Forward declarations
class WVlcAudioPrivate;
class WVlcEngine;

class SK_MEDIA_EXPORT WVlcAudio : public QObject, public WPrivatable
{
    Q_OBJECT

    Q_PROPERTY(QStringList options READ options WRITE setOptions NOTIFY optionsChanged)

    Q_PROPERTY(int networkCache READ networkCache WRITE setNetworkCache NOTIFY networkCacheChanged)

public:
    WVlcAudio(WVlcEngine * engine, QThread * thread = NULL, QObject * parent = NULL);

public: // Interface
    Q_INVOKABLE void setSource(const QString     & url,
                               const QStringList & options = QStringList(), int loop = false);

    Q_INVOKABLE void applyBuffering(float progress);

    Q_INVOKABLE void synchronize(int time);

    Q_INVOKABLE void play(int at = 0);

    Q_INVOKABLE void pause();
    Q_INVOKABLE void stop ();

    Q_INVOKABLE void seek(int msec);

    Q_INVOKABLE void setSpeed(qreal speed);

    Q_INVOKABLE void setVolume(int percent);

    Q_INVOKABLE void setProxy(const QString & host, const QString & password = QString());

    Q_INVOKABLE void clearProxy();

    Q_INVOKABLE void deletePlayer();

protected: // Events
    /* virtual */ bool event(QEvent * event);

signals:
    void waitingChanged(bool waiting);

    void optionsChanged();

    void networkCacheChanged();

public: // Properties
    QStringList options();
    void        setOptions(const QStringList & options);

    int  networkCache();
    void setNetworkCache(int msec);

private:
    W_DECLARE_PRIVATE(WVlcAudio)
};

#endif // SK_NO_VLCAUDIO
#endif // WVLCAUDIO_H
