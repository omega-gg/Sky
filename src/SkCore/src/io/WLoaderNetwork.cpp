//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkCore.

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

#include "WLoaderNetwork.h"

#ifndef SK_NO_LOADERNETWORK

// Qt includes
#include <QNetworkProxy>
#include <QSslConfiguration>

// Sk includes
#include <WControllerApplication>
#include <WControllerFile>
#include <WControllerNetwork>
#include <WControllerDownload>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

WLoaderNetworkPrivate::WLoaderNetworkPrivate(WLoaderNetwork * p) : WAbstractLoaderPrivate(p) {}

void WLoaderNetworkPrivate::init()
{
    Q_Q(WLoaderNetwork);

    manager = new QNetworkAccessManager(q);

    // FIXME: Fake request to avoid the first request freeze.
    if (q->parent() == wControllerDownload)
    {
        delete manager->get(QNetworkRequest());
    }

    QObject::connect(manager, SIGNAL(finished(QNetworkReply *)),
                     q,       SLOT(onFinished(QNetworkReply *)));
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WLoaderNetworkPrivate::onFinished(QNetworkReply * reply)
{
    Q_Q(WLoaderNetwork);

    WRemoteData * data = q->getData(reply);

    if (data == NULL) return;

    QString url = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toString();

    if (data->redirect() && url.isEmpty() == false)
    {
        url = WControllerNetwork::getUrlRedirect(reply->url(), url);

        q->redirect(reply, url);
    }
    else
    {
        QNetworkReply::NetworkError error = reply->error();

        if (error != QNetworkReply::NoError)
        {
            if (error == QNetworkReply::ContentOperationNotPermittedError
                &&
                WControllerNetwork::urlIsFile(data->url()))
            {
                q->complete(reply);

                return;
            }

            QString errorString = reply->errorString();

            qWarning("WLoaderNetworkPrivate::onFinished: Get failed %s code %d. %s",
                     reply->url().C_URL, error, errorString.C_STR);

            q->setError(data, errorString);
        }

        q->complete(reply);
    }
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

/* explicit */ WLoaderNetwork::WLoaderNetwork(QObject * parent)
    : WAbstractLoader(new WLoaderNetworkPrivate(this), parent)
{
    Q_D(WLoaderNetwork); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WLoaderNetwork::setProxy(const QString & host,
                                                int             port, const QString & password)
{
    Q_D(WLoaderNetwork);

    QNetworkProxy proxy(QNetworkProxy::HttpProxy, host, port, password);

    d->manager->setProxy(proxy);
}

/* Q_INVOKABLE */ void WLoaderNetwork::clearProxy()
{
    Q_D(WLoaderNetwork);

    d->manager->setProxy(QNetworkProxy());
}

//-------------------------------------------------------------------------------------------------
// Protected WAbstractLoader implementation
//-------------------------------------------------------------------------------------------------

/* virtual */ QIODevice * WLoaderNetwork::load(WRemoteData * data)
{
    Q_D(WLoaderNetwork);

    QNetworkRequest request(data->url());

    QSslConfiguration ssl = request.sslConfiguration();

    ssl.setPeerVerifyMode(QSslSocket::VerifyNone);

    request.setSslConfiguration(ssl);

    if (data->cookies() == false)
    {
        request.setAttribute(QNetworkRequest::CookieLoadControlAttribute,
                             QNetworkRequest::Manual);

        request.setAttribute(QNetworkRequest::CookieSaveControlAttribute,
                             QNetworkRequest::Manual);
    }

    if (data->header())
    {
        request.setRawHeader("User-Agent", "AppleWebKit");
    }

    return d->manager->get(request);
}

//-------------------------------------------------------------------------------------------------
// Protected WAbstractLoader reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WLoaderNetwork::abort(QIODevice * reply)
{
    QNetworkReply * networkReply = qobject_cast<QNetworkReply *> (reply);

    networkReply->abort();
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

QAbstractNetworkCache * WLoaderNetwork::cache() const
{
    Q_D(const WLoaderNetwork); return d->manager->cache();
}

void WLoaderNetwork::setCache(QAbstractNetworkCache * cache)
{
    Q_D(WLoaderNetwork);

    if (d->manager->cache() == cache) return;

    d->manager->setCache(cache);

    emit cacheChanged();
}

//-------------------------------------------------------------------------------------------------

QNetworkCookieJar * WLoaderNetwork::cookieJar() const
{
    Q_D(const WLoaderNetwork); return d->manager->cookieJar();
}

void WLoaderNetwork::setCookieJar(QNetworkCookieJar * cookieJar)
{
    Q_D(WLoaderNetwork);

    if (d->manager->cookieJar() == cookieJar) return;

    d->manager->setCookieJar(cookieJar);

    emit cookieJarChanged();
}

#endif // SK_NO_LOADERNETWORK
