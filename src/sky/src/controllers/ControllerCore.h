//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the Sky kit runtime.

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

#ifndef CONTROLLERCORE_H
#define CONTROLLERCORE_H

// Qt includes
#include <QImage>

// Sk includes
#include <WController>
#include <WFileWatcher>

// Defines
#define core ControllerCore::instance()

// Forward declarations
class WControllerFileReply;
class WBackendIndex;
class WDeclarativePlayer;

class ControllerCore : public WController
{
    Q_OBJECT

#ifdef SK_DESKTOP
    Q_PROPERTY(QString argument READ argument CONSTANT)
#endif

private:
    ControllerCore();

public: // Interface
#ifdef SK_DESKTOP
    Q_INVOKABLE void applyArguments(int & argc, char ** argv);
#endif

    Q_INVOKABLE void load();

    Q_INVOKABLE void updateBackends() const;
    Q_INVOKABLE void resetBackends () const;

    Q_INVOKABLE void clearComponentCache() const;

public: // Static functions
    Q_INVOKABLE static void applyBackend(WDeclarativePlayer * player);

    Q_INVOKABLE static QImage generateTagSource(const QString & source);
    Q_INVOKABLE static QImage generateTagPath  (const QString & path);

    Q_INVOKABLE static bool renameFile(const QString & oldPath, const QString & newPath);

private: // Functions
    void createIndex();

    WControllerFileReply * copyBackends(const QString & path) const;

private slots:
    void onLoaded     ();
    void onIndexLoaded();

    void onReload();

public: // Properties
#ifdef SK_DESKTOP
    QString argument() const;
#endif

private: // Variables
#ifdef SK_DESKTOP
    QString _argument;
#endif

    QString _path;

    WBackendIndex * _index;

    WFileWatcher _watcher;

private:
    Q_DISABLE_COPY      (ControllerCore)
    W_DECLARE_CONTROLLER(ControllerCore)
};

#endif // CONTROLLERCORE_H
