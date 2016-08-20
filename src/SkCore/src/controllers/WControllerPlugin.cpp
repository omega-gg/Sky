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

#include "WControllerPlugin.h"

#ifndef SK_NO_CONTROLLERPLUGIN

// Qt includes
#include <QCoreApplication>
#include <QHash>
#include <QDir>

// Sk includes
#include <WControllerScript>
#include <WPlugin>
#include <WFileWatcher>

W_INIT_CONTROLLER(WControllerPlugin)

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include <private/WController_p>

class WControllerPluginPrivate : public WControllerPrivate
{
public:
    WControllerPluginPrivate(WControllerPlugin * p);

    /* virtual */ ~WControllerPluginPrivate();

    void init();

public: // Functions
    bool isPlugin(const QString & path) const;

    void addPluginPath   (const QString & path);
    void removePluginPath(const QString & path);

public: // Variables
    QList<WPlugin *> plugins;

    WFileWatcher fileWatcher;

    QHash<QString, QString> paths;

protected:
    W_DECLARE_PUBLIC(WControllerPlugin)
};

//-------------------------------------------------------------------------------------------------

WControllerPluginPrivate::WControllerPluginPrivate(WControllerPlugin * p)
    : WControllerPrivate(p) {}

/* virtual */ WControllerPluginPrivate::~WControllerPluginPrivate()
{
    foreach (WPlugin * plugin, plugins) delete plugin;

    W_CLEAR_CONTROLLER(WControllerScript);
}

//-------------------------------------------------------------------------------------------------

void WControllerPluginPrivate::init()
{
    Q_Q(WControllerPlugin);

    QObject::connect(&fileWatcher, SIGNAL(filesCreated(QString, QStringList)),
                     q, SLOT(onFilesCreated(QString, QStringList)));

    QObject::connect(&fileWatcher, SIGNAL(filesDeleted(QString, QStringList)),
                     q, SLOT(onFilesDeleted(QString, QStringList)));
}

//-------------------------------------------------------------------------------------------------

bool WControllerPluginPrivate::isPlugin(const QString & path) const
{
    QFileInfo info(path);

#ifdef Q_OS_WIN
    if (info.suffix() == "dll") return true;
#elif defined Q_OS_MAC
    if (info.suffix() == "dylib") return true;
#elif defined Q_OS_LINUX
    if (info.suffix() == "so") return true;
#endif
    else return false;
}

void WControllerPluginPrivate::addPluginPath(const QString & path)
{
    if (isPlugin(path) == false) return;

    // If map already contains that plugin path
    if (paths.key(path).isNull() == false) return;

    QString name = WPlugin::nameFromPlugin(path);

    if (name.isNull()) return;

    qDebug("Adding plugin %s %s", name.C_STR, path.C_STR);

    paths.insert(name, path);

    Q_Q(WControllerPlugin);

    emit q->pluginDetected(name);
}

void WControllerPluginPrivate::removePluginPath(const QString & path)
{
    if (isPlugin(path) == false) return;

    // If map doesn't contain that plugin path
    QString name = paths.key(path);

    if (name.isNull()) return;

    paths.remove(name);

    Q_Q(WControllerPlugin);

    emit q->pluginUndetected(name);
}

//-------------------------------------------------------------------------------------------------
// Private ctor / dtor
//-------------------------------------------------------------------------------------------------

WControllerPlugin::WControllerPlugin() : WController(new WControllerPluginPrivate(this)) {}

//-------------------------------------------------------------------------------------------------
// Initialize
//-------------------------------------------------------------------------------------------------

