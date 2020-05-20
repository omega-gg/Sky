//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of Sky kit.

    - GNU Lesser General Public License Usage:
    This file may be used under the terms of the GNU Lesser General Public License version 3 as
    published by the Free Software Foundation and appearing in the LICENSE.md file included in the
    packaging of this file. Please review the following information to ensure the GNU Lesser
    General Public License requirements will be met: https://www.gnu.org/licenses/lgpl.html.
*/
//=================================================================================================

#include "WDeclarativeImage.h"

#ifndef SK_NO_DECLARATIVEIMAGE

// Qt includes
#include <QPainter>
#ifdef QT_4
#include <QStyleOptionGraphicsItem>
#endif

// Sk includes
#include <WControllerFile>
#include <WControllerView>
#include <WView>
#include <WImageFilter>
#include <WAbstractThreadAction>

#ifdef QT_LATEST
// Private includes
#include <private/qsgadaptationlayer_p.h>
#endif

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

#ifdef QT_4
/* explicit */ WDeclarativeImage::WDeclarativeImage(QDeclarativeItem * parent)
#else
/* explicit */ WDeclarativeImage::WDeclarativeImage(QQuickItem * parent)
#endif
    : WDeclarativeImageBase(new WDeclarativeImagePrivate(this), parent)
{
    Q_D(WDeclarativeImage); d->init();
}

//-------------------------------------------------------------------------------------------------
// Protected

#ifdef QT_4
WDeclarativeImage::WDeclarativeImage(WDeclarativeImagePrivate * p, QDeclarativeItem * parent)
#else
WDeclarativeImage::WDeclarativeImage(WDeclarativeImagePrivate * p, QQuickItem * parent)
#endif
    : WDeclarativeImageBase(p, parent)
{
    Q_D(WDeclarativeImage); d->init();
}

#ifdef QT_4

