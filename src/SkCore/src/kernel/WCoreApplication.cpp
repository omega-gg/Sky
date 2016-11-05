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

#include "WCoreApplication.h"

#ifndef SK_NO_COREAPPLICATION

/*!
    \class WCoreApplication

    \brief The WCoreApplication class provides an event loop for console Sk applications.

    \ingroup application
    \mainclass

    This class is used by non-GUI applications to provide their event
    loop. For non-GUI application that uses Sk, there should be exactly
    one WCoreApplication object. For GUI applications, see
    wApplication.

    \sa QCoreApplication, WApplication
*/

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include <private/Sk_p>

class SK_CORE_EXPORT WCoreApplicationPrivate : public WPrivate
{
public:
    WCoreApplicationPrivate(WCoreApplication * p);

    void init();

protected:
    W_DECLARE_PUBLIC(WCoreApplication)
};

//-------------------------------------------------------------------------------------------------

WCoreApplicationPrivate::WCoreApplicationPrivate(WCoreApplication * p) : WPrivate(p) {}

void WCoreApplicationPrivate::init() {}

//-------------------------------------------------------------------------------------------------
// Private ctor / dtor
//-------------------------------------------------------------------------------------------------

/*!
    Constructs a Sk kernel application. Kernel applications are
    applications without a graphical user interface. These type of
    applications are used at the console or as server processes.

    \warning The data pointed to by \a argc and \a argv must stay
    valid for the entire lifetime of the WCoreApplication object.
*/

WCoreApplication::WCoreApplication(int & argc, char ** argv)
    : QCoreApplication(argc, argv), WPrivatable(new WCoreApplicationPrivate(this))
{
    Q_D(WCoreApplication); d->init();
}

//-------------------------------------------------------------------------------------------------
// Static functions
//-------------------------------------------------------------------------------------------------

/* static */ QCoreApplication * WCoreApplication::create(int & argc, char ** argv, Sk::Type type)
{
    QCoreApplication * application = new QCoreApplication(argc, argv);

    W_CREATE_CONTROLLER(WControllerApplication);

    sk->d_func()->createApplication(application, argc, argv, type, true);

    return application;
}

#endif // SK_NO_COREAPPLICATION
