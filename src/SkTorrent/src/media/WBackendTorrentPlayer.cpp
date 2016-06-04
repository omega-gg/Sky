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

#include "WBackendTorrentPlayer.h"

// Sk includes
#include <WControllerFile>

#ifndef SK_NO_BACKENDTORRENTPLAYER

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

WBackendTorrentPlayerPrivate::WBackendTorrentPlayerPrivate(WBackendTorrentPlayer * p)
    : WAbstractBackendPrivate(p) {}

void WBackendTorrentPlayerPrivate::init() {}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

WBackendTorrentPlayer::WBackendTorrentPlayer()
    : WAbstractBackend(new WBackendTorrentPlayerPrivate(this))
{
    Q_D(WBackendTorrentPlayer); d->init();
}

//-------------------------------------------------------------------------------------------------
// Protected WAbstractBackend implementation
//-------------------------------------------------------------------------------------------------

/* virtual */ bool WBackendTorrentPlayer::backendSetSource(const QUrl & url)
{
    Q_D(WBackendTorrentPlayer);

    return false;
}

//-------------------------------------------------------------------------------------------------

/* virtual */ bool WBackendTorrentPlayer::backendPlay()
{
    return false;
}

/* virtual */ bool WBackendTorrentPlayer::backendReplay()
{
    return false;
}

//-------------------------------------------------------------------------------------------------

/* virtual */ bool WBackendTorrentPlayer::backendPause()
{
    return false;
}

/* virtual */ bool WBackendTorrentPlayer::backendStop()
{
    return false;
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WBackendTorrentPlayer::backendSetVolume(int) {}

//-------------------------------------------------------------------------------------------------

/* virtual */ bool WBackendTorrentPlayer::backendDelete()
{
    return false;
}

#endif // SK_NO_BACKENDTORRENTPLAYER
