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

#include "WMainViewResizer.h"

// Sk includes
#include <WControllerView>
#include <WResizer>

#ifndef SK_NO_MAINVIEWRESIZER

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include <private/WDeclarativeItem_p>

class SK_GUI_EXPORT WMainViewResizerPrivate : public WDeclarativeItemPrivate
{
protected:
    WMainViewResizerPrivate(WMainViewResizer * p);

    void init();

public: // Functions
    void refreshCoordinates();

public: // Variables
    WResizer * topLeft;
    WResizer * topRight;
    WResizer * bottomLeft;
    WResizer * bottomRight;
    WResizer * left;
    WResizer * right;
    WResizer * top;
    WResizer * bottom;

protected:
    W_DECLARE_PUBLIC(WMainViewResizer)
};

//-------------------------------------------------------------------------------------------------

WMainViewResizerPrivate::WMainViewResizerPrivate(WMainViewResizer * p)
    : WDeclarativeItemPrivate(p) {}

void WMainViewResizerPrivate::init()
{
    Q_Q(WMainViewResizer);

    topLeft     = new WResizer(WResizer::TopLeft,     q);
    topRight    = new WResizer(WResizer::TopRight,    q);
    bottomLeft  = new WResizer(WResizer::BottomLeft,  q);
    bottomRight = new WResizer(WResizer::BottomRight, q);
    left        = new WResizer(WResizer::Left,        q);
    right       = new WResizer(WResizer::Right,       q);
    top         = new WResizer(WResizer::Top,         q);
    bottom      = new WResizer(WResizer::Bottom,      q);

    refreshCoordinates();
}

//-------------------------------------------------------------------------------------------------

void WMainViewResizerPrivate::refreshCoordinates()
{
    Q_Q(WMainViewResizer);

    qreal width  = q->width();
    qreal height = q->height();

    //---------------------------------------------------------------------------------------------
    // Position

    qreal sizeX = width  - 8;
    qreal sizeY = height - 8;

    topLeft    ->setPos(QPointF(0,     0));
    topRight   ->setPos(QPointF(sizeX, 0));
    bottomLeft ->setPos(QPointF(0,     sizeY));
    bottomRight->setPos(QPointF(sizeX, sizeY));

    sizeX = width  - 4;
    sizeY = height - 4;

    left  ->setPos(QPointF(0,     8));
    right ->setPos(QPointF(sizeX, 8));
    top   ->setPos(QPointF(8,     0));
    bottom->setPos(QPointF(8,     sizeY));

    //---------------------------------------------------------------------------------------------
    // Size

    topLeft    ->setSize(QSizeF(8, 8));
    topRight   ->setSize(QSizeF(8, 8));
    bottomLeft ->setSize(QSizeF(8, 8));
    bottomRight->setSize(QSizeF(8, 8));

    sizeX = width  - 16;
    sizeY = height - 16;

    left  ->setSize(QSizeF(4,     sizeY));
    right ->setSize(QSizeF(4,     sizeY));
    top   ->setSize(QSizeF(sizeX, 4));
    bottom->setSize(QSizeF(sizeX, 4));
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

/* explicit */ WMainViewResizer::WMainViewResizer(QDeclarativeItem * parent)
    : WDeclarativeItem(new WMainViewResizerPrivate(this), parent)
{
    Q_D(WMainViewResizer); d->init();
}

//-------------------------------------------------------------------------------------------------
// Protected QDeclarativeItem reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WMainViewResizer::geometryChanged(const QRectF & newGeometry,
                                                     const QRectF & oldGeometry)
{
    Q_D(WMainViewResizer);

    WDeclarativeItem::geometryChanged(newGeometry, oldGeometry);

    d->refreshCoordinates();
}

#endif // SK_NO_MAINVIEWRESIZER
