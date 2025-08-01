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

// Sk includes
#include <WApplication>

// Application includes
#include "ControllerCore.h"

//-------------------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------------------

int main(int argc, char * argv[])
{
#ifdef QT_4
    QApplication * application = WApplication::create(argc, argv);
#else
    QGuiApplication * application = WApplication::create(argc, argv);
#endif

    if (application == NULL) return 0;

    //---------------------------------------------------------------------------------------------
    // Controllers

    W_CREATE_CONTROLLER(ControllerCore);

#ifdef SK_DESKTOP
    core->applyArguments(argc, argv);
#endif

    //---------------------------------------------------------------------------------------------

#ifndef SK_DEPLOY
    sk->setQrc(false);
#endif

    sk->startScript();

    return application->exec();
}
