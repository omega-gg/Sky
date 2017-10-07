//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkBackend module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#include "WBackendTmdb.h"

#ifndef SK_NO_BACKENDTMDB

// Sk includes
#include <WControllerNetwork>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include <private/WBackendNet_p>

class SK_BACKEND_EXPORT WBackendTmdbPrivate : public WBackendNetPrivate
{
public:
    WBackendTmdbPrivate(WBackendTmdb * p);

    void init();

public: // Functions
    QUrl getUrl(const QString & q) const;

protected:
    W_DECLARE_PUBLIC(WBackendTmdb)
};

//-------------------------------------------------------------------------------------------------

WBackendTmdbPrivate::WBackendTmdbPrivate(WBackendTmdb * p) : WBackendNetPrivate(p) {}

void WBackendTmdbPrivate::init() {}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

QUrl WBackendTmdbPrivate::getUrl(const QString & q) const
{
    QUrl url("http://www.bing.com/images/search");

    QString search = q;

    search.replace(QRegExp("[,.\\-_(){}\\[\\]]"), " ");

    search = search.simplified();

    search.replace(' ', "+");

    if (search.length() < 10 || search.count('+') < 3)
    {
        return QUrl();
    }

    return url;
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

WBackendTmdb::WBackendTmdb() : WBackendNet(new WBackendTmdbPrivate(this))
{
    Q_D(WBackendTmdb); d->init();
}

//-------------------------------------------------------------------------------------------------
// WBackendNet implementation
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QString WBackendTmdb::getId() const
{
    return "tmdb";
}

/* Q_INVOKABLE virtual */ QString WBackendTmdb::getTitle() const
{
    return "TMDb";
}

//-------------------------------------------------------------------------------------------------
// WBackendNet reimplementation
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
WBackendNetQuery WBackendTmdb::createQuery(const QString & method,
                                           const QString & label, const QString & q) const
{
    WBackendNetQuery query;

    if (method == "cover" && label == "track")
    {
        Q_D(const WBackendTmdb);

        query.url = d->getUrl(q);

        query.maxHost = 1;
    }

    return query;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
WBackendNetTrack WBackendTmdb::extractTrack(const QByteArray       & data,
                                            const WBackendNetQuery &) const
{
    WBackendNetTrack reply;

    /*QString content = Sk::readUtf8(data);

    QString cover = Sk::sliceIn(content, "<div class=\"item\"><a href=\"", "\"");

    if (cover.isEmpty()) return reply;

    cover = WControllerNetwork::decodeUrl(cover);

    reply.track.setCover(cover);*/

    return reply;
}

#endif // SK_NO_BACKENDTMDB
