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

#include "WImageFilterColor.h"

#ifndef SK_NO_IMAGEFILTERCOLOR

// Qt includes
#ifdef QT_LATEST
#include <QGradient>
#endif

//=================================================================================================
// WDeclarativeGradientStop
//=================================================================================================

/* explicit */ WDeclarativeGradientStop::WDeclarativeGradientStop(QObject * parent)
    : QObject(parent) {}

//-------------------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------------------

void WDeclarativeGradientStop::updateGradient()
{
    WDeclarativeGradient * gradient = qobject_cast<WDeclarativeGradient *> (parent());

    if (gradient) gradient->update();
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

qreal WDeclarativeGradientStop::position() const
{
    return _position;
}

void WDeclarativeGradientStop::setPosition(qreal position)
{
    _position = position;

    updateGradient();
}

//-------------------------------------------------------------------------------------------------

QColor WDeclarativeGradientStop::color() const
{
    return _color;
}

void WDeclarativeGradientStop::setColor(const QColor & color)
{
    _color = color;

    updateGradient();
}

//=================================================================================================
// WDeclarativeGradient
//=================================================================================================

/* explicit */ WDeclarativeGradient::WDeclarativeGradient(QObject * parent) : QObject(parent)
{
    _type = LinearVertical;

    _gradient = NULL;
}

/* virtual */ WDeclarativeGradient::~WDeclarativeGradient()
{
    delete _gradient;
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ QList<WDeclarativeGradientStop *> WDeclarativeGradient::getStops() const
{
    return _stops;
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WDeclarativeGradient::update()
{
    delete _gradient;

    _gradient = NULL;

    emit updated();
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

WDeclarativeGradient::GradientType WDeclarativeGradient::type() const
{
    return _type;
}

void WDeclarativeGradient::setType(GradientType type)
{
    if (_type == type) return;

    _type = type;

    update();

    emit typeChanged();
}

//-------------------------------------------------------------------------------------------------

#ifdef QT_4
QDeclarativeListProperty<WDeclarativeGradientStop> WDeclarativeGradient::stops()
#else
QQmlListProperty<WDeclarativeGradientStop> WDeclarativeGradient::stops()
#endif
{
#ifdef QT_4
    return QDeclarativeListProperty<WDeclarativeGradientStop>(this, _stops);
#else
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    return QQmlListProperty<WDeclarativeGradientStop>(this, &_stops);
#else
    return QQmlListProperty<WDeclarativeGradientStop>(this, _stops);
#endif
#endif
}

const QGradient * WDeclarativeGradient::gradient() const
{
    if (_gradient == NULL && _stops.isEmpty() == false)
    {
        if (_type == LinearVertical) _gradient = new QLinearGradient(0, 0,   0,   1.0);
        else                         _gradient = new QLinearGradient(0, 1.0, 1.0, 1.0);

        foreach (const WDeclarativeGradientStop * stop, _stops)
        {
            _gradient->setCoordinateMode(QGradient::ObjectBoundingMode);

            _gradient->setColorAt(stop->position(), stop->color());
        }
    }

    return _gradient;
}

//=================================================================================================
// WImageFilterColorPrivate
//=================================================================================================

#include <private/WImageFilter_p>

class SK_GUI_EXPORT WImageFilterColorPrivate : public WImageFilterPrivate
{
public:
    WImageFilterColorPrivate(WImageFilter * p);

    void init();

public: // Functions
    QRect getMargins(const QImage & image) const;

    QList<QRgb> getGradientColors(const QGradient * gradient, int size) const;

public: // Variables
    QColor color;

    WDeclarativeGradient * gradient;

protected:
    W_DECLARE_PUBLIC(WImageFilterColor)
};

//-------------------------------------------------------------------------------------------------

WImageFilterColorPrivate::WImageFilterColorPrivate(WImageFilter * p) : WImageFilterPrivate(p) {}

void WImageFilterColorPrivate::init()
{
    gradient = NULL;
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

QRect WImageFilterColorPrivate::getMargins(const QImage & image) const
{
    const QRgb * constBits = (const QRgb *) image.constBits();
    const QRgb * bits;

    int left = -1;

    for (int x = 0; left == -1 && x < image.width(); x++)
    {
        bits = constBits + x;

        for (int y = 0; y < image.height(); y++)
        {
            if (*bits != qRgba(0, 0, 0, 0))
            {
                left = x;

                break;
            }

            bits += image.width();
        }
    }

    if (left == -1) return QRect();

    int top = -1;

    bits = constBits;

    for (int y = 0; top == -1 && y < image.height(); y++)
    {
        for (int x = 0; x < image.width(); x++)
        {
            if (*bits != qRgba(0, 0, 0, 0))
            {
                top = y;

                break;
            }

            bits++;
        }
    }

    if (top == -1) return QRect();

    int right = -1;

    for (int x = (image.width() - 1); right == -1 && x > -1; x--)
    {
        bits = constBits + x;

        for (int y = 0; y < image.height(); y++)
        {
            if (*bits != qRgba(0, 0, 0, 0))
            {
                right = x;

                break;
            }

            bits += image.width();
        }
    }

    if (right == -1) return QRect();

    int bottom = -1;

    bits = constBits + (image.width() * image.height()) - 1;

    for (int y = (image.height() - 1); bottom == -1 && y > -1; y--)
    {
        for (int x = 0; x < image.width(); x++)
        {
            if (*bits != qRgba(0, 0, 0, 0))
            {
                bottom = y;

                break;
            }

            bits--;
        }
    }

    if (bottom == -1) return QRect();

    return QRect(left, top, right + 1, bottom + 1);
}

QList<QRgb> WImageFilterColorPrivate::getGradientColors(const QGradient * gradient, int size) const
{
    QList<QRgb> colors;

    qreal cursorInterval = 1.0 / size;
    qreal cursor         = 0;

    int stopIndex = 0;

    QGradientStop stopA = gradient->stops().at(stopIndex);
    QGradientStop stopB = gradient->stops().at(stopIndex + 1);

    qreal stopValue  = stopB.first;
    qreal stopCursor = 0;

    qreal stopSize = stopB.first - stopA.first;

    QColor stopColor = stopA.second;

    int stopRed   = stopB.second.red  () - stopA.second.red();
    int stopGreen = stopB.second.green() - stopA.second.green();
    int stopBlue  = stopB.second.blue () - stopA.second.blue();

    for (int i = 0; i < size; i++)
    {
        qreal factor = stopCursor / stopSize;

        QRgb color = qRgb(stopColor.red  () + (stopRed   * factor),
                          stopColor.green() + (stopGreen * factor),
                          stopColor.blue () + (stopBlue  * factor));

        colors.append(color);

        cursor += cursorInterval;

        if (cursor > stopValue)
        {
            stopIndex++;

            if (stopIndex < (gradient->stops().count() - 1))
            {
                stopA = gradient->stops().at(stopIndex);
                stopB = gradient->stops().at(stopIndex + 1);

                stopValue  = stopB.first;
                stopCursor = 0;

                stopSize = stopB.first - stopA.first;

                stopColor = stopA.second;

                stopRed   = stopB.second.red  () - stopA.second.red  ();
                stopGreen = stopB.second.green() - stopA.second.green();
                stopBlue  = stopB.second.blue () - stopA.second.blue ();
            }
        }
        else stopCursor += cursorInterval;
    }

    return colors;
}

//=================================================================================================
// WImageFilterColor
//=================================================================================================

/* explicit */ WImageFilterColor::WImageFilterColor(QObject * parent)
    : WImageFilter(new WImageFilterColorPrivate(this), parent)
{
    Q_D(WImageFilterColor); d->init();
}

//-------------------------------------------------------------------------------------------------
// WImageFilter implementation
//-------------------------------------------------------------------------------------------------

/* virtual */ bool WImageFilterColor::filter(QImage * image)
{
    Q_D(WImageFilterColor);

    QImage::Format format = image->format();

    if (format != QImage::Format_RGB32
        &&
        format != QImage::Format_ARGB32
        &&
        format != QImage::Format_ARGB32_Premultiplied) return false;

#ifdef QT_OLD
    const QImage alphaChannel = image->alphaChannel();
#else
    const QImage alphaChannel = image->convertToFormat(QImage::Format_Alpha8);
#endif

    QRect margins = d->getMargins(*image);

    const QGradient * gradient;

    if (d->gradient) gradient = d->gradient->gradient();
    else             gradient = NULL;

    if (gradient && gradient->stops().count() > 1)
    {
        if (d->gradient->type() == WDeclarativeGradient::LinearVertical)
        {
            QList<QRgb> colors = d->getGradientColors(gradient, margins.height() - margins.y());

            for (int y = margins.y(); y < margins.height(); y++)
            {
                QRgb * line = (QRgb *) image->scanLine(y) + margins.x();

                int indexColor = y - margins.y();

                for (int x = margins.x(); x < margins.width(); x++)
                {
                    if (*line != qRgba(0, 0, 0, 0))
                    {
                        *line = colors.at(indexColor);
                    }

                    line++;
                }
            }
        }
        else
        {
            QList<QRgb> colors = d->getGradientColors(gradient, margins.width() - margins.x());

            for (int y = margins.y(); y < margins.height(); y++)
            {
                QRgb * line = (QRgb *) image->scanLine(y) + margins.x();

                for (int x = margins.x(); x < margins.width(); x++)
                {
                    if (*line != qRgba(0, 0, 0, 0))
                    {
                        *line = colors.at(x - margins.x());
                    }

                    line++;
                }
            }
        }
    }
    else
    {
        QRgb color = qRgb(d->color.red(), d->color.green(), d->color.blue());

        for (int y = margins.y(); y < margins.height(); y++)
        {
            QRgb * line = (QRgb *) image->scanLine(y) + margins.x();

            for (int x = margins.x(); x < margins.width(); x++)
            {
                if (*line != qRgba(0, 0, 0, 0))
                {
                    *line = color;
                }

                line++;
            }
        }
    }

    image->setAlphaChannel(alphaChannel);

    return true;
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

QColor WImageFilterColor::color() const
{
    Q_D(const WImageFilterColor); return d->color;
}

void WImageFilterColor::setColor(const QColor & color)
{
    Q_D(WImageFilterColor);

    if (d->color == color) return;

    d->color = color;

    refreshFilter();

    emit colorChanged();
}

//-------------------------------------------------------------------------------------------------

WDeclarativeGradient * WImageFilterColor::gradient() const
{
    Q_D(const WImageFilterColor); return d->gradient;
}

void WImageFilterColor::setGradient(WDeclarativeGradient * gradient)
{
    Q_D(WImageFilterColor);

    if (d->gradient == gradient) return;

    if (d->gradient)
    {
        disconnect(d->gradient, 0, this, 0);
    }

    d->gradient = gradient;

    if (d->gradient)
    {
        connect(d->gradient, SIGNAL(updated()), this, SLOT(refreshFilter()));
    }

    refreshFilter();

    emit gradientChanged();
}

#endif // SK_NO_IMAGEFILTERCOLOR
