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
#include <WPlaylist>

//-------------------------------------------------------------------------------------------------
// Static variables

static const int LOADERRECENT_TRACKS = 3;

static const int LOADERRECENT_DELAY = 1; // 10 minutes

static const int LOADERRECENT_MAX_COUNT = 50;

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
    return WLoaderRecentPrivate::getQuery(url);
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

    WLoaderPlaylistReply().applyMethod(&method);
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

    if (reply) reply->abortAndDelete(q);

    reply = new WLoaderRecentReply;

    QObject::connect(reply, SIGNAL(loaded(const WLoaderPlaylistData &)),
                     q,     SLOT(onLoaded(const WLoaderPlaylistData &)));

    reply->moveToThread(wControllerPlaylist->thread());

    QStringList urls = getSourcesInput();

    method.invoke(reply, Q_ARG(const QStringList &, urls),
                         Q_ARG(const QStringList &, sources), Q_ARG(int, LOADERRECENT_MAX_COUNT));
}

//-------------------------------------------------------------------------------------------------

void WLoaderRecentPrivate::clearQueries()
{
    Q_Q(WLoaderRecent);

    if (reply)
    {
        reply->abortAndDelete(q);

        reply = NULL;
    }

    q->clearQueries();
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

QStringList WLoaderRecentPrivate::getSourcesOutput() const
{
    QStringList list;

    foreach (const WLoaderPlaylistNode & node, nodes)
    {
        const QList<QStringList> & urls = node.urls;

        if (urls.isEmpty()) continue;

        foreach (const QString & url, urls.first())
        {
            if (list.contains(url)) continue;

            list.append(url);
        }
    }

    return list;
}

//-------------------------------------------------------------------------------------------------

QHash<QString, const WTrack *> WLoaderRecentPrivate::getTracks(WPlaylist   * playlist,
                                                               QStringList * urls) const
{
    QHash<QString, const WTrack *> hash;

    if (playlist == NULL) return hash;

    foreach (const WTrack * track, playlist->trackPointers())
    {
        QString source = WControllerPlaylist::cleanSource(track->source());

        if (urls->contains(source)) continue;

        urls->append(source);

        hash.insert(source, track);
    }

    return hash;
}

//-------------------------------------------------------------------------------------------------
// Private static functions
//-------------------------------------------------------------------------------------------------

/* static */ WBackendNetQuery WLoaderRecentPrivate::getQuery(const QString & url)
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
    Q_Q(WLoaderRecent);

    reply->deleteLater();

    reply = NULL;

    q->applyActions(data);

    q->applySources(getSourcesOutput(), QHash<QString, const WTrack *>());

    QDateTime currentDate = QDateTime::currentDateTime();

    if (date.isValid() == false)
    {
        date = currentDate;

        q->processQueries();

        return;
    }

    if (currentDate < date.addSecs(LOADERRECENT_DELAY))
    {
        q->processQueries();

        return;
    }

    // NOTE: Once the delay is reached we reload the queries.

    date = currentDate;

    for (int i = 0; i < nodes.count(); i++)
    {
        WLoaderPlaylistNode * node = &(nodes[i]);

        WBackendNetQuery & query = node->query;

        // NOTE: If a query already exists we skip it.
        if (query.isValid()) continue;

        query = WLoaderRecentPrivate::getQuery(node->source);
    }

    q->processQueries();
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
    Q_D(WLoaderRecent); d->updateSources();
}

/* virtual */ void WLoaderRecent::onStop()
{
    Q_D(WLoaderRecent); d->clearQueries();
}

/* virtual */ void WLoaderRecent::onApplyPlaylist(WLoaderPlaylistNode * node, WPlaylist * playlist)
{
    Q_D(WLoaderRecent);

    QStringList sources;

    QHash<QString, const WTrack *> tracks = d->getTracks(playlist, &sources);

    QList<QStringList> & urls = node->urls;

    urls.clear();

    QStringList list;

    int index = 0;

    while (sources.count() && index < LOADERRECENT_TRACKS)
    {
        list.append(sources.takeFirst());

        index++;
    }

    urls.append(list);

    applySources(d->getSourcesOutput(), tracks, true);
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
