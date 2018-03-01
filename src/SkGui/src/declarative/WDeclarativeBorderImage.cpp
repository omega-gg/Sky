//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

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
#ifdef QT_4
#include <QDeclarativeEngine>
#else
#include <QQmlEngine>
#endif
#include <QPainter>
#include <QTileRules>

// Sk includes
#include <WControllerView>
#ifdef QT_LATEST
#include <WView>
#endif
#include <WImageFilter>
#include <WAbstractThreadAction>

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

WDeclarativeBorderGrid * WDeclarativeBorderImagePrivate::getBorder()
{
    if (border) return border;

    Q_Q(WDeclarativeBorderImage);

    border = new WDeclarativeBorderGrid(q);

    QObject::connect(border, SIGNAL(borderChanged()), q, SLOT(onUpdate()));

    return border;
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativeBorderImagePrivate::onUpdate()
{
    Q_Q(WDeclarativeBorderImage);

    const WDeclarativeBorderGrid * border = getBorder();

    margins = QMargins(border->left(), border->top(), border->right(), border->bottom());

    q->update();
}

//=================================================================================================
// WDeclarativeBorderGrid
//=================================================================================================

/* explicit */ WDeclarativeBorderGrid::WDeclarativeBorderGrid(QObject * parent) : QObject(parent)
{
    _left   = 0;
    _right  = 0;
    _top    = 0;
    _bottom = 0;
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

int WDeclarativeBorderGrid::left() const
{
    return _left;
}

void WDeclarativeBorderGrid::setLeft(int size)
{
    if (_left == size) return;

    _left = size;

    emit borderChanged();
}

//-------------------------------------------------------------------------------------------------

int WDeclarativeBorderGrid::right() const
{
    return _right;
}

void WDeclarativeBorderGrid::setRight(int size)
{
    if (_right == size) return;

    _right = size;

    emit borderChanged();
}

//-------------------------------------------------------------------------------------------------

int WDeclarativeBorderGrid::top() const
{
    return _top;
}

void WDeclarativeBorderGrid::setTop(int size)
{
    if (_top == size) return;

    _top = size;

    emit borderChanged();
}

//-------------------------------------------------------------------------------------------------

int WDeclarativeBorderGrid::bottom() const
{
    return _bottom;
}

void WDeclarativeBorderGrid::setBottom(int size)
{
    if (_bottom == size) return;

    _bottom = size;

    emit borderChanged();
}

//=================================================================================================
// WDeclarativeBorderImage
//=================================================================================================

#ifdef QT_4
/* explicit */ WDeclarativeBorderImage::WDeclarativeBorderImage(QDeclarativeItem * parent)
#else
/* explicit */ WDeclarativeBorderImage::WDeclarativeBorderImage(QQuickItem * parent)
#endif
    : WDeclarativeImageBase(new WDeclarativeBorderImagePrivate(this), parent)
{
    Q_D(WDeclarativeBorderImage); d->init();
}

//-------------------------------------------------------------------------------------------------
// Protected

#ifdef QT_4
WDeclarativeBorderImage::WDeclarativeBorderImage(WDeclarativeBorderImagePrivate * p,
                                                 QDeclarativeItem               * parent)
#else
WDeclarativeBorderImage::WDeclarativeBorderImage(WDeclarativeBorderImagePrivate * p,
                                                 QQuickItem                     * parent)
#endif
    : WDeclarativeImageBase(p, parent)
{
    Q_D(WDeclarativeBorderImage); d->init();
}

//-------------------------------------------------------------------------------------------------
// QGraphicsItem / QQuickPaintedItem reimplementation
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
/* virtual */ void WDeclarativeBorderImage::paint(QPainter * painter,
                                                  const QStyleOptionGraphicsItem *, QWidget *)
#else
/* virtual */ void WDeclarativeBorderImage::paint(QPainter * painter)
#endif
{
#ifdef QT_LATEST
    if (isVisible() == false) return;
#endif

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
// Properties
//-------------------------------------------------------------------------------------------------

WDeclarativeBorderGrid * WDeclarativeBorderImage::border()
{
    Q_D(WDeclarativeBorderImage); return d->getBorder();
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

    const WDeclarativeBorderGrid * border = getBorder();

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

#ifdef QT_4
/* explicit */
WDeclarativeBorderImageScale::WDeclarativeBorderImageScale(QDeclarativeItem * parent)
#else
/* explicit */
WDeclarativeBorderImageScale::WDeclarativeBorderImageScale(QQuickItem * parent)
#endif
    : WDeclarativeBorderImage(new WDeclarativeBorderImageScalePrivate(this), parent)
{
    Q_D(WDeclarativeBorderImageScale); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WDeclarativeBorderImageScale::applyScale()
{
    Q_D(WDeclarativeBorderImageScale);

    if (d->scalable == false || d->scaled) return;

    d->timer.stop();

    d->abortAction();

    d->scaleSize = QSize(width(), height());

    d->onScale();
}

//-------------------------------------------------------------------------------------------------
// QGraphicsItem / QQuickPaintedItem reimplementation
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
/* virtual */ void WDeclarativeBorderImageScale::paint(QPainter * painter,
                                                       const QStyleOptionGraphicsItem *, QWidget *)
#else
/* virtual */ void WDeclarativeBorderImageScale::paint(QPainter * painter)
#endif
{
#ifdef QT_LATEST
    if (isVisible() == false) return;
#endif

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
// Protected QGraphicsItem / QQuickItem reimplementation
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
#ifdef QT_4
                if (d->viewport->scale() == 1.0)
#else
                if (d->view->item()->scale() == 1.0)
#endif
                {
                    d->abortAction();

                    d->scaleSize = size;

#ifdef QT_4
                    d->timer.start();
#else
                    QTimer::singleShot(0, &d->timer, SLOT(start()));
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

    d->scalable = false;
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
