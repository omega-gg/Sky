//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of Sky kit.

    - GNU Lesser General Public License Usage:
    This file may be used under the terms of the GNU Lesser General Public License version 3 as
    published by the Free Software Foundation and appearing in the LICENSE.md file included in the
    packaging of this file. Please review the following information to ensure the GNU Lesser
    General Public License requirements will be met: https://www.gnu.org/licenses/lgpl.html.
*/
//=================================================================================================

#include "WDeclarativeItemPaint.h"

#ifndef SK_NO_DECLARATIVEITEMPAINT

// Sk includes
#include <WView>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include "WDeclarativeItemPaint_p.h"

WDeclarativeItemPaintPrivate::WDeclarativeItemPaintPrivate(WDeclarativeItemPaint * p)
    : WPrivate(p) {}

void WDeclarativeItemPaintPrivate::init()
{
    Q_Q(WDeclarativeItemPaint);

    view = NULL;

    q->setAntialiasing(false);
    q->setMipmap      (false);
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

/* explicit */ WDeclarativeItemPaint::WDeclarativeItemPaint(QQuickItem * parent)
    : QQuickPaintedItem(parent), WPrivatable(new WDeclarativeItemPaintPrivate(this))
{
    Q_D(WDeclarativeItemPaint); d->init();
}

//-------------------------------------------------------------------------------------------------
// Protected

WDeclarativeItemPaint::WDeclarativeItemPaint(WDeclarativeItemPaintPrivate * p, QQuickItem * parent)
    : QQuickPaintedItem(parent), WPrivatable(p)
{
    Q_D(WDeclarativeItemPaint); d->init();
}

//-------------------------------------------------------------------------------------------------
// Protected QQuickItem reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativeItemPaint::itemChange(ItemChange             change,
                                                     const ItemChangeData & value)
{
    if (change == ItemSceneChange)
    {
        Q_D(WDeclarativeItemPaint);

        d->view = static_cast<WView *> (value.window);

        emit viewChanged();
    }
    else if (change == ItemVisibleHasChanged && value.boolValue)
    {
        update();
    }

    QQuickPaintedItem::itemChange(change, value);
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

WView * WDeclarativeItemPaint::view() const
{
    Q_D(const WDeclarativeItemPaint); return d->view;
}

#endif // SK_NO_DECLARATIVEITEMPAINT
