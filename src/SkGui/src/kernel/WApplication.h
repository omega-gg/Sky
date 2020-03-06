//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkGui module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#ifndef WAPPLICATION_H
#define WAPPLICATION_H

// Qt includes
#include <QApplication>

// Sk includes
#include <WControllerApplication>

#ifndef SK_NO_APPLICATION

class SK_GUI_EXPORT WApplication : public QApplication
{
public: // Static functions
    static QApplication * create(int & argc, char ** argv, Sk::Type type = Sk::Single);

protected: // Static functions
    static QApplication * createApplication(int & argc, char ** argv, Sk::Type type);
};

#endif // SK_NO_APPLICATION
#endif // WAPPLICATION_H
