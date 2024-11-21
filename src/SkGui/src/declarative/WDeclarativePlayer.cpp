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
#include <WPlayer>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

WDeclarativePlayerPrivate::WDeclarativePlayerPrivate(WDeclarativePlayer * p)
#ifdef SK_SOFTWARE
    : WDeclarativeItemPaintPrivate(p) {}
#else
    : WDeclarativeItemPrivate(p) {}
#endif

//-------------------------------------------------------------------------------------------------

void WDeclarativePlayerPrivate::init()
{
    Q_Q(WDeclarativePlayer);

    player = new WPlayer(q);

    player->setView(q);

#if defined(QT_NEW) && defined(SK_SOFTWARE) == false
    frameUpdate = false;
#endif

#ifdef QT_4
    q->setFlag(QGraphicsItem::ItemHasNoContents, false);
#elif defined(SK_SOFTWARE) == false
    q->setFlag(QQuickItem::ItemHasContents);
#endif

    //---------------------------------------------------------------------------------------------
    // WPlayer

    QObject::connect(player, SIGNAL(loaded()), q, SIGNAL(loaded()));
    QObject::connect(player, SIGNAL(ended ()), q, SIGNAL(ended ()));

    QObject::connect(player, SIGNAL(clearCache()), q, SIGNAL(clearCache()));

    QObject::connect(player, SIGNAL(backendChanged()), q, SIGNAL(backendChanged()));
    QObject::connect(player, SIGNAL(hooksChanged  ()), q, SIGNAL(hooksChanged  ()));

    QObject::connect(player, SIGNAL(serverChanged()), q, SIGNAL(serverChanged()));

    QObject::connect(player, SIGNAL(sourceChanged()), q, SIGNAL(sourceChanged()));

    QObject::connect(player, SIGNAL(playlistChanged()), q, SIGNAL(playlistChanged()));
    QObject::connect(player, SIGNAL(playlistUpdated()), q, SIGNAL(playlistUpdated()));

    QObject::connect(player, SIGNAL(stateChanged    ()), q, SIGNAL(stateChanged    ()));
    QObject::connect(player, SIGNAL(stateLoadChanged()), q, SIGNAL(stateLoadChanged()));

    QObject::connect(player, SIGNAL(vbmlChanged()), q, SIGNAL(vbmlChanged()));
    QObject::connect(player, SIGNAL(liveChanged()), q, SIGNAL(liveChanged()));

    QObject::connect(player, SIGNAL(startedChanged()), q, SIGNAL(startedChanged()));
    QObject::connect(player, SIGNAL(endedChanged  ()), q, SIGNAL(endedChanged  ()));

    QObject::connect(player, SIGNAL(currentTimeChanged()), q, SIGNAL(currentTimeChanged()));
    QObject::connect(player, SIGNAL(durationChanged   ()), q, SIGNAL(durationChanged   ()));

    QObject::connect(player, SIGNAL(progressChanged()), q, SIGNAL(progressChanged()));

    QObject::connect(player, SIGNAL(speedChanged()), q, SIGNAL(speedChanged()));

    QObject::connect(player, SIGNAL(volumeChanged()), q, SIGNAL(volumeChanged()));

    QObject::connect(player, SIGNAL(autoPlayChanged()), q, SIGNAL(autoPlayChanged()));

    QObject::connect(player, SIGNAL(shuffleChanged()), q, SIGNAL(shuffleChanged()));

    QObject::connect(player, SIGNAL(repeatChanged()), q, SIGNAL(repeatChanged()));

    QObject::connect(player, SIGNAL(outputChanged    ()), q, SIGNAL(outputChanged    ()));
    QObject::connect(player, SIGNAL(qualityChanged   ()), q, SIGNAL(qualityChanged   ()));
    QObject::connect(player, SIGNAL(sourceModeChanged()), q, SIGNAL(sourceModeChanged()));

    QObject::connect(player, SIGNAL(outputActiveChanged ()), q, SIGNAL(outputActiveChanged ()));
    QObject::connect(player, SIGNAL(qualityActiveChanged()), q, SIGNAL(qualityActiveChanged()));

    QObject::connect(player, SIGNAL(fillModeChanged()), q, SIGNAL(fillModeChanged()));

    QObject::connect(player, SIGNAL(videosChanged()), q, SIGNAL(videosChanged()));
    QObject::connect(player, SIGNAL(audiosChanged()), q, SIGNAL(audiosChanged()));

    QObject::connect(player, SIGNAL(trackVideoChanged()), q, SIGNAL(trackVideoChanged()));
    QObject::connect(player, SIGNAL(trackAudioChanged()), q, SIGNAL(trackAudioChanged()));

    QObject::connect(player, SIGNAL(scanOutputChanged()), q, SIGNAL(scanOutputChanged()));

    QObject::connect(player, SIGNAL(currentOutputChanged()), q, SIGNAL(currentOutputChanged()));

    QObject::connect(player, SIGNAL(outputsChanged()), q, SIGNAL(outputsChanged()));

    QObject::connect(player, SIGNAL(subtitleChanged()), q, SIGNAL(subtitleChanged()));

    QObject::connect(player, SIGNAL(contextChanged()), q, SIGNAL(contextChanged()));

    QObject::connect(player, SIGNAL(chaptersChanged()), q, SIGNAL(chaptersChanged()));

    QObject::connect(player, SIGNAL(ambientChanged()), q, SIGNAL(ambientChanged()));

    QObject::connect(player, SIGNAL(subtitlesChanged()), q, SIGNAL(subtitlesChanged()));

    QObject::connect(player, SIGNAL(pauseTimeoutChanged()), q, SIGNAL(pauseTimeoutChanged()));

    QObject::connect(player, SIGNAL(countChanged()), q, SIGNAL(countChanged()));

    QObject::connect(player, SIGNAL(currentTrackUpdated()), q, SIGNAL(currentTrackUpdated()));

    QObject::connect(player, SIGNAL(tabsChanged()), q, SIGNAL(tabsChanged()));

    QObject::connect(player, SIGNAL(tabChanged     ()), q, SIGNAL(tabChanged     ()));
    QObject::connect(player, SIGNAL(tabIndexChanged()), q, SIGNAL(tabIndexChanged()));

    QObject::connect(player, SIGNAL(videoTagChanged()), q, SIGNAL(videoTagChanged()));
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
// Protected

#ifdef QT_4
WDeclarativePlayer::WDeclarativePlayer(WDeclarativePlayerPrivate * p, QDeclarativeItem * parent)
#else
WDeclarativePlayer::WDeclarativePlayer(WDeclarativePlayerPrivate * p, QQuickItem * parent)
#endif
#ifdef SK_SOFTWARE
    : WDeclarativeItemPaint(p, parent)
#else
    : WDeclarativeItem(p, parent)
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

    d->player->play();
}

