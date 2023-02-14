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
#include <WAbstractThreadAction>

//-------------------------------------------------------------------------------------------------
// Static variables

static const int SCANNER_INTERVAL = 20;

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

WDeclarativeScannerPrivate::WDeclarativeScannerPrivate(WDeclarativeScanner * p)
    : WDeclarativeItemPrivate(p) {}

void WDeclarativeScannerPrivate::init()
{
    player = NULL;
    cover  = NULL;

    x = -1;
    y = -1;

    size = -1;

    timerId = -1;

    count        = 0;
    currentCount = 0;

    interval = SCANNER_INTERVAL;

#ifdef QT_4
    Q_Q(WDeclarativeScanner);

    q->setFlag(QGraphicsItem::ItemHasNoContents, false);
#endif
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

bool WDeclarativeScannerPrivate::scan()
{
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

    Q_Q(WDeclarativeScanner);

    qreal ratioX = (qreal) image.width () / rect.width ();
    qreal ratioY = (qreal) image.height() / rect.height();

    qreal rectX = rect.x();
    qreal rectY = rect.y();

    int currentX = (x - rectX) * ratioX;
    int currentY = (y - rectY) * ratioY;

    int currentSize;

    if (size > 1) currentSize = size * qMin(ratioX, ratioY);
    else          currentSize = size;

    WDeclarativeScannerData data;

    data.ratioX = ratioX;
    data.ratioY = ratioY;

    data.rectX = rectX;
    data.rectY = rectY;

    data.action = WBarcodeReader::startScan(image, currentX, currentY, currentSize,
                                            WBarcodeReader::QRCode,
                                            q, SLOT(onLoaded(const WBarcodeResult &)));

    datas.append(data);

    return true;
}

void WDeclarativeScannerPrivate::stopTimer()
{
    if (timerId == -1) return;

    q_func()->killTimer(timerId);

    timerId = -1;
}

void WDeclarativeScannerPrivate::clearCount()
{
    currentCount = 0;

    clearData();
}

void WDeclarativeScannerPrivate::clearData()
{
    foreach (const WDeclarativeScannerData & data, datas)
    {
        data.action->abortAndDelete();
    }

    datas.clear();
}

void WDeclarativeScannerPrivate::clearItem()
{
    Q_Q(WDeclarativeScanner);

    if (timerId != -1)
    {
        q->killTimer(timerId);

        timerId = -1;
    }

    if (currentCount == 0) return;

    clearCount();

    emit q->loaded(QString(), QRectF());
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WDeclarativeScannerPrivate::onLoaded(const WBarcodeResult & result)
{
    Q_Q(WDeclarativeScanner);

    if (currentCount == 0) return;

    WDeclarativeScannerData data = datas.takeFirst();

    QString text = result.text;

    if (text.isEmpty() == false)
    {
        stopTimer();

        clearCount();

        QRect rect = result.rect;

        qreal ratioX = data.ratioX;
        qreal ratioY = data.ratioY;

        emit q->loaded(text, QRectF((qreal) rect.x     () / ratioX + data.rectX,
                                    (qreal) rect.y     () / ratioY + data.rectY,
                                    (qreal) rect.width () / ratioX,
                                    (qreal) rect.height() / ratioY));
    }
    else if (currentCount == count)
    {
        clearCount();

        emit q->loaded(text, QRectF());
    }
}

void WDeclarativeScannerPrivate::onClearPlayer()
{
    Q_Q(WDeclarativeScanner); q->setPlayer(NULL);
}

void WDeclarativeScannerPrivate::onClearCover()
{
    Q_Q(WDeclarativeScanner); q->setCover(NULL);
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

/* Q_INVOKABLE */ bool WDeclarativeScanner::scanFrame(int x, int y, int size, int count)
{
    Q_D(WDeclarativeScanner);

    if (d->player == NULL || d->cover == NULL) return false;

    d->stopTimer();

    d->clearData();

    d->x = x;
    d->y = y;

    d->size = size;

    d->currentCount = 1;

    if (count < 2)
    {
        d->count = 1;

        return d->scan();
    }
    else
    {
        d->count = count;

        if (d->scan() == false) return false;

        d->timerId = startTimer(d->interval);

        return true;
    }
}

//-------------------------------------------------------------------------------------------------
// Protected events
//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativeScanner::timerEvent(QTimerEvent *)
{
    Q_D(WDeclarativeScanner);

    d->currentCount++;

    if (d->scan() == false)
    {
        killTimer(d->timerId);

        d->timerId = -1;

        d->clearCount();

        emit loaded(QString(), QRectF());

        return;
    }

    if (d->currentCount == d->count)
    {
        killTimer(d->timerId);

        d->timerId = -1;
    }
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

WDeclarativePlayer * WDeclarativeScanner::player() const
{
    Q_D(const WDeclarativeScanner); return d->player;
}

void WDeclarativeScanner::setPlayer(WDeclarativePlayer * player)
{
    Q_D(WDeclarativeScanner);

    if (d->player == player) return;

    d->clearItem();

    if (d->player) disconnect(player, 0, this, 0);

    d->player = player;

    if (player) connect(player, SIGNAL(destroyed()), this, SLOT(onClearPlayer()));

    emit playerChanged();
}

WDeclarativeImage * WDeclarativeScanner::cover() const
{
    Q_D(const WDeclarativeScanner); return d->cover;
}

void WDeclarativeScanner::setCover(WDeclarativeImage * cover)
{
    Q_D(WDeclarativeScanner);

    if (d->cover == cover) return;

    d->clearItem();

    if (d->cover) disconnect(cover, 0, this, 0);

    d->cover = cover;

    if (cover) connect(cover, SIGNAL(destroyed()), this, SLOT(onClearCover()));

    emit coverChanged();
}

//-------------------------------------------------------------------------------------------------

int WDeclarativeScanner::interval() const
{
    Q_D(const WDeclarativeScanner); return d->interval;
}

void WDeclarativeScanner::setInterval(int interval)
{
    Q_D(WDeclarativeScanner);

    if (d->interval == interval) return;

    d->interval = interval;

    emit intervalChanged();
}

#endif // SK_NO_DECLARATIVESCANNER
