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

#include "WLoaderSuggest.h"

#ifndef SK_NO_LOADERSUGGEST

// Sk includes
#include <WControllerNetwork>
#include <WControllerPlaylist>
#include <WPlaylist>

//-------------------------------------------------------------------------------------------------
// Static variables

static const int LOADERSUGGEST_TRACKS = 3;
static const int LOADERSUGGEST_SLICES = 3;

static const int LOADERSUGGEST_MAX_COUNT   = 30;
static const int LOADERSUGGEST_MAX_QUERIES =  3;

static const int LOADERSUGGEST_MAX_SKIP = 100;

//=================================================================================================
// WLoaderSuggestReply
//=================================================================================================

class WLoaderSuggestReply : public WLoaderPlaylistReply
{
    Q_OBJECT

public:
    explicit WLoaderSuggestReply(const QStringList & titles);

protected: // WLoaderPlaylistReply reimplementation
    /* virtual */ WBackendNetQuery getQuery(const QString & url, int index) const;

private: // Variables
    QStringList titles;
};

/* explicit */ WLoaderSuggestReply::WLoaderSuggestReply(const QStringList & titles)
    : WLoaderPlaylistReply()
{
    this->titles = titles;
}

WBackendNetQuery WLoaderSuggestReply::getQuery(const QString & url, int index) const
{
#ifndef SK_NO_TORRENT
    if (WControllerNetwork::urlIsFile(url)
        ||
        // FIXME: We are not suggesting torrents for now.
        WControllerPlaylist::urlIsTorrent(url)) return WBackendNetQuery();
#endif
    if (WControllerNetwork::urlIsFile(url)) return WBackendNetQuery();

    WBackendNetQuery query = wControllerPlaylist->queryRelatedTracks(url, titles.at(index));

    query.timeout = 10000; // 10 seconds

    return query;
}

//=================================================================================================
// WLoaderSuggestPrivate
//=================================================================================================

WLoaderSuggestPrivate::WLoaderSuggestPrivate(WLoaderSuggest * p) : WLoaderPlaylistPrivate(p) {}

//-------------------------------------------------------------------------------------------------

void WLoaderSuggestPrivate::init()
{
    history = NULL;

    reply = NULL;

    const QMetaObject * meta = WLoaderPlaylistReply().metaObject();

    method = meta->method(meta->indexOfMethod("extract(QStringList,QStringList,int)"));
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WLoaderSuggestPrivate::updateSources()
{
    if (history == NULL) return;

    Q_Q(WLoaderSuggest);

    // NOTE: When the sources are empty we clear the previously loaded tracks.
    if (sources.isEmpty()) q->clearTracks();

    q->setQueryLoading(true);

    if (reply)
    {
        QObject::disconnect(reply, SIGNAL(loaded(const WLoaderPlaylistData &)),
                            q,     SLOT(onLoaded(const WLoaderPlaylistData &)));
    }

    QStringList titles;

    QStringList urls = getSourcesInput(titles);

    reply = new WLoaderSuggestReply(titles);

    QObject::connect(reply, SIGNAL(loaded(const WLoaderPlaylistData &)),
                     q,     SLOT(onLoaded(const WLoaderPlaylistData &)));

    reply->moveToThread(wControllerPlaylist->thread());

    method.invoke(reply, Q_ARG(const QStringList &, urls),
                         Q_ARG(const QStringList &, sources), Q_ARG(int, LOADERSUGGEST_MAX_COUNT));
}

void WLoaderSuggestPrivate::updatePlaylist(const QHash<QString, const WTrack *> & tracks)
{
    WPlaylist * playlist = item->toPlaylist();

    if (playlist == NULL) return;

    QStringList list = getSourcesOutput();

    if (playlist->isEmpty())
    {
        foreach (const QString & url, list)
        {
            const WTrack * trackPointer = tracks.value(url);

            if (trackPointer)
            {
                WTrack track = *trackPointer;

                track.setId(-1);

                playlist->addTrack(track);
            }
            else
            {
                WTrack track(url, WTrack::Default);

                playlist->addTrack(track);
            }
        }

        return;
    }

    QString source = WControllerPlaylist::cleanSource(playlist->trackSource(0));

    int total = 0;

    foreach (const QString & url, list)
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

                playlist->insertTrack(total, track);
            }
            else
            {
                WTrack track(url, WTrack::Default);

                playlist->insertTrack(total, track);
            }
        }
        else playlist->moveTrack(index, total);

        total++;
    }

    int countA = list.count();

    int countB = playlist->count();

    if (countA < countB)
    {
        playlist->removeTracks(countA, countB - countA);
    }

    /*foreach (const QString & url, list)
    {
        qDebug("LIST A %s", url.C_STR);
    }

    foreach (const WTrack * track, playlist->trackPointers())
    {
        qDebug("LIST B %s", track->source().C_STR);
    }*/
}

