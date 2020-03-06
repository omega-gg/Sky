//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkCore module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#ifndef WLOADERNETWORK_H
#define WLOADERNETWORK_H

// Sk includes
#include <WAbstractLoader>

#ifndef SK_NO_LOADERNETWORK

// Forward declarations
class QAbstractNetworkCache;
class QNetworkCookieJar;
class WLoaderNetworkPrivate;

class SK_CORE_EXPORT WLoaderNetwork : public WAbstractLoader
{
    Q_OBJECT

    Q_PROPERTY(QAbstractNetworkCache * cache READ cache WRITE setCache NOTIFY cacheChanged)

    Q_PROPERTY(QNetworkCookieJar * cookieJar READ cookieJar WRITE setCookieJar
               NOTIFY cookieJarChanged)

public:
    explicit WLoaderNetwork(QObject * parent = NULL);

public: // Interface
    Q_INVOKABLE void setProxy(const QString & host,
                              int             port, const QString & password = QString());

    Q_INVOKABLE void clearProxy();

protected: // WAbstractLoader implementation
    /* virtual */ QIODevice * load(WRemoteData * data);

protected: // WAbstractLoader reimplementation
    /* virtual */ void abort(QIODevice * reply);

signals:
    void cacheChanged();

    void cookieJarChanged();

public: // Properties
    QAbstractNetworkCache * cache() const;
    void                    setCache(QAbstractNetworkCache * cache);

    QNetworkCookieJar * cookieJar() const;
    void                setCookieJar(QNetworkCookieJar * cookieJar);

private:
    W_DECLARE_PRIVATE(WLoaderNetwork)

    Q_PRIVATE_SLOT(d_func(), void onFinished(QNetworkReply *))
};

#include <private/WLoaderNetwork_p>

#endif // SK_NO_LOADERNETWORK
#endif // WLOADERNETWORK_H
