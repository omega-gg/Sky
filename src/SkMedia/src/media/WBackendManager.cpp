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

#include "WBackendManager.h"

// Sk includes
#include <WControllerPlaylist>
#include <WControllerMedia>
#include <WBackendVlc>
#include <WHookTorrent>

// Private includes
#include <private/WBackendVlc_p>

#ifndef SK_NO_BACKENDMANAGER

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

WBackendManagerPrivate::WBackendManagerPrivate(WBackendManager * p) : WAbstractBackendPrivate(p) {}

/* virtual */ WBackendManagerPrivate::~WBackendManagerPrivate()
{
    foreach (const WBackendManagerItem & item, items)
    {
#ifndef SK_NO_TORRENT
        delete item.hook;
#endif

        item.backend->deleteBackend();
    }
}

//-------------------------------------------------------------------------------------------------

void WBackendManagerPrivate::init()
{
    Q_Q(WBackendManager);

    WBackendManagerItem item;

    WBackendVlc * backendVlc = new WBackendVlc(q);

    item.backend = backendVlc;

#ifndef SK_NO_TORRENT
    item.hook = new WHookTorrent(backendVlc);
#endif

    items.append(item);

    backend          = NULL;
    backendInterface = NULL;

    reply = NULL;
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WBackendManagerPrivate::loadSources()
{
    if (reply) return;

    Q_Q(WBackendManager);

    WAbstractBackend::SourceMode mode = q->getMode();

    // NOTE: When using Chromecast for video we want to increase source compatibility.
    if (outputData.type == WAbstractBackend::OutputChromecast
        &&
        mode != WAbstractBackend::SourceAudio)
    {
         reply = wControllerMedia->getMedia(source, q, WAbstractBackend::SourceSafe, currentTime);
    }
    else reply = wControllerMedia->getMedia(source, q, mode, currentTime);

    if (reply == NULL) return;

    if (reply->isLoaded())
    {
        applySources(true);

        delete reply;

        reply = NULL;
    }
    else QObject::connect(reply, SIGNAL(loaded(WMediaReply *)), q, SLOT(onLoaded()));
}

void WBackendManagerPrivate::applySources(bool play)
{
    medias = reply->medias();
    audios = reply->audios();

    QString media = WAbstractBackend::mediaFromQuality(medias, quality);

    if (media.isEmpty())
    {
        Q_Q(WBackendManager);

        q->stop();

        return;
    }

    applyBackend(media);

    backendInterface->loadSource(source, duration, currentTime);

    if (play) backendInterface->play();
}

void WBackendManagerPrivate::applyBackend(const QString & source)
{
#ifdef SK_NO_TORRENT
    Q_UNUSED(source);
#endif

    setBackend(items.first().backend);

#ifdef SK_NO_TORRENT
    backendInterface = backend;
#else
    if (WControllerPlaylist::urlIsTorrent(source))
    {
         backendInterface = items.first().hook;
    }
    else backendInterface = backend;
#endif
}

//-------------------------------------------------------------------------------------------------

void WBackendManagerPrivate::updateLoading()
{
    Q_Q(WBackendManager);

    if (currentTime == -1)
    {
         q->setStateLoad(WAbstractBackend::StateLoadStarting);
    }
    else q->setStateLoad(WAbstractBackend::StateLoadResuming);
}

void WBackendManagerPrivate::clearActive()
{
    Q_Q(WBackendManager);

    q->setOutputActive (WAbstractBackend::OutputNone);
    q->setQualityActive(WAbstractBackend::QualityDefault);
}

void WBackendManagerPrivate::clearReply()
{
    if (reply == NULL) return;

    delete reply;

    reply = NULL;
}

void WBackendManagerPrivate::clearMedia()
{
    clearReply();
}

//-------------------------------------------------------------------------------------------------

void WBackendManagerPrivate::setBackend(WAbstractBackend * backendNew)
{
    if (backend == backendNew) return;

    Q_Q(WBackendManager);

    if (backend) QObject::disconnect(backend, 0, q, 0);

    backend = backendNew;

#ifndef SK_NO_QML
    backend->setPlayer(player);
#endif

    backend->setVolume(volume);

    backend->setSpeed(speed);

    backend->setOutput (output);
    backend->setQuality(quality);

    backend->setFillMode(fillMode);

    backend->setTrackVideo(trackVideo);
    backend->setTrackAudio(trackAudio);

    backend->setSize(size);

    QObject::connect(backend, SIGNAL(stateChanged      ()), q, SLOT(onState     ()));
    QObject::connect(backend, SIGNAL(stateLoadChanged  ()), q, SLOT(onStateLoad ()));
    QObject::connect(backend, SIGNAL(currentTimeChanged()), q, SLOT(onTime      ()));
    QObject::connect(backend, SIGNAL(videosChanged     ()), q, SLOT(onVideos    ()));
    QObject::connect(backend, SIGNAL(audiosChanged     ()), q, SLOT(onAudios    ()));
    QObject::connect(backend, SIGNAL(trackVideoChanged ()), q, SLOT(onTrackVideo()));
    QObject::connect(backend, SIGNAL(trackAudioChanged ()), q, SLOT(onTrackAudio()));
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WBackendManagerPrivate::onLoaded()
{
    Q_Q(WBackendManager);

    if (reply->hasError())
    {
        q->stop();
    }
    else applySources(q->isPlaying());

    reply->deleteLater();

    reply = NULL;
}

//-------------------------------------------------------------------------------------------------

void WBackendManagerPrivate::onState()
{
    Q_Q(WBackendManager);

    q->setState(backend->state());
}

void WBackendManagerPrivate::onStateLoad()
{
    Q_Q(WBackendManager);

    q->setStateLoad(backend->stateLoad());
}

void WBackendManagerPrivate::onTime()
{

}

void WBackendManagerPrivate::onVideos()
{
    Q_Q(WBackendManager);

    q->applyVideos(backend->videos(), trackVideo);
}

void WBackendManagerPrivate::onAudios()
{
    Q_Q(WBackendManager);

    q->applyAudios(backend->audios(), trackAudio);
}

void WBackendManagerPrivate::onTrackVideo()
{
    Q_Q(WBackendManager);

    q->setTrackVideo(backend->trackVideo());
}

void WBackendManagerPrivate::onTrackAudio()
{
    Q_Q(WBackendManager);

    q->setTrackAudio(backend->trackAudio());
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

WBackendManager::WBackendManager(QObject * parent)
    : WAbstractBackend(new WBackendManagerPrivate(this), parent)
{
    Q_D(WBackendManager); d->init();
}

//-------------------------------------------------------------------------------------------------
// Protected WAbstractBackend implementation
//-------------------------------------------------------------------------------------------------

#ifdef QT_NEW

/* virtual */ WBackendNode * WBackendManager::backendCreateNode() const
{
    return new WBackendVlcNode;
}

#endif

/* virtual */ bool WBackendManager::backendSetSource(const QString & url)
{
    Q_D(WBackendManager);

    d->clearMedia();

    if (url.isEmpty())
    {
        if (d->backendInterface == NULL) return true;

        d->backendInterface->stop();

        d->clearActive();
    }
    else if (isPlaying())
    {
        d->updateLoading();

        backendStop();

        d->loadSources();
    }

    return true;
}

//-------------------------------------------------------------------------------------------------

/* virtual */ bool WBackendManager::backendPlay()
{
    Q_D(WBackendManager);

    if (isPaused() == false && d->currentMedia.isEmpty())
    {
        d->loadSources();

        d->updateLoading();
    }
    else if (d->backendInterface) d->backendInterface->play();

    return true;
}

/* virtual */ bool WBackendManager::backendPause()
{
    Q_D(WBackendManager);

    if (d->backendInterface) d->backendInterface->pause();

    return true;
}

/* virtual */ bool WBackendManager::backendStop()
{
    Q_D(WBackendManager);

    if (d->backendInterface) d->backendInterface->stop();

    d->clearActive();
    d->clearMedia ();

    return true;
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WBackendManager::backendSetVolume(qreal volume)
{
    Q_D(WBackendManager);

    if (d->backend) d->backend->setVolume(volume);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ bool WBackendManager::backendDelete()
{
    return false;
}

//-------------------------------------------------------------------------------------------------
// Protected WAbstractBackend reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WBackendManager::backendSeek(int msec)
{
    Q_D(WBackendManager);

    if (d->backendInterface) d->backendInterface->seek(msec);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WBackendManager::backendSetSpeed(qreal speed)
{
    Q_D(WBackendManager);

    if (d->backend) d->backend->setSpeed(speed);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WBackendManager::backendSetOutput(Output output)
{
    Q_D(WBackendManager);

    if (d->backend) d->backend->setOutput(output);
}

/* virtual */ void WBackendManager::backendSetQuality(Quality quality)
{
    Q_D(WBackendManager);

    if (d->backend) d->backend->setQuality(quality);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WBackendManager::backendSetFillMode(FillMode fillMode)
{
    Q_D(WBackendManager);

    if (d->backend) d->backend->setFillMode(fillMode);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WBackendManager::backendSetVideo(int id)
{
    Q_D(WBackendManager);

    if (d->backend) d->backend->setTrackVideo(id);
}

/* virtual */ void WBackendManager::backendSetAudio(int id)
{
    Q_D(WBackendManager);

    if (d->backend) d->backend->setTrackAudio(id);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WBackendManager::backendSetScanOutput(bool enabled)
{
}

/* virtual */ void WBackendManager::backendSetCurrentOutput(const WBackendOutput * output)
{
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WBackendManager::backendSetSize(const QSizeF & size)
{
    Q_D(WBackendManager);

    if (d->backend) d->backend->setSize(size);
}

//-------------------------------------------------------------------------------------------------

#ifdef QT_NEW

/* virtual */ void WBackendManager::backendSynchronize(WBackendFrame * frame)
{
    Q_D(WBackendManager);

    if (d->backend) d->backend->synchronize(frame);
}

#endif

//-------------------------------------------------------------------------------------------------

#ifdef QT_4
/* virtual */ void WBackendManager::backendDrawFrame(QPainter * painter, const QRect & rect)
#else
/* virtual */ void WBackendManager::backendDrawFrame(QPainter * painter, const QRect & rect)
#endif
{
    Q_D(WBackendManager);

    if (d->backend) d->backend->drawFrame(painter, rect);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WBackendManager::backendUpdateFrame()
{
    Q_D(WBackendManager);

    if (d->backend) d->backend->updateFrame();
}

/* virtual */ QImage WBackendManager::backendGetFrame() const
{
    Q_D(const WBackendManager);

    if (d->backend)
    {
        return d->backend->getFrame();
    }
    else return QImage();
}

//-------------------------------------------------------------------------------------------------

/* virtual */ QRectF WBackendManager::backendRect() const
{
    Q_D(const WBackendManager);

    if (d->backend)
    {
        return d->backend->getRect();
    }
    else return QRectF();
}

#endif // SK_NO_BACKENDMANAGER
