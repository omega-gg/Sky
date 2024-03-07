//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkBarcode.

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

#include "WDeclarativeScannerHover.h"

#ifndef SK_NO_DECLARATIVESCANNERHOVER

// Qt includes
#include <QPainter>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include "WDeclarativeScannerHover_p.h"

WDeclarativeScannerHoverPrivate::WDeclarativeScannerHoverPrivate(WDeclarativeScannerHover * p)
#ifdef QT_4
    : WDeclarativeItemPrivate(p) {}
#else
    : WDeclarativeItemPaintPrivate(p) {}
#endif

void WDeclarativeScannerHoverPrivate::init()
{
    active = false;

#ifdef QT_4
    Q_Q(WDeclarativeScannerHover);

    q->setFlag(QGraphicsItem::ItemHasNoContents, false);
#endif
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
/* explicit */ WDeclarativeScannerHover::WDeclarativeScannerHover(QDeclarativeItem * parent)
#else
/* explicit */ WDeclarativeScannerHover::WDeclarativeScannerHover(QQuickItem * parent)
#endif
#ifdef QT_4
    : WDeclarativeItem(new WDeclarativeScannerHoverPrivate(this), parent)
#else
    : WDeclarativeItemPaint(new WDeclarativeScannerHoverPrivate(this), parent)
#endif
{
    Q_D(WDeclarativeScannerHover); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

void WDeclarativeScannerHover::apply(const QPointF & topLeft,    const QPointF & topRight,
                                     const QPointF & bottomLeft, const QPointF & bottomRight)
{
    Q_D(WDeclarativeScannerHover);

    if (d->topLeft == topLeft && d->topRight == topRight
        &&
        d->bottomLeft == bottomLeft && d->bottomRight == bottomRight) return;

    d->topLeft     = topLeft;
    d->topRight    = topRight;
    d->bottomLeft  = bottomLeft;
    d->bottomRight = bottomRight;

    if (d->active == false)
    {
        d->active = true;

        update();

        emit activeChanged();
    }
    else update();
}

void WDeclarativeScannerHover::clear()
{
    Q_D(WDeclarativeScannerHover);

    if (d->active == false) return;

    d->topLeft     = QPointF();
    d->topRight    = QPointF();
    d->bottomLeft  = QPointF();
    d->bottomRight = QPointF();

    d->active = false;

    update();

    emit activeChanged();
}

//-------------------------------------------------------------------------------------------------
// QGraphicsItem / QQuickPaintedItem reimplementation
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
/* virtual */ void WDeclarativeScannerHover::paint(QPainter                       * painter,
                                                   const QStyleOptionGraphicsItem * option,
                                                   QWidget                        *)
#else
/* virtual */ void WDeclarativeScannerHover::paint(QPainter * painter)
#endif
{
    Q_D(WDeclarativeScannerHover);

    if (d->active == false) return;

    painter->setRenderHint(QPainter::Antialiasing);

    painter->setPen(QPen(d->color));

    painter->setBrush(QBrush(d->color));

    QPointF points[4] = { d->topLeft, d->topRight, d->bottomRight, d->bottomLeft };

    painter->drawPolygon(points, 4);
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

bool WDeclarativeScannerHover::isActive() const
{
    Q_D(const WDeclarativeScannerHover); return d->active;
}

QColor WDeclarativeScannerHover::color() const
{
    Q_D(const WDeclarativeScannerHover); return d->color;
}

void WDeclarativeScannerHover::setColor(const QColor & color)
{
    Q_D(WDeclarativeScannerHover);

    if (d->color== color) return;

    d->color = color;

    update();

    emit colorChanged();
}

#endif // SK_NO_DECLARATIVESCANNERHOVER