//-------------------------------------------------------------------------------------------------

void WLoaderSuggestPrivate::processQueries()
{
    for (int i = 0; i < nodes.count(); i++)
    {
        WLoaderSuggestNode * node = &(nodes[i]);

        const WBackendNetQuery & query = node->query;

        if (query.isValid() == false) continue;

        WPlaylist * playlist = getPlaylist();

        if (playlist == NULL) return;

        playlist->applyQuery(query);

        jobs.insert(playlist, node);
    }

    if (jobs.isEmpty() == false) return;

    Q_Q(WLoaderSuggest);

    q->setQueryLoading(false);
}

void WLoaderSuggestPrivate::clearQueries()
{
    Q_Q(WLoaderSuggest);

    if (reply)
    {
        QObject::disconnect(reply, SIGNAL(loaded(const WLoaderPlaylistData &)),
                            q,     SLOT(onLoaded(const WLoaderPlaylistData &)));

        reply = NULL;
    }

    foreach (WPlaylist * playlist, playlists)
    {
        QObject::disconnect(playlist, 0, q, 0);

        playlist->abortQueries();

        playlist->tryDelete();
    }

    playlists.clear();
    jobs     .clear();

    q->setQueryLoading(false);
}

//-------------------------------------------------------------------------------------------------

QStringList WLoaderSuggestPrivate::getSourcesInput(QStringList & titles) const
{
    QStringList list;

    foreach (const WTrack * track, history->trackPointers())
    {
        QString source = WControllerPlaylist::cleanSource(track->source());

        if (source.isEmpty() || list.contains(source)) continue;

        list.append(source);

        titles.append(track->title());
    }

    return list;
}

QStringList WLoaderSuggestPrivate::getSourcesOutput() const
{
    QStringList list;

    for (int i = 0; i < LOADERSUGGEST_SLICES; i++)
    {
        foreach (const WLoaderSuggestNode & node, nodes)
        {
            const QList<QStringList> & urls = node.urls;

            if (urls.isEmpty()) continue;

            foreach (const QString & url, urls.at(i))
            {
                if (list.contains(url)) continue;

                list.append(url);
            }
        }
    }

    return list;
}

//-------------------------------------------------------------------------------------------------

QHash<QString, const WTrack *> WLoaderSuggestPrivate::getTracks(WPlaylist   * playlist,
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

    //---------------------------------------------------------------------------------------------
    // NOTE: When a given source is sufficiently recent in the history we skip it.

    QList<const WTrack *> tracks = history->trackPointers();

    for (int i = 0; i < tracks.count() && i < LOADERSUGGEST_MAX_SKIP; i++)
    {
        QString source = WControllerPlaylist::cleanSource(tracks.at(i)->source());

        if (urls->contains(source) == false) continue;

        urls->removeOne(source);

        hash.remove(source);
    }

    return hash;
}

