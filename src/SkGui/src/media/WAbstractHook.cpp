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

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include "WAbstractHook_p.h"

WAbstractHookPrivate::WAbstractHookPrivate(WAbstractHook * p) : WPrivate(p) {}

//-------------------------------------------------------------------------------------------------

void WAbstractHookPrivate::init(WAbstractBackend * backend)
{
    Q_ASSERT(backend);

    Q_Q(WAbstractHook);

    this->backend = backend;

    q->setParent(backend);
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------
// Protected

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

/* Q_INVOKABLE virtual */ void WAbstractHook::seekTo(int msec)
{
    Q_D(WAbstractHook); d->backend->seekTo(msec);
}

//-------------------------------------------------------------------------------------------------
// Protected functions
//-------------------------------------------------------------------------------------------------

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

void WAbstractHook::setOutputActive(WAbstractBackend::Output output)
{
    Q_D(WAbstractHook); d->backend->setOutputActive(output);
}

void WAbstractHook::setQualityActive(WAbstractBackend::Quality quality)
{
    Q_D(WAbstractHook); d->backend->setQualityActive(quality);
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

WAbstractBackend * WAbstractHook::backend() const
{
    Q_D(const WAbstractHook); return d->backend;
}

#endif // SK_NO_ABSTRACTHOOK
