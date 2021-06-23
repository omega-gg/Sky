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

    void removeBackend(WBackendNet * backend);

    WBackendNet * getBackend(const QString & id);

    void clear();

public: // Variables
    QStringList ids;

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
    int count = ids.count();

    if (count >= maxCount)
    {
        int index = 0;

        while (index < count)
        {
            WBackendNet * backend = backends.at(index);

            if (backend->d_func()->lockCount == 0)
            {
                qDebug("REMOVE BACKEND %s", backend->id().C_STR);

                ids     .removeFirst();
                backends.removeFirst();

                backend->deleteLater();

                count--;

                if (count == maxCount) break;
            }
            else index++;
        }
    }

    ids.append(id);

    backends.append(backend);
}

void WBackendLoaderCache::removeBackend(WBackendNet * backend)
{
    for (int i = 0; i < ids.count(); i++)
    {
        if (backends.at(i) != backend) continue;

        ids     .removeAt(i);
        backends.removeAt(i);

        return;
    }
}

//-------------------------------------------------------------------------------------------------

WBackendNet * WBackendLoaderCache::getBackend(const QString & id)
{
    int index = ids.indexOf(id);

    if (index == -1) return NULL;

    ids.removeAt(index);

    ids.append(id);

    WBackendNet * backend = backends.takeAt(index);

    backends.append(backend);

    return backend;
}

//-------------------------------------------------------------------------------------------------

void WBackendLoaderCache::clear()
{
    int index = 0;

    int count = ids.count();

    while (index < count)
    {
        WBackendNet * backend = backends.at(index);

        if (backend->d_func()->lockCount == 0)
        {
            qDebug("CLEAR BACKEND %s", backend->id().C_STR);

            ids     .removeFirst();
            backends.removeFirst();

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

    wControllerPlaylist->d_func()->registerLoader(q);
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WBackendLoaderPrivate::onCreate(const QString & id)
{
    Q_Q(WBackendLoader);

    WBackendNet * backend = q->createBackend(id);

    backend->setParent(q);

    backend->d_func()->lockCount++;

    backendCache()->addBackend(id, backend);

    QObject::connect(backend, SIGNAL(destroyed()), q, SLOT(onDestroyed()));
}

void WBackendLoaderPrivate::onDestroyed()
{
    Q_Q(WBackendLoader);

    backendCache()->removeBackend(static_cast<WBackendNet *> (q->sender()));
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
// Static functions
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ void WBackendLoader::reloadBackends()
{
    foreach (WBackendNet * backend, backendCache()->backends)
    {
        backend->reload();
    }
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ void WBackendLoader::clearCache()
{
    backendCache()->clear();
}

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
// Protected static functions
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ WBackendNet * WBackendLoader::getBackend(const QString & id)
{
    WBackendNet * backend = backendCache()->getBackend(id);

    if (backend) backend->d_func()->lockCount++;

    return backend;
}

//-------------------------------------------------------------------------------------------------
// Protected virtual functions
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ WBackendNet * WBackendLoader::createBackend(const QString &) const
{
    return NULL;
}

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

    // NOTE: We want backend loading to be thread safe.
    d->create.invoke(this, Q_ARG(QString, id));

    WBackendNet * backend = backendCache()->getBackend(id);

    while (backend == NULL)
    {
        QCoreApplication::processEvents();

        backend = backendCache()->getBackend(id);
    }

    return backend;
}

#endif // SK_NO_BACKENDLOADER
