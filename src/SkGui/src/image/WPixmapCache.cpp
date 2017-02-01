//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkGui module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#include "WPixmapCache.h"

#ifndef SK_NO_PIXMAPCACHE

// Qt includes
#include <QImageReader>
#include <QPainter>
#include <QPixmap>

// Sk includes
#include <WControllerFile>
#include <WAbstractThreadAction>
#include <WCache>

// Forward declarations
class WPixmapCacheReply;

//-------------------------------------------------------------------------------------------------
// Static variables

static const int PIXMAPCACHE_MAX = 1048576 * 10; // 10 Megabytes

//-------------------------------------------------------------------------------------------------

inline qint64 getImageSize(const QImage & image)
{
    return (image.width() * image.height() * image.depth()) / 8;
}

inline qint64 getPixmapSize(const QPixmap & pixmap)
{
    return (pixmap.width() * pixmap.height() * pixmap.depth()) / 8;
}

//=================================================================================================
// WPixmapCacheKey
//=================================================================================================

struct WPixmapCacheKey
{
    const QString * path;

    const QSize * size;
    const QSize * area;
};

//-------------------------------------------------------------------------------------------------

inline bool operator==(const WPixmapCacheKey & keyA, const WPixmapCacheKey & keyB)
{
    return (*(keyA.size) == *(keyB.size) && *(keyA.area) == *(keyB.area)
            &&
            *(keyA.path) == *(keyB.path));
}

inline uint qHash(const WPixmapCacheKey & key)
{
    return qHash(*(key.path)) ^ key.size->width() ^ key.size->height()
                              ^ key.area->width() ^ key.area->height();
}

//=================================================================================================
// WPixmapCacheImageRead and WPixmapCacheImageReply
//=================================================================================================

class WPixmapCacheImageRead : public WAbstractThreadAction
{
    Q_OBJECT

public:
    WPixmapCacheImageRead(const QString & path, const QSize & size, const QSize & area)
    {
        this->path = path;

        this->size = size;
        this->area = area;
    }

protected: // WAbstractThreadAction reimplementation
    /* virtual */ WAbstractThreadReply * createReply() const;

protected: // WAbstractThreadAction implementation
    /* virtual */ bool run();

public: // Variables
    QString path;

    QSize size;
    QSize area;
};

//-------------------------------------------------------------------------------------------------

class WPixmapCacheImageReply : public WAbstractThreadReply
{
    Q_OBJECT

protected: // WAbstractThreadReply reimplementation
    /* virtual */ void onCompleted(bool ok);

signals:
    void loaded(const QImage & image);

public: // Variables
    QImage image;
};

//-------------------------------------------------------------------------------------------------

/* virtual */ WAbstractThreadReply * WPixmapCacheImageRead::createReply() const
{
    return new WPixmapCacheImageReply;
}

