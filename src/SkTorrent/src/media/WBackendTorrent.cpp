//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkTorrent.

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

#include "WBackendTorrent.h"

#ifndef SK_NO_BACKENDTORRENT

// Sk includes
#include <WHookTorrent>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include "WBackendTorrent_p.h"

WBackendTorrentPrivate::WBackendTorrentPrivate(WBackendTorrent * p) : WBackendManagerPrivate(p) {}

//-------------------------------------------------------------------------------------------------

void WBackendTorrentPrivate::init()
{
    Q_Q(WBackendTorrent);

    currentItem->hook = q->createHook(backend);
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

WBackendTorrent::WBackendTorrent(QObject * parent)
    : WBackendManager(new WBackendTorrentPrivate(this), parent)
{
    Q_D(WBackendTorrent); d->init();
}

//-------------------------------------------------------------------------------------------------
// Protected

WBackendTorrent::WBackendTorrent(WBackendTorrentPrivate * p, QObject * parent)
    : WBackendManager(p, parent)
{
    Q_D(WBackendTorrent); d->init();
}

//-------------------------------------------------------------------------------------------------
// Protected WBackendManager implementation
//-------------------------------------------------------------------------------------------------

/* virtual */ WAbstractHook * WBackendTorrent::createHook(WAbstractBackend * backend)
{
    return new WHookTorrent(backend);
}

#endif // SK_NO_BACKENDTORRENT