/* Q_INVOKABLE */ void WDeclarativePlayer::replay()
{
    Q_D(WDeclarativePlayer);

    d->player->replay();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WDeclarativePlayer::pause()
{
    Q_D(WDeclarativePlayer);

    d->player->pause();
}

/* Q_INVOKABLE */ void WDeclarativePlayer::stop()
{
    Q_D(WDeclarativePlayer);

    d->player->stop();
}

/* Q_INVOKABLE */ void WDeclarativePlayer::clear()
{
    Q_D(WDeclarativePlayer);

    d->player->clear();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WDeclarativePlayer::togglePlay()
{
    Q_D(WDeclarativePlayer);

    d->player->togglePlay();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WDeclarativePlayer::seek(int msec)
{
    Q_D(WDeclarativePlayer);

    d->player->seek(msec);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WDeclarativePlayer::setPreviousTrack()
{
    Q_D(WDeclarativePlayer);

    d->player->setPreviousTrack();
}

/* Q_INVOKABLE */ void WDeclarativePlayer::setNextTrack()
{
    Q_D(WDeclarativePlayer);

    d->player->setNextTrack();
}

/* Q_INVOKABLE */ void WDeclarativePlayer::reloadSource()
{
    Q_D(WDeclarativePlayer);

    d->player->reloadSource();
}

//-------------------------------------------------------------------------------------------------

void WDeclarativePlayer::updateFrame()
{
#if defined(QT_NEW) && defined(SK_SOFTWARE) == false
    Q_D(WDeclarativePlayer);

    backend()->synchronize(&d->frame);
#endif

    update();
}

/* Q_INVOKABLE */ QImage WDeclarativePlayer::getFrame() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->getFrame();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ QRectF WDeclarativePlayer::getRect() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->getRect();
}

/* Q_INVOKABLE */ QRectF WDeclarativePlayer::getGeometry() const
{
    QRectF rect = getRect();

    if (rect.isNull()) return rect;

    QSizeF size(rect.width(), rect.height());

    WAbstractBackend::FillMode mode = fillMode();

    if (mode == WAbstractBackend::Stretch)
    {
        size.scale(size, Qt::KeepAspectRatio);
    }
    else if (mode == WAbstractBackend::PreserveAspectFit)
    {
        size.scale(size, Qt::KeepAspectRatio);
    }
    else // if (mode == WAbstractBackend::PreserveAspectCrop)
    {
        size.scale(size, Qt::KeepAspectRatioByExpanding);
    }

    qreal sizeWidth  = size.width ();
    qreal sizeHeight = size.height();

    qreal x = (width () - sizeWidth)  / 2;
    qreal y = (height() - sizeHeight) / 2;

    return QRectF(x, y, sizeWidth, sizeHeight);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WDeclarativePlayer::updateHighlightedTab()
{
    Q_D(WDeclarativePlayer);

    return d->player->updateHighlightedTab();
}

/* Q_INVOKABLE */ QVariantList WDeclarativePlayer::chaptersData(bool sort) const
{
    Q_D(const WDeclarativePlayer);

    return d->player->chaptersData(sort);
}

/* Q_INVOKABLE */ QVariantList WDeclarativePlayer::subtitlesData() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->subtitlesData();
}

//---------------------------------------------------------------------------------------------
// Tracks

/* Q_INVOKABLE */ int WDeclarativePlayer::idVideo(int index) const
{
    Q_D(const WDeclarativePlayer);

    return d->player->idVideo(index);
}

/* Q_INVOKABLE */ int WDeclarativePlayer::idAudio(int index) const
{
    Q_D(const WDeclarativePlayer);

    return d->player->idAudio(index);
}

/* Q_INVOKABLE */ int WDeclarativePlayer::indexVideo(int id) const
{
    Q_D(const WDeclarativePlayer);

    return d->player->indexVideo(id);
}

/* Q_INVOKABLE */ int WDeclarativePlayer::indexAudio(int id) const
{
    Q_D(const WDeclarativePlayer);

    return d->player->indexAudio(id);
}

/* Q_INVOKABLE */ QString WDeclarativePlayer::videoName(int id) const
{
    Q_D(const WDeclarativePlayer);

    return d->player->videoName(id);
}

/* Q_INVOKABLE */ QString WDeclarativePlayer::audioName(int id) const
{
    Q_D(const WDeclarativePlayer);

    return d->player->audioName(id);
}

//-------------------------------------------------------------------------------------------------
// Virtual interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QString WDeclarativePlayer::toVbml(const QString & source,
                                                             int             currentTime) const
{
    Q_D(const WDeclarativePlayer);

    return d->player->toVbml(source, currentTime);
}

#if defined(QT_4) || defined(SK_SOFTWARE)

//-------------------------------------------------------------------------------------------------
// QGraphicsItem / QQuickPaintedItem reimplementation
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

    WAbstractBackend * backend = this->backend();

    if (backend)
    {
#ifdef QT_4
        backend->drawFrame(painter, option->rect);
#else
        backend->drawFrame(painter, boundingRect().toRect());
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

                node->setRect(backend()->getRect());
            }

            return node;
        }
        else return NULL;
    }
    else if (state == WAbstractBackend::FrameReset)
    {
        d->frame.state = WAbstractBackend::FrameDefault;

        if (oldNode) delete oldNode;

        WAbstractBackend * backend = this->backend();

        WBackendNode * node = backend->createNode();

        node->setTextures(d->frame.textures);

        d->frameUpdate = false;

        node->setRect(backend->getRect());

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

                node->setRect(backend()->getRect());
            }
        }
        else
        {
            WAbstractBackend * backend = this->backend();

            node = backend->createNode();

            node->setTextures(d->frame.textures);

            d->frameUpdate = false;

            node->setRect(backend->getRect());
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

    if (oldGeometry.size() != newGeometry.size())
    {
        WAbstractBackend * backend = this->backend();

        if (backend)
        {
            backend->setSize(newGeometry.size());

#if defined(QT_NEW) && defined(SK_SOFTWARE) == false
            d->frameUpdate = true;
#endif
        }
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
// Properties
//-------------------------------------------------------------------------------------------------

WPlayer * WDeclarativePlayer::player() const
{
    Q_D(const WDeclarativePlayer); return d->player;
}

WAbstractBackend * WDeclarativePlayer::backend() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->backend();
}

void WDeclarativePlayer::setBackend(WAbstractBackend * backend)
{
    Q_D(WDeclarativePlayer);

    if (d->player->applyBackend(backend) == false) return;

    backend->setSize(QSizeF(width(), height()));
}

QList<WAbstractHook *> WDeclarativePlayer::hooks() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->hooks();
}

