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

#include "WControllerDeclarative.h"

#ifndef SK_NO_CONTROLLERDECLARATIVE

// Qt includes
#include <QCoreApplication>
#include <QDeclarativeEngine>
#include <QDeclarativeContext>
#include <QDir>

W_INIT_CONTROLLER(WControllerDeclarative)

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include <private/WController_p>

class WControllerDeclarativePrivate : public WControllerPrivate
{
public:
    WControllerDeclarativePrivate(WControllerDeclarative * p);

    /* virtual */ ~WControllerDeclarativePrivate();

    void init();

public: // Variables
    QDeclarativeEngine * engine;

protected:
    W_DECLARE_PUBLIC(WControllerDeclarative)
};

//-------------------------------------------------------------------------------------------------

WControllerDeclarativePrivate::WControllerDeclarativePrivate(WControllerDeclarative * p)
    : WControllerPrivate(p) {}

/* virtual */ WControllerDeclarativePrivate::~WControllerDeclarativePrivate()
{
    W_CLEAR_CONTROLLER(WControllerDeclarative);
}

//-------------------------------------------------------------------------------------------------

void WControllerDeclarativePrivate::init()
{
    engine = new QDeclarativeEngine(qApp);

    engine->addImportPath(QDir::currentPath());
    engine->addPluginPath(QDir::currentPath());
}

//-------------------------------------------------------------------------------------------------
// Private ctor / dtor
//-------------------------------------------------------------------------------------------------

WControllerDeclarative::WControllerDeclarative()
    : WController(new WControllerDeclarativePrivate(this)) {}

//-------------------------------------------------------------------------------------------------
// Initialize
//-------------------------------------------------------------------------------------------------

/* virtual */ void WControllerDeclarative::init()
{
    Q_D(WControllerDeclarative); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

void WControllerDeclarative::clearComponentCache()
{
    Q_D(WControllerDeclarative);

    d->engine->clearComponentCache();
}

void WControllerDeclarative::setContextProperty(const QString & name, const QVariant & value)
{
    Q_D(WControllerDeclarative);

    d->engine->rootContext()->setContextProperty(name, value);
}

void WControllerDeclarative::setContextProperty(const QString & name, QObject * value)
{
    Q_D(WControllerDeclarative);

    d->engine->rootContext()->setContextProperty(name, value);
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

QDeclarativeEngine * WControllerDeclarative::engine() const
{
    Q_D(const WControllerDeclarative); return d->engine;
}

#endif // SK_NO_CONTROLLERDECLARATIVE
