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

#include "WAbstractHook.h"

#ifndef SK_NO_ABSTRACTHOOK

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include "WAbstractHook_p.h"

WAbstractHookPrivate::WAbstractHookPrivate(WAbstractHook * p) : WAbstractBackendPrivate(p) {}

//-------------------------------------------------------------------------------------------------

void WAbstractHookPrivate::init()
{
    backend = NULL;
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

WAbstractHook::WAbstractHook() : WAbstractBackend(new WAbstractHookPrivate(this))
{
    Q_D(WAbstractHook); d->init();
}

//-------------------------------------------------------------------------------------------------
// Protected

WAbstractHook::WAbstractHook(WAbstractHookPrivate * p) : WAbstractBackend(p)
{
    Q_D(WAbstractHook); d->init();
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

    if (d->backend)
    {
         return d->backend->backendSetSource(url);
    }
    else return false;
}

//-------------------------------------------------------------------------------------------------

/* virtual */ bool WAbstractHook::backendPlay()
{
    Q_D(WAbstractHook);

    if (d->backend)
    {
         return d->backend->backendPlay();
    }
    else return false;
}

/* virtual */ bool WAbstractHook::backendReplay()
{
    Q_D(WAbstractHook);

    if (d->backend)
    {
         return d->backend->backendReplay();
    }
    else return false;
}

//-------------------------------------------------------------------------------------------------

/* virtual */ bool WAbstractHook::backendPause()
{
    Q_D(WAbstractHook);

    if (d->backend)
    {
         return d->backend->backendPause();
    }
    else return false;
}

/* virtual */ bool WAbstractHook::backendStop()
{
    Q_D(WAbstractHook);

    if (d->backend)
    {
         return d->backend->backendStop();
    }
    else return false;
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WAbstractHook::backendSetVolume(int percent)
{
    Q_D(WAbstractHook);

    if (d->backend == NULL) return;

    d->backend->backendSetVolume(percent);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ bool WAbstractHook::backendDelete()
{
    Q_D(WAbstractHook);

    if (d->backend)
    {
         return d->backend->backendDelete();
    }
    else return false;
}

//-------------------------------------------------------------------------------------------------
// Protected WAbstractBackend reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WAbstractHook::backendSeekTo(int msec)
{
    Q_D(WAbstractHook);

    if (d->backend == NULL) return;

    d->backend->backendSeekTo(msec);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WAbstractHook::backendSetSpeed(qreal speed)
{
    Q_D(WAbstractHook);

    if (d->backend == NULL) return;

    d->backend->backendSetSpeed(speed);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WAbstractHook::backendSetRepeat(bool repeat)
{
    Q_D(WAbstractHook);

    if (d->backend == NULL) return;

    d->backend->backendSetRepeat(repeat);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WAbstractHook::backendSetQuality(Quality quality)
{
    Q_D(WAbstractHook);

    if (d->backend == NULL) return;

    d->backend->backendSetQuality(quality);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WAbstractHook::backendSetFillMode(FillMode fillMode)
{
    Q_D(WAbstractHook);

    if (d->backend == NULL) return;

    d->backend->backendSetFillMode(fillMode);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WAbstractHook::backendSetSize(const QSizeF & size)
{
    Q_D(WAbstractHook);

    if (d->backend == NULL) return;

    d->backend->backendSetSize(size);
}

/* virtual */ void WAbstractHook::backendDrawFrame(QPainter                       * painter,
                                                   const QStyleOptionGraphicsItem * option)
{
    Q_D(WAbstractHook);

    if (d->backend == NULL) return;

    d->backend->backendDrawFrame(painter, option);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WAbstractHook::backendUpdateFrame()
{
    Q_D(WAbstractHook);

    if (d->backend == NULL) return;

    d->backend->backendUpdateFrame();
}

/* virtual */ QImage WAbstractHook::backendGetFrame() const
{
    Q_D(const WAbstractHook);

    if (d->backend)
    {
         return d->backend->backendGetFrame();
    }
    else return QImage();
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

WAbstractBackend * WAbstractHook::backend() const
{
    Q_D(const WAbstractHook); return d->backend;
}

void WAbstractHook::setBackend(WAbstractBackend * backend)
{
    Q_D(WAbstractHook);

    if (d->backend == backend) return;

    d->backend = backend;

    emit backendChanged();
}

#endif // SK_NO_ABSTRACTHOOK
