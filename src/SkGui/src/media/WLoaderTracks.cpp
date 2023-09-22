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

#include "WLoaderTracks.h"

#ifndef SK_NO_LOADERTRACKS

// Sk includes
#include <WControllerPlaylist>
#include <WPlaylist>

//-------------------------------------------------------------------------------------------------
// Static variables

static const int LOADERTRACKS_MAX_COUNT = 500;

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

WLoaderTracksPrivate::WLoaderTracksPrivate(WLoaderTracks * p) : WLoaderPlaylistPrivate(p) {}

//-------------------------------------------------------------------------------------------------

void WLoaderTracksPrivate::init()
{
    type = WTrack::Track;

    history = NULL;

    reply = NULL;

    WLoaderPlaylistReply().applyMethod(&method);
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WLoaderTracksPrivate::updateSources()
{
    if (history == NULL) return;

    Q_Q(WLoaderTracks);

    // NOTE: When the sources are empty we clear the previously loaded tracks.
    if (sources.isEmpty()) q->clearTracks();

    q->setQueryLoading(true);

    if (reply)
    {
        QObject::disconnect(reply, SIGNAL(loaded(const WLoaderPlaylistData &)),
                            q,     SLOT(onLoaded(const WLoaderPlaylistData &)));
    }

    reply = new WLoaderPlaylistReply;

    QObject::connect(reply, SIGNAL(loaded(const WLoaderPlaylistData &)),
                     q,     SLOT(onLoaded(const WLoaderPlaylistData &)));

    reply->moveToThread(wControllerPlaylist->thread());

    QStringList urls = getSourcesInput();

    method.invoke(reply, Q_ARG(const QStringList &, urls),
                         Q_ARG(const QStringList &, sources), Q_ARG(int, LOADERTRACKS_MAX_COUNT));
}

//-------------------------------------------------------------------------------------------------

void WLoaderTracksPrivate::processQueries()
{
    Q_Q(WLoaderTracks);

    QHash<QString, const WTrack *> tracks;

    for (int i = 0; i < nodes.count(); i++)
    {
        WLoaderPlaylistNode * node = &(nodes[i]);

        const WBackendNetQuery & query = node->query;

        if (query.isValid() == false) continue;

        QString url = query.url;

        node->query = WBackendNetQuery();

        QStringList sources;

        sources.append(url);

        QList<QStringList> & urls = node->urls;

        urls.clear();

        urls.append(sources);

        int index = history->indexFromSource(url, true);

        if (index == -1) continue;

        tracks.insert(url, history->trackPointerAt(index));
    }

    q->applySources(getSourcesOutput(), tracks);

    q->setQueryLoading(false);
}

void WLoaderTracksPrivate::clearQueries()
{
    Q_Q(WLoaderTracks);

    if (reply)
    {
        QObject::disconnect(reply, SIGNAL(loaded(const WLoaderPlaylistData &)),
                            q,     SLOT(onLoaded(const WLoaderPlaylistData &)));

        reply = NULL;
    }

    q->clearQueries();
}

//-------------------------------------------------------------------------------------------------

QStringList WLoaderTracksPrivate::getSourcesInput() const
{
    QStringList list;

    foreach (const WTrack * track, history->trackPointers())
    {
        if (track->type() != type) continue;

        QString source = WControllerPlaylist::cleanSource(track->source());

        if (source.isEmpty() || list.contains(source)) continue;

        list.append(source);
    }

    foreach (const QString & url, baseUrls)
    {
        if (list.contains(url)) continue;

        list.append(url);
    }

    return list;
}

QStringList WLoaderTracksPrivate::getSourcesOutput() const
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

QHash<QString, const WTrack *> WLoaderTracksPrivate::getTracks(WPlaylist   * playlist,
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
// Private slots
//-------------------------------------------------------------------------------------------------

void WLoaderTracksPrivate::onPlaylistUpdated()
{
    if (active) updateSources();
}

void WLoaderTracksPrivate::onPlaylistDestroyed()
{
    history = NULL;

    if (active) clearQueries();
}

void WLoaderTracksPrivate::onLoaded(const WLoaderPlaylistData & data)
{
    Q_Q(WLoaderTracks);

    reply = NULL;

    q->applyActions(data);

    processQueries();
}

//=================================================================================================
// WLoaderTracks
//=================================================================================================

/* explicit */ WLoaderTracks::WLoaderTracks(WLibraryFolder * folder, int id)
    : WLoaderPlaylist(new WLoaderTracksPrivate(this), folder, id)
{
    Q_D(WLoaderTracks); d->init();
}

//-------------------------------------------------------------------------------------------------
// Protected WLoaderPlaylist implementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WLoaderTracks::onStart()
{
    Q_D(WLoaderTracks); d->updateSources();
}

/* virtual */ void WLoaderTracks::onStop()
{
    Q_D(WLoaderTracks); d->clearQueries();
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

WTrack::Type WLoaderTracks::type() const
{
    Q_D(const WLoaderTracks); return d->type;
}

void WLoaderTracks::setType(WTrack::Type type)
{
    Q_D(WLoaderTracks);

    if (d->type == type) return;

    d->type = type;

    if (d->active) d->updateSources();

    emit typeChanged();
}

WPlaylist * WLoaderTracks::history() const
{
    Q_D(const WLoaderTracks); return d->history;
}

void WLoaderTracks::setHistory(WPlaylist * history)
{
    Q_D(WLoaderTracks);

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

QStringList WLoaderTracks::baseUrls() const
{
    Q_D(const WLoaderTracks); return d->baseUrls;
}

void WLoaderTracks::setBaseUrls(const QStringList & urls)
{
    Q_D(WLoaderTracks);

    if (d->baseUrls == urls) return;

    d->baseUrls = urls;

    if (d->active) d->updateSources();

    emit baseUrlsChanged();
}

#endif // SK_NO_LOADERTRACKS
