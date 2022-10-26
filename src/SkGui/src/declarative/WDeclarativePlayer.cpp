//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkGui.

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

#include "WDeclarativePlayer.h"

#ifndef SK_NO_DECLARATIVEPLAYER

// Qt includes
#ifdef QT_4
#include <QStyleOptionGraphicsItem>
#endif

// Sk includes
#include <WControllerApplication>
#include <WControllerPlaylist>
#include <WBroadcastServer>
#include <WAbstractHook>
#include <WPlaylist>
#include <WTabsTrack>
#include <WTabTrack>
#include <WLibraryFolder>
#include <WView>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

WDeclarativePlayerPrivate::WDeclarativePlayerPrivate(WDeclarativePlayer * p)
#ifdef SK_SOFTWARE
    : WDeclarativeItemPaintPrivate(p) {}
#else
    : WDeclarativeItemPrivate(p) {}
#endif

/* virtual */ WDeclarativePlayerPrivate::~WDeclarativePlayerPrivate()
{
    if (backend) backend->deleteBackend();
}

//-------------------------------------------------------------------------------------------------

void WDeclarativePlayerPrivate::init()
{
    Q_Q(WDeclarativePlayer);

    backend = NULL;
    hook    = NULL;

    backendInterface = NULL;

    server = NULL;

#if defined(QT_NEW) && defined(SK_SOFTWARE) == false
    frameUpdate = false;
#endif

    folder = NULL;

    playlist       = NULL;
    playlistServer = NULL;

    tabs = NULL;
    tab  = NULL;

    currentTime = -1;

    state = WAbstractBackend::StateStopped;

    speed = 1.0;

    volume = 1.0;

    autoPlay = false;

    shuffle = false;

    shuffleIndex = -1;
    shuffleLock  = false;

    repeat = WDeclarativePlayer::RepeatNone;

    output  = WAbstractBackend::OutputMedia;
    quality = WAbstractBackend::QualityDefault;

    fillMode = WAbstractBackend::PreserveAspectFit;

    trackVideo = -1;
    trackAudio = -1;

    scanOutput = false;

    currentOutput = 0;

    pauseTimeout = -1;

    keepState = false;

    videoTag = false;

    timer.setSingleShot(true);

#ifdef QT_4
    q->setFlag(QGraphicsItem::ItemHasNoContents, false);
#elif defined(SK_SOFTWARE) == false
    q->setFlag(QQuickItem::ItemHasContents);
#endif

    QObject::connect(q, SIGNAL(playlistChanged()), q, SIGNAL(playlistUpdated()));
    QObject::connect(q, SIGNAL(playlistChanged()), q, SIGNAL(countChanged   ()));

    QObject::connect(&timer, SIGNAL(timeout()), q, SLOT(stop()));
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WDeclarativePlayerPrivate::applyPlaylist(WPlaylist * playlist)
{
    Q_Q(WDeclarativePlayer);

    if (folder)
    {
        folder->setActiveId(-1);

        QObject::disconnect(folder, SIGNAL(destroyed()), q, SLOT(onFolderDestroyed()));
    }

    this->playlist = playlist;

    if (playlist)
    {
        folder = playlist->parentFolder();

        if (folder)
        {
            folder->setActiveId(playlist->id());

            QObject::connect(folder, SIGNAL(destroyed()), q, SLOT(onFolderDestroyed()));
        }

        if (shuffle)
        {
            playlist->registerWatcher(q);

            resetShuffle();
        }
    }
    else
    {
        folder = NULL;

        if (shuffle) resetShuffle();
    }

    emit q->playlistChanged();
}

//-------------------------------------------------------------------------------------------------

void WDeclarativePlayerPrivate::loadSource(const QString & url, int duration, int currentTime)
{
    Q_Q(WDeclarativePlayer);

    if (updateBackend(url))
    {
        backendInterface->loadSource(url, duration, currentTime);

        backendInterface->play();
    }
    else backendInterface->loadSource(url, duration, currentTime);

    if (shuffle && shuffleLock == false)
    {
        resetShuffle();

        emit q->playlistUpdated();
    }
}

bool WDeclarativePlayerPrivate::updateBackend(const QString & url)
{
    WBackendInterface * currentBackend = NULL;
    WAbstractHook     * currentHook    = NULL;

    foreach (WAbstractHook * hook, hooks)
    {
        if (hook->check(url) == false) continue;

        currentBackend = hook;

        currentHook = hook;

        break;
    }

    if (currentBackend == NULL) currentBackend = backend;

    if (backendInterface == currentBackend) return false;

    if (backendInterface)
    {
        if (backend->isPlaying())
        {
            // NOTE: We have to freeze the state to avoid clearing highlightedTab.
            keepState = true;

            backendInterface->clear();

            keepState = false;

            setBackendInterface(currentBackend, currentHook);

            return true;
        }
        else
        {
            backendInterface->clear();

            setBackendInterface(currentBackend, currentHook);
        }
    }
    else setBackendInterface(currentBackend, currentHook);

    return false;
}

void WDeclarativePlayerPrivate::stop()
{
    if (tab) tab->setCurrentTime(-1);

    backendInterface->stop();
}

//-------------------------------------------------------------------------------------------------

void WDeclarativePlayerPrivate::updateRepeat()
{
    if (repeat == WDeclarativePlayer::RepeatOne
        ||
        (repeat == WDeclarativePlayer::RepeatAll && playlist == NULL))
    {
         backend->setRepeat(true);
    }
    else backend->setRepeat(false);
}

//-------------------------------------------------------------------------------------------------

void WDeclarativePlayerPrivate::resetShuffle()
{
    Q_Q(WDeclarativePlayer);

    shuffleHistory.clear();

    if (q->count() > 1)
    {
        int index = q->trackIndex();

        const WTrack * track = playlist->trackPointerAt(index);

        if (track)
        {
            shuffleTracks = playlist->trackPointers();

            shuffleTracks.removeOne(track);

            shuffleHistory.append(track);

            shuffleIndex = 0;

            return;
        }
    }

    shuffleTracks.clear();

    shuffleIndex = -1;
}

void WDeclarativePlayerPrivate::clearShuffle()
{
    shuffleHistory.clear();
    shuffleTracks .clear();

    shuffleIndex = -1;
}

//-------------------------------------------------------------------------------------------------

void WDeclarativePlayerPrivate::clearPlaylistAndTabs()
{
    Q_Q(WDeclarativePlayer);

    if (tabs)
    {
        QObject::disconnect(tabs, 0, q, 0);
        QObject::disconnect(tab,  0, q, 0);

        tabs = NULL;
        tab  = NULL;
    }

    if (playlist)
    {
        if (shuffle) playlist->unregisterWatcher(q);

        QObject::disconnect(playlist, 0, q, 0);

        playlist = NULL;
    }
}

//-------------------------------------------------------------------------------------------------

void WDeclarativePlayerPrivate::setBackendInterface(WBackendInterface * currentBackend,
                                                    WAbstractHook     * currentHook)
{
    if (backendInterface == currentBackend) return;

    Q_Q(WDeclarativePlayer);

    if (hook) QObject::disconnect(hook,    SIGNAL(sourceChanged()), q, SIGNAL(sourceChanged()));
    else      QObject::disconnect(backend, SIGNAL(sourceChanged()), q, SIGNAL(sourceChanged()));

    backendInterface = currentBackend;

    hook = currentHook;

    if (hook) QObject::connect(hook,    SIGNAL(sourceChanged()), q, SIGNAL(sourceChanged()));
    else      QObject::connect(backend, SIGNAL(sourceChanged()), q, SIGNAL(sourceChanged()));
}

void WDeclarativePlayerPrivate::setPlaylist(WPlaylist * playlist)
{
    if (this->playlist == playlist) return;

    if (shuffle && this->playlist)
    {
        Q_Q(WDeclarativePlayer);

        this->playlist->unregisterWatcher(q);
    }

    applyPlaylist(playlist);
}

void WDeclarativePlayerPrivate::setTab(WTabTrack * tab)
{
    if (this->tab == tab) return;

    Q_Q(WDeclarativePlayer);

    if (this->tab)
    {
        currentTime = -1;

        this->tab->setPlayer(NULL);

        this->tab->setStackEnabled(false);

        QObject::disconnect(this->tab, 0, q, 0);
    }

    this->tab = tab;

    if (tab)
    {
        QObject::connect(tab, SIGNAL(countChanged()), q, SIGNAL(countChanged()));

        QObject::connect(tab, SIGNAL(playlistUpdated()), q, SIGNAL(playlistUpdated()));

        QObject::connect(tab, SIGNAL(currentBookmarkChanged()),
                         q,   SLOT(onCurrentBookmarkChanged()));

        QObject::connect(tab, SIGNAL(currentBookmarkUpdated()),
                         q,   SLOT(onCurrentBookmarkUpdated()));

        QObject::connect(tab, SIGNAL(destroyed   ()),
                         q,   SLOT(onTabDestroyed()));

        tab->setPlayer(q);

        if (q->hasStarted())
        {
             tab->setStackEnabled(true);
        }
        else tab->setStackEnabled(false);

        onCurrentBookmarkChanged();
        onCurrentBookmarkUpdated();
    }
    else emit q->playlistChanged();

    emit q->tabChanged();
}

void WDeclarativePlayerPrivate::setShuffleTrack(const WTrack * track)
{
    shuffleLock = true;

    if (tab)
    {
         tab->setCurrentTrackPointer(track);
    }
    else playlist->setCurrentTrackPointer(track);

    shuffleLock = false;
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WDeclarativePlayerPrivate::onEnded()
{
    if (autoPlay == false)
    {
        Q_Q(WDeclarativePlayer);

        stop();

        emit q->ended();

        return;
    }

    if (repeat != WDeclarativePlayer::RepeatOne)
    {
        Q_Q(WDeclarativePlayer);

        if (q->hasNextTrack() == false)
        {
            if (playlist && repeat == WDeclarativePlayer::RepeatAll)
            {
                playlist->setCurrentIndex(0);

                backendInterface->replay();
            }
            else
            {
                stop();

                emit q->ended();
            }
        }
        else q->setNextTrack();
    }
    else backendInterface->replay();
}

void WDeclarativePlayerPrivate::onError()
{
    // NOTE: We want to save the current state before stopping the backend.
    if (tab) tab->setPlayer(NULL);
}

//-------------------------------------------------------------------------------------------------

void WDeclarativePlayerPrivate::onHookUpdated()
{
    Q_Q(WDeclarativePlayer);

    loadSource(q->source(), q->duration(), q->currentTime());
}

//-------------------------------------------------------------------------------------------------

void WDeclarativePlayerPrivate::onStateChanged()
{
    if (keepState) return;

    Q_Q(WDeclarativePlayer);

    WAbstractBackend::State state = backend->state();

    if (tab)
    {
        if (state == WAbstractBackend::StatePlaying)
        {
            timer.stop();

            tab->setStackEnabled(true);
        }
        else if (state == WAbstractBackend::StatePaused)
        {
            if (tabs->highlightedTab())
            {
                timer.stop();

                tab->setPlayer(NULL);

                backendInterface->stop();

                return;
            }

            if (pauseTimeout != -1) timer.start();
        }
        else // if (state == WAbstractBackend::StateStopped)
        {
            timer.stop();

            tab->setStackEnabled(false);

            tabs->setHighlightedTab(NULL);
        }
    }
    else if (state == WAbstractBackend::StatePaused)
    {
        if (pauseTimeout != -1) timer.start();
    }
    else timer.stop();

    if (this->state != state)
    {
        this->state = state;

        emit q->stateChanged();
    }
}

void WDeclarativePlayerPrivate::onDurationChanged()
{
    if (tab) tab->setDuration(backend->duration());
}

//-------------------------------------------------------------------------------------------------

void WDeclarativePlayerPrivate::onCurrentTrackChanged()
{
    Q_Q(WDeclarativePlayer);

    const WTrack * track = playlist->currentTrackPointer();

    loadSource(track->source(), track->duration(), -1);

    emit q->currentTrackUpdated();
}

//-------------------------------------------------------------------------------------------------

void WDeclarativePlayerPrivate::onCurrentTabChanged()
{
    Q_Q(WDeclarativePlayer);

    WTabTrack * currentTab = tabs->currentTabTrack();

    if (q->isPlaying())
    {
        if (tabs->highlightedTab())
        {
            if (tab == currentTab)
            {
                tabs->setHighlightedTab(NULL);
            }
        }
        else if (currentTab->currentTime() == -1)
        {
            tabs->setHighlightedTab(tab);
        }
        else setTab(currentTab);
    }
    else setTab(currentTab);
}

void WDeclarativePlayerPrivate::onHighlightedTabChanged()
{
    WTabTrack * tab = tabs->highlightedTab();

    if (tab)
    {
         setTab(tab);
    }
    else setTab(tabs->currentTabTrack());
}

//-------------------------------------------------------------------------------------------------

void WDeclarativePlayerPrivate::onCurrentBookmarkChanged()
{
    const WBookmarkTrack * bookmark = tab->currentBookmark();

    if (bookmark)
    {
        if (currentTime != -1)
        {
            tab->setCurrentTime(currentTime);

            currentTime = -1;
        }

        loadSource(bookmark->source(), bookmark->duration(), bookmark->currentTime());
    }
    else if (backend && backendInterface->source().isEmpty() == false)
    {
        backendInterface->loadSource(QString());
    }
}

void WDeclarativePlayerPrivate::onCurrentBookmarkUpdated()
{
    Q_Q(WDeclarativePlayer);

    const WBookmarkTrack * bookmark = tab->currentBookmark();

    if (bookmark)
    {
        WPlaylist * playlist = bookmark->playlist();

        if (playlist)
        {
             setPlaylist(playlist->toPlaylist());
        }
        else setPlaylist(NULL);

        if (backend)
        {
            backend->setSubtitle(bookmark->subtitle());
        }
    }
    else setPlaylist(NULL);

    emit q->currentTrackUpdated();
}

//-------------------------------------------------------------------------------------------------

void WDeclarativePlayerPrivate::onConnectedChanged()
{
    Q_Q(WDeclarativePlayer);

    q->stop();

    if (backend == NULL) return;

    if (server->isConnected())
    {
        server->sendReply(WBroadcastReply::VOLUME, QString::number(backend->volume()));

        if (view)
        {
            onScreen();

            server->sendReply(WBroadcastReply::FULLSCREEN, QString::number(view->isFullScreen()));

            server->sendReply(WBroadcastReply::VIDEOTAG, QString::number(videoTag));

#ifdef QT_4
            QObject::connect(qApp->desktop(), SIGNAL(screenCountChanged()), q, SLOT(onScreen()));
#else
            QObject::connect(qApp, SIGNAL(screenAdded  (QScreen *)), q, SLOT(onScreen()));
            QObject::connect(qApp, SIGNAL(screenRemoved(QScreen *)), q, SLOT(onScreen()));
#endif
            QObject::connect(view, SIGNAL(availableGeometryChanged()), q, SLOT(onScreen    ()));
            QObject::connect(view, SIGNAL(fullScreenChanged       ()), q, SLOT(onFullScreen()));
        }

        QObject::connect(q, SIGNAL(sourceChanged()), q, SLOT(onSource()));

        QObject::connect(backend, SIGNAL(stateChanged        ()), q, SLOT(onState    ()));
        QObject::connect(backend, SIGNAL(stateLoadChanged    ()), q, SLOT(onStateLoad()));
        QObject::connect(backend, SIGNAL(liveChanged         ()), q, SLOT(onLive     ()));
        QObject::connect(backend, SIGNAL(endedChanged        ()), q, SLOT(onEnd      ()));
        QObject::connect(backend, SIGNAL(currentTimeChanged  ()), q, SLOT(onTime     ()));
        QObject::connect(backend, SIGNAL(durationChanged     ()), q, SLOT(onDuration ()));
        QObject::connect(backend, SIGNAL(progressChanged     ()), q, SLOT(onProgress ()));
        QObject::connect(backend, SIGNAL(outputActiveChanged ()), q, SLOT(onOutput   ()));
        QObject::connect(backend, SIGNAL(qualityActiveChanged()), q, SLOT(onQuality  ()));
        QObject::connect(backend, SIGNAL(videosChanged       ()), q, SLOT(onVideos   ()));
        QObject::connect(backend, SIGNAL(audiosChanged       ()), q, SLOT(onAudios   ()));
    }
    else
    {
        if (view)
        {
            QObject::disconnect(qApp, NULL, q, NULL);
            QObject::disconnect(view, NULL, q, NULL);
        }

        QObject::disconnect(q, SIGNAL(sourceChanged()), q, SLOT(onSource()));

        QObject::disconnect(backend, SIGNAL(stateChanged        ()), q, SLOT(onState    ()));
        QObject::disconnect(backend, SIGNAL(stateLoadChanged    ()), q, SLOT(onStateLoad()));
        QObject::disconnect(backend, SIGNAL(liveChanged         ()), q, SLOT(onLive     ()));
        QObject::disconnect(backend, SIGNAL(endedChanged        ()), q, SLOT(onEnd      ()));
        QObject::disconnect(backend, SIGNAL(currentTimeChanged  ()), q, SLOT(onTime     ()));
        QObject::disconnect(backend, SIGNAL(durationChanged     ()), q, SLOT(onDuration ()));
        QObject::disconnect(backend, SIGNAL(progressChanged     ()), q, SLOT(onProgress ()));
        QObject::disconnect(backend, SIGNAL(outputActiveChanged ()), q, SLOT(onOutput   ()));
        QObject::disconnect(backend, SIGNAL(qualityActiveChanged()), q, SLOT(onQuality  ()));
        QObject::disconnect(backend, SIGNAL(videosChanged       ()), q, SLOT(onVideos   ()));
        QObject::disconnect(backend, SIGNAL(audiosChanged       ()), q, SLOT(onAudios   ()));
    }
}

void WDeclarativePlayerPrivate::onMessage(const WBroadcastMessage & message)
{
    WBroadcastMessage::Type type = message.type;

    if (type == WBroadcastMessage::SOURCE)
    {
        const QStringList & parameters = message.parameters;

        QString url = parameters.first();

        if (tab)
        {
            if (url.isEmpty())
            {
                loadSource(QString(), -1, -1);

                return;
            }

            Q_Q(WDeclarativePlayer);

            // NOTE: This playlist is useful to load the track data.
            if (playlistServer == NULL)
            {
                playlistServer = new WPlaylist;

                playlistServer->setParent(q);
            }

            WTrack track;

            if (WControllerPlaylist::urlIsVbmlUri(url))
            {
                // NOTE: Maybe we should consider doing this in a thread, but we need messages to
                //       be sequential.
                WControllerPlaylist::vbmlApplyTrack(&track, url);
            }
            else
            {
                track.setSource(url);

                track.setState(WTrack::Default);
            }

            track.setDuration(parameters.at(1).toInt());

            // NOTE: We have to apply the currentTime in onCurrentBookmarkChanged.
            currentTime = parameters.at(2).toInt();

            QString subtitle = q->subtitle();

            tab->setPlaylist(playlistServer);

            playlistServer->insertTrack(0, track);

            playlistServer->loadTrack(0);

            playlistServer->setCurrentIndex(0);

            // NOTE: Removing the previous track after setting the new one.
            if (playlistServer->count() > 1)
            {
                playlistServer->removeTrack(1);
            }

            // NOTE: We restore the tab subtitle manually.
            q->setSubtitle(subtitle);
        }
        else if (backend)
        {
            if (url.isEmpty())
            {
                loadSource(QString(), -1, -1);

                return;
            }

            int currentTime = parameters.at(2).toInt();

            if (backendInterface->source() == url)
            {
                Q_Q(WDeclarativePlayer);

                q->seek(currentTime);

                return;
            }

            clearPlaylistAndTabs();

            loadSource(url, parameters.at(1).toInt(), currentTime);
        }
        else
        {
            Q_Q(WDeclarativePlayer);

            q->setSource(url);
        }
    }
    else if (type == WBroadcastMessage::PLAY)
    {
        Q_Q(WDeclarativePlayer);

        q->play();
    }
    else if (type == WBroadcastMessage::REPLAY)
    {
        Q_Q(WDeclarativePlayer);

        q->replay();
    }
    else if (type == WBroadcastMessage::PAUSE)
    {
        Q_Q(WDeclarativePlayer);

        q->pause();
    }
    else if (type == WBroadcastMessage::STOP)
    {
        Q_Q(WDeclarativePlayer);

        q->stop();
    }
    else if (type == WBroadcastMessage::SEEK)
    {
        Q_Q(WDeclarativePlayer);

        q->seek(message.parameters.first().toInt());
    }
    else if (type == WBroadcastMessage::OUTPUT)
    {
        Q_Q(WDeclarativePlayer);

        q->setOutput(WAbstractBackend::outputFromString(message.parameters.first()));
    }
    else if (type == WBroadcastMessage::QUALITY)
    {
        Q_Q(WDeclarativePlayer);

        q->setQuality(WAbstractBackend::qualityFromString(message.parameters.first()));
    }
    else if (type == WBroadcastMessage::FILLMODE)
    {
        Q_Q(WDeclarativePlayer);

        q->setFillMode(WAbstractBackend::fillModeFromString(message.parameters.first()));
    }
    else if (type == WBroadcastMessage::SPEED)
    {
        Q_Q(WDeclarativePlayer);

        q->setSpeed(message.parameters.first().toFloat());
    }
    else if (type == WBroadcastMessage::VIDEO)
    {
        Q_Q(WDeclarativePlayer);

        q->setTrackVideo(message.parameters.first().toInt());
    }
    else if (type == WBroadcastMessage::AUDIO)
    {
        Q_Q(WDeclarativePlayer);

        q->setTrackAudio(message.parameters.first().toInt());
    }
    else if (type == WBroadcastMessage::SUBTITLE)
    {
        Q_Q(WDeclarativePlayer);

        q->setSubtitle(message.parameters.first());
    }
    else if (type == WBroadcastMessage::VOLUME)
    {
        Q_Q(WDeclarativePlayer);

        q->setVolume(message.parameters.first().toFloat());
    }
    else if (type == WBroadcastMessage::SCREEN)
    {
        if (view == NULL) return;

        view->moveToScreen(message.parameters.first().toInt());

        // NOTE: Making sure the window is visible.
        view->raise();
    }
    else if (type == WBroadcastMessage::FULLSCREEN)
    {
        if (view == NULL) return;

        bool fullScreen = message.parameters.first().toInt();

        // NOTE: Making sure the window is visible.
        if (fullScreen) view->raise();

        view->setFullScreen(fullScreen);
    }
    else if (type == WBroadcastMessage::VIDEOTAG)
    {
        Q_Q(WDeclarativePlayer);

        q->setVideoTag(message.parameters.first().toInt());
    }
}

void WDeclarativePlayerPrivate::onSource()
{
    // NOTE: We must call WBackendInterface::source to retrieve the proper source.
    server->sendReply(WBroadcastReply::SOURCE, backendInterface->source());
}

void WDeclarativePlayerPrivate::onState()
{
    server->sendReply(WBroadcastReply::STATE,
                      WAbstractBackend::stateToString(backend->state()));
}

void WDeclarativePlayerPrivate::onStateLoad()
{
    server->sendReply(WBroadcastReply::STATELOAD,
                      WAbstractBackend::stateLoadToString(backend->stateLoad()));
}

void WDeclarativePlayerPrivate::onLive()
{
    server->sendReply(WBroadcastReply::LIVE, QString::number(backend->isLive()));
}

void WDeclarativePlayerPrivate::onEnd()
{
    server->sendReply(WBroadcastReply::ENDED, QString::number(backend->hasEnded()));
}

void WDeclarativePlayerPrivate::onTime()
{
    server->sendReply(WBroadcastReply::TIME, QString::number(backend->currentTime()));
}

void WDeclarativePlayerPrivate::onDuration()
{
    server->sendReply(WBroadcastReply::DURATION, QString::number(backend->duration()));
}

void WDeclarativePlayerPrivate::onProgress()
{
    server->sendReply(WBroadcastReply::PROGRESS, QString::number(backend->progress()));
}

void WDeclarativePlayerPrivate::onOutput()
{
    server->sendReply(WBroadcastReply::OUTPUT,
                      WAbstractBackend::outputToString(backend->outputActive()));
}

void WDeclarativePlayerPrivate::onQuality()
{
    server->sendReply(WBroadcastReply::QUALITY,
                      WAbstractBackend::qualityToString(backend->qualityActive()));
}

void WDeclarativePlayerPrivate::onVideos()
{
    QStringList parameters;

    QList<WBackendTrack> videos = backend->videos();

    parameters.append(QString::number(backend->trackVideo()));

    foreach (const WBackendTrack & video, videos)
    {
        parameters.append(WAbstractBackend::trackToString(video));
    }

    server->sendReply(WBroadcastReply::VIDEOS, parameters);
}

void WDeclarativePlayerPrivate::onAudios()
{
    QStringList parameters;

    QList<WBackendTrack> audios = backend->audios();

    parameters.append(QString::number(backend->trackAudio()));

    foreach (const WBackendTrack & audio, audios)
    {
        parameters.append(WAbstractBackend::trackToString(audio));
    }

    server->sendReply(WBroadcastReply::AUDIOS, parameters);
}

void WDeclarativePlayerPrivate::onScreen()
{
    QStringList parameters;

    parameters.append(QString::number(view->screenNumber()));
    parameters.append(QString::number(view->screenCount ()));

    server->sendReply(WBroadcastReply::SCREEN, parameters);
}

void WDeclarativePlayerPrivate::onFullScreen()
{
    server->sendReply(WBroadcastReply::FULLSCREEN, QString::number(view->isFullScreen()));
}

//-------------------------------------------------------------------------------------------------

void WDeclarativePlayerPrivate::onHookDestroyed()
{
    Q_Q(WDeclarativePlayer);

    WAbstractHook * hook = static_cast<WAbstractHook *> (q->sender());

    if (backendInterface == hook)
    {
        setBackendInterface(backend, NULL);
    }

    hooks.removeOne(hook);

    emit q->hooksChanged();
}

void WDeclarativePlayerPrivate::onPlaylistDestroyed()
{
    Q_Q(WDeclarativePlayer);

    playlist = NULL;

    emit q->playlistChanged();
}

void WDeclarativePlayerPrivate::onFolderDestroyed()
{
    folder = NULL;
}

void WDeclarativePlayerPrivate::onTabsDestroyed()
{
    Q_Q(WDeclarativePlayer);

    clearPlaylistAndTabs();

    tabs = NULL;

    emit q->tabsChanged();
}

void WDeclarativePlayerPrivate::onTabDestroyed()
{
    Q_Q(WDeclarativePlayer);

    if (backend)
    {
        timer.stop();

        backendInterface->stop();
    }

    tab = NULL;

    emit q->tabChanged();
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
/* explicit */ WDeclarativePlayer::WDeclarativePlayer(QDeclarativeItem * parent)
#else
/* explicit */ WDeclarativePlayer::WDeclarativePlayer(QQuickItem * parent)
#endif
#ifdef SK_SOFTWARE
    : WDeclarativeItemPaint(new WDeclarativePlayerPrivate(this), parent)
#else
    : WDeclarativeItem(new WDeclarativePlayerPrivate(this), parent)
#endif
{
    Q_D(WDeclarativePlayer); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WDeclarativePlayer::play()
{
    Q_D(WDeclarativePlayer);

    if (d->backend == NULL) return;

    if (d->tab)
    {
        d->tab->setPlayer(this);

        if (d->backend->isStopped())
        {
            d->backendInterface->seek(d->tab->currentTime());
        }
    }

    d->backendInterface->play();
}

/* Q_INVOKABLE */ void WDeclarativePlayer::replay()
{
    Q_D(WDeclarativePlayer);

    if (d->backend == NULL) return;

    if (d->tab) d->tab->setPlayer(this);

    d->backendInterface->replay();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WDeclarativePlayer::pause()
{
    Q_D(WDeclarativePlayer);

    if (d->backend) d->backendInterface->pause();
}

/* Q_INVOKABLE */ void WDeclarativePlayer::stop()
{
    Q_D(WDeclarativePlayer);

    if (d->backend == NULL) return;

    if (d->tab) d->tab->setPlayer(NULL);

    d->backendInterface->stop();
}

/* Q_INVOKABLE */ void WDeclarativePlayer::clear()
{
    Q_D(WDeclarativePlayer);

    if (d->backend) d->backendInterface->clear();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WDeclarativePlayer::togglePlay()
{
    if (isPlaying()) pause();
    else             play ();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WDeclarativePlayer::seek(int msec)
{
    Q_D(WDeclarativePlayer);

    if (d->backend) d->backendInterface->seek(msec);

    if (d->state == WAbstractBackend::StatePaused && d->pauseTimeout > -1)
    {
        d->timer.start();
    }
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WDeclarativePlayer::setPreviousTrack()
{
    Q_D(WDeclarativePlayer);

    if (d->shuffle)
    {
        if (d->shuffleIndex == -1) return;

        const WTrack * track;

        if (d->shuffleTracks.isEmpty() && d->shuffleIndex == 0
            &&
            d->repeat == WDeclarativePlayer::RepeatAll)
        {
            d->shuffleIndex = d->shuffleHistory.count() - 1;

            track = d->shuffleHistory.at(d->shuffleIndex);

            d->setShuffleTrack(track);

            return;
        }

        if (d->shuffleIndex != 0)
        {
            d->shuffleIndex--;

            track = d->shuffleHistory.at(d->shuffleIndex);
        }
        else
        {
            int count = d->shuffleTracks.count();

            if (count == 0) return;

            int index = Sk::randomInt() % count;

            track = d->shuffleTracks.takeAt(index);

            d->shuffleHistory.prepend(track);
        }

        d->setShuffleTrack(track);
    }
    else if (d->tab)
    {
        if (d->repeat == WDeclarativePlayer::RepeatNone)
        {
             d->tab->setPreviousTrack(false);
        }
        else d->tab->setPreviousTrack(true);
    }
    else if (d->playlist)
    {
        if (d->repeat == WDeclarativePlayer::RepeatNone)
        {
             d->playlist->setPreviousTrack(false);
        }
        else d->playlist->setPreviousTrack(true);
    }
}

/* Q_INVOKABLE */ void WDeclarativePlayer::setNextTrack()
{
    Q_D(WDeclarativePlayer);

    if (d->shuffle)
    {
        if (d->shuffleIndex == -1) return;

        const WTrack * track;

        int last = d->shuffleHistory.count() - 1;

        if (d->shuffleTracks.isEmpty() && d->shuffleIndex == last
            &&
            d->repeat == WDeclarativePlayer::RepeatAll)
        {
            d->shuffleIndex = 0;

            track = d->shuffleHistory.at(d->shuffleIndex);

            d->setShuffleTrack(track);

            return;
        }

        if (d->shuffleIndex != last)
        {
            d->shuffleIndex++;

            track = d->shuffleHistory.at(d->shuffleIndex);
        }
        else
        {
            int count = d->shuffleTracks.count();

            if (count == 0) return;

            int index = Sk::randomInt() % count;

            track = d->shuffleTracks.takeAt(index);

            d->shuffleHistory.append(track);

            d->shuffleIndex++;
        }

        d->setShuffleTrack(track);
    }
    else if (d->tab)
    {
        if (d->repeat == WDeclarativePlayer::RepeatNone)
        {
             d->tab->setNextTrack(false);
        }
        else d->tab->setNextTrack(true);
    }
    else if (d->playlist)
    {
        if (d->repeat == WDeclarativePlayer::RepeatNone)
        {
             d->playlist->setNextTrack(false);
        }
        else d->playlist->setNextTrack(true);
    }
}

//-------------------------------------------------------------------------------------------------

#ifdef QT_NEW

void WDeclarativePlayer::updateFrame()
{
#ifndef SK_SOFTWARE
    Q_D(WDeclarativePlayer);

    d->backend->synchronize(&d->frame);
#endif

    update();
}

#endif

/* Q_INVOKABLE */ QImage WDeclarativePlayer::getFrame() const
{
    Q_D(const WDeclarativePlayer);

    if (d->backend)
    {
        d->backend->updateFrame();

        return d->backend->getFrame();
    }
    else return QImage();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ QRectF WDeclarativePlayer::getRect() const
{
    Q_D(const WDeclarativePlayer);

    if (d->backend)
    {
         return d->backend->getRect();
    }
    else return QRectF();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WDeclarativePlayer::updateHighlightedTab()
{
    Q_D(WDeclarativePlayer); d->onHighlightedTabChanged();
}

//---------------------------------------------------------------------------------------------
// Tracks

/* Q_INVOKABLE */ int WDeclarativePlayer::idVideo(int index) const
{
    Q_D(const WDeclarativePlayer);

    if (d->backend)
    {
         return d->backend->idVideo(index);
    }
    else return -1;
}

/* Q_INVOKABLE */ int WDeclarativePlayer::idAudio(int index) const
{
    Q_D(const WDeclarativePlayer);

    if (d->backend)
    {
         return d->backend->idAudio(index);
    }
    else return -1;
}

/* Q_INVOKABLE */ int WDeclarativePlayer::indexVideo(int id) const
{
    Q_D(const WDeclarativePlayer);

    if (d->backend)
    {
         return d->backend->indexVideo(id);
    }
    else return -1;
}

/* Q_INVOKABLE */ int WDeclarativePlayer::indexAudio(int id) const
{
    Q_D(const WDeclarativePlayer);

    if (d->backend)
    {
         return d->backend->indexAudio(id);
    }
    else return -1;
}

/* Q_INVOKABLE */ QString WDeclarativePlayer::videoName(int id) const
{
    Q_D(const WDeclarativePlayer);

    if (d->backend)
    {
         return d->backend->videoName(id);
    }
    else return QString();
}

/* Q_INVOKABLE */ QString WDeclarativePlayer::audioName(int id) const
{
    Q_D(const WDeclarativePlayer);

    if (d->backend)
    {
         return d->backend->audioName(id);
    }
    else return QString();
}

//-------------------------------------------------------------------------------------------------
// Virtual interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QString WDeclarativePlayer::toVbml(const QString & source,
                                                             int             currentTime) const
{
    Q_D(const WDeclarativePlayer);

    if (d->tab)
    {
        return d->tab->toVbml(source, currentTime);
    }
    else return QString();
}

#if defined(QT_4) || defined(SK_SOFTWARE)

//-------------------------------------------------------------------------------------------------
// QGraphicsItem reimplementation
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
/* virtual */ void WDeclarativePlayer::paint(QPainter                       * painter,
                                             const QStyleOptionGraphicsItem * option, QWidget *)
#else
/* virtual */ void WDeclarativePlayer::paint(QPainter * painter)
#endif
{
#ifdef QT_NEW
    if (isVisible() == false) return;
#endif

    Q_D(WDeclarativePlayer);

    if (d->backend)
    {
#ifdef QT_4
        d->backend->drawFrame(painter, option->rect);
#else
        d->backend->drawFrame(painter, boundingRect().toRect());
#endif
    }
}

#else

//-------------------------------------------------------------------------------------------------
// QQuickItem reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ QSGNode * WDeclarativePlayer::updatePaintNode(QSGNode             * oldNode,
                                                            UpdatePaintNodeData *)
{
    Q_D(WDeclarativePlayer);

    WAbstractBackend::FrameState state = d->frame.state;

    if (state == WAbstractBackend::FrameDefault)
    {
        if (oldNode)
        {
            WBackendNode * node = static_cast<WBackendNode *> (oldNode);

            node->setTextures(NULL);

            if (d->frameUpdate)
            {
                d->frameUpdate = false;

                node->setRect(d->backend->getRect());
            }

            return node;
        }
        else return NULL;
    }
    else if (state == WAbstractBackend::FrameReset)
    {
        d->frame.state = WAbstractBackend::FrameDefault;

        if (oldNode) delete oldNode;

        WBackendNode * node = d->backend->createNode();

        node->setTextures(d->frame.textures);

        d->frameUpdate = false;

        node->setRect(d->backend->getRect());

        return node;
    }
    else if (state == WAbstractBackend::FrameUpdate)
    {
        d->frame.state = WAbstractBackend::FrameDefault;

        WBackendNode * node;

        if (oldNode)
        {
            node = static_cast<WBackendNode *> (oldNode);

            node->setTextures(d->frame.textures);

            if (d->frameUpdate)
            {
                d->frameUpdate = false;

                node->setRect(d->backend->getRect());
            }
        }
        else
        {
            node = d->backend->createNode();

            node->setTextures(d->frame.textures);

            d->frameUpdate = false;

            node->setRect(d->backend->getRect());
        }

        return node;
    }
    else // if (state == WAbstractBackend::FrameClear)
    {
        if (oldNode) delete oldNode;

        return NULL;
    }
}

#endif

//-------------------------------------------------------------------------------------------------
// Protected QDeclarativeItem / QQuickItem reimplementation
//-------------------------------------------------------------------------------------------------

#ifdef QT_OLD
/* virtual */ void WDeclarativePlayer::geometryChanged(const QRectF & newGeometry,
                                                       const QRectF & oldGeometry)
#else
/* virtual */ void WDeclarativePlayer::geometryChange(const QRectF & newGeometry,
                                                      const QRectF & oldGeometry)
#endif
{
    Q_D(WDeclarativePlayer);

    if (oldGeometry.size() != newGeometry.size() && d->backend)
    {
        d->backend->setSize(newGeometry.size());

#if defined(QT_NEW) && defined(SK_SOFTWARE) == false
        d->frameUpdate = true;
#endif
    }

#ifdef QT_OLD
#ifdef SK_SOFTWARE
    WDeclarativeItemPaint::geometryChanged(newGeometry, oldGeometry);
#else
    WDeclarativeItem::geometryChanged(newGeometry, oldGeometry);
#endif
#else
#ifdef SK_SOFTWARE
    WDeclarativeItemPaint::geometryChange(newGeometry, oldGeometry);
#else
    WDeclarativeItem::geometryChange(newGeometry, oldGeometry);
#endif
#endif
}

//-------------------------------------------------------------------------------------------------
// Protected WPlaylistWatcher implementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativePlayer::beginTracksInsert(int first, int last)
{
    Q_D(WDeclarativePlayer);

    if (d->shuffle == false) return;

    for (int i = first; i <= last; i++)
    {
        const WTrack * track = d->playlist->trackPointerAt(i);

        d->shuffleTracks.append(track);
    }
}

/* virtual */ void WDeclarativePlayer::endTracksInsert()
{
    Q_D(WDeclarativePlayer);

    if (d->shuffle && d->shuffleIndex == -1)
    {
        d->resetShuffle();

        if (d->shuffleIndex == 0)
        {
            emit playlistUpdated();
        }
    }
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativePlayer::beginTracksRemove(int first, int last)
{
    Q_D(WDeclarativePlayer);

    if (d->shuffle == false) return;

    for (int i = first; i <= last; i++)
    {
        const WTrack * track = d->playlist->trackPointerAt(i);

        if (d->shuffleIndex == i)
        {
            d->shuffleIndex = -1;
        }
        else if (d->shuffleIndex > i)
        {
            d->shuffleIndex--;
        }

        d->shuffleTracks .removeOne(track);
        d->shuffleHistory.removeOne(track);
    }
}

/* virtual */ void WDeclarativePlayer::beginTracksClear()
{
    Q_D(WDeclarativePlayer);

    if (d->shuffle == false) return;

    d->shuffleIndex = -1;

    d->shuffleTracks .clear();
    d->shuffleHistory.clear();
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

WAbstractBackend * WDeclarativePlayer::backend() const
{
    Q_D(const WDeclarativePlayer); return d->backend;
}

void WDeclarativePlayer::setBackend(WAbstractBackend * backend)
{
    Q_D(WDeclarativePlayer);

    if (backend == NULL) return;

    if (d->backend)
    {
        qWarning("WDeclarativePlayer::setBackend: The backend is already set.");

        return;
    }

    d->backend = backend;

    d->setBackendInterface(backend, NULL);

    backend->setParent(this);
    backend->setPlayer(this);

    backend->setSize(QSizeF(width(), height()));

    d->updateRepeat();

    backend->setSpeed(d->speed);

    backend->setVolume(d->volume);

    backend->setOutput (d->output);
    backend->setQuality(d->quality);

    backend->setFillMode(d->fillMode);

    backend->setTrackVideo(d->trackVideo);
    backend->setTrackAudio(d->trackAudio);

    backend->setScanOutput(d->scanOutput);

    backend->setCurrentOutput(d->currentOutput);

    if (d->source.isEmpty() == false)
    {
        d->clearPlaylistAndTabs();

        d->loadSource(d->source, -1, -1);
    }

    connect(backend, SIGNAL(stateChanged    ()), this, SIGNAL(stateChanged    ()));
    connect(backend, SIGNAL(stateLoadChanged()), this, SIGNAL(stateLoadChanged()));

    connect(backend, SIGNAL(liveChanged()), this, SIGNAL(liveChanged()));

    connect(backend, SIGNAL(startedChanged()), this, SIGNAL(startedChanged()));
    connect(backend, SIGNAL(endedChanged  ()), this, SIGNAL(endedChanged  ()));

    connect(backend, SIGNAL(currentTimeChanged()), this, SIGNAL(currentTimeChanged()));
    connect(backend, SIGNAL(durationChanged   ()), this, SIGNAL(durationChanged   ()));

    connect(backend, SIGNAL(progressChanged()), this, SIGNAL(progressChanged()));

    connect(backend, SIGNAL(speedChanged()), this, SIGNAL(speedChanged()));

    connect(backend, SIGNAL(volumeChanged()), this, SIGNAL(volumeChanged()));

    connect(backend, SIGNAL(repeatChanged()), this, SIGNAL(repeatChanged()));

    connect(backend, SIGNAL(outputChanged ()), this, SIGNAL(outputChanged ()));
    connect(backend, SIGNAL(qualityChanged()), this, SIGNAL(qualityChanged()));

    connect(backend, SIGNAL(outputActiveChanged ()), this, SIGNAL(outputActiveChanged ()));
    connect(backend, SIGNAL(qualityActiveChanged()), this, SIGNAL(qualityActiveChanged()));

    connect(backend, SIGNAL(fillModeChanged()), this, SIGNAL(fillModeChanged()));

    connect(backend, SIGNAL(trackVideoChanged()), this, SIGNAL(trackVideoChanged()));
    connect(backend, SIGNAL(trackAudioChanged()), this, SIGNAL(trackAudioChanged()));

    connect(backend, SIGNAL(videosChanged()), this, SIGNAL(videosChanged()));
    connect(backend, SIGNAL(audiosChanged()), this, SIGNAL(audiosChanged()));

    connect(backend, SIGNAL(scanOutputChanged()), this, SIGNAL(scanOutputChanged()));

    connect(backend, SIGNAL(currentOutputChanged()), this, SIGNAL(currentOutputChanged()));

    connect(backend, SIGNAL(outputsChanged()), this, SIGNAL(outputsChanged()));

    connect(backend, SIGNAL(subtitleChanged()), this, SIGNAL(subtitleChanged()));

    connect(backend, SIGNAL(ended()), this, SLOT(onEnded()));

    connect(backend, SIGNAL(error(const QString &)), this, SLOT(onError()));

    connect(backend, SIGNAL(stateChanged   ()), this, SLOT(onStateChanged   ()));
    connect(backend, SIGNAL(durationChanged()), this, SLOT(onDurationChanged()));

    emit backendChanged();
}

QList<WAbstractHook *> WDeclarativePlayer::hooks() const
{
    Q_D(const WDeclarativePlayer); return d->hooks;
}

void WDeclarativePlayer::setHooks(const QList<WAbstractHook *> & hooks)
{
    Q_D(WDeclarativePlayer);

    if (d->backend == NULL)
    {
        qWarning("WDeclarativePlayer::setHooks: The backend is not set.");

        return;
    }

    if (d->hooks.isEmpty() == false)
    {
        qWarning("WDeclarativePlayer::setHooks: Hooks are already set.");

        return;
    }

    d->hooks = hooks;

    foreach (WAbstractHook * hook, hooks)
    {
        Q_ASSERT(hook);
        Q_ASSERT(hook->backend() == d->backend);

        connect(hook, SIGNAL(hookUpdated()), this, SLOT(onHookUpdated()));

        connect(hook, SIGNAL(destroyed()), this, SLOT(onHookDestroyed()));
    }

    emit hooksChanged();
}

//-------------------------------------------------------------------------------------------------

WBroadcastServer * WDeclarativePlayer::server() const
{
    Q_D(const WDeclarativePlayer); return d->server;
}

void WDeclarativePlayer::setServer(WBroadcastServer * server)
{
    Q_D(WDeclarativePlayer);

    if (server == NULL) return;

    if (d->server)
    {
        qWarning("WDeclarativePlayer::setServer: The server is already set.");

        return;
    }

    d->server = server;

    // NOTE: Stopping the playback upon server connect or disconnect.
    connect(server, SIGNAL(connectedChanged()), this, SLOT(onConnectedChanged()));

    connect(server, SIGNAL(message(const WBroadcastMessage &)),
            this,   SLOT(onMessage(const WBroadcastMessage &)));

    emit serverChanged();
}

//-------------------------------------------------------------------------------------------------

QString WDeclarativePlayer::source() const
{
    Q_D(const WDeclarativePlayer);

    if (d->backend)
    {
         return d->backendInterface->source();
    }
    else return d->source;
}

void WDeclarativePlayer::setSource(const QString & url)
{
    Q_D(WDeclarativePlayer);

    if (d->backend)
    {
        if (d->backendInterface->source() == url) return;

        d->clearPlaylistAndTabs();

        d->loadSource(url, -1, -1);
    }
    else if (d->source != url)
    {
        d->source = url;

        emit sourceChanged();
    }
}

//-------------------------------------------------------------------------------------------------

WPlaylist * WDeclarativePlayer::playlist() const
{
    Q_D(const WDeclarativePlayer); return d->playlist;
}

void WDeclarativePlayer::setPlaylist(WPlaylist * playlist)
{
    Q_D(WDeclarativePlayer);

    if (d->playlist == playlist) return;

    d->clearPlaylistAndTabs();

    if (playlist)
    {
        if (playlist->parent() == NULL) playlist->setParent(this);

        connect(playlist, SIGNAL(countChanged()), this, SIGNAL(countChanged()));

        connect(playlist, SIGNAL(playlistUpdated()), this, SIGNAL(playlistUpdated()));

        connect(playlist, SIGNAL(currentTrackUpdated()), this, SIGNAL(currentTrackUpdated()));

        connect(playlist, SIGNAL(currentTrackChanged()), this, SLOT(onCurrentTrackChanged()));

        connect(playlist, SIGNAL(destroyed()), this, SLOT(onPlaylistDestroyed()));
    }

    d->applyPlaylist(playlist);
}

//-------------------------------------------------------------------------------------------------

WAbstractBackend::State WDeclarativePlayer::state() const
{
    Q_D(const WDeclarativePlayer); return d->state;
}

//-------------------------------------------------------------------------------------------------

bool WDeclarativePlayer::isLoading() const
{
    Q_D(const WDeclarativePlayer);

    if (d->backend)
    {
         return d->backend->isLoading();
    }
    else return false;
}

//-------------------------------------------------------------------------------------------------

bool WDeclarativePlayer::isDefault() const
{
    Q_D(const WDeclarativePlayer);

    if (d->backend)
    {
         return d->backend->isDefault();
    }
    else return false;
}

bool WDeclarativePlayer::isStarting() const
{
    Q_D(const WDeclarativePlayer);

    if (d->backend)
    {
         return d->backend->isStarting();
    }
    else return false;
}

bool WDeclarativePlayer::isResuming() const
{
    Q_D(const WDeclarativePlayer);

    if (d->backend)
    {
         return d->backend->isResuming();
    }
    else return false;
}

bool WDeclarativePlayer::isBuffering() const
{
    Q_D(const WDeclarativePlayer);

    if (d->backend)
    {
         return d->backend->isBuffering();
    }
    else return false;
}

//-------------------------------------------------------------------------------------------------

bool WDeclarativePlayer::isPlaying() const
{
    Q_D(const WDeclarativePlayer); return (d->state == WAbstractBackend::StatePlaying);
}

bool WDeclarativePlayer::isPaused() const
{
    Q_D(const WDeclarativePlayer); return (d->state == WAbstractBackend::StatePaused);
}

bool WDeclarativePlayer::isStopped() const
{
    Q_D(const WDeclarativePlayer); return (d->state == WAbstractBackend::StateStopped);
}

//-------------------------------------------------------------------------------------------------

bool WDeclarativePlayer::isVideo() const
{
    Q_D(const WDeclarativePlayer);

    if (d->backend)
    {
         return d->backendInterface->sourceIsVideo();
    }
    else return false;
}

bool WDeclarativePlayer::isAudio() const
{
    Q_D(const WDeclarativePlayer);

    if (d->backend)
    {
         return d->backendInterface->sourceIsAudio();
    }
    else return false;
}

//-------------------------------------------------------------------------------------------------

bool WDeclarativePlayer::isLive() const
{
    Q_D(const WDeclarativePlayer);

    if (d->backend)
    {
         return d->backend->isLive();
    }
    else return false;
}

//-------------------------------------------------------------------------------------------------

bool WDeclarativePlayer::hasStarted() const
{
    Q_D(const WDeclarativePlayer);

    if (d->backend)
    {
         return d->backend->hasStarted();
    }
    else return false;
}

bool WDeclarativePlayer::hasEnded() const
{
    Q_D(const WDeclarativePlayer);

    if (d->backend)
    {
         return d->backend->hasEnded();
    }
    else return false;
}

//-------------------------------------------------------------------------------------------------

bool WDeclarativePlayer::hasOutput() const
{
    return (outputType() != WAbstractBackend::OutputDefault);
}

//-------------------------------------------------------------------------------------------------

int WDeclarativePlayer::currentTime() const
{
    Q_D(const WDeclarativePlayer);

    if (d->backend)
    {
         return d->backend->currentTime();
    }
    else return -1;
}

int WDeclarativePlayer::duration() const
{
    Q_D(const WDeclarativePlayer);

    if (d->backend)
    {
         return d->backend->duration();
    }
    else return -1;
}

//-------------------------------------------------------------------------------------------------

qreal WDeclarativePlayer::progress() const
{
    Q_D(const WDeclarativePlayer);

    if (d->backend)
    {
         return d->backend->progress();
    }
    else return 0.0;
}

//-------------------------------------------------------------------------------------------------

qreal WDeclarativePlayer::speed() const
{
    Q_D(const WDeclarativePlayer);

    if (d->backend)
    {
        return d->backend->speed();
    }
    else return d->speed;
}

void WDeclarativePlayer::setSpeed(qreal speed)
{
    Q_D(WDeclarativePlayer);

    if (d->backend)
    {
        d->backend->setSpeed(speed);
    }
    else if (d->speed == speed)
    {
        d->speed = speed;

        emit speedChanged();
    }
}

//-------------------------------------------------------------------------------------------------

qreal WDeclarativePlayer::volume() const
{
    Q_D(const WDeclarativePlayer);

    if (d->backend)
    {
        return d->backend->volume();
    }
    else return d->volume;
}

void WDeclarativePlayer::setVolume(qreal volume)
{
    Q_D(WDeclarativePlayer);

    if (d->backend)
    {
        d->backend->setVolume(volume);
    }
    else if (d->volume == volume)
    {
        d->volume = volume;

        emit volumeChanged();
    }
}

//-------------------------------------------------------------------------------------------------

bool WDeclarativePlayer::autoPlay() const
{
    Q_D(const WDeclarativePlayer); return d->autoPlay;
}

void WDeclarativePlayer::setAutoPlay(bool autoPlay)
{
    Q_D(WDeclarativePlayer);

    if (d->autoPlay == autoPlay) return;

    d->autoPlay = autoPlay;

    emit autoPlayChanged();
}

//-------------------------------------------------------------------------------------------------

bool WDeclarativePlayer::shuffle() const
{
    Q_D(const WDeclarativePlayer); return d->shuffle;
}

void WDeclarativePlayer::setShuffle(bool shuffle)
{
    Q_D(WDeclarativePlayer);

    if (d->shuffle == shuffle) return;

    d->shuffle = shuffle;

    if (shuffle) d->resetShuffle();
    else         d->clearShuffle();

    emit shuffleChanged();

    emit playlistUpdated();
}

//-------------------------------------------------------------------------------------------------

WDeclarativePlayer::Repeat WDeclarativePlayer::repeat() const
{
    Q_D(const WDeclarativePlayer); return d->repeat;
}

void WDeclarativePlayer::setRepeat(Repeat repeat)
{
    Q_D(WDeclarativePlayer);

    if (d->repeat == repeat)

    d->repeat = repeat;

    if (d->backend)
    {
        d->updateRepeat();
    }
    else emit repeatChanged();

    emit playlistUpdated();
}

//-------------------------------------------------------------------------------------------------

WAbstractBackend::Output WDeclarativePlayer::output() const
{
    Q_D(const WDeclarativePlayer);

    if (d->backend)
    {
        return d->backend->output();
    }
    else return d->output;
}

void WDeclarativePlayer::setOutput(WAbstractBackend::Output output)
{
    Q_D(WDeclarativePlayer);

    if (d->backend)
    {
        d->backend->setOutput(output);
    }
    else if (d->output == output)
    {
        d->output = output;

        emit outputChanged();
    }
}

//-------------------------------------------------------------------------------------------------

WAbstractBackend::Quality WDeclarativePlayer::quality() const
{
    Q_D(const WDeclarativePlayer);

    if (d->backend)
    {
        return d->backend->quality();
    }
    else return d->quality;
}

void WDeclarativePlayer::setQuality(WAbstractBackend::Quality quality)
{
    Q_D(WDeclarativePlayer);

    if (d->backend)
    {
        d->backend->setQuality(quality);
    }
    else if (d->quality == quality)
    {
        d->quality = quality;

        emit qualityChanged();
    }
}

//-------------------------------------------------------------------------------------------------

WAbstractBackend::Output WDeclarativePlayer::outputActive() const
{
    Q_D(const WDeclarativePlayer);

    if (d->backend)
    {
         return d->backend->outputActive();
    }
    else return WAbstractBackend::OutputNone;
}

WAbstractBackend::Quality WDeclarativePlayer::qualityActive() const
{
    Q_D(const WDeclarativePlayer);

    if (d->backend)
    {
         return d->backend->qualityActive();
    }
    else return WAbstractBackend::QualityDefault;
}

//-------------------------------------------------------------------------------------------------

WAbstractBackend::FillMode WDeclarativePlayer::fillMode() const
{
    Q_D(const WDeclarativePlayer);

    if (d->backend)
    {
        return d->backend->fillMode();
    }
    else return d->fillMode;
}

void WDeclarativePlayer::setFillMode(WAbstractBackend::FillMode fillMode)
{
    Q_D(WDeclarativePlayer);

    if (d->backend)
    {
        d->backend->setFillMode(fillMode);
    }
    else if (d->fillMode == fillMode)
    {
        d->fillMode = fillMode;

        emit fillModeChanged();
    }

#if defined(QT_NEW) && defined(SK_SOFTWARE) == false
    d->frameUpdate = true;
#endif

    update();
}

//-------------------------------------------------------------------------------------------------

int WDeclarativePlayer::trackVideo() const
{
    Q_D(const WDeclarativePlayer);

    if (d->backend)
    {
        return d->backend->trackVideo();
    }
    else return d->trackVideo;
}

void WDeclarativePlayer::setTrackVideo(int id)
{
    Q_D(WDeclarativePlayer);

    if (d->backend)
    {
        d->backend->setTrackVideo(id);
    }
    else if (d->trackVideo == id)
    {
        d->trackVideo = id;

        emit trackVideoChanged();
    }
}

int WDeclarativePlayer::trackAudio() const
{
    Q_D(const WDeclarativePlayer);

    if (d->backend)
    {
        return d->backend->trackAudio();
    }
    else return d->trackAudio;
}

void WDeclarativePlayer::setTrackAudio(int id)
{
    Q_D(WDeclarativePlayer);

    if (d->backend)
    {
        d->backend->setTrackAudio(id);
    }
    else if (d->trackAudio == id)
    {
        d->trackAudio = id;

        emit trackAudioChanged();
    }
}

int WDeclarativePlayer::countVideos() const
{
    Q_D(const WDeclarativePlayer);

    if (d->backend)
    {
        return d->backend->countVideos();
    }
    else return 0;
}

int WDeclarativePlayer::countAudios() const
{
    Q_D(const WDeclarativePlayer);

    if (d->backend)
    {
        return d->backend->countAudios();
    }
    else return 0;
}

//-------------------------------------------------------------------------------------------------

bool WDeclarativePlayer::scanOutput() const
{
    Q_D(const WDeclarativePlayer);

    if (d->backend)
    {
        return d->backend->scanOutput();
    }
    else return d->scanOutput;
}

void WDeclarativePlayer::setScanOutput(bool enabled)
{
    Q_D(WDeclarativePlayer);

    if (d->backend)
    {
        d->backend->setScanOutput(enabled);
    }
    else if (d->scanOutput == enabled)
    {
        d->scanOutput = enabled;

        emit scanOutputChanged();
    }
}

int WDeclarativePlayer::currentOutput() const
{
    Q_D(const WDeclarativePlayer);

    if (d->backend)
    {
        return d->backend->currentOutput();
    }
    else return d->currentOutput;
}

void WDeclarativePlayer::setCurrentOutput(int index)
{
    Q_D(WDeclarativePlayer);

    if (d->backend)
    {
        d->backend->setCurrentOutput(index);
    }
    else if (d->currentOutput == index)
    {
        d->currentOutput = index;

        emit currentOutputChanged();
    }
}

QString WDeclarativePlayer::outputName() const
{
    Q_D(const WDeclarativePlayer);

    if (d->backend)
    {
        return d->backend->outputName();
    }
    else return QString();
}

WAbstractBackend::OutputType WDeclarativePlayer::outputType() const
{
    Q_D(const WDeclarativePlayer);

    if (d->backend)
    {
        return d->backend->outputType();
    }
    else return WAbstractBackend::OutputDefault;
}

int WDeclarativePlayer::countOutputs() const
{
    Q_D(const WDeclarativePlayer);

    if (d->backend)
    {
        return d->backend->countOutputs();
    }
    else return 0;
}

//-------------------------------------------------------------------------------------------------

QString WDeclarativePlayer::subtitle() const
{
    Q_D(const WDeclarativePlayer);

    if (d->backend)
    {
        return d->backend->subtitle();
    }
    else return d->subtitle;
}

void WDeclarativePlayer::setSubtitle(const QString & subtitle)
{
    Q_D(WDeclarativePlayer);

    if (d->backend)
    {
        d->backend->setSubtitle(subtitle);

        if (d->tab) d->tab->setSubtitle(subtitle);
    }
    else if (d->subtitle == subtitle)
    {
        d->subtitle = subtitle;

        if (d->tab) d->tab->setSubtitle(subtitle);

        emit subtitleChanged();
    }
}

//-------------------------------------------------------------------------------------------------

int WDeclarativePlayer::pauseTimeout() const
{
    Q_D(const WDeclarativePlayer);

    return d->pauseTimeout;
}

void WDeclarativePlayer::setPauseTimeout(int msec)
{
    Q_D(WDeclarativePlayer);

    if (d->pauseTimeout == msec) return;

    if (d->pauseTimeout > -1 && d->state == WAbstractBackend::StatePaused)
    {
        d->timer.stop();
    }

    d->pauseTimeout = msec;

    if (msec > -1) d->timer.setInterval(msec);

    if (d->state == WAbstractBackend::StatePaused)
    {
        d->timer.start();
    }

    emit pauseTimeoutChanged();
}

//-------------------------------------------------------------------------------------------------

int WDeclarativePlayer::count() const
{
    Q_D(const WDeclarativePlayer);

    if (d->playlist)
    {
         return d->playlist->count();
    }
    else return -1;
}

//-------------------------------------------------------------------------------------------------

bool WDeclarativePlayer::hasPreviousTrack() const
{
    Q_D(const WDeclarativePlayer);

    if (d->repeat == WDeclarativePlayer::RepeatAll)
    {
        return (count() > 1);
    }
    else if (d->shuffle)
    {
        return (d->shuffleIndex > 0);
    }
    else if (d->tab)
    {
        return d->tab->hasPreviousTrack();
    }
    else if (d->playlist)
    {
        return d->playlist->hasPreviousTrack();
    }
    else return false;
}

bool WDeclarativePlayer::hasNextTrack() const
{
    Q_D(const WDeclarativePlayer);

    if (d->repeat == WDeclarativePlayer::RepeatAll)
    {
        return (count() > 1);
    }
    else if (d->shuffle)
    {
        if (d->shuffleIndex == -1
            ||
            (d->shuffleTracks.isEmpty() && d->shuffleIndex == d->shuffleHistory.count() - 1))
        {
             return false;
        }
        else return true;
    }
    else if (d->tab)
    {
        return d->tab->hasNextTrack();
    }
    else if (d->playlist)
    {
        return d->playlist->hasNextTrack();
    }
    else return false;
}

//-------------------------------------------------------------------------------------------------

WTrack::State WDeclarativePlayer::trackState() const
{
    Q_D(const WDeclarativePlayer);

    if (d->playlist)
    {
        const WTrack * track = static_cast<const WTrack *> (d->playlist->currentTrackPointer());

        if (track)
        {
             return track->state();
        }
        else return WTrack::Default;
    }
    else return WTrack::Default;
}

bool WDeclarativePlayer::trackIsDefault() const
{
    return (trackState() == WTrack::Default);
}

bool WDeclarativePlayer::trackIsLoading() const
{
    return (trackState() == WTrack::Loading);
}

bool WDeclarativePlayer::trackIsLoaded() const
{
    return (trackState() >= WTrack::Loaded);
}

//-------------------------------------------------------------------------------------------------

QString WDeclarativePlayer::trackTitle() const
{
    Q_D(const WDeclarativePlayer);

    if (d->tab)
    {
        return d->tab->title();
    }
    else if (d->playlist)
    {
        return d->playlist->currentTitle();
    }
    else return QString();
}

QString WDeclarativePlayer::trackCover() const
{
    Q_D(const WDeclarativePlayer);

    if (d->tab)
    {
        return d->tab->cover();
    }
    else if (d->playlist)
    {
        return d->playlist->currentCover();
    }
    else return QString();
}

//-------------------------------------------------------------------------------------------------

int WDeclarativePlayer::trackCurrentTime() const
{
    Q_D(const WDeclarativePlayer);

    if (d->tab)
    {
         return d->tab->currentTime();
    }
    else return -1;
}

int WDeclarativePlayer::trackDuration() const
{
    Q_D(const WDeclarativePlayer);

    if (d->tab)
    {
        return d->tab->duration();
    }
    else if (d->playlist)
    {
        return d->playlist->currentDuration();
    }
    else return -1;
}

//-------------------------------------------------------------------------------------------------

int WDeclarativePlayer::trackIndex() const
{
    Q_D(const WDeclarativePlayer);

    if (d->tab)
    {
        return d->tab->trackIndex();
    }
    else if (d->playlist)
    {
        return d->playlist->currentIndex();
    }
    else return -1;
}

//-------------------------------------------------------------------------------------------------

WTabsTrack * WDeclarativePlayer::tabs() const
{
    Q_D(const WDeclarativePlayer); return d->tabs;
}

void WDeclarativePlayer::setTabs(WTabsTrack * tabs)
{
    Q_D(WDeclarativePlayer);

    if (d->tabs == tabs) return;

    d->clearPlaylistAndTabs();

    d->tabs = tabs;

    if (d->tabs)
    {
        connect(d->tabs, SIGNAL(currentIndexChanged()), this, SIGNAL(tabIndexChanged()));

        connect(d->tabs, SIGNAL(currentTabChanged    ()), this, SLOT(onCurrentTabChanged    ()));
        connect(d->tabs, SIGNAL(highlightedTabChanged()), this, SLOT(onHighlightedTabChanged()));

        connect(d->tabs, SIGNAL(destroyed()), this, SLOT(onTabsDestroyed()));
    }

    emit tabsChanged();

    if (d->tabs) d->onCurrentTabChanged();
}

//-------------------------------------------------------------------------------------------------

WTabTrack * WDeclarativePlayer::tab() const
{
    Q_D(const WDeclarativePlayer); return d->tab;
}

int WDeclarativePlayer::tabIndex() const
{
    Q_D(const WDeclarativePlayer);

    if (d->tabs)
    {
        if (d->tabs->highlightedTab())
        {
             return d->tabs->highlightedIndex();
        }
        else return d->tabs->currentIndex();
    }
    else return -1;
}

//-------------------------------------------------------------------------------------------------

bool WDeclarativePlayer::videoTag() const
{
    Q_D(const WDeclarativePlayer); return d->videoTag;
}

void WDeclarativePlayer::setVideoTag(bool enabled)
{
    Q_D(WDeclarativePlayer);

    if (d->videoTag == enabled) return;

    d->videoTag = enabled;

    emit videoTagChanged();
}

#endif // SK_NO_DECLARATIVEPLAYER
