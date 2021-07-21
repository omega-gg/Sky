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
#include <WAbstractHook>
#include <WPlaylist>
#include <WTabsTrack>
#include <WTabTrack>
#include <WLibraryFolder>

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

#if defined(QT_LATEST) && defined(SK_SOFTWARE) == false
    frameUpdate = false;
#endif

    folder   = NULL;
    playlist = NULL;

    tabs = NULL;
    tab  = NULL;

    state = WAbstractBackend::StateStopped;

    speed = 1.0;

    volume = 1.0;

    autoPlay = false;

    shuffle = false;

    shuffleIndex = -1;
    shuffleLock  = false;

    repeat = WDeclarativePlayer::RepeatNone;

    output  = WAbstractBackend::OutputMedia;
    quality = WAbstractBackend::Quality720;

    fillMode = WAbstractBackend::PreserveAspectFit;

    scanOutput = false;

    currentOutput = 0;

    pauseTimeout = -1;

    keepState = false;

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

//-------------------------------------------------------------------------------------------------

void WDeclarativePlayerPrivate::setTab(WTabTrack * tab)
{
    if (this->tab == tab) return;

    Q_Q(WDeclarativePlayer);

    if (this->tab)
    {
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

//-------------------------------------------------------------------------------------------------

void WDeclarativePlayerPrivate::loadSource(const QString & source, int duration, int currentTime)
{
    Q_Q(WDeclarativePlayer);

    if (hook && hook->checkSource(source))
    {
        if (backendInterface && backendInterface == backend)
        {
            if (backend->isPlaying())
            {
                // NOTE: We have to keep state to avoid clearing highlightedTab.
                keepState = true;

                backend->clear();

                keepState = false;

                backendInterface = hook;

                hook->loadSource(source, duration, currentTime);

                hook->play();
            }
            else
            {
                backend->clear();

                backendInterface = hook;

                hook->loadSource(source, duration, currentTime);
            }
        }
        else
        {
            backendInterface = hook;

            hook->loadSource(source, duration, currentTime);
        }
    }
    else if (backend)
    {
        if (backendInterface && backendInterface == hook)
        {
            if (hook->backend()->isPlaying())
            {
                // NOTE: We have to keep state to avoid clearing highlightedTab.
                keepState = true;

                hook->clear();

                keepState = false;

                backendInterface = backend;

                backend->loadSource(source, duration, currentTime);

                backend->play();
            }
            else
            {
                hook->clear();

                backendInterface = backend;

                backend->loadSource(source, duration, currentTime);
            }
        }
        else
        {
            backendInterface = backend;

            backend->loadSource(source, duration, currentTime);
        }
    }
    else backendInterface = NULL;

    if (shuffle && shuffleLock == false)
    {
        resetShuffle();

        emit q->playlistUpdated();
    }

    emit q->sourceChanged();
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
        loadSource(bookmark->source(), bookmark->duration(), bookmark->currentTime());
    }
    else if (backend && backendInterface->source().isEmpty() == false)
    {
        Q_Q(WDeclarativePlayer);

        backendInterface->loadSource(QString());

        emit q->sourceChanged();
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
    }
    else setPlaylist(NULL);

    emit q->currentTrackUpdated();

    emit q->subtitleChanged();
}

//-------------------------------------------------------------------------------------------------

void WDeclarativePlayerPrivate::onHookDestroyed()
{
    Q_Q(WDeclarativePlayer);

    if (backendInterface == hook)
    {
        backendInterface = backend;
    }

    hook = NULL;

    emit q->hookChanged();
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

#ifdef QT_LATEST

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
#ifdef QT_LATEST
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

/* virtual */ void WDeclarativePlayer::geometryChanged(const QRectF & newGeometry,
                                                       const QRectF & oldGeometry)
{
    Q_D(WDeclarativePlayer);

    if (oldGeometry.size() != newGeometry.size() && d->backend)
    {
        d->backend->setSize(newGeometry.size());

#if defined(QT_LATEST) && defined(SK_SOFTWARE) == false
        d->frameUpdate = true;
#endif
    }

#ifdef SK_SOFTWARE
    WDeclarativeItemPaint::geometryChanged(newGeometry, oldGeometry);
#else
    WDeclarativeItem::geometryChanged(newGeometry, oldGeometry);
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

    if (d->backend == backend) return;

    if (d->backend)
    {
        d->source = d->backendInterface->source();

        if (d->backendInterface == d->backend)
        {
            d->backendInterface = backend;
        }

        disconnect(d->backend, 0, this, 0);

        d->backend->deleteBackend();
    }
    else if (d->backendInterface == d->backend)
    {
        d->backendInterface = backend;
    }

    d->backend = backend;

    if (backend)
    {
        backend->setParent    (this);
        backend->setParentItem(this);

        backend->setSize(QSizeF(width(), height()));

        d->updateRepeat();

        backend->setSpeed(d->speed);

        backend->setVolume(d->volume);

        backend->setOutput (d->output);
        backend->setQuality(d->quality);

        backend->setFillMode(d->fillMode);

        backend->setScanOutput(d->scanOutput);

        backend->setCurrentOutput(d->currentOutput);

        if (d->source.isEmpty() == false)
        {
            d->clearPlaylistAndTabs();

            d->loadSource(d->source, -1, -1);
        }

        connect(backend, SIGNAL(stateChanged    ()), this, SIGNAL(stateChanged    ()));
        connect(backend, SIGNAL(stateLoadChanged()), this, SIGNAL(stateLoadChanged()));

        connect(backend, SIGNAL(startedChanged()), this, SIGNAL(startedChanged()));
        connect(backend, SIGNAL(endedChanged  ()), this, SIGNAL(endedChanged  ()));

        connect(backend, SIGNAL(currentTimeChanged()), this, SIGNAL(currentTimeChanged()));
        connect(backend, SIGNAL(durationChanged   ()), this, SIGNAL(durationChanged   ()));

        connect(backend, SIGNAL(progressChanged()), this, SIGNAL(progressChanged()));

        connect(backend, SIGNAL(outputActiveChanged ()), this, SIGNAL(outputActiveChanged ()));
        connect(backend, SIGNAL(qualityActiveChanged()), this, SIGNAL(qualityActiveChanged()));

        connect(backend, SIGNAL(ended()), this, SLOT(onEnded()));

        connect(backend, SIGNAL(stateChanged   ()), this, SLOT(onStateChanged   ()));
        connect(backend, SIGNAL(durationChanged()), this, SLOT(onDurationChanged()));
    }

    emit backendChanged();
}

//-------------------------------------------------------------------------------------------------

WAbstractHook * WDeclarativePlayer::hook() const
{
    Q_D(const WDeclarativePlayer); return d->hook;
}

void WDeclarativePlayer::setHook(WAbstractHook * hook)
{
    Q_D(WDeclarativePlayer);

    if (d->hook == hook) return;

    if (d->backendInterface == d->hook)
    {
        d->backendInterface = d->backend;
    }

    if (d->hook)
    {
        disconnect(d->hook, 0, this, 0);

        delete d->hook;
    }

    d->hook = hook;

    if (hook)
    {
        hook->setParent(this);

        connect(hook, SIGNAL(destroyed()), this, SLOT(onHookDestroyed()));
    }

    emit hookChanged();
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
    else d->source = url;
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
    return (outputType() != WAbstractBackend::TypeDefault);
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
    Q_D(const WDeclarativePlayer); return d->speed;
}

void WDeclarativePlayer::setSpeed(qreal speed)
{
    Q_D(WDeclarativePlayer);

    if (d->speed == speed) return;

    d->speed = speed;

    if (d->backend) d->backend->setSpeed(speed);

    emit speedChanged();
}

//-------------------------------------------------------------------------------------------------

qreal WDeclarativePlayer::volume() const
{
    Q_D(const WDeclarativePlayer); return d->volume;
}

void WDeclarativePlayer::setVolume(qreal volume)
{
    Q_D(WDeclarativePlayer);

    if (d->volume == volume) return;

    d->volume = volume;

    if (d->backend) d->backend->setVolume(volume);

    emit volumeChanged();
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

    if (d->repeat == repeat) return;

    d->repeat = repeat;

    if (d->backend) d->updateRepeat();

    emit repeatChanged();

    emit playlistUpdated();
}

//-------------------------------------------------------------------------------------------------

WAbstractBackend::Output WDeclarativePlayer::output() const
{
    Q_D(const WDeclarativePlayer); return d->output;
}

void WDeclarativePlayer::setOutput(WAbstractBackend::Output output)
{
    Q_D(WDeclarativePlayer);

    if (d->output == output) return;

    d->output = output;

    if (d->backend) d->backend->setOutput(output);

    emit outputChanged();
}

//-------------------------------------------------------------------------------------------------

WAbstractBackend::Quality WDeclarativePlayer::quality() const
{
    Q_D(const WDeclarativePlayer); return d->quality;
}

void WDeclarativePlayer::setQuality(WAbstractBackend::Quality quality)
{
    Q_D(WDeclarativePlayer);

    if (d->quality == quality) return;

    d->quality = quality;

    if (d->backend) d->backend->setQuality(quality);

    emit qualityChanged();
}

//-------------------------------------------------------------------------------------------------

WAbstractBackend::Output WDeclarativePlayer::outputActive() const
{
    Q_D(const WDeclarativePlayer);

    if (d->backend)
    {
         return d->backend->outputActive();
    }
    else return WAbstractBackend::OutputInvalid;
}

WAbstractBackend::Quality WDeclarativePlayer::qualityActive() const
{
    Q_D(const WDeclarativePlayer);

    if (d->backend)
    {
         return d->backend->qualityActive();
    }
    else return WAbstractBackend::QualityInvalid;
}

//-------------------------------------------------------------------------------------------------

WAbstractBackend::FillMode WDeclarativePlayer::fillMode() const
{
    Q_D(const WDeclarativePlayer); return d->fillMode;
}

void WDeclarativePlayer::setFillMode(WAbstractBackend::FillMode fillMode)
{
    Q_D(WDeclarativePlayer);

    if (d->fillMode == fillMode) return;

    d->fillMode = fillMode;

    if (d->backend)
    {
        d->backend->setFillMode(fillMode);
    }

#if defined(QT_LATEST) && defined(SK_SOFTWARE) == false
    d->frameUpdate = true;
#endif

    update();

    emit fillModeChanged();
}

//-------------------------------------------------------------------------------------------------

bool WDeclarativePlayer::scanOutput() const
{
    Q_D(const WDeclarativePlayer); return d->scanOutput;
}

void WDeclarativePlayer::setScanOutput(bool enabled)
{
    Q_D(WDeclarativePlayer);

    if (d->scanOutput == enabled) return;

    d->scanOutput = enabled;

    if (d->backend)
    {
        d->backend->setScanOutput(enabled);
    }

    emit scanOutputChanged();
}

int WDeclarativePlayer::currentOutput() const
{
    Q_D(const WDeclarativePlayer); return d->currentOutput;
}

void WDeclarativePlayer::setCurrentOutput(int index)
{
    Q_D(WDeclarativePlayer);

    if (d->currentOutput == index) return;

    d->currentOutput = index;

    if (d->backend)
    {
        d->backend->setCurrentOutput(index);
    }

    emit currentOutputChanged();
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
    else return WAbstractBackend::TypeDefault;
}

//-------------------------------------------------------------------------------------------------

QString WDeclarativePlayer::subtitle() const
{
    Q_D(const WDeclarativePlayer);

    if (d->tab)
    {
         return d->tab->subtitle();
    }
    else return d->subtitle;
}

void WDeclarativePlayer::setSubtitle(const QString & subtitle)
{
    Q_D(WDeclarativePlayer);

    if (d->subtitle == subtitle) return;

    d->subtitle = subtitle;

    if (d->tab) d->tab->setSubtitle(subtitle);

    emit subtitleChanged();
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

#endif // SK_NO_DECLARATIVEPLAYER
