//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkCore module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#include "WCache.h"

#ifndef SK_NO_CACHE

// Sk includes
#include <WControllerFile>
#include <WControllerNetwork>
#include <WListId>

// Qt includes
#include <QCoreApplication>
#include <QNetworkReply>
#include <QNetworkProxy>
#include <QThread>
#include <QDir>
#include <QImageReader>

//-------------------------------------------------------------------------------------------------
// Static variables

static const int CACHE_MAX_JOBS     = 20;
static const int CACHE_MAX_REDIRECT = 16;

static const int CACHE_INTERVAL = 1000;

//=================================================================================================
// WCacheJob
//=================================================================================================

struct WCacheJob
{
    int id;

    QString urlBase;
    QString url;

    QString host;

    int redirectCount;

    int maxHost;

    QString error;
};

//=================================================================================================
// WCacheData
//=================================================================================================

struct WCacheData
{
    int id;

    QString url;
    QString urlCache;

    QString extension;

    qint64 size;
};

//=================================================================================================
// WCacheFiles
//=================================================================================================

struct WCacheFiles
{
    QList<WCacheFile *> list;
};

//=================================================================================================
// WCacheThread
//=================================================================================================

class WCacheThread : public QThread
{
    Q_OBJECT

public: // Enums
    enum EventType
    {
        EventGet = QEvent::User,
        EventAdd,
        EventWrite,
        EventAbort,
        EventPop,
        EventRemove,
        EventRemoveFile,
        EventProxy,
        EventClearProxy,
        EventSizeMax,
        EventMaxJobs,
        EventClearDatas,
        EventClearAll
    };

public:
    WCacheThread(WCache * cache, const QString & path, qint64 sizeMax);

    /* virtual */ ~WCacheThread();

protected: // QThread reimplementation
    /* virtual */ void run();

protected: // Events
    /* virtual */ bool event(QEvent * event);

private: // Functions
    void load();
    void save();

    WCacheJob * getJob(const QString & url);

    void processJobs();

    int hostCount(const QString & host);

    bool writeFile(QNetworkReply * reply, WCacheJob * job);

    void writeData(const QString & url, const QString & extension, const QByteArray & array);

    QString getUrlCache(int id, const QString & extension) const;

    void cleanFiles();

    void addData(int id, const QString & url,
                         const QString & urlCache, const QString & extension, qint64 size);

    void clearFolder();

private slots:
    void onFinished(QNetworkReply * reply);

    void onSave();

private: // Variables
    QNetworkAccessManager * manager;

    WCache * cache;

    QString path;
    QString pathIndex;

    qint64 size;
    qint64 sizeMax;

    WListId ids;

    QList<WCacheJob *> jobsPending;

    QHash<QNetworkReply *, WCacheJob *> jobs;

    QList<WCacheData *> datas;

    QHash<QString, WCacheData *> hash;

    QHash<QString, int> toRemove;

    QTimer * timer;

    int maxJobs;
};

//=================================================================================================
// WCacheThreadEventFile
//=================================================================================================

class WCacheThreadEventFile : public QEvent
{
public:
    WCacheThreadEventFile(WCacheThread::EventType type, WCacheFile * file, const QString & url,
                                                                           int             maxHost)
        : QEvent(static_cast<QEvent::Type> (type))
    {
        this->file = file;

        this->url = url;

        this->maxHost = maxHost;
    }

public: // Variables
    WCacheFile * file;

    QString url;

    int maxHost;
};

//=================================================================================================
// WCacheThreadEventWrite
//=================================================================================================

class WCacheThreadEventWrite : public QEvent
{
public:
    WCacheThreadEventWrite(const QString & url,
                           const QString & extension, const QByteArray & array)
        : QEvent(static_cast<QEvent::Type> (WCacheThread::EventWrite))
    {
        this->url       = url;
        this->extension = extension;

        this->array = array;
    }

public: // Variables
    QString url;
    QString extension;

    QByteArray array;
};

//=================================================================================================
// WCacheThreadEventUrls
//=================================================================================================

class WCacheThreadEventUrls : public QEvent
{
public:
    WCacheThreadEventUrls(WCacheThread::EventType type, const QStringList & urls)
        : QEvent(static_cast<QEvent::Type> (type))
    {
        this->urls = urls;
    }

public: // Variables
    QStringList urls;
};

//=================================================================================================
// WCacheThreadEventProxy
//=================================================================================================

class WCacheThreadEventProxy : public QEvent
{
public:
    WCacheThreadEventProxy(const QString & host, int port, const QString & password)
        : QEvent(static_cast<QEvent::Type> (WCacheThread::EventProxy))
    {
        this->host = host;
        this->port = port;

        this->password = password;
    }

public: // Variables
    QString host;
    int     port;

    QString password;
};

