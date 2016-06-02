//=================================================================================================
/*
    Copyright (C) 2015-2016 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkTorrent module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#include "WControllerTorrent.h"

#ifndef SK_NO_CONTROLLERTORRENT

W_INIT_CONTROLLER(WControllerTorrent)

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include <private/WControllerTorrent_p>

WControllerTorrentPrivate::WControllerTorrentPrivate(WControllerTorrent * p)
    : WControllerPrivate(p) {}

void WControllerTorrentPrivate::init() {}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

WControllerTorrent::WControllerTorrent()
    : WController(new WControllerTorrentPrivate(this), "WControllerTorrent") {}

//-------------------------------------------------------------------------------------------------
// Initialize
//-------------------------------------------------------------------------------------------------

void WControllerTorrent::init()
{
    Q_D(WControllerTorrent); d->init();
}

#endif // SK_NO_CONTROLLERTORRENT
