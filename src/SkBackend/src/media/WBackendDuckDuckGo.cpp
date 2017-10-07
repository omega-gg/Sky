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
    QString extractSource(const QString & source) const;

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

QString WBackendDuckDuckGoPrivate::extractSource(const QString & source) const
{
    if (source.startsWith("http") == false)
    {
        int index = source.indexOf("uddg=");

        if (index != -1)
        {
            QString url = source.mid(index + 5);

            return WControllerNetwork::decodeUrl(url);
        }
        else return QString();
    }
    else return source;
}

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
// WBackendNet reimplementation
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ bool WBackendDuckDuckGo::checkValidUrl(const QUrl & url) const
{
    QString source = WControllerNetwork::removeUrlPrefix(url);

    return source.startsWith("duckduckgo.com");
}

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
    WBackendNetQuery query;

    if (method == "search")
    {
        if (label == "urls")
        {
            Q_D(const WBackendDuckDuckGo);

            query.url  = d->getUrl(q);
            query.data = q;

            query.maxHost = 1;
        }
        else if (label == "site")
        {
            Q_D(const WBackendDuckDuckGo);

            query.url  = d->getUrl(q);
            query.id   = 1;
            query.data = q;

            query.maxHost = 1;
            query.delay   = 3000;
        }
    }

    return query;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
WBackendNetFolder WBackendDuckDuckGo::extractFolder(const QByteArray       & data,
                                                    const WBackendNetQuery & query) const
{
    Q_D(const WBackendDuckDuckGo);

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

            source = d->extractSource(source);

            if (source.isEmpty()) continue;

            index = WControllerNetwork::indexValue(string, index);

            QString title = WControllerNetwork::extractNodeAt(string, "</a>", index);

            title = WControllerNetwork::htmlToUtf8(title);

            title = WControllerNetwork::removeUrlPrefix(title);

            WLibraryFolderItem folder(WLibraryItem::FolderSearch, WLocalObject::Default);

            folder.source = QUrl::fromEncoded(source.toLatin1());

            folder.title = title;

            reply.items.append(folder);
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

            source = d->extractSource(source);

            if (source.isEmpty()) continue;

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