//=================================================================================================
// WCacheThreadEvent
//=================================================================================================

class WCacheThreadEvent : public QEvent
{
public:
    WCacheThreadEvent(WCacheThread::EventType type, const QVariant & value)
        : QEvent(static_cast<QEvent::Type> (type))
    {
        this->value = value;
    }

public: // Variables
    QVariant value;
};

//=================================================================================================
// WCacheEventUrls
//=================================================================================================

class WCacheEventUrls : public QEvent
{
public:
    WCacheEventUrls(WCachePrivate::EventType type, const QStringList & urls,
                                                   const QStringList & urlsCache)
        : QEvent(static_cast<QEvent::Type> (type))
    {
        this->urls      = urls;
        this->urlsCache = urlsCache;
    }

public: // Variables
    QStringList urls;
    QStringList urlsCache;
};

//=================================================================================================
// WCacheEventAdded
//=================================================================================================

class WCacheEventAdded : public QEvent
{
public:
    WCacheEventAdded(const QString & url, const QString & urlCache)
        : QEvent(static_cast<QEvent::Type> (WCachePrivate::EventAdded))
    {
        this->url      = url;
        this->urlCache = urlCache;
    }

public: // Variables
    QString url;
    QString urlCache;
};

//=================================================================================================
// WCacheEventFailed
//=================================================================================================

class WCacheEventFailed : public QEvent
{
public:
    WCacheEventFailed(const QString & url, const QString & error)
        : QEvent(static_cast<QEvent::Type> (WCachePrivate::EventFailed))
    {
        this->url   = url;
        this->error = error;
    }

public: // Variables
    QString url;
    QString error;
};

//=================================================================================================
// WCacheFileDelete
//=================================================================================================

class WCacheFileDelete : public WControllerFileAction
{
    Q_OBJECT

protected: // WAbstractThreadAction implementation
    /* virtual */ bool run();

public: // Variables
    QStringList urls;
};

/* virtual */ bool WCacheFileDelete::run()
{
    foreach (const QString & url, urls)
    {
        WControllerFile::deleteFile(url);
    }

    return true;
}

//=================================================================================================
// WCacheThread
//=================================================================================================

WCacheThread::WCacheThread(WCache * cache, const QString & path, qint64 sizeMax) : QThread()
{
    this->cache = cache;

    this->path = path;

    this->sizeMax = sizeMax;

    moveToThread(this);

    start(QThread::IdlePriority);
}

/* virtual */ WCacheThread::~WCacheThread()
{
    foreach (WCacheJob * job, jobsPending)
    {
        delete job;
    }

    QHashIterator<QNetworkReply *, WCacheJob *> i(jobs);

    while (i.hasNext())
    {
        i.next();

        delete i.key  ();
        delete i.value();
    }

    foreach (WCacheData * data, datas)
    {
        delete data;
    }
}

//-------------------------------------------------------------------------------------------------
// Protected QThread reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WCacheThread::run()
{
    pathIndex = path + "/index";

    size = 0;

    maxJobs = CACHE_MAX_JOBS;

    manager = new QNetworkAccessManager(this);

    timer = new QTimer(this);

    timer->setInterval(CACHE_INTERVAL);

    timer->setSingleShot(true);

    connect(manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(onFinished(QNetworkReply *)));

    connect(timer, SIGNAL(timeout()), this, SLOT(onSave()));

    load();

    exec();
}

//-------------------------------------------------------------------------------------------------
// Events
//-------------------------------------------------------------------------------------------------

