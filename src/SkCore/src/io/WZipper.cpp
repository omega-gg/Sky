//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkCore.

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

#include "WZipper.h"

#ifndef SK_NO_ZIPPER

// Qt includes
#include <QBuffer>

// QuaZip includes
#include <quaziodevice.h>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include <private/Sk_p>

class SK_CORE_EXPORT WZipperPrivate : public WPrivate
{
public:
    WZipperPrivate(WZipper * p);

    void init();

protected:
    W_DECLARE_PUBLIC(WZipper)
};

//-------------------------------------------------------------------------------------------------

WZipperPrivate::WZipperPrivate(WZipper * p) : WPrivate(p) {}

void WZipperPrivate::init() {}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

/* explicit */ WZipper::WZipper(QObject * parent)
    : QObject(parent), WPrivatable(new WZipperPrivate(this))
{
    Q_D(WZipper); d->init();
}

//-------------------------------------------------------------------------------------------------
// Static functions
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QByteArray WZipper::compress(const QByteArray & array)
{
    QBuffer buffer;

    buffer.open(QIODevice::WriteOnly);

    QuaZIODevice device(&buffer);

    device.open(QIODevice::WriteOnly);

    device.write(array.data(), array.size());

    device.flush();

    device.close();
    buffer.close();

    buffer.open(QIODevice::ReadOnly);

    return buffer.readAll();
}

#endif // SK_NO_ZIPPER
