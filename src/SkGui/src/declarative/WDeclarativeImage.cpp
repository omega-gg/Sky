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

#include "WDeclarativeImage.h"

#ifndef SK_NO_DECLARATIVEIMAGE

// Qt includes
#include <QPainter>
#include <QStyleOptionGraphicsItem>

// Sk includes
#include <WControllerView>
#include <WImageFilter>
#include <WAbstractThreadAction>

//=================================================================================================
// WDeclarativeImagePrivate
//=================================================================================================

WDeclarativeImagePrivate::WDeclarativeImagePrivate(WDeclarativeImage * p)
    : WDeclarativeImageBasePrivate(p) {}

void WDeclarativeImagePrivate::init()
{
    fillMode = WDeclarativeImage::Stretch;

    paintedWidth  = 0;
    paintedHeight = 0;
}

//=================================================================================================
// WDeclarativeImage
//=================================================================================================

/* explicit */ WDeclarativeImage::WDeclarativeImage(QDeclarativeItem * parent)
    : WDeclarativeImageBase(new WDeclarativeImagePrivate(this), parent)
{
    Q_D(WDeclarativeImage); d->init();
}

//-------------------------------------------------------------------------------------------------
// Protected

WDeclarativeImage::WDeclarativeImage(WDeclarativeImagePrivate * p, QDeclarativeItem * parent)
    : WDeclarativeImageBase(p, parent)
{
    Q_D(WDeclarativeImage); d->init();
}

//-------------------------------------------------------------------------------------------------
// QGraphicsItem reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativeImage::paint(QPainter * painter,
                                            const QStyleOptionGraphicsItem *, QWidget *)
{
    Q_D(WDeclarativeImage);

    const QPixmap & pixmap = getPixmap();

    if (pixmap.isNull()) return;

    qreal width  = this->width ();
    qreal height = this->height();

    int pixmapWidth  = pixmap.width ();
    int pixmapHeight = pixmap.height();

    bool smooth = painter->testRenderHint(QPainter::SmoothPixmapTransform);

    painter->setRenderHint(QPainter::SmoothPixmapTransform, d->smooth);

    if (pixmapWidth != width || pixmapHeight != height)
    {
        if (d->fillMode >= Tile)
        {
            QTransform transform;
            QTransform oldTransform;

            int drawWidth;
            int drawHeight;

            if (d->fillMode == TileVertically)
            {
                transform.scale(width / qreal(pixmapWidth), 1.0);

                drawWidth  = pixmapWidth;
                drawHeight = height;
            }
            else if (d->fillMode == TileHorizontally)
            {
                transform.scale(1.0, height / qreal(pixmapHeight));

                drawWidth  = width;
                drawHeight = pixmapHeight;
            }
            else
            {
                drawWidth  = width;
                drawHeight = height;
            }

            if (transform.isIdentity() == false)
            {
                oldTransform = painter->transform();

                painter->setWorldTransform(transform * oldTransform);

                painter->drawTiledPixmap(QRectF(0, 0, drawWidth, drawHeight), pixmap);

                painter->setWorldTransform(oldTransform);
            }
            else painter->drawTiledPixmap(QRectF(0, 0, drawWidth, drawHeight), pixmap);
        }
        else
        {
            qreal x;
            qreal y;

            qreal widthScale  = width  / qreal(pixmapWidth);
            qreal heightScale = height / qreal(pixmapHeight);

            if (d->fillMode == PreserveAspectFit)
            {
                if (widthScale <= heightScale)
                {
                    heightScale = widthScale;

                    x = 0;
                    y = (height - heightScale * pixmapHeight) / 2;
                }
                else
                {
                    widthScale = heightScale;

                    x = (width - widthScale * pixmapWidth) / 2;
                    y = 0;
                }
            }
            else if (d->fillMode == PreserveAspectCrop)
            {
                if (widthScale < heightScale)
                {
                    widthScale = heightScale;

                    x = (width - widthScale * pixmapWidth) / 2;
                    y = 0;
                }
                else if (widthScale > heightScale)
                {
                    heightScale = widthScale;

                    x = 0;
                    y = (height - heightScale * pixmapHeight) / 2;
                }
                else
                {
                    x = 0;
                    y = 0;
                }
            }
            else
            {
                x = 0;
                y = 0;
            }

            if (clip())
            {
                painter->save();

                painter->setClipRect(QRectF(0, 0, width, height), Qt::IntersectClip);

                painter->drawPixmap(QRectF((int) x,               (int) y,
                                           (int) width - (x * 2), (int) height - (y * 2)),
                                    pixmap,
                                    QRectF(0, 0, pixmapWidth, pixmapHeight));

                painter->restore();
            }
            else painter->drawPixmap(QRectF((int) x,               (int) y,
                                            (int) width - (x * 2), (int) height - (y * 2)),
                                     pixmap,
                                     QRectF(0, 0, pixmapWidth, pixmapHeight));
        }
    }
    else painter->drawPixmap(0, 0, pixmap);

    painter->setRenderHint(QPainter::SmoothPixmapTransform, smooth);
}

