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

#include "WBackendLoader.h"

#ifndef SK_NO_BACKENDLOADER

#ifdef Q_OS_LINUX
// Qt includes
#include <QStringList>
#endif

// Sk includes
#include <WControllerPlaylist>

// Private includes
#include "WBackendLoader_p.h"

//-------------------------------------------------------------------------------------------------
// Static variables

static const int BACKENDLOADER_MAX = 50;

//=================================================================================================
// WBackendLoaderCache
//=================================================================================================

class WBackendLoaderCache
{
public: // Ctor / dtor
    WBackendLoaderCache();

    ~WBackendLoaderCache();

public: // Functions
    void addBackend(const QString & id, WBackendNet * backend);

    WBackendNet * getBackend(const QString & id);

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

WBackendLoaderCache::~WBackendLoaderCache()
{
    foreach (WBackendNet * backend, backends)
    {
        backend->deleteLater();
    }
}

//-------------------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------------------

void WBackendLoaderCache::addBackend(const QString & id, WBackendNet * backend)
{
    while (ids.count() > maxCount)
    {
        WBackendNet * backend = backends.first();

        if (backend->d_func()->lockCount == 0)
        {
            ids     .removeFirst();
            backends.removeFirst();

            backend->deleteLater();
        }
    }

    ids.append(id);

    backends.append(backend);
}

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

    wControllerPlaylist->d_func()->registerLoader(q);

    q->moveToThread(wControllerPlaylist->d_func()->thread);
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

/* Q_INVOKABLE */ WBackendNet * WBackendLoader::create(const QString & id) const
{
    WBackendNet * backend = getBackend(id);

    if (backend)
    {
         return backend;
    }
    else return createNow(id);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ WBackendNet * WBackendLoader::match(const QString & pattern) const
{
    QString id = matchBackend(pattern);

    if (id.isEmpty())
    {
         return NULL;
    }
    else return create(id);
}

/* Q_INVOKABLE */ WBackendNet * WBackendLoader::matchCover(const QString & label,
                                                           const QString & q) const
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

/* Q_INVOKABLE static */ WBackendNet * WBackendLoader::getBackend(const QString & id)
{
    WBackendNet * backend = backendCache()->getBackend(id);

    if (backend) backend->d_func()->lockCount++;

    return backend;
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

/* Q_INVOKABLE virtual */ void WBackendLoader::createFolderItems(WLibraryFolder *) const {}

//-------------------------------------------------------------------------------------------------
// Protected virtual functions
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ WBackendNet * WBackendLoader::createBackend(const QString &) const
{
    return NULL;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QString WBackendLoader::matchBackend(const QString &) const
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

WBackendNet * WBackendLoader::createNow(const QString & id) const
{
    if (checkId(id) == false) return NULL;

    WBackendNet * backend = createBackend(id);

    backend->d_func()->lockCount++;

    backendCache()->addBackend(id, backend);

    return backend;
}

#endif // SK_NO_BACKENDLOADER