void WDeclarativePlayer::setHooks(const QList<WAbstractHook *> & hooks)
{
    Q_D(WDeclarativePlayer);

    d->player->setHooks(hooks);
}

//-------------------------------------------------------------------------------------------------

WBroadcastServer * WDeclarativePlayer::server() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->server();
}

void WDeclarativePlayer::setServer(WBroadcastServer * server)
{
    Q_D(WDeclarativePlayer);

    d->player->setServer(server);
}

//-------------------------------------------------------------------------------------------------

QString WDeclarativePlayer::source() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->source();
}

void WDeclarativePlayer::setSource(const QString & url)
{
    Q_D(WDeclarativePlayer);

    d->player->setSource(url);
}

//-------------------------------------------------------------------------------------------------

WPlaylist * WDeclarativePlayer::playlist() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->playlist();
}

void WDeclarativePlayer::setPlaylist(WPlaylist * playlist)
{
    Q_D(WDeclarativePlayer);

    d->player->setPlaylist(playlist);
}

//-------------------------------------------------------------------------------------------------

WAbstractBackend::State WDeclarativePlayer::state() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->state();
}

WAbstractBackend::StateLoad WDeclarativePlayer::stateLoad() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->stateLoad();
}

//-------------------------------------------------------------------------------------------------

bool WDeclarativePlayer::isLoading() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->isLoading();
}

