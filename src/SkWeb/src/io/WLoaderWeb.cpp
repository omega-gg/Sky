//=================================================================================================
/*
    Copyright (C) 2015-2016 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkWeb module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#include "WLoaderWeb.h"

#ifndef SK_NO_LOADERWEB

// Qt includes
#include <QNetworkProxy>
#include <QWebPage>
#include <QWebFrame>
#include <QBuffer>

// Sk includes
#include <WControllerDownload>

//=================================================================================================
// WLoaderWebPage
//=================================================================================================

class WLoaderWebPage : public QWebPage
{
    Q_OBJECT

public slots: // QWebPage reimplementation
    bool shouldInterruptJavaScript();

public: // Variables
    QBuffer * buffer;
};

//-------------------------------------------------------------------------------------------------
// QWebPage slots reimplementation
//-------------------------------------------------------------------------------------------------

bool WLoaderWebPage::shouldInterruptJavaScript()
{
    mainFrame()->setContent(QByteArray());

    return true;
}

//=================================================================================================
// WLoaderWebPrivate
//=================================================================================================

WLoaderWebPrivate::WLoaderWebPrivate(WLoaderWeb * p) : WAbstractLoaderPrivate(p) {}

//-------------------------------------------------------------------------------------------------

void WLoaderWebPrivate::init()
{
    Q_Q(WLoaderWeb);

    manager = new QNetworkAccessManager(q);
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WLoaderWebPrivate::onLoadFinished(bool ok)
{
    Q_Q(WLoaderWeb);

    WLoaderWebPage * page = static_cast<WLoaderWebPage *> (q->sender());

    QBuffer * buffer = page->buffer;

    pages.remove(buffer);

    if (ok == false)
    {
        WRemoteData * data = q->getData(buffer);

        q->setError(data, "Error(s) occured while loading the Webpage");
    }

    QByteArray bytes = page->mainFrame()->toHtml().toUtf8();

    buffer->setData(bytes);

    buffer->open(QIODevice::ReadOnly);

    q->complete(buffer);

    page->deleteLater();
}

//=================================================================================================
// WLoaderWeb
//=================================================================================================

/* explicit */ WLoaderWeb::WLoaderWeb(QObject * parent)
    : WAbstractLoader(new WLoaderWebPrivate(this), parent)
{
    Q_D(WLoaderWeb); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WLoaderWeb::setProxy(const QString & host,
                                            int             port, const QString & password)
{
    Q_D(WLoaderWeb);

    QNetworkProxy proxy(QNetworkProxy::HttpProxy, host, port, password);

    d->manager->setProxy(proxy);
}

/* Q_INVOKABLE */ void WLoaderWeb::clearProxy()
{
    Q_D(WLoaderWeb);

    d->manager->setProxy(QNetworkProxy());
}

//-------------------------------------------------------------------------------------------------
// Protected WAbstractLoader implementation
//-------------------------------------------------------------------------------------------------

/* virtual */ QIODevice * WLoaderWeb::load(WRemoteData * data)
{
    Q_D(WLoaderWeb);

    WLoaderWebPage * page = new WLoaderWebPage;

    QBuffer * buffer = new QBuffer;

    page->buffer = buffer;

    page->setNetworkAccessManager(d->manager);

    QWebSettings * settings = page->settings();

    settings->setAttribute(QWebSettings::AutoLoadImages,    false);
    settings->setAttribute(QWebSettings::JavascriptEnabled, false);

    connect(page, SIGNAL(loadFinished(bool)), this, SLOT(onLoadFinished(bool)));

    d->pages.insert(buffer, page);

    page->mainFrame()->load(data->url());

    return buffer;
}

//-------------------------------------------------------------------------------------------------
// Protected WAbstractLoader reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WLoaderWeb::abort(QIODevice * reply)
{
    Q_D(WLoaderWeb);

    WLoaderWebPage * page = d->pages.take(reply);

    disconnect(page, 0, this, 0);

    delete page;
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

QAbstractNetworkCache * WLoaderWeb::cache() const
{
    Q_D(const WLoaderWeb); return d->manager->cache();
}

void WLoaderWeb::setCache(QAbstractNetworkCache * cache)
{
    Q_D(WLoaderWeb);

    if (d->manager->cache() == cache) return;

    d->manager->setCache(cache);

    emit cacheChanged();
}

//-------------------------------------------------------------------------------------------------

QNetworkCookieJar * WLoaderWeb::cookieJar() const
{
    Q_D(const WLoaderWeb); return d->manager->cookieJar();
}

void WLoaderWeb::setCookieJar(QNetworkCookieJar * cookieJar)
{
    Q_D(WLoaderWeb);

    if (d->manager->cookieJar() == cookieJar) return;

    d->manager->setCookieJar(cookieJar);

    emit cookieJarChanged();
}

#endif // SK_NO_LOADERWEB

#include "WLoaderWeb.moc"
