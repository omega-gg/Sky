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
#include <WControllerApplication>
#include <WControllerNetwork>
#include <WControllerPlaylist>
#include <WControllerMedia>
#include <WBackendVlc>
#include <WAbstractHook>

// Private includes
#include <private/WBackendVlc_p>

//-------------------------------------------------------------------------------------------------
// Static variables

static const int BACKENDMANAGER_TIMEOUT_CLOCK       =   200;
static const int BACKENDMANAGER_TIMEOUT_SYNCHRONIZE =  3000;
static const int BACKENDMANAGER_TIMEOUT_RELOAD      = 30000; // 3 minutes

static const int BACKENDMANAGER_MAX_DELAY = 60000; // 1 minute

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

    loaded    = false;
    connected = false;
    clock     = false;
    loop      = false;
    freeze    = false;

    type = Track;

    timeA = -1;
    timeB = -1;

    start = 0;

    timerClock       = -1;
    timerSynchronize = -1;
    timerReload      = -1;

#ifndef SK_NO_QML
    QObject::connect(q, SIGNAL(playerChanged()), q, SLOT(onPlayerChanged()));
#endif

    QObject::connect(q, SIGNAL(repeatChanged()), q, SLOT(onRepeatChanged()));

    //---------------------------------------------------------------------------------------------
    // NOTE: We use the first backend for Chromecast output detection.

    QObject::connect(backend, SIGNAL(outputAdded(const WBackendOutput &)),
                     q,       SLOT(onOutputAdded(const WBackendOutput &)));

    QObject::connect(backend, SIGNAL(outputRemoved(int)), q, SLOT(onOutputRemoved(int)));
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WBackendManagerPrivate::loadSources(bool play)
{
    if (reply) return;

    WAbstractHook * hook = currentItem->hook;

    if (hook)
    {
        if (hook->check(source))
        {
            setBackendInterface(hook);

            loaded = true;

            type = Track;

            backendInterface->loadSource(source, duration, currentTime, NULL);

            // NOTE: We clear the arg so we don't apply it when seeking to another video slice.
            source = WControllerNetwork::removeFragmentValue(source, "arg");

            if (play == false) return;

            backendInterface->play();

            connectBackend();

            return;
        }

        setBackendInterface(backend);
    }

    loadMedia();

    // NOTE: We clear the arg so we don't apply it when seeking to another video slice.
    source = WControllerNetwork::removeFragmentValue(source, "arg");

    if (reply == NULL) return;

    if (reply->isLoaded())
    {
        applySources(play);

        delete reply;

        reply = NULL;
    }
    else QObject::connect(reply, SIGNAL(loaded(WMediaReply *)), q_func(), SLOT(onLoaded()));
}

void WBackendManagerPrivate::reloadSources(bool play)
{
    loadMedia();

    if (reply == NULL) return;

    if (reply->isLoaded())
    {
        if (urlSource != reply->urlSource())
        {
            applySources(play);
        }

        delete reply;

        reply = NULL;
    }
    else QObject::connect(reply, SIGNAL(loaded(WMediaReply *)), q_func(), SLOT(onReloaded()));
}

void WBackendManagerPrivate::loadMedia()
{
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
}

