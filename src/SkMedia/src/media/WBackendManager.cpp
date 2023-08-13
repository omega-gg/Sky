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

#ifndef SK_NO_BACKENDMANAGER

// Sk includes
#include <WControllerPlaylist>
#include <WControllerMedia>
#include <WBackendVlc>
#include <WAbstractHook>

// Private includes
#include <private/WBackendVlc_p>

//-------------------------------------------------------------------------------------------------
// Static variables

static const int BACKENDMANAGER_TIMEOUT = 200;

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

WBackendManagerPrivate::WBackendManagerPrivate(WBackendManager * p) : WAbstractBackendPrivate(p) {}

/* virtual */ WBackendManagerPrivate::~WBackendManagerPrivate()
{
    foreach (const WBackendManagerItem & item, items)
    {
        WAbstractHook * hook = item.hook;

        if (hook) delete hook;

        item.backend->deleteBackend();
    }
}

//-------------------------------------------------------------------------------------------------

void WBackendManagerPrivate::init()
{
    Q_Q(WBackendManager);

    WBackendManagerItem item;

    backend = new WBackendVlc(q);

    item.backend = backend;
    item.hook    = NULL;

    items.append(item);

    currentItem = &(items.first());

    backendInterface = backend;

    reply = NULL;

    type = Track;

    timeA = -1;
    timeB = -1;

    start = -1;

    loaded    = false;
    connected = false;

    timer = -1;

#ifndef SK_NO_QML
    QObject::connect(q, SIGNAL(playerChanged()), q, SLOT(onPlayerChanged()));
#endif

    //---------------------------------------------------------------------------------------------
    // NOTE: We use the first backend for Chromecast output detection.

    QObject::connect(backend, SIGNAL(outputAdded(const WBackendOutput &)),
                     q,       SLOT(onOutputAdded(const WBackendOutput &)));

    QObject::connect(backend, SIGNAL(outputRemoved(int)), q, SLOT(onOutputRemoved(int)));

    QObject::connect(backend, SIGNAL(currentOutputChanged()), q, SLOT(onOutputChanged()));
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WBackendManagerPrivate::loadSources()
{
    if (reply) return;

    WAbstractHook * hook = currentItem->hook;

    if (hook)
    {
        if (hook->check(source))
        {
            setBackendInterface(hook);

            type = Track;

            backendInterface->loadSource(source, duration, currentTime, NULL);

            backendInterface->play();

            connectBackend();

            return;
        }

        setBackendInterface(backend);
    }

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
    Q_Q(WBackendManager);

    medias = reply->medias();

    currentMedia = WAbstractBackend::mediaFromQuality(medias, quality);

    int timeA = reply->timeA();

    if (timeA == -1)
    {
        if (currentMedia.isEmpty())
        {
            q->stop();

            return;
        }

        loaded = true;

        type = Track;

        loadSource(source, currentMedia, currentTime);

        if (play) backendInterface->play();

        connectBackend();
    }
    else
    {
        Q_Q(WBackendManager);

        loaded = true;

        type = MultiTrack;

        this->timeA = timeA;

        timeB = reply->timeB();

        start = reply->start();

        q->setDuration(reply->duration());

        qDebug("Current source: timeA %d timeB %d start %d duration %d", timeA, timeB, start,
               duration);

        if (currentMedia.isEmpty() == false)
        {
            loadSource(source, currentMedia, currentTime - timeA + start);

            if (play) backendInterface->play();

            connectBackend();
        }
        else if (play)
        {
            time.restart();

            timer = q->startTimer(BACKENDMANAGER_TIMEOUT);
        }
    }
}

void WBackendManagerPrivate::loadSource(const QString & source,
                                        const QString & media, int currentTime)
{
    WAbstractHook * hook = currentItem->hook;

    if (hook)
    {
        if (hook->check(media))
        {
            setBackendInterface(hook);

            backendInterface->loadSource(media, duration, currentTime, NULL);

            return;
        }

        setBackendInterface(backend);
    }

    backendInterface->loadSource(source, duration, currentTime, reply);
}

void WBackendManagerPrivate::applyDefault()
{
    Q_Q(WBackendManager);

    clearMedia();

    disconnectBackend();

    backendInterface->stop();

    loaded = true;

    timeA = currentTime;

    time.restart();

    timer = q->startTimer(BACKENDMANAGER_TIMEOUT);

    q->setStateLoad(WAbstractBackend::StateLoadDefault);

    qDebug("Clear source: timeA %d timeB %d start %d", timeA, timeB, start);
}

void WBackendManagerPrivate::applyNextSource()
{
    if (state != WAbstractBackend::StatePlaying) return;

    clearMedia();

    disconnectBackend();

    backendInterface->stop();

    loadSources();
}

void WBackendManagerPrivate::applyTime(int currentTime)
{
    Q_Q(WBackendManager);

    qDebug("APPLY TIME %d", currentTime);

    if (currentTime >= duration)
    {
        q->setEnded(true);
    }
    else if (currentTime > timeB)
    {
        q->setCurrentTime(qMax(0, currentTime));

        applyNextSource();
    }
    else q->setCurrentTime(qMax(0, currentTime));
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
    stopTimer();

    clearReply();

    loaded = false;

    currentMedia = QString();
}

void WBackendManagerPrivate::stopTimer()
{
    if (timer == -1) return;

    Q_Q(WBackendManager);

    q->killTimer(timer);

    timer = -1;
}

//-------------------------------------------------------------------------------------------------

void WBackendManagerPrivate::connectBackend()
{
    if (connected) return;

    connected = true;

    Q_Q(WBackendManager);

    //---------------------------------------------------------------------------------------------
    // NOTE: We update the manager with the useful backend properties.

    WAbstractBackendPrivate * p = backend->d_func();

    if (type == Track)
    {
        q->setStateLoad(p->stateLoad);
    }
    // NOTE: For multi-tracks we want to avoid starting and resuming states when transitioning
    //       between two tracks.
    else if (stateLoad == WAbstractBackend::StateLoadDefault
             &&
             p->stateLoad != WAbstractBackend::StateLoadDefault)
    {
        q->setStateLoad(WAbstractBackend::StateLoadBuffering);
    }

    q->setProgress(p->progress);

    //---------------------------------------------------------------------------------------------

    QObject::connect(backend, SIGNAL(stateChanged        ()), q, SLOT(onState      ()));
    QObject::connect(backend, SIGNAL(stateLoadChanged    ()), q, SLOT(onStateLoad  ()));
    QObject::connect(backend, SIGNAL(liveChanged         ()), q, SLOT(onLive       ()));
    QObject::connect(backend, SIGNAL(startedChanged      ()), q, SLOT(onStarted    ()));
    QObject::connect(backend, SIGNAL(endedChanged        ()), q, SLOT(onEnded      ()));
    QObject::connect(backend, SIGNAL(currentTimeChanged  ()), q, SLOT(onCurrentTime()));
    QObject::connect(backend, SIGNAL(durationChanged     ()), q, SLOT(onDuration   ()));
    QObject::connect(backend, SIGNAL(progressChanged     ()), q, SLOT(onProgress   ()));
    QObject::connect(backend, SIGNAL(outputActiveChanged ()), q, SLOT(onOutput     ()));
    QObject::connect(backend, SIGNAL(qualityActiveChanged()), q, SLOT(onQuality    ()));
    QObject::connect(backend, SIGNAL(videosChanged       ()), q, SLOT(onVideos     ()));
    QObject::connect(backend, SIGNAL(audiosChanged       ()), q, SLOT(onAudios     ()));
    QObject::connect(backend, SIGNAL(trackVideoChanged   ()), q, SLOT(onTrackVideo ()));
    QObject::connect(backend, SIGNAL(trackAudioChanged   ()), q, SLOT(onTrackAudio ()));

    QObject::connect(backend, SIGNAL(error(const QString &)), q, SLOT(onError(const QString &)));
}

void WBackendManagerPrivate::disconnectBackend()
{
    if (connected == false) return;

    connected = false;

    Q_Q(WBackendManager);

    // NOTE: We want to avoid signals while updating the backend.
    QObject::disconnect(backend, 0, q, 0);
}

/*void WBackendManagerPrivate::setBackend(WAbstractBackend * backendNew)
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
}*/

void WBackendManagerPrivate::setBackendInterface(WBackendInterface * backendNew)
{
    if (backendInterface == backendNew) return;

    backendInterface->clear();

    backendInterface = backendNew;
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

#ifndef SK_NO_QML

void WBackendManagerPrivate::onPlayerChanged()
{
    backend->setPlayer(player);
}

#endif

//-------------------------------------------------------------------------------------------------

void WBackendManagerPrivate::onState()
{
    Q_Q(WBackendManager);

    qDebug("STATE %d", backend->state());

    q->setState(backend->state());
}

void WBackendManagerPrivate::onStateLoad()
{
    Q_Q(WBackendManager);

    qDebug("STATE LOAD %d", backend->stateLoad());

    q->setStateLoad(backend->stateLoad());
}

void WBackendManagerPrivate::onLive()
{
    Q_Q(WBackendManager);

    q->setLive(backend->isLive());
}

void WBackendManagerPrivate::onStarted()
{
    Q_Q(WBackendManager);

    qDebug("STARTED");

    q->setStarted(backend->hasStarted());
}

void WBackendManagerPrivate::onEnded()
{
    if (type == Track)
    {
        backend->setEnded(backend->hasEnded());

        return;
    }

    if (backend->hasEnded() == false
        ||
        state != WAbstractBackend::StatePlaying) return;

    qDebug("ENDED");

    applyDefault();
}

void WBackendManagerPrivate::onCurrentTime()
{
    if (type == Track)
    {
        Q_Q(WBackendManager);

        q->setCurrentTime(backend->currentTime());

        return;
    }

    int currentTime = backend->currentTime();

    if (currentTime == -1) return;

    applyTime(timeA + currentTime - start);
}

void WBackendManagerPrivate::onDuration()
{
    if (type == Track)
    {
        Q_Q(WBackendManager);

        q->setDuration(backend->duration());

        return;
    }

    if (currentTime - timeA < backend->duration() - start) return;

    applyDefault();
}

void WBackendManagerPrivate::onProgress()
{
    Q_Q(WBackendManager);

    q->setProgress(backend->progress());
}

void WBackendManagerPrivate::onOutput()
{
    Q_Q(WBackendManager);

    q->setOutputActive(backend->outputActive());
}

void WBackendManagerPrivate::onQuality()
{
    Q_Q(WBackendManager);

    q->setQualityActive(backend->qualityActive());
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

void WBackendManagerPrivate::onError(const QString & message)
{
    Q_Q(WBackendManager);

    q->stopError(message);
}

//-------------------------------------------------------------------------------------------------

void WBackendManagerPrivate::onOutputAdded(const WBackendOutput & output)
{
    Q_Q(WBackendManager); q->addOutput(output);
}

void WBackendManagerPrivate::onOutputRemoved(int index)
{
    Q_Q(WBackendManager); q->removeOutput(q->outputPointerAt(index));
}

void WBackendManagerPrivate::onOutputChanged()
{
    Q_Q(WBackendManager);

    WAbstractBackend * backend = items.first().backend;

    q->setCurrentOutput(backend->currentOutput());
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
// Protected

WBackendManager::WBackendManager(WBackendManagerPrivate * p, QObject * parent)
    : WAbstractBackend(p, parent)
{
    Q_D(WBackendManager); d->init();
}

//-------------------------------------------------------------------------------------------------
// Protected virtual functions
//-------------------------------------------------------------------------------------------------

/* virtual */ WAbstractHook * WBackendManager::createHook(WAbstractBackend *)
{
    return NULL;
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

/* virtual */ bool WBackendManager::backendSetSource(const QString & url, const WMediaReply *)
{
    Q_D(WBackendManager);

    d->clearMedia();

    if (url.isEmpty())
    {
        d->backendInterface->stop();

        d->clearActive();
    }
    else if (isPlaying())
    {
        d->updateLoading();

        d->disconnectBackend();

        d->backendInterface->stop();

        d->loadSources();
    }

    return true;
}

//-------------------------------------------------------------------------------------------------

/* virtual */ bool WBackendManager::backendPlay()
{
    Q_D(WBackendManager);

    if (d->loaded == false)
    {
        if (isPaused() == false)
        {
            d->updateLoading();

            d->disconnectBackend();

            d->loadSources();
        }
    }
    else if (d->currentMedia.isEmpty())
    {
        d->time.restart();

        d->timer = startTimer(BACKENDMANAGER_TIMEOUT);
    }
    else d->backendInterface->play();

    return true;
}

/* virtual */ bool WBackendManager::backendPause()
{
    Q_D(WBackendManager);

    if (d->loaded && d->currentMedia.isEmpty())
    {
        d->stopTimer();
    }
    else d->backendInterface->pause();

    return true;
}

/* virtual */ bool WBackendManager::backendStop()
{
    Q_D(WBackendManager);

    if (d->loaded && d->currentMedia.isEmpty())
    {
        d->stopTimer();
    }
    else d->backendInterface->stop();

    d->clearActive();

    return true;
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WBackendManager::backendSetVolume(qreal volume)
{
    Q_D(WBackendManager);

    d->backend->setVolume(volume);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ bool WBackendManager::backendDelete()
{
    Q_D(WBackendManager);

    d->clearMedia ();
    d->clearActive();

    foreach (const WBackendManagerItem & item, d->items)
    {
        WAbstractHook * hook = item.hook;

        if (hook) delete hook;

        item.backend->deleteBackend();
    }

    d->items.clear();

    return false;
}

//-------------------------------------------------------------------------------------------------
// Protected WAbstractBackend reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WBackendManager::backendSeek(int msec)
{
    Q_D(WBackendManager);

    if (d->type != WBackendManagerPrivate::Track)
    {
        if (d->loaded == false) return;

        if (msec < d->timeA || msec > d->timeB)
        {
            d->applyNextSource();
        }
        else if (d->currentMedia.isEmpty() == false)
        {
            msec -= d->timeA;

            if (msec < d->backend->duration() - d->start)
            {
                d->backendInterface->seek(msec + d->start);
            }
            else d->applyDefault();
        }
    }
    else d->backendInterface->seek(msec);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WBackendManager::backendSetSpeed(qreal speed)
{
    Q_D(WBackendManager);

    d->backend->setSpeed(speed);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WBackendManager::backendSetOutput(Output output)
{
    Q_D(WBackendManager);

    d->backend->setOutput(output);
}

/* virtual */ void WBackendManager::backendSetQuality(Quality quality)
{
    Q_D(WBackendManager);

    d->backend->setQuality(quality);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WBackendManager::backendSetFillMode(FillMode fillMode)
{
    Q_D(WBackendManager);

    d->backend->setFillMode(fillMode);
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

    d->backend->setTrackAudio(id);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WBackendManager::backendSetScanOutput(bool enabled)
{
    Q_D(WBackendManager);

    WAbstractBackend * backend = d->items.first().backend;

    backend->setScanOutput(enabled);
}

/* virtual */ void WBackendManager::backendSetCurrentOutput(const WBackendOutput * output)
{
    Q_D(WBackendManager);

    WAbstractBackend * backend = d->items.first().backend;

    backend->setCurrentOutput(indexOutput(output));
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WBackendManager::backendSetSize(const QSizeF & size)
{
    Q_D(WBackendManager);

    d->backend->setSize(size);
}

//-------------------------------------------------------------------------------------------------

#ifdef QT_NEW

/* virtual */ void WBackendManager::backendSynchronize(WBackendFrame * frame)
{
    Q_D(WBackendManager);

    d->backend->synchronize(frame);
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

    d->backend->drawFrame(painter, rect);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WBackendManager::backendUpdateFrame()
{
    Q_D(WBackendManager);

    d->backend->updateFrame();
}

/* virtual */ QImage WBackendManager::backendGetFrame() const
{
    Q_D(const WBackendManager);

    return d->backend->getFrame();
}

//-------------------------------------------------------------------------------------------------

/* virtual */ QRectF WBackendManager::backendRect() const
{
    Q_D(const WBackendManager);

    return d->backend->getRect();
}

//-------------------------------------------------------------------------------------------------
// Protected events
//-------------------------------------------------------------------------------------------------

/* virtual */ void WBackendManager::timerEvent(QTimerEvent *)
{
    Q_D(WBackendManager);

    int currentTime = d->currentTime + d->time.elapsed();

    d->time.restart();

    d->applyTime(currentTime);
}

#endif // SK_NO_BACKENDMANAGER
