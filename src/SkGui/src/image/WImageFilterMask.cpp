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

#include "WImageFilterMask.h"

#ifndef SK_NO_IMAGEFILTERMASK

// Qt includes
#include <QPainter>
#ifndef QT_5
#include <QHash>
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
#include <QPainterPath>
#endif

//-------------------------------------------------------------------------------------------------
// Static variables

static const int IMAGEFILTERMASK_MAX = 10;

//-------------------------------------------------------------------------------------------------
// Inline functions

inline uint qHash(const QSize & key)
{
    // NOTE: It seems I need to initialize bytes to 0 for this to work.
    QByteArray array(sizeof(int) * 4, 0);

    int * data = reinterpret_cast<int *> (array.data());

    *data = key.width();
    data++;

    *data = key.height();

    return qHash(array);
}

//=================================================================================================
// WImageFilterMaskPrivate
//=================================================================================================

#include <private/WImageFilter_p>

class SK_GUI_EXPORT WImageFilterMaskPrivate : public WImageFilterPrivate
{
public:
    WImageFilterMaskPrivate(WImageFilter * p);

    void init();

    QImage pushMask(const QSize & size, qreal ratio);

    void clearMasks();

    void updateCache();

public: // Variables
    QHash<QSize, QImage> masks;
    QList<QSize>         sizes;

    int width;
    int height;

    int radius;

    bool checkSize;

    int maxCache;

protected:
    W_DECLARE_PUBLIC(WImageFilterMask)
};

//-------------------------------------------------------------------------------------------------

WImageFilterMaskPrivate::WImageFilterMaskPrivate(WImageFilter * p) : WImageFilterPrivate(p) {}

void WImageFilterMaskPrivate::init()
{
    update = false;

    width  = 32;
    height = 32;

    checkSize = false;

    radius = 8;

    maxCache = IMAGEFILTERMASK_MAX;
}

//-------------------------------------------------------------------------------------------------

QImage WImageFilterMaskPrivate::pushMask(const QSize & size, qreal ratio)
{
#ifdef QT_4
    QImage mask(size, QImage::Format_ARGB32_Premultiplied);
#else
    QImage mask(size, QImage::Format_Alpha8);
#endif

    mask.fill(Qt::transparent);

    QPainter painter(&mask);

    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath path;

    //---------------------------------------------------------------------------------------------
    // NOTE: We take the pixel ratio into account.

    int width  = this->width  * ratio;
    int height = this->height * ratio;

    int radius = this->radius * ratio;

    //---------------------------------------------------------------------------------------------

    int x = (size.width () - width)  / 2;
    int y = (size.height() - height) / 2;

    path.addRoundedRect(x, y, width, height, radius, radius);

    painter.fillPath(path, Qt::black);

    painter.drawPath(path);

    masks.insert(size, mask);

    sizes.append(size);

    updateCache();

    return mask;
}

//-------------------------------------------------------------------------------------------------

void WImageFilterMaskPrivate::clearMasks()
{
    masks.clear();
    sizes.clear();
}

//-------------------------------------------------------------------------------------------------

void WImageFilterMaskPrivate::updateCache()
{
    while (sizes.count() > maxCache)
    {
        masks.remove(sizes.takeFirst());
    }
}

//=================================================================================================
// WImageFilterMask
//=================================================================================================

/* explicit */ WImageFilterMask::WImageFilterMask(QObject * parent)
    : WImageFilter(new WImageFilterMaskPrivate(this), parent)
{
    Q_D(WImageFilterMask); d->init();
}

//-------------------------------------------------------------------------------------------------
// WImageFilter implementation
//-------------------------------------------------------------------------------------------------

/* virtual */ bool WImageFilterMask::filter(QImage * image, qreal ratio)
{
    Q_D(WImageFilterMask);

    QSize size = image->size();

    if (d->checkSize && (qAbs(size.width() - d->width) > d->radius
                         ||
                         qAbs(size.height() - d->height) > d->radius)) return false;

    QImage mask = d->masks.value(size);

    if (mask.isNull() == false)
    {
        d->sizes.removeOne(size);
        d->sizes.append   (size);
    }
    else mask = d->pushMask(size, ratio);

#ifdef QT_4
    image->setAlphaChannel(mask.alphaChannel());
#else
    image->setAlphaChannel(mask);
#endif

    return true;
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

int WImageFilterMask::width() const
{
    Q_D(const WImageFilterMask); return d->width;
}

void WImageFilterMask::setWidth(int width)
{
    Q_D(WImageFilterMask);

    if (d->width == width) return;

    d->width = width;

    d->clearMasks();

    refreshFilter();

    emit widthChanged();
}

int WImageFilterMask::height() const
{
    Q_D(const WImageFilterMask); return d->height;
}

void WImageFilterMask::setHeight(int height)
{
    Q_D(WImageFilterMask);

    if (d->height == height) return;

    d->height = height;

    d->clearMasks();

    refreshFilter();

    emit heightChanged();
}

int WImageFilterMask::radius() const
{
    Q_D(const WImageFilterMask); return d->radius;
}

void WImageFilterMask::setRadius(int radius)
{
    Q_D(WImageFilterMask);

    if (d->radius == radius) return;

    d->radius = radius;

    d->clearMasks();

    refreshFilter();

    emit widthChanged();
}

//-------------------------------------------------------------------------------------------------

bool WImageFilterMask::checkSize() const
{
    Q_D(const WImageFilterMask); return d->checkSize;
}

void WImageFilterMask::setCheckSize(bool check)
{
    Q_D(WImageFilterMask);

    if (d->checkSize == check) return;

    d->checkSize = check;

    d->clearMasks();

    refreshFilter();

    emit checkSizeChanged();
}

//-------------------------------------------------------------------------------------------------

int WImageFilterMask::maxCache() const
{
    Q_D(const WImageFilterMask); return d->maxCache;
}

void WImageFilterMask::setMaxCache(int max)
{
    Q_D(WImageFilterMask);

    if (d->maxCache == max) return;

    // NOTE: We want to avoid the infinite loop in updateCache()
    if (max < 1)
    {
         d->maxCache = 1;
    }
    else d->maxCache = max;

    d->updateCache();

    emit maxCacheChanged();
}

#endif // SK_NO_IMAGEFILTERMASK