//-------------------------------------------------------------------------------------------------
// Protected functions
//-------------------------------------------------------------------------------------------------

void WDeclarativeImage::updatePaintedGeometry()
{
    Q_D(WDeclarativeImage);

    if (d->fillMode == PreserveAspectFit)
    {
        const QPixmap & pixmap = currentPixmap();

        if (pixmap.width() == 0 || pixmap.height() == 0) return;

        qreal width  = widthValid () ? this->width () : pixmap.width ();
        qreal height = heightValid() ? this->height() : pixmap.height();

        qreal widthScale  = width  / qreal(pixmap.width ());
        qreal heightScale = height / qreal(pixmap.height());

        if (widthScale <= heightScale)
        {
            d->paintedWidth  = width;
            d->paintedHeight = widthScale * qreal(pixmap.height());
        }
        else if (heightScale < widthScale)
        {
            d->paintedWidth  = heightScale * qreal(pixmap.width());
            d->paintedHeight = height;
        }

        if (widthValid() && heightValid() == false)
        {
             setImplicitHeight(d->paintedHeight);
        }
        else setImplicitHeight(pixmap.height());

        if (heightValid() && widthValid() == false)
        {
             setImplicitWidth(d->paintedWidth);
        }
        else setImplicitWidth(pixmap.width());
    }
    else if (d->fillMode == PreserveAspectCrop)
    {
        const QPixmap & pixmap = currentPixmap();

        if (pixmap.width() == 0 || pixmap.height() == 0) return;

        qreal widthScale  = width()  / qreal(pixmap.width ());
        qreal heightScale = height() / qreal(pixmap.height());

        if (widthScale < heightScale)
        {
            widthScale = heightScale;
        }
        else if (heightScale < widthScale)
        {
            heightScale = widthScale;
        }

        d->paintedHeight = heightScale * qreal(pixmap.height());
        d->paintedWidth  = widthScale  * qreal(pixmap.width ());
    }
    else
    {
        d->paintedWidth  = width ();
        d->paintedHeight = height();
    }

    emit paintedGeometryChanged();
}

//-------------------------------------------------------------------------------------------------
// Protected virtual functions
//-------------------------------------------------------------------------------------------------

/* virtual */ const QPixmap & WDeclarativeImage::getPixmap()
{
    return currentPixmap();
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativeImage::updatePixmap() {}

//-------------------------------------------------------------------------------------------------
// Protected QGraphicsItem reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativeImage::geometryChanged(const QRectF & newGeometry,
                                                      const QRectF & oldGeometry)
{
    WDeclarativeImageBase::geometryChanged(newGeometry, oldGeometry);

    updatePaintedGeometry();
}

//-------------------------------------------------------------------------------------------------
// Protected WDeclarativeImageBase reimplementation
//-------------------------------------------------------------------------------------------------

