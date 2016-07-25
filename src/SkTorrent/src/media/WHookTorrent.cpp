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

#include "WHookTorrent.h"

// Sk includes
#include <WControllerNetwork>
#include <WControllerTorrent>

#ifndef SK_NO_HOOKTORRENT

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include "WHookTorrent_p.h"

WHookTorrentPrivate::WHookTorrentPrivate(WHookTorrent * p) : WAbstractHookPrivate(p) {}

void WHookTorrentPrivate::init()
{
    reply = NULL;
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

WHookTorrent::WHookTorrent(WAbstractBackend * backend)
    : WAbstractHook(new WHookTorrentPrivate(this), backend)
{
    Q_D(WHookTorrent); d->init();
}

//-------------------------------------------------------------------------------------------------
// Protected WAbstractHook implementation
//-------------------------------------------------------------------------------------------------

/* virtual */ bool WHookTorrent::hookCheckSource(const QUrl & url)
{
    QString extension = WControllerNetwork::extractUrlExtension(url);

    if (extension == "torrent")
    {
         return true;
    }
    else return false;
}

//-------------------------------------------------------------------------------------------------
// Protected WAbstractHook reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ bool WHookTorrent::backendSetSource(const QUrl & url)
{
    Q_D(WHookTorrent);

    d->reply = wControllerTorrent->getTorrent(url, this);

    if (d->reply)
    {
         return true;
    }
    else return false;
}

#endif // SK_NO_HOOKTORRENT
