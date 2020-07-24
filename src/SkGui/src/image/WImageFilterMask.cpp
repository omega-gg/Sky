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

//=================================================================================================
// WImageFilterMaskPrivate
//=================================================================================================

#include <private/WImageFilter_p>

class SK_GUI_EXPORT WImageFilterMaskPrivate : public WImageFilterPrivate
{
public:
    WImageFilterMaskPrivate(WImageFilter * p);

    void init();

    void updateMask();

public: // Variables
    QImage mask;

    bool update;

    int width;
    int height;

    int radius;

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

    radius = 8;
}

//-------------------------------------------------------------------------------------------------

void WImageFilterMaskPrivate::updateMask()
{
    if (update == false) return;

    update = false;

    if (width < 1 || height < 1 || radius < 1)
    {
        mask = QImage();

        return;
    }

#ifdef QT_4
    mask = QImage(width, height, QImage::Format_ARGB32_Premultiplied);
#else
    mask = QImage(width, height, QImage::Format_Alpha8);
#endif

    mask.fill(Qt::transparent);

    QPainter painter(&mask);

    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath path;

    path.addRoundedRect(0, 0, width, height, radius, radius);

    painter.fillPath(path, Qt::black);

    painter.drawPath(path);
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

/* virtual */ bool WImageFilterMask::filter(QImage * image)
{
    Q_D(WImageFilterMask);

    d->updateMask();

    if (d->mask.size() == image->size())
    {
#ifdef QT_4
        image->setAlphaChannel(d->mask.alphaChannel());
#else
        image->setAlphaChannel(d->mask);
#endif

        return true;
    }
    else return false;
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

    d->update = true;

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

    d->update = true;

    refreshFilter();

    emit heightChanged();
}

//-------------------------------------------------------------------------------------------------

int WImageFilterMask::radius() const
{
    Q_D(const WImageFilterMask); return d->radius;
}

void WImageFilterMask::setRadius(int radius)
{
    Q_D(WImageFilterMask);

    if (d->radius == radius) return;

    d->radius = radius;

    d->update = true;

    refreshFilter();

    emit widthChanged();
}

#endif // SK_NO_IMAGEFILTERMASK
