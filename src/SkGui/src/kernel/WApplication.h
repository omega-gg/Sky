//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkGui.

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

#ifndef WAPPLICATION_H
#define WAPPLICATION_H

// Qt includes
#include <QApplication>
#ifdef QT_6
#include <QQuickWindow>
#endif

// Sk includes
#include <WControllerApplication>

#ifndef SK_NO_APPLICATION

class SK_GUI_EXPORT WApplication : public QApplication
{
public: // Static functions
#ifdef QT_OLD
    static QApplication * create(int & argc, char ** argv, Sk::Type type = Sk::Single);
#else
    // NOTE Qt: Passing a Null api will set the default renderer.
    static
    QApplication * create(int & argc, char ** argv, Sk::Type type = Sk::Single,
                          QSGRendererInterface::GraphicsApi api = QSGRendererInterface::Null);
#endif

protected: // Static functions
    static QApplication * createApplication(int & argc, char ** argv, Sk::Type type);
};

#endif // SK_NO_APPLICATION
#endif // WAPPLICATION_H
