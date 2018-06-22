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

#include "WDeclarativeImageSvg.h"

#ifndef SK_NO_DECLARATIVEIMAGESVG

// Qt includes
#include <QSvgRenderer>
#include <QPainter>

// Sk includes
#include <WControllerFile>
#include <WControllerView>
#ifdef QT_LATEST
#include <WView>
#endif
#include <WCache>

#ifdef QT_LATEST
// Private includes
#include <private/qsgadaptationlayer_p.h>
#endif

//=================================================================================================
// WDeclarativeImageSvgPrivate
//=================================================================================================

WDeclarativeImageSvgPrivate::WDeclarativeImageSvgPrivate(WDeclarativeImageSvg * p)
#ifdef QT_4
    : WDeclarativeItemPrivate(p) {}
#else
    : WDeclarativeTexturePrivate(p) {}
#endif

void WDeclarativeImageSvgPrivate::init()
{
    Q_Q(WDeclarativeImageSvg);

    file = NULL;

    renderer = new QSvgRenderer(q);

    status = WDeclarativeImageSvg::Null;

    loadMode = static_cast<WDeclarativeImageSvg::LoadMode> (wControllerView->loadMode());

    fillMode = WDeclarativeImageSvg::Stretch;

    loadLater = false;

    progress = 0.0;

#ifdef QT_4
    q->setFlag(QGraphicsItem::ItemHasNoContents, false);

    QObject::connect(renderer, SIGNAL(repaintNeeded()), q, SLOT(onUpdate()));
#else
    scaleDelayed = true;
    scaleDelay   = wControllerView->scaleDelay();
    scaleLater   = true;

    timer.setInterval(scaleDelay);

    timer.setSingleShot(true);

    QObject::connect(renderer, SIGNAL(repaintNeeded()), q, SLOT(update()));

    QObject::connect(&timer, SIGNAL(timeout()), q, SLOT(onTimeout()));
#endif
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

#ifdef QT_LATEST

void WDeclarativeImageSvgPrivate::updatePixmap(int width, int height)
{
    if (width < 1 || height < 1)
    {
        pixmap = QPixmap();

        return;
    }

    pixmap = QPixmap(width, height);

    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);

    renderer->render(&painter);

    updateTexture = true;
}

#endif

//-------------------------------------------------------------------------------------------------

void WDeclarativeImageSvgPrivate::load()
{
    if (loadMode == WDeclarativeImageSvg::LoadVisible && q_func()->isVisible() == false)
    {
        loadLater = true;
    }
    else loadUrl();
}

void WDeclarativeImageSvgPrivate::loadVisible()
{
    if (loadLater == false) return;

    loadLater = false;

    if (url.isValid()) loadUrl();
}

//-------------------------------------------------------------------------------------------------

void WDeclarativeImageSvgPrivate::loadUrl()
{
    Q_Q(WDeclarativeImageSvg);

    q->svgClear();

    if (url.isEmpty())
    {
        if (file) clearFile();

        clearUrl(WDeclarativeImageSvg::Null);

        return;
    }

    if (file)
    {
        QObject::disconnect(file, 0, q, 0);

        file->deleteLater();
    }

    file = wControllerFile->getHttp(url, q);

    if (file)
    {
        if (file->isLoading())
        {
            progress = 0.0;
            status   = WDeclarativeImageSvg::Loading;

            QObject::connect(file, SIGNAL(loaded(WCacheFile *)), q, SLOT(onLoaded(WCacheFile *)));

            QObject::connect(file, SIGNAL(progress(qint64, qint64)),
                             q,    SLOT(onProgress(qint64, qint64)));

            emit q->progressChanged();
            emit q->statusChanged  ();
        }
        else
        {
            QUrl url = file->urlCache();

            clearFile();

            applyUrl(url);
        }
    }
    else applyUrl(url);
}

//-------------------------------------------------------------------------------------------------

void WDeclarativeImageSvgPrivate::applyUrl(const QUrl & url)
{
    Q_Q(WDeclarativeImageSvg);

    QString source = WControllerFile::toLocalFile(url);

    if (renderer->load(source))
    {
        progress = 1.0;
        status   = WDeclarativeImageSvg::Ready;
    }
    else
    {
        progress = 0.0;
        status   = WDeclarativeImageSvg::Error;
    }

    size = renderer->defaultSize();

#ifdef QT_LATEST
    updateTexture = true;
#endif

    q->svgChange();

    emit q->progressChanged();
    emit q->statusChanged  ();

    emit q->loaded();
}

