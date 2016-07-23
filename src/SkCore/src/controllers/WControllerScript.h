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

#ifndef WCONTROLLERSCRIPT_H
#define WCONTROLLERSCRIPT_H

// Qt includes
#include <QScriptEngine>
#include <QScriptValue>

// Sk includes
#include <WController>
#include <WScript>

#ifndef SK_NO_CONTROLLERSCRIPT

// Forward declarations
class WControllerScriptPrivate;
class QScriptEngine;

// Defines
#define wControllerScript WControllerScript::instance()

class SK_CORE_EXPORT WControllerScript : public WController
{
    Q_OBJECT

private:
    WControllerScript();

protected: // Initialize
    /* virtual */ void init();

public: // Interface
    Q_INVOKABLE bool loadPlugin(const QString & extension);

    Q_INVOKABLE bool importFile(const QString & fileName);

    Q_INVOKABLE QScriptValue parseJson(const QByteArray & data) const;

public: // Properties
    QScriptEngine * engine();

private:
    W_DECLARE_PRIVATE   (WControllerScript)
    W_DECLARE_CONTROLLER(WControllerScript)

    friend class WScriptPrivate;
};

#endif // SK_NO_CONTROLLERSCRIPT
#endif // WCONTROLLERSCRIPT_H
