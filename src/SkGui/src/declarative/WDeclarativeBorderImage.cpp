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

#include "WDeclarativeBorderImage.h"

#ifndef SK_NO_DECLARATIVEBORDERIMAGE

// Qt includes
#include <QDeclarativeEngine>
#include <QPainter>
#include <QTileRules>

// Sk includes
#include <WControllerView>
#include <WImageFilter>
#include <WAbstractThreadAction>

// Private includes
#include "private/qdeclarativescalegrid_p_p.h"

//=================================================================================================
// WDeclarativeBorderImagePrivate
//=================================================================================================

WDeclarativeBorderImagePrivate::WDeclarativeBorderImagePrivate(WDeclarativeBorderImage * p)
    : WDeclarativeImageBasePrivate(p) {}

void WDeclarativeBorderImagePrivate::init()
{
    border = NULL;

    horizontalTileMode = WDeclarativeBorderImage::Stretch;
    verticalTileMode   = WDeclarativeBorderImage::Stretch;
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

QDeclarativeScaleGrid * WDeclarativeBorderImagePrivate::getScaleGrid()
{
    if (border) return border;

    Q_Q(WDeclarativeBorderImage);

    border = new QDeclarativeScaleGrid(q);

    static int borderChanged = -1;

    if (borderChanged == -1)
    {
        borderChanged = QDeclarativeScaleGrid::staticMetaObject.indexOfSignal("borderChanged()");
    }

    static int onUpdate = -1;

    if (onUpdate == -1)
    {
        onUpdate = WDeclarativeBorderImage::staticMetaObject.indexOfSlot("onUpdate()");
    }

    QMetaObject::connect(border, borderChanged, q, onUpdate);

    return border;
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativeBorderImagePrivate::onUpdate()
{
    Q_Q(WDeclarativeBorderImage);

    const QDeclarativeScaleGrid * border = getScaleGrid();

    margins = QMargins(border->left(), border->top(), border->right(), border->bottom());

    q->update();
}

//=================================================================================================
// WDeclarativeBorderImage
//=================================================================================================

/* explicit */ WDeclarativeBorderImage::WDeclarativeBorderImage(QDeclarativeItem * parent)
    : WDeclarativeImageBase(new WDeclarativeBorderImagePrivate(this), parent)
{
    Q_D(WDeclarativeBorderImage); d->init();
}

//-------------------------------------------------------------------------------------------------
// Protected

WDeclarativeBorderImage::WDeclarativeBorderImage(WDeclarativeBorderImagePrivate * p,
                                                 QDeclarativeItem               * parent)
    : WDeclarativeImageBase(p, parent)
{
    Q_D(WDeclarativeBorderImage); d->init();
}

//-------------------------------------------------------------------------------------------------
// QGraphicsItem reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativeBorderImage::paint(QPainter * painter,
                                                  const QStyleOptionGraphicsItem *, QWidget *)
{
    Q_D(WDeclarativeBorderImage);

    const QPixmap & pixmap = getPixmap();

    if (pixmap.isNull()) return;

    qreal width  = this->width ();
    qreal height = this->height();

    if (width <= 0.0 || height <= 0.0) return;

    bool smooth = painter->testRenderHint(QPainter::SmoothPixmapTransform);

    painter->setRenderHint(QPainter::SmoothPixmapTransform, d->smooth);

    QTileRules rules((Qt::TileRule) d->horizontalTileMode, (Qt::TileRule) d->verticalTileMode);

    qDrawBorderPixmap(painter, QRect(0, 0, width, height),
                      d->margins, pixmap, pixmap.rect(), d->margins, rules);

    painter->setRenderHint(QPainter::SmoothPixmapTransform, smooth);
}

//-------------------------------------------------------------------------------------------------
// Protected virtual functions
//-------------------------------------------------------------------------------------------------

/* virtual */ const QPixmap & WDeclarativeBorderImage::getPixmap()
{
    return currentPixmap();
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

QDeclarativeScaleGrid * WDeclarativeBorderImage::border()
{
    Q_D(WDeclarativeBorderImage); return d->getScaleGrid();
}

//-------------------------------------------------------------------------------------------------

WDeclarativeBorderImage::TileMode WDeclarativeBorderImage::horizontalTileMode() const
{
    Q_D(const WDeclarativeBorderImage); return d->horizontalTileMode;
}

void WDeclarativeBorderImage::setHorizontalTileMode(TileMode mode)
{
    Q_D(WDeclarativeBorderImage);

    if (d->horizontalTileMode == mode) return;

    d->horizontalTileMode = mode;

    update();

    emit horizontalTileModeChanged();
}

//-------------------------------------------------------------------------------------------------

WDeclarativeBorderImage::TileMode WDeclarativeBorderImage::verticalTileMode() const
{
    Q_D(const WDeclarativeBorderImage); return d->verticalTileMode;
}

void WDeclarativeBorderImage::setVerticalTileMode(TileMode mode)
{
    Q_D(WDeclarativeBorderImage);

    if (d->verticalTileMode == mode) return;

    d->verticalTileMode = mode;

    update();

    emit verticalTileModeChanged();
}

//=================================================================================================
// WDeclarativeBorderImageScalePrivate
//=================================================================================================

WDeclarativeBorderImageScalePrivate::
WDeclarativeBorderImageScalePrivate(WDeclarativeBorderImageScale * p)
    : WDeclarativeBorderImagePrivate(p) {}

void WDeclarativeBorderImageScalePrivate::init()
{
    Q_Q(WDeclarativeBorderImageScale);

    action = NULL;

    ratioX = 1.0;
    ratioY = 1.0;

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

void WDeclarativeBorderImageScalePrivate::resize(const QPixmap & pixmap)
{
    Q_Q(WDeclarativeBorderImageScale);

    restore();

    const QDeclarativeScaleGrid * border = getScaleGrid();

    margins = QMargins(border->left(), border->top(), border->right(), border->bottom());

    if (sourceSize.isValid() == false)
    {
        scaleResize = pixmap.size();

        scaleResize.scale(q->width(), q->height(), Qt::KeepAspectRatio);

        ratioX = (qreal) scaleResize.width () / (qreal) pixmap.width ();
        ratioY = (qreal) scaleResize.height() / (qreal) pixmap.height();

        scaleMargins = QMargins(border->left () * ratioX, border->top   () * ratioY,
                                border->right() * ratioX, border->bottom() * ratioY);
    }
    else scaleMargins = margins;
}

//-------------------------------------------------------------------------------------------------

void WDeclarativeBorderImageScalePrivate::restore()
{
    timer.stop();

    abortAction();

    scalePixmap = QPixmap();
    scaleSize   = QSize  ();

    scaled = false;
}

//-------------------------------------------------------------------------------------------------

void WDeclarativeBorderImageScalePrivate::abortAction()
{
    if (action == NULL) return;

    action->abortAndDelete();

    action = NULL;
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativeBorderImageScalePrivate::onUpdate()
{
    if (scalable == false) return;

    Q_Q(WDeclarativeBorderImageScale);

    resize(q->currentPixmap());

    q->update();
}

//-------------------------------------------------------------------------------------------------

void WDeclarativeBorderImageScalePrivate::onScale()
{
    Q_Q(WDeclarativeBorderImageScale);

    QString path = pix.path();

    if (WPixmapCache::imageIsLocal(path) == false || sourceDefault)
    {
        scalePixmap = q->currentPixmap().scaled(scaleResize, Qt::KeepAspectRatio,
                                                             Qt::SmoothTransformation);

        margins = scaleMargins;

        scaled = true;

        q->update();
    }
    else action = WPixmapCache::loadImage(path, scaleResize, q, SLOT(onLoaded(const QImage &)));
}

//-------------------------------------------------------------------------------------------------

void WDeclarativeBorderImageScalePrivate::onLoaded(const QImage & image)
{
    Q_Q(WDeclarativeBorderImageScale);

    action = NULL;

    scalePixmap = QPixmap::fromImage(image);

    if (filter)
    {
        filter->applyFilter(&scalePixmap);
    }

    margins = scaleMargins;

    scaled = true;

    q->update();
}

//=================================================================================================
// WDeclarativeBorderImageScale
//=================================================================================================

/* explicit */
WDeclarativeBorderImageScale::WDeclarativeBorderImageScale(QDeclarativeItem * parent)
    : WDeclarativeBorderImage(new WDeclarativeBorderImageScalePrivate(this), parent)
{
    Q_D(WDeclarativeBorderImageScale); d->init();
}

//-------------------------------------------------------------------------------------------------
// QGraphicsItem reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativeBorderImageScale::paint(QPainter * painter,
                                                       const QStyleOptionGraphicsItem *, QWidget *)
{
    Q_D(WDeclarativeBorderImageScale);

    const QPixmap & pixmap = getPixmap();

    if (pixmap.isNull()) return;

    qreal width  = this->width ();
    qreal height = this->height();

    if (width <= 0.0 || height <= 0.0) return;

    bool smooth = painter->testRenderHint(QPainter::SmoothPixmapTransform);

    painter->setRenderHint(QPainter::SmoothPixmapTransform, d->smooth);

    QTileRules rules((Qt::TileRule) d->horizontalTileMode, (Qt::TileRule) d->verticalTileMode);

    qDrawBorderPixmap(painter, QRect(0, 0, width, height),
                      d->scaleMargins, pixmap, pixmap.rect(), d->margins, rules);

    painter->setRenderHint(QPainter::SmoothPixmapTransform, smooth);
}

//-------------------------------------------------------------------------------------------------
// Protected QGraphicsItem reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativeBorderImageScale::geometryChanged(const QRectF & newGeometry,
                                                                 const QRectF & oldGeometry)
{
    Q_D(WDeclarativeBorderImageScale);

    WDeclarativeBorderImage::geometryChanged(newGeometry, oldGeometry);

    if (d->scalable && oldGeometry.size() != newGeometry.size())
    {
        d->resize(currentPixmap());
    }
}

//-------------------------------------------------------------------------------------------------
// Protected WDeclarativeImageBase reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativeBorderImageScale::pixmapChange()
{
    Q_D(WDeclarativeBorderImageScale);

    const QPixmap & pixmap = currentPixmap();

    setImplicitWidth (pixmap.width ());
    setImplicitHeight(pixmap.height());

    if (pixmap.isNull() || d->sourceSize.isValid())
    {
         d->scalable = false;

         d->restore();
    }
    else
    {
        d->scalable = true;

        d->resize(pixmap);
    }
}

/* virtual */ void WDeclarativeBorderImageScale::pixmapClear()
{
    Q_D(WDeclarativeBorderImageScale);

    d->timer.stop();

    d->abortAction();
}

//-------------------------------------------------------------------------------------------------
// Protected WDeclarativeImage reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ const QPixmap & WDeclarativeBorderImageScale::getPixmap()
{
    Q_D(WDeclarativeBorderImageScale);

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

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

bool WDeclarativeBorderImageScale::isScaling() const
{
    Q_D(const WDeclarativeBorderImageScale); return d->scaling;
}

void WDeclarativeBorderImageScale::setScaling(bool scaling)
{
    Q_D(WDeclarativeBorderImageScale);

    if (d->scaling == scaling) return;

    d->scaling = scaling;

    if (scaling == false)
    {
        d->restore();
    }

    emit scalingChanged();
}

//-------------------------------------------------------------------------------------------------

bool WDeclarativeBorderImageScale::scaleDelayed() const
{
    Q_D(const WDeclarativeBorderImageScale); return d->scaleDelayed;
}

void WDeclarativeBorderImageScale::setScaleDelayed(bool delayed)
{
    Q_D(WDeclarativeBorderImageScale);

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

int WDeclarativeBorderImageScale::scaleDelay() const
{
    Q_D(const WDeclarativeBorderImageScale); return d->scaleDelay;
}

void WDeclarativeBorderImageScale::setScaleDelay(int delay)
{
    Q_D(WDeclarativeBorderImageScale);

    if (d->scaleDelay == delay) return;

    d->scaleDelay = delay;

    d->timer.setInterval(delay);

    emit scaleDelayChanged();
}

#endif // SK_NO_DECLARATIVEBORDERIMAGE
