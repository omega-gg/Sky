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

#include "WVlcPlayer.h"

#ifndef SK_NO_VLCPLAYER

// Qt includes
#include <QCoreApplication>
#include <QMutex>

// Private includes
#include <private/WVlcEngine_p>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include "WVlcPlayer_p.h"

WVlcPlayerPrivate::WVlcPlayerPrivate(WVlcPlayer * p) : WPrivate(p) {}

void WVlcPlayerPrivate::init(WVlcEngine * engine, QThread * thread)
{
    Q_Q(WVlcPlayer);

    this->engine = engine;

    player = NULL;

    backend = NULL;

    output = WAbstractBackend::OutputMedia;

    networkCache = -1;

    if (thread) q->moveToThread(thread);

    QCoreApplication::postEvent(q, new QEvent(static_cast<QEvent::Type>
                                              (WVlcPlayerPrivate::EventCreate)),
                                Qt::HighEventPriority * 100);
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

QString WVlcPlayerPrivate::encodeUrl(const QString & url) const
{
    QString result = url;

    return result.replace(" ", "%20");
}

//-------------------------------------------------------------------------------------------------

void WVlcPlayerPrivate::clearDiscoverers()
{
    foreach (libvlc_renderer_discoverer_t * discoverer, discoverers)
    {
        // NOTE: This calls also stops the discoverer.
        libvlc_renderer_discoverer_release(discoverer);
    }

    discoverers.clear();

    renderers.clear();

    // NOTE: Since we are no longer receiving renderers events we clear them completely.
    QCoreApplication::postEvent(backend, new QEvent(static_cast<QEvent::Type>
                                                    (WVlcPlayer::EventOutputClear)));
}

//-------------------------------------------------------------------------------------------------
// Private static events
//-------------------------------------------------------------------------------------------------
// Player

/* static */ void WVlcPlayerPrivate::onPlaying(const struct libvlc_event_t *, void * data)
{
    WVlcPlayerPrivate * d = static_cast<WVlcPlayerPrivate *> (data);

    if (d->backend == NULL) return;

    QCoreApplication::postEvent(d->backend,
                                new QEvent(static_cast<QEvent::Type> (WVlcPlayer::EventPlaying)));
}

/* static */ void WVlcPlayerPrivate::onPaused(const struct libvlc_event_t *, void * data)
{
    WVlcPlayerPrivate * d = static_cast<WVlcPlayerPrivate *> (data);

    if (d->backend == NULL) return;

    QCoreApplication::postEvent(d->backend,
                                new QEvent(static_cast<QEvent::Type> (WVlcPlayer::EventPaused)));
}

/* static */ void WVlcPlayerPrivate::onStopped(const struct libvlc_event_t *, void * data)
{
    WVlcPlayerPrivate * d = static_cast<WVlcPlayerPrivate *> (data);

    if (d->backend == NULL) return;

    QCoreApplication::postEvent(d->backend,
                                new QEvent(static_cast<QEvent::Type> (WVlcPlayer::EventStopped)));
}

//-------------------------------------------------------------------------------------------------

/* static */ void WVlcPlayerPrivate::onBuffering(const struct libvlc_event_t *, void * data)
{
    WVlcPlayerPrivate * d = static_cast<WVlcPlayerPrivate *> (data);

    if (d->backend == NULL) return;

    QCoreApplication::postEvent(d->backend, new QEvent(static_cast<QEvent::Type>
                                                       (WVlcPlayer::EventBuffering)));
}

//-------------------------------------------------------------------------------------------------

/* static */ void WVlcPlayerPrivate::onLengthChanged(const struct libvlc_event_t * event,
                                                     void                        * data)
{
    WVlcPlayerPrivate * d = static_cast<WVlcPlayerPrivate *> (data);

    if (d->backend == NULL) return;

#ifdef Q_OS_LINUX
    qint64 length = event->u.media_player_length_changed.new_length;
#else
    libvlc_time_t length = event->u.media_player_length_changed.new_length;
#endif

    QCoreApplication::postEvent(d->backend,
                                new WVlcPlayerEvent(WVlcPlayer::EventLengthChanged, length));
}

/* static */ void WVlcPlayerPrivate::onTimeChanged(const struct libvlc_event_t * event,
                                                   void                        * data)
{
    WVlcPlayerPrivate * d = static_cast<WVlcPlayerPrivate *> (data);

    if (d->backend == NULL) return;

    int time = event->u.media_player_time_changed.new_time;

    QCoreApplication::postEvent(d->backend,
                                new WVlcPlayerEvent(WVlcPlayer::EventTimeChanged, time));
}

//-------------------------------------------------------------------------------------------------

/* static */ void WVlcPlayerPrivate::onEndReached(const struct libvlc_event_t *, void * data)
{
    WVlcPlayerPrivate * d = static_cast<WVlcPlayerPrivate *> (data);

    if (d->backend == NULL) return;

    QCoreApplication::postEvent(d->backend, new QEvent(static_cast<QEvent::Type>
                                                       (WVlcPlayer::EventEndReached)));
}

//-------------------------------------------------------------------------------------------------

/* static */ void WVlcPlayerPrivate::onEncounteredError(const struct libvlc_event_t *, void * data)
{
    WVlcPlayerPrivate * d = static_cast<WVlcPlayerPrivate *> (data);

    if (d->backend == NULL) return;

    QCoreApplication::postEvent(d->backend,
                                new QEvent(static_cast<QEvent::Type> (WVlcPlayer::EventError)));
}

//-------------------------------------------------------------------------------------------------
// Renderer

/* static */ void WVlcPlayerPrivate::onRendererAdded(const struct libvlc_event_t * event,
                                                     void                        * data)
{
    WVlcPlayerPrivate * d = static_cast<WVlcPlayerPrivate *> (data);

    libvlc_renderer_item_t * item = event->u.renderer_discoverer_item_added.item;

    d->renderers.append(item);

    QString string = libvlc_renderer_item_type(item);

    WAbstractBackend::OutputType type;

    if (string == "chromecast") type = WAbstractBackend::TypeChromecast;
    else                        type = WAbstractBackend::TypeUnknown;

    WVlcOutputEvent * eventOutput = new WVlcOutputEvent(libvlc_renderer_item_name(item), type);

    QCoreApplication::postEvent(d->backend, eventOutput);
}

/* static */ void WVlcPlayerPrivate::onRendererDeleted(const struct libvlc_event_t * event,
                                                       void                        * data)
{
    WVlcPlayerPrivate * d = static_cast<WVlcPlayerPrivate *> (data);

    libvlc_renderer_item_t * item = event->u.renderer_discoverer_item_deleted.item;

    for (int i = 0; i < d->renderers.count(); i++)
    {
        if (d->renderers.at(i) != item) continue;

        d->renderers.removeAt(i);

        QCoreApplication::postEvent(d->backend,
                                    new WVlcPlayerEvent(WVlcPlayer::EventOutputRemove, i));

        return;
    }
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

WVlcPlayer::WVlcPlayer(WVlcEngine * engine, QThread * thread, QObject * parent)
    : QObject(parent), WPrivatable(new WVlcPlayerPrivate(this))
{
    Q_D(WVlcPlayer); d->init(engine, thread);
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WVlcPlayer::setBackend(QObject * backend, libvlc_video_format_cb  setup,
                                                                 libvlc_video_cleanup_cb cleanup,
                                                                 libvlc_video_lock_cb    lock,
                                                                 libvlc_video_unlock_cb  unlock,
                                                                 libvlc_video_display_cb display)
{
    QCoreApplication::postEvent(this, new WVlcPlayerEventBackend(backend, setup, cleanup, lock,
                                                                 unlock, display));
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WVlcPlayer::setSource(const QString & media, const QString & audio)
{
    QCoreApplication::postEvent(this, new WVlcPlayerEventSource(media, audio));
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WVlcPlayer::play(int at)
{
    QCoreApplication::postEvent(this,
                                new WVlcPlayerPrivateEvent(WVlcPlayerPrivate::EventPlay, at));
}

/* Q_INVOKABLE */ void WVlcPlayer::pause()
{
    QCoreApplication::postEvent(this, new QEvent(static_cast<QEvent::Type>
                                                 (WVlcPlayerPrivate::EventPause)));
}

/* Q_INVOKABLE */ void WVlcPlayer::stop()
{
    QCoreApplication::postEvent(this, new QEvent(static_cast<QEvent::Type>
                                                 (WVlcPlayerPrivate::EventStop)));
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WVlcPlayer::seek(int msec)
{
    QCoreApplication::postEvent(this, new WVlcPlayerPrivateEvent(WVlcPlayerPrivate::EventSeek,
                                                                 msec));
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WVlcPlayer::setSpeed(qreal speed)
{
    QCoreApplication::postEvent(this, new WVlcPlayerPrivateEvent(WVlcPlayerPrivate::EventSpeed,
                                                                 speed));
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WVlcPlayer::setVolume(int percent)
{
    QCoreApplication::postEvent(this, new WVlcPlayerPrivateEvent(WVlcPlayerPrivate::EventVolume,
                                                                 percent));
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WVlcPlayer::setScanOutput(bool enabled)
{
    QCoreApplication::postEvent(this, new WVlcPlayerPrivateEvent(WVlcPlayerPrivate::EventScan,
                                                                 enabled));
}

/* Q_INVOKABLE */ void WVlcPlayer::setCurrentOutput(int index)
{
    QCoreApplication::postEvent(this, new WVlcPlayerPrivateEvent(WVlcPlayerPrivate::EventOutput,
                                                                 index));
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WVlcPlayer::setProxy(const QString & host,
                                            int             port, const QString & password)
{
    Q_D(WVlcPlayer);

    d->mutex.lock();

    d->proxyHost     = host + ':' + QString::number(port);
    d->proxyPassword = password;

    d->mutex.unlock();
}

/* Q_INVOKABLE */ void WVlcPlayer::clearProxy()
{
    Q_D(WVlcPlayer);

    d->mutex.lock();

    d->proxyHost     = QString();
    d->proxyPassword = QString();

    d->mutex.unlock();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WVlcPlayer::deletePlayer()
{
    QCoreApplication::postEvent(this, new QEvent(static_cast<QEvent::Type>
                                                 (WVlcPlayerPrivate::EventDelete)),
                                Qt::HighEventPriority * 100);
}

//-------------------------------------------------------------------------------------------------
// Events
//-------------------------------------------------------------------------------------------------

/* virtual */ bool WVlcPlayer::event(QEvent * event)
{
    Q_D(WVlcPlayer);

    QEvent::Type type = event->type();

    if (type == static_cast<QEvent::Type> (WVlcPlayerPrivate::EventCreate))
    {
        d->player = libvlc_media_player_new(d->engine->d_func()->instance);

        // FIXME: Applying the player default volume.
        //libvlc_audio_set_volume(d->player, 100);

        libvlc_event_manager_t * manager = libvlc_media_player_event_manager(d->player);

        libvlc_event_attach(manager, libvlc_MediaPlayerPlaying, d->onPlaying, d);
        libvlc_event_attach(manager, libvlc_MediaPlayerPaused,  d->onPaused,  d);
        libvlc_event_attach(manager, libvlc_MediaPlayerStopped, d->onStopped, d);

        libvlc_event_attach(manager, libvlc_MediaPlayerBuffering, d->onBuffering, d);

        libvlc_event_attach(manager, libvlc_MediaPlayerLengthChanged, d->onLengthChanged, d);
        libvlc_event_attach(manager, libvlc_MediaPlayerTimeChanged,   d->onTimeChanged,   d);

        libvlc_event_attach(manager, libvlc_MediaPlayerEndReached, d->onEndReached, d);

        libvlc_event_attach(manager, libvlc_MediaPlayerEncounteredError, d->onEncounteredError, d);

        return true;
    }
    else if (d->player == NULL)
    {
        return QObject::event(event);
    }
    else if (type == static_cast<QEvent::Type> (WVlcPlayerPrivate::EventBackend))
    {
        WVlcPlayerEventBackend * eventBackend = static_cast<WVlcPlayerEventBackend *> (event);

        if (d->backend)
        {
            qWarning("WVlcPlayer::event: Player already has a backend.");

            return true;
        }

        Q_ASSERT(eventBackend->backend);

        d->backend = eventBackend->backend;

        libvlc_video_set_format_callbacks(d->player, eventBackend->setup, eventBackend->cleanup);

        libvlc_video_set_callbacks(d->player, eventBackend->lock, eventBackend->unlock,
                                   eventBackend->display, eventBackend->backend);

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcPlayerPrivate::EventSource))
    {
        WVlcPlayerEventSource * eventSource = static_cast<WVlcPlayerEventSource *> (event);

        WAbstractBackend::Output output;

        QString cache;

        QString proxy;
        QString proxyPassword;

        d->mutex.lock();

        output = d->output;

        // NOTE: I'm not sure we need this anymore.
        if (d->networkCache != -1)
        {
            cache = "network-caching=" + QString::number(d->networkCache);
        }

        if (d->proxyHost.isEmpty() == false)
        {
            proxy         = "http-proxy="     + d->proxyHost;
            proxyPassword = "http-proxy-pwd=" + d->proxyPassword;
        }

        d->mutex.unlock();

        libvlc_media_t * media;

        if (output == WAbstractBackend::OutputAudio)
        {
            const QString & audio = eventSource->audio;

            if (audio.isEmpty())
            {
                media = libvlc_media_new_location(d->engine->d_func()->instance,
                                                  d->encodeUrl(eventSource->media).C_UTF);

                libvlc_media_add_option(media, "no-video");
            }
            else media = libvlc_media_new_location(d->engine->d_func()->instance,
                                                   d->encodeUrl(eventSource->audio).C_UTF);
        }
        else
        {
            media = libvlc_media_new_location(d->engine->d_func()->instance,
                                              d->encodeUrl(eventSource->media).C_UTF);

            if (output != WAbstractBackend::OutputVideo)
            {
                const QString & audio = eventSource->audio;

                if (audio.isEmpty() == false)
                {
                    QString input = "input-slave=" + d->encodeUrl(audio);

                    libvlc_media_add_option(media, input.C_UTF);
                }
            }
            else libvlc_media_add_option(media, "no-audio");
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

        foreach (const QString & option, d->options)
        {
            libvlc_media_add_option(media, option.C_STR);
        }

        libvlc_media_player_set_media(d->player, media);

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcPlayerPrivate::EventPlay))
    {
        WVlcPlayerPrivateEvent * eventPlayer = static_cast<WVlcPlayerPrivateEvent *> (event);

        int at = eventPlayer->value.toInt();

        libvlc_media_player_play(d->player);

        if (at) libvlc_media_player_set_time(d->player, at);

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcPlayerPrivate::EventPause))
    {
        libvlc_media_player_set_pause(d->player, true);

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcPlayerPrivate::EventStop))
    {
        libvlc_media_player_stop(d->player);

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcPlayerPrivate::EventSeek))
    {
        WVlcPlayerPrivateEvent * eventPlayer = static_cast<WVlcPlayerPrivateEvent *> (event);

        libvlc_media_player_set_time(d->player, eventPlayer->value.toInt());

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcPlayerPrivate::EventSpeed))
    {
        WVlcPlayerPrivateEvent * eventPlayer = static_cast<WVlcPlayerPrivateEvent *> (event);

        libvlc_media_player_set_rate(d->player, eventPlayer->value.toFloat());

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcPlayerPrivate::EventVolume))
    {
        WVlcPlayerPrivateEvent * eventPlayer = static_cast<WVlcPlayerPrivateEvent *> (event);

        libvlc_audio_set_volume(d->player, eventPlayer->value.toInt());

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcPlayerPrivate::EventScan))
    {
        WVlcPlayerPrivateEvent * eventPlayer = static_cast<WVlcPlayerPrivateEvent *> (event);

        if (eventPlayer->value.toBool() == false)
        {
            d->clearDiscoverers();

            return true;
        }

        //-----------------------------------------------------------------------------------------
        // NOTE: Maybe this should all be done at the VlcEngine level.

        libvlc_rd_description_t ** services;

        libvlc_instance_t * instance = d->engine->d_func()->instance;

        ssize_t count = libvlc_renderer_discoverer_list_get(instance, &services);

        for (int i = 0; i < count; i++)
        {
            libvlc_rd_description_t * service = services[i];

            libvlc_renderer_discoverer_t * discoverer
                = libvlc_renderer_discoverer_new(instance, service->psz_name);

            libvlc_event_manager_t * manager
                = libvlc_renderer_discoverer_event_manager(discoverer);

            libvlc_event_attach(manager, libvlc_RendererDiscovererItemAdded,
                                d->onRendererAdded, d);

            libvlc_event_attach(manager, libvlc_RendererDiscovererItemDeleted,
                                d->onRendererDeleted, d);

            d->discoverers.append(discoverer);

            libvlc_renderer_discoverer_start(discoverer);
        }

        libvlc_renderer_discoverer_list_release(services, count);

        //-----------------------------------------------------------------------------------------

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcPlayerPrivate::EventOutput))
    {
        WVlcPlayerPrivateEvent * eventPlayer = static_cast<WVlcPlayerPrivateEvent *> (event);

        int index = eventPlayer->value.toInt();

        if (index < 0 || index >= d->renderers.count())
        {
             libvlc_media_player_set_renderer(d->player, NULL);
        }
        else libvlc_media_player_set_renderer(d->player, d->renderers.at(index));

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcPlayerPrivate::EventDelete))
    {
        libvlc_media_player_stop(d->player);

        libvlc_media_player_release(d->player);

        d->player = NULL;

        d->clearDiscoverers();

        if (d->backend)
        {
            QCoreApplication::postEvent(d->backend, new QEvent(static_cast<QEvent::Type>
                                                               (WVlcPlayer::EventDelete)),
                                        Qt::HighEventPriority * 100);
        }

        return true;
    }
    else return QObject::event(event);
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

QStringList WVlcPlayer::options()
{
    Q_D(WVlcPlayer);

    const QMutexLocker locker(&d->mutex);

    return d->options;
}

void WVlcPlayer::setOptions(const QStringList & options)
{
    Q_D(WVlcPlayer);

    QMutexLocker locker(&d->mutex);

    if (d->options == options) return;

    d->options = options;

    locker.unlock();

    emit optionsChanged();
}

//-------------------------------------------------------------------------------------------------

WAbstractBackend::Output WVlcPlayer::output()
{
    Q_D(WVlcPlayer);

    const QMutexLocker locker(&d->mutex);

    return d->output;
}

void WVlcPlayer::setOutput(WAbstractBackend::Output output)
{
    Q_D(WVlcPlayer);

    QMutexLocker locker(&d->mutex);

    if (d->output == output) return;

    d->output = output;

    locker.unlock();

    emit outputChanged();
}

//-------------------------------------------------------------------------------------------------

int WVlcPlayer::networkCache()
{
    Q_D(WVlcPlayer);

    QMutexLocker locker(&d->mutex);

    return d->networkCache;
}

void WVlcPlayer::setNetworkCache(int msec)
{
    Q_D(WVlcPlayer);

    QMutexLocker locker(&d->mutex);

    if (d->networkCache == msec) return;

    d->networkCache = msec;

    locker.unlock();

    emit networkCacheChanged();
}

#endif // SK_NO_VLCPLAYER
