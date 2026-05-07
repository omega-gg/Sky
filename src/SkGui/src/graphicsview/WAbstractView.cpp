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

#include "WAbstractView.h"

#ifndef SK_NO_ABSTRACTVIEW

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

WAbstractViewPrivate::WAbstractViewPrivate(WAbstractView * p) : WPrivate(p) {}

//-------------------------------------------------------------------------------------------------

void WAbstractViewPrivate::init(Qt::WindowFlags flags)
{
    Q_Q(WAbstractView);

    this->flags = flags;

#ifndef SK_WINDOW_NATIVE
#ifdef QT_4
    q->setWindowFlags(flags);
#else
    q->setFlags(Qt::Window | flags);
#endif
#endif
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
WAbstractView::WAbstractView(QWidget * parent, Qt::WindowFlags flags)
    : QDeclarativeView(parent), WPrivatable(new WAbstractViewPrivate(this))
#else
WAbstractView::WAbstractView(QWindow * parent, Qt::WindowFlags flags)
    : QQuickWindow(parent), WPrivatable(new WAbstractViewPrivate(this))
#endif
{
    Q_D(WAbstractView); d->init(flags);
}

//-------------------------------------------------------------------------------------------------
// Protected

#ifdef QT_4
WAbstractView::WAbstractView(WAbstractViewPrivate * p, QWidget * parent, Qt::WindowFlags flags)
    : QDeclarativeView(parent), WPrivatable(p)
#else
WAbstractView::WAbstractView(WAbstractViewPrivate * p, QWindow * parent, Qt::WindowFlags flags)
    : QQuickWindow(parent), WPrivatable(p)
#endif
{
    Q_D(WAbstractView); d->init(flags);
}

#endif // SK_NO_ABSTRACTVIEW
