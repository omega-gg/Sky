//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkCore module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#include "WScript.h"

#ifndef SK_NO_SCRIPT

// Qt includes
#include <QScriptEngine>
#include <QFile>

// Sk includes
#include <WControllerScript.h>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include "WScript_p.h"

WScriptPrivate::WScriptPrivate(WScript * p) : WPrivate(p) {}

void WScriptPrivate::init(QScriptEngine * engine)
{
    W_CONTROLLER(WControllerScript, controller);

    if (engine == NULL) this->engine = controller->engine();
    else                this->engine = engine;
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

WScript::WScript(QScriptEngine * engine, QObject * parent)
    : QObject(parent), WPrivatable(new WScriptPrivate(this))
{
    Q_D(WScript); d->init(engine);
}

//-------------------------------------------------------------------------------------------------
// Protected

WScript::WScript(WScriptPrivate * p, QScriptEngine * engine, QObject * parent)
    : QObject(parent), WPrivatable(p)
{
    Q_D(WScript); d->init(engine);
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* virtual */ bool WScript::load(const QString & fileName)
{
    Q_D(WScript);

    qDebug("Loading script %s", fileName.C_STR);

    QFile scriptFile(fileName);

    if (scriptFile.open(QIODevice::ReadOnly) == false)
    {
        qWarning("WScript::load: Failed to load script. Cannot open file %s", fileName.C_STR);

        return false;
    }

    d->fileName = fileName;

    d->script = d->engine->evaluate(scriptFile.readAll(), fileName);

    scriptFile.close();

    return true;
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WScript::setContent(const QString & content)
{
    Q_D(WScript);

    if (d->fileName == "") return;

    QFile scriptFile(d->fileName);

    if (scriptFile.open(QIODevice::WriteOnly) == false)
    {
        qWarning("WScript::setContent: Failed to open file %s.", d->fileName.C_STR);
    }

    scriptFile.write(content.C_STR, content.length());

    scriptFile.close();

    emit scriptUpdated();
}

//-------------------------------------------------------------------------------------------------
// Protected interface
//-------------------------------------------------------------------------------------------------

QScriptValue WScript::construct(const QString & ctorName, const QScriptValueList & args)
{
    Q_D(WScript);

    QScriptValue ctor = d->engine->evaluate(ctorName);

    ctor = ctor.construct(args);

    qDebug("Ctor script output [%s]", ctor.toString().C_STR);

    return ctor;
}

QScriptValue WScript::construct(const QString & ctorName, QObject * param)
{
    Q_D(WScript);

    QScriptValueList args;

    return construct(ctorName, args << d->engine->newQObject(param));
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

QString WScript::fileName() const
{
    Q_D(const WScript); return d->fileName;
}

#endif // SK_NO_SCRIPT
