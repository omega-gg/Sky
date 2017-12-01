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
    QUrl url("https://duckduckgo.com/");

    QString search = q.simplified();

#ifdef QT_4
    url.addQueryItem("q", search);

    url.addQueryItem("kl", "en-us");

    url.addQueryItem("kp", "-1");
#else
    QUrlQuery query(url);

    query.addQueryItem("q", search);

    query.addQueryItem("kl", "en-us");

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
// WBackendNet reimplementation
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ bool WBackendDuckDuckGo::isSearchEngine() const
{
    return true;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ bool WBackendDuckDuckGo::checkValidUrl(const QUrl & url) const
{
    QString source = WControllerNetwork::removeUrlPrefix(url);

    return source.startsWith("duckduckgo.com");
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
WBackendNetQuery WBackendDuckDuckGo::createQuery(const QString & method,
                                                 const QString & label, const QString & q) const
{
    WBackendNetQuery query;

    if (method == "search")
    {
        if (label == "urls")
        {
            Q_D(const WBackendDuckDuckGo);

            query.url  = d->getUrl(q);
            query.data = q;
        }
        else if (label == "site")
        {
            Q_D(const WBackendDuckDuckGo);

            query.url  = d->getUrl(q);
            query.id   = 2;
            query.data = q;
        }
    }

    return query;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
WBackendNetFolder WBackendDuckDuckGo::extractFolder(const QByteArray       & data,
                                                    const WBackendNetQuery & query) const
{
    WBackendNetFolder reply;

    QString content = Sk::readUtf8(data);

    int id = query.id;

    if (id == 1) // Search urls
    {
        QStringList urls;

        QString string = Sk::sliceIn(data, "('d',[", "]);");

        QStringList list = WControllerNetwork::splitJson(string);

        foreach (const QString & string, list)
        {
            QString source = WControllerNetwork::extractJsonUtf8(string, "c");

            if (source.isEmpty()) continue;

            source = WControllerNetwork::urlName(source);

            if (urls.contains(source)) continue;

            urls.append(source);

            WLibraryFolderItem folder(WLibraryItem::FolderSearch, WLocalObject::Default);

            folder.title = source;

            reply.items.append(folder);
        }
    }
    else if (id == 3) // Search site
    {
        QString string = Sk::sliceIn(data, "('d',[", "]);");

        QStringList list = WControllerNetwork::splitJson(string);

        foreach (const QString & string, list)
        {
            QString source = WControllerNetwork::extractJsonUtf8(string, "c");

            if (source.isEmpty()) continue;

            QString title = WControllerNetwork::extractJsonUtf8(string, "t");

            if (title == "EOF") break;

            WLibraryFolderItem folder(WLibraryItem::FolderSearch, WLocalObject::Default);

            folder.source = WControllerNetwork::encodedUrl(source);

            folder.title = title;

            reply.items.append(folder);
        }
    }
    else
    {
        QString source = Sk::sliceIn(content, "nrje('", "'");

        WBackendNetQuery * nextQuery = &(reply.nextQuery);

        nextQuery->url = WControllerNetwork::encodedUrl("https://duckduckgo.com" + source);

        nextQuery->id = id + 1;
    }

    reply.scanItems = true;

    return reply;
}

#endif // SK_NO_BACKENDDUCKDUCKGO
