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
#include <WControllerPlaylist>
#include <WLibraryFolder>
#include <WPlaylist>

//-------------------------------------------------------------------------------------------------
// Static variables

static const int LOADERPLAYLIST_MAX_QUERIES = 3;

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

void WLoaderPlaylistPrivate::clearNodes()
{
    Q_Q(WLoaderPlaylist);

    q->clearTracks();

    sources.clear();

    nodes.clear();
}

WPlaylist * WLoaderPlaylistPrivate::getPlaylist()
{
    foreach (WPlaylist * playlist, playlists)
    {
        if (playlist->queryIsLoading()) continue;

        return playlist;
    }

    if (playlists.count() == LOADERPLAYLIST_MAX_QUERIES) return NULL;

    Q_Q(WLoaderPlaylist);

    WPlaylist * playlist = new WPlaylist;

    playlist->setParent(q);

    playlists.append(playlist);

    QObject::connect(playlist, SIGNAL(queryCompleted()), q, SLOT(onQueryCompleted()));

    return playlist;
}

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

void WLoaderPlaylistPrivate::onQueryCompleted()
{
    Q_Q(WLoaderPlaylist);

    WPlaylist * playlist = static_cast<WPlaylist *> (q->sender());

    WLoaderPlaylistNode * node = jobs.take(playlist);

    if (node)
    {
        //node->query = WBackendNetQuery();

        q->onApplyPlaylist(node, playlist);
    }

    // NOTE: We don't need these tracks anymore so we clear them along with pending queries.
    playlist->clearTracks();

    q->processQueries();
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
        onStop();

        d->clearNodes();

        onStart();
    }
    else d->clearNodes();
}

//-------------------------------------------------------------------------------------------------
// Protected virtual functions
//-------------------------------------------------------------------------------------------------

/* virtual */ void WLoaderPlaylist::onApplyPlaylist(WLoaderPlaylistNode *, WPlaylist *) {}

//-------------------------------------------------------------------------------------------------
// Protected functions

void WLoaderPlaylist::applyActions(const WLoaderPlaylistData & data)
{
    Q_D(WLoaderPlaylist);

    for (int i = 0; i < data.actions.count(); i++)
    {
        const WLoaderPlaylistAction & action = data.actions.at(i);

        WLoaderPlaylist::Action type = action.type;

        if (type == Insert)
        {
            WLoaderPlaylistNode node;

            node.source = action.url;
            node.query  = action.query;

            d->nodes.insert(action.index, node);
        }
        else if (type == Move)
        {
            int from = action.index;

            i++;

            int to = data.actions.at(i).index;

            d->nodes.move(from, to);
        }
        else // if (type == Remove)
        {
            d->nodes.removeAt(action.index);
        }
    }

    /*foreach (const QString & source, d->sources)
    {
        qDebug("BEFORE %s", source.C_STR);
    }*/

    d->sources = data.sources;

    /*foreach (const QString & source, d->sources)
    {
        qDebug("AFTER %s", source.C_STR);
    }*/
}

void WLoaderPlaylist::applySources(const QStringList                    & sources,
                                   const QHash<QString, const WTrack *> & tracks, bool load)
{
    Q_D(WLoaderPlaylist);

    WPlaylist * playlist = d->item->toPlaylist();

    if (playlist == NULL) return;

    if (playlist->isEmpty())
    {
        foreach (const QString & url, sources)
        {
            const WTrack * trackPointer = tracks.value(url);

            if (trackPointer)
            {
                WTrack track = *trackPointer;

                track.setId(-1);

                // NOTE: This is required to have a clean source.
                track.setSource(url);

                playlist->addTrack(track);

                if (load && track.isDefault())
                {
                    playlist->loadTrack(playlist->count() - 1);
                }
            }
            else
            {
                WTrack track(url, WTrack::Default);

                playlist->addTrack(track);

                if (load) playlist->loadTrack(playlist->count() - 1);
            }
        }

        return;
    }

    QString source = WControllerPlaylist::cleanSource(playlist->trackSource(0));

    int total = 0;

    foreach (const QString & url, sources)
    {
        if (source == url)
        {
            total++;

            source = WControllerPlaylist::cleanSource(playlist->trackSource(total));

            continue;
        }

        int index = playlist->indexFromSource(url, true);

        if (index == -1)
        {
            const WTrack * trackPointer = tracks.value(url);

            if (trackPointer)
            {
                WTrack track = *trackPointer;

                track.setId(-1);

                // NOTE: This is required to have a clean source.
                track.setSource(url);

                playlist->insertTrack(total, track);

                if (load && track.isDefault())
                {
                    playlist->loadTrack(total);
                }
            }
            else
            {
                WTrack track(url, WTrack::Default);

                playlist->insertTrack(total, track);

                if (load) playlist->loadTrack(total);
            }
        }
        else playlist->moveTrack(index, total);

        total++;
    }

    int countA = sources.count();

    int countB = playlist->count();

    if (countA < countB)
    {
        playlist->removeTracks(countA, countB - countA);
    }

    /*foreach (const QString & url, sources)
    {
        qDebug("LIST A %s", url.C_STR);
    }

    foreach (const WTrack * track, playlist->trackPointers())
    {
        qDebug("LIST B %s", track->source().C_STR);
    }*/
}


