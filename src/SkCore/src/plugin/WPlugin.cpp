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

#include "WPlugin.h"

#ifndef SK_NO_PLUGIN

// Qt includes
#include <QPluginLoader>
#include <QFileInfo>
#include <QDomDocument>

// Sk includes
#include <WControllerPlugin>
#include <WControllerXml>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include "WPlugin_p.h"

WPluginPrivate::WPluginPrivate(WPlugin * p) : WPrivate(p) {}

/* virtual */ WPluginPrivate::~WPluginPrivate()
{
    Q_Q(WPlugin);

    W_GET_CONTROLLER(WControllerPlugin, controller);

    if (controller) controller->undeclarePlugin(q);
}

void WPluginPrivate::init()
{    
    Q_Q(WPlugin);

    loader = new QPluginLoader(q);

    W_CONTROLLER(WControllerPlugin, controller);

    controller->declarePlugin(q);
}

//-------------------------------------------------------------------------------------------------

bool WPluginPrivate::loadSpecs(const QString & specFileName)
{
    Q_Q(WPlugin);

    if (QFile::exists(specFileName) == false) return false;

    QDomDocument specifications;

    if (WControllerXml::loadFromFile(&specifications, specFileName) == false) return false;

    // Parsing specs
    QDomNamedNodeMap attributes = WControllerXml::firstElementAttributes(specifications, "plugin");

    name          = attributes.namedItem("name").nodeValue();
    version       = attributes.namedItem("version").nodeValue();
    compatVersion = attributes.namedItem("compatVersion").nodeValue();

    license     = WControllerXml::firstElementValue(specifications, "license");
    description = WControllerXml::firstElementValue(specifications, "description");
    url         = WControllerXml::firstElementValue(specifications, "url");

    qDebug("name [%s]",          name         .C_STR);
    qDebug("version [%s]",       version      .C_STR);
    qDebug("compatVersion [%s]", compatVersion.C_STR);
    qDebug("license [%s]",       license      .C_STR);
    qDebug("description [%s]",   description  .C_STR);
    qDebug("url [%s]",           url          .C_STR);

    return q->loadSpecs(&specifications);
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

WPlugin::WPlugin(const QString & fileName, QObject * parent)
    : QObject(parent), WPrivatable(new WPluginPrivate(this))
{
    Q_D(WPlugin);

    d->init();

    load(fileName);
}

/* explicit */ WPlugin::WPlugin(QObject * parent)
    : QObject(parent), WPrivatable(new WPluginPrivate(this))
{
    Q_D(WPlugin); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

bool WPlugin::load(const QString & fileName)
{
    Q_D(WPlugin);

    if (QFile::exists(fileName) == false)
    {
        qWarning("WPlugin::load: Failed to load plugin. File does not exist %s.", fileName.C_STR);

        return false;
    }

    QString specFileName = WPlugin::specFileName(fileName);

    if (d->loadSpecs(specFileName) == false)
    {
        d->name = WPlugin::nameFromPlugin(fileName);
    }

    if (d->loader->isLoaded()) unload();

    d->loader->setFileName(fileName);

    bool result = d->loader->load();

    if (result == false)
    {
        qWarning("WPlugin::load: Failed to load plugin %s. %s.", fileName.C_STR,
                                                                 d->loader->errorString().C_STR);

        return false;
    }

    d->fileName = fileName;

    loadEvent();

    qDebug("plugin loaded %s", fileName.C_STR);

    return true;
}

bool WPlugin::unload()
{
    Q_D(WPlugin);

    unloadEvent();

    bool result = d->loader->unload();

    qDebug("plugin unloaded %s %d", d->fileName.C_STR, result);

    return result;
}

//-------------------------------------------------------------------------------------------------

bool WPlugin::isLoaded() const
{
    Q_D(const WPlugin); return d->loader->isLoaded();
}

QString WPlugin::errorString() const
{
    Q_D(const WPlugin); return d->loader->errorString();
}

//-------------------------------------------------------------------------------------------------
// Static functions
//-------------------------------------------------------------------------------------------------

/* static */ QString WPlugin::specFileName(const QString & pluginFileName)
{
    QFileInfo info(pluginFileName);

    if (info.exists() == false) return QString();

    return info.absolutePath() + "/" + info.baseName() + ".pluginspec";
}

/* static */ QString WPlugin::nameFromPlugin(const QString & fileName)
{
    // Do we have a spec file ?
    QString name = nameFromSpecFile(specFileName(fileName));

    if (name.isNull() == false) return name;

    // If not, we generate the name from fileName
    QFileInfo info(fileName);

    if (info.exists() == false) return QString();

    name = info.baseName();

    return name;
}

/* static */ QString WPlugin::nameFromSpecFile(const QString & fileName)
{
    if (fileName.isEmpty()) return QString();

    QDomDocument specifications;

    if (WControllerXml::loadFromFile(&specifications, fileName) == false)
    {
        return QString();
    }

    // Getting attributes
    QDomNamedNodeMap attributes = WControllerXml::firstElementAttributes(specifications, "plugin");

    return attributes.namedItem("name").nodeValue();
}

//-------------------------------------------------------------------------------------------------
// Protected functions
//-------------------------------------------------------------------------------------------------

/* virtual */ bool WPlugin::loadSpecs(QDomDocument *)
{
    return true;
}

//-------------------------------------------------------------------------------------------------
// Events
//-------------------------------------------------------------------------------------------------

/* virtual */ void WPlugin::loadEvent()
{
    Q_D(WPlugin);

    WPluginInterface * interface = reinterpret_cast<WPluginInterface *> (d->loader->instance());

    if (interface == NULL)
    {
        qWarning("WPlugin::loadEvent: Invalid plugin %s.", d->fileName.C_STR);

        return;
    }

    interface->load();
}

/* virtual */ void WPlugin::unloadEvent()
{
    Q_D(WPlugin);

    WPluginInterface * interface = reinterpret_cast<WPluginInterface *> (d->loader->instance());

    interface->unload();
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

QString WPlugin::fileName() const
{
    Q_D(const WPlugin); return d->fileName;
}

QObject * WPlugin::instance() const
{
    Q_D(const WPlugin); return d->loader->instance();
}

//-------------------------------------------------------------------------------------------------

QString WPlugin::name() const
{
    Q_D(const WPlugin); return d->name;
}

QString WPlugin::version() const
{
    Q_D(const WPlugin); return d->version;
}

QString WPlugin::compatVersion() const
{
    Q_D(const WPlugin); return d->compatVersion;
}

QString WPlugin::description() const
{
    Q_D(const WPlugin); return d->description;
}

QString WPlugin::url() const
{
    Q_D(const WPlugin); return d->url;
}

QString WPlugin::license() const
{
    Q_D(const WPlugin); return d->license;
}

#endif // SK_NO_PLUGIN
