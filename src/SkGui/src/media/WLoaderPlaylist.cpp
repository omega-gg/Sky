//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkGui.

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

#include "WLoaderPlaylist.h"

#ifndef SK_NO_LOADERPLAYLIST

// Sk includes
#include <WLibraryFolder>

//-------------------------------------------------------------------------------------------------
// WPrivate
//-------------------------------------------------------------------------------------------------

WLoaderPlaylistPrivate::WLoaderPlaylistPrivate(WLoaderPlaylist * p) : WPrivate(p) {}

//-------------------------------------------------------------------------------------------------

void WLoaderPlaylistPrivate::init(WLibraryFolder * folder, int id)
{
    Q_ASSERT(folder);

    this->folder = folder;
    this->id     = id;

    item = NULL;

    running = false;
    active  = false;
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WLoaderPlaylistPrivate::setItem(WLibraryItem * item)
{
    if (this->item == item) return;

    Q_Q(WLoaderPlaylist);

    if (item) QObject::disconnect(item, 0, q, 0);

    this->item = item;

    if (item)
    {
        if (item->isLoading())
        {
            QObject::connect(item, SIGNAL(loaded()), q, SLOT(onLoaded()));
        }
        else onLoaded();
    }
    else if (active)
    {
        active = false;

        q->onStop();
    }
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WLoaderPlaylistPrivate::onCurrentIdChanged()
{
    if (folder->currentId() == id)
    {
        setItem(folder->currentItem());
    }
    else setItem(NULL);
}

void WLoaderPlaylistPrivate::onLoaded()
{
    Q_Q(WLoaderPlaylist);

    QObject::disconnect(item, 0, q, 0);

    active = true;

    q->onStart();
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

WLoaderPlaylist::WLoaderPlaylist(WLibraryFolder * folder, int id)
    : QObject(folder), WPrivatable(new WLoaderPlaylistPrivate(this))
{
    Q_D(WLoaderPlaylist);

    d->init(folder, id);
}

//-------------------------------------------------------------------------------------------------
// Protected

WLoaderPlaylist::WLoaderPlaylist(WLoaderPlaylistPrivate * p, WLibraryFolder * folder, int id)
    : QObject(folder), WPrivatable(p)
{
    Q_D(WLoaderPlaylist);

    d->init(folder, id);
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WLoaderPlaylist::start()
{
    setRunning(true);
}

/* Q_INVOKABLE */ void WLoaderPlaylist::stop()
{
    setRunning(false);
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

bool WLoaderPlaylist::isRunning() const
{
    Q_D(const WLoaderPlaylist); return d->running;
}

void WLoaderPlaylist::setRunning(bool running)
{
    Q_D(WLoaderPlaylist);

    if (d->running == running) return;

    d->running = running;

    if (running)
    {
        if (d->folder->currentId() == d->id)
        {
            d->setItem(d->folder->currentItem());
        }

        connect(d->folder, SIGNAL(currentIdChanged()), this, SLOT(onCurrentIdChanged()));
    }
    else
    {
        disconnect(d->folder, 0, this, 0);

        d->setItem(NULL);
    }

    emit runningChanged();
}

#endif // SK_NO_LOADERPLAYLIST
