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

#ifndef WDECLARATIVENOISE_H
#define WDECLARATIVENOISE_H

// Sk includes
#ifdef QT_4
#include <WDeclarativeItem>
#else
#include <WDeclarativeItemPaint>
#endif

#ifndef SK_NO_DECLARATIVENOISE

class WDeclarativeNoisePrivate;

#ifdef QT_4
class SK_GUI_EXPORT WDeclarativeNoise : public WDeclarativeItem
#else
class SK_GUI_EXPORT WDeclarativeNoise : public WDeclarativeItemPaint
#endif
{
    Q_OBJECT

    Q_PROPERTY(QSize density READ density WRITE setDensity NOTIFY densityChanged)

    Q_PROPERTY(int interval READ interval WRITE setInterval NOTIFY intervalChanged)

    Q_PROPERTY(int increment READ increment WRITE setIncrement NOTIFY incrementChanged)

    Q_PROPERTY(QColor colorBack  READ colorBack  WRITE setColorBack  NOTIFY colorBackChanged)
    Q_PROPERTY(QColor colorFront READ colorFront WRITE setColorFront NOTIFY colorFrontChanged)

public:
#ifdef QT_4
    explicit WDeclarativeNoise(QDeclarativeItem * parent = NULL);
#else
    explicit WDeclarativeNoise(QQuickItem * parent = NULL);
#endif

public: // QDeclarativeItem reimplementation
    /* virtual */ void componentComplete();

public: // QGraphicsItem / QQuickPaintedItem reimplementation
#ifdef QT_4
    /* virtual */ void paint(QPainter * painter, const QStyleOptionGraphicsItem * option,
                                                 QWidget                        * widget);
#else
    /* virtual */ void paint(QPainter * painter);
#endif

protected: // Events
    /* virtual */ void timerEvent(QTimerEvent * event);

protected: // QGraphicsItem / QQuickItem reimplementation
#ifdef QT_4
    /* virtual */ QVariant itemChange(GraphicsItemChange change, const QVariant & value);
#else
    /* virtual */ void itemChange(ItemChange change, const ItemChangeData & value);
#endif

signals:
    void densityChanged();

    void intervalChanged();

    void incrementChanged();

    void colorBackChanged ();
    void colorFrontChanged();

public: // Properties
    QSize density() const;
    void  setDensity(const QSize & density);

    int  interval() const;
    void setInterval(int interval);

    int  increment() const;
    void setIncrement(int increment);

    QColor colorBack() const;
    void   setColorBack(const QColor & color);

    QColor colorFront() const;
    void   setColorFront(const QColor & color);

private:
    W_DECLARE_PRIVATE(WDeclarativeNoise)
};

QML_DECLARE_TYPE(WDeclarativeNoise)

#endif // SK_NO_DECLARATIVENOISE
#endif // WDECLARATIVENOISE_H