//-------------------------------------------------------------------------------------------------

void WLoaderPlaylist::processQueries()
{
    Q_D(WLoaderPlaylist);

    for (int i = 0; i < d->nodes.count(); i++)
    {
        WLoaderPlaylistNode * node = &(d->nodes[i]);

        const WBackendNetQuery & query = node->query;

        if (query.isValid() == false) continue;

        WPlaylist * playlist = d->getPlaylist();

        if (playlist == NULL) return;

        playlist->applyQuery(query);

        node->query = WBackendNetQuery();

        d->jobs.insert(playlist, node);
    }

    if (d->jobs.isEmpty() == false) return;

    setQueryLoading(false);
}

void WLoaderPlaylist::clearQueries()
{
    Q_D(WLoaderPlaylist);

    foreach (WPlaylist * playlist, d->playlists)
    {
        QObject::disconnect(playlist, 0, this, 0);

        playlist->clearTracks();

        playlist->tryDelete();
    }

    d->playlists.clear();
    d->jobs     .clear();

    setQueryLoading(false);
}

//-------------------------------------------------------------------------------------------------

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

    if (loading)
    {
         d->item->d_func()->setQueryLoading(true);
    }
    else d->item->d_func()->setQueryFinished();
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

WLibraryFolder * WLoaderPlaylist::folder() const
{
    Q_D(const WLoaderPlaylist); return d->folder;
}

int WLoaderPlaylist::id() const
{
    Q_D(const WLoaderPlaylist); return d->id;
}

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

                total++;

                if (total == maximum) break;
            }
            else if (index < total)
            {
                int to = total - 1;

                list.move(index, to);

                WLoaderPlaylistAction action(WLoaderPlaylist::Move);

                action.index = index;

                actions.append(action);

                action = WLoaderPlaylistAction(WLoaderPlaylist::Insert);

                action.index = to;

                actions.append(action);
            }
            else
            {
                list.move(index, total);

                WLoaderPlaylistAction action(WLoaderPlaylist::Move);

                action.index = index;

                actions.append(action);

                action = WLoaderPlaylistAction(WLoaderPlaylist::Insert);

                action.index = total;

                actions.append(action);

                total++;

                if (total == maximum) break;
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

            total++;

            if (total == maximum) break;
        }
    }

    count = list.count();

    while (count > maximum)
    {
        list.removeAt(total);

        WLoaderPlaylistAction action(WLoaderPlaylist::Remove);

        action.index = total;

        actions.append(action);

        count--;
    }

    data.sources = list;

    emit loaded(data);
}

/* Q_INVOKABLE */ void WLoaderPlaylistReply::abortAndDelete(const QObject * receiver)
{
    disconnect(this,     SIGNAL(loaded(const WLoaderPlaylistData &)),
               receiver, SLOT(onLoaded(const WLoaderPlaylistData &)));

    deleteLater();
}

/* Q_INVOKABLE */ void WLoaderPlaylistReply::applyMethod(QMetaMethod * method)
{
    const QMetaObject * meta = WLoaderPlaylistReply().metaObject();

    *method = meta->method(meta->indexOfMethod("extract(QStringList,QStringList,int)"));
}

//-------------------------------------------------------------------------------------------------
// Protected functions
//-------------------------------------------------------------------------------------------------

/* virtual */ WBackendNetQuery WLoaderPlaylistReply::getQuery(const QString & url, int) const
{
    return WBackendNetQuery(url);
}

#endif // SK_NO_LOADERPLAYLIST
