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

#include "WModelRange.h"

#ifndef SK_NO_MODELRANGE

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include <private/Sk_p>

class SK_GUI_EXPORT WModelRangePrivate : public WPrivate
{
protected:
    WModelRangePrivate(WModelRange * p);

    void init();

public: // Functions
    void emitValueAndPosition(qreal value, qreal position);

public: // Variables
    Qt::Orientation orientation;

    qreal value;

    qreal scale;

    qreal singleStep;
    qreal pageStep;

    qreal minimum;
    qreal maximum;

    qreal handleMinimum;
    qreal handleMaximum;

    bool isValue;

protected:
    W_DECLARE_PUBLIC(WModelRange)
};

//-------------------------------------------------------------------------------------------------

WModelRangePrivate::WModelRangePrivate(WModelRange * p) : WPrivate(p) {}

void WModelRangePrivate::init()
{
    orientation = Qt::Vertical;

    value = 0.0;

    scale = 0.0;

    singleStep =  8.0;
    pageStep   = 16.0;

    minimum = 0.0;
    maximum = 1.0;

    handleMinimum = 0.0;
    handleMaximum = 0.0;

    isValue = true;
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WModelRangePrivate::emitValueAndPosition(qreal value, qreal position)
{
    Q_Q(WModelRange);

    qreal newValue    = q->value   ();
    qreal newPosition = q->position();

    if (qFuzzyCompare(value, newValue) == false)
    {
        emit q->valueChanged();
    }

    if (qFuzzyCompare(position, newPosition) == false)
    {
        emit q->positionChanged();

        emit q->handleChanged();
    }
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

/* explicit */ WModelRange::WModelRange(QObject * parent)
    : QObject(parent), WPrivatable(new WModelRangePrivate(this))
{
    Q_D(WModelRange); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WModelRange::setRange(qreal minimum, qreal maximum)
{
    Q_D(WModelRange);

    if (d->minimum == minimum && d->maximum == maximum) return;

    qreal value    = this->value   ();
    qreal position = this->position();

    d->minimum = qMin(minimum,    maximum);
    d->maximum = qMax(d->minimum, maximum);

    emit rangeChanged();

    d->value   = value;
    d->isValue = true;

    if (d->value < d->minimum)
    {
        d->value = d->minimum;

        emit valueChanged();
    }
    else if (d->value > d->maximum)
    {
        d->value = d->maximum;

        emit valueChanged();
    }

    qreal newPosition = this->position();

    if (newPosition != position)
    {
        emit positionChanged();

        emit handleChanged();
    }
}

/* Q_INVOKABLE */ void WModelRange::setHandleRange(qreal minimum, qreal maximum)
{
    Q_D(WModelRange);

    if (d->handleMinimum == minimum && d->handleMaximum == maximum) return;

    qreal value    = this->value   ();
    qreal position = this->position();

    d->handleMinimum = qMin(minimum,          maximum);
    d->handleMaximum = qMax(d->handleMinimum, maximum);

    emit handleRangeChanged();

    d->value   = value;
    d->isValue = true;

    qreal newPosition = this->position();

    if (newPosition != position)
    {
        emit positionChanged();
    }

    emit handleChanged();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WModelRange::scroll(qreal steps)
{
    Q_D(WModelRange);

    setValue(value() + (steps * d->singleStep));
}

/* Q_INVOKABLE */ void WModelRange::pageScroll(qreal pageSteps)
{
    Q_D(WModelRange);

    setValue(value() + (pageSteps * d->pageStep));
}

//-------------------------------------------------------------------------------------------------

qreal WModelRange::valueToPosition(qreal value) const
{
    Q_D(const WModelRange);

    qreal range = d->maximum - d->minimum;

    if (range == 0.0) return d->minimum;

    qreal ratio = (d->handleMaximum - d->handleMinimum) / range;

    return value * ratio;
}

qreal WModelRange::positionToValue(qreal position) const
{
    Q_D(const WModelRange);

    qreal range = d->handleMaximum - d->handleMinimum;

    if (range == 0.0) return d->handleMinimum;

    qreal ratio = (d->maximum - d->minimum) / range;

    return position * ratio;
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

Qt::Orientation WModelRange::orientation() const
{
    Q_D(const WModelRange); return d->orientation;
}

void WModelRange::setOrientation(Qt::Orientation orientation)
{
    Q_D(WModelRange);

    if (d->orientation == orientation) return;

    d->orientation = orientation;

    emit orientationChanged();
}

//-------------------------------------------------------------------------------------------------

qreal WModelRange::value() const
{
    Q_D(const WModelRange);

    qreal value;

    if (d->isValue)
    {
         value = d->value;
    }
    else value = positionToValue(d->value);

    value += d->minimum;

    return qBound(d->minimum, value, d->maximum);
}

void WModelRange::setValue(qreal value)
{
    Q_D(WModelRange);

    if (d->isValue)
    {
        if (qFuzzyCompare(value, d->value)) return;
    }
    else if ((d->handleMaximum - d->handleMinimum)
             &&
             qFuzzyCompare(value, positionToValue(d->value))) return;

    qreal oldValue    = this->value   ();
    qreal oldPosition = this->position();

    d->value = value - d->minimum;

    d->isValue = true;

    d->emitValueAndPosition(oldValue, oldPosition);
}

//-------------------------------------------------------------------------------------------------

qreal WModelRange::position() const
{
    Q_D(const WModelRange);

    qreal position;

    if (d->isValue)
    {
         position = valueToPosition(d->value);
    }
    else position = d->value;

    position += d->handleMinimum;

    return qBound(d->handleMinimum, position, d->handleMaximum);
}

void WModelRange::setPosition(qreal position)
{
    Q_D(WModelRange);

    if (d->isValue)
    {
         if (qFuzzyCompare(position, valueToPosition(d->value))) return;
    }
    else if (qFuzzyCompare(position, d->value)) return;

    qreal oldValue    = this->value   ();
    qreal oldPosition = this->position();

    d->value = position - d->handleMinimum;

    d->isValue = false;

    d->emitValueAndPosition(oldValue, oldPosition);
}

//-------------------------------------------------------------------------------------------------

qreal WModelRange::scale() const
{
    Q_D(const WModelRange); return d->scale;
}

void WModelRange::setScale(qreal scale)
{
    Q_D(WModelRange);

    if (d->scale == scale) return;

    setValue(value() * scale / d->scale);

    d->scale = scale;

    emit scaleChanged();
}

//-------------------------------------------------------------------------------------------------

qreal WModelRange::singleStep() const
{
    Q_D(const WModelRange); return d->singleStep;
}

void WModelRange::setSingleStep(qreal singleStep)
{
    Q_D(WModelRange);

    if (d->singleStep == singleStep) return;

    d->singleStep = qMax(1.0, singleStep);

    emit singleStepChanged();
}

qreal WModelRange::pageStep() const
{
    Q_D(const WModelRange); return d->pageStep;
}

void WModelRange::setPageStep(qreal pageStep)
{
    Q_D(WModelRange);

    if (d->pageStep == pageStep) return;

    d->pageStep = qMax(1.0, pageStep);

    emit pageStepChanged();
}

//-------------------------------------------------------------------------------------------------

QSizeF WModelRange::range() const
{
    Q_D(const WModelRange); return QSizeF(d->minimum, d->maximum);
}

void WModelRange::setRange(const QSizeF & range)
{
    qreal minimum = range.width ();
    qreal maximum = range.height();

    if (minimum <= maximum)
    {
        setRange(minimum, maximum);
    }
}

//-------------------------------------------------------------------------------------------------

qreal WModelRange::minimum() const
{
    Q_D(const WModelRange); return d->minimum;
}

void WModelRange::setMinimum(qreal minimum)
{
    Q_D(WModelRange); setRange(minimum, d->maximum);
}

qreal WModelRange::maximum() const
{
    Q_D(const WModelRange); return d->maximum;
}

void WModelRange::setMaximum(qreal maximum)
{
    Q_D(WModelRange); setRange(d->minimum, maximum);
}

//-------------------------------------------------------------------------------------------------

QSizeF WModelRange::handleRange() const
{
    Q_D(const WModelRange); return QSizeF(d->handleMinimum, d->handleMaximum);
}

void WModelRange::setHandleRange(const QSizeF & range)
{
    qreal minimum = range.width ();
    qreal maximum = range.height();

    if (minimum <= maximum)
    {
        setHandleRange(minimum, maximum);
    }
}

//-------------------------------------------------------------------------------------------------

qreal WModelRange::handleMinimum() const
{
    Q_D(const WModelRange); return d->handleMinimum;
}

void WModelRange::setHandleMinimum(qreal minimum)
{
    Q_D(WModelRange); setHandleRange(minimum, qMax(minimum, d->handleMaximum));
}

qreal WModelRange::handleMaximum() const
{
    Q_D(const WModelRange); return d->handleMaximum;
}

void WModelRange::setHandleMaximum(qreal maximum)
{
    Q_D(WModelRange); setHandleRange(d->handleMinimum, qMax(d->handleMinimum, maximum));
}

//-------------------------------------------------------------------------------------------------

bool WModelRange::atMinimum() const
{
    Q_D(const WModelRange);

    if (d->handleMinimum == d->handleMaximum) return true;

    if (d->isValue)
    {
         return (d->value <= d->minimum);
    }
    else return (d->value <= d->handleMinimum);
}

bool WModelRange::atMaximum() const
{
    Q_D(const WModelRange);

    if (d->handleMinimum == d->handleMaximum) return true;

    if (d->isValue)
    {
         return (d->value >= d->maximum);
    }
    else return (d->value >= d->handleMaximum);
}

#endif // SK_NO_MODELRANGE
