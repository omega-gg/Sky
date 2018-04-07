//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkGui module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#ifndef WDECLARATIVEBORDERS_H
#define WDECLARATIVEBORDERS_H

// Sk includes
#ifdef SK_SOFTWARE
#include <WDeclarativeItemPaint>
#else
#include <WDeclarativeItem>
#endif

#ifndef SK_NO_DECLARATIVEBORDERS

// Forward declarations
class WDeclarativeBordersPrivate;

#ifdef SK_SOFTWARE
class SK_GUI_EXPORT WDeclarativeBorders : public WDeclarativeItemPaint
#else
class SK_GUI_EXPORT WDeclarativeBorders : public WDeclarativeItem
#endif
{
    Q_OBJECT

    Q_PROPERTY(qreal borderLeft   READ left   WRITE setLeft   NOTIFY leftChanged)
    Q_PROPERTY(qreal borderRight  READ right  WRITE setRight  NOTIFY rightChanged)
    Q_PROPERTY(qreal borderTop    READ top    WRITE setTop    NOTIFY topChanged)
    Q_PROPERTY(qreal borderBottom READ bottom WRITE setBottom NOTIFY bottomChanged)

    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

public:
#ifdef QT_4
    explicit WDeclarativeBorders(QDeclarativeItem * parent = NULL);
#else
    explicit WDeclarativeBorders(QQuickItem * parent = NULL);
#endif

#if defined(QT_4) || defined(SK_SOFTWARE)
public: // QGraphicsItem / QQuickPaintedItem reimplementation
#ifdef QT_4
    /* virtual */ void paint(QPainter * painter, const QStyleOptionGraphicsItem * option,
                                                 QWidget                        * widget);
#else
    /* virtual */ void paint(QPainter * painter);
#endif
#endif

#if defined(QT_LATEST) && defined(SK_SOFTWARE) == false
public: // QQuickItem reimplementation
    /* virtual */ QSGNode * updatePaintNode(QSGNode * oldNode, UpdatePaintNodeData * data);

protected: // QQuickItem reimplementation
    /* virtual */ void geometryChanged(const QRectF & newGeometry, const QRectF & oldGeometry);
#endif

signals:
    void leftChanged  ();
    void rightChanged ();
    void topChanged   ();
    void bottomChanged();

    void colorChanged();

public: // properties
    qreal left() const;
    void  setLeft(qreal size);

    qreal right() const;
    void  setRight(qreal size);

    qreal top() const;
    void  setTop(qreal size);

    qreal bottom() const;
    void  setBottom(qreal size);

    QColor color() const;
    void   setColor(const QColor & color);

private:
    W_DECLARE_PRIVATE(WDeclarativeBorders)
};

#endif // SK_NO_DECLARATIVEBORDERS
#endif // WDECLARATIVEBORDERS_H