/* virtual */ void WControllerPlugin::init()
{
    Q_D(WControllerPlugin); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

WPlugin * WControllerPlugin::plugin(const QString & name)
{
    Q_D(WControllerPlugin);

#ifdef QT_DEBUG
    QString debugName = name + 'D';

    foreach (WPlugin * plugin, d->plugins)
    {
        if (plugin->name() == debugName) return plugin;
    }
#endif

    foreach (WPlugin * plugin, d->plugins)
    {
        if (plugin->name() == name) return plugin;
    }
    return NULL;
}

QString WControllerPlugin::pluginFileName(const QString & name)
{
    Q_D(WControllerPlugin);

    QString fileName = name;

#if defined Q_OS_MACX || defined Q_OS_LINUX
    fileName.prepend("lib");
#endif

#ifdef QT_DEBUG
    QString fileName2 = fileName;

    fileName2.append('D');

    fileName2 = d->paths.value(fileName2);

    if (fileName2.isNull() == false) return fileName2;
#endif

    return d->paths.value(fileName);
}

bool WControllerPlugin::addPath(const QString dirPath)
{
    Q_D(WControllerPlugin);

    if (d->fileWatcher.contains(dirPath)) return false;

    QDir dir(dirPath);

    if (dir.exists() == false) return false;

    // Updating library path
    QStringList paths = qApp->libraryPaths();

    qDebug("Pushing back: %s", dir.absolutePath().C_STR);

    if (paths.contains(dir.absolutePath()) == false)
    {
        paths.append(dir.absolutePath());

        qApp->setLibraryPaths(paths);
    }

    // Adding watcher
    d->fileWatcher.addFolder(dirPath);

    QFileInfoList infoList = dir.entryInfoList(QDir::Files);

    foreach (const QFileInfo & info, infoList)
    {
        d->addPluginPath(info.filePath());
    }

    return true;
}

bool WControllerPlugin::loadPlugin(const QString & name)
{
    // Do we have a path matching that name ?
    QString path = pluginFileName(name);

    if (path.isNull())
    {
        qWarning("WControllerPlugin::loadPlugin: Failed to load plugin %s. Path is empty.",
                 name.C_STR);

        return false;
    }

    // If it exists, unloading old plugin
    unloadPlugin(name);

    WPlugin * plugin = new WPlugin(path, this);

    // There was a problem during the loading
    if (plugin->isLoaded() == false)
    {
        qWarning("WControllerPlugin::loadPlugin: Failed to load plugin %s. Invalid path %s.",
                 name.C_STR, path.C_STR);

        delete plugin;

        return false;
    }

    return true;
}

bool WControllerPlugin::unloadPlugin(const QString & name)
{
    WPlugin * oldPlugin = plugin(name);

    if (oldPlugin)
    {
        oldPlugin->unload();

        delete oldPlugin;

        return true;
    }

    return false;
}

bool WControllerPlugin::loadScriptPlugin(const QString & extension)
{
    return wControllerScript->loadPlugin(extension);
}

//-------------------------------------------------------------------------------------------------

QStringList WControllerPlugin::availablePlugins() const
{
    Q_D(const WControllerPlugin);

    QStringList plugins;

    QHashIterator<QString, QString> i(d->paths);

    while (i.hasNext())
    {
        i.next();

        plugins.append(i.key());
    }

    return plugins;
}

//-------------------------------------------------------------------------------------------------
// Private WPlugin interface
//-------------------------------------------------------------------------------------------------

void WControllerPlugin::declarePlugin(WPlugin * plugin)
{
    Q_D(WControllerPlugin);

    if (d->plugins.contains(plugin)) return;

    d->plugins.append(plugin);
}

void WControllerPlugin::undeclarePlugin(WPlugin * plugin)
{
    Q_D(WControllerPlugin);

    if (d->plugins.contains(plugin) == false) return;

    d->plugins.removeOne(plugin);
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WControllerPlugin::onFilesCreated(const QString &, const QStringList & fileNames)
{
    Q_D(WControllerPlugin);

    foreach (const QString & name, fileNames)
    {
        d->addPluginPath(name);
    }
}

void WControllerPlugin::onFilesDeleted(const QString &, const QStringList & fileNames)
{
    Q_D(WControllerPlugin);

    foreach (const QString & name, fileNames)
    {
        d->removePluginPath(name);
    }
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

QList<WPlugin *> WControllerPlugin::plugins() const
{
    Q_D(const WControllerPlugin); return d->plugins;
}

#endif // SK_NO_CONTROLLERPLUGIN
