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

#ifndef WCONTROLLERPLUGIN_H
#define WCONTROLLERPLUGIN_H

// Sk includes
#include <WController>
#include <WPlugin>

#ifndef SK_NO_CONTROLLERPLUGIN

// Forward declarations
class QStringList;
class WControllerPluginPrivate;

// Defines
#define wControllerPlugin WControllerPlugin::instance()

class SK_CORE_EXPORT WControllerPlugin : public WController
{
    Q_OBJECT

private:
    WControllerPlugin();

protected: // Initialize
    /* virtual */ void init();

public: // Interface
    // Loading
    bool loadPlugin  (const QString & name);
    bool unloadPlugin(const QString & name);

    // Adding path to watch
    bool addPath(const QString dirPath);

public: // Interface
    // Retrieve plugin from name
    WPlugin * plugin(const QString & name);

    // Plugin file name
    QString pluginFileName(const QString & name);

    // Available plugins
    QStringList availablePlugins() const;

private: // WPlugin interface
    void declarePlugin  (WPlugin * plugin);
    void undeclarePlugin(WPlugin * plugin);

private slots:
    void onFilesCreated(const QString & parentPath, const QStringList & fileNames);
    void onFilesDeleted(const QString & parentPath, const QStringList & fileNames);

signals:
    void pluginDetected  (const QString & name);
    void pluginUndetected(const QString & name);

public: // Properties
    QList<WPlugin *> plugins() const;

private:
    W_DECLARE_PRIVATE   (WControllerPlugin)
    W_DECLARE_CONTROLLER(WControllerPlugin)

    friend class WPluginPrivate;
};

#endif // SK_NO_CONTROLLERPLUGIN
#endif // WCONTROLLERPLUGIN_H
