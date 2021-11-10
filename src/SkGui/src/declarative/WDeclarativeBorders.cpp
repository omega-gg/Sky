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

#include "WDeclarativeBorders.h"

#ifndef SK_NO_DECLARATIVEBORDERS

// Qt includes
#if defined(QT_4) || defined(SK_SOFTWARE)
#include <QPainter>
#else
#include <QSGGeometryNode>
#include <QSGFlatColorMaterial>
#endif

// Private includes
#ifdef SK_SOFTWARE
#include <private/WDeclarativeItemPaint_p>
#else
#include <private/WDeclarativeItem_p>
#endif

#if defined(QT_LATEST) && defined(SK_SOFTWARE) == false

//=================================================================================================
// WDeclarativeBordersLine
//=================================================================================================

class SK_GUI_EXPORT WDeclarativeBordersLine : public QSGGeometryNode
{
public:
    WDeclarativeBordersLine(QSGFlatColorMaterial * material);

public: // Interface
    void setRect(const QRectF & rect);

private: // Variables
    QSGGeometry _geometry;
};

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

WDeclarativeBordersLine::WDeclarativeBordersLine(QSGFlatColorMaterial * material)
    : _geometry(QSGGeometry::defaultAttributes_Point2D(), 4)
{
    setGeometry(&_geometry);

    setMaterial(material);
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

void WDeclarativeBordersLine::setRect(const QRectF & rect)
{
    QSGGeometry::updateRectGeometry(&_geometry, rect);

    markDirty(QSGNode::DirtyGeometry);
}

//=================================================================================================
// WDeclarativeBordersNode
//=================================================================================================

class SK_GUI_EXPORT WDeclarativeBordersNode : public QSGNode
{
public:
    WDeclarativeBordersNode();

public: // Properties
    WDeclarativeBordersLine left;
    WDeclarativeBordersLine right;
    WDeclarativeBordersLine top;
    WDeclarativeBordersLine bottom;

    QSGFlatColorMaterial material;
};

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

WDeclarativeBordersNode::WDeclarativeBordersNode() : left(&material), right (&material),
                                                     top (&material), bottom(&material)
{
    appendChildNode(&left);
    appendChildNode(&right);
    appendChildNode(&top);
    appendChildNode(&bottom);
}

#endif

//=================================================================================================
// WDeclarativeBordersPrivate
//=================================================================================================

#ifdef SK_SOFTWARE
class SK_GUI_EXPORT WDeclarativeBordersPrivate : public WDeclarativeItemPaintPrivate
#else
class SK_GUI_EXPORT WDeclarativeBordersPrivate : public WDeclarativeItemPrivate
#endif
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

#if defined(QT_LATEST) && defined(SK_SOFTWARE) == false
    bool updateLeft;
    bool updateRight;
    bool updateTop;
    bool updateBottom;

    bool updateColor;
#endif

protected:
    W_DECLARE_PUBLIC(WDeclarativeBorders)
};

//-------------------------------------------------------------------------------------------------

WDeclarativeBordersPrivate::WDeclarativeBordersPrivate(WDeclarativeBorders * p)
#ifdef SK_SOFTWARE
    : WDeclarativeItemPaintPrivate(p) {}
#else
    : WDeclarativeItemPrivate(p) {}
#endif

void WDeclarativeBordersPrivate::init()
{
    left   = 0.0;
    right  = 0.0;
    top    = 0.0;
    bottom = 0.0;

#ifdef QT_4
    Q_Q(WDeclarativeBorders);

    q->setFlag(QGraphicsItem::ItemHasNoContents, false);
#elif defined(SK_SOFTWARE) == false
    Q_Q(WDeclarativeBorders);

    updateLeft   = true;
    updateRight  = true;
    updateTop    = true;
    updateBottom = true;

    updateColor = true;

    q->setFlag(QQuickItem::ItemHasContents);
#endif

}

//=================================================================================================
// WDeclarativeBorders
//=================================================================================================

#ifdef QT_4
/* explicit */ WDeclarativeBorders::WDeclarativeBorders(QDeclarativeItem * parent)
#else
/* explicit */ WDeclarativeBorders::WDeclarativeBorders(QQuickItem * parent)
#endif
#ifdef SK_SOFTWARE
    : WDeclarativeItemPaint(new WDeclarativeBordersPrivate(this), parent)
#else
    : WDeclarativeItem(new WDeclarativeBordersPrivate(this), parent)
#endif
{
    Q_D(WDeclarativeBorders); d->init();
}

#if defined(QT_4) || defined(SK_SOFTWARE)

//-------------------------------------------------------------------------------------------------
// QGraphicsItem / QQuickPaintedItem reimplementation
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
/* virtual */ void WDeclarativeBorders::paint(QPainter * painter,
                                              const QStyleOptionGraphicsItem *, QWidget *)
