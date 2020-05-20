//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of Sky kit.

    - GNU Lesser General Public License Usage:
    This file may be used under the terms of the GNU Lesser General Public License version 3 as
    published by the Free Software Foundation and appearing in the LICENSE.md file included in the
    packaging of this file. Please review the following information to ensure the GNU Lesser
    General Public License requirements will be met: https://www.gnu.org/licenses/lgpl.html.
*/
//=================================================================================================

#include "WControllerScript.h"

#ifndef SK_NO_CONTROLLERSCRIPT

// Qt includes
#ifdef QT_4
#include <QScriptEngine>
#else
#include <QJSEngine>
#endif

W_INIT_CONTROLLER(WControllerScript)

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include <private/WController_p>

class WControllerScriptPrivate : public WControllerPrivate
{
public:
    WControllerScriptPrivate(WControllerScript * p);

    /* virtual */ ~WControllerScriptPrivate();

    void init();

private: // Variables
#ifdef QT_4
    QScriptEngine * engine;
#else
    QJSEngine * engine;
#endif

protected:
    W_DECLARE_PUBLIC(WControllerScript)
};

//-------------------------------------------------------------------------------------------------

WControllerScriptPrivate::WControllerScriptPrivate(WControllerScript * p)
    : WControllerPrivate(p) {}

/* virtual */ WControllerScriptPrivate::~WControllerScriptPrivate()
{
    W_CLEAR_CONTROLLER(WControllerScript);
}

//-------------------------------------------------------------------------------------------------

void WControllerScriptPrivate::init()
{
    Q_Q(WControllerScript);

#ifdef QT_4
    engine = new QScriptEngine(q);
#else
    engine = new QJSEngine(q);
#endif
}

//-------------------------------------------------------------------------------------------------
// Private ctor / dtor
//-------------------------------------------------------------------------------------------------

WControllerScript::WControllerScript() : WController(new WControllerScriptPrivate(this)) {}

//-------------------------------------------------------------------------------------------------
// Initialize
//-------------------------------------------------------------------------------------------------

/* virtual */ void WControllerScript::init()
{
    Q_D(WControllerScript); d->init();
}

//-------------------------------------------------------------------------------------------------
// Private properties
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
QScriptEngine * WControllerScript::engine()
#else
QJSEngine * WControllerScript::engine()
#endif
{
    Q_D(WControllerScript); return d->engine;
}

#endif // SK_NO_CONTROLLERSCRIPT
