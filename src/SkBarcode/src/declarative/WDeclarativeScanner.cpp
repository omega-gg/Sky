//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkBarcode.

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

#include "WDeclarativeScanner.h"

#ifndef SK_NO_DECLARATIVESCANNER

// Sk includes
#include <WDeclarativePlayer>
#include <WDeclarativeImage>
#include <WBarcodeReader>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

WDeclarativeScannerPrivate::WDeclarativeScannerPrivate(WDeclarativeScanner * p)
    : WDeclarativeItemPrivate(p) {}

void WDeclarativeScannerPrivate::init()
{
    ratioX = 0.0;
    ratioY = 0.0;

    rectX = 0.0;
    rectY = 0.0;

#ifdef QT_4
    Q_Q(WDeclarativeScanner);

    q->setFlag(QGraphicsItem::ItemHasNoContents, false);
#endif
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WDeclarativeScannerPrivate::onLoaded(const WBarcodeResult & result)
{
    Q_Q(WDeclarativeScanner);

    QString text = result.text;

    if (text.isEmpty() == false)
    {
        QRect rect = result.rect;

        emit q->loaded(text, QRectF((qreal) rect.x     () / ratioX + rectX,
                                    (qreal) rect.y     () / ratioY + rectY,
                                    (qreal) rect.width () / ratioX,
                                    (qreal) rect.height() / ratioY));
    }
    else emit q->loaded(text, QRectF());
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
/* explicit */ WDeclarativeScanner::WDeclarativeScanner(QDeclarativeItem * parent)
#else
/* explicit */ WDeclarativeScanner::WDeclarativeScanner(QQuickItem * parent)
#endif
    : WDeclarativeItem(new WDeclarativeScannerPrivate(this), parent)
{
    Q_D(WDeclarativeScanner); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ bool WDeclarativeScanner::scanFrame(WDeclarativePlayer * player,
                                                      WDeclarativeImage  * cover, int x, int y)
{
    Q_ASSERT(player);
    Q_ASSERT(cover);

    QRectF rect;
    QImage image;

    if (cover->isVisible())
    {
        rect = cover->getRect();

        if (rect.contains(x, y) == false) return false;

        image = cover->toImage();
    }
    else
    {
        rect = player->getRect();

        if (rect.contains(x, y) == false) return false;

        image = player->getFrame();
    }

    Q_D(WDeclarativeScanner);

    d->ratioX = (qreal) image.width () / rect.width ();
    d->ratioY = (qreal) image.height() / rect.height();

    d->rectX = rect.x();
    d->rectY = rect.y();

    x = (x - d->rectX) * d->ratioX;
    y = (y - d->rectY) * d->ratioY;

    WBarcodeReader::startScan(image, x, y, WBarcodeReader::QRCode,
                              this, SLOT(onLoaded(const WBarcodeResult &)));

    return true;
}

#endif // SK_NO_DECLARATIVESCANNER
