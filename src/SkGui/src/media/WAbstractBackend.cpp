//=================================================================================================
/*
    Copyright (C) 2015-2016 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkGui module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#include "WAbstractBackend.h"

// Sk includes
#include <WControllerPlaylist>

#ifndef SK_NO_ABSTRACTBACKEND

//-------------------------------------------------------------------------------------------------
// Static variables

static const int ABSTRACTBACKEND_TIMER_INTERVAL = 2000;

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include "WAbstractBackend_p.h"

WAbstractBackendPrivate::WAbstractBackendPrivate(WAbstractBackend * p) : WPrivate(p) {}

void WAbstractBackendPrivate::init()
{
    parentItem = NULL;

    state     = WAbstractBackend::StateStopped;
    stateLoad = WAbstractBackend::StateLoadDefault;

    started = false;
    ended   = false;

    currentTime = -1;
    duration    = -1;

    speed = 1.0;

    volume = 1.0;

    repeat = false;

    output  = WAbstractBackend::OutputMedia;
    quality = WAbstractBackend::QualityMedium;

    outputActive  = WAbstractBackend::OutputInvalid;
    qualityActive = WAbstractBackend::QualityInvalid;

    fillMode = WAbstractBackend::PreserveAspectFit;

    deleting = false;
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WAbstractBackendPrivate::setStarted(bool started)
{
    if (this->started == started) return;

    Q_Q(WAbstractBackend);

    this->started = started;

    emit q->startedChanged();
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

WAbstractBackend::WAbstractBackend()
    : QObject(), WPrivatable(new WAbstractBackendPrivate(this))
{
    Q_D(WAbstractBackend); d->init();
}

//-------------------------------------------------------------------------------------------------
// Protected

WAbstractBackend::WAbstractBackend(WAbstractBackendPrivate * p)
    : QObject(), WPrivatable(p)
{
    Q_D(WAbstractBackend); d->init();
}

//-------------------------------------------------------------------------------------------------

/* virtual */ WAbstractBackend::~WAbstractBackend() {}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WAbstractBackend::loadSource(const QUrl & url,
                                                    int          duration,
                                                    int          currentTime)
{
    Q_D(WAbstractBackend);

    if (d->source != url)
    {
        if (d->state == StatePaused) stop();

        setDuration   (duration);
        setCurrentTime(currentTime);

        d->source = url;

        backendSetSource(url);

        emit sourceChanged();
    }
    else seekTo(currentTime);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WAbstractBackend::play()
{
    Q_D(WAbstractBackend);

    if (d->state == StatePlaying || d->source.isValid() == false) return;

    if (backendPlay()) setState(StatePlaying);
}

/* Q_INVOKABLE */ void WAbstractBackend::replay()
{
    Q_D(WAbstractBackend);

    if (d->source.isValid() == false) return;

    setCurrentTime(-1);

    if (backendReplay()) setState(StatePlaying);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WAbstractBackend::pause()
{
    Q_D(WAbstractBackend);

    if (d->state == StatePaused) return;

    if (backendPause())
    {
        if (d->stateLoad == StateLoadStarting || d->stateLoad == StateLoadResuming)
        {
            stop();
        }
        else setState(StatePaused);
    }
}

/* Q_INVOKABLE */ void WAbstractBackend::stop()
{
    Q_D(WAbstractBackend);

    if (d->state == StateStopped) return;

    if (backendStop()) setState(StateStopped);
}

/* Q_INVOKABLE */ void WAbstractBackend::clear()
{
    loadSource(QUrl());
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WAbstractBackend::seekTo(int msec)
{
    Q_D(WAbstractBackend);

    if (d->currentTime == msec) return;

    d->currentTime = msec;

    if (d->started) backendSeekTo(msec);

    emit currentTimeChanged();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ const QSizeF & WAbstractBackend::getSize() const
{
    Q_D(const WAbstractBackend); return d->size;
}

/* Q_INVOKABLE */ void WAbstractBackend::setSize(const QSizeF & size)
{
    Q_D(WAbstractBackend);

    if (d->size == size) return;

    d->size = size;

    backendSetSize(size);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WAbstractBackend::drawFrame(QPainter                       * painter,
                                                   const QStyleOptionGraphicsItem * option)
{
    backendDrawFrame(painter, option);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WAbstractBackend::updateFrame()
{
    backendUpdateFrame();
}

/* Q_INVOKABLE */ QImage WAbstractBackend::getFrame() const
{
    return backendGetFrame();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ bool WAbstractBackend::deleteBackend()
{
    Q_D(WAbstractBackend);

    if (d->deleting) return false;

    if (backendDelete())
    {
        delete this;

        return true;
    }
    else
    {
        d->deleting = true;

        setParent(NULL);

        setParentItem(NULL);

        setState(StateStopped);

        return false;
    }
}

//-------------------------------------------------------------------------------------------------
// Protected functions
//-------------------------------------------------------------------------------------------------

void WAbstractBackend::setState(State state)
{
    Q_D(WAbstractBackend);

    if (d->state == state) return;

    d->state = state;

    if (state == StateStopped)
    {
        setStateLoad(StateLoadDefault);

        d->setStarted(false);

        setCurrentTime(-1);
    }
    else if (state == StatePlaying)
    {
        d->setStarted(true);
    }
    else setStateLoad(StateLoadDefault);

    emit stateChanged();
}

void WAbstractBackend::setStateLoad(StateLoad stateLoad)
{
    Q_D(WAbstractBackend);

    if (d->stateLoad == stateLoad) return;

    d->stateLoad = stateLoad;

    emit stateLoadChanged();
}

//-------------------------------------------------------------------------------------------------

void WAbstractBackend::setEnded(bool ended)
{
    Q_D(WAbstractBackend);

    if (d->ended == ended) return;

    d->ended = ended;

    if (ended)
    {
        setCurrentTime(-1);

        emit endedChanged();

        emit this->ended();
    }
    else emit endedChanged();
}

//-------------------------------------------------------------------------------------------------

void WAbstractBackend::setCurrentTime(int msec)
{
    Q_D(WAbstractBackend);

    if (d->currentTime == msec) return;

    d->currentTime = msec;

    if (msec != -1) setEnded(false);

    emit currentTimeChanged();
}

void WAbstractBackend::setDuration(int msec)
{
    Q_D(WAbstractBackend);

    if (d->duration == msec) return;

    d->duration = msec;

    emit durationChanged();
}

//-------------------------------------------------------------------------------------------------

void WAbstractBackend::setOutputActive(Output output)
{
    Q_D(WAbstractBackend);

    if (d->outputActive == output) return;

    d->outputActive = output;

    emit outputActiveChanged();
}

void WAbstractBackend::setQualityActive(Quality quality)
{
    Q_D(WAbstractBackend);

    if (d->qualityActive == quality) return;

    d->qualityActive = quality;

    emit qualityActiveChanged();
}

//-------------------------------------------------------------------------------------------------

void WAbstractBackend::deleteNow()
{
    Q_D(WAbstractBackend);

    if (d->deleting) delete this;
}

//-------------------------------------------------------------------------------------------------
// Protected virtual functions
//-------------------------------------------------------------------------------------------------

/* virtual */ void WAbstractBackend::backendSeekTo(int)
{
    qWarning("WAbstractBackend::backendSeekTo: SeekTo is not supported.");
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WAbstractBackend::backendSetSpeed(qreal)
{
    qWarning("WAbstractBackend::backendSetSpeed: SetSpeed is not supported.");
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WAbstractBackend::backendSetRepeat(bool)
{
    qWarning("WAbstractBackend::backendSetRepeat: SetRepeat is not supported.");
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WAbstractBackend::backendSetOutput(Output)
{
    qWarning("WAbstractBackend::backendSetOutput: SetOutput is not supported.");
}

/* virtual */ void WAbstractBackend::backendSetQuality(Quality)
{
    qWarning("WAbstractBackend::backendSetQuality: SetQuality is not supported.");
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WAbstractBackend::backendSetFillMode(FillMode)
{
    qWarning("WAbstractBackend::backendSetFillMode: SetFillMode is not supported.");
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WAbstractBackend::backendSetSize(const QSizeF &)
{
    qWarning("WAbstractBackend::backendSetSize: SetSize is not supported.");
}

/* virtual */
void WAbstractBackend::backendDrawFrame(QPainter *,
                                        const QStyleOptionGraphicsItem *)
{
    qWarning("WAbstractBackend::backendDrawFrame: DrawFrame is not supported.");
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WAbstractBackend::backendUpdateFrame()
{
    qWarning("WAbstractBackend::backendUpdateFrame: UpdateFrame is not supported.");
}

/* virtual */ QImage WAbstractBackend::backendGetFrame() const
{
    qWarning("WAbstractBackend::backendGetFrame: GetFrame is not supported.");

    return QImage();
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

QGraphicsItem * WAbstractBackend::parentItem() const
{
    Q_D(const WAbstractBackend); return d->parentItem;
}

void WAbstractBackend::setParentItem(QGraphicsItem * parent)
{
    Q_D(WAbstractBackend);

    if (d->parentItem == parent) return;

    d->parentItem = parent;

    emit parentItemChanged();
}

//-------------------------------------------------------------------------------------------------

QUrl WAbstractBackend::source() const
{
    Q_D(const WAbstractBackend); return d->source;
}

void WAbstractBackend::setSource(const QUrl & url)
{
    loadSource(url);
}

//-------------------------------------------------------------------------------------------------

WAbstractBackend::State WAbstractBackend::state() const
{
    Q_D(const WAbstractBackend); return d->state;
}

WAbstractBackend::StateLoad WAbstractBackend::stateLoad() const
{
    Q_D(const WAbstractBackend); return d->stateLoad;
}

//-------------------------------------------------------------------------------------------------

bool WAbstractBackend::isLoading() const
{
    Q_D(const WAbstractBackend); return (d->stateLoad != StateLoadDefault);
}

//-------------------------------------------------------------------------------------------------

bool WAbstractBackend::isStarting() const
{
    Q_D(const WAbstractBackend); return (d->stateLoad == StateLoadStarting);
}

bool WAbstractBackend::isResuming() const
{
    Q_D(const WAbstractBackend); return (d->stateLoad == StateLoadResuming);
}

bool WAbstractBackend::isBuffering() const
{
    Q_D(const WAbstractBackend); return (d->stateLoad == StateLoadBuffering);
}

//-------------------------------------------------------------------------------------------------

bool WAbstractBackend::isPlaying() const
{
    Q_D(const WAbstractBackend); return (d->state == StatePlaying);
}

bool WAbstractBackend::isPaused() const
{
    Q_D(const WAbstractBackend); return (d->state == StatePaused);
}

bool WAbstractBackend::isStopped() const
{
    Q_D(const WAbstractBackend); return (d->state == StateStopped);
}

//-------------------------------------------------------------------------------------------------

bool WAbstractBackend::hasStarted() const
{
    Q_D(const WAbstractBackend); return d->started;
}

bool WAbstractBackend::hasEnded() const
{
    Q_D(const WAbstractBackend); return d->ended;
}

//-------------------------------------------------------------------------------------------------

bool WAbstractBackend::hasVideo() const
{
    Q_D(const WAbstractBackend); return (d->outputActive != OutputAudio);
}

bool WAbstractBackend::hasAudio() const
{
    Q_D(const WAbstractBackend); return (d->outputActive != OutputVideo);
}

//-------------------------------------------------------------------------------------------------

int WAbstractBackend::currentTime() const
{
    Q_D(const WAbstractBackend); return d->currentTime;
}

int WAbstractBackend::duration() const
{
    Q_D(const WAbstractBackend); return d->duration;
}

//-------------------------------------------------------------------------------------------------

qreal WAbstractBackend::speed() const
{
    Q_D(const WAbstractBackend); return d->speed;
}

void WAbstractBackend::setSpeed(qreal speed)
{
    Q_D(WAbstractBackend);

    if (d->speed == speed) return;

    d->speed = speed;

    backendSetSpeed(speed);

    emit speedChanged();
}

//-------------------------------------------------------------------------------------------------

qreal WAbstractBackend::volume() const
{
    Q_D(const WAbstractBackend); return d->volume;
}

void WAbstractBackend::setVolume(qreal volume)
{
    Q_D(WAbstractBackend);

    volume = qBound(0.0, volume, 1.0);

    if (d->volume == volume) return;

    d->volume = volume;

    backendSetVolume(volume);

    emit volumeChanged();
}

//-------------------------------------------------------------------------------------------------

bool WAbstractBackend::repeat() const
{
    Q_D(const WAbstractBackend); return d->repeat;
}

void WAbstractBackend::setRepeat(bool repeat)
{
    Q_D(WAbstractBackend);

    if (d->repeat == repeat) return;

    d->repeat = repeat;

    backendSetRepeat(repeat);

    emit repeatChanged();
}

//-------------------------------------------------------------------------------------------------

WAbstractBackend::Output WAbstractBackend::output() const
{
    Q_D(const WAbstractBackend); return d->output;
}

void WAbstractBackend::setOutput(Output output)
{
    Q_D(WAbstractBackend);

    if (d->output == output) return;

    d->output = output;

    backendSetOutput(output);

    emit outputChanged();
}

//-------------------------------------------------------------------------------------------------

WAbstractBackend::Quality WAbstractBackend::quality() const
{
    Q_D(const WAbstractBackend); return d->quality;
}

void WAbstractBackend::setQuality(Quality quality)
{
    Q_D(WAbstractBackend);

    if (d->quality == quality) return;

    d->quality = quality;

    backendSetQuality(quality);

    emit qualityChanged();
}

//-------------------------------------------------------------------------------------------------

WAbstractBackend::Output WAbstractBackend::outputActive() const
{
    Q_D(const WAbstractBackend); return d->outputActive;
}

WAbstractBackend::Quality WAbstractBackend::qualityActive() const
{
    Q_D(const WAbstractBackend); return d->qualityActive;
}

//-------------------------------------------------------------------------------------------------

WAbstractBackend::FillMode WAbstractBackend::fillMode() const
{
    Q_D(const WAbstractBackend); return d->fillMode;
}

void WAbstractBackend::setFillMode(FillMode fillMode)
{
    Q_D(WAbstractBackend);

    if (d->fillMode == fillMode) return;

    d->fillMode = fillMode;

    backendSetFillMode(fillMode);

    emit fillModeChanged();
}

#endif // SK_NO_ABSTRACTBACKEND
