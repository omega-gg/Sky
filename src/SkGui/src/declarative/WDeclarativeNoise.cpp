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

// Sk includes
#include <WControllerApplication>

#ifdef QT_NEW
// Qt private includes
#include <private/qsgadaptationlayer_p.h>
#endif

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include "WDeclarativeNoise_p.h"

WDeclarativeNoisePrivate::WDeclarativeNoisePrivate(WDeclarativeNoise * p)
#ifdef QT_4
    : WDeclarativeItemPrivate(p) {}
#else
    : WDeclarativeTexturePrivate(p) {}
#endif

void WDeclarativeNoisePrivate::init()
{
    Q_Q(WDeclarativeNoise);

    density = QSize(64, 36);

    interval = 16;

    increment = 20;

    color = Qt::white;

#ifdef QT_4
    q->setFlag(QGraphicsItem::ItemHasNoContents, false);
#else
    // We want to avoid texture filtering to get sharp pixels.
    q->setSmooth(false);
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

    int red   = color.red  ();
    int green = color.green();
    int blue  = color.blue ();

    for (int y = 0; y < image.height(); y++)
    {
        QRgb * line = (QRgb *) image.scanLine(y);

        for (int x = 0; x < image.width(); x++)
        {
            int alpha = Sk::randomInt() % 256;

            *line = qRgba(red, green, blue, alpha);

            line++;
        }
    }

#ifdef QT_NEW
    pixmap = QPixmap::fromImage(image);

    updateTexture = true;
#endif

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
    : WDeclarativeTexture(new WDeclarativeNoisePrivate(this), parent)
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
    WDeclarativeTexture::componentComplete();
#endif

    d->updateSize();

    if (isVisible() && d->increment)
    {
        d->timer.start(d->interval, this);
    }
}

#ifdef QT_4

//-------------------------------------------------------------------------------------------------
// QGraphicsItem reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativeNoise::paint(QPainter * painter,
                                            const QStyleOptionGraphicsItem *, QWidget *)
{
    Q_D(WDeclarativeNoise);

    painter->setPen(Qt::NoPen);

    QRectF rect(0, 0, width(), height());

    painter->drawRect(rect);

    painter->drawImage(rect, d->image, QRectF(0, 0, d->image.width(), d->image.height()));
}

 #endif

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

            int rand = Sk::randomInt() % (d->increment + 1);

            int alpha = (qAlpha(color) + rand) % 256;

            color = qRgba(qRed(color), qGreen(color), qBlue(color), alpha);

            line++;
        }
    }

#ifdef QT_NEW
    d->pixmap = QPixmap::fromImage(d->image);

    d->updateTexture = true;
#endif

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
    WDeclarativeTexture::itemChange(change, value);
#endif
}

#ifdef QT_OLD
/* virtual */ void WDeclarativeNoise::geometryChanged(const QRectF & newGeometry,
                                                      const QRectF & oldGeometry)
#else
/* virtual */ void WDeclarativeNoise::geometryChange(const QRectF & newGeometry,
                                                     const QRectF & oldGeometry)
#endif
{
#ifdef QT_OLD
    WDeclarativeItem::geometryChanged(newGeometry, oldGeometry);
#else
    WDeclarativeTexture::geometryChange(newGeometry, oldGeometry);
#endif

    if (oldGeometry.size() == newGeometry.size()) return;

    Q_D(WDeclarativeNoise);

    d->updateGeometry = true;
}

//-------------------------------------------------------------------------------------------------
// Protected WDeclarativeTexture implementation
//-------------------------------------------------------------------------------------------------

/* virtual */ const QPixmap & WDeclarativeNoise::getPixmap()
{
    Q_D(WDeclarativeNoise); return d->pixmap;
}

//-------------------------------------------------------------------------------------------------
// Protected WDeclarativeTexture reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativeNoise::applyGeometry(QSGInternalImageNode * node,
                                                    const QPixmap        & pixmap)
{
    Q_D(WDeclarativeNoise);

    qreal width  = this->width ();
    qreal height = this->height();

    // NOTE: We take the pixel ratio into account.
    qreal ratio = ratioPixel();

    int pixmapWidth  = pixmap.width () / ratio;
    int pixmapHeight = pixmap.height() / ratio;

    QRectF rect;

    if ((pixmapWidth != width || pixmapHeight != height)
        &&
        d->fillMode == WDeclarativeNoise::PreserveAspectCrop)
    {
        qreal widthScale  = width  / pixmapWidth;
        qreal heightScale = height / pixmapHeight;

        if (widthScale < heightScale)
        {
            widthScale = heightScale;

            qreal x = (width - widthScale * pixmapWidth) / 2;

            rect = QRectF(x, 0, width - (x * 2), height);
        }
        else if (widthScale > heightScale)
        {
            heightScale = widthScale;

            qreal y = (height - heightScale * pixmapHeight) / 2;

            rect = QRectF(0, y, width, height - (y * 2));
        }
        else rect = QRectF(0, 0, width, height);
    }
    else rect = QRectF(0, 0, width, height);

    node->setHorizontalWrapMode(QSGTexture::ClampToEdge);
    node->setVerticalWrapMode  (QSGTexture::ClampToEdge);

    node->setSubSourceRect(QRectF(0, 0, 1, 1));

    node->setTargetRect     (rect);
    node->setInnerTargetRect(rect);
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

#ifdef QT_NEW

WDeclarativeNoise::FillMode WDeclarativeNoise::fillMode() const
{
    Q_D(const WDeclarativeNoise); return d->fillMode;
}

void WDeclarativeNoise::setFillMode(FillMode fillMode)
{
    Q_D(WDeclarativeNoise);

    if (d->fillMode == fillMode) return;

    d->fillMode = fillMode;

    d->updateGeometry = true;

    update();

    emit fillModeChanged();
}

#endif

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

QColor WDeclarativeNoise::color() const
{
    Q_D(const WDeclarativeNoise); return d->color;
}

void WDeclarativeNoise::setColor(const QColor & color)
{
    Q_D(WDeclarativeNoise);

    if (d->color== color) return;

    d->color = color;

    update();

    emit colorChanged();
}

#endif // SK_NO_DECLARATIVENOISE