/* virtual */ bool WCacheThread::event(QEvent * event)
{
    QEvent::Type type = event->type();

    if (type == static_cast<QEvent::Type> (WCacheThread::EventGet))
    {
        WCacheThreadEventFile * eventFile = static_cast<WCacheThreadEventFile *> (event);

        WCacheJob * job = new WCacheJob;

        job->id = ids.generateId();

        QString url = eventFile->url;

        job->urlBase = url;
        job->url     = url;

        job->host = QUrl(url).host();

        job->redirectCount = 0;

        job->maxHost = eventFile->maxHost;

        jobsPending.append(job);

        processJobs();

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WCacheThread::EventAdd))
    {
        WCacheThreadEventFile * eventFile = static_cast<WCacheThreadEventFile *> (event);

        WCacheJob * job = getJob(eventFile->url);

        if (job)
        {
            int max = eventFile->maxHost;

            int maxHost = job->maxHost;

            if (max != -1 && (maxHost == -1 || maxHost > max))
            {
                job->maxHost = max;
            }
        }

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WCacheThread::EventWrite))
    {
        WCacheThreadEventWrite * eventWrite = static_cast<WCacheThreadEventWrite *> (event);

        writeData(eventWrite->url, eventWrite->extension, eventWrite->array);

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WCacheThread::EventAbort))
    {
        WCacheThreadEvent * eventCache = static_cast<WCacheThreadEvent *> (event);

        QString url = eventCache->value.toString();

        foreach (WCacheJob * job, jobsPending)
        {
            if (job->url == url)
            {
                jobsPending.removeOne(job);

                ids.removeOne(job->id);

                delete job;

                return true;
            }
        }

        QHashIterator<QNetworkReply *, WCacheJob *> i(jobs);

        while (i.hasNext())
        {
            i.next();

            WCacheJob * job = i.value();

            if (job->url == url)
            {
                QNetworkReply * reply = i.key();

                jobs.remove(reply);

                ids.removeOne(job->id);

                delete reply;
                delete job;

                return true;
            }
        }

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WCacheThread::EventPop))
    {
        WCacheThreadEventUrls * eventUrls = static_cast<WCacheThreadEventUrls *> (event);

        foreach (const QString & url, eventUrls->urls)
        {
            WCacheData * data = hash.value(url);

            if (data == NULL) continue;

            datas.removeOne(data);
            datas.append   (data);
        }

        save();

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WCacheThread::EventRemove))
    {
        WCacheThreadEventUrls * eventUrls = static_cast<WCacheThreadEventUrls *> (event);

        QStringList urls;
        QStringList urlsCache;

        foreach (const QString & url, eventUrls->urls)
        {
            WCacheData * data = hash.value(url);

            if (data == NULL) continue;

            datas.removeOne(data);

            const QString & dataUrl      = data->url;
            const QString & dataUrlCache = data->urlCache;

            hash.remove(dataUrl);

            toRemove.insert(dataUrlCache, data->id);

            urls     .append(dataUrl);
            urlsCache.append(dataUrlCache);

            delete data;
        }

        QCoreApplication::postEvent(cache, new WCacheEventUrls(WCachePrivate::EventRemoved,
                                                               urls, urlsCache));

        save();

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WCacheThread::EventRemoveFile))
    {
        WCacheThreadEventUrls * eventUrls = static_cast<WCacheThreadEventUrls *> (event);

        foreach (const QString & url, eventUrls->urls)
        {
            int id = toRemove.take(url);

            if (id) ids.removeOne(id);
        }

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WCacheThread::EventProxy))
    {
        WCacheThreadEventProxy * eventProxy = static_cast<WCacheThreadEventProxy *> (event);

        QNetworkProxy proxy(QNetworkProxy::HttpProxy,
                            eventProxy->host, eventProxy->port, eventProxy->password);

        manager->setProxy(proxy);

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WCacheThread::EventClearProxy))
    {
        manager->setProxy(QNetworkProxy());

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WCacheThread::EventSizeMax))
    {
        WCacheThreadEvent * eventCache = static_cast<WCacheThreadEvent *> (event);

        sizeMax = eventCache->value.toLongLong();

        cleanFiles();

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WCacheThread::EventMaxJobs))
    {
        WCacheThreadEvent * eventCache = static_cast<WCacheThreadEvent *> (event);

        maxJobs = eventCache->value.toInt();

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WCacheThread::EventClearDatas))
    {
        timer->stop();

        foreach (WCacheData * data, datas)
        {
            ids.removeOne(data->id);

            delete data;
        }

        datas.clear();
        hash .clear();

        clearFolder();

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WCacheThread::EventClearAll))
    {
        timer->stop();

        foreach (WCacheJob * job, jobsPending)
        {
            delete job;
        }

        jobsPending.clear();

        QHashIterator<QNetworkReply *, WCacheJob *> i(jobs);

        while (i.hasNext())
        {
            i.next();

            delete i.key  ();
            delete i.value();
        }

        jobs.clear();

        foreach (WCacheData * data, datas)
        {
            delete data;
        }

        datas.clear();
        hash .clear();

        ids.clear();

        clearFolder();

        return true;
    }
    else return QThread::event(event);
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WCacheThread::load()
{
    QFile file(pathIndex);

    if (file.exists() == false)
    {
        QCoreApplication::postEvent(cache, new WCacheEventUrls(WCachePrivate::EventLoaded,
                                                               QStringList(), QStringList()));

        return;
    }

    if (file.open(QIODevice::ReadOnly) == false)
    {
        qWarning("WCacheThread::load: Failed to open index %s.", pathIndex.C_STR);

        QCoreApplication::postEvent(cache, new WCacheEventUrls(WCachePrivate::EventLoaded,
                                                               QStringList(), QStringList()));

        return;
    }

    QStringList urls;
    QStringList urlsCache;

    QDataStream stream(&file);

    int count;

    stream >> count;

    QString pathFile = path + '/';

    while (count)
    {
        int id;

        QString url;

        QString extension;

        qint64 size;

        stream >> id >> url >> extension >> size;

        QString urlCache;

        if (extension.isEmpty())
        {
             urlCache = pathFile + QString::number(id);
        }
        else urlCache = pathFile + QString::number(id) + '.' + extension;

        addData(id, url, urlCache, extension, size);

        ids.insertId(id);

        urls     .append(url);
        urlsCache.append(urlCache);

        this->size += size;

        count--;
    }

    file.close();

    QCoreApplication::postEvent(cache,
                                new WCacheEventUrls(WCachePrivate::EventLoaded, urls, urlsCache));

    cleanFiles();
}