void WDeclarativeImageSvgPrivate::clearUrl(WDeclarativeImageSvg::Status status)
{
    Q_Q(WDeclarativeImageSvg);

    progress = 0.0;

    this->status = status;

    size = QSize();

#ifdef QT_LATEST
    if (pixmap.isNull() == false)
    {
        pixmap = QPixmap();

        updateTexture = true;
    }
#endif

    q->svgChange();

    emit q->progressChanged();
    emit q->statusChanged  ();

    emit q->loaded();
}

//-------------------------------------------------------------------------------------------------

void WDeclarativeImageSvgPrivate::clearFile()
{
    Q_Q(WDeclarativeImageSvg);

    QObject::disconnect(file, 0, q, 0);

    file->deleteLater();

    file = NULL;
}

//-------------------------------------------------------------------------------------------------

QRectF WDeclarativeImageSvgPrivate::getRect(qreal width, qreal height) const
{
    if (fillMode == WDeclarativeImageSvg::PreserveAspectFit)
    {
        QSizeF size = this->size;

        size.scale(QSizeF(width, height), Qt::KeepAspectRatio);

        QPointF pos((width - size.width()) / 2, (height - size.height()) / 2);

        return QRectF(pos.x(), pos.y(), size.width(), size.height());
    }
    else if (fillMode == WDeclarativeImageSvg::PreserveAspectCrop)
    {
        QSizeF size = this->size;

        size.scale(QSizeF(width, height), Qt::KeepAspectRatioByExpanding);

        QPointF pos((width - size.width()) / 2, (height - size.height()) / 2);

        return QRectF(pos.x(), pos.y(), size.width(), size.height());
    }
    else return QRectF(0, 0, width, height);
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WDeclarativeImageSvgPrivate::onProgress(qint64 received, qint64 total)
{
    if (status == WDeclarativeImageSvg::Loading && total)
    {
        Q_Q(WDeclarativeImageSvg);

        progress = (qreal) received / total;

        emit q->progressChanged();
    }
}

void WDeclarativeImageSvgPrivate::onLoaded(WCacheFile * file)
{
    this->file = NULL;

    if (file->hasError())
    {
         clearUrl(WDeclarativeImageSvg::Error);
    }
    else applyUrl(file->urlCache());

    file->deleteLater();
}

//-------------------------------------------------------------------------------------------------

#ifdef QT_4

void WDeclarativeImageSvgPrivate::onUpdate()
{
    Q_Q(WDeclarativeImageSvg); q->update();
}

#else

//-------------------------------------------------------------------------------------------------

void WDeclarativeImageSvgPrivate::onTimeout()
{
    Q_Q(WDeclarativeImageSvg);

    updateGeometry = true;

    q->update();
}

#endif

//=================================================================================================
// WDeclarativeImageSvg
//=================================================================================================

#ifdef QT_4
/* explicit */ WDeclarativeImageSvg::WDeclarativeImageSvg(QDeclarativeItem * parent)
    : WDeclarativeItem(new WDeclarativeImageSvgPrivate(this), parent)
#else
/* explicit */ WDeclarativeImageSvg::WDeclarativeImageSvg(QQuickItem * parent)
    : WDeclarativeTexture(new WDeclarativeImageSvgPrivate(this), parent)
#endif
{
    Q_D(WDeclarativeImageSvg); d->init();
}

//-------------------------------------------------------------------------------------------------
// Protected

#ifdef QT_4
WDeclarativeImageSvg::WDeclarativeImageSvg(WDeclarativeImageSvgPrivate * p,
                                           QDeclarativeItem            * parent)
    : WDeclarativeItem(p, parent)
#else
WDeclarativeImageSvg::WDeclarativeImageSvg(WDeclarativeImageSvgPrivate * p, QQuickItem * parent)
    : WDeclarativeTexture(p, parent)
#endif
{
    Q_D(WDeclarativeImageSvg); d->init();
}

//-------------------------------------------------------------------------------------------------
// QDeclarativeItem / QQuickItem reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativeImageSvg::componentComplete()
{
    Q_D(WDeclarativeImageSvg);

#ifdef QT_4
    WDeclarativeItem::componentComplete();
#else
    WDeclarativeTexture::componentComplete();
#endif

    if (d->url.isValid())
    {
        d->load();
    }
}

#ifdef QT_4

//-------------------------------------------------------------------------------------------------
// QGraphicsItem reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativeImageSvg::paint(QPainter * painter,
                                               const QStyleOptionGraphicsItem *, QWidget *)
{
    Q_D(WDeclarativeImageSvg);

    qreal width  = this->width ();
    qreal height = this->height();

    QRectF rect = d->getRect(width, height);

    if (clip())
    {
        painter->save();

        painter->setClipRect(QRectF(0, 0, width, height), Qt::IntersectClip);

        d->renderer->render(painter, rect);

        painter->restore();
    }
    else d->renderer->render(painter, rect);
}