//-------------------------------------------------------------------------------------------------
// QGraphicsItem / reimplementation
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

    painter->setRenderHint(QPainter::SmoothPixmapTransform, true);

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
                transform.scale(width / pixmapWidth, 1.0);

                drawWidth  = pixmapWidth;
                drawHeight = height;
            }
            else if (d->fillMode == TileHorizontally)
            {
                transform.scale(1.0, height / pixmapHeight);

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

            qreal widthScale  = width  / pixmapWidth;
            qreal heightScale = height / pixmapHeight;

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

#endif

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

        qreal widthScale  = width  / pixmap.width ();
        qreal heightScale = height / pixmap.height();

        if (widthScale <= heightScale)
        {
            d->paintedWidth  = width;
            d->paintedHeight = widthScale * pixmap.height();
        }
        else if (heightScale < widthScale)
        {
            d->paintedWidth  = heightScale * pixmap.width();
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

        qreal widthScale  = width () / pixmap.width ();
        qreal heightScale = height() / pixmap.height();

        if (widthScale < heightScale)
        {
            widthScale = heightScale;
        }
        else if (heightScale < widthScale)
        {
            heightScale = widthScale;
        }

        d->paintedHeight = heightScale * pixmap.height();
        d->paintedWidth  = widthScale  * pixmap.width ();
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

/* virtual */ void WDeclarativeImage::updatePixmap() {}

//-------------------------------------------------------------------------------------------------
// Protected QGraphicsItem / QQuickItem reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativeImage::geometryChanged(const QRectF & newGeometry,
                                                      const QRectF & oldGeometry)
{
    WDeclarativeImageBase::geometryChanged(newGeometry, oldGeometry);

    if (oldGeometry.size() == newGeometry.size()) return;

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

#ifdef QT_LATEST

//-------------------------------------------------------------------------------------------------
// Protected WDeclarativeTexture reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativeImage::applyGeometry(QSGInternalImageNode * node,
                                                    const QPixmap        & pixmap)
{
    Q_D(WDeclarativeImage);

    qreal width  = this->width ();
    qreal height = this->height();

    int pixmapWidth  = pixmap.width ();
    int pixmapHeight = pixmap.height();

    QRectF rect;

    if (d->fillMode >= WDeclarativeImage::Tile)
    {
        rect = QRectF(0, 0, width, height);

        if (d->fillMode == WDeclarativeImage::TileVertically)
        {
            node->setHorizontalWrapMode(QSGTexture::ClampToEdge);
            node->setVerticalWrapMode  (QSGTexture::Repeat);

            node->setSubSourceRect(QRectF(0, 0, 1, width / pixmapHeight));
        }
        else if (d->fillMode == WDeclarativeImage::TileHorizontally)
        {
            node->setHorizontalWrapMode(QSGTexture::Repeat);
            node->setVerticalWrapMode  (QSGTexture::ClampToEdge);

            node->setSubSourceRect(QRectF(0, 0, width / pixmapWidth, 1));
        }
        else
        {
            node->setHorizontalWrapMode(QSGTexture::Repeat);
            node->setVerticalWrapMode  (QSGTexture::Repeat);

            node->setSubSourceRect(QRectF(0, 0, width / pixmapWidth, height / pixmapHeight));
        }
    }
    else if (pixmapWidth != width || pixmapHeight != height)
    {
        if (d->fillMode == WDeclarativeImage::PreserveAspectFit)
        {
            qreal widthScale  = width  / pixmapWidth;
            qreal heightScale = height / pixmapHeight;

            if (widthScale < heightScale)
            {
                heightScale = widthScale;

                qreal y = (height - heightScale * pixmapHeight) / 2;

                rect = QRectF(0, y, width, height - (y * 2));
            }
            else if (widthScale > heightScale)
            {
                widthScale = heightScale;

                qreal x = (width - widthScale * pixmapWidth) / 2;

                rect = QRectF(x, 0, width - (x * 2), height);
            }
            else rect = QRectF(0, 0, width, height);
        }
        else if (d->fillMode == WDeclarativeImage::PreserveAspectCrop)
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
    }
    else
    {
        rect = QRectF(0, 0, width, height);

        node->setHorizontalWrapMode(QSGTexture::ClampToEdge);
        node->setVerticalWrapMode  (QSGTexture::ClampToEdge);

        node->setSubSourceRect(QRectF(0, 0, 1, 1));
    }

    node->setTargetRect     (rect);
    node->setInnerTargetRect(rect);
}

#endif

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

#ifdef QT_LATEST
    d->updateGeometry = true;
#endif

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

void WDeclarativeImageScalePrivate::update()
{
    Q_Q(WDeclarativeImageScale);

    const QPixmap & pixmap = q->currentPixmap();

    if (pixmap.isNull()
        ||
        fillMode > WDeclarativeImage::PreserveAspectCrop || sourceSize.isValid())
    {
         scalable = false;
    }
    else scalable = true;
}

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

#ifdef QT_LATEST

void WDeclarativeImageScalePrivate::onStart()
{
    if (scalable && scaled == false) timer.start();
}

#endif

void WDeclarativeImageScalePrivate::onScale()
{
    Q_Q(WDeclarativeImageScale);

    if (sourceDefault)
    {
        QSize size = pixmapDefault.size();

        if (fillMode == WDeclarativeImage::PreserveAspectFit)
        {
            size.scale(scaleSize, Qt::KeepAspectRatio);
        }
        else if (fillMode == WDeclarativeImage::PreserveAspectCrop)
        {
            size.scale(scaleSize, Qt::KeepAspectRatioByExpanding);
        }
        else size.scale(scaleSize, Qt::IgnoreAspectRatio);

        action = WPixmapCache::loadImage(WControllerFile::toLocalFile(urlDefault),
                                         size, q, SLOT(onLoaded(const QImage &)));

        return;
    }

    QString path = pix.path();

    if (WPixmapCache::imageIsLocal(path))
    {
        QSize size = pix.pixmap().size();

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
    else
    {
        const QPixmap & pixmap = q->currentPixmap();

#ifdef QT_LATEST
        if (pixmap.isNull()) return;
#endif

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

#ifdef QT_LATEST
        updateTexture = true;
#endif

        q->update();
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

#ifdef QT_LATEST
    updateTexture = true;
#endif

    q->update();
}

//=================================================================================================
// WDeclarativeImageScale
//=================================================================================================

#ifdef QT_4
/* explicit */ WDeclarativeImageScale::WDeclarativeImageScale(QDeclarativeItem * parent)
#else
/* explicit */ WDeclarativeImageScale::WDeclarativeImageScale(QQuickItem * parent)
#endif
    : WDeclarativeImage(new WDeclarativeImageScalePrivate(this), parent)
{
    Q_D(WDeclarativeImageScale); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WDeclarativeImageScale::applyScale()
{
    Q_D(WDeclarativeImageScale);

    if (d->scalable == false || d->scaled) return;

    d->timer.stop();

    d->abortAction();

    d->scaleSize = QSize(width(), height());

    d->onScale();
}

//-------------------------------------------------------------------------------------------------
// Protected QGraphicsItem / QQuickItem reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativeImageScale::geometryChanged(const QRectF & newGeometry,
                                                           const QRectF & oldGeometry)
{
    Q_D(WDeclarativeImageScale);

    WDeclarativeImage::geometryChanged(newGeometry, oldGeometry);

    if (d->scalable && oldGeometry.size() != newGeometry.size())
    {
#ifdef QT_LATEST
        if (d->scaled) d->updateTexture = true;
#endif

        d->restore();
    }
}

//-------------------------------------------------------------------------------------------------
// Protected WDeclarativeImageBase reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ const QPixmap & WDeclarativeImageScale::getPixmap()
{
    Q_D(WDeclarativeImageScale);

    if (d->scalable == false)
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
                if (d->view->isScaling())
                {
                    d->abortAction();

                    d->scaleSize = size;

#ifdef QT_4
                    d->timer.start();
#else
                    QTimer::singleShot(0, this, SLOT(onStart()));
#endif
                }
            }
            else
            {
                d->abortAction();

                d->scaleSize = size;

                d->onScale();
            }
        }

        return currentPixmap();
    }
    else return d->scalePixmap;
}

/* virtual */ void WDeclarativeImageScale::pixmapClear()
{
    Q_D(WDeclarativeImageScale);

    d->timer.stop();

    d->abortAction();

    d->scalable = false;
}

//-------------------------------------------------------------------------------------------------
// Protected WDeclarativeImage reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativeImageScale::updatePixmap()
{
    Q_D(WDeclarativeImageScale);

    if (d->scaling == false) return;

    d->restore();
    d->update ();
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

    if (scaling)
    {
        d->update();

#ifdef QT_LATEST
        d->updateTexture = true;
#endif
    }
    else
    {
#ifdef QT_LATEST
        if (d->scaled) d->updateTexture = true;
#endif

        d->restore();

        d->scalable = false;
    }

    update();

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
