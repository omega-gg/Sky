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

#ifdef QT_OLD
    // FIXME: Fake request to avoid the first request freeze.
    if (q->parent() == wControllerDownload)
    {
        delete manager->get(QNetworkRequest());
    }
#endif

    QObject::connect(manager, SIGNAL(finished(QNetworkReply *)),
                     q,       SLOT(onFinished(QNetworkReply *)));
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

QNetworkReply * WLoaderNetworkPrivate::getReply(const QNetworkRequest & request,
                                                WRemoteData           * data) const
{
    QString method = data->method();
    QString body   = data->body();

    if (method.isEmpty())
    {
        if (body.isEmpty())
        {
            return manager->get(request);
        }
        else return manager->post(request, body.toUtf8());
    }
#ifdef QT_4
    else
    {
        if (body.isEmpty())
        {
            return manager->sendCustomRequest(request, method.toLatin1());
        }

        QBuffer * buffer = new QBuffer;

        buffer->setData(data);

        buffer->open(QIODevice::ReadOnly);

        QNetworkReply * reply = manager->sendCustomRequest(request, method.toLatin1(), buffer);

        buffer->setParent(reply);

        return reply;
    }
#else
    else if (body.isEmpty())
    {
         return manager->sendCustomRequest(request, method.toLatin1());
    }
    else return manager->sendCustomRequest(request, method.toLatin1(), body.toUtf8());
#endif
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WLoaderNetworkPrivate::onMetaDataChanged()
{
    Q_Q(WLoaderNetwork);

    QNetworkReply * reply = static_cast<QNetworkReply *> (q->sender());

    QObject::disconnect(reply, 0, q, 0);

    QString type = reply->header(QNetworkRequest::ContentTypeHeader).toString();

    if (type.toLower().contains("text")) return;

    q->complete(reply);

    reply->abort();
}

void WLoaderNetworkPrivate::onFinished(QNetworkReply * reply)
{
    Q_Q(WLoaderNetwork);

    QObject::disconnect(reply, 0, q, 0);

    WRemoteData * data = q->getData(reply);

    if (data == NULL) return;

    if (data->redirect())
    {
        QString url = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toString();

        if (url.isEmpty() == false)
        {
            url = WControllerNetwork::getUrlRedirect(reply->url(), url);

            q->redirect(reply, url);

            return;
        }
    }

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

#ifdef QT_6
    // NOTE Qt6: We want to handle redirections manually from the onFinished slot.
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                         QNetworkRequest::ManualRedirectPolicy);
#endif

    // NOTE: That's our default header.
    request.setRawHeader("User-Agent", "Mozilla/5.0 AppleWebKit/537 Chrome/90 Safari/537");

    QString header = data->header();

    if (header.isNull() == false)
    {
        // NOTE: When the header is empty we send the request right away.
        if (header.isEmpty())
        {
            return d->manager->get(request);
        }

        QStringList list = Sk::slicesIn(header, "\"", "\"");

        int count = list.count() - 1;

        if (count > 0)
        {
            int index = 0;

            while (index < count)
            {
                request.setRawHeader(list.at(index).C_STR, list.at(index + 1).C_STR);

                index += 2;
            }
        }
    }

    if (data->scope() == ScopeText)
    {
        QNetworkReply * reply = d->getReply(request, data);

        connect(reply, SIGNAL(metaDataChanged()), this, SLOT(onMetaDataChanged()));

        return reply;
    }
    else return d->getReply(request, data);
}

//-------------------------------------------------------------------------------------------------
// Protected WAbstractLoader reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WLoaderNetwork::abort(QIODevice * reply)
{
    QNetworkReply * networkReply = qobject_cast<QNetworkReply *> (reply);

    disconnect(networkReply, 0, this, 0);

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
