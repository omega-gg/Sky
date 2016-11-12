//=================================================================================================
/*
    Copyright (C) 2015-2016 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkGui module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#ifndef WABSTRACTVIEW_P_H
#define WABSTRACTVIEW_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

// Sk includes
#include <private/Sk_p>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#ifndef SK_NO_ABSTRACTVIEW

class SK_GUI_EXPORT WAbstractViewPrivate : public WPrivate
{
public:
    WAbstractViewPrivate(WAbstractView * p);

#ifdef Q_OS_WIN
    /* virtual */ ~WAbstractViewPrivate();
#endif

    void init(Qt::WindowFlags flags);

#ifdef Q_OS_WIN
public: // Events
    static LRESULT CALLBACK events(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
#endif

public: // Variables
    Qt::WindowFlags flags;

#ifdef Q_OS_WIN
    HWND id;

    HWND      handle;
    HINSTANCE instance;

    int x;
    int y;

    int width;
    int height;

    qreal opacity;
#endif

protected:
    W_DECLARE_PUBLIC(WAbstractView)
};

#endif // SK_NO_ABSTRACTVIEW
#endif // WABSTRACTVIEW_P_H
