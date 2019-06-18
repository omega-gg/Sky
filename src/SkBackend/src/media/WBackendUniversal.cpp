//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkBackend module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#include "WBackendUniversal.h"

#ifndef SK_NO_BACKENDUNIVERSAL

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include "WBackendUniversal_p.h"

WBackendUniversalPrivate::WBackendUniversalPrivate(WBackendUniversal * p)
    : WBackendNetPrivate(p) {}

void WBackendUniversalPrivate::init() {}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

WBackendUniversal::WBackendUniversal() : WBackendNet(new WBackendUniversalPrivate(this))
{
    Q_D(WBackendUniversal); d->init();
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

QString WBackendUniversal::source() const
{
    Q_D(const WBackendUniversal); return d->source;
}

void WBackendUniversal::setSource(const QString & source)
{
    Q_D(WBackendUniversal);

    if (d->source == source) return;

    d->source = source;

    emit sourceChanged();
}

#endif // SK_NO_BACKENDUNIVERSAL
