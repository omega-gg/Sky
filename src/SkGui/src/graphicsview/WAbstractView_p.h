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

#ifndef WABSTRACTVIEW_P_H
#define WABSTRACTVIEW_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

#ifdef SK_WIN_NATIVE
// Qt includes
#include <QMetaMethod>
#ifdef QT_LATEST
#include <QTimer>
#endif
#endif

#if defined(SK_WIN_NATIVE) && defined(QT_4)
// Windows includes
#include <qt_windows.h>
#endif

// Private includes
#include <private/Sk_p>

#ifndef SK_NO_ABSTRACTVIEW

class SK_GUI_EXPORT WAbstractViewPrivate : public WPrivate
{
public:
    WAbstractViewPrivate(WAbstractView * p);

#ifdef SK_WIN_NATIVE
    /* virtual */ ~WAbstractViewPrivate();
#endif

    void init(Qt::WindowFlags flags);

#ifdef SK_WIN_NATIVE
public: // Functions
    void applyFullScreen  ();
    void restoreFullScreen();

    void setFlag(LONG flag, bool enabled) const;

    bool isWindows10();

public: // Static functions
    static HICON getIcon(const QIcon & icon, int width, int height);

public: // Static events
    static LRESULT CALLBACK events(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);

public: // Slots
#ifdef QT_LATEST
    void onCreate();

    void onMove();
#endif

    void onFocus();
#endif

public: // Variables
    Qt::WindowFlags flags;

#ifdef SK_WIN_NATIVE
    HWND id;
    HWND handle;

#ifdef QT_LATEST
    QWindow * viewport;
#endif

    int x;
    int y;

    int width;
    int height;

    int minimumWidth;
    int minimumHeight;

    int maximumWidth;
    int maximumHeight;

    QRect rect;

    bool  visible;
    qreal opacity;

    bool maximized;
    bool fullScreen;

    bool windowSnap;
    bool windowMaximize;
    bool windowClip;

    int version;

    QMetaMethod method;

#ifdef QT_LATEST
    QTimer timer;
#endif
#endif

protected:
    W_DECLARE_PUBLIC(WAbstractView)
};

#endif // SK_NO_ABSTRACTVIEW
#endif // WABSTRACTVIEW_P_H
