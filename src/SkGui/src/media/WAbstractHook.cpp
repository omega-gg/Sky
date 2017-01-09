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

WAbstractHookPrivate::WAbstractHookPrivate(WAbstractHook * p) : WAbstractBackendPrivate(p) {}

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
    : WAbstractBackend(new WAbstractHookPrivate(this))
{
    Q_D(WAbstractHook); d->init(backend);
}

WAbstractHook::WAbstractHook(WAbstractHookPrivate * p, WAbstractBackend * backend)
    : WAbstractBackend(p)
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
// Protected WAbstractBackend implementation
//-------------------------------------------------------------------------------------------------

/* virtual */ bool WAbstractHook::backendSetSource(const QUrl & url)
{
    Q_D(WAbstractHook);

    return d->backend->backendSetSource(url);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ bool WAbstractHook::backendPlay()
{
    Q_D(WAbstractHook);

    return d->backend->backendPlay();
}

/* virtual */ bool WAbstractHook::backendReplay()
{
    Q_D(WAbstractHook);

    return d->backend->backendReplay();
}

//-------------------------------------------------------------------------------------------------

/* virtual */ bool WAbstractHook::backendPause()
{
    Q_D(WAbstractHook);

    return d->backend->backendPause();
}

/* virtual */ bool WAbstractHook::backendStop()
{
    Q_D(WAbstractHook);

    return d->backend->backendStop();
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WAbstractHook::backendSetVolume(qreal volume)
{
    Q_D(WAbstractHook);

    d->backend->backendSetVolume(volume);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ bool WAbstractHook::backendDelete()
{
    Q_D(WAbstractHook);

    return d->backend->backendDelete();
}

//-------------------------------------------------------------------------------------------------
// Protected WAbstractBackend reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WAbstractHook::backendSeekTo(int msec)
{
    Q_D(WAbstractHook);

    d->backend->backendSeekTo(msec);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WAbstractHook::backendSetSpeed(qreal speed)
{
    Q_D(WAbstractHook);

    d->backend->backendSetSpeed(speed);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WAbstractHook::backendSetRepeat(bool repeat)
{
    Q_D(WAbstractHook);

    d->backend->backendSetRepeat(repeat);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WAbstractHook::backendSetQuality(Quality quality)
{
    Q_D(WAbstractHook);

    d->backend->backendSetQuality(quality);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WAbstractHook::backendSetFillMode(FillMode fillMode)
{
    Q_D(WAbstractHook);

    d->backend->backendSetFillMode(fillMode);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WAbstractHook::backendSetSize(const QSizeF & size)
{
    Q_D(WAbstractHook);

    d->backend->backendSetSize(size);
}

/* virtual */ void WAbstractHook::backendDrawFrame(QPainter                       * painter,
                                                   const QStyleOptionGraphicsItem * option)
{
    Q_D(WAbstractHook);

    d->backend->backendDrawFrame(painter, option);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WAbstractHook::backendUpdateFrame()
{
    Q_D(WAbstractHook);

    d->backend->backendUpdateFrame();
}

/* virtual */ QImage WAbstractHook::backendGetFrame() const
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

#endif // SK_NO_ABSTRACTHOOK