//-------------------------------------------------------------------------------------------------

bool WDeclarativePlayer::isDefault() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->isDefault();
}

bool WDeclarativePlayer::isStarting() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->isStarting();
}

bool WDeclarativePlayer::isResuming() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->isResuming();
}

bool WDeclarativePlayer::isBuffering() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->isBuffering();
}

//-------------------------------------------------------------------------------------------------

bool WDeclarativePlayer::isPlaying() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->isPlaying();
}

bool WDeclarativePlayer::isPaused() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->isPaused();
}

bool WDeclarativePlayer::isStopped() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->isStopped();
}

//-------------------------------------------------------------------------------------------------

bool WDeclarativePlayer::isVideo() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->isVideo();
}

bool WDeclarativePlayer::isAudio() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->isAudio();
}

//-------------------------------------------------------------------------------------------------

bool WDeclarativePlayer::isVbml() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->isVbml();
}

bool WDeclarativePlayer::isLive() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->isLive();
}

//-------------------------------------------------------------------------------------------------

bool WDeclarativePlayer::hasStarted() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->hasStarted();
}

bool WDeclarativePlayer::hasEnded() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->hasEnded();
}

//-------------------------------------------------------------------------------------------------

bool WDeclarativePlayer::hasOutput() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->hasOutput();
}

//-------------------------------------------------------------------------------------------------

int WDeclarativePlayer::currentTime() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->currentTime();
}

int WDeclarativePlayer::duration() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->duration();
}

//-------------------------------------------------------------------------------------------------

qreal WDeclarativePlayer::progress() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->progress();
}

