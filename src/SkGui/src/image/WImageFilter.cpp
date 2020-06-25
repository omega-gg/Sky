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

#include "WImageFilter.h"

#ifndef SK_NO_IMAGEFILTER

// Qt includes
#include <QPixmap>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include "WImageFilter_p.h"

WImageFilterPrivate::WImageFilterPrivate(WImageFilter * p) : WPrivate(p) {}

void WImageFilterPrivate::init()
{
    update     = false;
    autoUpdate = true;
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

/* explicit */ WImageFilter::WImageFilter(QObject * parent)
    : QObject(parent), WPrivatable(new WImageFilterPrivate(this))
{
    Q_D(WImageFilter); d->init();
}

//-------------------------------------------------------------------------------------------------
// Protected

WImageFilter::WImageFilter(WImageFilterPrivate * p, QObject * parent)
    : QObject(parent), WPrivatable(p)
{
    Q_D(WImageFilter); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

bool WImageFilter::applyFilter(QImage * image) const
{
    Q_ASSERT(image);

    if (image->isNull()) return false;

    return filter(image);
}

bool WImageFilter::applyFilter(QPixmap * pixmap) const
{
    Q_ASSERT(pixmap);

    if (pixmap->isNull()) return false;

    QImage image = pixmap->toImage();

    bool result = filter(&image);

    *pixmap = QPixmap::fromImage(image);

    return result;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WImageFilter::updateFilter()
{
    emit filterUpdated();
}

//-------------------------------------------------------------------------------------------------
// Protected slots
//-------------------------------------------------------------------------------------------------

void WImageFilter::refreshFilter()
{
    Q_D(WImageFilter);

    if (d->autoUpdate)
    {
        updateFilter();
    }
    else d->update = true;
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

bool WImageFilter::autoUpdate() const
{
    Q_D(const WImageFilter); return d->autoUpdate;
}

void WImageFilter::setAutoUpdate(bool autoUpdate)
{
    Q_D(WImageFilter);

    if (d->autoUpdate == autoUpdate) return;

    d->autoUpdate = autoUpdate;

    if (autoUpdate && d->update)
    {
        updateFilter();
    }
    else d->update = false;

    emit autoUpdateChanged();
}

#endif // SK_NO_IMAGEFILTER
