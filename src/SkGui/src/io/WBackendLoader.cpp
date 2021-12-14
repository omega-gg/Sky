//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkGui.

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

#include "WBackendLoader.h"

#ifndef SK_NO_BACKENDLOADER

// Qt includes
#include <QCoreApplication>
#include <QMutex>

// Sk includes
#include <WControllerPlaylist>

// Forward declarations
class WBackendLoaderCache;

//-------------------------------------------------------------------------------------------------
// Static variables

static const int BACKENDLOADER_MAX = 50;

//=================================================================================================
// WBackendLoaderCache
//=================================================================================================

class WBackendLoaderCache
{
public:
    WBackendLoaderCache();

public: // Functions
    void addBackend(const QString & id, WBackendNet * backend);

    WBackendNet * getBackend(const QString & id);

    void updateBackend(const QString & id);

    void cleanCache();

    void clear();

public: // Variables
    // NOTE: Do we really need a mutex here ?
    QMutex mutex;

    QHash<QString, WBackendNet *> hash;

    QList<WBackendNet *> backends;

    int maxCount;
};

Q_GLOBAL_STATIC(WBackendLoaderCache, backendCache)

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

WBackendLoaderCache::WBackendLoaderCache()
{
    maxCount = BACKENDLOADER_MAX;
}

//-------------------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------------------

void WBackendLoaderCache::addBackend(const QString & id, WBackendNet * backend)
{
    cleanCache();

    backends.append(backend);

    mutex.lock();

    // NOTE: We append on the hash at the end because 'createNow' depends on it.
    hash.insert(id, backend);

    mutex.unlock();
}

//-------------------------------------------------------------------------------------------------

WBackendNet * WBackendLoaderCache::getBackend(const QString & id)
{
    const QMutexLocker locker(&mutex);

    return hash.value(id);
}

void WBackendLoaderCache::updateBackend(const QString & id)
{
    foreach (WBackendNet * backend, backends)
    {
        if (backend->id() != id) continue;

        backend->d_func()->lockCount++;

        // NOTE: This is useful for caching considerations.
        backends.removeOne(backend);
        backends.append   (backend);

        return;
    }
}

//-------------------------------------------------------------------------------------------------

void WBackendLoaderCache::cleanCache()
{
    int count = backends.count();

    if (count < maxCount) return;

    int index = 0;

    while (index < count)
    {
        WBackendNet * backend = backends.at(index);

        if (backend->d_func()->lockCount == 0)
        {
            QString id = backend->id();

            qDebug("REMOVE BACKEND %s", id.C_STR);

            mutex.lock();

            // NOTE: We remove the id first to avoid returning an invalid backend.
            hash.remove(id);

            mutex.unlock();

            backends.removeAt(index);

            backend->deleteLater();

            count--;

            if (count == maxCount) break;
        }
        else index++;
    }
}

void WBackendLoaderCache::clear()
{
    int index = 0;

    int count = backends.count();

    while (index < count)
    {
        WBackendNet * backend = backends.at(index);

        if (backend->d_func()->lockCount == 0)
        {
            QString id = backend->id();

            qDebug("CLEAR BACKEND %s", id.C_STR);

            mutex.lock();

            // NOTE: We remove the hash first to avoid returning an invalid backend.
            hash.remove(id);

            mutex.unlock();

            backends.removeAt(index);

            backend->deleteLater();

            count--;
        }
        else index++;
    }
}

//=================================================================================================
// WBackendLoaderPrivate
//=================================================================================================

WBackendLoaderPrivate::WBackendLoaderPrivate(WBackendLoader * p) : WPrivate(p) {}

/* virtual */ WBackendLoaderPrivate::~WBackendLoaderPrivate()
{
    Q_Q(WBackendLoader);

    W_GET_CONTROLLER(WControllerPlaylist, controller);

    if (controller) controller->d_func()->unregisterLoader(q);
}

//-------------------------------------------------------------------------------------------------

