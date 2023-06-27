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
#include <WPlaylist>

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

    qRegisterMetaType<WLoaderPlaylistData>("WLoaderPlaylistData");
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

/* Q_INVOKABLE */ void WLoaderPlaylist::reload()
{
    Q_D(WLoaderPlaylist);

    if (d->active)
    {
        onStop ();
        onClear();
        onStart();
    }
    else onClear();
}

//-------------------------------------------------------------------------------------------------
// Protected functions

void WLoaderPlaylist::clearTracks()
{
    Q_D(WLoaderPlaylist);

    WPlaylist * playlist = d->item->toPlaylist();

    // NOTE: The playlist has to be loaded before clearing otherwise we might remove its title.
    if (playlist->count()) playlist->clearTracks();
}

void WLoaderPlaylist::setQueryLoading(bool loading)
{
    Q_D(WLoaderPlaylist);

    if (d->item == NULL) return;

    d->item->d_func()->setQueryLoading(loading);
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

//=================================================================================================
// WLoaderPlaylistReply
//=================================================================================================

/* Q_INVOKABLE */ void WLoaderPlaylistReply::extract(const QStringList & urls,
                                                     const QStringList & sources, int maximum)
{
    WLoaderPlaylistData data;

    QList<WLoaderPlaylistAction> & actions = data.actions;

    QStringList list = sources;

    int index = 0;

    while (index < list.count())
    {
        if (urls.contains(list.at(index)))
        {
            index++;

            continue;
        }

        list.removeAt(index);

        WLoaderPlaylistAction action(WLoaderPlaylist::Remove);

        action.index = index;

        actions.append(action);
    }

    int count = list.count();

    int total = 0;

    for (int i = 0; i < urls.count(); i++)
    {
        const QString & url = urls.at(i);

        if (total < count)
        {
            if (list.at(total) == url)
            {
                total++;

                if (total == maximum) break;

                continue;
            }

            int index = list.indexOf(url);

            if (index == -1)
            {
                WBackendNetQuery query = getQuery(url, i);

                if (query.isValid() == false) continue;

                list.insert(total, url);

                WLoaderPlaylistAction action(WLoaderPlaylist::Insert);

                action.index = total;
                action.url   = url;
                action.query = query;

                actions.append(action);
            }
            else
            {
                int to;

                if (index < total) to = total - 1;
                else               to = total;

                list.move(index, to);

                WLoaderPlaylistAction action(WLoaderPlaylist::Move);

                action.index = index;

                actions.append(action);

                action = WLoaderPlaylistAction(WLoaderPlaylist::Insert);

                action.index = to;

                actions.append(action);
            }
        }
        else
        {
            WBackendNetQuery query = getQuery(url, i);

            if (query.isValid() == false) continue;

            list.insert(total, url);

            WLoaderPlaylistAction action(WLoaderPlaylist::Insert);

            action.index = total;
            action.url   = url;
            action.query = query;

            actions.append(action);
        }

        total++;

        if (total == maximum) break;
    }

    count = list.count();

    while (count > maximum)
    {
        list.removeAt(total);

        WLoaderPlaylistAction action(WLoaderPlaylist::Remove);

        action.index = total;

        actions.append(action);

        total++;

        count--;
    }

    data.sources = list;

    emit loaded(data);

    deleteLater();
}

//-------------------------------------------------------------------------------------------------
// Protected functions
//-------------------------------------------------------------------------------------------------

/* virtual */ WBackendNetQuery WLoaderPlaylistReply::getQuery(const QString &, int) const
{
    return WBackendNetQuery();
}

#endif // SK_NO_LOADERPLAYLIST
