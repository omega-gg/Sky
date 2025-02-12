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

#include "WVlcAudio.h"

#ifndef SK_NO_VLCAUDIO

// Qt includes
#include <QCoreApplication>

// Sk includes
#include <WVlcEngine>
#include <WAbstractBackend>

// Private includes
#include <private/WVlcEngine_p>

//-------------------------------------------------------------------------------------------------
// Static variables

static const int AUDIO_RESYNCHRONIZE = 2000; // 2 seconds

static const int AUDIO_DELAY = 1000; // 1 second

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

WVlcAudioPrivate::WVlcAudioPrivate(WVlcAudio * p) : WPrivate(p) {}

void WVlcAudioPrivate::init(WVlcEngine * engine, QThread * thread)
{
    Q_Q(WVlcAudio);

    this->engine = engine;
    this->thread = thread;

    player = NULL;

    playing   = false;
    buffering = false;

    wait = false;

    delay = -1;

    // FIXME: Should we set this to 1000 by default like VLC ?
    networkCache = -1;

    timer.setInterval(AUDIO_DELAY);

    timer.setSingleShot(true);

    const QMetaObject * meta = q->metaObject();

    method = meta->method(meta->indexOfMethod("onWait(bool)"));

    QObject::connect(&timer, SIGNAL(timeout()), q, SIGNAL(triggerPause()));

    if (thread == NULL)
    {
        create();

        return;
    }

    q->moveToThread(thread);

    timer.moveToThread(thread);

    QCoreApplication::postEvent(q, new QEvent(static_cast<QEvent::Type>
                                                   (WVlcAudioPrivate::EventCreate)),
                                Qt::HighEventPriority * 100);
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WVlcAudioPrivate::create()
{
    player = libvlc_media_player_new(engine->d_func()->instance);

    libvlc_event_manager_t * manager = libvlc_media_player_event_manager(player);

    libvlc_event_attach(manager, libvlc_MediaPlayerPlaying, onPlaying, this);
    libvlc_event_attach(manager, libvlc_MediaPlayerPaused,  onPaused,  this);
    libvlc_event_attach(manager, libvlc_MediaPlayerStopped, onStopped, this);

    libvlc_event_attach(manager, libvlc_MediaPlayerTimeChanged, onTime, this);
}

void WVlcAudioPrivate::setSource(const QString & url, int loop)
{
    if (player == NULL) return;

    QString cache;

    QString proxy;
    QString proxyPassword;

    mutex.lock();

    // NOTE: I'm not sure we need this anymore.
    if (networkCache != -1)
    {
        cache = "network-caching=" + QString::number(networkCache);
    }

    if (proxyHost.isEmpty() == false)
    {
        proxy         = "http-proxy="     + proxyHost;
        proxyPassword = "http-proxy-pwd=" + proxyPassword;
    }

    mutex.unlock();

    QString source = url;

    source.replace(" ", "%20");

#if LIBVLC_VERSION_MAJOR < 4
    libvlc_media_t * media = libvlc_media_new_location(engine->d_func()->instance, source.C_UTF);
#else
    libvlc_media_t * media = libvlc_media_new_location(source.C_UTF);
#endif

    libvlc_media_add_option(media, "no-video");

    if (loop)
    {
        // NOTE: We use the maximum value given we can't set an infinite value.
        libvlc_media_add_option(media, "input-repeat=65535");
    }

    if (cache.isNull() == false)
    {
        libvlc_media_add_option(media, cache.C_STR);
    }

    if (proxy.isNull() == false)
    {
        libvlc_media_add_option(media, proxy        .C_STR);
        libvlc_media_add_option(media, proxyPassword.C_STR);
    }

    foreach (const QString & option, options)
    {
        libvlc_media_add_option(media, option.C_STR);
    }

    libvlc_media_player_set_media(player, media);
}

void WVlcAudioPrivate::play(int time)
{
    if (player == NULL) return;

    if (playing)
    {
        if (delay == -1)
        {
            applyTime(time);

            return;
        }

        qint64 gap = libvlc_media_player_get_time(player) - time;

        if (delay == gap) return;

        if (qAbs(gap) > AUDIO_RESYNCHRONIZE)
        {
            applyTime(time);

            return;
        }

        delay = gap;

        libvlc_audio_set_delay(player, gap * 1000);

        return;
    }

    libvlc_state_t state = libvlc_media_player_get_state(player);

    if (state == libvlc_Opening) return;

    buffering = false;

    if (state == libvlc_Paused && delay == -1)
    {
        applyTime(time);
    }

    libvlc_media_player_play(player);
}

void WVlcAudioPrivate::pause()
{
    if (player == NULL || playing == false) return;

    playing = false;

    wait = false;

    libvlc_media_player_set_pause(player, 1);

    clearDelay();
}

void WVlcAudioPrivate::stop()
{
    if (player == NULL) return;

    playing   = false;
    buffering = false;

    wait = false;

#if LIBVLC_VERSION_MAJOR < 4
    libvlc_media_player_stop(player);

    clearDelay();
#else
    libvlc_media_player_set_pause(player, 1);

    clearDelay();

    libvlc_media_player_set_time(player, 0, false);
#endif
}

void WVlcAudioPrivate::applyBuffering()
{
    if (playing)
    {
        playing = false;

        libvlc_media_player_set_pause(player, 1);

        clearDelay();
    }
    else buffering = true;
}

void WVlcAudioPrivate::setSpeed(qreal speed)
{
    if (player == NULL) return;

    libvlc_media_player_set_rate(player, speed);
}

void WVlcAudioPrivate::setVolume(int percent)
{
    if (player == NULL) return;

    libvlc_audio_set_volume(player, percent);
}

void WVlcAudioPrivate::deletePlayer()
{
    if (player == NULL) return;

    stop();

    libvlc_media_player_release(player);
}

//-------------------------------------------------------------------------------------------------

void WVlcAudioPrivate::applyPlay()
{
    if (buffering)
    {
        buffering = false;

        libvlc_media_player_set_pause(player, 1);
    }
    else playing = true;
}

void WVlcAudioPrivate::applyTime(int time)
{
    setWait(true);

    delay = 0;

#if LIBVLC_VERSION_MAJOR < 4
    libvlc_media_player_set_time(player, time);
#else
    libvlc_media_player_set_time(player, time, false);
#endif
}

void WVlcAudioPrivate::clearDelay()
{
    if (delay == -1) return;

    delay = -1;

    libvlc_audio_set_delay(player, 0);
}

void WVlcAudioPrivate::setWait(bool enabled)
{
    Q_Q(WVlcAudio);

    // NOTE Qt: QTimer(s) cannot be called from a different thread.
    method.invoke(q, Qt::QueuedConnection, Q_ARG(bool, enabled));
}

//-------------------------------------------------------------------------------------------------
// Private static events
//-------------------------------------------------------------------------------------------------

/* static */ void WVlcAudioPrivate::onPlaying(const struct libvlc_event_t *, void * data)
{
    WVlcAudioPrivate * d = static_cast<WVlcAudioPrivate *> (data);

    d->applyPlay();
}

/* static */ void WVlcAudioPrivate::onPaused(const struct libvlc_event_t *, void * data)
{
    WVlcAudioPrivate * d = static_cast<WVlcAudioPrivate *> (data);

    d->playing = false;
}

/* static */ void WVlcAudioPrivate::onStopped(const struct libvlc_event_t *, void * data)
{
    WVlcAudioPrivate * d = static_cast<WVlcAudioPrivate *> (data);

    d->playing   = false;
    d->buffering = false;
}

/* static */ void WVlcAudioPrivate::onTime(const struct libvlc_event_t *, void * data)
{
    WVlcAudioPrivate * d = static_cast<WVlcAudioPrivate *> (data);

#if LIBVLC_VERSION_MAJOR > 3
    // FIXME VLC 4.0.0: Sometimes the playing event is not called.
    if (d->playing == false && libvlc_media_player_get_state(d->player) == libvlc_Playing)
    {
        d->applyPlay();
    }
#endif

    d->setWait(false);
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WVlcAudioPrivate::onWait(bool enabled)
{
    QMutexLocker locker(&mutex);

    if (wait == enabled) return;

    Q_Q(WVlcAudio);

    wait = enabled;

    if (wait == false)
    {
        timer.stop();

        emit q->triggerPlay();
    }
    else timer.start();

    emit q->waitingChanged();
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

WVlcAudio::WVlcAudio(WVlcEngine * engine, QThread * thread, QObject * parent)
    : QObject(parent), WPrivatable(new WVlcAudioPrivate(this))
{
    Q_D(WVlcAudio); d->init(engine, thread);
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WVlcAudio::setSource(const QString & url, int loop)
{
    Q_D(WVlcAudio);

    if (d->thread == NULL)
    {
        d->setSource(url, loop);

        return;
    }

    QCoreApplication::postEvent(this, new WVlcAudioEventSource(url, loop));
}

/* Q_INVOKABLE */ void WVlcAudio::play(int time)
{
    Q_D(WVlcAudio);

    if (d->thread == NULL)
    {
        d->play(time);

        return;
    }

    QCoreApplication::postEvent(this, new WVlcAudioPrivateEvent(WVlcAudioPrivate::EventPlay,
                                                                time));
}

/* Q_INVOKABLE */ void WVlcAudio::pause()
{
    Q_D(WVlcAudio);

    if (d->thread == NULL)
    {
        d->pause();

        return;
    }

    QCoreApplication::postEvent(this, new QEvent(static_cast<QEvent::Type>
                                                 (WVlcAudioPrivate::EventPause)));
}

/* Q_INVOKABLE */ void WVlcAudio::stop()
{
    Q_D(WVlcAudio);

    if (d->thread == NULL)
    {
        d->stop();

        return;
    }

    QCoreApplication::postEvent(this, new QEvent(static_cast<QEvent::Type>
                                                 (WVlcAudioPrivate::EventStop)));
}

/* Q_INVOKABLE */ void WVlcAudio::applyBuffering()
{
    Q_D(WVlcAudio);

    if (d->thread == NULL)
    {
        d->applyBuffering();

        return;
    }

    QCoreApplication::postEvent(this, new QEvent(static_cast<QEvent::Type>
                                                 (WVlcAudioPrivate::EventBuffering)));
}

/* Q_INVOKABLE */ void WVlcAudio::setSpeed(qreal speed)
{
    Q_D(WVlcAudio);

    if (d->thread == NULL)
    {
        d->setSpeed(speed);

        return;
    }

    QCoreApplication::postEvent(this, new WVlcAudioPrivateEvent(WVlcAudioPrivate::EventSpeed,
                                                                speed));
}

/* Q_INVOKABLE */ void WVlcAudio::setVolume(int percent)
{
    Q_D(WVlcAudio);

    if (d->thread == NULL)
    {
        d->setVolume(percent);

        return;
    }

    QCoreApplication::postEvent(this, new WVlcAudioPrivateEvent(WVlcAudioPrivate::EventVolume,
                                                                percent));
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WVlcAudio::setProxy(const QString & host, const QString & password)
{
    Q_D(WVlcAudio);

    d->mutex.lock();

    d->proxyHost     = host;
    d->proxyPassword = password;

    d->mutex.unlock();
}

/* Q_INVOKABLE */ void WVlcAudio::clearProxy()
{
    Q_D(WVlcAudio);

    d->mutex.lock();

    d->proxyHost     = QString();
    d->proxyPassword = QString();

    d->mutex.unlock();
}

/* Q_INVOKABLE */ void WVlcAudio::deletePlayer()
{
    Q_D(WVlcAudio);

    if (d->thread == NULL)
    {
        d->deletePlayer();

        return;
    }

    QCoreApplication::postEvent(this, new QEvent(static_cast<QEvent::Type>
                                                 (WVlcAudioPrivate::EventDelete)),
                                Qt::HighEventPriority * 100);
}

//-------------------------------------------------------------------------------------------------
// Events
//-------------------------------------------------------------------------------------------------

/* virtual */ bool WVlcAudio::event(QEvent * event)
{
    Q_D(WVlcAudio);

    QEvent::Type type = event->type();

    if (type == static_cast<QEvent::Type> (WVlcAudioPrivate::EventCreate))
    {
        d->create();

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcAudioPrivate::EventSource))
    {
        WVlcAudioEventSource * eventSource = static_cast<WVlcAudioEventSource *> (event);

        d->setSource(eventSource->url, eventSource->loop);

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcAudioPrivate::EventPlay))
    {
        WVlcAudioPrivateEvent * eventPlayer = static_cast<WVlcAudioPrivateEvent *> (event);

        d->play(eventPlayer->value.toInt());

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcAudioPrivate::EventPause))
    {
        d->pause();

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcAudioPrivate::EventStop))
    {
        d->stop();

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcAudioPrivate::EventBuffering))
    {
        d->applyBuffering();

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcAudioPrivate::EventSpeed))
    {
        WVlcAudioPrivateEvent * eventPlayer = static_cast<WVlcAudioPrivateEvent *> (event);

        d->setSpeed(eventPlayer->value.toInt());

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcAudioPrivate::EventVolume))
    {
        WVlcAudioPrivateEvent * eventPlayer = static_cast<WVlcAudioPrivateEvent *> (event);

        d->setVolume(eventPlayer->value.toInt());

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcAudioPrivate::EventDelete))
    {
        d->deletePlayer();

        return true;
    }
    else return QObject::event(event);
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

bool WVlcAudio::isWaiting()
{
    Q_D(WVlcAudio);

    const QMutexLocker locker(&d->mutex);

    return d->wait;
}

QStringList WVlcAudio::options()
{
    Q_D(WVlcAudio);

    const QMutexLocker locker(&d->mutex);

    return d->options;
}

void WVlcAudio::setOptions(const QStringList & options)
{
    Q_D(WVlcAudio);

    QMutexLocker locker(&d->mutex);

    if (d->options == options) return;

    d->options = options;

    locker.unlock();

    emit optionsChanged();
}

int WVlcAudio::networkCache()
{
    Q_D(WVlcAudio);

    QMutexLocker locker(&d->mutex);

    return d->networkCache;
}

void WVlcAudio::setNetworkCache(int msec)
{
    Q_D(WVlcAudio);

    QMutexLocker locker(&d->mutex);

    if (d->networkCache == msec) return;

    d->networkCache = msec;

    locker.unlock();

    emit networkCacheChanged();
}

#endif // SK_NO_VLCAUDIO