//-------------------------------------------------------------------------------------------------

qreal WDeclarativePlayer::speed() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->speed();
}

void WDeclarativePlayer::setSpeed(qreal speed)
{
    Q_D(WDeclarativePlayer);

    d->player->setSpeed(speed);
}

//-------------------------------------------------------------------------------------------------

qreal WDeclarativePlayer::volume() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->volume();
}

void WDeclarativePlayer::setVolume(qreal volume)
{
    Q_D(WDeclarativePlayer);

    d->player->setVolume(volume);
}

//-------------------------------------------------------------------------------------------------

bool WDeclarativePlayer::autoPlay() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->autoPlay();
}

void WDeclarativePlayer::setAutoPlay(bool autoPlay)
{
    Q_D(WDeclarativePlayer);

    d->player->setAutoPlay(autoPlay);
}

//-------------------------------------------------------------------------------------------------

bool WDeclarativePlayer::shuffle() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->shuffle();
}

void WDeclarativePlayer::setShuffle(bool shuffle)
{
    Q_D(WDeclarativePlayer);

    d->player->setShuffle(shuffle);
}

//-------------------------------------------------------------------------------------------------

WDeclarativePlayer::Repeat WDeclarativePlayer::repeat() const
{
    Q_D(const WDeclarativePlayer);

    return static_cast<Repeat> (d->player->repeat());
}

void WDeclarativePlayer::setRepeat(Repeat repeat)
{
    Q_D(WDeclarativePlayer);

    d->player->setRepeat(static_cast<WPlayer::Repeat> (repeat));
}

//-------------------------------------------------------------------------------------------------

WAbstractBackend::Output WDeclarativePlayer::output() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->output();
}

void WDeclarativePlayer::setOutput(WAbstractBackend::Output output)
{
    Q_D(WDeclarativePlayer);

    d->player->setOutput(output);
}

WAbstractBackend::Quality WDeclarativePlayer::quality() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->quality();
}

void WDeclarativePlayer::setQuality(WAbstractBackend::Quality quality)
{
    Q_D(WDeclarativePlayer);

    d->player->setQuality(quality);
}

WAbstractBackend::SourceMode WDeclarativePlayer::sourceMode() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->sourceMode();
}

void WDeclarativePlayer::setSourceMode(WAbstractBackend::SourceMode mode)
{
    Q_D(WDeclarativePlayer);

    d->player->setSourceMode(mode);
}

//-------------------------------------------------------------------------------------------------

WAbstractBackend::Output WDeclarativePlayer::outputActive() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->outputActive();
}

WAbstractBackend::Quality WDeclarativePlayer::qualityActive() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->qualityActive();
}

//-------------------------------------------------------------------------------------------------

WAbstractBackend::FillMode WDeclarativePlayer::fillMode() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->fillMode();
}

void WDeclarativePlayer::setFillMode(WAbstractBackend::FillMode fillMode)
{
    Q_D(WDeclarativePlayer);

    d->player->setFillMode(fillMode);

#if defined(QT_NEW) && defined(SK_SOFTWARE) == false
    d->frameUpdate = true;
#endif

    update();
}

//-------------------------------------------------------------------------------------------------

int WDeclarativePlayer::trackVideo() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->trackVideo();
}

void WDeclarativePlayer::setTrackVideo(int id)
{
    Q_D(WDeclarativePlayer);

    d->player->setTrackVideo(id);
}

int WDeclarativePlayer::trackAudio() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->trackAudio();
}

void WDeclarativePlayer::setTrackAudio(int id)
{
    Q_D(WDeclarativePlayer);

    d->player->setTrackAudio(id);
}

int WDeclarativePlayer::countVideos() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->countVideos();
}

int WDeclarativePlayer::countAudios() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->countAudios();
}

//-------------------------------------------------------------------------------------------------

bool WDeclarativePlayer::scanOutput() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->scanOutput();
}

void WDeclarativePlayer::setScanOutput(bool enabled)
{
    Q_D(WDeclarativePlayer);

    d->player->setScanOutput(enabled);
}

int WDeclarativePlayer::currentOutput() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->currentOutput();
}

