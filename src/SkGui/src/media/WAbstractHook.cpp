//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkGui module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#include "WAbstractHook.h"

#ifndef SK_NO_ABSTRACTHOOK

// Sk includes
#include <WControllerPlaylist>

// Private includes
#include <private/WAbstractBackend_p>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

WAbstractHookPrivate::WAbstractHookPrivate(WAbstractHook * p) : WPrivate(p) {}

/* virtual */ WAbstractHookPrivate::~WAbstractHookPrivate()
{
    if (filterActive) backend->setFilter(NULL);
}

//-------------------------------------------------------------------------------------------------

void WAbstractHookPrivate::init(WAbstractBackend * backend)
{
    Q_ASSERT(backend);

    Q_Q(WAbstractHook);

    this->backend = backend;

    filterActive = false;

    QObject::connect(backend, SIGNAL(destroyed()), q, SLOT(onBackendDestroyed()));
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WAbstractHookPrivate::onBackendDestroyed()
{
    Q_Q(WAbstractHook);

    filterActive = false;

    q->deleteLater();
}

//-------------------------------------------------------------------------------------------------
// Protected ctor / dtor
//-------------------------------------------------------------------------------------------------

WAbstractHook::WAbstractHook(WAbstractBackend * backend)
    : QObject(), WPrivatable(new WAbstractHookPrivate(this))
{
    Q_D(WAbstractHook); d->init(backend);
}

WAbstractHook::WAbstractHook(WAbstractHookPrivate * p, WAbstractBackend * backend)
    : QObject(), WPrivatable(p)
{
    Q_D(WAbstractHook); d->init(backend);
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ bool WAbstractHook::checkSource(const QUrl & url)
{
    return hookCheckSource(url);
}

//-------------------------------------------------------------------------------------------------
// WBackendInterface implementation
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QUrl WAbstractHook::source() const
{
    Q_D(const WAbstractHook); return d->source;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ bool WAbstractHook::sourceIsVideo() const
{
    Q_D(const WAbstractHook);

    return wControllerPlaylist->sourceIsVideo(d->source);
}

/* Q_INVOKABLE virtual */ bool WAbstractHook::sourceIsAudio() const
{
    Q_D(const WAbstractHook);

    return wControllerPlaylist->sourceIsAudio(d->source);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ void WAbstractHook::loadSource(const QUrl & url, int duration,
                                                                           int currentTime)
{
    Q_D(WAbstractHook); d->backend->loadSource(url, duration, currentTime);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ void WAbstractHook::play()
{
    Q_D(WAbstractHook); d->backend->play();
}

/* Q_INVOKABLE virtual */ void WAbstractHook::replay()
{
    Q_D(WAbstractHook); d->backend->replay();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ void WAbstractHook::pause()
{
    Q_D(WAbstractHook); d->backend->pause();
}

/* Q_INVOKABLE virtual */ void WAbstractHook::stop()
{
    Q_D(WAbstractHook); d->backend->stop();
}

/* Q_INVOKABLE virtual */ void WAbstractHook::clear()
{
    Q_D(WAbstractHook); d->backend->clear();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ void WAbstractHook::seek(int msec)
{
    Q_D(WAbstractHook); d->backend->seek(msec);
}

//-------------------------------------------------------------------------------------------------
// Protected functions
//-------------------------------------------------------------------------------------------------

void WAbstractHook::applySource(const QUrl & url)
{
    Q_D(WAbstractHook);

    d->backend->d_func()->source = url;
}

//-------------------------------------------------------------------------------------------------

void WAbstractHook::applyState(WAbstractBackend::State state)
{
    Q_D(WAbstractHook);

    d->backend->d_func()->state = state;
}

void WAbstractHook::applyStateLoad(WAbstractBackend::StateLoad stateLoad)
{
    Q_D(WAbstractHook);

    d->backend->d_func()->stateLoad = stateLoad;
}

//-------------------------------------------------------------------------------------------------

void WAbstractHook::applyCurrentTime(int msec)
{
    Q_D(WAbstractHook);

    d->backend->d_func()->currentTime = msec;
}

//-------------------------------------------------------------------------------------------------
// Backend functions

void WAbstractHook::setState(WAbstractBackend::State state)
{
    Q_D(WAbstractHook); d->backend->setState(state);
}

void WAbstractHook::setStateLoad(WAbstractBackend::StateLoad stateLoad)
{
    Q_D(WAbstractHook); d->backend->setStateLoad(stateLoad);
}

//-------------------------------------------------------------------------------------------------

void WAbstractHook::setEnded(bool ended)
{
    Q_D(WAbstractHook); d->backend->setEnded(ended);
}

//-------------------------------------------------------------------------------------------------

void WAbstractHook::setCurrentTime(int msec)
{
    Q_D(WAbstractHook); d->backend->setCurrentTime(msec);
}

void WAbstractHook::setDuration(int msec)
{
    Q_D(WAbstractHook); d->backend->setDuration(msec);
}

//-------------------------------------------------------------------------------------------------

void WAbstractHook::setProgress(qreal progress)
{
    Q_D(WAbstractHook); d->backend->setProgress(progress);
}

//-------------------------------------------------------------------------------------------------

void WAbstractHook::setOutputActive(WAbstractBackend::Output output)
{
    Q_D(WAbstractHook); d->backend->setOutputActive(output);
}

void WAbstractHook::setQualityActive(WAbstractBackend::Quality quality)
{
    Q_D(WAbstractHook); d->backend->setQualityActive(quality);
}

//-------------------------------------------------------------------------------------------------
// Backend abstract functions

bool WAbstractHook::backendSetSource(const QUrl & url)
{
    Q_D(WAbstractHook);

    return d->backend->backendSetSource(url);
}

//-------------------------------------------------------------------------------------------------

bool WAbstractHook::backendPlay()
{
    Q_D(WAbstractHook);

    return d->backend->backendPlay();
}

bool WAbstractHook::backendPause()
{
    Q_D(WAbstractHook);

    return d->backend->backendPause();
}

bool WAbstractHook::backendStop()
{
    Q_D(WAbstractHook);

    return d->backend->backendStop();
}

//-------------------------------------------------------------------------------------------------

void WAbstractHook::backendSetVolume(qreal volume)
{
    Q_D(WAbstractHook); d->backend->backendSetVolume(volume);
}

//-------------------------------------------------------------------------------------------------

bool WAbstractHook::backendDelete()
{
    Q_D(WAbstractHook);

    return d->backend->backendDelete();
}

//-------------------------------------------------------------------------------------------------
// Backend virtual functions

void WAbstractHook::backendSeek(int msec)
{
    Q_D(WAbstractHook); d->backend->backendSeek(msec);
}

void WAbstractHook::backendSetSpeed(qreal speed)
{
    Q_D(WAbstractHook); d->backend->backendSetSpeed(speed);
}

//-------------------------------------------------------------------------------------------------

void WAbstractHook::backendSetOutput(WAbstractBackend::Output output)
{
    Q_D(WAbstractHook); d->backend->backendSetOutput(output);
}

void WAbstractHook::backendSetQuality(WAbstractBackend::Quality quality)
{
    Q_D(WAbstractHook); d->backend->backendSetQuality(quality);
}

//-------------------------------------------------------------------------------------------------

void WAbstractHook::backendSetFillMode(WAbstractBackend::FillMode fillMode)
{
    Q_D(WAbstractHook); d->backend->backendSetFillMode(fillMode);
}

//-------------------------------------------------------------------------------------------------

void WAbstractHook::backendSetSize(const QSizeF & size)
{
    Q_D(WAbstractHook); d->backend->backendSetSize(size);
}

//-------------------------------------------------------------------------------------------------

void WAbstractHook::backendDrawFrame(QPainter * painter, const QStyleOptionGraphicsItem * option)
{
    Q_D(WAbstractHook); d->backend->backendDrawFrame(painter, option);
}

//-------------------------------------------------------------------------------------------------

void WAbstractHook::backendUpdateFrame()
{
    Q_D(WAbstractHook); d->backend->backendUpdateFrame();
}

QImage WAbstractHook::backendGetFrame() const
{
    Q_D(const WAbstractHook);

    return d->backend->backendGetFrame();
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

WAbstractBackend * WAbstractHook::backend() const
{
    Q_D(const WAbstractHook); return d->backend;
}

//-------------------------------------------------------------------------------------------------

bool WAbstractHook::filterActive() const
{
    Q_D(const WAbstractHook); return d->filterActive;
}

void WAbstractHook::setFilterActive(bool active)
{
    Q_D(WAbstractHook);

    if (d->filterActive == active) return;

    d->filterActive = active;

    if (active)
    {
         d->backend->setFilter(this);
    }
    else d->backend->setFilter(NULL);

    emit filterActiveChanged();
}

#endif // SK_NO_ABSTRACTHOOK
