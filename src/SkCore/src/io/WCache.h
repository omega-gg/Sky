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

#ifndef WCACHE_H
#define WCACHE_H

// Qt includes
#include <QObject>
#include <QUrl>

// Sk includes
#include <Sk>

#ifndef SK_NO_CACHE

// Forward declarations
class WCache;
class WCachePrivate;

//-------------------------------------------------------------------------------------------------
// WCacheFile
//-------------------------------------------------------------------------------------------------

class SK_CORE_EXPORT WCacheFile : public QObject
{
    Q_OBJECT

private:
    WCacheFile(WCache * cache, QObject * parent);
public:
    /* virtual */ ~WCacheFile();

signals:
    void loaded(WCacheFile * file);

    void progress(qint64 bytesReceived, qint64 bytesTotal);

public: // Properties
    QUrl url     () const;
    QUrl urlCache() const;

    bool isLoading() const;
    bool isLoaded () const;

    int maxHost() const;

    bool hasError() const;

    QString error() const;

private: // Variables
    WCache * _cache;

    QUrl _url;
    QUrl _urlCache;

    bool _loaded;

    int _maxHost;

    QString _error;

private:
    friend class WCache;
    friend class WCachePrivate;
};

//-------------------------------------------------------------------------------------------------
// WCache
//-------------------------------------------------------------------------------------------------

class SK_CORE_EXPORT WCache : public QObject, public WPrivatable
{
    Q_OBJECT

    Q_PROPERTY(bool isEmpty READ isEmpty NOTIFY emptyChanged)

    Q_PROPERTY(QString path READ path CONSTANT)

    Q_PROPERTY(qint64 sizeMax READ sizeMax WRITE setSizeMax NOTIFY sizeMaxChanged)

    Q_PROPERTY(int maxJobs READ maxJobs WRITE setMaxJobs NOTIFY maxJobsChanged)

public:
    WCache(const QString & path, qint64    sizeMax = 1048576 * 10, // 10 Megabytes
                                 QObject * parent  = NULL);

public: // Interface
    Q_INVOKABLE WCacheFile * getFile(const QUrl & url, QObject * parent = NULL, int maxHost = -1);

    Q_INVOKABLE WCacheFile * writeFile(const QUrl       & url,
                                       const QByteArray & array, QObject * parent = NULL);

    Q_INVOKABLE void addFile(const QUrl & url, const QByteArray & array);

    Q_INVOKABLE void removeFiles(const QList<QUrl> & urls);
    Q_INVOKABLE void removeFile (const QUrl        & url);

    Q_INVOKABLE void load(const QList<QUrl> & urls, int maxHost = -1);

    Q_INVOKABLE void abortLoad();

    Q_INVOKABLE void setProxy(const QString & host,
                              int             port, const QString & password = QString());

    Q_INVOKABLE void clearProxy();

    Q_INVOKABLE void clearFiles();
    Q_INVOKABLE void clearAll  ();

protected: // Events
    bool event(QEvent * event);

signals:
    void filesRemoved(const QList<QUrl> & urls, const QList<QUrl> & urlsCache);
    void filesCleared();

    void emptyChanged();

    void sizeMaxChanged();

    void maxJobsChanged();

public: // Properties
    bool isEmpty() const;

    QString path() const;

    qint64 sizeMax() const;
    void   setSizeMax(qint64 max);

    int  maxJobs() const;
    void setMaxJobs(int max);

private:
    W_DECLARE_PRIVATE(WCache)

    Q_PRIVATE_SLOT(d_func(), void onProgress(qint64, qint64))
    Q_PRIVATE_SLOT(d_func(), void onLoaded  (WCacheFile *))

    Q_PRIVATE_SLOT(d_func(), void onPop())

    friend class WCacheFile;
};

#include <private/WCache_p>

#endif // SK_NO_CACHE
#endif // WCACHE_H