void WDeclarativeImage::pixmapChange()
{
    Q_D(WDeclarativeImage);

    if (d->fillMode != PreserveAspectFit)
    {
        WDeclarativeImageBase::pixmapChange();
    }

    updatePaintedGeometry();

    updatePixmap();
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

WDeclarativeImage::FillMode WDeclarativeImage::fillMode() const
{
    Q_D(const WDeclarativeImage); return d->fillMode;
}

void WDeclarativeImage::setFillMode(FillMode fillMode)
{
    Q_D(WDeclarativeImage);

    if (d->fillMode == fillMode) return;

    d->fillMode = fillMode;

    updatePaintedGeometry();

    updatePixmap();

    update();

    emit fillModeChanged();
}

//-------------------------------------------------------------------------------------------------

qreal WDeclarativeImage::paintedWidth() const
{
    Q_D(const WDeclarativeImage); return d->paintedWidth;
}

qreal WDeclarativeImage::paintedHeight() const
{
    Q_D(const WDeclarativeImage); return d->paintedHeight;
}

//=================================================================================================
// WDeclarativeImageScalePrivate
//=================================================================================================

WDeclarativeImageScalePrivate::WDeclarativeImageScalePrivate(WDeclarativeImageScale * p)
    : WDeclarativeImagePrivate(p) {}

void WDeclarativeImageScalePrivate::init()
{
    Q_Q(WDeclarativeImageScale);

    action = NULL;

    scaling  = true;
    scalable = false;
    scaled   = false;

    scaleDelayed = true;
    scaleDelay   = wControllerView->scaleDelay();

    timer.setInterval(scaleDelay);

    timer.setSingleShot(true);

    QObject::connect(&timer, SIGNAL(timeout()), q, SLOT(onScale()));
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WDeclarativeImageScalePrivate::restore()
{
    timer.stop();

    abortAction();

    scalePixmap = QPixmap();
    scaleSize   = QSize  ();

    scaled = false;
}

//-------------------------------------------------------------------------------------------------

void WDeclarativeImageScalePrivate::abortAction()
{
    if (action == NULL) return;

    action->abortAndDelete();

    action = NULL;
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WDeclarativeImageScalePrivate::onScale()
{
    Q_Q(WDeclarativeImageScale);

    const QPixmap & pixmap = q->currentPixmap();

    QString path = pix.path();

    if (WPixmapCache::imageIsLocal(path) == false || sourceDefault)
    {
        if (fillMode == WDeclarativeImage::PreserveAspectFit)
        {
            scalePixmap = pixmap.scaled(scaleSize, Qt::KeepAspectRatio,
                                                   Qt::SmoothTransformation);
        }
        else if (fillMode == WDeclarativeImage::PreserveAspectCrop)
        {
            scalePixmap = pixmap.scaled(scaleSize, Qt::KeepAspectRatioByExpanding,
                                                   Qt::SmoothTransformation);
        }
        else scalePixmap = pixmap.scaled(scaleSize, Qt::IgnoreAspectRatio,
                                                    Qt::SmoothTransformation);

        scaled = true;

        q->update();
    }
    else
    {
        QSize size = pixmap.size();

        if (fillMode == WDeclarativeImage::PreserveAspectFit)
        {
            size.scale(scaleSize, Qt::KeepAspectRatio);
        }
        else if (fillMode == WDeclarativeImage::PreserveAspectCrop)
        {
            size.scale(scaleSize, Qt::KeepAspectRatioByExpanding);
        }
        else size.scale(scaleSize, Qt::IgnoreAspectRatio);

        action = WPixmapCache::loadImage(path, size, q, SLOT(onLoaded(const QImage &)));
    }
}

//-------------------------------------------------------------------------------------------------

void WDeclarativeImageScalePrivate::onLoaded(const QImage & image)
{
    Q_Q(WDeclarativeImageScale);

    action = NULL;

    scalePixmap = QPixmap::fromImage(image);

    if (filter)
    {
        filter->applyFilter(&scalePixmap);
    }

    scaled = true;

    q->update();
}

//=================================================================================================
// WDeclarativeImageScale
//=================================================================================================

/* explicit */ WDeclarativeImageScale::WDeclarativeImageScale(QDeclarativeItem * parent)
    : WDeclarativeImage(new WDeclarativeImageScalePrivate(this), parent)
{
    Q_D(WDeclarativeImageScale); d->init();
}

//-------------------------------------------------------------------------------------------------
// Protected QGraphicsItem reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativeImageScale::geometryChanged(const QRectF & newGeometry,
                                                           const QRectF & oldGeometry)
{
    Q_D(WDeclarativeImageScale);

    WDeclarativeImage::geometryChanged(newGeometry, oldGeometry);

    if (d->scaling && d->scalable && oldGeometry.size() != newGeometry.size())
    {
        d->restore();
    }
}

//-------------------------------------------------------------------------------------------------
// Protected WDeclarativeImageBase reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativeImageScale::pixmapClear()
{
    Q_D(WDeclarativeImageScale);

    d->timer.stop();

    d->abortAction();
}

//-------------------------------------------------------------------------------------------------
// Protected WDeclarativeImage reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ const QPixmap & WDeclarativeImageScale::getPixmap()
{
    Q_D(WDeclarativeImageScale);

    if (d->scaling == false || d->scalable == false)
    {
        return currentPixmap();
    }
    else if (d->scaled == false)
    {
        QSize size = QSize(width(), height());

        if (d->scaleSize != size)
        {
            if (d->scaleDelayed)
            {
                if (d->viewport->scale() == 1.0)
                {
                    d->scaleSize = size;

                    d->abortAction();

                    d->timer.start();
                }
            }
            else
            {
                d->scaleSize = size;

                d->abortAction();

                d->onScale();
            }
        }

        return currentPixmap();
    }
    else return d->scalePixmap;
}

/* virtual */ void WDeclarativeImageScale::updatePixmap()
{
    Q_D(WDeclarativeImageScale);

    if (d->scaling) d->restore();

    const QPixmap & pixmap = currentPixmap();

    if (pixmap.isNull() || d->fillMode > PreserveAspectCrop || d->sourceSize.isValid())
    {
         d->scalable = false;
    }
    else d->scalable = true;
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

bool WDeclarativeImageScale::isScaling() const
{
    Q_D(const WDeclarativeImageScale); return d->scaling;
}

void WDeclarativeImageScale::setScaling(bool scaling)
{
    Q_D(WDeclarativeImageScale);

    if (d->scaling == scaling) return;

    d->scaling = scaling;

    if (scaling == false)
    {
        d->restore();
    }

    emit scalingChanged();
}

//-------------------------------------------------------------------------------------------------

bool WDeclarativeImageScale::scaleDelayed() const
{
    Q_D(const WDeclarativeImageScale); return d->scaleDelayed;
}

void WDeclarativeImageScale::setScaleDelayed(bool delayed)
{
    Q_D(WDeclarativeImageScale);

    if (d->scaleDelayed == delayed) return;

    d->scaleDelayed = delayed;

    if (delayed == false && d->timer.isActive())
    {
        pixmapClear();

        d->onScale();
    }

    emit scaleDelayedChanged();
}

//-------------------------------------------------------------------------------------------------

int WDeclarativeImageScale::scaleDelay() const
{
    Q_D(const WDeclarativeImageScale); return d->scaleDelay;
}

void WDeclarativeImageScale::setScaleDelay(int delay)
{
    Q_D(WDeclarativeImageScale);

    if (d->scaleDelay == delay) return;

    d->scaleDelay = delay;

    d->timer.setInterval(delay);

    emit scaleDelayChanged();
}

#endif // SK_NO_DECLARATIVEIMAGE
