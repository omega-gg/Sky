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

#include "WTrackNet.h"

#ifndef SK_NO_TRACKNET

// Sk includes
#include <WControllerApplication>
#include <WAbstractPlaylist>
#include <WControllerPlaylist>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include "WTrackNet_p.h"

WTrackNetPrivate::WTrackNetPrivate(WTrackNet * p) : WAbstractTrackPrivate(p) {}

void WTrackNetPrivate::init() {}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

/* explicit */ WTrackNet::WTrackNet(const QUrl & source, State state)
    : WAbstractTrack(new WTrackNetPrivate(this), state)
{
    Q_D(WTrackNet);

    d->init();

    d->source = source;
}

//-------------------------------------------------------------------------------------------------
// Operators
//-------------------------------------------------------------------------------------------------

WTrackNet::WTrackNet(const WTrackNet & other)
    : WAbstractTrack(new WTrackNetPrivate(this))
{
    Q_D(WTrackNet);

    d->init();

    (*this) = other;
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

WPlaylistNet * WTrackNet::playlistNet() const
{
    Q_D(const WTrackNet);

    if (d->playlist)
    {
         return d->playlist->toPlaylistNet();
    }
    else return NULL;
}

#endif // SK_NO_TRACKNET
