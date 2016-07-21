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

#include "WController.h"

// Sk includes
#include <WControllerApplication>

// Private includes
#include <private/WControllerApplication_p>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include "WController_p.h"

WControllerPrivate::WControllerPrivate(WController * p) : WPrivate(p) {}

/* virtual */ WControllerPrivate::~WControllerPrivate()
{
    Q_Q(WController); sk->d_func()->undeclareController(q);
}

//-------------------------------------------------------------------------------------------------

void WControllerPrivate::init()
{
    Q_Q(WController);

    isCreated = false;

    sk->d_func()->declareController(q);
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------
// Protected

WController::WController() : QObject(), WPrivatable(new WControllerPrivate(this))
{
    Q_D(WController); d->init();
}

WController::WController(WControllerPrivate * p) : QObject(), WPrivatable(p)
{
    Q_D(WController); d->init();
}

WController::WController(WControllerApplicationPrivate * p) : QObject(), WPrivatable(p)
{
    Q_D(WController); d->isCreated = false;
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

void WController::initController()
{
    Q_D(WController);

    if (d->isCreated == false)
    {
        d->isCreated = true;

        init();
    }
}

//-------------------------------------------------------------------------------------------------
// Protected functions
//-------------------------------------------------------------------------------------------------

/* virtual */ void WController::init() {}
