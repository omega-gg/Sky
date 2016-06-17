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

#include "WDeclarativeBorders.h"

#ifndef SK_NO_DECLARATIVEBORDERS

// Qt includes
#include <QPainter>

// Private includes
#include <private/WDeclarativeItem_p>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WDeclarativeBordersPrivate : public WDeclarativeItemPrivate
{
public:
    WDeclarativeBordersPrivate(WDeclarativeBorders * p);

    void init();

public: // Variables
    qreal left;
    qreal right;
    qreal top;
    qreal bottom;

    QColor color;

protected:
    W_DECLARE_PUBLIC(WDeclarativeBorders)
};

//-------------------------------------------------------------------------------------------------

WDeclarativeBordersPrivate::WDeclarativeBordersPrivate(WDeclarativeBorders * p)
    : WDeclarativeItemPrivate(p) {}

void WDeclarativeBordersPrivate::init()
{
    Q_Q(WDeclarativeBorders);

    left   = 0.0;
    right  = 0.0;
    top    = 0.0;
    bottom = 0.0;

    q->setFlag(QGraphicsItem::ItemHasNoContents, false);
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

/* explicit */ WDeclarativeBorders::WDeclarativeBorders(QDeclarativeItem * parent)
    : WDeclarativeItem(new WDeclarativeBordersPrivate(this), parent)
{
    Q_D(WDeclarativeBorders); d->init();
}

//-------------------------------------------------------------------------------------------------
// QGraphicsItem reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativeBorders::paint(QPainter * painter,
                                              const QStyleOptionGraphicsItem *, QWidget *)
{
    Q_D(WDeclarativeBorders);

    painter->setPen(Qt::NoPen);

    painter->setBrush(d->color);

    painter->drawRect(QRectF(0,                  0, d->left,  height()));
    painter->drawRect(QRectF(width() - d->right, 0, d->right, height()));

    painter->drawRect(QRectF(0,                    0, width(), d->top));
    painter->drawRect(QRectF(0, height() - d->bottom, width(), d->bottom));
}

//-------------------------------------------------------------------------------------------------
// properties
//-------------------------------------------------------------------------------------------------

qreal WDeclarativeBorders::left() const
{
    Q_D(const WDeclarativeBorders); return d->left;
}

void WDeclarativeBorders::setLeft(qreal left)
{
    Q_D(WDeclarativeBorders);

    if (d->left == left) return;

    d->left = left;

    update();

    emit leftChanged();
}

//-------------------------------------------------------------------------------------------------

qreal WDeclarativeBorders::right() const
{
    Q_D(const WDeclarativeBorders); return d->right;
}

void WDeclarativeBorders::setRight(qreal right)
{
    Q_D(WDeclarativeBorders);

    if (d->right == right) return;

    d->right = right;

    update();

    emit rightChanged();
}

//-------------------------------------------------------------------------------------------------

qreal WDeclarativeBorders::top() const
{
    Q_D(const WDeclarativeBorders); return d->top;
}

void WDeclarativeBorders::setTop(qreal top)
{
    Q_D(WDeclarativeBorders);

    if (d->top == top) return;

    d->top = top;

    update();

    emit topChanged();
}

//-------------------------------------------------------------------------------------------------

qreal WDeclarativeBorders::bottom() const
{
    Q_D(const WDeclarativeBorders); return d->bottom;
}

void WDeclarativeBorders::setBottom(qreal bottom)
{
    Q_D(WDeclarativeBorders);

    if (d->bottom == bottom) return;

    d->bottom = bottom;

    update();

    emit bottomChanged();
}

//-------------------------------------------------------------------------------------------------

QColor WDeclarativeBorders::color() const
{
    Q_D(const WDeclarativeBorders); return d->color;
}

void WDeclarativeBorders::setColor(const QColor & color)
{
    Q_D(WDeclarativeBorders);

    if (d->color == color) return;

    d->color = color;

    update();

    emit colorChanged();
}

#endif // SK_NO_DECLARATIVEBORDERS
