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
#include "WVlcPlayer_p.h"
#include <private/WVlcEngine_p>

//-------------------------------------------------------------------------------------------------
// Static variables

static const int PLAYER_RESYNCHRONIZE = 10000; // 10 seconds

//=================================================================================================
// WVlcPlayerPrivateAudio
//=================================================================================================

WVlcPlayerPrivateAudio::WVlcPlayerPrivateAudio(WVlcPlayerPrivate * p, libvlc_instance_t * instance)
{
    player = libvlc_media_player_new(instance);

    opening   = true;
    playing   = false;
    buffering = false;

    delay = -1;

    libvlc_event_manager_t * manager = libvlc_media_player_event_manager(player);

    libvlc_event_attach(manager, libvlc_MediaPlayerPlaying, onPlaying, this);
    libvlc_event_attach(manager, libvlc_MediaPlayerPaused,  onPaused,  this);
    libvlc_event_attach(manager, libvlc_MediaPlayerStopped, onStopped, this);

#if LIBVLC_VERSION_MAJOR > 3
    libvlc_event_attach(manager, libvlc_MediaPlayerTimeChanged, onTime, this);
#endif

    libvlc_media_player_set_rate(player, libvlc_media_player_get_rate(p->player));

    libvlc_audio_set_volume(player, libvlc_audio_get_volume(p->player));
}