void WCacheThread::save()
{
    if (timer->isActive()) return;

    timer->start();
}

//-------------------------------------------------------------------------------------------------

WCacheJob * WCacheThread::getJob(const QString & url)
{
    foreach (WCacheJob * job, jobsPending)
    {
        if (job->url == url) return job;
    }

    QHashIterator<QNetworkReply *, WCacheJob *> i(jobs);

    while (i.hasNext())
    {
        i.next();

        WCacheJob * job = i.value();

        if (job->url == url) return job;
    }

    return NULL;
}

//-------------------------------------------------------------------------------------------------

void WCacheThread::processJobs()
{
    if (jobs.count() == maxJobs) return;

    foreach (WCacheJob * job, jobsPending)
    {
        const QString & url = job->url;

        int maxHost = job->maxHost;

        if (maxHost == -1 || hostCount(job->host) < maxHost)
        {
            QNetworkReply * reply = manager->get(QNetworkRequest(url));

            connect(reply, SIGNAL(downloadProgress(qint64, qint64)),
                    cache, SLOT(onProgress        (qint64, qint64)));

            jobsPending.removeOne(job);

            jobs.insert(reply, job);

            if (jobs.count() == maxJobs) return;
        }
    }
}

int WCacheThread::hostCount(const QString & host)
{
    int count = 0;

    foreach (WCacheJob * job, jobs)
    {
        if (job->host == host) count++;
    }

    return count;
}

//-------------------------------------------------------------------------------------------------

bool WCacheThread::writeFile(QNetworkReply * reply, WCacheJob * job)
{
    const QString & url = job->urlBase;

    if (reply->error() != QNetworkReply::NoError)
    {
        QString error = reply->errorString();

        qWarning("WCacheThread::writeFile: Get failed %s code %d. %s", url.C_STR, reply->error(),
                 error.C_STR);

        QCoreApplication::postEvent(cache, new WCacheEventFailed(url, error));

        return false;
    }

    qint64 size = reply->size();

    if (size >= sizeMax)
    {
        qWarning("WCacheThread::writeFile: File is too large %s.", url.C_STR);

        QCoreApplication::postEvent(cache, new WCacheEventFailed(url, "File is too large"));

        return false;
    }

    this->size += size;

    cleanFiles();

    int id = job->id;

    QString extension = WControllerNetwork::extractUrlExtension(url);

    QString urlCache = getUrlCache(id, extension);

    QFile file(urlCache);

    if (file.open(QIODevice::WriteOnly) == false)
    {
        qWarning("WCacheThread::writeFile: Cannot open file %s.", urlCache.C_STR);

        QCoreApplication::postEvent(cache, new WCacheEventFailed(url, "Cannot open file"));

        return false;
    }

    file.write(reply->readAll());

    file.close();

    addData(id, url, urlCache, extension, size);

    toRemove.remove(urlCache);

    QCoreApplication::postEvent(cache, new WCacheEventAdded(url, urlCache));

    save();

    return true;
}

//-------------------------------------------------------------------------------------------------

void WCacheThread::writeData(const QString & url,
                             const QString & extension, const QByteArray & array)
{
    qint64 size = array.size() * sizeof(char);

    if (size >= sizeMax)
    {
        qWarning("WCacheThread::writeData: Data is too large %s.", url.C_STR);

        QCoreApplication::postEvent(cache, new WCacheEventFailed(url, "Data is too large"));

        return;
    }

    this->size += size;

    cleanFiles();

    int id = ids.generateId();

    QString urlExtension;

    if (extension.isEmpty())
    {
         urlExtension = WControllerNetwork::extractUrlExtension(url);
    }
    else urlExtension = extension;

    QString urlCache = getUrlCache(id, urlExtension);

    QFile file(urlCache);

    if (file.open(QIODevice::WriteOnly) == false)
    {
        qWarning("WCacheThread::writeData: Cannot open file %s.", urlCache.C_STR);

        ids.removeOne(id);

        QCoreApplication::postEvent(cache, new WCacheEventFailed(url, "Cannot open file"));

        return;
    }

    file.write(array);

    file.close();

    addData(id, url, urlCache, urlExtension, size);

    toRemove.remove(urlCache);

    QCoreApplication::postEvent(cache, new WCacheEventAdded(url, urlCache));

    save();
}

