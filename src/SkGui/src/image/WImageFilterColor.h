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

#ifndef WIMAGEFILTERCOLOR_H
#define WIMAGEFILTERCOLOR_H

// Qt includes
#ifdef QT_4
#include <QDeclarativeItem>
#else
#include <QQuickItem>
#endif

// Sk includes
#include <WImageFilter>

#ifndef SK_NO_IMAGEFILTERCOLOR

// Forward declarations
#ifdef QT_NEW
class QGradient;
#endif
class WImageFilterColorPrivate;

//-------------------------------------------------------------------------------------------------
// WDeclarativeGradientStop
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WDeclarativeGradientStop : public QObject
{
    Q_OBJECT

    Q_PROPERTY(qreal  position READ position WRITE setPosition)
    Q_PROPERTY(QColor color    READ color    WRITE setColor)

public:
    explicit WDeclarativeGradientStop(QObject * parent = NULL);

private: // Functions
    void updateGradient();

public: // Properties
    qreal position() const;
    void  setPosition(qreal position);

    QColor color() const;
    void   setColor(const QColor & color);

private: // Variables
    qreal  _position;
    QColor _color;
};

//-------------------------------------------------------------------------------------------------
// WDeclarativeGradient
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WDeclarativeGradient : public QObject
{
    Q_OBJECT

    Q_ENUMS(GradientType)

    Q_PROPERTY(GradientType type READ type WRITE setType NOTIFY typeChanged)

#ifdef QT_4
    Q_PROPERTY(QDeclarativeListProperty<WDeclarativeGradientStop> stops READ stops)
#else
    Q_PROPERTY(QQmlListProperty<WDeclarativeGradientStop> stops READ stops)
#endif

    Q_CLASSINFO("DefaultProperty", "stops")

public: // Enums
    enum GradientType { LinearVertical, LinearHorizontal };

public:
    explicit WDeclarativeGradient(QObject * parent = NULL);

    /* virtual */ ~WDeclarativeGradient();

public: // Interface
    Q_INVOKABLE QList<WDeclarativeGradientStop *> getStops() const;

private: // Functions
    void update();

signals:
    void updated();

    void typeChanged();

public: // Properties
    GradientType type() const;
    void         setType(GradientType type);

#ifdef QT_4
    QDeclarativeListProperty<WDeclarativeGradientStop> stops();
#else
    QQmlListProperty<WDeclarativeGradientStop> stops();
#endif

    const QGradient * gradient() const;

private: // Variables
    GradientType _type;

    QList<WDeclarativeGradientStop *> _stops;

    mutable QGradient * _gradient;

private:
    friend class WDeclarativeGradientStop;
};

//-------------------------------------------------------------------------------------------------
// WImageFilterColor
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WImageFilterColor : public WImageFilter
{
    Q_OBJECT

    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

    Q_PROPERTY(WDeclarativeGradient * gradient READ gradient WRITE setGradient
               NOTIFY gradientChanged)

public:
    explicit WImageFilterColor(QObject * parent = NULL);

protected: // WImageFilter implementation
    /* virtual */ bool filter(QImage * image);

signals:
    void colorChanged   ();
    void gradientChanged();

public: // Properties
    QColor color() const;
    void   setColor(const QColor & color);

    WDeclarativeGradient * gradient() const;
    void                   setGradient(WDeclarativeGradient * gradient);

private:
    W_DECLARE_PRIVATE(WImageFilterColor)
};

#endif // SK_NO_IMAGEFILTERCOLOR
#endif // WIMAGEFILTERCOLOR_H
