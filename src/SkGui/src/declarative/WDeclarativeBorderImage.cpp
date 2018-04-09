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

#ifdef QT_LATEST
// Private includes
#include <private/qsgadaptationlayer_p.h>
#endif

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

#ifdef QT_LATEST
    updateGeometry = true;
#endif

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

#ifdef QT_4

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

    painter->setRenderHint(QPainter::SmoothPixmapTransform, true);

    QTileRules rules((Qt::TileRule) d->horizontalTileMode, (Qt::TileRule) d->verticalTileMode);

    qDrawBorderPixmap(painter, QRect(0, 0, width, height),
                      getMargins(), pixmap, pixmap.rect(), d->margins, rules);

    painter->setRenderHint(QPainter::SmoothPixmapTransform, smooth);
}

#endif

//-------------------------------------------------------------------------------------------------
// Protected virtual functions
//-------------------------------------------------------------------------------------------------

/* virtual */ const QMargins & WDeclarativeBorderImage::getMargins() const
{
    Q_D(const WDeclarativeBorderImage); return d->margins;
}

#ifdef QT_LATEST

//-------------------------------------------------------------------------------------------------
// Protected WDeclarativeTexture reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativeBorderImage::applyGeometry(QSGInternalImageNode * node,
                                                          const QPixmap        & pixmap)
{
    Q_D(WDeclarativeBorderImage);

    int width  = this->width ();
    int height = this->height();

    node->setTargetRect(QRectF(0, 0, width, height));

    const QMargins & margins = getMargins();

    int left   = margins.left  ();
    int right  = margins.right ();
    int top    = margins.top   ();
    int bottom = margins.bottom();

    QRectF target(left, top, width - (left + right), height - (top + bottom));

    node->setInnerTargetRect(target);

    int pixmapWidth  = pixmap.width ();
    int pixmapHeight = pixmap.height();

    left   = d->margins.left  ();
    right  = d->margins.right ();
    top    = d->margins.top   ();
    bottom = d->margins.bottom();

    QRectF source((qreal) left                           / pixmapWidth,
                  (qreal) top                            / pixmapHeight,
                  (qreal) (pixmapWidth  - left - right)  / pixmapWidth,
                  (qreal) (pixmapHeight - top  - bottom) / pixmapHeight);

    node->setInnerSourceRect(source);

    int tileWidth;
    int tileHeight;

    if (d->horizontalTileMode != Stretch && source.width() > 0)
    {
         tileWidth = target.width() / (source.width() * pixmapWidth);
    }
    else tileWidth = 1;

    if (d->verticalTileMode != Stretch && source.height() > 0)
    {
         tileHeight = target.height() / (source.height() * pixmapHeight);
    }
    else tileHeight = 1;

    node->setSubSourceRect(QRectF(0, 0, tileWidth, tileHeight));
}

#endif

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

#ifdef QT_LATEST
    d->updateGeometry = true;
#endif

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

#ifdef QT_LATEST
    d->updateGeometry = true;
#endif

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

    scaleResize = pixmap.size();

    scaleResize.scale(q->width(), q->height(), Qt::KeepAspectRatio);

    qreal ratioX = (qreal) scaleResize.width () / pixmap.width ();
    qreal ratioY = (qreal) scaleResize.height() / pixmap.height();

    scaleMargins = QMargins(border->left () * ratioX, border->top   () * ratioY,
                            border->right() * ratioX, border->bottom() * ratioY);
}

//-------------------------------------------------------------------------------------------------

void WDeclarativeBorderImageScalePrivate::update()
{
    Q_Q(WDeclarativeBorderImageScale);

    const QPixmap & pixmap = q->currentPixmap();

    q->setImplicitWidth (pixmap.width ());
    q->setImplicitHeight(pixmap.height());

    if (pixmap.isNull())
    {
        scalable = false;

        return;
    }

    if (sourceSize.isValid())
    {
        scalable = false;

        scaleMargins = margins;
    }
    else
    {
        scalable = true;

        resize(pixmap);
    }
}

void WDeclarativeBorderImageScalePrivate::restore()
{
    timer.stop();

    abortAction();

    scaleSize = QSize();

    if (scaled == false) return;

    scalePixmap = QPixmap();

    const WDeclarativeBorderGrid * border = getBorder();

    margins = QMargins(border->left(), border->top(), border->right(), border->bottom());

    scaled = false;

#ifdef QT_LATEST
    updateTexture = true;
#endif
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
    Q_Q(WDeclarativeBorderImageScale);

    if (scalable)
    {
        restore();

        resize(q->currentPixmap());
    }
    else
    {
        const WDeclarativeBorderGrid * border = getBorder();

        margins = QMargins(border->left(), border->top(), border->right(), border->bottom());

        scaleMargins = margins;
    }

#ifdef QT_LATEST
    updateGeometry = true;
#endif

    q->update();
}

//-------------------------------------------------------------------------------------------------

void WDeclarativeBorderImageScalePrivate::onScale()
{
    Q_Q(WDeclarativeBorderImageScale);

    QString path = pix.path();

    if (WPixmapCache::imageIsLocal(path) == false || sourceDefault)
    {
        const QPixmap & pixmap = q->currentPixmap();

#ifdef QT_LATEST
        if (pixmap.isNull()) return;
#endif

        scalePixmap = pixmap.scaled(scaleResize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        margins = scaleMargins;

        scaled = true;

#ifdef QT_LATEST
        updateTexture = true;
#endif

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

#ifdef QT_LATEST
    updateTexture = true;
#endif

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
// Protected QGraphicsItem / QQuickItem reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativeBorderImageScale::geometryChanged(const QRectF & newGeometry,
                                                                 const QRectF & oldGeometry)
{
    Q_D(WDeclarativeBorderImageScale);

    WDeclarativeBorderImage::geometryChanged(newGeometry, oldGeometry);

    if (oldGeometry.size() == newGeometry.size()) return;

    if (d->scalable)
    {
        d->restore();

        d->resize(currentPixmap());
    }
    else if (d->sourceSize.isValid() == false)
    {
        const QPixmap & pixmap = currentPixmap();

        if (pixmap.isNull()) return;

        d->resize(pixmap);
    }
}

//-------------------------------------------------------------------------------------------------
// Protected WDeclarativeTexture reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ const QPixmap & WDeclarativeBorderImageScale::getPixmap()
{
    Q_D(WDeclarativeBorderImageScale);

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

    if (d->scaling)
    {
        d->restore();
        d->update ();
    }
    else
    {
        const QPixmap & pixmap = currentPixmap();

        setImplicitWidth (pixmap.width ());
        setImplicitHeight(pixmap.height());

        if (pixmap.isNull()) return;

        if (d->sourceSize.isValid())
        {
            d->scaleMargins = d->margins;
        }
        else d->resize(pixmap);
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
// Protected WDeclarativeBorderImage reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ const QMargins & WDeclarativeBorderImageScale::getMargins() const
{
    Q_D(const WDeclarativeBorderImageScale); return d->scaleMargins;
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
