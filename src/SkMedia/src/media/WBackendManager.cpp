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
#include <WBackendVlc>

// Private includes
#include <private/WBackendVlc_p>

#ifndef SK_NO_BACKENDMANAGER

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

WBackendManagerPrivate::WBackendManagerPrivate(WBackendManager * p) : WAbstractBackendPrivate(p) {}

/* virtual */ WBackendManagerPrivate::~WBackendManagerPrivate()
{
    foreach (WAbstractBackend * backend, backends)
    {
        backend->deleteBackend();
    }
}

//-------------------------------------------------------------------------------------------------

void WBackendManagerPrivate::init()
{
    Q_Q(WBackendManager);

    WBackendVlc * backend = new WBackendVlc(q);

    backends.append(backend);

    backendInterface = backend;
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
    return true;
}

//-------------------------------------------------------------------------------------------------

/* virtual */ bool WBackendManager::backendPlay()
{
    return true;
}

/* virtual */ bool WBackendManager::backendPause()
{
    return true;
}

/* virtual */ bool WBackendManager::backendStop()
{
    return true;
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WBackendManager::backendSetVolume(qreal volume)
{
}

//-------------------------------------------------------------------------------------------------

/* virtual */ bool WBackendManager::backendDelete()
{
    return false;
}

#endif // SK_NO_BACKENDMANAGER
