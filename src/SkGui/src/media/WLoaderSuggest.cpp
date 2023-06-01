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
// WLoaderSuggestData
//=================================================================================================

struct WLoaderSuggestData
{
};

//=================================================================================================
// WLoaderSuggestReply
//=================================================================================================

class WLoaderSuggestReply : public QObject
{
    Q_OBJECT

public: // Interface
    Q_INVOKABLE void extract(const QStringList & sources);

signals:
    void loaded(const WLoaderSuggestData & data);
};

/* Q_INVOKABLE */ void WLoaderSuggestReply::extract(const QStringList & sources)
{
    WLoaderSuggestData data;

    emit loaded(data);
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

    const QMetaObject * meta = WLoaderSuggestReply().metaObject();

    method = meta->method(meta->indexOfMethod("extract(QStringList&)"));
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
        QString source = track->source();

        if (list.contains(source)) continue;

        list.append(source);
    }

    return list;
}

WBackendNetQuery WLoaderSuggestPrivate::getQuery(const QString & url) const
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

    int count = d->sources.count();

    int total = 0;

    QStringList list = d->getSources();

    foreach (const QString & url, list)
    {
        if (total < count)
        {
            if (d->sources.at(total) == url)
            {
                total++;

                continue;
            }

            int index = d->sources.indexOf(url);

            if (index == -1)
            {
                WBackendNetQuery query = d->getQuery(url);

                if (query.isValid() == false) continue;
            }
            else
            {
                // FIXME

                d->sources.removeAt(index);
            }

            d->sources.insert(total, url);
        }
        else
        {
            WBackendNetQuery query = d->getQuery(url);

            if (query.isValid() == false) continue;

            d->sources.append(url);
        }

        total++;

        if (total == LOADERSUGGEST_MAX) break;
    }

    count = d->sources.count();

    while (count > LOADERSUGGEST_MAX)
    {
        d->sources.removeLast();

        count--;
    }

    foreach (const QString & string, d->sources)
    {
        qDebug("SOURCE %s", string.C_STR);
    }
}

/* virtual */ void WLoaderSuggest::onStop()
{
}

#endif // SK_NO_LOADERSUGGEST

#include "WLoaderSuggest.moc"
