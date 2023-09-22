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

#include "WLoaderHub.h"

#ifndef SK_NO_LOADERHUB

// Sk includes
#include <WControllerPlaylist>
#include <WPlaylist>

//-------------------------------------------------------------------------------------------------
// Static variables

static const int LOADERHUB_MAX_COUNT = 500;

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

WLoaderHubPrivate::WLoaderHubPrivate(WLoaderHub * p) : WLoaderPlaylistPrivate(p) {}

//-------------------------------------------------------------------------------------------------

void WLoaderHubPrivate::init()
{
    type = WTrack::Track;

    history = NULL;

    reply = NULL;

    WLoaderPlaylistReply().applyMethod(&method);
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WLoaderHubPrivate::updateSources()
{
    if (history == NULL) return;

    Q_Q(WLoaderHub);

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
                         Q_ARG(const QStringList &, sources), Q_ARG(int, LOADERHUB_MAX_COUNT));
}

//-------------------------------------------------------------------------------------------------

void WLoaderHubPrivate::processQueries()
{
    Q_Q(WLoaderHub);

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

void WLoaderHubPrivate::clearQueries()
{
    Q_Q(WLoaderHub);

    if (reply)
    {
        QObject::disconnect(reply, SIGNAL(loaded(const WLoaderPlaylistData &)),
                            q,     SLOT(onLoaded(const WLoaderPlaylistData &)));

        reply = NULL;
    }

    q->clearQueries();
}

//-------------------------------------------------------------------------------------------------

QStringList WLoaderHubPrivate::getSourcesInput() const
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

QStringList WLoaderHubPrivate::getSourcesOutput() const
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

QHash<QString, const WTrack *> WLoaderHubPrivate::getTracks(WPlaylist   * playlist,
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

void WLoaderHubPrivate::onPlaylistUpdated()
{
    if (active) updateSources();
}

void WLoaderHubPrivate::onPlaylistDestroyed()
{
    history = NULL;

    if (active) clearQueries();
}

void WLoaderHubPrivate::onLoaded(const WLoaderPlaylistData & data)
{
    Q_Q(WLoaderHub);

    reply = NULL;

    q->applyActions(data);

    processQueries();
}

//=================================================================================================
// WLoaderHub
//=================================================================================================

/* explicit */ WLoaderHub::WLoaderHub(WLibraryFolder * folder, int id)
    : WLoaderPlaylist(new WLoaderHubPrivate(this), folder, id)
{
    Q_D(WLoaderHub); d->init();
}

//-------------------------------------------------------------------------------------------------
// Protected WLoaderPlaylist implementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WLoaderHub::onStart()
{
    Q_D(WLoaderHub); d->updateSources();
}

/* virtual */ void WLoaderHub::onStop()
{
    Q_D(WLoaderHub); d->clearQueries();
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

WTrack::Type WLoaderHub::type() const
{
    Q_D(const WLoaderHub); return d->type;
}

void WLoaderHub::setType(WTrack::Type type)
{
    Q_D(WLoaderHub);

    if (d->type == type) return;

    d->type = type;

    if (d->active) d->updateSources();

    emit typeChanged();
}

WPlaylist * WLoaderHub::history() const
{
    Q_D(const WLoaderHub); return d->history;
}

void WLoaderHub::setHistory(WPlaylist * history)
{
    Q_D(WLoaderHub);

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

QStringList WLoaderHub::baseUrls() const
{
    Q_D(const WLoaderHub); return d->baseUrls;
}

void WLoaderHub::setBaseUrls(const QStringList & urls)
{
    Q_D(WLoaderHub);

    if (d->baseUrls == urls) return;

    d->baseUrls = urls;

    if (d->active) d->updateSources();

    emit baseUrlsChanged();
}

#endif // SK_NO_LOADERHUB
