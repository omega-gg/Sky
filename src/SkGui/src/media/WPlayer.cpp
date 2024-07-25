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

#include "WPlayer.h"

#ifndef SK_NO_PLAYER

// Qt includes
#ifdef QT_4
#include <QApplication>
#include <QDesktopWidget>
#endif

// Sk includes
#include <WControllerApplication>
#include <WControllerPlaylist>
#include <WBroadcastServer>
#include <WAbstractHook>
#include <WTabsTrack>
#include <WTabTrack>
#ifndef SK_NO_QML
#include <WView>
#include <WDeclarativePlayer>
#endif

//-------------------------------------------------------------------------------------------------
// Inline functions
//-------------------------------------------------------------------------------------------------

inline bool sortChapter(const WChapter & chapterA, const WChapter & chapterB)
{
    return (chapterA.time() < chapterB.time());
}

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

WPlayerPrivate::WPlayerPrivate(WPlayer * p) : WPrivate(p) {}

/* virtual */ WPlayerPrivate::~WPlayerPrivate()
{
    if (backend) backend->deleteBackend();
}

//-------------------------------------------------------------------------------------------------

void WPlayerPrivate::init()
{
    Q_Q(WPlayer);

#ifndef SK_NO_QML
    view = NULL;
#endif

    backend = NULL;
    hook    = NULL;

    backendInterface = NULL;

    server = NULL;

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

    repeat = WPlayer::RepeatNone;

    output  = WAbstractBackend::OutputMedia;
    quality = WAbstractBackend::QualityDefault;
    mode    = WAbstractBackend::SourceDefault;

    fillMode = WAbstractBackend::PreserveAspectFit;

    trackVideo = -1;
    trackAudio = -1;

    scanOutput = false;

    currentOutput = 0;

    pauseTimeout = -1;

    keepState = false;

    videoTag = false;

    timer.setSingleShot(true);

    QObject::connect(q, SIGNAL(playlistChanged()), q, SIGNAL(playlistUpdated()));
    QObject::connect(q, SIGNAL(playlistChanged()), q, SIGNAL(countChanged   ()));

    QObject::connect(&timer, SIGNAL(timeout()), q, SLOT(stop()));
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WPlayerPrivate::applyPlaylist(WPlaylist * playlist)
{
    Q_Q(WPlayer);

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

void WPlayerPrivate::loadSource(const QString & url, int duration, int currentTime)
{
    bool isPlaying = backend->isPlaying();

    updateBackend(url, isPlaying);

    backendInterface->loadSource(url, duration, currentTime);

    if (isPlaying) backendInterface->play();

    updateShuffle();
}

bool WPlayerPrivate::updateBackend(const QString & url, bool isPlaying)
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
        if (isPlaying)
        {
            // NOTE: We have to freeze the state to avoid clearing highlightedTab.
            keepState = true;

            backendInterface->clear();

            keepState = false;

            setBackendInterface(currentBackend, currentHook);
        }
        else
        {
            backendInterface->clear();

            setBackendInterface(currentBackend, currentHook);
        }
    }
    else setBackendInterface(currentBackend, currentHook);

    return true;
}

void WPlayerPrivate::stop()
{
    if (tab) tab->setCurrentTime(-1);

    backendInterface->stop();
}

//-------------------------------------------------------------------------------------------------

void WPlayerPrivate::updateRepeat(WTrack::Type type)
{
    if (backend == NULL) return;

    if (repeat == WPlayer::RepeatOne
        ||
        (repeat == WPlayer::RepeatAll && playlist == NULL)
        ||
        type == WTrack::Hub)
    {
         backend->setRepeat(true);
    }
    else backend->setRepeat(false);
}

//-------------------------------------------------------------------------------------------------

void WPlayerPrivate::updateShuffle()
{
    if (shuffle == false || shuffleLock) return;

    Q_Q(WPlayer);

    resetShuffle();

    emit q->playlistUpdated();
}