void WBackendLoaderPrivate::init()
{
    Q_Q(WBackendLoader);

    const QMetaObject * meta = q->metaObject();

    create = meta->method(meta->indexOfMethod("onCreate(QString)"));
    update = meta->method(meta->indexOfMethod("onUpdate(QString)"));
    remove = meta->method(meta->indexOfMethod("onRemove(QString)"));
    reload = meta->method(meta->indexOfMethod("onReload()"));
    clear  = meta->method(meta->indexOfMethod("onClear()"));

    wControllerPlaylist->d_func()->registerLoader(q);
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WBackendLoaderPrivate::removeBackend(const QString & id)
{
    Q_Q(WBackendLoader);

    // NOTE: We want backend removal to be thread safe.
    remove.invoke(q, Q_ARG(QString, id));
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WBackendLoaderPrivate::onCreate(const QString & id)
{
    Q_Q(WBackendLoader);

    WBackendLoaderCache * cache = backendCache();

    // NOTE: Maybe the backend was already created in a previous call.
    WBackendNet * backend = cache->getBackend(id);

    if (backend)
    {
        qDebug("BACKEND ALREADY CREATED %s", id.C_STR);

        backend->d_func()->lockCount++;

        return;
    }

    qDebug("CREATE BACKEND %s", id.C_STR);

    backend = q->createBackend(id);

    backend->setParent(q);

    backend->d_func()->loader = q;

    backend->d_func()->lockCount++;

    // NOTE: We call 'addBackend' at the end because 'createNow' depends on it.
    cache->addBackend(id, backend);
}

void WBackendLoaderPrivate::onUpdate(const QString & id)
{
    backendCache()->updateBackend(id);
}

void WBackendLoaderPrivate::onRemove(const QString & id)
{
    WBackendNet * backend = backendCache()->getBackend(id);

    if (backend == NULL) return;

    backend->d_func()->lockCount--;

    backendCache()->cleanCache();
}

void WBackendLoaderPrivate::onReload()
{
    foreach (WBackendNet * backend, backendCache()->backends)
    {
        backend->reload();
    }
}

void WBackendLoaderPrivate::onClear()
{
    backendCache()->clear();
}

//=================================================================================================
// WBackendLoader
//=================================================================================================

WBackendLoader::WBackendLoader(QObject * parent)
    : QObject(parent), WPrivatable(new WBackendLoaderPrivate(this))
{
    Q_D(WBackendLoader); d->init();
}

//-------------------------------------------------------------------------------------------------
// Protected

WBackendLoader::WBackendLoader(WBackendLoaderPrivate * p, QObject * parent)
    : QObject(parent), WPrivatable(p)
{
    Q_D(WBackendLoader); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ WBackendNet * WBackendLoader::create(const QString & id)
{
    WBackendNet * backend = getBackend(id);

    if (backend)
    {
         return backend;
    }
    else return createNow(id);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ WBackendNet * WBackendLoader::match(const QString & url)
{
    QString id = idFromUrl(url);

    if (id.isEmpty())
    {
         return NULL;
    }
    else return create(id);
}

/* Q_INVOKABLE */ WBackendNet * WBackendLoader::matchCover(const QString & label,
                                                           const QString & q)
{
    QStringList ids = getCoverIds();

    foreach (const QString & id, ids)
    {
        WBackendNet * backend = create(id);

        if (backend->checkCover(label, q)) return backend;
    }

    return NULL;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ QString WBackendLoader::idFromUrl(const QString & url) const
{
    return getId(WControllerPlaylist::simpleSource(url).toLower());
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ QString WBackendLoader::coverFromUrl(const QString & url) const
{
    QString id = idFromUrl(url);

    if (id.isEmpty())
    {
         return QString();
    }
    else return coverFromId(id);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WBackendLoader::reloadBackends()
{
    Q_D(WBackendLoader);

    // NOTE: We want backend reloading to be thread safe.
    d->reload.invoke(this);
}

/* Q_INVOKABLE */ void WBackendLoader::clearCache()
{
    Q_D(WBackendLoader);

    // NOTE: We want backend clearing to be thread safe.
    d->clear.invoke(this);
}

//-------------------------------------------------------------------------------------------------
// Static functions
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ int WBackendLoader::getMaxCache()
{
    return backendCache()->maxCount;
}

/* Q_INVOKABLE static */ void WBackendLoader::setMaxCache(int max)
{
    backendCache()->maxCount = max;
}

//-------------------------------------------------------------------------------------------------
// Virtual interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ bool WBackendLoader::checkId(const QString &) const
{
    return false;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ void WBackendLoader::createFolderItems(WLibraryFolder *,
                                                                 WLibraryItem::Type) const {}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QString WBackendLoader::coverFromId(const QString &) const
{
    return QString();
}

//-------------------------------------------------------------------------------------------------
// Protected virtual functions
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ WBackendNet * WBackendLoader::getBackend(const QString & id)
{
    WBackendNet * backend = backendCache()->getBackend(id);

    if (backend == NULL) return NULL;

    WBackendLoader * loader = backend->d_func()->loader;

    if (loader)
    {
        // NOTE: We want backend updating to be thread safe.
        loader->d_func()->update.invoke(loader, Q_ARG(QString, id));
    }

    return backend;
}

/* Q_INVOKABLE virtual */ WBackendNet * WBackendLoader::createBackend(const QString &) const
{
    return NULL;
}

/* Q_INVOKABLE virtual */ void WBackendLoader::waitBackend(WBackendNet *) const {}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QString WBackendLoader::getId(const QString &) const
{
    return QString();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QStringList WBackendLoader::getCoverIds() const
{
    return QStringList();
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

WBackendNet * WBackendLoader::createNow(const QString & id)
{
    if (checkId(id) == false) return NULL;

    Q_D(WBackendLoader);

    // NOTE: We want backend creation to be thread safe.
    d->create.invoke(this, Q_ARG(QString, id));

    WBackendLoaderCache * cache = backendCache();

    WBackendNet * backend = cache->getBackend(id);

    while (backend == NULL)
    {
        QCoreApplication::processEvents();

        backend = cache->getBackend(id);
    }

    waitBackend(backend);

    return backend;
}

#endif // SK_NO_BACKENDLOADER