void WDeclarativePlayer::setCurrentOutput(int index)
{
    Q_D(WDeclarativePlayer);

    d->player->setCurrentOutput(index);
}

QString WDeclarativePlayer::outputName() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->outputName();
}

WAbstractBackend::OutputType WDeclarativePlayer::outputType() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->outputType();
}

int WDeclarativePlayer::countOutputs() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->countOutputs();
}

//-------------------------------------------------------------------------------------------------

QString WDeclarativePlayer::subtitle() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->subtitle();
}

void WDeclarativePlayer::setSubtitle(const QString & subtitle)
{
    Q_D(WDeclarativePlayer);

    d->player->setSubtitle(subtitle);
}

QString WDeclarativePlayer::context() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->context();
}

QString WDeclarativePlayer::contextId() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->contextId();
}

QList<WChapter> WDeclarativePlayer::chapters() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->chapters();
}

QString WDeclarativePlayer::ambient() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->ambient();
}

QList<WSubtitle> WDeclarativePlayer::subtitles() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->subtitles();
}

//-------------------------------------------------------------------------------------------------

int WDeclarativePlayer::pauseTimeout() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->pauseTimeout();
}

void WDeclarativePlayer::setPauseTimeout(int msec)
{
    Q_D(WDeclarativePlayer);

    d->player->setPauseTimeout(msec);
}

//-------------------------------------------------------------------------------------------------

int WDeclarativePlayer::count() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->count();
}

//-------------------------------------------------------------------------------------------------

bool WDeclarativePlayer::hasPreviousTrack() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->hasPreviousTrack();
}

bool WDeclarativePlayer::hasNextTrack() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->hasNextTrack();
}

//-------------------------------------------------------------------------------------------------

WTrack::Type WDeclarativePlayer::trackType() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->trackType();
}

bool WDeclarativePlayer::trackIsLive() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->trackIsLive();
}

bool WDeclarativePlayer::trackIsHub() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->trackIsHub();
}

bool WDeclarativePlayer::trackIsChannel() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->trackIsChannel();
}

bool WDeclarativePlayer::trackIsInteractive() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->trackIsInteractive();
}

bool WDeclarativePlayer::trackIsLite() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->trackIsLite();
}

//-------------------------------------------------------------------------------------------------

WTrack::State WDeclarativePlayer::trackState() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->trackState();
}

bool WDeclarativePlayer::trackIsDefault() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->trackIsDefault();
}

bool WDeclarativePlayer::trackIsLoading() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->trackIsLoading();
}

bool WDeclarativePlayer::trackIsLoaded() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->trackIsLoaded();
}

//-------------------------------------------------------------------------------------------------

QString WDeclarativePlayer::trackTitle() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->trackTitle();
}

QString WDeclarativePlayer::trackCover() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->trackCover();
}

//-------------------------------------------------------------------------------------------------

int WDeclarativePlayer::trackCurrentTime() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->trackCurrentTime();
}

int WDeclarativePlayer::trackDuration() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->trackDuration();
}

QDateTime WDeclarativePlayer::trackDate() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->trackDate();
}

//-------------------------------------------------------------------------------------------------

int WDeclarativePlayer::trackIndex() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->trackIndex();
}

//-------------------------------------------------------------------------------------------------

WTabsTrack * WDeclarativePlayer::tabs() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->tabs();
}

void WDeclarativePlayer::setTabs(WTabsTrack * tabs)
{
    Q_D(WDeclarativePlayer);

    d->player->setTabs(tabs);
}

//-------------------------------------------------------------------------------------------------

WTabTrack * WDeclarativePlayer::tab() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->tab();
}

int WDeclarativePlayer::tabIndex() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->tabIndex();
}

//-------------------------------------------------------------------------------------------------

bool WDeclarativePlayer::videoTag() const
{
    Q_D(const WDeclarativePlayer);

    return d->player->videoTag();
}

void WDeclarativePlayer::setVideoTag(bool enabled)
{
    Q_D(WDeclarativePlayer);

    d->player->setVideoTag(enabled);
}

#endif // SK_NO_DECLARATIVEPLAYER
