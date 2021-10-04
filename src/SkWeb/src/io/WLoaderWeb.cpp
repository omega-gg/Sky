//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkWeb.

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

#include "WLoaderWeb.h"

#ifndef SK_NO_LOADERWEB

// Qt includes
#ifdef QT_4
#include <QNetworkProxy>
#include <QWebPage>
#include <QWebFrame>
#else
#include <QWebEnginePage>
#include <QWebEngineSettings>
#endif
#include <QBuffer>

// Sky includes
#include <WControllerDownload>

//=================================================================================================
// WLoaderWebPage
//=================================================================================================

#ifdef QT_4
class WLoaderWebPage : public QWebPage
#else
class WLoaderWebPage : public QWebEnginePage
#endif
{
    Q_OBJECT

#ifdef QT_4
public slots: // QWebPage reimplementation
    bool shouldInterruptJavaScript();
#endif

public: // Variables
#ifdef QT_LATEST
    WLoaderWeb * loader;
#endif

    QBuffer * buffer;
};

#ifdef QT_4

//-------------------------------------------------------------------------------------------------
// QWebPage slots reimplementation
//-------------------------------------------------------------------------------------------------

bool WLoaderWebPage::shouldInterruptJavaScript()
{
    mainFrame()->setContent(QByteArray());

    return true;
}

#endif

//=================================================================================================
// WLoaderWebPrivate
//=================================================================================================

WLoaderWebPrivate::WLoaderWebPrivate(WLoaderWeb * p) : WAbstractLoaderPrivate(p) {}

//-------------------------------------------------------------------------------------------------

void WLoaderWebPrivate::init()
{
#ifdef QT_4
    Q_Q(WLoaderWeb);

    manager = new QNetworkAccessManager(q);
#endif
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
        q->setError(q->getData(buffer), "Error(s) occured while loading the Webpage");
    }

#ifdef QT_4
    QByteArray bytes = page->mainFrame()->toHtml().toUtf8();

    buffer->setData(bytes);

    buffer->open(QIODevice::ReadOnly);

    q->complete(buffer);
#else
    page->toHtml([q, buffer](const QString & html)
    {
        buffer->setData(html.toUtf8());

        buffer->open(QIODevice::ReadOnly);

        q->complete(buffer);
    });
#endif

    QObject::disconnect(page, 0, q, 0);

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

#ifdef QT_4

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

#endif

//-------------------------------------------------------------------------------------------------
// Protected WAbstractLoader implementation
//-------------------------------------------------------------------------------------------------

/* virtual */ QIODevice * WLoaderWeb::load(WRemoteData * data)
{
    Q_D(WLoaderWeb);

    WLoaderWebPage * page = new WLoaderWebPage;

    QBuffer * buffer = new QBuffer;

    page->buffer = buffer;

#ifdef QT_4
    page->setNetworkAccessManager(d->manager);

    QWebSettings * settings = page->settings();

    settings->setAttribute(QWebSettings::AutoLoadImages, false);
#else
    QWebEngineSettings * settings = page->settings();

    settings->setAttribute(QWebEngineSettings::AutoLoadImages, false);
#endif

    connect(page, SIGNAL(loadFinished(bool)), this, SLOT(onLoadFinished(bool)));

    d->pages.insert(buffer, page);

#ifdef QT_4
    page->mainFrame()->load(data->url());
#else
    page->load(data->url());
#endif

    return buffer;
}

//-------------------------------------------------------------------------------------------------
// Protected WAbstractLoader reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WLoaderWeb::abort(QIODevice * reply)
{
    Q_D(WLoaderWeb);

    WLoaderWebPage * page = d->pages.take(reply);

    reply->open(QIODevice::ReadOnly);

    complete(reply);

    disconnect(page, 0, this, 0);

    delete page;
}

#ifdef QT_4

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

#endif

#endif // SK_NO_LOADERWEB

#include "WLoaderWeb.moc"
