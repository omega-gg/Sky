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

static const int SCANNER_INTERVAL =  20;
static const int SCANNER_DURATION = 300; // NOTE: Wait for 300ms maximum and then abort.

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

    timerIdA = -1;
    timerIdB = -1;

    count        = 0;
    currentCount = 0;

    interval = SCANNER_INTERVAL;
    duration = SCANNER_DURATION;

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

    if (cover && cover->isVisible())
    {
        rect = cover->getRect();

        if (rect.contains(x, y) == false) return false;

        image = cover->toImage();
    }
    else
    {
        if (player->isVisible() == false) return false;

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

//-------------------------------------------------------------------------------------------------

void WDeclarativeScannerPrivate::stopTimer()
{
    if (timerIdA == -1) return;

    clearTimer();
}

void WDeclarativeScannerPrivate::clearTimer()
{
    Q_Q(WDeclarativeScanner);

    q->killTimer(timerIdA);
    q->killTimer(timerIdB);

    timerIdA = -1;
    timerIdB = -1;
}

//-------------------------------------------------------------------------------------------------

void WDeclarativeScannerPrivate::clearCount()
{
    currentCount = 0;

    clearData();
}

void WDeclarativeScannerPrivate::clearResult()
{
    text = QString();

    rect = QRectF();

    topLeft     = QPointF();
    topRight    = QPointF();
    bottomLeft  = QPointF();
    bottomRight = QPointF();
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
    stopTimer();

    if (currentCount == 0) return;

    Q_Q(WDeclarativeScanner);

    clearCount ();
    clearResult();

    emit q->loaded();
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

        qreal rectX = data.rectX;
        qreal rectY = data.rectY;

        this->text = text;

        this->rect = QRectF((qreal) rect.x     () / ratioX + rectX,
                            (qreal) rect.y     () / ratioY + rectY,
                            (qreal) rect.width () / ratioX,
                            (qreal) rect.height() / ratioY);

        QPoint topLeft     = result.topLeft;
        QPoint topRight    = result.topRight;
        QPoint bottomLeft  = result.bottomLeft;
        QPoint bottomRight = result.bottomRight;

        this->topLeft = QPointF((qreal) topLeft.x() / ratioX + rectX,
                                (qreal) topLeft.y() / ratioY + rectY);

        this->topRight = QPointF((qreal) topRight.x() / ratioX + rectX,
                                 (qreal) topRight.y() / ratioY + rectY);

        this->bottomLeft = QPointF((qreal) bottomLeft.x() / ratioX + rectX,
                                   (qreal) bottomLeft.y() / ratioY + rectY);

        this->bottomRight = QPointF((qreal) bottomRight.x() / ratioX + rectX,
                                    (qreal) bottomRight.y() / ratioY + rectY);

        emit q->loaded();
    }
    else if (currentCount == count)
    {
        clearCount();

        this->text = text;

        rect = QRectF();

        topLeft     = QPointF();
        topRight    = QPointF();
        bottomLeft  = QPointF();
        bottomRight = QPointF();

        emit q->loaded();
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

    if (d->player == NULL && d->cover == NULL) return false;

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

        d->timerIdA = startTimer(d->interval);
        d->timerIdB = startTimer(d->duration);

        return true;
    }
}

//-------------------------------------------------------------------------------------------------
// Protected events
//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativeScanner::timerEvent(QTimerEvent * event)
{
    Q_D(WDeclarativeScanner);

    int id = event->timerId();

    if (id == d->timerIdA)
    {
        d->currentCount++;

        if (d->scan() == false)
        {
            d->clearTimer ();
            d->clearCount ();
            d->clearResult();

            emit loaded();
        }
        else if (d->currentCount == d->count)
        {
            d->clearTimer();
        }
    }
    else // if (id == d->timerIdB)
    {
        d->clearTimer ();
        d->clearCount ();
        d->clearResult();

        emit loaded();
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

    if (d->player) disconnect(d->player, 0, this, 0);

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

    if (d->cover) disconnect(d->cover, 0, this, 0);

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

int WDeclarativeScanner::duration() const
{
    Q_D(const WDeclarativeScanner); return d->duration;
}

void WDeclarativeScanner::setDuration(int duration)
{
    Q_D(WDeclarativeScanner);

    if (d->duration == duration) return;

    d->duration = duration;

    emit durationChanged();
}

//-------------------------------------------------------------------------------------------------

QString WDeclarativeScanner::text() const
{
    Q_D(const WDeclarativeScanner); return d->text;
}

QRectF WDeclarativeScanner::rect() const
{
    Q_D(const WDeclarativeScanner); return d->rect;
}

QPointF WDeclarativeScanner::topLeft() const
{
    Q_D(const WDeclarativeScanner); return d->topLeft;
}

QPointF WDeclarativeScanner::topRight() const
{
    Q_D(const WDeclarativeScanner); return d->topRight;
}

QPointF WDeclarativeScanner::bottomLeft() const
{
    Q_D(const WDeclarativeScanner); return d->bottomLeft;
}

QPointF WDeclarativeScanner::bottomRight() const
{
    Q_D(const WDeclarativeScanner); return d->bottomRight;
}

#endif // SK_NO_DECLARATIVESCANNER
