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

#include "WLoaderRecent.h"

#ifndef SK_NO_LOADERRECENT

// Sk includes
#include <WControllerNetwork>
#include <WControllerPlaylist>
#include <WLibraryFolder>

//-------------------------------------------------------------------------------------------------
// Static variables

static const int LOADERRECENT_TRACKS = 5;

static const int LOADERRECENT_MAX_COUNT   = 50;
static const int LOADERRECENT_MAX_QUERIES =  3;

//=================================================================================================
// WLoaderRecentReply
//=================================================================================================

class WLoaderRecentReply : public WLoaderPlaylistReply
{
    Q_OBJECT

protected: // WLoaderPlaylistReply reimplementation
    /* virtual */ WBackendNetQuery getQuery(const QString & url, int index) const;
};

WBackendNetQuery WLoaderRecentReply::getQuery(const QString & url, int) const
{
#ifndef SK_NO_TORRENT
    if (WControllerNetwork::urlIsFile(url)
        ||
        // FIXME: We are not suggesting torrents for now.
        WControllerPlaylist::urlIsTorrent(url)) return WBackendNetQuery();
#endif
    if (WControllerNetwork::urlIsFile(url)) return WBackendNetQuery();

    WBackendNetQuery query = wControllerPlaylist->queryPlaylist(url);

    query.timeout = 10000; // 10 seconds

    return query;
}

//=================================================================================================
// WLoaderRecentPrivate
//=================================================================================================

WLoaderRecentPrivate::WLoaderRecentPrivate(WLoaderRecent * p) : WLoaderPlaylistPrivate(p) {}

//-------------------------------------------------------------------------------------------------

void WLoaderRecentPrivate::init()
{
    feeds = NULL;

    reply = NULL;

    const QMetaObject * meta = WLoaderPlaylistReply().metaObject();

    method = meta->method(meta->indexOfMethod("extract(QStringList,QStringList,int)"));
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WLoaderRecentPrivate::updateSources()
{
    if (feeds == NULL) return;

    Q_Q(WLoaderRecent);

    // NOTE: When the sources are empty we clear the previously loaded tracks.
    if (sources.isEmpty()) q->clearTracks();

    q->setQueryLoading(true);

    if (reply)
    {
        QObject::disconnect(reply, SIGNAL(loaded(const WLoaderPlaylistData &)),
                            q,     SLOT(onLoaded(const WLoaderPlaylistData &)));
    }

    reply = new WLoaderRecentReply;

    QObject::connect(reply, SIGNAL(loaded(const WLoaderPlaylistData &)),
                     q,     SLOT(onLoaded(const WLoaderPlaylistData &)));

    reply->moveToThread(wControllerPlaylist->thread());

    QStringList urls = getSourcesInput();

    method.invoke(reply, Q_ARG(const QStringList &, urls),
                         Q_ARG(const QStringList &, sources), Q_ARG(int, LOADERRECENT_MAX_COUNT));
}

void WLoaderRecentPrivate::clearQueries()
{

}

//-------------------------------------------------------------------------------------------------

QStringList WLoaderRecentPrivate::getSourcesInput() const
{
    QStringList list;

    foreach (const WLibraryFolderItem * item, feeds->items())
    {
        if (item->type != WLibraryItem::PlaylistFeed) continue;

        QString source = item->source;

        if (source.isEmpty() || list.contains(source)) continue;

        list.append(source);
    }

    return list;
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WLoaderRecentPrivate::onFolderUpdated()
{
    if (active) updateSources();
}

void WLoaderRecentPrivate::onFolderDestroyed()
{
    feeds = NULL;

    if (active) clearQueries();
}

void WLoaderRecentPrivate::onLoaded(const WLoaderPlaylistData & data)
{
    reply = NULL;

    for (int i = 0; i < data.actions.count(); i++)
    {
        const WLoaderPlaylistAction & action = data.actions.at(i);

        WLoaderPlaylist::Action type = action.type;

        if (type == WLoaderPlaylist::Insert)
        {
            WLoaderRecentNode node;

            node.source = action.url;
            node.query  = action.query;

            nodes.insert(action.index, node);
        }
        else if (type == WLoaderPlaylist::Move)
        {
            int from = action.index;

            i++;

            int to = data.actions.at(i).index;

            nodes.move(from, to);
        }
        else if (type == WLoaderPlaylist::Remove)
        {
            nodes.removeAt(action.index);
        }
    }

    sources = data.sources;

    //updatePlaylist(QHash<QString, const WTrack *>());

    //processQueries();
}

//=================================================================================================
// WLoaderRecent
//=================================================================================================

/* explicit */ WLoaderRecent::WLoaderRecent(WLibraryFolder * folder, int id)
    : WLoaderPlaylist(new WLoaderRecentPrivate(this), folder, id)
{
    Q_D(WLoaderRecent); d->init();
}

//-------------------------------------------------------------------------------------------------
// Protected WLoaderPlaylist implementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WLoaderRecent::onStart()
{
}

/* virtual */ void WLoaderRecent::onStop()
{
}

/* virtual */ void WLoaderRecent::onClear()
{
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

WLibraryFolder * WLoaderRecent::feeds() const
{
    Q_D(const WLoaderRecent); return d->feeds;
}

void WLoaderRecent::setFeeds(WLibraryFolder * feeds)
{
    Q_D(WLoaderRecent);

    if (d->feeds == feeds) return;

    if (d->feeds)
    {
        disconnect(d->feeds, 0, this, 0);

        if (d->active) d->clearQueries();
    }

    d->feeds = feeds;

    if (feeds)
    {
        connect(feeds, SIGNAL(countChanged()), this, SLOT(onFolderUpdated  ()));
        connect(feeds, SIGNAL(destroyed   ()), this, SLOT(onFolderDestroyed()));

        if (d->active) d->updateSources();
    }

    emit feedsChanged();
}

#endif // SK_NO_LOADERRECENT

#include "WLoaderRecent.moc"