void WBackendManagerPrivate::applySources(bool play)
{
    Q_Q(WBackendManager);

    medias = reply->medias();

    currentMedia = WAbstractBackend::mediaFromQuality(medias, quality);

    q->setVbml(reply->isVbml());

    int timeA = reply->timeA();

    if (timeA == -1)
    {
        if (currentMedia.isEmpty())
        {
            q->stop();

            freeze = false;

            q->setAmbient(QString());

            q->setSubtitles(QStringList());

            return;
        }

        freeze = false;

        loaded = true;

        type = Track;

        q->setAmbient(reply->ambient());

        q->setSubtitles(reply->subtitles());

        loadSource(source, currentMedia, currentTime);

        if (play == false) return;

        backendInterface->play();

        connectBackend();

        return;
    }

    QString url = reply->urlSource();

    WTrack::Type typeRoot = reply->type();

    loaded = true;

    clock = (reply->typeSource() == WTrack::Hub);

    this->timeA = timeA;

    timeB = reply->timeB();

    start = reply->start();

    if (typeRoot == WTrack::Channel)
    {
        type = Channel;

        urlSource = url;

        timeZone = reply->timeZone();

        loop = true;

        q->setLive(true);

        q->setDuration(reply->duration());

        QDateTime date = WControllerApplication::currentDateUtc(timeZone);

        q->setCurrentTime(WControllerApplication::getMsecsWeek(date));
    }
    else if (typeRoot == WTrack::Interactive)
    {
        type = Interactive;

        q->setDuration(reply->duration());

        q->setCurrentTime(reply->currentTime());

        q->setContext(reply->context(), reply->contextId());
    }
    else
    {
        type = MultiTrack;

        loop = (typeRoot == WTrack::Hub);

        q->setDuration(reply->duration());
    }

    q->setAmbient(reply->ambient());

    q->setSubtitles(reply->subtitles());

    qDebug("Current source: %s timeA %d timeB %d start %d duration %d clock %d", source.C_STR,
           timeA, timeB, start, duration, clock);

    if (currentMedia.isEmpty() == false)
    {
        loadSource(url, currentMedia, currentTime - timeA + start);

        if (play == false) return;

        backendInterface->play();

        connectBackend();
    }
    else if (play)
    {
        startClock();

        if (type == Channel)
        {
            timerSynchronize = q->startTimer(BACKENDMANAGER_TIMEOUT_SYNCHRONIZE);
            timerReload      = q->startTimer(BACKENDMANAGER_TIMEOUT_RELOAD);
        }

        q->setStateLoad(WAbstractBackend::StateLoadDefault);
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

void WBackendManagerPrivate::stopBackend()
{
    clearMedia();

    disconnectBackend();

    backendInterface->stop();
}

void WBackendManagerPrivate::applyDefault()
{
    Q_Q(WBackendManager);

    stopBackend();

    loaded = true;

    timeA = currentTime;

    startClock();

    q->setStateLoad(WAbstractBackend::StateLoadDefault);

    qDebug("Clear source: timeA %d timeB %d start %d", timeA, timeB, start);
}

void WBackendManagerPrivate::applyTime(int currentTime)
{
    Q_Q(WBackendManager);

    time.restart();

    if (currentTime >= duration)
    {
        if (type == Interactive)
        {
            if (clock == false && timeB == duration)
            {
                stopBackend();

                q->setEnded(true);
            }
            // NOTE: When the last media is a hub we loop the playback on it. When playing the
            //       entire hub, we let the backend handle the loop for smoother playback.
            else if (backend->duration() != timeB - timeA)
            {
                freeze = true;

                q->seek(timeA);
            }
            else q->setCurrentTime(timeA);
        }
        else if (loop == false)
        {
            stopBackend();

            q->setEnded(true);
        }
        else q->seek(0);
    }
    else if (currentTime >= timeB)
    {
        q->setCurrentTime(currentTime);

        freeze = true;

        stopBackend();

        if (type == Interactive)
        {
            source = WControllerNetwork::removeFragmentValue(source, "id");
        }

        loadSources(q->isPlaying());
    }
    else q->setCurrentTime(currentTime);
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

void WBackendManagerPrivate::startClock()
{
    time.restart();

    timerClock = q_func()->startTimer(BACKENDMANAGER_TIMEOUT_CLOCK);
}

void WBackendManagerPrivate::stopClock()
{
    if (timerClock == -1) return;

    q_func()->killTimer(timerClock);

    timerClock = -1;
}

void WBackendManagerPrivate::startSynchronize()
{
    Q_Q(WBackendManager);

    QDateTime date = WControllerApplication::currentDateUtc(timeZone);

    int time = WControllerApplication::getMsecsWeek(date);

    if (time < backend->duration())
    {
        q->setCurrentTime(time);
    }
    // NOTE: When the time exceeds the duration we interpolate right away.
    else q->seek(time);

    timerSynchronize = q->startTimer(BACKENDMANAGER_TIMEOUT_SYNCHRONIZE);
    timerReload      = q->startTimer(BACKENDMANAGER_TIMEOUT_RELOAD);
}

void WBackendManagerPrivate::stopSynchronize()
{
    if (timerSynchronize == -1) return;

    Q_Q(WBackendManager);

    q->killTimer(timerSynchronize);
    q->killTimer(timerReload);

    timerSynchronize = -1;
    timerReload      = -1;
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

    q->setOutputActive(p->outputActive);

    q->setQualityActive(p->qualityActive);

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

    //---------------------------------------------------------------------------------------------
    // NOTE: We use the first backend for Chromecast output detection.

    QObject::connect(backend, SIGNAL(outputAdded(const WBackendOutput &)),
                     q,       SLOT(onOutputAdded(const WBackendOutput &)));

    QObject::connect(backend, SIGNAL(outputRemoved(int)), q, SLOT(onOutputRemoved(int)));
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

//-------------------------------------------------------------------------------------------------

void WBackendManagerPrivate::clearActive()
{
    Q_Q(WBackendManager);

    q->setProgress(0);

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
    stopClock      ();
    stopSynchronize();

    clearReply();

    loaded = false;
    clock  = false;
    loop   = false;

    currentMedia = QString();
}

//-------------------------------------------------------------------------------------------------

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

        freeze = false;

        q->setAmbient(QString());

        q->setSubtitles(QStringList());
    }
    else applySources(q->isPlaying());

    reply->deleteLater();

    reply = NULL;
}

void WBackendManagerPrivate::onReloaded()
{
    if (reply->hasError() == false && urlSource != reply->urlSource())
    {
        Q_Q(WBackendManager);

        disconnectBackend();

        applySources(q->isPlaying());
    }

    reply->deleteLater();

    reply = NULL;
}

#ifndef SK_NO_QML

void WBackendManagerPrivate::onPlayerChanged()
{
    backend->setPlayer(player);
}

#endif

void WBackendManagerPrivate::onRepeatChanged()
{
    backend->setRepeat(repeat);
}

//-------------------------------------------------------------------------------------------------

void WBackendManagerPrivate::onState()
{
    if (type != Track) return;

    Q_Q(WBackendManager);

    q->setState(backend->state());
}

void WBackendManagerPrivate::onStateLoad()
{
    Q_Q(WBackendManager);

    if (type == Track)
    {
        q->setStateLoad(backend->stateLoad());

        return;
    }

    WAbstractBackend::StateLoad stateLoad = backend->stateLoad();

    qDebug("STATE LOAD %d", stateLoad);

    q->setStateLoad(stateLoad);

#ifdef SK_NO_QML
    freeze = false;
#else
    if (freeze)
    {
        freeze = false;

        q->updateFrame();
    }
#endif

    if (stateLoad == WAbstractBackend::StateLoadDefault)
    {
        if (timerClock != -1) return;

        qDebug("CLOCK");

        startClock();

        if (type == Channel) startSynchronize();
    }
    else
    {
        stopClock      ();
        stopSynchronize();
    }
}

void WBackendManagerPrivate::onLive()
{
    if (type == Track)
    {
        Q_Q(WBackendManager);

        q->setLive(backend->isLive());

        return;
    }

    if (backend->isLive() == false) return;

    clock = true;
}

void WBackendManagerPrivate::onStarted()
{
    if (type != Track) return;

    Q_Q(WBackendManager);

    q->setStarted(backend->hasStarted());
}

void WBackendManagerPrivate::onEnded()
{
    if (type == Track)
    {
        Q_Q(WBackendManager);

        q->setEnded(backend->hasEnded());

        return;
    }

    if (backend->hasEnded() == false) return;

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

    if (clock) return;

    int currentTime = backend->currentTime();

    if (currentTime == -1) return;

    applyTime(timeA + qMax(0, currentTime - start));
}

void WBackendManagerPrivate::onDuration()
{
    if (type == Track)
    {
        Q_Q(WBackendManager);

        q->setDuration(backend->duration());

        return;
    }

    if (clock || currentTime - timeA < backend->duration() - start) return;

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
    Q_Q(WBackendManager);

    outputs.append(q->addOutput(output));
}

void WBackendManagerPrivate::onOutputRemoved(int index)
{
    Q_Q(WBackendManager);

    // NOTE: We ignore the 'Default' output.
    q->removeOutput(outputs.takeAt(index - 1));
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
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ int WBackendManager::getTimeA() const
{
    Q_D(const WBackendManager); return d->timeA;
}

/* Q_INVOKABLE */ int WBackendManager::getTimeB() const
{
    Q_D(const WBackendManager); return d->timeB;
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

    if (WControllerNetwork::extractFragmentValue(url, "arg").isEmpty())
    {
        d->stopBackend();

        if (url.isEmpty())
        {
            d->clearActive();
        }
        else if (d->state == StatePlaying)
        {
            d->loadSources(true);

            d->updateLoading();
        }
    }
    else if (d->state == StatePlaying)
    {
        d->freeze = true;

        d->stopBackend();

        d->loadSources(true);

        setStateLoad(WAbstractBackend::StateLoadBuffering);
    }
    else d->stopBackend();

    return true;
}

//-------------------------------------------------------------------------------------------------

/* virtual */ bool WBackendManager::backendPlay()
{
    Q_D(WBackendManager);

    if (d->loaded == false)
    {
        if (d->source.isEmpty() || d->reply) return true;

        d->loadSources(true);

        d->updateLoading();

        return true;
    }

    if (d->type == WBackendManagerPrivate::Track)
    {
        d->backendInterface->play();

        d->connectBackend();

        return d->backend->isPlaying();
    }

    if (d->currentMedia.isEmpty() == false)
    {
        d->backendInterface->play();

        d->connectBackend();

        if (d->clock) d->startClock();
    }
    else d->startClock();

    if (d->type == WBackendManagerPrivate::Channel)
    {
        d->startSynchronize();
    }

    return true;
}

/* virtual */ bool WBackendManager::backendPause()
{
    Q_D(WBackendManager);

    if (d->loaded == false) return true;

    if (d->type == WBackendManagerPrivate::Track)
    {
        d->backendInterface->pause();

        return d->backend->isPaused();
    }

    d->stopClock      ();
    d->stopSynchronize();

    if (d->currentMedia.isEmpty() == false)
    {
        d->backendInterface->pause();
    }

    return true;
}

/* virtual */ bool WBackendManager::backendStop()
{
    Q_D(WBackendManager);

    if (d->loaded == false) return true;

    if (d->type == WBackendManagerPrivate::Track)
    {
        d->stopBackend();

        d->clearActive();

        return d->backend->isStopped();
    }

    if (d->currentMedia.isEmpty())
    {
        d->stopClock      ();
        d->stopSynchronize();
    }
    else d->stopBackend();

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

    // NOTE: When the source is not loaded and the currentTime changes we have to load it again.
    if (d->loaded == false)
    {
        if (d->source.isEmpty() || d->reply == NULL) return;

        d->clearReply();

        // NOTE: When reloading we clear the contextId in case it's an interactive track.
        d->source = WControllerNetwork::removeFragmentValue(d->source, "id");

        d->loadSources(isPlaying());

        return;
    }

    if (d->type == WBackendManagerPrivate::Track)
    {
        d->backendInterface->seek(msec);

        return;
    }

    if (msec < d->timeA || msec > d->timeB)
    {
        d->freeze = true;

        d->stopBackend();

        if (d->type == WBackendManagerPrivate::Interactive)
        {
            d->source = WControllerNetwork::removeFragmentValue(d->source, "id");
        }

        d->loadSources(isPlaying());
    }
    else if (d->clock)
    {
        if (d->backend->isLive()) return;

        int duration = d->backend->duration() - d->start;

        if (duration < 0) return;

        msec -= d->timeA;

        // NOTE: Parenthesis are required to avoid integer overflow.
        d->backendInterface->seek(msec - duration * (msec / duration) + d->start);
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

/* virtual */ void WBackendManager::backendSetSourceMode(SourceMode mode)
{
    Q_D(WBackendManager);

    d->backend->setSourceMode(mode);
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

    d->backend->setTrackVideo(id);
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

    if (d->freeze) return;

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

    if (d->freeze) return;

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

/* virtual */ void WBackendManager::timerEvent(QTimerEvent * event)
{
    Q_D(WBackendManager);

    int id = event->timerId();

    if (id == d->timerClock)
    {
        d->applyTime(d->currentTime + d->time.elapsed());
    }
    else if (id == d->timerSynchronize)
    {
        QDateTime date = WControllerApplication::currentDateUtc(d->timeZone);

        int time = WControllerApplication::getMsecsWeek(date);

        if (qAbs(time - d->currentTime) <= BACKENDMANAGER_MAX_DELAY) return;

        qDebug("SYNCHRONIZE");

        seek(time);
    }
    else // if (id == d->timerReload)
    {
        if (d->reply) return;

        qDebug("RELOADING");

        wControllerMedia->clearMedia(d->source);

        d->reloadSources(isPlaying());
    }
}

#endif // SK_NO_BACKENDMANAGER
