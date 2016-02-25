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

#include "WControllerScript.h"

#ifndef SK_NO_CONTROLLERSCRIPT

// Qt includes
#include <QScriptEngine>
#include <QFile>

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
    QScriptEngine * engine;

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

    engine = new QScriptEngine(q);
}

//-------------------------------------------------------------------------------------------------
// Private ctor / dtor
//-------------------------------------------------------------------------------------------------

WControllerScript::WControllerScript()
    : WController(new WControllerScriptPrivate(this), "WControllerScript") {}

//-------------------------------------------------------------------------------------------------
// Initialize
//-------------------------------------------------------------------------------------------------

void WControllerScript::init()
{
    Q_D(WControllerScript); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ bool WControllerScript::loadPlugin(const QString & extension)
{
    Q_D(WControllerScript);

    QScriptValue value;

    value = d->engine->importExtension(extension);

    if (value.isError())
    {
        if (d->engine->hasUncaughtException())
        {
             qWarning("WControllerScript::loadPlugin: Failed to load script plugin %s. %s.",
                      extension.C_STR, value.toString().C_STR);
        }
        else qWarning("WControllerScript::loadPlugin: Failed to load script plugin %s.",
                      extension.C_STR);

        return false;
    }
    else
    {
        qWarning("Plugin loaded %s.", extension.C_STR);

        return true;
    }
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ bool WControllerScript::importFile(const QString & fileName)
{
    Q_D(WControllerScript);

    QFile file(fileName);

    if (file.open(QIODevice::ReadOnly) == false) return false;

    return d->engine->evaluate(file.readAll(), fileName).isValid();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ QScriptValue WControllerScript::parseJson(const QByteArray & data) const
{
    Q_D(const WControllerScript);

    QScriptValueList params;

    params << QString(data);

    return d->engine->evaluate("JSON.parse").call(QScriptValue(), params);
}

//-------------------------------------------------------------------------------------------------
// Private properties
//-------------------------------------------------------------------------------------------------

QScriptEngine * WControllerScript::engine()
{
    Q_D(WControllerScript); return d->engine;
}

#endif // SK_NO_CONTROLLERSCRIPT
