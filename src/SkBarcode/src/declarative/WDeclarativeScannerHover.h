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

#ifndef WDECLARATIVESCANNERHOVER_H
#define WDECLARATIVESCANNERHOVER_H

// Sk includes
#ifdef QT_4
#include <WDeclarativeItem>
#else
#include <WDeclarativeItemPaint>
#endif

#ifndef SK_NO_DECLARATIVESCANNERHOVER

class WDeclarativeScannerHoverPrivate;

#ifdef QT_4
class SK_BARCODE_EXPORT WDeclarativeScannerHover : public WDeclarativeItem
#else
class SK_BARCODE_EXPORT WDeclarativeScannerHover : public WDeclarativeItemPaint
#endif
{
    Q_OBJECT

    Q_PROPERTY(bool isActive READ isActive NOTIFY activeChanged)

    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

public:
#ifdef QT_4
    explicit WDeclarativeScannerHover(QDeclarativeItem * parent = NULL);
#else
    explicit WDeclarativeScannerHover(QQuickItem * parent = NULL);
#endif

public: // Interface
    Q_INVOKABLE void apply(const QPointF & topLeft,    const QPointF & topRight,
                           const QPointF & bottomLeft, const QPointF & bottomRight);

    Q_INVOKABLE void clear();

public: // QGraphicsItem / QQuickPaintedItem reimplementation
#ifdef QT_4
    /* virtual */ void paint(QPainter * painter, const QStyleOptionGraphicsItem * option,
                                                 QWidget                        * widget);
#else
    /* virtual */ void paint(QPainter * painter);
#endif

signals:
    void activeChanged();

    void colorChanged();

public: // Properties
    bool isActive() const;

    QColor color() const;
    void   setColor(const QColor & color);

private:
    W_DECLARE_PRIVATE(WDeclarativeScannerHover)
};

#endif // SK_NO_DECLARATIVESCANNERHOVER
#endif // WDECLARATIVESCANNERHOVER_H
