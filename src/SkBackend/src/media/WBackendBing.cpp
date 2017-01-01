//=================================================================================================
/*
    Copyright (C) 2015-2016 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkBackend module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#include "WBackendBing.h"

#ifndef SK_NO_BACKENDBING

// Qt includes
#ifdef QT_LATEST
#include <QUrlQuery>
#endif

// Sk includes
#include <WControllerApplication>
#include <WControllerNetwork>

//-------------------------------------------------------------------------------------------------
// Static variables

static const QString BACKENDBING_FILTERS
    = "filterui:imagesize-custom_512_512+filterui:aspect-wide";

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include <private/WBackendNet_p>

class SK_BACKEND_EXPORT WBackendBingPrivate : public WBackendNetPrivate
{
public:
    WBackendBingPrivate(WBackendBing * p);

    void init();

public: // Functions
    QUrl getUrl(const QString & q) const;

protected:
    W_DECLARE_PUBLIC(WBackendBing)
};

//-------------------------------------------------------------------------------------------------

WBackendBingPrivate::WBackendBingPrivate(WBackendBing * p) : WBackendNetPrivate(p) {}

void WBackendBingPrivate::init() {}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

QUrl WBackendBingPrivate::getUrl(const QString & q) const
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

#ifdef QT_4
    url.addQueryItem("q", search);

    url.addQueryItem("qft", BACKENDBING_FILTERS);
#else
    QUrlQuery query(url);

    query.addQueryItem("q", search);

    query.addQueryItem("qft", BACKENDBING_FILTERS);

    url.setQuery(query);
#endif

    return url;
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

WBackendBing::WBackendBing() : WBackendNet(new WBackendBingPrivate(this))
{
    Q_D(WBackendBing); d->init();
}

//-------------------------------------------------------------------------------------------------
// WBackendNet implementation
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QString WBackendBing::getId() const
{
    return "bing";
}

/* Q_INVOKABLE virtual */ QString WBackendBing::getTitle() const
{
    return "Bing";
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ bool WBackendBing::checkValidUrl(const QUrl & url) const
{
    QString source = WControllerNetwork::removeUrlPrefix(url);

    return source.startsWith("bing.com");
}

//-------------------------------------------------------------------------------------------------
// WBackendNet reimplementation
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
WBackendNetQuery WBackendBing::createQuery(const QString & method,
                                           const QString & label, const QString & q) const
{
    WBackendNetQuery backendQuery;

    if (method == "cover" && label == "track")
    {
        Q_D(const WBackendBing);

        backendQuery.url = d->getUrl(q);

        backendQuery.maxHost = 1;
    }

    return backendQuery;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
WBackendNetTrack WBackendBing::extractTrack(const QByteArray       & data,
                                            const WBackendNetQuery &) const
{
    WBackendNetTrack reply;

    QString content = Sk::readUtf8(data);

    QString cover = Sk::sliceIn(content, "<div class=\"item\"><a href=\"", "\"");

    if (cover.isEmpty()) return reply;

    cover = WControllerNetwork::decodeUrl(cover);

    reply.track.setCover(cover);

    return reply;
}

#endif // SK_NO_BACKENDBING
