//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkMultimedia.

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

#include "WFilterBarcode.h"

#ifndef SK_NO_FILTERBARCODE

//=================================================================================================
// WFilterRunnable
//=================================================================================================

class WFilterRunnable : public QVideoFilterRunnable
{
public:
    explicit WFilterRunnable(WFilterBarcode * filter);

public: // QVideoFilterRunnable implementation
    /* virtual */ QVideoFrame run(QVideoFrame * input,
                                  const QVideoSurfaceFormat & surfaceFormat, RunFlags flags);

private: // Variables
    WFilterBarcode * filter;
};

/* explicit */ WFilterRunnable::WFilterRunnable(WFilterBarcode * filter)
{
    this->filter = filter;
}

/* virtual */ QVideoFrame WFilterRunnable::run(QVideoFrame * input, const QVideoSurfaceFormat &,
                                               QVideoFilterRunnable::RunFlags)
{
    WFilterBarcodePrivate * p = filter->d_func();

    // NOTE: We wait for the last run to finish before starting a new one.
    if (p->loading) return *input;

    p->loading = true;

#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    input->map(QAbstractVideoBuffer::ReadOnly);

    QImage image = imageFromVideoFrame(videoFrame);

    input->unmap();
#else
    QImage image = input->image();
#endif

    p->reader.startRead(image, WBarcodeReader::Any, filter, SLOT(onLoaded(const QString &)),
                        p->target);

    return *input;
}

//=================================================================================================
// WFilterBarcodePrivate
//=================================================================================================

WFilterBarcodePrivate::WFilterBarcodePrivate(WFilterBarcode * p) : WPrivate(p) {}

void WFilterBarcodePrivate::init()
{
    loading = false;
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WFilterBarcodePrivate::onLoaded(const QString & text)
{
    loading = false;

    if (text.isEmpty()) return;

    Q_Q(WFilterBarcode);

    emit q->loaded(text);
}

//=================================================================================================
// WFilterBarcode
//=================================================================================================

/* explicit */ WFilterBarcode::WFilterBarcode(QObject * parent)
    : QAbstractVideoFilter(parent), WPrivatable(new WFilterBarcodePrivate(this))
{
    Q_D(WFilterBarcode); d->init();
}

//-------------------------------------------------------------------------------------------------
// QAbstractVideoFilter implementation
//-------------------------------------------------------------------------------------------------

/* virtual */ QVideoFilterRunnable * WFilterBarcode::createFilterRunnable()
{
    return new WFilterRunnable(this);
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

QRect WFilterBarcode::target() const
{
    Q_D(const WFilterBarcode); return d->target;
}

void WFilterBarcode::setTarget(const QRect & target)
{
    Q_D(WFilterBarcode);

    if (d->target == target) return;

    d->target = target;

    emit targetChanged();
}

#endif // SK_NO_FILTERBARCODE