//-------------------------------------------------------------------------------------------------

QString WCacheThread::getUrlCache(int id, const QString & extension) const
{
    if (extension.isEmpty())
    {
         return path + '/' + QString::number(id);
    }
    else return path + '/' + QString::number(id) + '.' + extension;
}

//-------------------------------------------------------------------------------------------------

void WCacheThread::cleanFiles()
{
    QStringList urls;
    QStringList urlsCache;

    while (datas.count() && size >= sizeMax)
    {
        WCacheData * data = datas.takeFirst();

        const QString & url      = data->url;
        const QString & urlCache = data->urlCache;

        hash.remove(url);

        toRemove.insert(urlCache, data->id);

        urls     .append(url);
        urlsCache.append(urlCache);

        size -= data->size;

        delete data;
    }

    if (urls.count())
    {
        QCoreApplication::postEvent(cache, new WCacheEventUrls(WCachePrivate::EventRemoved,
                                                               urls, urlsCache));

        save();
    }
}

//-------------------------------------------------------------------------------------------------

void WCacheThread::addData(int id,
                           const QString & url,
                           const QString & urlCache, const QString & extension, qint64 size)
{
    WCacheData * data = new WCacheData;

    data->id = id;

    data->url      = url;
    data->urlCache = urlCache;

    data->extension = extension;

    data->size = size;

    datas.append(data);

    hash.insert(url, data);
}

//-------------------------------------------------------------------------------------------------

