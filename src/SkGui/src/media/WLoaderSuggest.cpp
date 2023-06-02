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
#include <WControllerPlaylist>
#include <WPlaylist>

//-------------------------------------------------------------------------------------------------
// Static variables

static const int LOADERSUGGEST_MAX = 10;

//=================================================================================================
// WLoaderSuggestAction
//=================================================================================================

struct WLoaderSuggestAction
{
public:
    WLoaderSuggestAction(WLoaderSuggestPrivate::Type type)
    {
        this->type = type;

        index = -1;
    }

public: // Variables
    WLoaderSuggestPrivate::Type type;

    int index;

    QString url;
};

//=================================================================================================
// WLoaderSuggestData
//=================================================================================================

struct WLoaderSuggestData
{
    QStringList sources;

    QList<WLoaderSuggestAction> actions;
};

//=================================================================================================
// WLoaderSuggestReply
//=================================================================================================

class WLoaderSuggestReply : public QObject
{
    Q_OBJECT

public: // Interface
    Q_INVOKABLE void extract(const QStringList & urls, const QStringList & sources);

private: // Functions
    WBackendNetQuery getQuery(const QString & url) const;

signals:
    void loaded(const WLoaderSuggestData & data);
};

/* Q_INVOKABLE */ void WLoaderSuggestReply::extract(const QStringList & urls,
                                                    const QStringList & sources)
{
    WLoaderSuggestData data;

    QList<WLoaderSuggestAction> & actions = data.actions;

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

        WLoaderSuggestAction action(WLoaderSuggestPrivate::Remove);

        action.index = index;

        actions.append(action);
    }

    int count = list.count();

    int total = 0;

    foreach (const QString & url, urls)
    {
        if (total < count)
        {
            if (list.at(total) == url)
            {
                total++;

                if (total == LOADERSUGGEST_MAX) break;

                continue;
            }

            int index = list.indexOf(url);

            if (index == -1)
            {
                WBackendNetQuery query = getQuery(url);

                if (query.isValid() == false) continue;
            }
            else
            {
                if (index < total)
                {
                     list.move(index, total - 1);
                }
                else list.move(index, total);

                WLoaderSuggestAction action(WLoaderSuggestPrivate::Move);

                action.index = index;

                actions.append(action);

                action = WLoaderSuggestAction(WLoaderSuggestPrivate::Insert);

                action.index = total;

                actions.append(action);

                total++;

                if (total == LOADERSUGGEST_MAX) break;

                continue;
            }
        }
        else
        {
            WBackendNetQuery query = getQuery(url);

            if (query.isValid() == false) continue;
        }

        list.insert(total, url);

        WLoaderSuggestAction action(WLoaderSuggestPrivate::Insert);

        action.index = total;

        action.url = url;

        actions.append(action);

        total++;

        if (total == LOADERSUGGEST_MAX) break;
    }

    count = list.count();

    while (count > LOADERSUGGEST_MAX)
    {
        list.removeAt(total);

        WLoaderSuggestAction action(WLoaderSuggestPrivate::Remove);

        action.index = total;

        actions.append(action);

        total++;

        count--;
    }

    data.sources = list;

    emit loaded(data);

    deleteLater();
}

WBackendNetQuery WLoaderSuggestReply::getQuery(const QString & url) const
{
#ifndef SK_NO_TORRENT
    // FIXME: We are not suggesting torrents for now.
    if (WControllerPlaylist::urlIsTorrent(url)) return WBackendNetQuery();
#endif

    WBackendNet * backend = wControllerPlaylist->backendFromUrl(url);

    if (backend == NULL) return WBackendNetQuery();

    QString id = backend->getTrackId(url);

    WBackendNetQuery query = backend->createQuery("related", "tracks", id);

    backend->tryDelete();

    return query;
}

//=================================================================================================
// WLoaderSuggestPrivate
//=================================================================================================

WLoaderSuggestPrivate::WLoaderSuggestPrivate(WLoaderSuggest * p) : WLoaderPlaylistPrivate(p) {}

//-------------------------------------------------------------------------------------------------

void WLoaderSuggestPrivate::init(WPlaylist * history)
{
    Q_ASSERT(history);

    this->history = history;

    qRegisterMetaType<WLoaderSuggestData>("WLoaderSuggestData");

    const QMetaObject * meta = WLoaderSuggestReply().metaObject();

    method = meta->method(meta->indexOfMethod("extract(QStringList,QStringList)"));
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

QStringList WLoaderSuggestPrivate::getSources() const
{
    QStringList list;

    QList<const WTrack *> tracks = history->trackPointers();

    foreach (const WTrack * track, tracks)
    {
        QString source = WControllerPlaylist::cleanSource(track->source());

        if (list.contains(source)) continue;

        list.append(source);
    }

    return list;
}

//-------------------------------------------------------------------------------------------------
// Slots
//-------------------------------------------------------------------------------------------------

void WLoaderSuggestPrivate::onLoaded(const WLoaderSuggestData & data)
{
    // NOTE: When the loader is no longer active we skip the data processing.
    if (active == false) return;

    for (int i = 0; i < data.actions.count(); i++)
    {
        const WLoaderSuggestAction & action = data.actions.at(i);

        qDebug("ACTION %d %d %s", action.type, action.index, action.url.C_STR);

        WLoaderSuggestPrivate::Type type = action.type;

        if (type == WLoaderSuggestPrivate::Insert)
        {
            WLoaderSuggestNode node;

            node.source = action.url;

            nodes.insert(action.index, node);
        }
        else if (type == WLoaderSuggestPrivate::Move)
        {
            int from = action.index;

            i++;

            int to = data.actions.at(i).index;

            if (from < to)
            {
                 nodes.move(from, to - 1);
            }
            else nodes.move(from, to);
        }
        else if (type == WLoaderSuggestPrivate::Remove)
        {
            nodes.removeAt(i);
        }
    }

    foreach (const QString & source, sources)
    {
        qDebug("BEFORE %s", source.C_STR);
    }

    sources = data.sources;

    foreach (const QString & source, sources)
    {
        qDebug("AFTER %s", source.C_STR);
    }
}

//=================================================================================================
// WLoaderSuggest
//=================================================================================================

/* explicit */ WLoaderSuggest::WLoaderSuggest(WLibraryFolder * folder, int id,
                                              WPlaylist      * history)
    : WLoaderPlaylist(new WLoaderSuggestPrivate(this), folder, id)
{
    Q_D(WLoaderSuggest);

    d->init(history);
}

//-------------------------------------------------------------------------------------------------
// Protected WLoaderPlaylist implementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WLoaderSuggest::onStart()
{
    Q_D(WLoaderSuggest);

    QStringList urls = d->getSources();

    WLoaderSuggestReply * reply = new WLoaderSuggestReply;

    QObject::connect(reply, SIGNAL(loaded(const WLoaderSuggestData &)),
                     this,  SLOT(onLoaded(const WLoaderSuggestData &)));

    reply->moveToThread(wControllerPlaylist->thread());

    d->method.invoke(reply, Q_ARG(const QStringList &, urls),
                            Q_ARG(const QStringList &, d->sources));
}

/* virtual */ void WLoaderSuggest::onStop()
{
}

#endif // SK_NO_LOADERSUGGEST

#include "WLoaderSuggest.moc"
