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

#include "WImageFilter.h"

// Qt includes
#include <QPixmap>

#ifndef SK_NO_IMAGEFILTER

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include "WImageFilter_p.h"

WImageFilterPrivate::WImageFilterPrivate(WImageFilter * p) : WPrivate(p) {}

void WImageFilterPrivate::init()
{
    autoUpdate    = true;
    updatePending = false;
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
    else d->updatePending = true;
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

    if (d->autoUpdate && d->updatePending)
    {
        updateFilter();
    }
    else d->updatePending = false;

    emit autoUpdateChanged();
}

#endif // SK_NO_IMAGEFILTER
