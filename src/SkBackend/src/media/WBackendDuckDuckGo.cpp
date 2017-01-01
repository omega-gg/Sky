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

#include "WBackendDuckDuckGo.h"

#ifndef SK_NO_BACKENDDUCKDUCKGO

// Qt includes
#ifdef QT_LATEST
#include <QUrlQuery>
#endif

// Sk includes
#include <WControllerApplication>
#include <WControllerNetwork>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include <private/WBackendNet_p>

class SK_BACKEND_EXPORT WBackendDuckDuckGoPrivate : public WBackendNetPrivate
{
public:
    WBackendDuckDuckGoPrivate(WBackendDuckDuckGo * p);

    void init();

public: // Functions
    QUrl getUrl(const QString & q) const;

protected:
    W_DECLARE_PUBLIC(WBackendDuckDuckGo)
};

//-------------------------------------------------------------------------------------------------

WBackendDuckDuckGoPrivate::WBackendDuckDuckGoPrivate(WBackendDuckDuckGo * p)
    : WBackendNetPrivate(p) {}

void WBackendDuckDuckGoPrivate::init() {}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

QUrl WBackendDuckDuckGoPrivate::getUrl(const QString & q) const
{
    QUrl url("https://duckduckgo.com/html");

    QString search = q.simplified();

    search.replace(' ', '+');

#ifdef QT_4
    url.addQueryItem("q", search);

    url.addQueryItem("kp", "-1");
#else
    QUrlQuery query(url);

    query.addQueryItem("q", search);

    query.addQueryItem("kp", "-1");

    url.setQuery(query);
#endif

    return url;
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

WBackendDuckDuckGo::WBackendDuckDuckGo() : WBackendNet(new WBackendDuckDuckGoPrivate(this))
{
    Q_D(WBackendDuckDuckGo); d->init();
}

//-------------------------------------------------------------------------------------------------
// WBackendNet implementation
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QString WBackendDuckDuckGo::getId() const
{
    return "duckduckgo";
}

/* Q_INVOKABLE virtual */ QString WBackendDuckDuckGo::getTitle() const
{
    return "DuckDuckGo";
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ bool WBackendDuckDuckGo::checkValidUrl(const QUrl & url) const
{
    QString source = WControllerNetwork::removeUrlPrefix(url);

    return source.startsWith("duckduckgo.com");
}

//-------------------------------------------------------------------------------------------------
// WBackendNet reimplementation
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ bool WBackendDuckDuckGo::isSearchEngine() const
{
    return true;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
WBackendNetQuery WBackendDuckDuckGo::createQuery(const QString & method,
                                                 const QString & label, const QString & q) const
{
    WBackendNetQuery backendQuery;

    if (method == "search")
    {
        if (label == "urls")
        {
            Q_D(const WBackendDuckDuckGo);

            backendQuery.url  = d->getUrl(q);
            backendQuery.data = q;

            backendQuery.cookies = true;
            backendQuery.maxHost = 1;
        }
        else if (label == "site")
        {
            Q_D(const WBackendDuckDuckGo);

            backendQuery.url  = d->getUrl(q);
            backendQuery.id   = 1;
            backendQuery.data = q;

            backendQuery.cookies = true;
            backendQuery.maxHost = 1;
            backendQuery.delay   = 3000;
        }
    }

    return backendQuery;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
WBackendNetFolder WBackendDuckDuckGo::extractFolder(const QByteArray       & data,
                                                    const WBackendNetQuery & query) const
{
    WBackendNetFolder reply;

    QString content = Sk::readUtf8(data);

    QStringList list
        = Sk::slices(content,
                     "<div class=\"result results_links results_links_deep web-result \"", "</a>");

    if (query.id == 1)
    {
        foreach (const QString & string, list)
        {
            int index = WControllerNetwork::indexAttribute(string, "href");

            if (index == -1) continue;

            QString source = WControllerNetwork::extractAttributeUtf8At(string, index);

            index = WControllerNetwork::indexValue(string, index);

            QString title = WControllerNetwork::extractNodeAt(string, "</a>", index);

            title = WControllerNetwork::htmlToUtf8(title);

            title = WControllerNetwork::removeUrlPrefix(title);

            WLibraryFolderItem playlist(WLibraryItem::PlaylistNet, WLocalObject::Default);

            playlist.source = QUrl::fromEncoded(source.toLatin1());

            playlist.title = title;

            reply.items.append(playlist);
        }
    }
    else // if (query.id == 0)
    {
        QList<QUrl> urls;

        foreach (const QString & string, list)
        {
            int index = WControllerNetwork::indexAttribute(string, "href");

            if (index == -1) continue;

            QString source = WControllerNetwork::extractAttributeUtf8At(string, index);

            source = WControllerNetwork::urlName(source);

            if (urls.contains(source)) continue;

            urls.append(source);

            WLibraryFolderItem folder(WLibraryItem::FolderSearch, WLocalObject::Default);

            folder.title = source;

            reply.items.append(folder);
        }
    }

    reply.scanItems = true;

    return reply;
}

#endif // SK_NO_BACKENDDUCKDUCKGO