#else
/* virtual */ void WDeclarativeBorders::paint(QPainter * painter)
#endif
{
    Q_D(WDeclarativeBorders);

    painter->setPen(Qt::NoPen);

    painter->setBrush(d->color);

    qreal width  = this->width ();
    qreal height = this->height();

    painter->drawRect(QRectF(0,                0, d->left,  height));
    painter->drawRect(QRectF(width - d->right, 0, d->right, height));

    width -= d->left + d->right;

    painter->drawRect(QRectF(d->left,                  0, width, d->top));
    painter->drawRect(QRectF(d->left, height - d->bottom, width, d->bottom));
}

#endif

#if defined(QT_LATEST) && defined(SK_SOFTWARE) == false

//-------------------------------------------------------------------------------------------------
// QQuickItem reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ QSGNode * WDeclarativeBorders::updatePaintNode(QSGNode             * oldNode,
                                                             UpdatePaintNodeData *)
{
    Q_D(WDeclarativeBorders);

    WDeclarativeBordersNode * node = static_cast<WDeclarativeBordersNode *> (oldNode);

    if (node == NULL) node = new WDeclarativeBordersNode;

    qreal width  = this->width ();
    qreal height = this->height();

    if (d->updateLeft)
    {
        d->updateLeft   = false;
        d->updateTop    = false;
        d->updateBottom = false;

        node->left.setRect(QRectF(0, 0, d->left, height));

        if (d->updateRight)
        {
            d->updateRight = false;

            node->right.setRect(QRectF(width - d->right, 0, d->right, height));
        }

        width -= d->left + d->right;

        node->top   .setRect(QRectF(d->left,                  0, width, d->top));
        node->bottom.setRect(QRectF(d->left, height - d->bottom, width, d->bottom));
    }
    else if (d->updateRight)
    {
        d->updateRight  = false;
        d->updateTop    = false;
        d->updateBottom = false;

        node->right.setRect(QRectF(width - d->right, 0, d->right, height));

        width -= d->left + d->right;

        node->top   .setRect(QRectF(d->left,                  0, width, d->top));
        node->bottom.setRect(QRectF(d->left, height - d->bottom, width, d->bottom));
    }
    else
    {
        width -= d->left + d->right;

        if (d->updateTop)
        {
            d->updateTop = false;

            node->top.setRect(QRectF(d->left, 0, width, d->top));
        }

        if (d->updateBottom)
        {
            d->updateBottom = false;

            node->bottom.setRect(QRectF(d->left, height - d->bottom, width, d->bottom));
        }
    }

    if (d->updateColor)
    {
        d->updateColor = false;

        node->material.setColor(d->color);

        //-----------------------------------------------------------------------------------------
        // FIXME Qt5.12.2: Mark dirty does not seem to work properly...

        /*node->left  .markDirty(QSGNode::DirtyMaterial);
        node->right .markDirty(QSGNode::DirtyMaterial);
        node->top   .markDirty(QSGNode::DirtyMaterial);
        node->bottom.markDirty(QSGNode::DirtyMaterial);*/

        //-----------------------------------------------------------------------------------------
        // We have to remove and append nodes to apply the color.

        node->removeAllChildNodes();

        node->appendChildNode(&(node->left));
        node->appendChildNode(&(node->right));
        node->appendChildNode(&(node->top));
        node->appendChildNode(&(node->bottom));

        //-----------------------------------------------------------------------------------------
    }

    return node;
}

//-------------------------------------------------------------------------------------------------
// Protected QQuickItem reimplementation
//-------------------------------------------------------------------------------------------------

#ifdef QT_OLD
/* virtual */ void WDeclarativeBorders::geometryChanged(const QRectF & newGeometry,
                                                        const QRectF & oldGeometry)
#else
/* virtual */ void WDeclarativeBorders::geometryChange(const QRectF & newGeometry,
                                                       const QRectF & oldGeometry)
#endif
{
    Q_D(WDeclarativeBorders);

#ifdef QT_OLD
    WDeclarativeItem::geometryChanged(newGeometry, oldGeometry);
#else
    WDeclarativeItem::geometryChange(newGeometry, oldGeometry);
#endif

    if (oldGeometry.size() == newGeometry.size()) return;

    if (d->left)   d->updateLeft   = true;
    if (d->right)  d->updateRight  = true;
    if (d->top)    d->updateTop    = true;
    if (d->bottom) d->updateBottom = true;
}

#endif

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

#if defined(QT_LATEST) && defined(SK_SOFTWARE) == false
    d->updateLeft = true;
#endif

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

#if defined(QT_LATEST) && defined(SK_SOFTWARE) == false
    d->updateRight = true;
#endif

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

#if defined(QT_LATEST) && defined(SK_SOFTWARE) == false
    d->updateTop = true;
#endif

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

#if defined(QT_LATEST) && defined(SK_SOFTWARE) == false
    d->updateBottom = true;
#endif

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

#if defined(QT_LATEST) && defined(SK_SOFTWARE) == false
    d->updateColor = true;
#endif

    update();

    emit colorChanged();
}

#endif // SK_NO_DECLARATIVEBORDERS