/* virtual */ bool WPixmapCacheImageRead::run()
{
    WPixmapCacheImageReply * reply = qobject_cast<WPixmapCacheImageReply *> (this->reply());

    if (WPixmapCache::readImage(&(reply->image), path, size, area) == false)
    {
        qWarning("WPixmapCacheImageRead::run: Failed to read file %s.", path.C_STR);

        return false;
    }
    else return true;
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WPixmapCacheImageReply::onCompleted(bool)
{
    emit loaded(image);
}

//=================================================================================================
// WPixmapCacheRead and WPixmapCacheReply
//=================================================================================================

class  WPixmapCachePrivate;
struct WPixmapCacheData;

class WPixmapCacheRead : public WAbstractThreadAction
{
    Q_OBJECT

public:
    WPixmapCacheRead(WPixmapCachePrivate * pixmap, WPixmapCacheData * data, const QString & path,
                                                                            const QSize   & size,
                                                                            const QSize   & area)
    {
        this->pixmap = pixmap;
        this->data   = data;

        this->path = path;

        this->size = size;
        this->area = area;
    }

protected: // WAbstractThreadAction reimplementation
    /* virtual */ WAbstractThreadReply * createReply() const;

protected: // WAbstractThreadAction implementation
    /* virtual */ bool run();

public: // Variables
    WPixmapCachePrivate * pixmap;
    WPixmapCacheData    * data;

    QString path;

    QSize size;
    QSize area;
};

//-------------------------------------------------------------------------------------------------

class WPixmapCacheReply : public WAbstractThreadReply
{
    Q_OBJECT

public:
    WPixmapCacheReply(WPixmapCachePrivate * pixmap, WPixmapCacheData * data)
    {
        this->pixmap = pixmap;
        this->data   = data;
    }

protected: // WAbstractThreadReply reimplementation
    /* virtual */ void onCompleted(bool ok);

signals:
    void loaded();

public: // Variables
    WPixmapCachePrivate * pixmap;
    WPixmapCacheData    * data;

    QImage image;

private:
    friend class WPixmapCache;
};

//=================================================================================================
// WPixmapCacheData
//=================================================================================================

class WPixmapCacheData
{
public:
    ~WPixmapCacheData()
    {
        if (action) action->abortAndDelete();
    }

public: // Variables
    QString path;

    QSize size;
    QSize area;

    QPixmap pixmap;
    qint64  pixmapSize;

    QList<WPixmapCache *> pixmaps;

    WPixmapCacheRead  * action;
    WPixmapCacheReply * reply;

    bool toDelete;
};

//=================================================================================================
// WPixmapCacheDataNull
//=================================================================================================

struct WPixmapCacheDataNull
{
    QString path;
    QSize   size;

    QPixmap pixmap;
};

Q_GLOBAL_STATIC(WPixmapCacheDataNull, dataNull);

//=================================================================================================
// WPixmapCacheStore
//=================================================================================================

class WPixmapCacheStore : public QObject
{
    Q_OBJECT

public:
    WPixmapCacheStore();

    /* virtual */ ~WPixmapCacheStore();

public: // Interface
    bool addSize(qint64 sizeFile, const QString & path);

    void removeData(WPixmapCacheData * data);
    void deleteData(WPixmapCacheData * data);

    void cleanPixmaps();
    void clearPixmaps();

public slots:
    void updateCache();

    void onFilesRemoved(const QList<QUrl> & paths, const QList<QUrl> & pathsCache);
    void onFilesCleared();

public: // Properties
    void setSizeMax(qint64 max);

public: // Variables
    QString path;

    QHash<WPixmapCacheKey, WPixmapCacheData *> pixmaps;

    QList<WPixmapCacheData *> datas;

    QHash<QString, QPixmap> hash;

    qint64 size;
    qint64 sizeMax;
};

Q_GLOBAL_STATIC(WPixmapCacheStore, pixmapStore);

//=================================================================================================
// WPixmapCachePrivate
//=================================================================================================

class SK_GUI_EXPORT WPixmapCachePrivate : public WPrivate
{
public:
    WPixmapCachePrivate(WPixmapCache * p);

    /* virtual */ ~WPixmapCachePrivate();

    void init();

public: // Functions
    void readCache(const QString & path, const QSize & size, const QSize & area,
                                                             bool          asynchronous,
                                                             QObject     * receiver,
                                                             const char  * method);

    void readFile(const QString & path, const QSize & size, const QSize & area,
                                                            bool          asynchronous,
                                                            QObject     * receiver,
                                                            const char  * method);

    bool readData(const QString & path, const QSize & size, const QSize & area);

    bool loadImage(const QString & path, const QSize & size, const QSize & area);
    void loadFile (const QString & path, const QSize & size, const QSize & area);

    void tryToCache();
    void addToCache();

    void removeData(QObject * receiver);

public: // Variables
    WPixmapCacheData * data;

protected:
    W_DECLARE_PUBLIC(WPixmapCache)
};

//=================================================================================================
// WPixmapCacheRead
//=================================================================================================

/* virtual */ WAbstractThreadReply * WPixmapCacheRead::createReply() const
{
    return new WPixmapCacheReply(pixmap, data);
}

/* virtual */ bool WPixmapCacheRead::run()
{
    WPixmapCacheReply * reply = qobject_cast<WPixmapCacheReply *> (this->reply());

    if (WPixmapCache::readImage(&(reply->image), path, size, area) == false)
    {
        qWarning("WPixmapCacheRead::run: Failed to read file %s.", path.C_STR);

        return false;
    }
    else return true;
}

//=================================================================================================
// WLibraryFolderReadReply
//=================================================================================================

/* virtual */ void WPixmapCacheReply::onCompleted(bool ok)
{
    data->action = NULL;

    if (ok)
    {
        data->pixmap = QPixmap::fromImage(image);

        qint64 size = getImageSize(image);

        data->pixmapSize = size;

        data->reply = NULL;

        WPixmapCacheStore * store = pixmapStore();

        if (store->datas.contains(data)
            &&
            store->addSize(size, data->path) == false)
        {
            data->toDelete = true;
        }
    }
    else if (data->pixmaps.count())
    {
        foreach (WPixmapCache * pixmap, data->pixmaps)
        {
            pixmap->d_func()->data = NULL;
        }

        WPixmapCacheStore * store = pixmapStore();

        store->datas.removeOne(data);

        store->deleteData(data);
    }
    else
    {
        pixmap->data = NULL;

        delete data;
    }

    emit loaded();
}

//=================================================================================================
// WPixmapCacheStore
//=================================================================================================

WPixmapCacheStore::WPixmapCacheStore() : QObject()
{
    size    = 0;
    sizeMax = PIXMAPCACHE_MAX;

    updateCache();

    connect(wControllerFile, SIGNAL(cacheChanged()), this, SLOT(updateCache()));
}

/* virtual */ WPixmapCacheStore::~WPixmapCacheStore()
{
    foreach (WPixmapCacheData * data, datas)
    {
        delete data;
    }
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

bool WPixmapCacheStore::addSize(qint64 sizeFile, const QString & path)
{
    if (sizeFile < sizeMax)
    {
        size += sizeFile;

        cleanPixmaps();

        return true;
    }
    else
    {
        qWarning("WPixmapCacheStore::addSize: Image is too large for cache %s.", path.C_STR);

        return false;
    }
}

//-------------------------------------------------------------------------------------------------

void WPixmapCacheStore::removeData(WPixmapCacheData * data)
{
    if (data->pixmaps.isEmpty())
    {
        delete data;
    }
    else data->toDelete = true;
}

void WPixmapCacheStore::deleteData(WPixmapCacheData * data)
{
    WPixmapCacheKey key = { &(data->path), &(data->size), &(data->area) };

    pixmaps.remove(key);

    delete data;
}

//-------------------------------------------------------------------------------------------------

void WPixmapCacheStore::cleanPixmaps()
{
    while (datas.isEmpty() == false && size >= sizeMax)
    {
        WPixmapCacheData * data = datas.takeFirst();

        size -= data->pixmapSize;

        if (data->pixmaps.isEmpty())
        {
            deleteData(data);
        }
        else data->toDelete = true;
    }
}

void WPixmapCacheStore::clearPixmaps()
{
    foreach (WPixmapCacheData * data, datas)
    {
        removeData(data);
    }

    pixmaps.clear();
    datas  .clear();

    size = 0;
}

//-------------------------------------------------------------------------------------------------
// Slots
//-------------------------------------------------------------------------------------------------

void WPixmapCacheStore::updateCache()
{
    WCache * cache = wControllerFile->cache();

    if (cache)
    {
        path = cache->path();

        connect(cache, SIGNAL(filesRemoved(const QList<QUrl> &, const QList<QUrl> &)),
                this,  SLOT(onFilesRemoved(const QList<QUrl> &, const QList<QUrl> &)));

        connect(cache, SIGNAL(filesCleared()), this, SLOT(onFilesCleared()));
    }
}

//-------------------------------------------------------------------------------------------------

void WPixmapCacheStore::onFilesRemoved(const QList<QUrl> &, const QList<QUrl> & pathsCache)
{
    QMutableHashIterator<WPixmapCacheKey, WPixmapCacheData *> i(pixmaps);

    while (i.hasNext())
    {
        i.next();

        WPixmapCacheData * data = i.value();

        if (pathsCache.contains(data->path))
        {
            datas.removeOne(data);

            size -= data->pixmapSize;

            i.remove();

            removeData(data);
        }
    }
}

void WPixmapCacheStore::onFilesCleared()
{
    QMutableHashIterator<WPixmapCacheKey, WPixmapCacheData *> i(pixmaps);

    while (i.hasNext())
    {
        i.next();

        WPixmapCacheData * data = i.value();

        if (data->path.startsWith(path))
        {
            datas.removeOne(data);

            size -= data->pixmapSize;

            i.remove();

            removeData(data);
        }
    }
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

void WPixmapCacheStore::setSizeMax(qint64 max)
{
    if (sizeMax == max) return;

    sizeMax = max;

    cleanPixmaps();
}

//=================================================================================================
// WPixmapCachePrivate
//=================================================================================================

WPixmapCachePrivate::WPixmapCachePrivate(WPixmapCache * p) : WPrivate(p) {}

/* virtual */ WPixmapCachePrivate::~WPixmapCachePrivate()
{
    removeData(NULL);
}

//-------------------------------------------------------------------------------------------------

void WPixmapCachePrivate::init()
{
    data = NULL;
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WPixmapCachePrivate::readCache(const QString & path, const QSize & size,
                                                          const QSize & area,
                                                          bool          asynchronous,
                                                          QObject     * receiver,
                                                          const char  * method)
{
    if (path.startsWith("image:///"))
    {
        if (loadImage(path, size, area))
        {
            tryToCache();
        }
        else data = NULL;
    }
    else if (asynchronous)
    {
        loadFile(path, size, area);

        addToCache();

        if (receiver)
        {
            QObject::connect(data->reply, SIGNAL(loaded()), receiver, method);
        }
    }
    else if (readData(path, size, area))
    {
        tryToCache();
    }
    else data = NULL;
}

void WPixmapCachePrivate::readFile(const QString & path, const QSize & size,
                                                         const QSize & area,
                                                         bool          asynchronous,
                                                         QObject     * receiver,
                                                         const char  * method)
{
    if (path.startsWith("image:///"))
    {
        if (loadImage(path, size, area))
        {
             data->toDelete = true;
        }
        else data = NULL;
    }
    else if (asynchronous)
    {
        loadFile(path, size, area);

        data->toDelete = true;

        if (receiver)
        {
            QObject::connect(data->reply, SIGNAL(loaded()), receiver, method);
        }
    }
    else if (readData(path, size, area))
    {
         data->toDelete = true;
    }
    else data = NULL;
}

//-------------------------------------------------------------------------------------------------

bool WPixmapCachePrivate::readData(const QString & path, const QSize & size, const QSize & area)
{
    QPixmap pixmap;

    if (WPixmapCache::readPixmap(&pixmap, path, size, area) == false)
    {
        qWarning("WPixmapCachePrivate::readData: Failed to read file %s.", path.C_STR);

        return false;
    }

    data = new WPixmapCacheData;

    data->path = path;

    data->size = size;
    data->area = area;

    data->pixmap = pixmap;

    data->pixmapSize = getPixmapSize(pixmap);

    data->action = NULL;
    data->reply  = NULL;

    return true;
}

//-------------------------------------------------------------------------------------------------

bool WPixmapCachePrivate::loadImage(const QString & path, const QSize & size, const QSize & area)
{
    QString source = path;

    source.remove(0, 9);

    QPixmap pixmap = pixmapStore()->hash.value(source);

    if (pixmap.isNull())
    {
        qWarning("WPixmapCachePrivate::loadImage: Failed to load image %s.", path.C_STR);

        return false;
    }

    data = new WPixmapCacheData;

    data->path = path;

    data->size = size;
    data->area = area;

    data->pixmap = WPixmapCache::getPixmapScaled(pixmap, size);

    data->pixmapSize = getPixmapSize(pixmap);

    data->action = NULL;
    data->reply  = NULL;

    return true;
}

void WPixmapCachePrivate::loadFile(const QString & path, const QSize & size, const QSize & area)
{
    data = new WPixmapCacheData;

    data->path = path;

    data->size = size;
    data->area = area;

    data->pixmapSize = 0;

    WPixmapCacheRead * action = new WPixmapCacheRead(this, data, path, size, area);

    data->action = action;

    data->reply = qobject_cast<WPixmapCacheReply *> (wControllerFile->startReadAction(action));
}

//-------------------------------------------------------------------------------------------------

void WPixmapCachePrivate::tryToCache()
{
    WPixmapCacheStore * store = pixmapStore();

    if (store->addSize(data->pixmapSize, data->path))
    {
        Q_Q(WPixmapCache);

        data->pixmaps.append(q);

        data->toDelete = false;

        WPixmapCacheKey key = { &(data->path), &(data->size), &(data->area) };

        store->pixmaps.insert(key, data);

        store->datas.append(data);
    }
    else data->toDelete = true;
}

void WPixmapCachePrivate::addToCache()
{
    Q_Q(WPixmapCache);

    WPixmapCacheStore * store = pixmapStore();

    data->pixmaps.append(q);

    data->toDelete = false;

    WPixmapCacheKey key = { &(data->path), &(data->size), &(data->area) };

    store->pixmaps.insert(key, data);

    store->datas.append(data);
}

//-------------------------------------------------------------------------------------------------

void WPixmapCachePrivate::removeData(QObject * receiver)
{
    if (data == NULL) return;

    QList<WPixmapCache *> & pixmaps = data->pixmaps;

    if (pixmaps.isEmpty())
    {
        WPixmapCacheStore * store = pixmapStore();

        store->datas.removeOne(data);

        store->deleteData(data);
    }
    else
    {
        Q_Q(WPixmapCache);

        pixmaps.removeOne(q);

        if (data->toDelete && pixmaps.isEmpty())
        {
            pixmapStore()->deleteData(data);
        }
        else if (receiver)
        {
            WPixmapCacheReply * reply = data->reply;

            if (reply) QObject::disconnect(reply, 0, receiver, 0);
        }
    }
}

//=================================================================================================
// WPixmapCache
//=================================================================================================

WPixmapCache::WPixmapCache() : WPrivatable(new WPixmapCachePrivate(this))
{
    Q_D(WPixmapCache); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

void WPixmapCache::load(const QString & path, const QSize & size, const QSize & area,
                                                                  bool          asynchronous,
                                                                  bool          cache,
                                                                  QObject     * receiver,
                                                                  const char  * method)
{
    Q_D(WPixmapCache);

    if (cache == false)
    {
        d->removeData(receiver);

        d->readFile(path, size, area, asynchronous, receiver, method);

        return;
    }

    WPixmapCacheStore * store = pixmapStore();

    WPixmapCacheKey key = { &path, &size, &area };

    WPixmapCacheData * data = store->pixmaps.value(key);

    if (data)
    {
        if (d->data == data) return;

        d->removeData(receiver);

        d->data = data;

        WPixmapCacheRead * action = data->action;

        if (action)
        {
            if (asynchronous == false)
            {
                WPixmapCacheReply * reply = data->reply;

                action->abortAndDelete();

                data->action = NULL;

                QPixmap * pixmap = &(data->pixmap);

                if (readPixmap(pixmap, path, size, area) == false)
                {
                    qWarning("WPixmapCache::load: Failed to read file %s.", path.C_STR);

                    d->data = NULL;

                    foreach (WPixmapCache * pixmap, data->pixmaps)
                    {
                        pixmap->d_func()->data = NULL;
                    }

                    store->datas.removeOne(data);

                    store->deleteData(data);

                    emit reply->loaded();

                    return;
                }

                qint64 size = getPixmapSize(*pixmap);

                data->pixmapSize = size;

                data->reply = NULL;

                if (store->addSize(size, path) == false)
                {
                    data->toDelete = true;
                }

                emit reply->loaded();

                return;
            }
            else if (receiver)
            {
                QObject::connect(data->reply, SIGNAL(loaded()), receiver, method);
            }
        }

        if (store->datas.removeOne(data))
        {
            data->pixmaps.append(this);

            store->datas.append(data);
        }
        else if (store->addSize(data->pixmapSize, data->path))
        {
            data->pixmaps.append(this);

            data->toDelete = false;

            store->pixmaps.insert(key, data);

            store->datas.append(data);
        }
        else data->toDelete = true;
    }
    else
    {
        d->removeData(receiver);

        d->readCache(path, size, area, asynchronous, receiver, method);
    }
}

//-------------------------------------------------------------------------------------------------

void WPixmapCache::applyPixmap(const QPixmap & pixmap, const QString & path)
{
    Q_D(WPixmapCache);

    setPixmap(pixmap);

    d->data->path = path;
}

void WPixmapCache::changePixmap(const QPixmap & pixmap)
{
    Q_D(WPixmapCache);

    if (d->data)
    {
        d->data->pixmap = pixmap;
    }
}

//-------------------------------------------------------------------------------------------------

void WPixmapCache::disconnect(QObject * receiver)
{
    Q_D(WPixmapCache);

    if (d->data && receiver)
    {
        WPixmapCacheReply * reply = d->data->reply;

        if (reply) QObject::disconnect(reply, 0, receiver, 0);
    }
}

void WPixmapCache::clear(QObject * receiver)
{
    Q_D(WPixmapCache);

    d->removeData(receiver);

    d->data = NULL;
}

//-------------------------------------------------------------------------------------------------
// Static functions
//-------------------------------------------------------------------------------------------------

/* static */ bool WPixmapCache::imageIsLocal(const QString & path)
{
    if (path.isEmpty() || path.startsWith("image:///"))
    {
         return false;
    }
    else return true;
}

//-------------------------------------------------------------------------------------------------

/* static */ WAbstractThreadAction * WPixmapCache::loadImage(const QString & path,
                                                             QObject       * receiver,
                                                             const char    * method)
{
    return loadImage(path, QSize(), QSize(), receiver, method);
}

/* static */ WAbstractThreadAction * WPixmapCache::loadImage(const QString & path,
                                                             const QSize   & size,
                                                             QObject       * receiver,
                                                             const char    * method)
{
    return loadImage(path, size, QSize(), receiver, method);
}

/* static */ WAbstractThreadAction * WPixmapCache::loadImage(const QString & path,
                                                             const QSize   & size,
                                                             const QSize   & area,
                                                             QObject       * receiver,
                                                             const char    * method)
{
    WPixmapCacheImageRead * action = new WPixmapCacheImageRead(path, size, area);

    WPixmapCacheImageReply * reply = qobject_cast<WPixmapCacheImageReply *>
                                     (wControllerFile->startReadAction(action));

    QObject::connect(reply, SIGNAL(loaded(const QImage &)), receiver, method);

    return action;
}

//-------------------------------------------------------------------------------------------------

/* static */ QSize WPixmapCache::getSize(const QImageReader & reader, const QSize & size)
{
    int width  = size.width ();
    int height = size.height();

    if (width > 0 || height > 0)
    {
        QSize sizeReader = reader.size();

        sizeReader.scale(width, height, Qt::KeepAspectRatioByExpanding);

        return sizeReader;
    }
    else return reader.size();
}

/* static */ QSize WPixmapCache::getArea(const QSize & size, const QSize & area)
{
    if (area.width() <= 0)
    {
        return QSize(size.width(), area.height());
    }
    else if (area.height() <= 0)
    {
        return QSize(area.width(), size.height());
    }
    else return area;
}

//-------------------------------------------------------------------------------------------------

/* static */ void WPixmapCache::applySize(QImageReader * reader, const QSize & size)
{
    int width  = size.width ();
    int height = size.height();

    if (width > 0 || height > 0)
    {
        QSize sizeReader = reader->size();

        sizeReader.scale(width, height, Qt::KeepAspectRatioByExpanding);

        reader->setScaledSize(sizeReader);
    }
}

//-------------------------------------------------------------------------------------------------

/* static */ QPixmap WPixmapCache::getPixmapScaled(const QPixmap & pixmap, const QSize & size)
{
    int width  = size.width ();
    int height = size.height();

    if (width > 0 || height > 0)
    {
         return pixmap.scaled(QSize(width, height), Qt::KeepAspectRatioByExpanding,
                                                    Qt::SmoothTransformation);
    }
    else return pixmap;
}

//-------------------------------------------------------------------------------------------------

/* static */ bool WPixmapCache::readImage(QImage * image, const QString & path,
                                                          const QSize   & size,
                                                          const QSize   & area)
{
    QFile file(WControllerFile::filePath(path));

    if (file.open(QIODevice::ReadOnly) == false)
    {
        qWarning("WPixmapCache::readImage: Failed to open file %s.", path.C_STR);

        return false;
    }

    QImageReader reader(&file);

    if (area.width() > 0 || area.height() > 0)
    {
        QSize sizeFront = getSize(reader, size);

        QSize sizeArea = getArea(sizeFront, area);

        int width  = sizeArea.width ();
        int height = sizeArea.height();

        int marginX = (width  - sizeFront.width ()) / 2;
        int marginY = (height - sizeFront.height()) / 2;

        if (marginX > 0 && marginY > 0)
        {
            sizeFront = QSize(width - marginX * 2, height - marginY * 2);

            reader.setScaledSize(sizeFront);

            QImage front;

            reader.read(&front);

            QImage content(sizeArea, QImage::Format_ARGB32_Premultiplied);

            content.fill(Qt::transparent);

            QPainter painter(&content);

            painter.drawImage(marginX, marginY, front);

            *image = content;
        }
        else
        {
            reader.setScaledSize(sizeFront);

            reader.read(image);
        }
    }
    else
    {
        applySize(&reader, size);

        reader.read(image);
    }

    return true;
}

/* static */ bool WPixmapCache::readPixmap(QPixmap * pixmap, const QString & path,
                                                             const QSize   & size,
                                                             const QSize   & area)
{
    QFile file(WControllerFile::filePath(path));

    if (file.open(QIODevice::ReadOnly) == false)
    {
        qWarning("WPixmapCache::readPixmap: Failed to open file %s.", path.C_STR);

        return false;
    }

    QImageReader reader(&file);

    if (area.width() > 0 || area.height() > 0)
    {
        QSize sizeFront = getSize(reader, size);

        QSize sizeArea = getArea(sizeFront, area);

        int width  = sizeArea.width ();
        int height = sizeArea.height();

        int marginX = (width  - sizeFront.width ()) / 2;
        int marginY = (height - sizeFront.height()) / 2;

        if (marginX > 0 && marginY > 0)
        {
            sizeFront = QSize(width - marginX * 2, height - marginY * 2);

            reader.setScaledSize(sizeFront);

            QPixmap front = QPixmap::fromImageReader(&reader);

            QPixmap content(sizeArea);

            content.fill(Qt::transparent);

            QPainter painter(&content);

            painter.drawPixmap(marginX, marginY, front);

            *pixmap = content;
        }
        else
        {
            reader.setScaledSize(sizeFront);

            *pixmap = QPixmap::fromImageReader(&reader);
        }
    }
    else
    {
        applySize(&reader, size);

        *pixmap = QPixmap::fromImageReader(&reader);
    }

    return true;
}

//-------------------------------------------------------------------------------------------------

/* static */ void WPixmapCache::registerPixmap(const QString & id, const QPixmap & pixmap)
{
    pixmapStore()->hash.insert(id, pixmap);
}

/* static */ void WPixmapCache::unregisterPixmap(const QString & id)
{
    pixmapStore()->hash.remove(id);
}

/* static */ void WPixmapCache::unregisterPixmaps()
{
    pixmapStore()->hash.clear();
}

//-------------------------------------------------------------------------------------------------

/* static */ void WPixmapCache::clearCache()
{
    pixmapStore()->clearPixmaps();
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

bool WPixmapCache::isNull() const
{
    Q_D(const WPixmapCache);

    if (d->data)
    {
         return d->data->pixmap.isNull();
    }
    else return true;
}

bool WPixmapCache::isLoading() const
{
    Q_D(const WPixmapCache);

    if (d->data)
    {
         return (d->data->action);
    }
    else return false;
}

bool WPixmapCache::isLoaded() const
{
    return (isLoading() == false);
}

//-------------------------------------------------------------------------------------------------

const QPixmap & WPixmapCache::pixmap() const
{
    Q_D(const WPixmapCache);

    if (d->data)
    {
         return d->data->pixmap;
    }
    else return dataNull()->pixmap;
}

void WPixmapCache::setPixmap(const QPixmap & pixmap, QObject * receiver)
{
    Q_D(WPixmapCache);

    d->removeData(receiver);

    d->data = new WPixmapCacheData;

    d->data->pixmap = pixmap;

    d->data->pixmapSize = getPixmapSize(pixmap);

    d->data->action = NULL;
    d->data->reply  = NULL;

    d->data->toDelete = false;
}

//-------------------------------------------------------------------------------------------------

const QString & WPixmapCache::path() const
{
    Q_D(const WPixmapCache);

    if (d->data)
    {
         return d->data->path;
    }
    else return dataNull()->path;
}

const QSize & WPixmapCache::size() const
{
    Q_D(const WPixmapCache);

    if (d->data)
    {
         return d->data->size;
    }
    else return dataNull()->size;
}

//-------------------------------------------------------------------------------------------------

int WPixmapCache::width() const
{
    Q_D(const WPixmapCache);

    if (d->data)
    {
         return d->data->pixmap.width();
    }
    else return -1;
}

int WPixmapCache::height() const
{
    Q_D(const WPixmapCache);

    if (d->data)
    {
         return d->data->pixmap.height();
    }
    else return -1;
}

//-------------------------------------------------------------------------------------------------

QRect WPixmapCache::rect() const
{
    Q_D(const WPixmapCache);

    if (d->data)
    {
         return d->data->pixmap.rect();
    }
    else return QRect();
}

//-------------------------------------------------------------------------------------------------

/* static */ qint64 WPixmapCache::sizeMax()
{
    return pixmapStore()->sizeMax;
}

/* static */ void WPixmapCache::setSizeMax(qint64 max)
{
    pixmapStore()->setSizeMax(max);
}

#endif // SK_NO_PIXMAPCACHE

#include "WPixmapCache.moc"
