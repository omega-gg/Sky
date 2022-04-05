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

#include "WDeclarativeImageBase.h"

#ifndef SK_NO_DECLARATIVEIMAGEBASE

#ifdef QT_4
// Qt includes
#include <QImageReader>
#endif

// Sk includes
#include <WControllerView>
#include <WControllerFile>
#ifdef QT_NEW
#include <WView>
#endif
#include <WCache>
#include <WImageFilter>
#ifdef QT_6
#include <QQmlContext>
#endif

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

WDeclarativeImageBasePrivate::WDeclarativeImageBasePrivate(WDeclarativeImageBase * p)
#ifdef QT_4
    : WDeclarativeItemPrivate(p) {}
#else
    : WDeclarativeTexturePrivate(p) {}
#endif

//-------------------------------------------------------------------------------------------------

void WDeclarativeImageBasePrivate::init()
{
    file = NULL;

    status = WDeclarativeImageBase::Null;

    loadMode = static_cast<WDeclarativeImageBase::LoadMode> (wControllerView->loadMode());

    asynchronous = false;

    progress = 0.0;

    filter = NULL;

    sourceDefault = true;

    explicitSize = false;

    loadLater = false;

    cache = true;

#ifdef QT_4
    Q_Q(WDeclarativeImageBase);

    q->setFlag(QGraphicsItem::ItemHasNoContents, false);
#endif
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WDeclarativeImageBasePrivate::loadVisible()
{
    if (loadLater == false) return;

    loadLater = false;

    loadUrl();
}

//-------------------------------------------------------------------------------------------------

void WDeclarativeImageBasePrivate::loadUrl()
{
    Q_Q(WDeclarativeImageBase);

    q->pixmapClear();

    WCache * cache = wControllerFile->cache();

    if (cache) QObject::disconnect(cache, 0, q, 0);

    if (url.isEmpty())
    {
        if (file) clearFile();

        pix.clear(q);

        q->clearUrl(WDeclarativeImageBase::Null);

        return;
    }

    if (file)
    {
        QObject::disconnect(file, 0, q, 0);

        file->deleteLater();
    }

    QString source = WControllerFile::resolvedUrl(q, url);

    file = wControllerFile->getHttp(source, q);

    if (file)
    {
        if (file->isLoading())
        {
            if (pix.pixmap().isNull())
            {
                pix.clear(q);

                applySourceDefault();
            }
            else pix.disconnect(q);

            progress = 0.0;
            status   = WDeclarativeImageBase::Loading;

            QObject::connect(file, SIGNAL(loaded(WCacheFile *)), q, SLOT(onLoaded(WCacheFile *)));

            QObject::connect(file, SIGNAL(progress       (qint64, qint64)),
                             q,    SLOT  (requestProgress(qint64, qint64)));

            emit q->progressChanged();
            emit q->statusChanged  ();
        }
        else
        {
            QString url = file->urlCache();

            clearFile();

            q->applyUrl(url, asynchronous);
        }
    }
    else q->applyUrl(source, asynchronous);
}

//-------------------------------------------------------------------------------------------------

void WDeclarativeImageBasePrivate::loadDefault()
{
    Q_Q(WDeclarativeImageBase);

    readDefault();

    q->pixmapChange();

#ifdef QT_NEW
    updateTexture = true;
#endif

    q->update();
}

void WDeclarativeImageBasePrivate::readDefault()
{
    Q_Q(WDeclarativeImageBase);

    QString source = WControllerFile::resolvedUrl(q, urlDefault);

    if (defaultSize.isValid())
    {
         WPixmapCache::readPixmap(&(pixmapDefault), WControllerFile::toLocalFile(source),
                                  defaultSize, sourceArea);
    }
    else WPixmapCache::readPixmap(&(pixmapDefault), WControllerFile::toLocalFile(source),
                                  sourceSize, sourceArea);

    if (filter) applyFilter();
}

//-------------------------------------------------------------------------------------------------

void WDeclarativeImageBasePrivate::applyRequest()
{
    Q_Q(WDeclarativeImageBase);

    progress = 1.0;
    status   = WDeclarativeImageBase::Ready;

    setSourceDefault(pix.isNull());

    if (filter) applyFilter();

    q->pixmapChange();

#ifdef QT_NEW
    updateTexture = true;
#endif

    q->update();

    /*if (sourceSize.width() != pix.width() || sourceSize.height() != pix.height())
    {
        emit q->sourceSizeChanged();
    }*/

    emit q->progressChanged();
    emit q->statusChanged  ();

    emit q->loaded();
}

void WDeclarativeImageBasePrivate::applyFilter()
{
    if (sourceDefault == false || pixmapDefault.isNull())
    {
        QPixmap pixmap = pix.pixmap();

        filter->applyFilter(&pixmap);

        // NOTE: We cannot 'applyPixmap' while loading or 'requestFinished' is never called.
        if (pix.isLoading() == false)
        {
            QString path = pix.path();

            pix.applyPixmap(pixmap, path);
        }
        else pix.changePixmap(pixmap);
    }
    else filter->applyFilter(&pixmapDefault);
}

//-------------------------------------------------------------------------------------------------

void WDeclarativeImageBasePrivate::setSourceDefault(bool sourceDefault)
{
    if (this->sourceDefault == sourceDefault) return;

    Q_Q(WDeclarativeImageBase);

    this->sourceDefault = sourceDefault;

    emit q->defaultChanged();
}

void WDeclarativeImageBasePrivate::applySourceDefault()
{
    Q_Q(WDeclarativeImageBase);

    if (sourceDefault)
    {
        if (urlDefault.isEmpty() == false && pixmapDefault.isNull())
        {
            loadDefault();
        }
        else if (filter)
        {
            filter->applyFilter(&pixmapDefault);

            q->pixmapChange();

#ifdef QT_NEW
            updateTexture = true;
#endif

            q->update();
        }
    }
    else
    {
        sourceDefault = true;

        if (urlDefault.isEmpty() == false && pixmapDefault.isNull())
        {
            readDefault();
        }

        q->pixmapChange();

#ifdef QT_NEW
        updateTexture = true;
#endif

        q->update();

        emit q->defaultChanged();
    }
}

//-------------------------------------------------------------------------------------------------

void WDeclarativeImageBasePrivate::setExplicitSize(bool explicitSize)
{
    Q_Q(WDeclarativeImageBase);

    this->explicitSize = explicitSize;

    emit q->explicitSizeChanged();
}

//-------------------------------------------------------------------------------------------------

void WDeclarativeImageBasePrivate::clearFile()
{
    Q_Q(WDeclarativeImageBase);

    QObject::disconnect(file, 0, q, 0);

    file->deleteLater();

    file = NULL;
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WDeclarativeImageBasePrivate::onLoaded(WCacheFile * file)
{
    Q_Q(WDeclarativeImageBase);

    this->file = NULL;

    if (file->hasError())
    {
         q->clearUrl(WDeclarativeImageBase::Error);
    }
    else q->applyUrl(file->urlCache(), true);

    file->deleteLater();
}

//-------------------------------------------------------------------------------------------------

void WDeclarativeImageBasePrivate::onFilterUpdated()
{
    if (loadLater) return;

    Q_Q(WDeclarativeImageBase);

    // FIXME: Maybe we could avoid reloading everything.
    q->reload();
}

void WDeclarativeImageBasePrivate::onFilterClear()
{
    filter = NULL;

    onFilterUpdated();
}

//-------------------------------------------------------------------------------------------------

void WDeclarativeImageBasePrivate::onFilesRemoved(const QStringList & urls, const QStringList &)
{
    if (urls.contains(url)) loadUrl();
}

void WDeclarativeImageBasePrivate::onFilesCleared()
{
    loadUrl();
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
/* explicit */ WDeclarativeImageBase::WDeclarativeImageBase(QDeclarativeItem * parent)
    : WDeclarativeItem(new WDeclarativeImageBasePrivate(this), parent)
#else
/* explicit */ WDeclarativeImageBase::WDeclarativeImageBase(QQuickItem * parent)
    : WDeclarativeTexture(new WDeclarativeImageBasePrivate(this), parent)
#endif
{
    Q_D(WDeclarativeImageBase); d->init();
}

//-------------------------------------------------------------------------------------------------
// Protected

#ifdef QT_4
WDeclarativeImageBase::WDeclarativeImageBase(WDeclarativeImageBasePrivate * p,
                                             QDeclarativeItem             * parent)
    : WDeclarativeItem(p, parent)
#else
WDeclarativeImageBase::WDeclarativeImageBase(WDeclarativeImageBasePrivate * p, QQuickItem * parent)
    : WDeclarativeTexture(p, parent)
#endif
{
    Q_D(WDeclarativeImageBase); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WDeclarativeImageBase::loadSource(const QString & url, bool force)
{
    Q_D(WDeclarativeImageBase);

    if (force) d->url = QString();

    setSource(url);
}

/* Q_INVOKABLE */ void WDeclarativeImageBase::loadNow(const QString & url)
{
    Q_D(WDeclarativeImageBase);

    QString source;

    if (url.isEmpty())
    {
         source = d->url;
    }
    else source = url;

    LoadMode loadMode     = d->loadMode;
    bool     asynchronous = d->asynchronous;

    d->loadMode     = LoadAlways;
    d->asynchronous = false;

    if (d->status == Loading)
    {
        loadSource(source, true);
    }
    else if (d->loadLater)
    {
        d->loadLater = false;

        loadSource(source, true);
    }
    else setSource(source);

    d->loadMode     = loadMode;
    d->asynchronous = asynchronous;
}

//-------------------------------------------------------------------------------------------------s

/* Q_INVOKABLE */ void WDeclarativeImageBase::reload()
{
    Q_D(WDeclarativeImageBase);

    if (isComponentComplete() == false) return;

    if (d->loadMode == WDeclarativeImageBase::LoadVisible && isVisible() == false)
    {
        d->pixmapDefault = QPixmap();

        d->loadLater = true;
    }
    else if (d->urlDefault.isEmpty() == false)
    {
        if (d->sourceDefault == false)
        {
            d->pixmapDefault = QPixmap();

            if (d->url.isEmpty() == false)
            {
                d->loadUrl();
            }
        }
        else d->loadDefault();
    }
    else if (d->url.isEmpty() == false)
    {
        d->loadUrl();
    }
}

//-------------------------------------------------------------------------------------------------

#ifdef QT_4
/* Q_INVOKABLE */ void WDeclarativeImageBase::setItemShot(QGraphicsObject * object)
#else
/* Q_INVOKABLE */ void WDeclarativeImageBase::setItemShot(QQuickItem * item)
#endif
{
#ifdef QT_4
    Q_ASSERT(object);

    QPixmap pixmap = WControllerView::takeItemShot(object, Qt::transparent);
#else
    Q_ASSERT(item);

    QPixmap pixmap = WControllerView::takeItemShot(item, Qt::transparent);
#endif

    setPixmap(pixmap);
}

//-------------------------------------------------------------------------------------------------
// QDeclarativeItem / QQuickItem reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativeImageBase::componentComplete()
{
    Q_D(WDeclarativeImageBase);

#ifdef QT_4
    WDeclarativeItem::componentComplete();
#else
    WDeclarativeTexture::componentComplete();
#endif

    if (d->url.isEmpty() == false)
    {
        load();
    }
    else if (d->urlDefault.isEmpty() == false)
    {
        d->loadDefault();
    }
}

//-------------------------------------------------------------------------------------------------
// Protected Functions
//-------------------------------------------------------------------------------------------------

const QPixmap & WDeclarativeImageBase::currentPixmap() const
{
    Q_D(const WDeclarativeImageBase);

    if (d->sourceDefault)
    {
         return d->pixmapDefault;
    }
    else return d->pix.pixmap();
}

//-------------------------------------------------------------------------------------------------
// Protected virtual functions
//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativeImageBase::load()
{
    Q_D(WDeclarativeImageBase);

    if (d->loadMode == LoadVisible && isVisible() == false)
    {
        d->loadLater = true;
    }
    else d->loadUrl();
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativeImageBase::applyUrl(const QString & url, bool asynchronous)
{
    Q_D(WDeclarativeImageBase);

    QPixmap pixmap = d->pix.pixmap();

    if (d->explicitSize)
    {
         d->pix.load(WControllerFile::toString(url), d->sourceSize, d->sourceArea, asynchronous,
                     d->cache, this, SLOT(requestFinished()));
    }
    else d->pix.load(WControllerFile::toString(url), QSize(), QSize(), asynchronous,
                     d->cache, this, SLOT(requestFinished()));

    if (d->pix.isLoading())
    {
        WCache * cache = wControllerFile->cache();

        if (cache)
        {
            connect(cache, SIGNAL(filesRemoved(QStringList, QStringList)),
                    this,  SLOT(onFilesRemoved(QStringList, QStringList)));

            connect(cache, SIGNAL(filesCleared()), this, SLOT(onFilesCleared()));
        }

        if (asynchronous)
        {
            // NOTE: We enforce the previous pixmap to avoid blinking when loading the new one.
            if (pixmap.isNull())
            {
                d->applySourceDefault();
            }
            else d->pix.changePixmap(pixmap);
        }
        else d->applySourceDefault();

        if (d->progress)
        {
            d->progress = 0.0;

            emit progressChanged();
        }

        if (d->status != Loading)
        {
            d->status = Loading;

            emit statusChanged();
        }
    }
    else requestFinished();
}

/* virtual */ void WDeclarativeImageBase::clearUrl(WDeclarativeImageBase::Status status)
{
    Q_D(WDeclarativeImageBase);

    d->applySourceDefault();

    d->progress = 0.0;
    d->status   = status;

    emit progressChanged();
    emit statusChanged  ();

    emit loaded();
}

//-------------------------------------------------------------------------------------------------

#ifdef QT_4

/* virtual */ const QPixmap & WDeclarativeImageBase::getPixmap()
{
    return currentPixmap();
}

#endif

//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativeImageBase::pixmapChange()
{
    const QPixmap & pixmap = currentPixmap();

    setImplicitWidth (pixmap.width ());
    setImplicitHeight(pixmap.height());
}

/* virtual */ void WDeclarativeImageBase::pixmapClear() {}

//-------------------------------------------------------------------------------------------------
// Protected slots
//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativeImageBase::requestFinished()
{
    Q_D(WDeclarativeImageBase);

    WCache * cache = wControllerFile->cache();

    if (cache) QObject::disconnect(cache, 0, this, 0);

    if (d->pix.isNull())
    {
        clearUrl(Error);
    }
    else d->applyRequest();
}

/* virtual */ void WDeclarativeImageBase::requestProgress(qint64 received, qint64 total)
{
    Q_D(WDeclarativeImageBase);

    if (d->status == Loading && total)
    {
        d->progress = (qreal) received / total;

        emit progressChanged();
    }
}

//-------------------------------------------------------------------------------------------------
// Protected QGraphicsItem / QQuickItem reimplementation
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
/* virtual */ QVariant WDeclarativeImageBase::itemChange(GraphicsItemChange change,
                                                         const QVariant &   value)
#else
/* virtual */ void WDeclarativeImageBase::itemChange(ItemChange             change,
                                                     const ItemChangeData & value)
#endif
{
#ifdef QT_4
    if (change == ItemVisibleHasChanged)
    {
        Q_D(WDeclarativeImageBase);

#else
    Q_D(WDeclarativeImageBase);

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

#ifdef QT_NEW

#ifdef QT_OLD
/* virtual */ void WDeclarativeImageBase::geometryChanged(const QRectF & newGeometry,
                                                          const QRectF & oldGeometry)
#else
/* virtual */ void WDeclarativeImageBase::geometryChange(const QRectF & newGeometry,
                                                         const QRectF & oldGeometry)
#endif
{
    Q_D(WDeclarativeImageBase);

#ifdef QT_OLD
    WDeclarativeItem::geometryChanged(newGeometry, oldGeometry);
#else
    WDeclarativeTexture::geometryChange(newGeometry, oldGeometry);
#endif

    if (oldGeometry.size() == newGeometry.size()) return;

    d->updateGeometry = true;
}

//-------------------------------------------------------------------------------------------------
// Protected WDeclarativeItem reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativeImageBase::updateRatioPixel()
{
    reload();
}

//-------------------------------------------------------------------------------------------------
// Protected WDeclarativeTexture implementation
//-------------------------------------------------------------------------------------------------

/* virtual */ const QPixmap & WDeclarativeImageBase::getPixmap()
{
    return currentPixmap();
}

#endif

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

QPixmap WDeclarativeImageBase::pixmap() const
{
    Q_D(const WDeclarativeImageBase); return d->pix.pixmap();
}

void WDeclarativeImageBase::setPixmap(const QPixmap & pixmap)
{
    Q_D(WDeclarativeImageBase);

    if (d->url.isEmpty() == false)
    {
        d->url = QString();

        emit sourceChanged();
    }

    d->pix.setPixmap(pixmap, this);

    d->applyRequest();
}

//-------------------------------------------------------------------------------------------------

WDeclarativeImageBase::Status WDeclarativeImageBase::status() const
{
    Q_D(const WDeclarativeImageBase); return d->status;
}

//-------------------------------------------------------------------------------------------------

bool WDeclarativeImageBase::isNull() const
{
    Q_D(const WDeclarativeImageBase); return (d->status == Null);
}

bool WDeclarativeImageBase::isLoading() const
{
    Q_D(const WDeclarativeImageBase); return (d->status == Loading);
}

bool WDeclarativeImageBase::isReady() const
{
    Q_D(const WDeclarativeImageBase); return (d->status == Ready);
}

//-------------------------------------------------------------------------------------------------

bool WDeclarativeImageBase::isSourceDefault() const
{
    Q_D(const WDeclarativeImageBase); return d->sourceDefault;
}

bool WDeclarativeImageBase::isExplicitSize() const
{
    Q_D(const WDeclarativeImageBase); return d->explicitSize;
}

//-------------------------------------------------------------------------------------------------

QString WDeclarativeImageBase::source() const
{
    Q_D(const WDeclarativeImageBase); return d->url;
}

void WDeclarativeImageBase::setSource(const QString & url)
{
    Q_D(WDeclarativeImageBase);

    if (d->url.isEmpty() == url.isEmpty() && d->url == url) return;

    d->url = url;

    if (d->loadMode == LoadVisible && isVisible() == false && url.isEmpty() == false)
    {
        d->setSourceDefault(false);
    }

    if (isComponentComplete()) load();

    emit sourceChanged();
}

//-------------------------------------------------------------------------------------------------

QString WDeclarativeImageBase::sourceDefault() const
{
    Q_D(const WDeclarativeImageBase); return d->urlDefault;
}

void WDeclarativeImageBase::setSourceDefault(const QString & url)
{
    Q_D(WDeclarativeImageBase);

    if (d->urlDefault.isEmpty() == url.isEmpty() && d->urlDefault == url) return;

    d->urlDefault = url;

    if (isComponentComplete())
    {
        if (d->loadMode == LoadVisible && isVisible() == false)
        {
            d->pixmapDefault = QPixmap();

            d->loadLater = true;

            if (d->sourceDefault)
            {
                pixmapChange();
            }
        }
        else if (d->sourceDefault)
        {
            if (url.isEmpty())
            {
                d->pixmapDefault = QPixmap();
            }
            else d->readDefault();

            pixmapChange();

#ifdef QT_NEW
            d->updateTexture = true;
#endif

            update();
        }
        else d->pixmapDefault = QPixmap();
    }

    emit sourceDefaultChanged();
}

//-------------------------------------------------------------------------------------------------

QSize WDeclarativeImageBase::sourceSize() const
{
    Q_D(const WDeclarativeImageBase); return d->sourceSize;

    /*if (d->explicitSize)
    {
        return d->sourceSize;
    }
    else return QSize(implicitWidth(), implicitHeight());*/
}

void WDeclarativeImageBase::setSourceSize(const QSize & size)
{
    Q_D(WDeclarativeImageBase);

    if (d->sourceSize == size) return;

    d->sourceSize = size;

    int width  = size.width ();
    int height = size.height();

    // NOTE: Size can be valid even when the other half is -1.
    if (width > 0 || height > 0)
    {
        // NOTE: Making sure sourceSize.isValid returns true. This is important to avoid
        //       unnecessary scaling in ImageScale(s).
        if (width < 0)
        {
            d->sourceSize.setWidth(0);
        }
        else if (height < 0)
        {
            d->sourceSize.setHeight(0);
        }

        d->setExplicitSize(true);
    }
    else d->setExplicitSize(false);

    reload();

    emit sourceSizeChanged();
}

void WDeclarativeImageBase::resetSourceSize()
{
    Q_D(WDeclarativeImageBase);

    if (d->explicitSize == false) return;

    d->sourceSize = QSize();

    d->setExplicitSize(false);

    reload();

    emit sourceSizeChanged();
}

//-------------------------------------------------------------------------------------------------

QSize WDeclarativeImageBase::defaultSize() const
{
    Q_D(const WDeclarativeImageBase); return d->defaultSize;
}

void WDeclarativeImageBase::setDefaultSize(const QSize & size)
{
    Q_D(WDeclarativeImageBase);

    if (d->defaultSize == size) return;

    d->defaultSize = size;

    emit defaultSizeChanged();
}

void WDeclarativeImageBase::resetDefaultSize()
{
    Q_D(WDeclarativeImageBase);

    if (d->defaultSize.isValid() == false) return;

    d->defaultSize = QSize();

    emit defaultSizeChanged();
}

//-------------------------------------------------------------------------------------------------

QSize WDeclarativeImageBase::sourceArea() const
{
    Q_D(const WDeclarativeImageBase); return d->sourceArea;
}

void WDeclarativeImageBase::setSourceArea(const QSize & size)
{
    Q_D(WDeclarativeImageBase);

    if (d->sourceArea == size) return;

    d->sourceArea = size;

    reload();

    emit sourceAreaChanged();
}

void WDeclarativeImageBase::resetSourceArea()
{
    Q_D(WDeclarativeImageBase);

    if (d->sourceArea.isValid() == false) return;

    d->sourceArea = QSize();

    reload();

    emit sourceAreaChanged();
}

//-------------------------------------------------------------------------------------------------

WDeclarativeImageBase::LoadMode WDeclarativeImageBase::loadMode() const
{
    Q_D(const WDeclarativeImageBase); return d->loadMode;
}

void WDeclarativeImageBase::setLoadMode(LoadMode mode)
{
    Q_D(WDeclarativeImageBase);

    if (d->loadMode == mode) return;

    d->loadMode = mode;

    if (mode != LoadVisible)
    {
        d->loadVisible();
    }

    emit loadModeChanged();
}

//-------------------------------------------------------------------------------------------------

bool WDeclarativeImageBase::asynchronous() const
{
    Q_D(const WDeclarativeImageBase); return d->asynchronous;
}

void WDeclarativeImageBase::setAsynchronous(bool enabled)
{
    Q_D(WDeclarativeImageBase);

    if (d->asynchronous == enabled) return;

    d->asynchronous = enabled;

    if (enabled && d->status == Loading)
    {
        load();
    }

    emit asynchronousChanged();
}

//-------------------------------------------------------------------------------------------------

bool WDeclarativeImageBase::cache() const
{
    Q_D(const WDeclarativeImageBase); return d->cache;
}

void WDeclarativeImageBase::setCache(bool enabled)
{
    Q_D(WDeclarativeImageBase);

    if (d->cache == enabled) return;

    d->cache = enabled;

    if (isComponentComplete()) load();

    emit cacheChanged();
}

//-------------------------------------------------------------------------------------------------

qreal WDeclarativeImageBase::progress() const
{
    Q_D(const WDeclarativeImageBase); return d->progress;
}

//-------------------------------------------------------------------------------------------------

WImageFilter * WDeclarativeImageBase::filter() const
{
    Q_D(const WDeclarativeImageBase); return d->filter;
}

void WDeclarativeImageBase::setFilter(WImageFilter * filter)
{
    Q_D(WDeclarativeImageBase);

    if (d->filter == filter) return;

    if (d->filter)
    {
        disconnect(d->filter, 0, this, 0);
    }

    d->filter = filter;

    if (filter)
    {
        connect(filter, SIGNAL(filterUpdated()), this, SLOT(onFilterUpdated()));

        // NOTE: Sometimes the filter gets destroyed before the image.
        connect(filter, SIGNAL(destroyed()), this, SLOT(onFilterClear()));
    }

    d->onFilterUpdated();

    emit filterChanged();
}

//-------------------------------------------------------------------------------------------------

qreal WDeclarativeImageBase::ratioWidth()  const
{
    Q_D(const WDeclarativeImageBase);

    if (d->pix.isNull()) return 1.0;

    const QImage & image = d->pix.pixmap().toImage();

    return (qreal) image.width() / image.height();
}

qreal WDeclarativeImageBase::ratioHeight() const
{
    Q_D(const WDeclarativeImageBase);

    if (d->pix.isNull()) return 1.0;

    const QImage & image = d->pix.pixmap().toImage();

    return (qreal) image.height() / image.width();
}

#endif // SK_NO_DECLARATIVEIMAGEBASE
