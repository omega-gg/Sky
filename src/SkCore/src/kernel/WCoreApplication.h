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

#ifndef WCOREAPPLICATION_H
#define WCOREAPPLICATION_H

// Qt includes
#include <QCoreApplication>

// Sk includes
#include <Sk>

#ifndef SK_NO_COREAPPLICATION

class SK_CORE_EXPORT WCoreApplication : public QCoreApplication
{
public: // Static functions
    static QCoreApplication * create(int & argc, char ** argv);
};

#endif // SK_NO_COREAPPLICATION
#endif // WCOREAPPLICATION_H
