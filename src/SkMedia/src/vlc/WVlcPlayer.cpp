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

// Sk includes
#ifdef VLCPLAYER_AUDIO
#include <WControllerMedia>
#include <WVlcAudio>
#endif
#include <WVlcEngine>

// Private includes
#include <private/WVlcEngine_p>

//-------------------------------------------------------------------------------------------------
// Static variables

static const int PLAYER_RETRY_GAP = 1000; // 1 second

static const int PLAYER_RETRY_COUNT = 1;

//=================================================================================================
// WVlcPlayerPrivate
//=================================================================================================

WVlcPlayerPrivate::WVlcPlayerPrivate(WVlcPlayer * p) : WPrivate(p) {}

void WVlcPlayerPrivate::init(WVlcEngine * engine, QThread * thread)
{
    Q_Q(WVlcPlayer);

    this->thread = thread;
    this->engine = engine;

    player = NULL;

#ifdef VLCPLAYER_AUDIO
    playerAudio = NULL;
#endif

    backend = NULL;

#ifdef VLCPLAYER_AUDIO
    hasAudio  = false;
#endif
    hasOutput = false;

#if LIBVLC_VERSION_MAJOR > 3
    opening = false;
#endif
    playing = false;

#ifdef VLCPLAYER_AUDIO
    wait = false;

    playLater = false;
#endif

    retry = 0;

    trackId = -1;

#if LIBVLC_VERSION_MAJOR > 3
    trackWidth  = -1;
    trackHeight = -1;

    currentTime = 0;
#endif

    output  = WAbstractBackend::OutputMedia;
    quality = "";

    scanOutput = false;

    // FIXME: Should we set this to 1000 by default like VLC ?
    networkCache = -1;

    adjustReady = false;

    if (thread == NULL)
    {
        create();

        return;
    }

    q->moveToThread(thread);

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

void WVlcPlayerPrivate::create()
{
    player = libvlc_media_player_new(engine->d_func()->instance);

    // FIXME: Applying the player default volume.
    //libvlc_audio_set_volume(player, 100);

    libvlc_event_manager_t * manager = libvlc_media_player_event_manager(player);

#if LIBVLC_VERSION_MAJOR > 3
    libvlc_event_attach(manager, libvlc_MediaPlayerMediaChanged, onChanged, this);
    libvlc_event_attach(manager, libvlc_MediaPlayerOpening,      onOpening, this);
#endif

    libvlc_event_attach(manager, libvlc_MediaPlayerPlaying, onPlaying, this);
    libvlc_event_attach(manager, libvlc_MediaPlayerPaused,  onPaused,  this);
    libvlc_event_attach(manager, libvlc_MediaPlayerStopped, onStopped, this);

    libvlc_event_attach(manager, libvlc_MediaPlayerBuffering, onBuffering, this);

    libvlc_event_attach(manager, libvlc_MediaPlayerLengthChanged, onLength, this);
    libvlc_event_attach(manager, libvlc_MediaPlayerTimeChanged,   onTime,   this);

#if LIBVLC_VERSION_MAJOR < 4
    libvlc_event_attach(manager, libvlc_MediaPlayerEndReached, onEndReached, this);
#endif

    libvlc_event_attach(manager, libvlc_MediaPlayerEncounteredError, onEncounteredError, this);
}

void WVlcPlayerPrivate::setBackend(WAbstractBackend      * backend,
                                   libvlc_video_format_cb  setup,
                                   libvlc_video_cleanup_cb cleanup,
                                   libvlc_video_lock_cb    lock,
                                   libvlc_video_unlock_cb  unlock,
                                   libvlc_video_display_cb display)
{
    if (this->backend)
    {
        qWarning("WVlcPlayer::event: Player already has a backend.");

        return;
    }

    Q_ASSERT(backend);

    this->backend = backend;

    libvlc_video_set_format_callbacks(player, setup, cleanup);

    libvlc_video_set_callbacks(player, lock, unlock, display, backend);
}

void WVlcPlayerPrivate::setSource(const QString & url,
                                  const QString & audio, const QStringList & options, int loop)
{
    WAbstractBackend::Output output;

    QString cache;

    QString proxy;
    QString proxyPassword;

    QStringList preferences;

    mutex.lock();

    output = this->output;

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

    preferences = this->options;

    mutex.unlock();

    libvlc_media_t * media;

    if (output == WAbstractBackend::OutputAudio)
    {
#ifdef VLCPLAYER_AUDIO
        if (hasAudio)
        {
            hasAudio = false;

            playerAudio->stop();
        }
#endif

        if (audio.isEmpty())
        {
             media = createMedia(url);
        }
        else media = createMedia(audio);

        // FIXME VLC 3.0.18: This option seems to fail the second time we call it.
        libvlc_media_add_option(media, "no-video");
    }
    else
    {
        media = createMedia(url);

        if (output == WAbstractBackend::OutputVideo)
        {
#ifdef VLCPLAYER_AUDIO
            if (hasAudio)
            {
                hasAudio = false;

                playerAudio->stop();
            }
#endif

            libvlc_media_add_option(media, "no-audio");
        }
        else
        {
            if (audio.isEmpty() == false)
            {
#ifdef VLCPLAYER_AUDIO
                // NOTE VLC: When using an external renderer we have to stick with input-slave
                //           because it does not support multiple players.
                if (hasOutput == false)
                {
                    if (playerAudio == NULL)
                    {
                        Q_Q(WVlcPlayer);

                        // NOTE: We create this instance on the VLC threads.
                        playerAudio = new WVlcAudio(engine);

                        playerAudio->setSpeed(libvlc_media_player_get_rate(player));

                        playerAudio->setVolume(libvlc_audio_get_volume(player));

                        mutex.lock();

                        playerAudio->setProxy(proxyHost, proxyPassword);

                        playerAudio->setOptions(preferences);

                        playerAudio->setNetworkCache(networkCache);

                        mutex.unlock();

                        QObject::connect(playerAudio, SIGNAL(waitingChanged(bool)),
                                         q,           SLOT(onWaitingChanged(bool)));
                    }

                    hasAudio = true;

                    playerAudio->setSource(audio, options, loop);
                }
                else
                {
                    if (hasAudio)
                    {
                        hasAudio = false;

                        playerAudio->stop();
                    }
#endif

                    QString input = "input-slave=" + encodeUrl(audio);

                    libvlc_media_add_option(media, input.C_UTF);
#ifdef VLCPLAYER_AUDIO
                }
#endif
            }
#ifdef VLCPLAYER_AUDIO
            else if (hasAudio)
            {
                hasAudio = false;

                playerAudio->stop();
            }
#endif
        }

        if (quality.isEmpty() == false)
        {
            libvlc_media_add_option(media, QString("preferred-resolution=" + quality).C_STR);
        }
    }

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

    foreach (const QString & option, preferences)
    {
        libvlc_media_add_option(media, option.C_STR);
    }

#if LIBVLC_VERSION_MAJOR > 3
    opening = true;
#endif
    playing = false;

    retry = 0;

    trackId = -1;

#if LIBVLC_VERSION_MAJOR > 3
    trackWidth  = -1;
    trackHeight = -1;
#endif

    libvlc_media_player_set_media(player, media);
}

void WVlcPlayerPrivate::play(int time)
{
#if LIBVLC_VERSION_MAJOR > 3
    if (opening)
    {
        currentTime = time;

        return;
    }
#endif

    if (playing)
    {
        if (time) seek(time);

        return;
    }

#if LIBVLC_VERSION_MAJOR > 3
    currentTime = time;
#endif

    libvlc_media_player_play(player);

#if LIBVLC_VERSION_MAJOR < 4
    if (time) libvlc_media_player_set_time(player, time);
#endif

#ifdef VLCPLAYER_AUDIO
    if (hasAudio) playerAudio->play(time);
#endif
}

void WVlcPlayerPrivate::pause()
{
    if (playing == false) return;

    playing = false;

    libvlc_media_player_set_pause(player, 1);

#ifdef VLCPLAYER_AUDIO
    if (hasAudio) playerAudio->pause();
#endif
}

void WVlcPlayerPrivate::stop()
{
#if LIBVLC_VERSION_MAJOR > 3
    opening = false;
#endif
    playing = false;

#if LIBVLC_VERSION_MAJOR < 4
    libvlc_media_player_stop(player);
#else
    currentTime = 0;

    libvlc_media_player_set_pause(player, 1);

    libvlc_media_player_set_time(player, 0, false);
#endif

#ifdef VLCPLAYER_AUDIO
    if (hasAudio) playerAudio->stop();
#endif
}

void WVlcPlayerPrivate::seek(int msec)
{
    retry = 0;

#if LIBVLC_VERSION_MAJOR < 4
    libvlc_media_player_set_time(player, msec);
#else
    libvlc_media_player_set_time(player, msec, false);
#endif

#ifdef VLCPLAYER_AUDIO
    if (hasAudio) playerAudio->seek(msec);
#endif
}

void WVlcPlayerPrivate::setSpeed(float speed)
{
    libvlc_media_player_set_rate(player, speed);

#ifdef VLCPLAYER_AUDIO
    if (hasAudio) playerAudio->setSpeed(speed);
#endif
}

void WVlcPlayerPrivate::setVolume(int volume)
{
    libvlc_audio_set_volume(player, volume);

#ifdef VLCPLAYER_AUDIO
    if (hasAudio) playerAudio->setVolume(volume);
#endif
}

void WVlcPlayerPrivate::setVideo(int id)
{
#if LIBVLC_VERSION_MAJOR < 4
    libvlc_video_set_track(player, id);
#else
    libvlc_media_track_t * track = getTrack(id, libvlc_track_video);

    if (track == NULL) return;

    libvlc_media_player_select_track(player, track);
#endif
}

void WVlcPlayerPrivate::setAudio(int id)
{
#if LIBVLC_VERSION_MAJOR < 4
    libvlc_audio_set_track(player, id);
#else
    libvlc_media_track_t * track = getTrack(id, libvlc_track_audio);

    if (track == NULL) return;

    libvlc_media_player_select_track(player, track);
#endif
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

void WVlcPlayerPrivate::setOutput(int index)
{
    const QList<libvlc_renderer_item_t *> & renderers = engine->d_func()->renderers;

    if (index < 0 || index >= renderers.count())
    {
        hasOutput = false;

        libvlc_media_player_set_renderer(player, NULL);
    }
    else
    {
        hasOutput = true;

        libvlc_media_player_set_renderer(player, renderers.at(index));
    }
}

void WVlcPlayerPrivate::setAdjust(const WBackendAdjust & adjust)
{
    if (this->adjust == adjust) return;

    this->adjust = adjust;

    // NOTE VLC: Adjustments only works when the player is genuinely playing (onTime event).
    adjustReady = true;
}

void WVlcPlayerPrivate::deletePlayer()
{
#ifdef VLCPLAYER_AUDIO
    if (playerAudio)
    {
        Q_Q(WVlcPlayer);

        QObject::disconnect(playerAudio, 0, q, 0);

        // NOTE: We stop after disconnecting the playerAudio to skip the waitingChanged signal.
        stop();

        hasAudio = false;

        playerAudio->deletePlayer();

        delete playerAudio;

        playerAudio = NULL;
    }
    else stop();
#else
    stop();
#endif

    libvlc_media_player_release(player);

    player = NULL;

    setScanOutput(false);

    if (backend)
    {
        QCoreApplication::postEvent(backend, new QEvent(static_cast<QEvent::Type>
                                                           (WVlcPlayer::EventDelete)),
                                    Qt::HighEventPriority * 100);
    }
}

#if LIBVLC_VERSION_MAJOR > 3

void WVlcPlayerPrivate::applyOpen()
{
    opening = false;

    if (currentTime == 0) return;

    libvlc_media_player_set_time(player, currentTime, false);

    currentTime = 0;
}

#endif

void WVlcPlayerPrivate::applyPlay()
{
    playing = true;

#ifdef VLCPLAYER_AUDIO
    playLater = false;
#endif

    if (backend == NULL) return;

    QCoreApplication::postEvent(backend,
                                new QEvent(static_cast<QEvent::Type> (WVlcPlayer::EventPlaying)));

    // NOTE: When the playback started the media tracks should be retrievable.

#if LIBVLC_VERSION_MAJOR < 4
    libvlc_media_track_t ** tracks;

    int count = libvlc_media_tracks_get(libvlc_media_player_get_media(player), &tracks);

    if (count == 0) return;

    QList<WBackendTrack> backendTracks;

    trackId = -1;

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

    trackId = -1;

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

            libvlc_video_track_t * video = track->video;

            trackWidth  = video->i_width;
            trackHeight = video->i_height;
        }

        backendTracks.append(WBackendTrack(id, WAbstractBackend::TrackVideo, track->psz_language));
    }

    if (trackId == -1 && countVideo)
    {
        applyTrackDefault(libvlc_media_tracklist_at(videos, 0));
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

void WVlcPlayerPrivate::applyEnd()
{
    if (backend)
    {
        if (checkTime(backend->currentTime(), backend->duration())) return;

        QCoreApplication::postEvent(backend, new QEvent(static_cast<QEvent::Type>
                                                        (WVlcPlayer::EventEndReached)));
    }
    else if (checkTime(libvlc_media_player_get_time  (player),
                       libvlc_media_player_get_length(player))) return;
}

void WVlcPlayerPrivate::applySize(unsigned int * width, unsigned int * height)
{
#if LIBVLC_VERSION_MAJOR < 4
    if (trackId == -1)
    {
        libvlc_video_get_size(player, 0, width, height);
    }
    else libvlc_video_get_size(player, trackId, width, height);
#else
    if (trackId == -1)
    {
        libvlc_media_t * media = libvlc_media_player_get_media(player);

        libvlc_media_tracklist_t * videos = libvlc_media_get_tracklist(media, libvlc_track_video);

        int countVideo = libvlc_media_tracklist_count(videos);

        if (countVideo)
        {
            applyTrackDefault(libvlc_media_tracklist_at(videos, 0));

            *width  = trackWidth;
            *height = trackHeight;
        }

        libvlc_media_tracklist_delete(videos);
    }
    else
    {
        *width  = trackWidth;
        *height = trackHeight;
    }
#endif
}

#if LIBVLC_VERSION_MAJOR > 3

void WVlcPlayerPrivate::applyTrackDefault(libvlc_media_track_t * track)
{
    trackId = track->i_id;

    libvlc_video_track_t * video = track->video;

    trackWidth  = video->i_width;
    trackHeight = video->i_height;
}

#endif

void WVlcPlayerPrivate::applyAdjust()
{
    bool enable = adjust.enable;

    libvlc_video_set_adjust_int(player, libvlc_adjust_Enable, enable);

    if (enable == false) return;

    libvlc_video_set_adjust_float(player, libvlc_adjust_Contrast,   adjust.contrast);
    libvlc_video_set_adjust_float(player, libvlc_adjust_Brightness, adjust.brightness);
    libvlc_video_set_adjust_float(player, libvlc_adjust_Hue,        adjust.hue);
    libvlc_video_set_adjust_float(player, libvlc_adjust_Saturation, adjust.saturation);
    libvlc_video_set_adjust_float(player, libvlc_adjust_Gamma,      adjust.gamma);
}

bool WVlcPlayerPrivate::checkTime(int at, int duration)
{
    // FIXME VLC 3.0.21: Sometimes the seeking fails, so we try try again. The position has to be
    //                   valid otherwise it might have been set to zero already in the onTime
    //                   callback after reaching the end.
    //                   We don't do it when playing on a remote output because of latency.
    if (hasOutput || at == 0 || duration < 1
        ||
        (duration - at < PLAYER_RETRY_GAP || retry >= PLAYER_RETRY_COUNT)) return false;

    Q_Q(WVlcPlayer);

    qDebug("PLAYER RETRY");

    retry++;

    QCoreApplication::postEvent(q, new QEvent(static_cast<QEvent::Type>
                                              (WVlcPlayerPrivate::EventStop)));

    QCoreApplication::postEvent(q, new WVlcPlayerPrivateEvent(WVlcPlayerPrivate::EventPlay, at));

    return true;
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

#if LIBVLC_VERSION_MAJOR > 3

/* static */ void WVlcPlayerPrivate::onChanged(const struct libvlc_event_t *, void * data)
{
    WVlcPlayerPrivate * d = static_cast<WVlcPlayerPrivate *> (data);

    d->applyOpen();
}

/* static */ void WVlcPlayerPrivate::onOpening(const struct libvlc_event_t *, void * data)
{
    WVlcPlayerPrivate * d = static_cast<WVlcPlayerPrivate *> (data);

    d->applyOpen();
}

#endif

/* static */ void WVlcPlayerPrivate::onPlaying(const struct libvlc_event_t *, void * data)
{
    WVlcPlayerPrivate * d = static_cast<WVlcPlayerPrivate *> (data);

    d->applyPlay();
}

/* static */ void WVlcPlayerPrivate::onPaused(const struct libvlc_event_t *, void * data)
{
    WVlcPlayerPrivate * d = static_cast<WVlcPlayerPrivate *> (data);

#ifdef VLCPLAYER_AUDIO
    if (d->hasAudio)
    {
        if (d->playLater)
        {
            d->playLater = false;

            if (d->playing)
            {
                libvlc_media_player_play(d->player);

                return;
            }
        }

        d->playing = false;

        // NOTE: We avoid sending EventPaused when we're waiting for the audio.
        if (d->wait) return;

        d->playerAudio->pause();
    }
    else d->playing = false;
#else
    d->playing = false;
#endif

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

        d->applyEnd();

        return;
    }
#endif

    d->playing = false;

#ifdef VLCPLAYER_AUDIO
    if (d->hasAudio) d->playerAudio->stop();
#endif

    if (d->backend == NULL) return;

    QCoreApplication::postEvent(d->backend,
                                new QEvent(static_cast<QEvent::Type> (WVlcPlayer::EventStopped)));
}

//-------------------------------------------------------------------------------------------------

/* static */ void WVlcPlayerPrivate::onBuffering(const struct libvlc_event_t * event, void * data)
{
    WVlcPlayerPrivate * d = static_cast<WVlcPlayerPrivate *> (data);

#ifdef VLCPLAYER_AUDIO
    float progress = event->u.media_player_buffering.new_cache;

    if (d->hasAudio) d->playerAudio->applyBuffering(progress);

    if (d->backend == NULL) return;

    QCoreApplication::postEvent(d->backend,
                                new WVlcPlayerEvent(WVlcPlayer::EventBuffering, progress));
#else
    if (d->backend == NULL) return;

    float progress = event->u.media_player_buffering.new_cache;

    QCoreApplication::postEvent(d->backend,
                                new WVlcPlayerEvent(WVlcPlayer::EventBuffering, progress));
#endif
}

//-------------------------------------------------------------------------------------------------

/* static */ void WVlcPlayerPrivate::onLength(const struct libvlc_event_t * event, void * data)
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

/* static */ void WVlcPlayerPrivate::onTime(const struct libvlc_event_t * event, void * data)
{
    WVlcPlayerPrivate * d = static_cast<WVlcPlayerPrivate *> (data);

#if LIBVLC_VERSION_MAJOR > 3
    // FIXME VLC 4.0.0: Sometimes the playing event is not called.
    if (d->playing == false && libvlc_media_player_get_state(d->player) == libvlc_Playing)
    {
        d->applyPlay();
    }
#endif

    if (d->adjustReady)
    {
        d->adjustReady = false;

        d->applyAdjust();
    }

#ifdef VLCPLAYER_AUDIO
    if (d->hasAudio)
    {
        int time = event->u.media_player_time_changed.new_time;

        if (d->playing) d->playerAudio->synchronize(time);

        if (d->backend == NULL) return;

        QCoreApplication::postEvent(d->backend,
                                    new WVlcPlayerEvent(WVlcPlayer::EventTimeChanged, time));

        return;
    }
#endif

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

    d->applyEnd();
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

#ifdef VLCPLAYER_AUDIO

void WVlcPlayerPrivate::onWaitingChanged(bool waiting)
{
    if (wait == waiting) return;

    wait = waiting;

    if (wait)
    {
        playLater = false;

        if (libvlc_media_player_get_state(player) != libvlc_Playing) return;

        libvlc_media_player_set_pause(player, 1);

        return;
    }

    // FIXME VLC 4.0.0: set_pause(player, 1) and player_play(player) are asynchronous, so we use
    //                  a playLater boolean to ensure that we don't get stuck in pause mode.

    playLater = true;

    if (libvlc_media_player_get_state(player) == libvlc_Playing) return;

    libvlc_media_player_play(player);
}

#endif

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

/* Q_INVOKABLE */ void WVlcPlayer::setBackend(WAbstractBackend      * backend,
                                              libvlc_video_format_cb  setup,
                                              libvlc_video_cleanup_cb cleanup,
                                              libvlc_video_lock_cb    lock,
                                              libvlc_video_unlock_cb  unlock,
                                              libvlc_video_display_cb display)
{
    Q_D(WVlcPlayer);

    if (d->thread == NULL)
    {
        d->setBackend(backend, setup, cleanup, lock, unlock, display);

        return;
    }

    QCoreApplication::postEvent(this, new WVlcPlayerEventBackend(backend, setup, cleanup, lock,
                                                                 unlock, display));
}

/* Q_INVOKABLE */ void WVlcPlayer::setSource(const QString     & url,
                                             const QString     & audio,
                                             const QStringList & options, int loop)
{
    Q_D(WVlcPlayer);

    if (d->thread == NULL)
    {
        d->setSource(url, audio, options, loop);

        return;
    }

    QCoreApplication::postEvent(this, new WVlcPlayerEventSource(url, audio, options, loop));
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WVlcPlayer::play(int at)
{
    Q_D(WVlcPlayer);

    if (d->thread == NULL)
    {
        d->play(at);

        return;
    }

    QCoreApplication::postEvent(this,
                                new WVlcPlayerPrivateEvent(WVlcPlayerPrivate::EventPlay, at));
}

/* Q_INVOKABLE */ void WVlcPlayer::pause()
{
    Q_D(WVlcPlayer);

    if (d->thread == NULL)
    {
        d->pause();

        return;
    }

    QCoreApplication::postEvent(this, new QEvent(static_cast<QEvent::Type>
                                                 (WVlcPlayerPrivate::EventPause)));
}

/* Q_INVOKABLE */ void WVlcPlayer::stop()
{
    Q_D(WVlcPlayer);

    if (d->thread == NULL)
    {
        d->stop();

        return;
    }

    QCoreApplication::postEvent(this, new QEvent(static_cast<QEvent::Type>
                                                 (WVlcPlayerPrivate::EventStop)));
}

/* Q_INVOKABLE */ void WVlcPlayer::seek(int msec)
{
    Q_D(WVlcPlayer);

    if (d->thread == NULL)
    {
        d->seek(msec);

        return;
    }

    QCoreApplication::postEvent(this, new WVlcPlayerPrivateEvent(WVlcPlayerPrivate::EventSeek,
                                                                 msec));
}

/* Q_INVOKABLE */ void WVlcPlayer::setSpeed(qreal speed)
{
    Q_D(WVlcPlayer);

    if (d->thread == NULL)
    {
        d->setSpeed(speed);

        return;
    }

    QCoreApplication::postEvent(this, new WVlcPlayerPrivateEvent(WVlcPlayerPrivate::EventSpeed,
                                                                 speed));
}

/* Q_INVOKABLE */ void WVlcPlayer::setVolume(int percent)
{
    Q_D(WVlcPlayer);

    if (d->thread == NULL)
    {
        d->setVolume(percent);

        return;
    }

    QCoreApplication::postEvent(this, new WVlcPlayerPrivateEvent(WVlcPlayerPrivate::EventVolume,
                                                                 percent));
}

/* Q_INVOKABLE */ void WVlcPlayer::setVideo(int id)
{
    Q_D(WVlcPlayer);

    if (d->thread == NULL)
    {
        d->setVideo(id);

        return;
    }

    QCoreApplication::postEvent(this, new WVlcPlayerPrivateEvent(WVlcPlayerPrivate::EventVideo,
                                                                 id));
}

/* Q_INVOKABLE */ void WVlcPlayer::setAudio(int id)
{
    Q_D(WVlcPlayer);

    if (d->thread == NULL)
    {
        d->setAudio(id);

        return;
    }

    QCoreApplication::postEvent(this, new WVlcPlayerPrivateEvent(WVlcPlayerPrivate::EventAudio,
                                                                 id));
}

/* Q_INVOKABLE */ void WVlcPlayer::setScanOutput(bool enabled)
{
    Q_D(WVlcPlayer);

    if (d->thread == NULL)
    {
        d->setScanOutput(enabled);

        return;
    }

    QCoreApplication::postEvent(this, new WVlcPlayerPrivateEvent(WVlcPlayerPrivate::EventScan,
                                                                 enabled));
}

/* Q_INVOKABLE */ void WVlcPlayer::setOutput(int index)
{
    Q_D(WVlcPlayer);

    if (d->thread == NULL)
    {
        d->setOutput(index);

        return;
    }

    QCoreApplication::postEvent(this, new WVlcPlayerPrivateEvent(WVlcPlayerPrivate::EventOutput,
                                                                 index));
}

/* Q_INVOKABLE */ void WVlcPlayer::setAdjust(const WBackendAdjust & adjust)
{
    Q_D(WVlcPlayer);

    if (d->thread == NULL)
    {
        d->setAdjust(adjust);

        return;
    }

    QCoreApplication::postEvent(this, new WVlcPlayerEventAdjust(adjust));
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WVlcPlayer::setProxy(const QString & host, const QString & password)
{
    Q_D(WVlcPlayer);

    d->mutex.lock();

    d->proxyHost     = host;
    d->proxyPassword = password;

#ifdef VLCPLAYER_AUDIO
    if (d->hasAudio) d->playerAudio->setProxy(host, password);
#endif

    d->mutex.unlock();
}

/* Q_INVOKABLE */ void WVlcPlayer::clearProxy()
{
    Q_D(WVlcPlayer);

    d->mutex.lock();

    d->proxyHost     = QString();
    d->proxyPassword = QString();

#ifdef VLCPLAYER_AUDIO
    if (d->hasAudio) d->playerAudio->clearProxy();
#endif

    d->mutex.unlock();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WVlcPlayer::deletePlayer()
{
    Q_D(WVlcPlayer);

    if (d->thread == NULL)
    {
        d->deletePlayer();

        return;
    }

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
        d->create();

        return true;
    }
    else if (d->player == NULL)
    {
        return QObject::event(event);
    }
    else if (type == static_cast<QEvent::Type> (WVlcPlayerPrivate::EventBackend))
    {
        WVlcPlayerEventBackend * eventBackend = static_cast<WVlcPlayerEventBackend *> (event);

        d->setBackend(eventBackend->backend, eventBackend->setup,
                                             eventBackend->cleanup,
                                             eventBackend->lock,
                                             eventBackend->unlock,
                                             eventBackend->display);

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcPlayerPrivate::EventSource))
    {
        WVlcPlayerEventSource * eventSource = static_cast<WVlcPlayerEventSource *> (event);

        d->setSource(eventSource->url, eventSource->audio, eventSource->options,
                     eventSource->loop);

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcPlayerPrivate::EventPlay))
    {
        WVlcPlayerPrivateEvent * eventPlayer = static_cast<WVlcPlayerPrivateEvent *> (event);

        d->play(eventPlayer->value.toInt());

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcPlayerPrivate::EventPause))
    {
        d->pause();

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

        d->seek(eventPlayer->value.toInt());

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcPlayerPrivate::EventSpeed))
    {
        WVlcPlayerPrivateEvent * eventPlayer = static_cast<WVlcPlayerPrivateEvent *> (event);

        d->setSpeed(eventPlayer->value.toFloat());

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcPlayerPrivate::EventVolume))
    {
        WVlcPlayerPrivateEvent * eventPlayer = static_cast<WVlcPlayerPrivateEvent *> (event);

        d->setVolume(eventPlayer->value.toInt());

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcPlayerPrivate::EventVideo))
    {
        WVlcPlayerPrivateEvent * eventPlayer = static_cast<WVlcPlayerPrivateEvent *> (event);

        d->setVideo(eventPlayer->value.toInt());

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcPlayerPrivate::EventAudio))
    {
        WVlcPlayerPrivateEvent * eventPlayer = static_cast<WVlcPlayerPrivateEvent *> (event);

        d->setAudio(eventPlayer->value.toInt());

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcPlayerPrivate::EventScan))
    {
        WVlcPlayerPrivateEvent * eventPlayer = static_cast<WVlcPlayerPrivateEvent *> (event);

        d->setScanOutput(eventPlayer->value.toBool());

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcPlayerPrivate::EventOutput))
    {
        WVlcPlayerPrivateEvent * eventPlayer = static_cast<WVlcPlayerPrivateEvent *> (event);

        d->setOutput(eventPlayer->value.toInt());

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcPlayerPrivate::EventAdjust))
    {
        WVlcPlayerEventAdjust * eventAdjust = static_cast<WVlcPlayerEventAdjust *> (event);

        d->setAdjust(eventAdjust->adjust);

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcPlayerPrivate::EventDelete))
    {
        d->deletePlayer();

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

#ifdef VLCPLAYER_AUDIO
    if (d->hasAudio) d->playerAudio->setOptions(options);
#endif

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

#ifdef VLCPLAYER_AUDIO
    if (d->hasAudio) d->playerAudio->setNetworkCache(msec);
#endif

    locker.unlock();

    emit networkCacheChanged();
}

#endif // SK_NO_VLCPLAYER