WPlaylist * WLoaderSuggestPrivate::getPlaylist()
{
    foreach (WPlaylist * playlist, playlists)
    {
        if (playlist->queryIsLoading()) continue;

        return playlist;
    }

    if (playlists.count() == LOADERSUGGEST_MAX_QUERIES) return NULL;

    Q_Q(WLoaderSuggest);

    WPlaylist * playlist = new WPlaylist;

    playlist->setParent(q);

    playlists.append(playlist);

    QObject::connect(playlist, SIGNAL(queryCompleted()), q, SLOT(onQueryCompleted()));

    return playlist;
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WLoaderSuggestPrivate::onPlaylistUpdated()
{
    if (active) updateSources();
}

void WLoaderSuggestPrivate::onPlaylistDestroyed()
{
    history = NULL;

    if (active) clearQueries();
}

void WLoaderSuggestPrivate::onLoaded(const WLoaderPlaylistData & data)
{
    reply = NULL;

    for (int i = 0; i < data.actions.count(); i++)
    {
        const WLoaderPlaylistAction & action = data.actions.at(i);

        WLoaderPlaylist::Action type = action.type;

        if (type == WLoaderPlaylist::Insert)
        {
            WLoaderSuggestNode node;

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

    /*foreach (const QString & source, sources)
    {
        qDebug("BEFORE %s", source.C_STR);
    }*/

    sources = data.sources;

    /*foreach (const QString & source, sources)
    {
        qDebug("AFTER %s", source.C_STR);
    }*/

    updatePlaylist(QHash<QString, const WTrack *>());

    processQueries();
}

void WLoaderSuggestPrivate::onQueryCompleted()
{
    Q_Q(WLoaderSuggest);

    WPlaylist * playlist = static_cast<WPlaylist *> (q->sender());

    WLoaderSuggestNode * node = jobs.take(playlist);

    node->query = WBackendNetQuery();

    QStringList sources;

    QHash<QString, const WTrack *> tracks = getTracks(playlist, &sources);

    QList<QStringList> & urls = node->urls;

    urls.clear();

    for (int i = 0; i < LOADERSUGGEST_SLICES; i++)
    {
        QStringList list;

        int index = 0;

        while (sources.count() && index < LOADERSUGGEST_TRACKS)
        {
            list.append(sources.takeFirst());

            index++;
        }

        urls.append(list);
    }

    updatePlaylist(tracks);

    processQueries();
}

//=================================================================================================
// WLoaderSuggest
//=================================================================================================

/* explicit */ WLoaderSuggest::WLoaderSuggest(WLibraryFolder * folder, int id)
    : WLoaderPlaylist(new WLoaderSuggestPrivate(this), folder, id)
{
    Q_D(WLoaderSuggest); d->init();
}

//-------------------------------------------------------------------------------------------------
// Protected WLoaderPlaylist implementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WLoaderSuggest::onStart()
{
    Q_D(WLoaderSuggest); d->updateSources();
}

/* virtual */ void WLoaderSuggest::onStop()
{
    Q_D(WLoaderSuggest); d->clearQueries();
}

/* virtual */ void WLoaderSuggest::onClear()
{
    Q_D(WLoaderSuggest);

    d->item->toPlaylist()->clearTracks();

    d->sources.clear();

    d->nodes.clear();
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

WPlaylist * WLoaderSuggest::history() const
{
    Q_D(const WLoaderSuggest); return d->history;
}

void WLoaderSuggest::setHistory(WPlaylist * history)
{
    Q_D(WLoaderSuggest);

    if (d->history == history) return;

    if (d->history)
    {
        disconnect(d->history, 0, this, 0);

        if (d->active) d->clearQueries();
    }

    d->history = history;

    if (history)
    {
        connect(history, SIGNAL(playlistUpdated()), this, SLOT(onPlaylistUpdated  ()));
        connect(history, SIGNAL(destroyed      ()), this, SLOT(onPlaylistDestroyed()));

        if (d->active) d->updateSources();
    }

    emit historyChanged();
}

#endif // SK_NO_LOADERSUGGEST

#include "WLoaderSuggest.moc"
