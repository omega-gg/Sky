//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkGui module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#include "WDeclarativeApplication.h"

#ifndef SK_NO_DECLARATIVEAPPLICATION

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include <private/WDeclarativeCoreApplication_p>

class SK_GUI_EXPORT WDeclarativeApplicationPrivate : public WDeclarativeCoreApplicationPrivate
{
public:
    WDeclarativeApplicationPrivate(WDeclarativeApplication * p);

    void init();

protected:
    W_DECLARE_PUBLIC(WDeclarativeApplication)
};

//-------------------------------------------------------------------------------------------------

WDeclarativeApplicationPrivate::WDeclarativeApplicationPrivate(WDeclarativeApplication * p)
    : WDeclarativeCoreApplicationPrivate(p) {}

void WDeclarativeApplicationPrivate::init() {}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

/* explicit */ WDeclarativeApplication::WDeclarativeApplication(QObject * parent)
    : WDeclarativeCoreApplication(new WDeclarativeApplicationPrivate(this), parent)
{
    Q_D(WDeclarativeApplication); d->init();
}

#endif // SK_NO_DECLARATIVEAPPLICATION