/* virtual */ WVlcPlayerPrivateAudio::~WVlcPlayerPrivateAudio()
{
    libvlc_media_player_release(player);
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

void WVlcPlayerPrivateAudio::pause()
{
    if (playing == false) return;

    playing = false;

    libvlc_media_player_set_pause(player, true);

    clearDelay();
}

void WVlcPlayerPrivateAudio::stop()
{
    opening   = true;
    playing   = false;
    buffering = false;

#if LIBVLC_VERSION_MAJOR < 4
    libvlc_media_player_stop(player);

    clearDelay();
#else
    libvlc_media_player_set_pause(player, true);

    clearDelay();

    libvlc_media_player_set_time(player, 0, false);
#endif
}

void WVlcPlayerPrivateAudio::setSpeed(float speed)
{
    libvlc_media_player_set_rate(player, speed);
}

void WVlcPlayerPrivateAudio::setVolume(int volume)
{
    libvlc_audio_set_volume(player, volume);
}

//-------------------------------------------------------------------------------------------------

void WVlcPlayerPrivateAudio::applyBuffering()
{
    if (playing)
    {
        pause();
    }
    else buffering = true;
}

void WVlcPlayerPrivateAudio::applyDelay(int time)
{
    if (playing)
    {
        if (delay == -1)
        {
            applyTime(time);

            return;
        }

        qint64 gap = libvlc_media_player_get_time(player) - time;

        if (delay == gap) return;

        if (qAbs(gap) > PLAYER_RESYNCHRONIZE)
        {
            applyTime(time);

            return;
        }

        delay = gap;

        libvlc_audio_set_delay(player, gap * 1000);

        return;
    }

    buffering = false;

    if (libvlc_media_player_get_state(player) == libvlc_Paused)
    {
        if (delay == -1) applyTime(time);
    }

    libvlc_media_player_play(player);
}

//-------------------------------------------------------------------------------------------------
// Static events
//-------------------------------------------------------------------------------------------------

/* static */ void WVlcPlayerPrivateAudio::onPlaying(const struct libvlc_event_t *, void * data)
{
    WVlcPlayerPrivateAudio * d = static_cast<WVlcPlayerPrivateAudio *> (data);

    d->applyPlay();
}

/* static */ void WVlcPlayerPrivateAudio::onPaused(const struct libvlc_event_t *, void * data)
{
    WVlcPlayerPrivateAudio * d = static_cast<WVlcPlayerPrivateAudio *> (data);

    d->playing = false;
}

/* static */ void WVlcPlayerPrivateAudio::onStopped(const struct libvlc_event_t *, void * data)
{
    WVlcPlayerPrivateAudio * d = static_cast<WVlcPlayerPrivateAudio *> (data);

    d->opening   = true;
    d->playing   = false;
    d->buffering = false;
}

#if LIBVLC_VERSION_MAJOR > 3

/* static */ void WVlcPlayerPrivateAudio::onTime(const struct libvlc_event_t *, void * data)
{
    WVlcPlayerPrivateAudio * d = static_cast<WVlcPlayerPrivateAudio *> (data);

    // FIXME VLC 4.0.0: Sometimes the playing event is not called.
    if (d->playing) return;

    d->applyPlay();
}

#endif

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WVlcPlayerPrivateAudio::applyPlay()
{
    opening = false;

    if (buffering)
    {
        buffering = false;

        libvlc_media_player_set_pause(player, true);
    }
    else playing = true;
}

void WVlcPlayerPrivateAudio::applyTime(int time)
{
    delay = 0;

#if LIBVLC_VERSION_MAJOR < 4
    libvlc_media_player_set_time(player, time);
#else
    libvlc_media_player_set_time(player, time, false);
#endif
}

void WVlcPlayerPrivateAudio::clearDelay()
{
    if (delay == -1) return;

    delay = -1;

    libvlc_audio_set_delay(player, 0);
}

//=================================================================================================
// WVlcPlayerPrivate
//=================================================================================================

WVlcPlayerPrivate::WVlcPlayerPrivate(WVlcPlayer * p) : WPrivate(p) {}

void WVlcPlayerPrivate::init(WVlcEngine * engine, QThread * thread)
{
    Q_Q(WVlcPlayer);

    this->engine = engine;

    player = NULL;

    audio = NULL;

    backend = NULL;

    hasAudio  = false;
    hasOutput = false;

    playing = false;

    trackId = 0;

    currentTime = 0;

    output  = WAbstractBackend::OutputMedia;
    quality = "";

    scanOutput = false;

    // FIXME: Should we set this to 1000 by default like VLC ?
    networkCache = -1;

    if (thread) q->moveToThread(thread);

    QCoreApplication::postEvent(q, new QEvent(static_cast<QEvent::Type>
                                              (WVlcPlayerPrivate::EventCreate)),
                                Qt::HighEventPriority * 100);
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

libvlc_media_t * WVlcPlayerPrivate::createMedia(const QString & url) const
{
#if LIBVLC_VERSION_MAJOR < 4
    return libvlc_media_new_location(engine->d_func()->instance, encodeUrl(url).C_UTF);
#else
    return libvlc_media_new_location(encodeUrl(url).C_UTF);
#endif
}

QString WVlcPlayerPrivate::encodeUrl(const QString & url) const
{
    QString result = url;

    return result.replace(" ", "%20");
}

void WVlcPlayerPrivate::play(int time)
{
    if (playing) return;

    currentTime = time;

    libvlc_media_player_play(player);
}

void WVlcPlayerPrivate::pause()
{
    if (playing == false) return;

    playing = false;

    libvlc_media_player_set_pause(player, true);

    if (hasAudio) audio->pause();
}

void WVlcPlayerPrivate::stop()
{
    playing = false;

    currentTime = 0;

#if LIBVLC_VERSION_MAJOR < 4
    libvlc_media_player_stop(player);
#else
    libvlc_media_player_pause(player);

    libvlc_media_player_set_time(player, 0, false);
#endif

    if (hasAudio) audio->stop();
}

void WVlcPlayerPrivate::seek(int time)
{
#if LIBVLC_VERSION_MAJOR < 4
    libvlc_media_player_set_time(player, time);
#else
    libvlc_media_player_set_time(player, time, false);
#endif

    if (hasAudio) audio->pause();
}

void WVlcPlayerPrivate::setSpeed(float speed)
{
    libvlc_media_player_set_rate(player, speed);

    if (hasAudio) audio->setSpeed(speed);
}

void WVlcPlayerPrivate::setVolume(int volume)
{
    libvlc_audio_set_volume(player, volume);

    if (hasAudio) audio->setVolume(volume);
}

void WVlcPlayerPrivate::setScanOutput(bool enabled)
{
    if (scanOutput == enabled) return;

    scanOutput = enabled;

    WVlcEnginePrivate * p = engine->d_func();

    p->startScan(this, enabled);

    if (enabled)
    {
        const QList<WBackendOutput> & outputs = p->outputs;

        if (outputs.isEmpty()) return;

        QCoreApplication::postEvent(backend, new WVlcOutputsEvent(outputs));
    }
    // NOTE: Since we are no longer receiving renderers events we clear them completely.
    else QCoreApplication::postEvent(backend, new QEvent(static_cast<QEvent::Type>
                                                         (WVlcPlayer::EventOutputClear)));
}

void WVlcPlayerPrivate::applyPlay()
{
    playing = true;

    if (currentTime)
    {
#if LIBVLC_VERSION_MAJOR < 4
        libvlc_media_player_set_time(player, currentTime);
#else
        libvlc_media_player_set_time(player, currentTime, false);
#endif

        currentTime = 0;
    }

    if (backend == NULL) return;

    QCoreApplication::postEvent(backend,
                                new QEvent(static_cast<QEvent::Type> (WVlcPlayer::EventPlaying)));

    // NOTE: When the playback started the media tracks should be retrievable.

#if LIBVLC_VERSION_MAJOR < 4
    libvlc_media_track_t ** tracks;

    int count = libvlc_media_tracks_get(libvlc_media_player_get_media(player), &tracks);

    if (count == 0) return;

    QList<WBackendTrack> backendTracks;

    trackId = 0;

    int trackVideo = -1;
    int trackAudio = -1;

    int idVideo = libvlc_video_get_track(player);
    int idAudio = libvlc_audio_get_track(player);

    for (int i = 0; i < count; i++)
    {
        libvlc_media_track_t * track = tracks[i];

        libvlc_track_type_t type = track->i_type;

        if (type == libvlc_track_video)
        {
            int id = track->i_id;

            if (id == idVideo)
            {
                trackId    = id;
                trackVideo = id;
            }

            backendTracks.append(WBackendTrack(id, WAbstractBackend::TrackVideo,
                                               track->psz_language));
        }
        else if (type == libvlc_track_audio)
        {
            int id = track->i_id;

            if (id == idAudio) trackAudio = id;

            backendTracks.append(WBackendTrack(id, WAbstractBackend::TrackAudio,
                                               track->psz_language));
        }
    }

    libvlc_media_tracks_release(tracks, count);
#else
    libvlc_media_t * media = libvlc_media_player_get_media(player);

    libvlc_media_tracklist_t * videos = libvlc_media_get_tracklist(media, libvlc_track_video);
    libvlc_media_tracklist_t * audios = libvlc_media_get_tracklist(media, libvlc_track_audio);

    int countVideo = libvlc_media_tracklist_count(videos);
    int countAudio = libvlc_media_tracklist_count(audios);

    if (countVideo + countAudio == 0)
    {
        libvlc_media_tracklist_delete(videos);
        libvlc_media_tracklist_delete(audios);

        return;
    }

    QList<WBackendTrack> backendTracks;

    trackId = 0;

    int trackVideo = -1;
    int trackAudio = -1;

    for (int i = 0; i < countVideo; i++)
    {
        libvlc_media_track_t * track = libvlc_media_tracklist_at(videos, i);

        int id = track->i_id;

        if (track->selected)
        {
            trackId    = id;
            trackVideo = id;
        }

        backendTracks.append(WBackendTrack(id, WAbstractBackend::TrackVideo, track->psz_language));
    }

    for (int i = 0; i < countAudio; i++)
    {
        libvlc_media_track_t * track = libvlc_media_tracklist_at(audios, i);

        int id = track->i_id;

        if (track->selected) trackAudio = id;

        backendTracks.append(WBackendTrack(id, WAbstractBackend::TrackAudio, track->psz_language));
    }

    libvlc_media_tracklist_delete(videos);
    libvlc_media_tracklist_delete(audios);
#endif

    WVlcTracksEvent * eventTracks = new WVlcTracksEvent(backendTracks, trackVideo, trackAudio);

    QCoreApplication::postEvent(backend, eventTracks);
}

#if LIBVLC_VERSION_MAJOR > 3

libvlc_media_track_t * WVlcPlayerPrivate::getTrack(int id, libvlc_track_type_t type) const
{
    libvlc_media_t * media = libvlc_media_player_get_media(player);

    libvlc_media_tracklist_t * tracks = libvlc_media_get_tracklist(media, type);

    int count = libvlc_media_tracklist_count(tracks);

    for (int i = 0; i < count; i++)
    {
        libvlc_media_track_t * track = libvlc_media_tracklist_at(tracks, i);

        if (track->i_id != id) continue;

        libvlc_media_tracklist_delete(tracks);

        return track;
    }

    libvlc_media_tracklist_delete(tracks);

    return NULL;
}

#endif

//-------------------------------------------------------------------------------------------------
// Private static events
//-------------------------------------------------------------------------------------------------

/* static */ void WVlcPlayerPrivate::onPlaying(const struct libvlc_event_t *, void * data)
{
    WVlcPlayerPrivate * d = static_cast<WVlcPlayerPrivate *> (data);

    d->applyPlay();
}

/* static */ void WVlcPlayerPrivate::onPaused(const struct libvlc_event_t *, void * data)
{
    WVlcPlayerPrivate * d = static_cast<WVlcPlayerPrivate *> (data);

    d->playing = false;

    if (d->hasAudio) d->audio->pause();

    if (d->backend == NULL) return;

    QCoreApplication::postEvent(d->backend,
                                new QEvent(static_cast<QEvent::Type> (WVlcPlayer::EventPaused)));
}

/* static */ void WVlcPlayerPrivate::onStopped(const struct libvlc_event_t *, void * data)
{
    WVlcPlayerPrivate * d = static_cast<WVlcPlayerPrivate *> (data);

#if LIBVLC_VERSION_MAJOR > 3
    if (d->playing)
    {
        d->playing = false;

        if (d->hasAudio) d->audio->stop();

        if (d->backend == NULL) return;

        QCoreApplication::postEvent(d->backend, new QEvent(static_cast<QEvent::Type>
                                                           (WVlcPlayer::EventEndReached)));

        return;
    }
#else
    d->playing = false;
#endif

    if (d->hasAudio) d->audio->stop();

    if (d->backend == NULL) return;

    QCoreApplication::postEvent(d->backend,
                                new QEvent(static_cast<QEvent::Type> (WVlcPlayer::EventStopped)));
}

//-------------------------------------------------------------------------------------------------

/* static */ void WVlcPlayerPrivate::onBuffering(const struct libvlc_event_t *, void * data)
{
    WVlcPlayerPrivate * d = static_cast<WVlcPlayerPrivate *> (data);

    if (d->hasAudio) d->audio->applyBuffering();

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

#if LIBVLC_VERSION_MAJOR > 3
    // FIXME VLC 4.0.0: Sometimes the playing event is not called.
    if (d->playing == false)
    {
        d->applyPlay();
    }
#endif

    if (d->hasAudio)
    {
        int time = event->u.media_player_time_changed.new_time;

        d->audio->applyDelay(time);

        if (d->backend == NULL) return;

        QCoreApplication::postEvent(d->backend,
                                    new WVlcPlayerEvent(WVlcPlayer::EventTimeChanged, time));
    }

    if (d->backend == NULL) return;

    int time = event->u.media_player_time_changed.new_time;

    QCoreApplication::postEvent(d->backend,
                                new WVlcPlayerEvent(WVlcPlayer::EventTimeChanged, time));
}

//-------------------------------------------------------------------------------------------------

#if LIBVLC_VERSION_MAJOR < 4

/* static */ void WVlcPlayerPrivate::onEndReached(const struct libvlc_event_t *, void * data)
{
    WVlcPlayerPrivate * d = static_cast<WVlcPlayerPrivate *> (data);

    if (d->backend == NULL) return;

    QCoreApplication::postEvent(d->backend, new QEvent(static_cast<QEvent::Type>
                                                       (WVlcPlayer::EventEndReached)));
}

#endif

//-------------------------------------------------------------------------------------------------

/* static */ void WVlcPlayerPrivate::onEncounteredError(const struct libvlc_event_t *, void * data)
{
    WVlcPlayerPrivate * d = static_cast<WVlcPlayerPrivate *> (data);

    if (d->backend == NULL) return;

    QCoreApplication::postEvent(d->backend,
                                new QEvent(static_cast<QEvent::Type> (WVlcPlayer::EventError)));
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WVlcPlayerPrivate::onOutputAdded(const WBackendOutput & output)
{
    QList<WBackendOutput> outputs;

    outputs.append(output);

    QCoreApplication::postEvent(backend, new WVlcOutputsEvent(outputs));
}

void WVlcPlayerPrivate::onOutputRemoved(int index)
{
    QCoreApplication::postEvent(backend,
                                new WVlcPlayerEvent(WVlcPlayer::EventOutputRemove, index));
}

void WVlcPlayerPrivate::onOutputCleared()
{
    QCoreApplication::postEvent(backend, new QEvent(static_cast<QEvent::Type>
                                                    (WVlcPlayer::EventOutputClear)));
}

//=================================================================================================
// WVlcPlayer
//=================================================================================================

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

/* Q_INVOKABLE */ void WVlcPlayer::setSource(const QString & media,
                                             const QString & audio, int loop)
{
    QCoreApplication::postEvent(this, new WVlcPlayerEventSource(media, audio, loop));
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

/* Q_INVOKABLE */ void WVlcPlayer::setVideo(int id)
{
    QCoreApplication::postEvent(this, new WVlcPlayerPrivateEvent(WVlcPlayerPrivate::EventVideo,
                                                                 id));
}

/* Q_INVOKABLE */ void WVlcPlayer::setAudio(int id)
{
    QCoreApplication::postEvent(this, new WVlcPlayerPrivateEvent(WVlcPlayerPrivate::EventAudio,
                                                                 id));
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WVlcPlayer::setScanOutput(bool enabled)
{
    QCoreApplication::postEvent(this, new WVlcPlayerPrivateEvent(WVlcPlayerPrivate::EventScan,
                                                                 enabled));
}

/* Q_INVOKABLE */ void WVlcPlayer::setOutput(int index)
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

#if LIBVLC_VERSION_MAJOR < 4
        libvlc_event_attach(manager, libvlc_MediaPlayerEndReached, d->onEndReached, d);
#endif

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
            d->hasAudio = false;

            const QString & audio = eventSource->audio;

            if (audio.isEmpty())
            {
                 media = d->createMedia(eventSource->media);
            }
            else media = d->createMedia(audio);

            // FIXME VLC 3.0.18: This option seems to fail the second time we call it.
            libvlc_media_add_option(media, "no-video");
        }
        else
        {
            media = d->createMedia(eventSource->media);

            if (output == WAbstractBackend::OutputVideo)
            {
                d->hasAudio = false;

                libvlc_media_add_option(media, "no-audio");
            }
            else
            {
                const QString & audio = eventSource->audio;

                if (audio.isEmpty() == false)
                {
                    // NOTE VLC: When using an external renderer we have to stick with input-slave
                    //           because it does not support multiple players.
                    if (d->hasOutput == false)
                    {
                        d->hasAudio = true;

                        if (d->audio == NULL)
                        {
                            d->audio = new WVlcPlayerPrivateAudio(d,
                                                                  d->engine->d_func()->instance);
                        }

                        libvlc_media_t * mediaAudio = d->createMedia(audio);

                        libvlc_media_add_option(media,      "no-audio");
                        libvlc_media_add_option(mediaAudio, "no-video");

                        if (eventSource->loop)
                        {
                            const char * string = "input-repeat=65535";

                            // NOTE: We use the maximum value given we can't set an infinite value.
                            libvlc_media_add_option(media,      string);
                            libvlc_media_add_option(mediaAudio, string);
                        }

                        if (cache.isNull() == false)
                        {
                            const char * string = cache.C_STR;

                            libvlc_media_add_option(media,      string);
                            libvlc_media_add_option(mediaAudio, string);
                        }

                        if (proxy.isNull() == false)
                        {
                            const char * stringA = proxy        .C_STR;
                            const char * stringB = proxyPassword.C_STR;

                            libvlc_media_add_option(media, stringA);
                            libvlc_media_add_option(media, stringB);

                            libvlc_media_add_option(mediaAudio, stringA);
                            libvlc_media_add_option(mediaAudio, stringB);
                        }

                        foreach (const QString & option, d->options)
                        {
                            const char * string = option.C_STR;

                            libvlc_media_add_option(media,      string);
                            libvlc_media_add_option(mediaAudio, string);
                        }

                        libvlc_media_player_set_media(d->player,        media);
                        libvlc_media_player_set_media(d->audio->player, mediaAudio);

                        return true;
                    }

                    d->hasAudio = false;

                    QString input = "input-slave=" + d->encodeUrl(audio);

                    libvlc_media_add_option(media, input.C_UTF);

                    // FIXME VLC 3.0.18: This seems to fix the missing audio bytes at the end of
                    //                   the video.
                    libvlc_media_add_option(media, "demux=avformat");
                }
                else d->hasAudio = false;
            }

            if (d->quality.isEmpty() == false)
            {
                libvlc_media_add_option(media,
                                        QString("preferred-resolution=" + d->quality).C_STR);
            }
        }

        if (eventSource->loop)
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

#if LIBVLC_VERSION_MAJOR < 4
        if (at) libvlc_media_player_set_time(d->player, at);
#else
        if (at) libvlc_media_player_set_time(d->player, at, false);
#endif

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcPlayerPrivate::EventPause))
    {
        libvlc_media_player_set_pause(d->player, true);

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcPlayerPrivate::EventStop))
    {
        d->stop();

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcPlayerPrivate::EventSeek))
    {
        WVlcPlayerPrivateEvent * eventPlayer = static_cast<WVlcPlayerPrivateEvent *> (event);

#if LIBVLC_VERSION_MAJOR < 4
        libvlc_media_player_set_time(d->player, eventPlayer->value.toInt());
#else
        libvlc_media_player_set_time(d->player, eventPlayer->value.toInt(), false);
#endif

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
    else if (type == static_cast<QEvent::Type> (WVlcPlayerPrivate::EventVideo))
    {
        WVlcPlayerPrivateEvent * eventPlayer = static_cast<WVlcPlayerPrivateEvent *> (event);

#if LIBVLC_VERSION_MAJOR < 4
        libvlc_video_set_track(d->player, eventPlayer->value.toInt());
#else
        libvlc_media_track_t * track = d->getTrack(eventPlayer->value.toInt(), libvlc_track_video);

        if (track == NULL) return true;

        libvlc_media_player_select_track(d->player, track);
#endif

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcPlayerPrivate::EventAudio))
    {
        WVlcPlayerPrivateEvent * eventPlayer = static_cast<WVlcPlayerPrivateEvent *> (event);

#if LIBVLC_VERSION_MAJOR < 4
        libvlc_audio_set_track(d->player, eventPlayer->value.toInt());
#else
        libvlc_media_track_t * track = d->getTrack(eventPlayer->value.toInt(), libvlc_track_audio);

        if (track == NULL) return true;

        libvlc_media_player_select_track(d->player, track);
#endif

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcPlayerPrivate::EventScan))
    {
        WVlcPlayerPrivateEvent * eventPlayer = static_cast<WVlcPlayerPrivateEvent *> (event);

        bool scan = eventPlayer->value.toBool();

        d->setScanOutput(scan);

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcPlayerPrivate::EventOutput))
    {
        WVlcPlayerPrivateEvent * eventPlayer = static_cast<WVlcPlayerPrivateEvent *> (event);

        int index = eventPlayer->value.toInt();

        const QList<libvlc_renderer_item_t *> & renderers = d->engine->d_func()->renderers;

        if (index < 0 || index >= renderers.count())
        {
            d->hasOutput = false;

            libvlc_media_player_set_renderer(d->player, NULL);
        }
        else
        {
            d->hasOutput = true;

            libvlc_media_player_set_renderer(d->player, renderers.at(index));
        }

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcPlayerPrivate::EventDelete))
    {
        d->stop();

        libvlc_media_player_release(d->player);

        if (d->audio)
        {
            d->hasAudio = false;

            delete d->audio;

            d->audio = NULL;
        }

        d->player = NULL;

        d->setScanOutput(false);

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

QString WVlcPlayer::quality()
{
    Q_D(WVlcPlayer);

    const QMutexLocker locker(&d->mutex);

    return d->quality;
}

void WVlcPlayer::setQuality(const QString & quality)
{
    Q_D(WVlcPlayer);

    QMutexLocker locker(&d->mutex);

    if (d->quality == quality) return;

    d->quality = quality;

    locker.unlock();

    emit qualityChanged();
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
