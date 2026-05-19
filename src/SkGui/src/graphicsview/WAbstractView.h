//=================================================================================================
/*
    Copyright (C) 2015-2026 Sky kit authors. <http://omega.gg/Sky>

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

#ifndef WABSTRACTVIEW_H
#define WABSTRACTVIEW_H

// Qt includes
#ifdef QT_4
#include <QDeclarativeView>
#else
#include <QQuickWindow>
#endif

// Sk includes
#include <Sk>

#ifndef SK_NO_ABSTRACTVIEW

class WAbstractViewPrivate;

#ifdef QT_4
class SK_GUI_EXPORT WAbstractView : public QDeclarativeView, public WPrivatable
#else
class SK_GUI_EXPORT WAbstractView : public QQuickWindow, public WPrivatable
#endif
{
    Q_OBJECT

public:
#ifdef QT_4
    WAbstractView(QWidget * parent = NULL, Qt::WindowFlags flags = Qt::Widget);
#else
    WAbstractView(QWindow * parent = NULL, Qt::WindowFlags flags = Qt::Widget);
#endif
protected:
#ifdef QT_4
    WAbstractView(WAbstractViewPrivate * p,
                  QWidget * parent = NULL, Qt::WindowFlags flags = Qt::Widget);
#else
    WAbstractView(WAbstractViewPrivate * p,
                  QWindow * parent = NULL, Qt::WindowFlags flags = Qt::Widget);
#endif

public: // Interface
#ifdef Q_OS_WIN
    // Q_INVOKABLE void setWindowSnap    (bool enabled);
    // Q_INVOKABLE void setWindowMaximize(bool enabled);
    // Q_INVOKABLE void setWindowClip    (bool enabled);
#endif

private:
    W_DECLARE_PRIVATE(WAbstractView)
};

#include <private/WAbstractView_p>

#endif // SK_NO_ABSTRACTVIEW
#endif // WABSTRACTVIEW_H
