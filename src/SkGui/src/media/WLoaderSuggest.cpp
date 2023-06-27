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

static const int LOADERSUGGEST_MAX_COUNT = 30;

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

//-------------------------------------------------------------------------------------------------

void WLoaderSuggestPrivate::processQueries()
{
    Q_Q(WLoaderSuggest);

    if (q->processQueries()) return;

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

    q->clearQueries();

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
        foreach (const WLoaderPlaylistNode & node, nodes)
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
    Q_Q(WLoaderSuggest);

    reply = NULL;

    q->applyActions(data);

    q->applySources(getSourcesOutput(), QHash<QString, const WTrack *>());

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

/* virtual */ void WLoaderSuggest::onApplyPlaylist(WLoaderPlaylistNode * node,
                                                   WPlaylist           * playlist)
{
    Q_D(WLoaderSuggest);

    QStringList sources;

    QHash<QString, const WTrack *> tracks = d->getTracks(playlist, &sources);

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

    applySources(d->getSourcesOutput(), tracks);
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
