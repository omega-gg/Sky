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

#include "WDeclarativeNoise.h"

#ifndef SK_NO_DECLARATIVENOISE

// Qt includes
#include <QPainter>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include "WDeclarativeNoise_p.h"

WDeclarativeNoisePrivate::WDeclarativeNoisePrivate(WDeclarativeNoise * p)
#ifdef QT_4
    : WDeclarativeItemPrivate(p) {}
#else
    : WDeclarativeItemPaintPrivate(p) {}
#endif

void WDeclarativeNoisePrivate::init()
{
#ifdef QT_4
    Q_Q(WDeclarativeNoise);
#endif

    density = QSize(64, 36);

    interval = 16;

    increment = 20;

    colorBack  = Qt::black;
    colorFront = Qt::white;

#ifdef QT_4
    q->setFlag(QGraphicsItem::ItemHasNoContents, false);
#endif
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WDeclarativeNoisePrivate::updateSize()
{
    image = QImage(density, QImage::Format_ARGB32);

    resetColor();
}

//-------------------------------------------------------------------------------------------------

void WDeclarativeNoisePrivate::resetColor()
{
    Q_Q(WDeclarativeNoise);

    int red   = colorFront.red  ();
    int green = colorFront.green();
    int blue  = colorFront.blue ();

    for (int y = 0; y < image.height(); y++)
    {
        QRgb * line = (QRgb *) image.scanLine(y);

        for (int x = 0; x < image.width(); x++)
        {
            int alpha = qrand() % 256;

            *line = qRgba(red, green, blue, alpha);

            line++;
        }
    }

    q->update();
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
/* explicit */ WDeclarativeNoise::WDeclarativeNoise(QDeclarativeItem * parent)
    : WDeclarativeItem(new WDeclarativeNoisePrivate(this), parent)
#else
/* explicit */ WDeclarativeNoise::WDeclarativeNoise(QQuickItem * parent)
    : WDeclarativeItemPaint(new WDeclarativeNoisePrivate(this), parent)
#endif
{
    Q_D(WDeclarativeNoise); d->init();
}

//-------------------------------------------------------------------------------------------------
// QDeclarativeItem reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativeNoise::componentComplete()
{
    Q_D(WDeclarativeNoise);

#ifdef QT_4
    WDeclarativeItem::componentComplete();
#else
    WDeclarativeItemPaint::componentComplete();
#endif

    d->updateSize();

    if (isVisible() && d->increment)
    {
        d->timer.start(d->interval, this);
    }
}

//-------------------------------------------------------------------------------------------------
// QGraphicsItem reimplementation
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
/* virtual */ void WDeclarativeNoise::paint(QPainter * painter,
                                            const QStyleOptionGraphicsItem *, QWidget *)
#else
/* virtual */ void WDeclarativeNoise::paint(QPainter * painter)
#endif
{
#ifdef QT_LATEST
    if (isVisible() == false) return;
#endif

    Q_D(WDeclarativeNoise);

    painter->setPen(Qt::NoPen);

    painter->setBrush(d->colorBack);

    QRectF rect(0, 0, width(), height());

    painter->drawRect(rect);

    painter->drawImage(rect, d->image, QRectF(0, 0, d->image.width(), d->image.height()));
}

//-------------------------------------------------------------------------------------------------
// Events
//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativeNoise::timerEvent(QTimerEvent *)
{
    Q_D(WDeclarativeNoise);

    for (int y = 0; y < d->image.height(); y++)
    {
        QRgb * line = (QRgb *) d->image.scanLine(y);

        for (int x = 0; x < d->image.width(); x++)
        {
            QRgb & color = *line;

            int rand = qrand() % (d->increment + 1);

            int alpha = (qAlpha(color) + rand) % 256;

            color = qRgba(qRed(color), qGreen(color), qBlue(color), alpha);

            line++;
        }
    }

    update();
}

//-------------------------------------------------------------------------------------------------
// Protected QGraphicsItem reimplementation
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
/* virtual */ QVariant WDeclarativeNoise::itemChange(GraphicsItemChange change,
                                                     const QVariant &   value)
#else
/* virtual */ void WDeclarativeNoise::itemChange(ItemChange change, const ItemChangeData & value)
#endif
{
#ifdef QT_4
    if (change == ItemVisibleHasChanged)
    {
        Q_D(WDeclarativeNoise);
#else
    Q_D(WDeclarativeNoise);

    if (d->view && change == ItemVisibleHasChanged)
    {
#endif

#ifdef QT_4
        if (value.toBool())
#else
        if (value.boolValue)
#endif
        {
            if (d->increment)
            {
                d->timer.start(d->interval, this);
            }
        }
        else d->timer.stop();
    }

#ifdef QT_4
    return WDeclarativeItem::itemChange(change, value);
#else
    WDeclarativeItemPaint::itemChange(change, value);
#endif
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

QSize WDeclarativeNoise::density() const
{
    Q_D(const WDeclarativeNoise); return d->density;
}

void WDeclarativeNoise::setDensity(const QSize & density)
{
    Q_D(WDeclarativeNoise);

    if (d->density == density) return;

    d->density = density;

    if (isComponentComplete())
    {
        d->updateSize();
    }

    emit densityChanged();
}

//-------------------------------------------------------------------------------------------------

int WDeclarativeNoise::interval() const
{
    Q_D(const WDeclarativeNoise); return d->interval;
}

void WDeclarativeNoise::setInterval(int interval)
{
    Q_D(WDeclarativeNoise);

    if (d->interval == interval) return;

    d->interval = interval;

    if (d->timer.isActive() && d->increment)
    {
        d->timer.start(interval, this);
    }

    emit intervalChanged();
}

//-------------------------------------------------------------------------------------------------

int WDeclarativeNoise::increment() const
{
    Q_D(const WDeclarativeNoise); return d->increment;
}

void WDeclarativeNoise::setIncrement(int increment)
{
    Q_D(WDeclarativeNoise);

    if (d->increment == increment) return;

    d->increment = increment;

    if (increment)
    {
        if (isVisible())
        {
            d->timer.start(d->interval, this);
        }
    }
    else d->timer.stop();

    emit incrementChanged();
}

//-------------------------------------------------------------------------------------------------

QColor WDeclarativeNoise::colorBack() const
{
    Q_D(const WDeclarativeNoise); return d->colorBack;
}

void WDeclarativeNoise::setColorBack(const QColor & color)
{
    Q_D(WDeclarativeNoise);

    if (d->colorBack == color) return;

    d->colorBack = color;

    update();

    emit colorBackChanged();
}

QColor WDeclarativeNoise::colorFront() const
{
    Q_D(const WDeclarativeNoise); return d->colorFront;
}

void WDeclarativeNoise::setColorFront(const QColor & color)
{
    Q_D(WDeclarativeNoise);

    if (d->colorFront == color) return;

    d->colorFront = color;

    d->resetColor();

    emit colorFrontChanged();
}

#endif // SK_NO_DECLARATIVENOISE
