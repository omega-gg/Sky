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
#include <QtGlobal>
#if QT_VERSION < QT_VERSION_CHECK(5, 10, 0)
#include <QApplication>
#else
#include <QGuiApplication>
#ifdef QT_6
    #include <QQuickWindow>
#endif
#endif

// Sk includes
#include <WControllerApplication>

#ifndef SK_NO_APPLICATION

class SK_GUI_EXPORT WApplication
{
public: // Static functions
#ifdef QT_4
    static QApplication * create(int & argc, char ** argv, Sk::Type type = Sk::Single);
#elif defined(QT_5)
    static QGuiApplication * create(int & argc, char ** argv, Sk::Type type = Sk::Single);
#else
    // NOTE Qt: Passing a Null api will set the default renderer.
    static
    QGuiApplication * create(int & argc, char ** argv, Sk::Type type = Sk::Single,
                             QSGRendererInterface::GraphicsApi api = QSGRendererInterface::Null);
#endif

protected: // Static functions
#ifdef QT_4
    static QApplication * createApplication(int & argc, char ** argv, Sk::Type type);
#else
    static QGuiApplication * createApplication(int & argc, char ** argv, Sk::Type type);
#endif
};

#endif // SK_NO_APPLICATION
#endif // WAPPLICATION_H