void WPlayerPrivate::resetShuffle()
{
    Q_Q(WPlayer);

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

void WPlayerPrivate::clearShuffle()
{
    shuffleHistory.clear();
    shuffleTracks .clear();

    shuffleIndex = -1;
}

//-------------------------------------------------------------------------------------------------

void WPlayerPrivate::clearPlaylistAndTabs()
{
    Q_Q(WPlayer);

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

#ifndef SK_NO_QML

WView * WPlayerPrivate::getView() const
{
    if (view)
    {
        return view->view();
    }
    else return NULL;
}

#endif

//-------------------------------------------------------------------------------------------------

void WPlayerPrivate::setBackendInterface(WBackendInterface * currentBackend,
                                         WAbstractHook     * currentHook)
{
    if (backendInterface == currentBackend) return;

    Q_Q(WPlayer);

    if (hook) QObject::disconnect(hook,    SIGNAL(sourceChanged()), q, SIGNAL(sourceChanged()));
    else      QObject::disconnect(backend, SIGNAL(sourceChanged()), q, SIGNAL(sourceChanged()));

    backendInterface = currentBackend;

    hook = currentHook;

    if (hook) QObject::connect(hook,    SIGNAL(sourceChanged()), q, SIGNAL(sourceChanged()));
    else      QObject::connect(backend, SIGNAL(sourceChanged()), q, SIGNAL(sourceChanged()));
}

void WPlayerPrivate::setPlaylist(WPlaylist * playlist)
{
    if (this->playlist == playlist) return;

    if (shuffle && this->playlist)
    {
        Q_Q(WPlayer);

        this->playlist->unregisterWatcher(q);
    }

    applyPlaylist(playlist);
}

void WPlayerPrivate::setTab(WTabTrack * tab)
{
    if (this->tab == tab) return;

    Q_Q(WPlayer);

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

void WPlayerPrivate::setShuffleTrack(const WTrack * track)
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

void WPlayerPrivate::onEnded()
{
    if (autoPlay == false)
    {
        Q_Q(WPlayer);

        stop();

        emit q->ended();

        return;
    }

    if (repeat != WPlayer::RepeatOne)
    {
        Q_Q(WPlayer);

        if (q->hasNextTrack() == false)
        {
            if (playlist && repeat == WPlayer::RepeatAll)
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

void WPlayerPrivate::onError()
{
    // NOTE: We want to save the current state before stopping the backend.
    if (tab) tab->setPlayer(NULL);
}

//-------------------------------------------------------------------------------------------------

void WPlayerPrivate::onHookUpdated()
{
    Q_Q(WPlayer);

    bool isPlaying = backend->isPlaying();

    QString source = q->source();

    int duration    = q->duration   ();
    int currentTime = q->currentTime();

    // NOTE: When the backendInterface stays the same we don't do anything.
    if (updateBackend(source, isPlaying) == false) return;

    backendInterface->loadSource(source, duration, currentTime);

    if (isPlaying) backendInterface->play();

    updateShuffle();
}

//-------------------------------------------------------------------------------------------------

void WPlayerPrivate::onStateChanged()
{
    if (keepState) return;

    Q_Q(WPlayer);

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

void WPlayerPrivate::onDurationChanged()
{
    if (tab) tab->setDuration(backend->duration());
}

//-------------------------------------------------------------------------------------------------

void WPlayerPrivate::onCurrentTrackChanged()
{
    Q_Q(WPlayer);

    const WTrack * track = playlist->currentTrackPointer();

    loadSource(track->source(), track->duration(), -1);

    emit q->currentTrackUpdated();
}

//-------------------------------------------------------------------------------------------------

void WPlayerPrivate::onCurrentTabChanged()
{
    Q_Q(WPlayer);

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

void WPlayerPrivate::onHighlightedTabChanged()
{
    WTabTrack * tab = tabs->highlightedTab();

    if (tab)
    {
         setTab(tab);
    }
    else setTab(tabs->currentTabTrack());
}

//-------------------------------------------------------------------------------------------------

void WPlayerPrivate::onCurrentBookmarkChanged()
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

void WPlayerPrivate::onCurrentBookmarkUpdated()
{
    Q_Q(WPlayer);

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

    updateRepeat(tab->type());

    emit q->currentTrackUpdated();
}

//-------------------------------------------------------------------------------------------------

void WPlayerPrivate::onConnectedChanged()
{
    Q_Q(WPlayer);

    q->stop();

    if (backend == NULL) return;

    if (server->isConnected())
    {
        server->sendReply(WBroadcastReply::VOLUME, QString::number(backend->volume()));

#ifndef SK_NO_QML
        WView * view = getView();

        if (view)
        {
            onScreen();

            server->sendReply(WBroadcastReply::FULLSCREEN, QString::number(view->isFullScreen()));

            QObject::connect(view, SIGNAL(availableGeometryChanged()), q, SLOT(onScreen    ()));
            QObject::connect(view, SIGNAL(fullScreenChanged       ()), q, SLOT(onFullScreen()));
        }
#endif

        server->sendReply(WBroadcastReply::VIDEOTAG, QString::number(videoTag));

        server->sendReply(WBroadcastReply::CLEAR);

#ifdef Q_OS_WIN
        // FIXME: Currently, runOnStartup is only supported on Windows.
        server->sendReply(WBroadcastReply::STARTUP, QString::number(sk->runOnStartup()));
#endif

#ifdef SK_DESKTOP
        server->sendReply(WBroadcastReply::SHUTDOWN);
#endif

#ifndef SK_NO_QML
#ifdef QT_4
        QObject::connect(qApp->desktop(), SIGNAL(screenCountChanged()), q, SLOT(onScreen()));
#else
        QObject::connect(qApp, SIGNAL(screenAdded  (QScreen *)), q, SLOT(onScreen()));
        QObject::connect(qApp, SIGNAL(screenRemoved(QScreen *)), q, SLOT(onScreen()));
#endif
#endif

#ifdef SK_DESKTOP
        QObject::connect(sk, SIGNAL(runOnStartupChanged()), q, SLOT(onStartup()));
#endif

        QObject::connect(q, SIGNAL(sourceChanged()), q, SLOT(onSource()));

        QObject::connect(backend, SIGNAL(stateChanged        ()), q, SLOT(onState      ()));
        QObject::connect(backend, SIGNAL(stateLoadChanged    ()), q, SLOT(onStateLoad  ()));
        QObject::connect(backend, SIGNAL(vbmlChanged         ()), q, SLOT(onVbml       ()));
        QObject::connect(backend, SIGNAL(liveChanged         ()), q, SLOT(onLive       ()));
        QObject::connect(backend, SIGNAL(startedChanged      ()), q, SLOT(onStart      ()));
        QObject::connect(backend, SIGNAL(endedChanged        ()), q, SLOT(onEnd        ()));
        QObject::connect(backend, SIGNAL(currentTimeChanged  ()), q, SLOT(onCurrentTime()));
        QObject::connect(backend, SIGNAL(durationChanged     ()), q, SLOT(onDuration   ()));
        QObject::connect(backend, SIGNAL(progressChanged     ()), q, SLOT(onProgress   ()));
        QObject::connect(backend, SIGNAL(outputActiveChanged ()), q, SLOT(onOutput     ()));
        QObject::connect(backend, SIGNAL(qualityActiveChanged()), q, SLOT(onQuality    ()));
        QObject::connect(backend, SIGNAL(contextChanged      ()), q, SLOT(onContext    ()));
        QObject::connect(backend, SIGNAL(chaptersChanged     ()), q, SLOT(onChapters   ()));
        QObject::connect(backend, SIGNAL(videosChanged       ()), q, SLOT(onVideos     ()));
        QObject::connect(backend, SIGNAL(audiosChanged       ()), q, SLOT(onAudios     ()));
        QObject::connect(backend, SIGNAL(ambientChanged      ()), q, SLOT(onAmbient    ()));
        QObject::connect(backend, SIGNAL(subtitlesChanged    ()), q, SLOT(onSubtitles  ()));
    }
    else
    {
#ifndef SK_NO_QML
        QObject::disconnect(qApp, NULL, q, NULL);
#endif

        QObject::disconnect(sk, NULL, q, NULL);

#ifndef SK_NO_QML
        WView * view = getView();

        if (view) QObject::disconnect(view, NULL, q, NULL);
#endif

        QObject::disconnect(q, SIGNAL(sourceChanged()), q, SLOT(onSource()));

        QObject::disconnect(backend, SIGNAL(stateChanged        ()), q, SLOT(onState      ()));
        QObject::disconnect(backend, SIGNAL(stateLoadChanged    ()), q, SLOT(onStateLoad  ()));
        QObject::disconnect(backend, SIGNAL(vbmlChanged         ()), q, SLOT(onVbml       ()));
        QObject::disconnect(backend, SIGNAL(liveChanged         ()), q, SLOT(onLive       ()));
        QObject::disconnect(backend, SIGNAL(startedChanged      ()), q, SLOT(onStart      ()));
        QObject::disconnect(backend, SIGNAL(endedChanged        ()), q, SLOT(onEnd        ()));
        QObject::disconnect(backend, SIGNAL(currentTimeChanged  ()), q, SLOT(onCurrentTime()));
        QObject::disconnect(backend, SIGNAL(durationChanged     ()), q, SLOT(onDuration   ()));
        QObject::disconnect(backend, SIGNAL(progressChanged     ()), q, SLOT(onProgress   ()));
        QObject::disconnect(backend, SIGNAL(outputActiveChanged ()), q, SLOT(onOutput     ()));
        QObject::disconnect(backend, SIGNAL(qualityActiveChanged()), q, SLOT(onQuality    ()));
        QObject::disconnect(backend, SIGNAL(contextChanged      ()), q, SLOT(onContext    ()));
        QObject::disconnect(backend, SIGNAL(chaptersChanged     ()), q, SLOT(onChapters   ()));
        QObject::disconnect(backend, SIGNAL(videosChanged       ()), q, SLOT(onVideos     ()));
        QObject::disconnect(backend, SIGNAL(audiosChanged       ()), q, SLOT(onAudios     ()));
        QObject::disconnect(backend, SIGNAL(ambientChanged      ()), q, SLOT(onAmbient    ()));
        QObject::disconnect(backend, SIGNAL(subtitlesChanged    ()), q, SLOT(onSubtitles  ()));
    }
}

void WPlayerPrivate::onMessage(const WBroadcastMessage & message)
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

            Q_Q(WPlayer);

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

            playlistServer->setCurrentIndex(0);

            // NOTE: Removing the previous track after setting the new one.
            if (playlistServer->count() > 1)
            {
                playlistServer->removeTrack(1);
            }

            // NOTE: We restore the tab subtitle manually.
            q->setSubtitle(subtitle);

            // NOTE: We load the track at the end because backend instanciation might delay the
            //       prior calls. This can cause message events to be processed in the wrong order.
            playlistServer->loadTrack(0);
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
                Q_Q(WPlayer);

                q->seek(currentTime);

                return;
            }

            clearPlaylistAndTabs();

            loadSource(url, parameters.at(1).toInt(), currentTime);
        }
        else
        {
            Q_Q(WPlayer);

            q->setSource(url);
        }
    }
    else if (type == WBroadcastMessage::PLAY)
    {
        Q_Q(WPlayer);

        q->play();
    }
    else if (type == WBroadcastMessage::REPLAY)
    {
        Q_Q(WPlayer);

        q->replay();
    }
    else if (type == WBroadcastMessage::PAUSE)
    {
        Q_Q(WPlayer);

        q->pause();
    }
    else if (type == WBroadcastMessage::STOP)
    {
        Q_Q(WPlayer);

        q->stop();
    }
    else if (type == WBroadcastMessage::SEEK)
    {
        Q_Q(WPlayer);

        q->seek(message.parameters.first().toInt());
    }
    else if (type == WBroadcastMessage::OUTPUT)
    {
        Q_Q(WPlayer);

        q->setOutput(WAbstractBackend::outputFromString(message.parameters.first()));
    }
    else if (type == WBroadcastMessage::QUALITY)
    {
        Q_Q(WPlayer);

        q->setQuality(WAbstractBackend::qualityFromString(message.parameters.first()));
    }
    else if (type == WBroadcastMessage::MODE)
    {
        Q_Q(WPlayer);

        q->setSourceMode(WAbstractBackend::modeFromString(message.parameters.first()));
    }
    else if (type == WBroadcastMessage::FILLMODE)
    {
        Q_Q(WPlayer);

        q->setFillMode(WAbstractBackend::fillModeFromString(message.parameters.first()));
    }
    else if (type == WBroadcastMessage::SPEED)
    {
        Q_Q(WPlayer);

        q->setSpeed(message.parameters.first().toFloat());
    }
    else if (type == WBroadcastMessage::VIDEO)
    {
        Q_Q(WPlayer);

        q->setTrackVideo(message.parameters.first().toInt());
    }
    else if (type == WBroadcastMessage::AUDIO)
    {
        Q_Q(WPlayer);

        q->setTrackAudio(message.parameters.first().toInt());
    }
    else if (type == WBroadcastMessage::SUBTITLE)
    {
        Q_Q(WPlayer);

        q->setSubtitle(message.parameters.first());
    }
    else if (type == WBroadcastMessage::VOLUME)
    {
        Q_Q(WPlayer);

        q->setVolume(message.parameters.first().toFloat());
    }
#ifndef SK_NO_QML
    else if (type == WBroadcastMessage::SCREEN)
    {
        WView * view = getView();

        if (view == NULL) return;

        view->moveToScreen(message.parameters.first().toInt());

        // NOTE: Making sure the window is visible.
        view->raise();
    }
    else if (type == WBroadcastMessage::FULLSCREEN)
    {
        WView * view = getView();

        if (view == NULL) return;

        bool fullScreen = message.parameters.first().toInt();

        // NOTE: Making sure the window is visible.
        if (fullScreen) view->raise();

        view->setFullScreen(fullScreen);
    }
#endif
    else if (type == WBroadcastMessage::VIDEOTAG)
    {
        Q_Q(WPlayer);

        q->setVideoTag(message.parameters.first().toInt());
    }
    else if (type == WBroadcastMessage::CLEAR)
    {
        Q_Q(WPlayer);

        emit q->clearCache();
    }
#ifdef SK_DESKTOP
    else if (type == WBroadcastMessage::STARTUP)
    {
        sk->setRunOnStartup(message.parameters.first().toInt());
    }
    else if (type == WBroadcastMessage::SHUTDOWN)
    {
        sk->shutdown();
    }
#endif
}

void WPlayerPrivate::onSource()
{
    // NOTE: We must call WBackendInterface::source to retrieve the proper source.
    server->sendReply(WBroadcastReply::SOURCE, backendInterface->source());
}

void WPlayerPrivate::onState()
{
    server->sendReply(WBroadcastReply::STATE,
                      WAbstractBackend::stateToString(backend->state()));
}

void WPlayerPrivate::onStateLoad()
{
    server->sendReply(WBroadcastReply::STATELOAD,
                      WAbstractBackend::stateLoadToString(backend->stateLoad()));
}

void WPlayerPrivate::onVbml()
{
    server->sendReply(WBroadcastReply::VBML, QString::number(backend->isVbml()));
}

void WPlayerPrivate::onLive()
{
    server->sendReply(WBroadcastReply::LIVE, QString::number(backend->isLive()));
}

void WPlayerPrivate::onStart()
{
    server->sendReply(WBroadcastReply::STARTED, QString::number(backend->hasStarted()));
}

void WPlayerPrivate::onEnd()
{
    server->sendReply(WBroadcastReply::ENDED, QString::number(backend->hasEnded()));
}

void WPlayerPrivate::onCurrentTime()
{
    server->sendReply(WBroadcastReply::TIME, QString::number(backend->currentTime()));
}

void WPlayerPrivate::onDuration()
{
    server->sendReply(WBroadcastReply::DURATION, QString::number(backend->duration()));
}

void WPlayerPrivate::onProgress()
{
    server->sendReply(WBroadcastReply::PROGRESS, QString::number(backend->progress()));
}

void WPlayerPrivate::onOutput()
{
    server->sendReply(WBroadcastReply::OUTPUT,
                      WAbstractBackend::outputToString(backend->outputActive()));
}

void WPlayerPrivate::onQuality()
{
    server->sendReply(WBroadcastReply::QUALITY,
                      WAbstractBackend::qualityToString(backend->qualityActive()));
}

void WPlayerPrivate::onContext()
{
    QStringList parameters;

    parameters.append(backend->context  ());
    parameters.append(backend->contextId());

    server->sendReply(WBroadcastReply::CONTEXT, parameters);
}

void WPlayerPrivate::onChapters()
{
    QStringList parameters;

    QList<WChapter> chapters = backend->chapters();

    foreach (const WChapter & chapter, chapters)
    {
        parameters.append(QString::number(chapter.time()));

        parameters.append(chapter.title());
        parameters.append(chapter.cover());
    }

    server->sendReply(WBroadcastReply::CHAPTERS, parameters);
}

void WPlayerPrivate::onVideos()
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

void WPlayerPrivate::onAudios()
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

void WPlayerPrivate::onAmbient()
{
    server->sendReply(WBroadcastReply::AMBIENT, backend->ambient());
}

void WPlayerPrivate::onSubtitles()
{
    server->sendReply(WBroadcastReply::SUBTITLES, backend->subtitles());
}

#ifndef SK_NO_QML

void WPlayerPrivate::onScreen()
{
    QStringList parameters;

    WView * view = getView();

    parameters.append(QString::number(view->screenNumber()));
    parameters.append(QString::number(view->screenCount ()));

    server->sendReply(WBroadcastReply::SCREEN, parameters);
}

void WPlayerPrivate::onFullScreen()
{
    WView * view = getView();

    server->sendReply(WBroadcastReply::FULLSCREEN, QString::number(view->isFullScreen()));
}

#endif

#ifdef SK_DESKTOP

void WPlayerPrivate::onStartup()
{
    server->sendReply(WBroadcastReply::STARTUP, QString::number(sk->runOnStartup()));
}

#endif

//-------------------------------------------------------------------------------------------------

void WPlayerPrivate::onHookDestroyed()
{
    Q_Q(WPlayer);

    WAbstractHook * hook = static_cast<WAbstractHook *> (q->sender());

    if (backendInterface == hook)
    {
        setBackendInterface(backend, NULL);
    }

    hooks.removeOne(hook);

    emit q->hooksChanged();
}

void WPlayerPrivate::onPlaylistDestroyed()
{
    Q_Q(WPlayer);

    playlist = NULL;

    emit q->playlistChanged();
}

void WPlayerPrivate::onFolderDestroyed()
{
    folder = NULL;
}

void WPlayerPrivate::onTabsDestroyed()
{
    Q_Q(WPlayer);

    clearPlaylistAndTabs();

    tabs = NULL;

    emit q->tabsChanged();
}

void WPlayerPrivate::onTabDestroyed()
{
    Q_Q(WPlayer);

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

/* explicit */ WPlayer::WPlayer(QObject * parent)
    : QObject(parent), WPrivatable(new WPlayerPrivate(this))
{
    Q_D(WPlayer); d->init();
}

//-------------------------------------------------------------------------------------------------
// Protected

WPlayer::WPlayer(WPlayerPrivate * p, QObject * parent)
    : QObject(parent), WPrivatable(p)
{
    Q_D(WPlayer); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WPlayer::play()
{
    Q_D(WPlayer);

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

/* Q_INVOKABLE */ void WPlayer::replay()
{
    Q_D(WPlayer);

    if (d->backend == NULL) return;

    if (d->tab) d->tab->setPlayer(this);

    d->backendInterface->replay();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WPlayer::pause()
{
    Q_D(WPlayer);

    if (d->backend) d->backendInterface->pause();
}

/* Q_INVOKABLE */ void WPlayer::stop()
{
    Q_D(WPlayer);

    if (d->backend == NULL) return;

    if (d->tab) d->tab->setPlayer(NULL);

    d->backendInterface->stop();
}

/* Q_INVOKABLE */ void WPlayer::clear()
{
    Q_D(WPlayer);

    if (d->backend) d->backendInterface->clear();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WPlayer::togglePlay()
{
    if (isPlaying()) pause();
    else             play ();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WPlayer::seek(int msec)
{
    Q_D(WPlayer);

    if (d->backend) d->backendInterface->seek(msec);

    if (d->state == WAbstractBackend::StatePaused && d->pauseTimeout > -1)
    {
        d->timer.start();
    }
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WPlayer::setPreviousTrack()
{
    Q_D(WPlayer);

    if (d->shuffle)
    {
        if (d->shuffleIndex == -1) return;

        const WTrack * track;

        if (d->shuffleTracks.isEmpty() && d->shuffleIndex == 0
            &&
            d->repeat == WPlayer::RepeatAll)
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
        if (d->repeat == WPlayer::RepeatNone)
        {
             d->tab->setPreviousTrack(false);
        }
        else d->tab->setPreviousTrack(true);
    }
    else if (d->playlist)
    {
        if (d->repeat == WPlayer::RepeatNone)
        {
             d->playlist->setPreviousTrack(false);
        }
        else d->playlist->setPreviousTrack(true);
    }
}

/* Q_INVOKABLE */ void WPlayer::setNextTrack()
{
    Q_D(WPlayer);

    if (d->shuffle)
    {
        if (d->shuffleIndex == -1) return;

        const WTrack * track;

        int last = d->shuffleHistory.count() - 1;

        if (d->shuffleTracks.isEmpty() && d->shuffleIndex == last
            &&
            d->repeat == WPlayer::RepeatAll)
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
        if (d->repeat == WPlayer::RepeatNone)
        {
             d->tab->setNextTrack(false);
        }
        else d->tab->setNextTrack(true);
    }
    else if (d->playlist)
    {
        if (d->repeat == WPlayer::RepeatNone)
        {
             d->playlist->setNextTrack(false);
        }
        else d->playlist->setNextTrack(true);
    }
}

/* Q_INVOKABLE */ void WPlayer::reloadSource()
{
    Q_D(WPlayer);

    if (d->tab == NULL) return;

    const WBookmarkTrack * bookmark = d->tab->currentBookmark();

    if (bookmark)
    {
        d->loadSource(bookmark->source(), duration(), currentTime());
    }
    else if (d->backend)
    {
        d->backendInterface->loadSource(d->backendInterface->source());
    }
}

//-------------------------------------------------------------------------------------------------

void WPlayer::updateFrame()
{
#if defined(QT_NEW) && defined(SK_NO_QML) == false
    Q_D(WPlayer);

    if (d->view) d->view->updateFrame();
#endif
}

/* Q_INVOKABLE */ QImage WPlayer::getFrame() const
{
    Q_D(const WPlayer);

    if (d->backend)
    {
        d->backend->updateFrame();

        return d->backend->getFrame();
    }
    else return QImage();
}

/* Q_INVOKABLE */ QRectF WPlayer::getRect() const
{
    Q_D(const WPlayer);

    if (d->backend)
    {
         return d->backend->getRect();
    }
    else return QRectF();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ bool WPlayer::applyBackend(WAbstractBackend * backend)
{
    Q_D(WPlayer);

    if (backend == NULL) return false;

    if (d->backend)
    {
         qWarning("WPlayer::applyBackend: The backend is already set.");

         return false;
    }

    d->backend = backend;

    d->setBackendInterface(backend, NULL);

    backend->setParent(this);
    backend->setPlayer(this);

    d->updateRepeat(trackType());

    backend->setSpeed(d->speed);

    backend->setVolume(d->volume);

    backend->setOutput    (d->output);
    backend->setQuality   (d->quality);
    backend->setSourceMode(d->mode);

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

    connect(backend, SIGNAL(loaded()), this, SIGNAL(loaded()));

    connect(backend, SIGNAL(stateLoadChanged()), this, SIGNAL(stateLoadChanged()));

    connect(backend, SIGNAL(vbmlChanged()), this, SIGNAL(vbmlChanged()));
    connect(backend, SIGNAL(liveChanged()), this, SIGNAL(liveChanged()));

    connect(backend, SIGNAL(startedChanged()), this, SIGNAL(startedChanged()));
    connect(backend, SIGNAL(endedChanged  ()), this, SIGNAL(endedChanged  ()));

    connect(backend, SIGNAL(currentTimeChanged()), this, SIGNAL(currentTimeChanged()));
    connect(backend, SIGNAL(durationChanged   ()), this, SIGNAL(durationChanged   ()));

    connect(backend, SIGNAL(progressChanged()), this, SIGNAL(progressChanged()));

    connect(backend, SIGNAL(speedChanged()), this, SIGNAL(speedChanged()));

    connect(backend, SIGNAL(volumeChanged()), this, SIGNAL(volumeChanged()));

    connect(backend, SIGNAL(repeatChanged()), this, SIGNAL(repeatChanged()));

    connect(backend, SIGNAL(outputChanged    ()), this, SIGNAL(outputChanged    ()));
    connect(backend, SIGNAL(qualityChanged   ()), this, SIGNAL(qualityChanged   ()));
    connect(backend, SIGNAL(sourceModeChanged()), this, SIGNAL(sourceModeChanged()));

    connect(backend, SIGNAL(outputActiveChanged ()), this, SIGNAL(outputActiveChanged ()));
    connect(backend, SIGNAL(qualityActiveChanged()), this, SIGNAL(qualityActiveChanged()));

    connect(backend, SIGNAL(fillModeChanged()), this, SIGNAL(fillModeChanged()));

    connect(backend, SIGNAL(videosChanged()), this, SIGNAL(videosChanged()));
    connect(backend, SIGNAL(audiosChanged()), this, SIGNAL(audiosChanged()));

    connect(backend, SIGNAL(trackVideoChanged()), this, SIGNAL(trackVideoChanged()));
    connect(backend, SIGNAL(trackAudioChanged()), this, SIGNAL(trackAudioChanged()));

    connect(backend, SIGNAL(scanOutputChanged()), this, SIGNAL(scanOutputChanged()));

    connect(backend, SIGNAL(currentOutputChanged()), this, SIGNAL(currentOutputChanged()));

    connect(backend, SIGNAL(outputsChanged()), this, SIGNAL(outputsChanged()));

    connect(backend, SIGNAL(subtitleChanged()), this, SIGNAL(subtitleChanged()));

    connect(backend, SIGNAL(contextChanged()), this, SIGNAL(contextChanged()));

    connect(backend, SIGNAL(chaptersChanged()), this, SIGNAL(chaptersChanged()));

    connect(backend, SIGNAL(ambientChanged()), this, SIGNAL(ambientChanged()));

    connect(backend, SIGNAL(subtitlesChanged()), this, SIGNAL(subtitlesChanged()));

    connect(backend, SIGNAL(ended()), this, SLOT(onEnded()));

    connect(backend, SIGNAL(error(const QString &)), this, SLOT(onError()));

    connect(backend, SIGNAL(stateChanged   ()), this, SLOT(onStateChanged   ()));
    connect(backend, SIGNAL(durationChanged()), this, SLOT(onDurationChanged()));

    emit backendChanged();

    return true;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WPlayer::updateHighlightedTab()
{
    Q_D(WPlayer); d->onHighlightedTabChanged();
}

/* Q_INVOKABLE */ QVariantList WPlayer::chaptersData(bool sort) const
{
    QVariantList list;

    QList<WChapter> chapters = this->chapters();

    if (sort) std::sort(chapters.begin(), chapters.end(), sortChapter);

    foreach (const WChapter & chapter, chapters)
    {
        list.append(chapter.toMap());
    }

    return list;
}

//---------------------------------------------------------------------------------------------
// Tracks

/* Q_INVOKABLE */ int WPlayer::idVideo(int index) const
{
    Q_D(const WPlayer);

    if (d->backend)
    {
         return d->backend->idVideo(index);
    }
    else return -1;
}

/* Q_INVOKABLE */ int WPlayer::idAudio(int index) const
{
    Q_D(const WPlayer);

    if (d->backend)
    {
         return d->backend->idAudio(index);
    }
    else return -1;
}

/* Q_INVOKABLE */ int WPlayer::indexVideo(int id) const
{
    Q_D(const WPlayer);

    if (d->backend)
    {
         return d->backend->indexVideo(id);
    }
    else return -1;
}

/* Q_INVOKABLE */ int WPlayer::indexAudio(int id) const
{
    Q_D(const WPlayer);

    if (d->backend)
    {
         return d->backend->indexAudio(id);
    }
    else return -1;
}

/* Q_INVOKABLE */ QString WPlayer::videoName(int id) const
{
    Q_D(const WPlayer);

    if (d->backend)
    {
         return d->backend->videoName(id);
    }
    else return QString();
}

/* Q_INVOKABLE */ QString WPlayer::audioName(int id) const
{
    Q_D(const WPlayer);

    if (d->backend)
    {
         return d->backend->audioName(id);
    }
    else return QString();
}

//-------------------------------------------------------------------------------------------------
// Virtual interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QString WPlayer::toVbml(const QString & source, int currentTime) const
{
    Q_D(const WPlayer);

    if (d->tab)
    {
        return d->tab->toVbml(source, currentTime);
    }
    else return QString();
}

//-------------------------------------------------------------------------------------------------
// Protected WPlaylistWatcher implementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WPlayer::beginTracksInsert(int first, int last)
{
    Q_D(WPlayer);

    if (d->shuffle == false) return;

    for (int i = first; i <= last; i++)
    {
        const WTrack * track = d->playlist->trackPointerAt(i);

        d->shuffleTracks.append(track);
    }
}

/* virtual */ void WPlayer::endTracksInsert()
{
    Q_D(WPlayer);

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

/* virtual */ void WPlayer::beginTracksRemove(int first, int last)
{
    Q_D(WPlayer);

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

/* virtual */ void WPlayer::beginTracksClear()
{
    Q_D(WPlayer);

    if (d->shuffle == false) return;

    d->shuffleIndex = -1;

    d->shuffleTracks .clear();
    d->shuffleHistory.clear();
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

#ifndef SK_NO_QML

WDeclarativePlayer * WPlayer::view() const
{
    Q_D(const WPlayer); return d->view;
}

void WPlayer::setView(WDeclarativePlayer * view)
{
    Q_D(WPlayer);

    if (d->view == view) return;

    d->view = view;

    emit viewChanged();
}

#endif

WAbstractBackend * WPlayer::backend() const
{
    Q_D(const WPlayer); return d->backend;
}

void WPlayer::setBackend(WAbstractBackend * backend)
{
    applyBackend(backend);
}

QList<WAbstractHook *> WPlayer::hooks() const
{
    Q_D(const WPlayer); return d->hooks;
}

void WPlayer::setHooks(const QList<WAbstractHook *> & hooks)
{
    Q_D(WPlayer);

    if (d->backend == NULL)
    {
        qWarning("WPlayer::setHooks: The backend is not set.");

        return;
    }

    if (d->hooks.isEmpty() == false)
    {
        qWarning("WPlayer::setHooks: Hooks are already set.");

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

WBroadcastServer * WPlayer::server() const
{
    Q_D(const WPlayer); return d->server;
}

void WPlayer::setServer(WBroadcastServer * server)
{
    Q_D(WPlayer);

    if (server == NULL) return;

    if (d->server)
    {
        qWarning("WPlayer::setServer: The server is already set.");

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

QString WPlayer::source() const
{
    Q_D(const WPlayer);

    if (d->backend)
    {
         return d->backendInterface->source();
    }
    else return d->source;
}

void WPlayer::setSource(const QString & url)
{
    Q_D(WPlayer);

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

WPlaylist * WPlayer::playlist() const
{
    Q_D(const WPlayer); return d->playlist;
}

void WPlayer::setPlaylist(WPlaylist * playlist)
{
    Q_D(WPlayer);

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

WAbstractBackend::State WPlayer::state() const
{
    Q_D(const WPlayer); return d->state;
}

WAbstractBackend::StateLoad WPlayer::stateLoad() const
{
    Q_D(const WPlayer);

    if (d->backend)
    {
         return d->backend->stateLoad();
    }
    else return WAbstractBackend::StateLoadDefault;
}

//-------------------------------------------------------------------------------------------------

bool WPlayer::isLoading() const
{
    Q_D(const WPlayer);

    if (d->backend)
    {
         return d->backend->isLoading();
    }
    else return false;
}

//-------------------------------------------------------------------------------------------------

bool WPlayer::isDefault() const
{
    Q_D(const WPlayer);

    if (d->backend)
    {
         return d->backend->isDefault();
    }
    else return false;
}

bool WPlayer::isStarting() const
{
    Q_D(const WPlayer);

    if (d->backend)
    {
         return d->backend->isStarting();
    }
    else return false;
}

bool WPlayer::isResuming() const
{
    Q_D(const WPlayer);

    if (d->backend)
    {
         return d->backend->isResuming();
    }
    else return false;
}

bool WPlayer::isBuffering() const
{
    Q_D(const WPlayer);

    if (d->backend)
    {
         return d->backend->isBuffering();
    }
    else return false;
}

//-------------------------------------------------------------------------------------------------

bool WPlayer::isPlaying() const
{
    Q_D(const WPlayer); return (d->state == WAbstractBackend::StatePlaying);
}

bool WPlayer::isPaused() const
{
    Q_D(const WPlayer); return (d->state == WAbstractBackend::StatePaused);
}

bool WPlayer::isStopped() const
{
    Q_D(const WPlayer); return (d->state == WAbstractBackend::StateStopped);
}

//-------------------------------------------------------------------------------------------------

bool WPlayer::isVideo() const
{
    Q_D(const WPlayer);

    if (d->backend)
    {
         return d->backendInterface->sourceIsVideo();
    }
    else return false;
}

bool WPlayer::isAudio() const
{
    Q_D(const WPlayer);

    if (d->backend)
    {
         return d->backendInterface->sourceIsAudio();
    }
    else return false;
}

//-------------------------------------------------------------------------------------------------

bool WPlayer::isVbml() const
{
    Q_D(const WPlayer);

    if (d->backend)
    {
         return d->backend->isVbml();
    }
    else return false;
}

bool WPlayer::isLive() const
{
    Q_D(const WPlayer);

    if (d->backend)
    {
         return d->backend->isLive();
    }
    else return false;
}

//-------------------------------------------------------------------------------------------------

bool WPlayer::hasStarted() const
{
    Q_D(const WPlayer);

    if (d->backend)
    {
         return d->backend->hasStarted();
    }
    else return false;
}

bool WPlayer::hasEnded() const
{
    Q_D(const WPlayer);

    if (d->backend)
    {
         return d->backend->hasEnded();
    }
    else return false;
}

//-------------------------------------------------------------------------------------------------

bool WPlayer::hasOutput() const
{
    return (outputType() != WAbstractBackend::OutputDefault);
}

//-------------------------------------------------------------------------------------------------

int WPlayer::currentTime() const
{
    Q_D(const WPlayer);

    if (d->backend)
    {
         return d->backend->currentTime();
    }
    else return -1;
}

int WPlayer::duration() const
{
    Q_D(const WPlayer);

    if (d->backend)
    {
         return d->backend->duration();
    }
    else return -1;
}

//-------------------------------------------------------------------------------------------------

qreal WPlayer::progress() const
{
    Q_D(const WPlayer);

    if (d->backend)
    {
         return d->backend->progress();
    }
    else return 0.0;
}

//-------------------------------------------------------------------------------------------------

qreal WPlayer::speed() const
{
    Q_D(const WPlayer);

    if (d->backend)
    {
        return d->backend->speed();
    }
    else return d->speed;
}

void WPlayer::setSpeed(qreal speed)
{
    Q_D(WPlayer);

    if (d->backend)
    {
        d->backend->setSpeed(speed);
    }
    else if (d->speed != speed)
    {
        d->speed = speed;

        emit speedChanged();
    }
}

//-------------------------------------------------------------------------------------------------

qreal WPlayer::volume() const
{
    Q_D(const WPlayer);

    if (d->backend)
    {
        return d->backend->volume();
    }
    else return d->volume;
}

void WPlayer::setVolume(qreal volume)
{
    Q_D(WPlayer);

    if (d->backend)
    {
        d->backend->setVolume(volume);
    }
    else if (d->volume != volume)
    {
        d->volume = volume;

        emit volumeChanged();
    }
}

//-------------------------------------------------------------------------------------------------

bool WPlayer::autoPlay() const
{
    Q_D(const WPlayer); return d->autoPlay;
}

void WPlayer::setAutoPlay(bool autoPlay)
{
    Q_D(WPlayer);

    if (d->autoPlay == autoPlay) return;

    d->autoPlay = autoPlay;

    emit autoPlayChanged();
}

//-------------------------------------------------------------------------------------------------

bool WPlayer::shuffle() const
{
    Q_D(const WPlayer); return d->shuffle;
}

void WPlayer::setShuffle(bool shuffle)
{
    Q_D(WPlayer);

    if (d->shuffle == shuffle) return;

    d->shuffle = shuffle;

    if (shuffle) d->resetShuffle();
    else         d->clearShuffle();

    emit shuffleChanged();

    emit playlistUpdated();
}

//-------------------------------------------------------------------------------------------------

WPlayer::Repeat WPlayer::repeat() const
{
    Q_D(const WPlayer); return d->repeat;
}

void WPlayer::setRepeat(Repeat repeat)
{
    Q_D(WPlayer);

    if (d->repeat == repeat) return;

    d->repeat = repeat;

    if (d->backend)
    {
        d->updateRepeat(trackType());
    }
    else emit repeatChanged();

    emit playlistUpdated();
}

//-------------------------------------------------------------------------------------------------

WAbstractBackend::Output WPlayer::output() const
{
    Q_D(const WPlayer);

    if (d->backend)
    {
        return d->backend->output();
    }
    else return d->output;
}

void WPlayer::setOutput(WAbstractBackend::Output output)
{
    Q_D(WPlayer);

    if (d->backend)
    {
        d->backend->setOutput(output);
    }
    else if (d->output != output)
    {
        d->output = output;

        emit outputChanged();
    }
}

WAbstractBackend::Quality WPlayer::quality() const
{
    Q_D(const WPlayer);

    if (d->backend)
    {
        return d->backend->quality();
    }
    else return d->quality;
}

void WPlayer::setQuality(WAbstractBackend::Quality quality)
{
    Q_D(WPlayer);

    if (d->backend)
    {
        d->backend->setQuality(quality);
    }
    else if (d->quality != quality)
    {
        d->quality = quality;

        emit qualityChanged();
    }
}

WAbstractBackend::SourceMode WPlayer::sourceMode() const
{
    Q_D(const WPlayer);

    if (d->backend)
    {
        return d->backend->sourceMode();
    }
    else return d->mode;
}

void WPlayer::setSourceMode(WAbstractBackend::SourceMode mode)
{
    Q_D(WPlayer);

    if (d->backend)
    {
        d->backend->setSourceMode(mode);
    }
    else if (d->mode != mode)
    {
        d->mode = mode;

        emit sourceModeChanged();
    }
}

//-------------------------------------------------------------------------------------------------

WAbstractBackend::Output WPlayer::outputActive() const
{
    Q_D(const WPlayer);

    if (d->backend)
    {
         return d->backend->outputActive();
    }
    else return WAbstractBackend::OutputNone;
}

WAbstractBackend::Quality WPlayer::qualityActive() const
{
    Q_D(const WPlayer);

    if (d->backend)
    {
         return d->backend->qualityActive();
    }
    else return WAbstractBackend::QualityDefault;
}

//-------------------------------------------------------------------------------------------------

WAbstractBackend::FillMode WPlayer::fillMode() const
{
    Q_D(const WPlayer);

    if (d->backend)
    {
        return d->backend->fillMode();
    }
    else return d->fillMode;
}

void WPlayer::setFillMode(WAbstractBackend::FillMode fillMode)
{
    Q_D(WPlayer);

    if (d->backend)
    {
        d->backend->setFillMode(fillMode);
    }
    else if (d->fillMode != fillMode)
    {
        d->fillMode = fillMode;

        emit fillModeChanged();
    }
}

//-------------------------------------------------------------------------------------------------

int WPlayer::trackVideo() const
{
    Q_D(const WPlayer);

    if (d->backend)
    {
        return d->backend->trackVideo();
    }
    else return d->trackVideo;
}

void WPlayer::setTrackVideo(int id)
{
    Q_D(WPlayer);

    if (d->backend)
    {
        d->backend->setTrackVideo(id);
    }
    else if (d->trackVideo != id)
    {
        d->trackVideo = id;

        emit trackVideoChanged();
    }
}

int WPlayer::trackAudio() const
{
    Q_D(const WPlayer);

    if (d->backend)
    {
        return d->backend->trackAudio();
    }
    else return d->trackAudio;
}

void WPlayer::setTrackAudio(int id)
{
    Q_D(WPlayer);

    if (d->backend)
    {
        d->backend->setTrackAudio(id);
    }
    else if (d->trackAudio != id)
    {
        d->trackAudio = id;

        emit trackAudioChanged();
    }
}

int WPlayer::countVideos() const
{
    Q_D(const WPlayer);

    if (d->backend)
    {
        return d->backend->countVideos();
    }
    else return 0;
}

int WPlayer::countAudios() const
{
    Q_D(const WPlayer);

    if (d->backend)
    {
        return d->backend->countAudios();
    }
    else return 0;
}

//-------------------------------------------------------------------------------------------------

bool WPlayer::scanOutput() const
{
    Q_D(const WPlayer);

    if (d->backend)
    {
        return d->backend->scanOutput();
    }
    else return d->scanOutput;
}

void WPlayer::setScanOutput(bool enabled)
{
    Q_D(WPlayer);

    if (d->backend)
    {
        d->backend->setScanOutput(enabled);
    }
    else if (d->scanOutput != enabled)
    {
        d->scanOutput = enabled;

        emit scanOutputChanged();
    }
}

int WPlayer::currentOutput() const
{
    Q_D(const WPlayer);

    if (d->backend)
    {
        return d->backend->currentOutput();
    }
    else return d->currentOutput;
}

void WPlayer::setCurrentOutput(int index)
{
    Q_D(WPlayer);

    if (d->backend)
    {
        d->backend->setCurrentOutput(index);
    }
    else if (d->currentOutput != index)
    {
        d->currentOutput = index;

        emit currentOutputChanged();
    }
}

QString WPlayer::outputName() const
{
    Q_D(const WPlayer);

    if (d->backend)
    {
        return d->backend->outputName();
    }
    else return QString();
}

WAbstractBackend::OutputType WPlayer::outputType() const
{
    Q_D(const WPlayer);

    if (d->backend)
    {
        return d->backend->outputType();
    }
    else return WAbstractBackend::OutputDefault;
}

int WPlayer::countOutputs() const
{
    Q_D(const WPlayer);

    if (d->backend)
    {
        return d->backend->countOutputs();
    }
    else return 0;
}

//-------------------------------------------------------------------------------------------------

QString WPlayer::subtitle() const
{
    Q_D(const WPlayer);

    if (d->backend)
    {
        return d->backend->subtitle();
    }
    else return d->subtitle;
}

void WPlayer::setSubtitle(const QString & subtitle)
{
    Q_D(WPlayer);

    if (d->backend)
    {
        d->backend->setSubtitle(subtitle);

        if (d->tab) d->tab->setSubtitle(subtitle);
    }
    else if (d->subtitle != subtitle)
    {
        d->subtitle = subtitle;

        if (d->tab) d->tab->setSubtitle(subtitle);

        emit subtitleChanged();
    }
}

QString WPlayer::context() const
{
    Q_D(const WPlayer);

    if (d->backend)
    {
        return d->backend->context();
    }
    else return QString();
}

QString WPlayer::contextId() const
{
    Q_D(const WPlayer);

    if (d->backend)
    {
        return d->backend->contextId();
    }
    else return QString();
}

QList<WChapter> WPlayer::chapters() const
{
    Q_D(const WPlayer);

    if (d->backend)
    {
        return d->backend->chapters();
    }
    else return QList<WChapter>();
}

QString WPlayer::ambient() const
{
    Q_D(const WPlayer);

    if (d->backend)
    {
        return d->backend->ambient();
    }
    else return QString();
}

QStringList WPlayer::subtitles() const
{
    Q_D(const WPlayer);

    if (d->backend)
    {
        return d->backend->subtitles();
    }
    else return QStringList();
}

//-------------------------------------------------------------------------------------------------

int WPlayer::pauseTimeout() const
{
    Q_D(const WPlayer);

    return d->pauseTimeout;
}

void WPlayer::setPauseTimeout(int msec)
{
    Q_D(WPlayer);

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

int WPlayer::count() const
{
    Q_D(const WPlayer);

    if (d->playlist)
    {
         return d->playlist->count();
    }
    else return -1;
}

//-------------------------------------------------------------------------------------------------

bool WPlayer::hasPreviousTrack() const
{
    Q_D(const WPlayer);

    if (d->repeat == WPlayer::RepeatAll)
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

bool WPlayer::hasNextTrack() const
{
    Q_D(const WPlayer);

    if (d->repeat == WPlayer::RepeatAll)
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

WTrack::Type WPlayer::trackType() const
{
    Q_D(const WPlayer);

    if (d->tab)
    {
        return d->tab->type();
    }
    else if (d->playlist)
    {
        const WTrack * track = static_cast<const WTrack *> (d->playlist->currentTrackPointer());

        if (track)
        {
             return track->type();
        }
        else return WTrack::Track;
    }
    else return WTrack::Track;
}

bool WPlayer::trackIsLive() const
{
    return (trackType() == WTrack::Live);
}

bool WPlayer::trackIsHub() const
{
    return (trackType() == WTrack::Hub);
}

bool WPlayer::trackIsChannel() const
{
    return (trackType() == WTrack::Channel);
}

bool WPlayer::trackIsInteractive() const
{
    return (trackType() == WTrack::Interactive);
}

bool WPlayer::trackIsLite() const
{
    return (trackType() == WTrack::Lite);
}

//-------------------------------------------------------------------------------------------------

WTrack::State WPlayer::trackState() const
{
    Q_D(const WPlayer);

    if (d->tab)
    {
        return d->tab->state();
    }
    else if (d->playlist)
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

bool WPlayer::trackIsDefault() const
{
    return (trackState() == WTrack::Default);
}

bool WPlayer::trackIsLoading() const
{
    return (trackState() == WTrack::Loading);
}

bool WPlayer::trackIsLoaded() const
{
    return (trackState() >= WTrack::Loaded);
}

//-------------------------------------------------------------------------------------------------

QString WPlayer::trackTitle() const
{
    Q_D(const WPlayer);

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

QString WPlayer::trackCover() const
{
    Q_D(const WPlayer);

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

int WPlayer::trackCurrentTime() const
{
    Q_D(const WPlayer);

    if (d->tab)
    {
         return d->tab->currentTime();
    }
    else return -1;
}

int WPlayer::trackDuration() const
{
    Q_D(const WPlayer);

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

QDateTime WPlayer::trackDate() const
{
    Q_D(const WPlayer);

    if (d->tab)
    {
         return d->tab->date();
    }
    else return QDateTime();
}

//-------------------------------------------------------------------------------------------------

int WPlayer::trackIndex() const
{
    Q_D(const WPlayer);

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

WTabsTrack * WPlayer::tabs() const
{
    Q_D(const WPlayer); return d->tabs;
}

void WPlayer::setTabs(WTabsTrack * tabs)
{
    Q_D(WPlayer);

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

WTabTrack * WPlayer::tab() const
{
    Q_D(const WPlayer); return d->tab;
}

int WPlayer::tabIndex() const
{
    Q_D(const WPlayer);

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

bool WPlayer::videoTag() const
{
    Q_D(const WPlayer); return d->videoTag;
}

void WPlayer::setVideoTag(bool enabled)
{
    Q_D(WPlayer);

    if (d->videoTag == enabled) return;

    d->videoTag = enabled;

    emit videoTagChanged();
}

#endif // SK_NO_PLAYER
