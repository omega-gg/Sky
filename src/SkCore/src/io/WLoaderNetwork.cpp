//=================================================================================================
/*
    Copyright (C) 2015-2016 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkCore module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
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

    // FIXME: Fake request to avoid first request freeze.
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

    QUrl url = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();

    if (data->redirect() && url.isValid())
    {
        url = WControllerNetwork::getUrlRedirect(reply->url(), url);

        q->redirect(reply, url);
    }
    else
    {
        QNetworkReply::NetworkError error = reply->error();

        if (error != QNetworkReply::NoError)
        {
            if (error == QNetworkReply::ContentOperationNotPermittedError)
            {
                QString path = data->url().toString();

                if (WControllerNetwork::urlIsFile(path))
                {
                    if (QFileInfo(WControllerFile::filePath(path)).isDir())
                    {
                        q->complete(reply);

                        return;
                    }
                }
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