#endif

//-------------------------------------------------------------------------------------------------
// Protected functions
//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativeImageSvg::svgChange()
{
    Q_D(WDeclarativeImageSvg);

    setImplicitWidth (d->size.width ());
    setImplicitHeight(d->size.height());
}

/* virtual */ void WDeclarativeImageSvg::svgClear() {}

//-------------------------------------------------------------------------------------------------
// Protected QGraphicsItem / QQuickItem reimplementation
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
/* virtual */ QVariant WDeclarativeImageSvg::itemChange(GraphicsItemChange change,
                                                        const QVariant &   value)
#else
/* virtual */ void WDeclarativeImageSvg::itemChange(ItemChange             change,
                                                    const ItemChangeData & value)
#endif
{
#ifdef QT_4
    if (change == ItemVisibleHasChanged)
    {
        Q_D(WDeclarativeImageSvg);

#else
    Q_D(WDeclarativeImageSvg);

    if (d->view && change == ItemVisibleHasChanged && value.boolValue)
    {
#endif
        d->loadVisible();
    }

#ifdef QT_4
    return WDeclarativeItem::itemChange(change, value);
#else
    WDeclarativeTexture::itemChange(change, value);
#endif
}

//-------------------------------------------------------------------------------------------------

#ifdef QT_LATEST

/* virtual */ void WDeclarativeImageSvg::geometryChanged(const QRectF & newGeometry,
                                                         const QRectF & oldGeometry)
{
    WDeclarativeTexture::geometryChanged(newGeometry, oldGeometry);

    if (oldGeometry.size() == newGeometry.size()) return;

    Q_D(WDeclarativeImageSvg);

    d->updateGeometry = true;

    if (d->scaleDelayed && (d->scaleLater || d->timer.isActive() == false))
    {
        d->timer.start();
    }
}

//-------------------------------------------------------------------------------------------------
// WDeclarativeTexture implementation
//-------------------------------------------------------------------------------------------------

/* virtual */ const QPixmap & WDeclarativeImageSvg::getPixmap()
{
    Q_D(WDeclarativeImageSvg);

    if (d->updateTexture)
    {
        if (d->fillMode == PreserveAspectFit)
        {
            QSizeF size = d->size;

            size.scale(QSizeF(width(), height()), Qt::KeepAspectRatio);

            d->updatePixmap(size.width(), size.height());
        }
        else if (d->fillMode == PreserveAspectCrop)
        {
            QSizeF size = d->size;

            size.scale(QSizeF(width(), height()), Qt::KeepAspectRatioByExpanding);

            d->updatePixmap(size.width(), size.height());
        }
        else d->updatePixmap(width(), height());
    }
    else if (d->updateGeometry && d->timer.isActive() == false)
    {
        if (d->fillMode == PreserveAspectFit)
        {
            QSizeF size = d->size;

            size.scale(QSizeF(width(), height()), Qt::KeepAspectRatio);

            int width  = size.width ();
            int height = size.height();

            if (d->pixmap.width() != width && d->pixmap.height() != height)
            {
                d->updatePixmap(width, height);
            }
        }
        else if (d->fillMode == PreserveAspectCrop)
        {
            QSizeF size = d->size;

            size.scale(QSizeF(width(), height()), Qt::KeepAspectRatioByExpanding);

            int width  = size.width ();
            int height = size.height();

            if (d->pixmap.width() != width && d->pixmap.height() != height)
            {
                d->updatePixmap(width, height);
            }
        }
        else
        {
            int width  = this->width ();
            int height = this->height();

            if (d->pixmap.width() != width && d->pixmap.height() != height)
            {
                d->updatePixmap(width, height);
            }
        }
    }

    return d->pixmap;
}

//-------------------------------------------------------------------------------------------------
// Protected WDeclarativeTexture reimplementation
//-------------------------------------------------------------------------------------------------

/*virtual */ void WDeclarativeImageSvg::applyGeometry(QSGInternalImageNode * node, const QPixmap &)
{
    Q_D(WDeclarativeImageSvg);

    QRectF rect = d->getRect(width(), height());

    node->setTargetRect     (rect);
    node->setInnerTargetRect(rect);
}

#endif

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

WDeclarativeImageSvg::Status WDeclarativeImageSvg::status() const
{
    Q_D(const WDeclarativeImageSvg); return d->status;
}

//-------------------------------------------------------------------------------------------------

bool WDeclarativeImageSvg::isNull() const
{
    Q_D(const WDeclarativeImageSvg); return (d->status == Null);
}

bool WDeclarativeImageSvg::isLoading() const
{
    Q_D(const WDeclarativeImageSvg); return (d->status == Loading);
}

bool WDeclarativeImageSvg::isReady() const
{
    Q_D(const WDeclarativeImageSvg); return (d->status == Ready);
}

//-------------------------------------------------------------------------------------------------

QUrl WDeclarativeImageSvg::source() const
{
    Q_D(const WDeclarativeImageSvg); return d->url;
}

void WDeclarativeImageSvg::setSource(const QUrl & url)
{
    Q_D(WDeclarativeImageSvg);

    if (d->url.isEmpty() == url.isEmpty() && d->url == url) return;

    d->url = url;

    if (isComponentComplete()) d->load();

    emit sourceChanged();
}

//-------------------------------------------------------------------------------------------------

WDeclarativeImageSvg::LoadMode WDeclarativeImageSvg::loadMode() const
{
    Q_D(const WDeclarativeImageSvg); return d->loadMode;
}

void WDeclarativeImageSvg::setLoadMode(LoadMode mode)
{
    Q_D(WDeclarativeImageSvg);

    if (d->loadMode == mode) return;

    d->loadMode = mode;

    if (mode != LoadVisible)
    {
        d->loadVisible();
    }

    emit loadModeChanged();
}

//-------------------------------------------------------------------------------------------------

WDeclarativeImageSvg::FillMode WDeclarativeImageSvg::fillMode() const
{
    Q_D(const WDeclarativeImageSvg); return d->fillMode;
}

void WDeclarativeImageSvg::setFillMode(FillMode fillMode)
{
    Q_D(WDeclarativeImageSvg);

    if (d->fillMode == fillMode) return;

    d->fillMode = fillMode;

    update();

    emit fillModeChanged();
}

//-------------------------------------------------------------------------------------------------

qreal WDeclarativeImageSvg::progress() const
{
    Q_D(const WDeclarativeImageSvg); return d->progress;
}

//-------------------------------------------------------------------------------------------------

#ifdef QT_LATEST

bool WDeclarativeImageSvg::scaleDelayed() const
{
    Q_D(const WDeclarativeImageSvg); return d->scaleDelayed;
}

void WDeclarativeImageSvg::setScaleDelayed(bool delayed)
{
    Q_D(WDeclarativeImageSvg);

    if (d->scaleDelayed == delayed) return;

    d->scaleDelayed = delayed;

    if (delayed == false && d->timer.isActive())
    {
        d->timer.stop();

        d->onTimeout();
    }

    emit scaleDelayedChanged();
}

//-------------------------------------------------------------------------------------------------

int WDeclarativeImageSvg::scaleDelay() const
{
    Q_D(const WDeclarativeImageSvg); return d->scaleDelay;
}

void WDeclarativeImageSvg::setScaleDelay(int delay)
{
    Q_D(WDeclarativeImageSvg);

    if (d->scaleDelay == delay) return;

    d->scaleDelay = delay;

    d->timer.setInterval(delay);

    emit scaleDelayChanged();
}

//-------------------------------------------------------------------------------------------------

bool WDeclarativeImageSvg::scaleLater() const
{
    Q_D(const WDeclarativeImageSvg); return d->scaleLater;
}

void WDeclarativeImageSvg::setScaleLater(bool enabled)
{
    Q_D(WDeclarativeImageSvg);

    if (d->scaleLater == enabled) return;

    d->scaleLater = enabled;

    emit scaleLaterChanged();
}

#else

//=================================================================================================
// WDeclarativeImageSvgScalePrivate
//=================================================================================================

WDeclarativeImageSvgScalePrivate::WDeclarativeImageSvgScalePrivate(WDeclarativeImageSvgScale * p)
    : WDeclarativeImageSvgPrivate(p) {}

void WDeclarativeImageSvgScalePrivate::init()
{
    Q_Q(WDeclarativeImageSvgScale);

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

void WDeclarativeImageSvgScalePrivate::update()
{
    scalable = size.isValid();
}

void WDeclarativeImageSvgScalePrivate::restore()
{
    timer.stop();

    scalePixmap = QPixmap();
    scaleSize   = QSize  ();

    scaled = false;
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WDeclarativeImageSvgScalePrivate::onScale()
{
    Q_Q(WDeclarativeImageSvgScale);

    qreal width  = q->width ();
    qreal height = q->height();

    scalePixmap = QPixmap(width, height);

    scalePixmap.fill(Qt::transparent);

    QPainter painter(&scalePixmap);

    QRectF rect = getRect(width, height);

    renderer->render(&painter, rect);

    scaled = true;

    q->update();
}

//=================================================================================================
// WDeclarativeImageSvgScale
//=================================================================================================

/* explicit */ WDeclarativeImageSvgScale::WDeclarativeImageSvgScale(QDeclarativeItem * parent)
    : WDeclarativeImageSvg(new WDeclarativeImageSvgScalePrivate(this), parent)
{
    Q_D(WDeclarativeImageSvgScale); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WDeclarativeImageSvgScale::applyScale()
{
    Q_D(WDeclarativeImageSvgScale);

    if (d->scalable == false || d->scaled) return;

    d->timer.stop();

    d->scaleSize = QSize(width(), height());

    d->onScale();
}

//-------------------------------------------------------------------------------------------------
// QGraphicsItem reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativeImageSvgScale::paint(QPainter                       * painter,
                                                    const QStyleOptionGraphicsItem * option,
                                                    QWidget                        * widget)
{
    Q_D(WDeclarativeImageSvgScale);

    if (d->scalable)
    {
        if (d->scaled)
        {
            bool smooth = painter->testRenderHint(QPainter::SmoothPixmapTransform);

            painter->setRenderHint(QPainter::SmoothPixmapTransform);

            if (clip())
            {
                painter->save();

                painter->setClipRect(QRectF(0, 0, width(), height()), Qt::IntersectClip);

                painter->drawPixmap(0, 0, d->scalePixmap);

                painter->restore();
            }
            else painter->drawPixmap(0, 0, d->scalePixmap);

            painter->setRenderHint(QPainter::SmoothPixmapTransform, smooth);

            return;
        }

        QSize size = QSize(width(), height());

        if (d->scaleSize != size)
        {
            if (d->scaleDelayed)
            {
                if (d->viewport->scale() == 1.0)
                {
                    d->scaleSize = size;

                    d->timer.start();
                }
            }
            else
            {
                d->scaleSize = size;

                d->onScale();
            }
        }
    }

    WDeclarativeImageSvg::paint(painter, option, widget);
}

//-------------------------------------------------------------------------------------------------
// Protected QGraphicsItem reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativeImageSvgScale::geometryChanged(const QRectF & newGeometry,
                                                              const QRectF & oldGeometry)
{
    Q_D(WDeclarativeImageSvgScale);

    WDeclarativeImageSvg::geometryChanged(newGeometry, oldGeometry);

    if (d->scalable && oldGeometry.size() != newGeometry.size())
    {
        d->restore();
    }
}

//-------------------------------------------------------------------------------------------------
// Protected WDeclarativeImageSvg reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativeImageSvgScale::svgChange()
{
    Q_D(WDeclarativeImageSvgScale);

    WDeclarativeImageSvg::svgChange();

    if (d->scaling == false) return;

    d->restore();
    d->update ();
}

/* virtual */ void WDeclarativeImageSvgScale::svgClear()
{
    Q_D(WDeclarativeImageSvgScale);

    d->timer.stop();

    d->scalable = false;
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

bool WDeclarativeImageSvgScale::isScaling() const
{
    Q_D(const WDeclarativeImageSvgScale); return d->scaling;
}

void WDeclarativeImageSvgScale::setScaling(bool scaling)
{
    Q_D(WDeclarativeImageSvgScale);

    if (d->scaling == scaling) return;

    d->scaling = scaling;

    if (scaling == false)
    {
        d->restore();

        d->scalable = false;
    }
    else d->update();

    update();

    emit scalingChanged();
}

//-------------------------------------------------------------------------------------------------

bool WDeclarativeImageSvgScale::scaleDelayed() const
{
    Q_D(const WDeclarativeImageSvgScale); return d->scaleDelayed;
}

void WDeclarativeImageSvgScale::setScaleDelayed(bool delayed)
{
    Q_D(WDeclarativeImageSvgScale);

    if (d->scaleDelayed == delayed) return;

    d->scaleDelayed = delayed;

    if (delayed == false && d->timer.isActive())
    {
        svgClear();

        d->onScale();
    }

    emit scaleDelayedChanged();
}

//-------------------------------------------------------------------------------------------------

int WDeclarativeImageSvgScale::scaleDelay() const
{
    Q_D(const WDeclarativeImageSvgScale); return d->scaleDelay;
}

void WDeclarativeImageSvgScale::setScaleDelay(int delay)
{
    Q_D(WDeclarativeImageSvgScale);

    if (d->scaleDelay == delay) return;

    d->scaleDelay = delay;

    d->timer.setInterval(delay);

    emit scaleDelayChanged();
}

#endif

#endif // SK_NO_DECLARATIVEIMAGESVG
