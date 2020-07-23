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

//=================================================================================================
// WImageFilterMaskPrivate
//=================================================================================================

#include <private/WImageFilter_p>

class SK_GUI_EXPORT WImageFilterMaskPrivate : public WImageFilterPrivate
{
public:
    WImageFilterMaskPrivate(WImageFilter * p);

    void init();

protected:
    W_DECLARE_PUBLIC(WImageFilterMask)
};

//-------------------------------------------------------------------------------------------------

WImageFilterMaskPrivate::WImageFilterMaskPrivate(WImageFilter * p) : WImageFilterPrivate(p) {}

void WImageFilterMaskPrivate::init() {}

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

/* virtual */ bool WImageFilterMask::filter(QImage * image) const
{
    QImage::Format format = image->format();

    if (format != QImage::Format_RGB32
        &&
        format != QImage::Format_ARGB32
        &&
        format != QImage::Format_ARGB32_Premultiplied) return false;

    return true;
}

#endif // SK_NO_IMAGEFILTERMASK
