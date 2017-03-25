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

#include "WCoreApplication.h"

#ifndef SK_NO_COREAPPLICATION

// Sk includes
#include <WControllerApplication>

//-------------------------------------------------------------------------------------------------
// Static functions
//-------------------------------------------------------------------------------------------------

/* static */ QCoreApplication * WCoreApplication::create(int & argc, char ** argv)
{
    QCoreApplication * application = new QCoreApplication(argc, argv);

    W_CREATE_CONTROLLER(WControllerApplication);

    sk->d_func()->initApplication(application, Sk::Multiple, false);

    return application;
}

#endif // SK_NO_COREAPPLICATION
