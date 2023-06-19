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

#ifndef WLOADERNETWORK_H
#define WLOADERNETWORK_H

// Sk includes
#include <WAbstractLoader>

#ifndef SK_NO_LOADERNETWORK

// Forward declarations
class QAbstractNetworkCache;
class QNetworkCookieJar;
class WLoaderNetworkPrivate;

#ifdef QT_6
Q_MOC_INCLUDE("QAbstractNetworkCache")
Q_MOC_INCLUDE("QNetworkCookieJar")
#endif

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

    Q_PRIVATE_SLOT(d_func(), void onMetaDataChanged())

    Q_PRIVATE_SLOT(d_func(), void onFinished(QNetworkReply *))
};

#include <private/WLoaderNetwork_p>

#endif // SK_NO_LOADERNETWORK
#endif // WLOADERNETWORK_H
