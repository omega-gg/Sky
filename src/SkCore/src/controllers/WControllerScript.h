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

#ifndef WCONTROLLERSCRIPT_H
#define WCONTROLLERSCRIPT_H

// Sk includes
#include <WController>

#ifndef SK_NO_CONTROLLERSCRIPT

// Forward declarations
#ifdef QT_4
class QScriptEngine;
#else
class QJSEngine;
#endif
class WControllerScriptPrivate;

// Defines
#define wControllerScript WControllerScript::instance()

class SK_CORE_EXPORT WControllerScript : public WController
{
    Q_OBJECT

private:
    WControllerScript();

protected: // Initialize
    /* virtual */ void init();

public: // Properties
#ifdef QT_4
    QScriptEngine * engine();
#else
    QJSEngine * engine();
#endif

private:
    W_DECLARE_PRIVATE   (WControllerScript)
    W_DECLARE_CONTROLLER(WControllerScript)
};

#endif // SK_NO_CONTROLLERSCRIPT
#endif // WCONTROLLERSCRIPT_H
