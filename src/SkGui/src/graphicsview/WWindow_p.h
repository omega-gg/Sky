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

#ifndef WWINDOW_P_H
#define WWINDOW_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

// Private includes
#ifdef QT_NEW
#include <private/WView_p>
#endif
#include <private/WDeclarativeMouseArea_p>

#ifndef SK_NO_WINDOW

//-------------------------------------------------------------------------------------------------
// WViewportPrivate
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WViewportPrivate : public WDeclarativeMouseAreaPrivate
{
public:
    WViewportPrivate(WViewport * p);

    void init();

protected:
    W_DECLARE_PUBLIC(WViewport)
};

//-------------------------------------------------------------------------------------------------
// WWindowPrivate
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
class SK_GUI_EXPORT WWindowPrivate : public WViewportPrivate
#else
class SK_GUI_EXPORT WWindowPrivate : public WViewPrivate
#endif
{
public:
    WWindowPrivate(WWindow * p);

    /* virtual */ ~WWindowPrivate();

    void init();

#ifdef QT_NEW
public: // Functions
    void deleteItems();
#endif

public: // Variables
#ifdef QT_4
    WDeclarativeMouseArea * viewport;
#else
    WViewport * viewport;

    QList<QObject *> items;
#endif

    QString icon;

    bool visible;

protected:
    W_DECLARE_PUBLIC(WWindow)
};

#endif // SK_NO_WINDOW
#endif // WWINDOW_P_H
