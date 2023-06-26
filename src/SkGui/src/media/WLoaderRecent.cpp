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
#include <WControllerPlaylist>
#include <WLibraryFolder>

//-------------------------------------------------------------------------------------------------
// Static variables

static const int LOADERRECENT_TRACKS = 5;

static const int LOADERRECENT_MAX_COUNT   = 50;
static const int LOADERRECENT_MAX_QUERIES =  3;

//=================================================================================================
// WLoaderRecentAction
//=================================================================================================

struct WLoaderRecentAction
{
public:
    WLoaderRecentAction(WLoaderRecentPrivate::Type type)
    {
        this->type = type;

        index = -1;
    }

public: // Variables
    WLoaderRecentPrivate::Type type;

    int index;

    QString url;

    WBackendNetQuery query;
};

//=================================================================================================
// WLoaderRecentData
//=================================================================================================

struct WLoaderRecentData
{
    QStringList sources;

    QList<WLoaderRecentAction> actions;
};

//=================================================================================================
// WLoaderRecentReply
//=================================================================================================

class WLoaderRecentReply : public QObject
{
    Q_OBJECT

public: // Interface
    Q_INVOKABLE void extract(const QStringList & urls, const QStringList & sources);

signals:
    void loaded(const WLoaderRecentData & data);
};

/* Q_INVOKABLE */ void WLoaderRecentReply::extract(const QStringList & urls,
                                                   const QStringList & sources)
{
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

    qRegisterMetaType<WLoaderRecentData>("WLoaderRecentData");

    const QMetaObject * meta = WLoaderRecentReply().metaObject();

    method = meta->method(meta->indexOfMethod("extract(QStringList,QStringList)"));
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
        QObject::disconnect(reply, SIGNAL(loaded(const WLoaderRecentData &)),
                            q,     SLOT(onLoaded(const WLoaderRecentData &)));
    }

    reply = new WLoaderRecentReply;

    QObject::connect(reply, SIGNAL(loaded(const WLoaderRecentData &)),
                     q,     SLOT(onLoaded(const WLoaderRecentData &)));

    reply->moveToThread(wControllerPlaylist->thread());

    QStringList urls = getSourcesInput();

    method.invoke(reply, Q_ARG(const QStringList &, urls), Q_ARG(const QStringList &, sources));
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