void WCacheThread::clearFolder()
{
    WControllerFile::deleteFolderContent(path, false);

    size = 0;

    QCoreApplication::postEvent(cache, new QEvent(static_cast<QEvent::Type>
                                                  (WCachePrivate::EventCleared)));
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WCacheThread::onFinished(QNetworkReply * reply)
{
    WCacheJob * job = jobs.take(reply);

    if (job == NULL)
    {
        processJobs();

        return;
    }

    QString url = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toString();

    if (url.isEmpty())
    {
        if (writeFile(reply, job) == false)
        {
            ids.removeOne(job->id);
        }

        delete job;
    }
    else
    {
        if (job->redirectCount < CACHE_MAX_REDIRECT)
        {
            job->url = WControllerNetwork::getUrlRedirect(reply->url(), url);

            job->redirectCount++;

            jobsPending.prepend(job);
        }
        else
        {
            ids.removeOne(job->id);

            delete job;
        }
    }

    processJobs();

    reply->deleteLater();
}

//-------------------------------------------------------------------------------------------------

void WCacheThread::onSave()
{
    QFile file(pathIndex);

    if (file.open(QIODevice::WriteOnly) == false)
    {
        qWarning("WCacheThread::onSave: Failed to open index %s.", pathIndex.C_STR);

        return;
    }

    QDataStream stream(&file);

    stream << datas.count();

    foreach (WCacheData * data, datas)
    {
        stream << data->id << data->url << data->extension << (qint64) data->size;
    }
}

//=================================================================================================
// WCacheFile
//=================================================================================================
// Private

WCacheFile::WCacheFile(WCache * cache, QObject * parent) : QObject(parent)
{
    _cache = cache;
}

//-------------------------------------------------------------------------------------------------
// Public

/* virtual */ WCacheFile::~WCacheFile()
{
    _cache->d_func()->clearFile(this);
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

QString WCacheFile::url() const
{
    return _url;
}

QString WCacheFile::urlCache() const
{
    return _urlCache;
}

//-------------------------------------------------------------------------------------------------

QString WCacheFile::extension() const
{
    return _extension;
}

//-------------------------------------------------------------------------------------------------

bool WCacheFile::isLoading() const
{
    return (_loaded == false);
}

bool WCacheFile::isLoaded() const
{
    return _loaded;
}

//-------------------------------------------------------------------------------------------------

int WCacheFile::maxHost() const
{
    return _maxHost;
}

//-------------------------------------------------------------------------------------------------

bool WCacheFile::hasError() const
{
    return (_error.isEmpty() == false);
}

QString WCacheFile::error() const
{
    return _error;
}

//=================================================================================================
// WCachePrivate
//=================================================================================================

WCachePrivate::WCachePrivate(WCache * p) : WPrivate(p) {}

/* virtual */ WCachePrivate::~WCachePrivate()
{
    thread->quit();
    thread->wait();

    delete thread;
}

//-------------------------------------------------------------------------------------------------

void WCachePrivate::init(const QString & path, qint64 sizeMax)
{
    Q_Q(WCache);

    this->path = path;

    this->sizeMax = sizeMax;

    loaded = false;
    empty  = true;

    maxJobs = CACHE_MAX_JOBS;

    if (QFile::exists(path) == false)
    {
        QDir().mkpath(path);
    }

    timer.setInterval(200);

    timer.setSingleShot(true);

    QObject::connect(&timer, SIGNAL(timeout()), q, SLOT(onPop()));

    thread = new WCacheThread(q, path, sizeMax);
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WCachePrivate::get(WCacheFile * file, const QString & url)
{
    WCacheFiles * files = urlsPending.value(url);

    if (files)
    {
        files->list.append(file);

        if (loaded)
        {
            QCoreApplication::postEvent(thread,
                                        new WCacheThreadEventFile(WCacheThread::EventAdd,
                                                                  file, url, file->_maxHost));
        }
    }
    else
    {
        files = new WCacheFiles;

        files->list.append(file);

        urlsPending.insert(url, files);

        if (loaded)
        {
            QCoreApplication::postEvent(thread,
                                        new WCacheThreadEventFile(WCacheThread::EventGet,
                                                                  file, url, file->_maxHost));
        }
    }
}

void WCachePrivate::write(WCacheFile * file, const QString & url,
                                             const QString & extension, const QByteArray & array)
{
    WCacheFiles * files = urlsPending.value(url);

    if (files)
    {
        files->list.append(file);

        return;
    }

    files = new WCacheFiles;

    files->list.append(file);

    urlsPending.insert(url, files);

    if (loaded)
    {
        QCoreApplication::postEvent(thread, new WCacheThreadEventWrite(url, extension, array));
    }
}

//-------------------------------------------------------------------------------------------------

void WCachePrivate::pop(const QString & url)
{
    if (loaded == false) return;

    urlsPop.removeOne(url);
    urlsPop.append   (url);

    if (timer.isActive() == false)
    {
        timer.start();
    }
}

//-------------------------------------------------------------------------------------------------

void WCachePrivate::clearFile(WCacheFile * file)
{
    if (file->_loaded) return;

    const QString & url = file->_url;

    WCacheFiles * files = urlsPending.value(url);

    if (files == NULL) return;

    QList<WCacheFile *> * list = &(files->list);

    list->removeOne(file);

    if (list->isEmpty())
    {
        urlsPending.remove(url);

        urlsPop.removeOne(url);

        QCoreApplication::postEvent(thread, new WCacheThreadEvent(WCacheThread::EventAbort, url));

        delete files;
    }
}

//-------------------------------------------------------------------------------------------------

void WCachePrivate::setEmpty(bool empty)
{
    Q_Q(WCache);

    this->empty = empty;

    q->emptyChanged();
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WCachePrivate::onProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    Q_Q(WCache);

    QNetworkReply * reply = static_cast<QNetworkReply *> (q->sender());

    if (reply == NULL) return;

    WCacheFiles * files = urlsPending.value(reply->url().toString());

    if (files == NULL) return;

    foreach (WCacheFile * file, files->list)
    {
        emit file->progress(bytesReceived, bytesTotal);
    }
}

void WCachePrivate::onLoaded(WCacheFile * file)
{
    urlsLoad.remove(file->_url);

    delete file;
}

//-------------------------------------------------------------------------------------------------

void WCachePrivate::onPop()
{
    QCoreApplication::postEvent(thread,
                                new WCacheThreadEventUrls(WCacheThread::EventPop, urlsPop));

    urlsPop.clear();
}

//=================================================================================================
// WCache
//=================================================================================================

WCache::WCache(const QString & path, qint64 sizeMax, QObject * parent)
    : QObject(parent), WPrivatable(new WCachePrivate(this))
{
    Q_D(WCache); d->init(path, sizeMax);
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ WCacheFile * WCache::getFile(const QString & url, QObject * parent, int maxHost)
{
    if (url.isEmpty()) return NULL;

    Q_D(WCache);

    WCacheFile * file;

    if (parent) file = new WCacheFile(this, parent);
    else        file = new WCacheFile(this, this);

    file->_url = url;

    file->_maxHost = maxHost;

    QString path = d->urls.value(url);

    if (path.isEmpty())
    {
        file->_loaded = false;

        d->get(file, url);
    }
    else
    {
        file->_urlCache = path;

        file->_loaded = true;

        d->pop(url);
    }

    return file;
}

/* Q_INVOKABLE */ QString WCache::getFileUrl(const QString & url)
{
    Q_D(WCache);

    QString path = d->urls.value(url);

    if (path.isEmpty() == false)
    {
        d->pop(url);
    }

    return path;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ WCacheFile * WCache::writeFile(const QString    & url,
                                                 const QByteArray & array,
                                                 const QString    & extension, QObject * parent)
{
    if (url.isEmpty()) return NULL;

    Q_D(WCache);

    WCacheFile * file;

    if (parent) file = new WCacheFile(this, parent);
    else        file = new WCacheFile(this, this);

    file->_url = url;

    file->_extension = extension;

    file->_maxHost = -1;

    QString path = d->urls.value(url);

    if (path.isEmpty())
    {
        file->_loaded = false;

        d->write(file, url, extension, array);
    }
    else
    {
        file->_urlCache = path;

        file->_loaded = true;

        d->pop(url);
    }

    return file;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WCache::addFile(const QString & url, const QByteArray & array,
                                                            const QString    & extension)
{
    WCacheFile * file = writeFile(url, array, extension, NULL);

    if (file) delete file;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WCache::removeFiles(const QStringList & urls)
{
    Q_D(WCache);

    QStringList list;

    foreach (const QString & url, urls)
    {
        if (d->urls.contains(url))
        {
            d->urls.remove(url);

            list.append(url);
        }
    }

    if (list.isEmpty() == false)
    {
        QCoreApplication::postEvent(d->thread,
                                    new WCacheThreadEventUrls(WCacheThread::EventRemove, urls));
    }
}

/* Q_INVOKABLE */ void WCache::removeFile(const QString & url)
{
    Q_D(WCache);

    if (d->urls.contains(url) == false) return;

    d->urls.remove(url);

    QCoreApplication::postEvent(d->thread, new WCacheThreadEventUrls(WCacheThread::EventRemove,
                                                                     QStringList() << url));
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WCache::load(const QStringList & urls, int maxHost)
{
    if (urls.isEmpty()) return;

    Q_D(WCache);

    QHash<QString, WCacheFile *> hash;

    foreach (const QString & url, urls)
    {
        if (url.isEmpty()) continue;

        if (d->urls.contains(url) == false)
        {
            WCacheFile * file = d->urlsLoad.take(url);

            if (file == NULL)
            {
                file = new WCacheFile(this, this);

                file->_url = url;

                file->_maxHost = maxHost;

                file->_loaded = false;

                d->get(file, url);

                connect(file, SIGNAL(loaded(WCacheFile *)), this, SLOT(onLoaded(WCacheFile *)));
            }

            hash.insert(url, file);
        }
        else d->pop(url);
    }

    QHashIterator<QString, WCacheFile *> i(d->urlsLoad);

    while (i.hasNext())
    {
        i.next();

        delete i.value();
    }

    d->urlsLoad = hash;
}

/* Q_INVOKABLE */ void WCache::abortLoad()
{
    Q_D(WCache);

    QHashIterator<QString, WCacheFile *> i(d->urlsLoad);

    while (i.hasNext())
    {
        i.next();

        delete i.value();
    }

    d->urlsLoad.clear();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WCache::setProxy(const QString & host, int port, const QString & password)
{
    Q_D(WCache);

    QCoreApplication::postEvent(d->thread, new WCacheThreadEventProxy(host, port, password));
}

/* Q_INVOKABLE */ void WCache::clearProxy()
{
    Q_D(WCache);

    QCoreApplication::postEvent(d->thread, new QEvent(static_cast<QEvent::Type>
                                                      (WCacheThread::EventClearProxy)));
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WCache::clearFiles()
{
    Q_D(WCache);

    d->urls   .clear();
    d->urlsPop.clear();

    QCoreApplication::postEvent(d->thread, new QEvent(static_cast<QEvent::Type>
                                                      (WCacheThread::EventClearDatas)));

    if (d->empty == false)
    {
        d->setEmpty(true);
    }
}

/* Q_INVOKABLE */ void WCache::clearAll()
{
    Q_D(WCache);

    abortLoad();

    QHash<QString, WCacheFiles *> hash = d->urlsPending;

    d->urlsPending.clear();

    QHashIterator<QString, WCacheFiles *> i(hash);

    while (i.hasNext())
    {
        i.next();

        WCacheFiles * files = i.value();

        foreach (WCacheFile * file, files->list)
        {
            file->_error = "Files cleared";

            file->_loaded = true;

            emit file->loaded(file);
        }

        delete files;
    }

    d->urls   .clear();
    d->urlsPop.clear();

    QCoreApplication::postEvent(d->thread, new QEvent(static_cast<QEvent::Type>
                                                      (WCacheThread::EventClearAll)));

    if (d->empty == false)
    {
        d->setEmpty(true);
    }
}

//-------------------------------------------------------------------------------------------------
// Events
//-------------------------------------------------------------------------------------------------

/* virtual */ bool WCache::event(QEvent * event)
{
    QEvent::Type type = event->type();

    if (type == static_cast<QEvent::Type> (WCachePrivate::EventLoaded))
    {
        Q_D(WCache);

        WCacheEventUrls * eventUrls = static_cast<WCacheEventUrls *> (event);

        const QStringList & urls = eventUrls->urls;

        QList<WCacheFile *> list;

        if (urls.isEmpty() == false)
        {
            int i = 0;

            foreach (const QString & url, urls)
            {
                QString urlCache = eventUrls->urlsCache.at(i);

                WCacheFiles * files = d->urlsPending.take(url);

                if (files)
                {
                    foreach (WCacheFile * file, files->list)
                    {
                        file->_urlCache = urlCache;

                        file->_loaded = true;

                        list.append(file);
                    }

                    delete files;
                }

                d->urls.insert(url, urlCache);

                i++;
            }

            if (d->empty)
            {
                d->setEmpty(false);
            }
        }

        d->loaded = true;

        QHashIterator<QString, WCacheFiles *> i(d->urlsPending);

        while (i.hasNext())
        {
            i.next();

            const QList<WCacheFile *> & list = i.value()->list;

            if (list.isEmpty() == false)
            {
                const QString & url = i.key();

                WCacheFile * file = list.first();

                QCoreApplication
                ::postEvent(d->thread, new WCacheThreadEventFile(WCacheThread::EventGet,
                                                                 file, url, file->_maxHost));

                for (int i = 1; i < list.count(); i++)
                {
                    file = list.at(i);

                    QCoreApplication
                    ::postEvent(d->thread, new WCacheThreadEventFile(WCacheThread::EventAdd,
                                                                     file, url, file->_maxHost));
                }
            }
        }

        foreach (WCacheFile * file, list)
        {
            emit file->loaded(file);
        }

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WCachePrivate::EventAdded))
    {
        Q_D(WCache);

        WCacheEventAdded * eventAdded = static_cast<WCacheEventAdded *> (event);

        const QString & url      = eventAdded->url;
        const QString & urlCache = eventAdded->urlCache;

        d->urls.insert(url, urlCache);

        if (d->empty)
        {
            d->setEmpty(false);
        }

        WCacheFiles * files = d->urlsPending.take(url);

        if (files == NULL) return true;

        foreach (WCacheFile * file, files->list)
        {
            file->_urlCache = urlCache;

            file->_loaded = true;

            emit file->loaded(file);
        }

        delete files;

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WCachePrivate::EventFailed))
    {
        Q_D(WCache);

        WCacheEventFailed * eventFailed = static_cast<WCacheEventFailed *> (event);

        const QString & url = eventFailed->url;

        WCacheFiles * files = d->urlsPending.take(url);

        if (files == NULL) return true;

        foreach (WCacheFile * file, files->list)
        {
            file->_error = eventFailed->error;

            file->_loaded = true;

            emit file->loaded(file);
        }

        delete files;

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WCachePrivate::EventRemoved))
    {
        Q_D(WCache);

        WCacheEventUrls * eventUrls = static_cast<WCacheEventUrls *> (event);

        const QStringList & urls      = eventUrls->urls;
        const QStringList & urlsCache = eventUrls->urlsCache;

        int i = 0;

        foreach (const QString & url, urls)
        {
            d->urls.remove(url);

            d->urlsPop.removeOne(url);

            i++;
        }

        if (d->urls.isEmpty())
        {
            d->setEmpty(true);
        }

        emit filesRemoved(urls, urlsCache);

        QCoreApplication::postEvent(d->thread,
                                    new WCacheThreadEventUrls(WCacheThread::EventRemoveFile,
                                                              urlsCache));

        WCacheFileDelete * action = new WCacheFileDelete;

        action->urls = urlsCache;

        wControllerFile->startReadAction(action);

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WCachePrivate::EventCleared))
    {
        emit filesCleared();

        return true;
    }
    else return QObject::event(event);
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

bool WCache::isEmpty() const
{
    Q_D(const WCache); return d->empty;
}

//-------------------------------------------------------------------------------------------------

QString WCache::path() const
{
    Q_D(const WCache); return d->path;
}

//-------------------------------------------------------------------------------------------------

qint64 WCache::sizeMax() const
{
    Q_D(const WCache); return d->sizeMax;
}

void WCache::setSizeMax(qint64 max)
{
    Q_D(WCache);

    if (d->sizeMax == max) return;

    d->sizeMax = max;

    QCoreApplication::postEvent(d->thread, new WCacheThreadEvent(WCacheThread::EventSizeMax, max));

    emit sizeMaxChanged();
}

//-------------------------------------------------------------------------------------------------

int WCache::maxJobs() const
{
    Q_D(const WCache); return d->maxJobs;
}

void WCache::setMaxJobs(int max)
{
    Q_D(WCache);

    if (d->maxJobs == max) return;

    d->maxJobs = max;

    QCoreApplication::postEvent(d->thread, new WCacheThreadEvent(WCacheThread::EventMaxJobs, max));

    emit maxJobsChanged();
}

#endif // SK_NO_CACHE

#include "WCache.moc"
