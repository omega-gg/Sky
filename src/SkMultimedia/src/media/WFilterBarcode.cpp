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

// Qt includes
#ifdef QT_5
#include <QTimer>
#else
#include <QVideoFrame>
#endif

#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
// Qt private includes
// NOTE: This is required for qt_convert functions.
#include <private/qvideoframeconversionhelper_p.h>
#endif

//-------------------------------------------------------------------------------------------------
// Static variables

static const int FILTERBARCODE_INTERVAL = 200;

#ifdef QT_5

//=================================================================================================
// WFilterRunnable
//=================================================================================================

class WFilterRunnable : public QVideoFilterRunnable
{
public:
    explicit WFilterRunnable(WFilterBarcode * filter);

    /* virtual */ ~WFilterRunnable();

#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
public: // Functions
    QImage imageFromFrame(const QVideoFrame & frame) const;
#endif

public: // QVideoFilterRunnable implementation
    /* virtual */ QVideoFrame run(QVideoFrame * frame,
                                  const QVideoSurfaceFormat & surfaceFormat, RunFlags flags);

private: // Variables
    WFilterBarcode * filter;

    // NOTE: We declare the timer here to start it from the runnable thread. This needs to be a
    //       pointer otherwise it gets stopped from another thread which triggers a warning.
    QTimer * timer;
};

/* explicit */ WFilterRunnable::WFilterRunnable(WFilterBarcode * filter)
{
    this->filter = filter;

    timer = NULL;
}

/* virtual */ WFilterRunnable::~WFilterRunnable()
{
    if (timer) timer->deleteLater();
}

#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)

void imageCleanup(void * data)
{
    delete [] (uchar *) data;
}

QImage WFilterRunnable::imageFromFrame(const QVideoFrame & frame) const
{
    QImage::Format format = frame.imageFormatFromPixelFormat(frame.pixelFormat());

    if (format == QImage::Format_Invalid)
    {
        return QImage(frame.bits(), frame.width(), frame.height(), format);
    }

    uchar * data = new uchar[(frame.width() * frame.height()) * 4];

    if      (format == QVideoFrame::Format_BGRA32)  qt_convert_BGRA32_to_ARGB32 (frame, data);
    else if (format == QVideoFrame::Format_BGR24)   qt_convert_BGR24_to_ARGB32  (frame, data);
    else if (format == QVideoFrame::Format_BGR565)  qt_convert_BGR565_to_ARGB32 (frame, data);
    else if (format == QVideoFrame::Format_BGR555)  qt_convert_BGR555_to_ARGB32 (frame, data);
    else if (format == QVideoFrame::Format_AYUV444) qt_convert_AYUV444_to_ARGB32(frame, data);
    else if (format == QVideoFrame::Format_YUV444)  qt_convert_YUV444_to_ARGB32 (frame, data);
    else if (format == QVideoFrame::Format_YUV420P) qt_convert_YUV420P_to_ARGB32(frame, data);
    else if (format == QVideoFrame::Format_YV12)    qt_convert_YV12_to_ARGB32   (frame, data);
    else if (format == QVideoFrame::Format_UYVY)    qt_convert_UYVY_to_ARGB32   (frame, data);
    else if (format == QVideoFrame::Format_YUYV)    qt_convert_YUYV_to_ARGB32   (frame, data);
    else if (format == QVideoFrame::Format_NV12)    qt_convert_NV12_to_ARGB32   (frame, data);
    else if (format == QVideoFrame::Format_NV21)    qt_convert_NV21_to_ARGB32   (frame, data);

    return QImage(data, frame.width(), frame.height(), QImage::Format_ARGB32, imageCleanup, data);
}

#endif

/* virtual */ QVideoFrame WFilterRunnable::run(QVideoFrame * frame, const QVideoSurfaceFormat &,
                                               QVideoFilterRunnable::RunFlags)
{
    WFilterBarcodePrivate * p = filter->d_func();

    // NOTE: We wait for the last run to finish before starting a new one.
    if (p->loading) return *frame;

    if (timer == NULL)
    {
        timer = new QTimer;

        timer->setInterval(filter->d_func()->interval);

        timer->setSingleShot(true);
    }
    else if (timer->isActive()) return *frame;

    p->loading = true;

    QImage image;

#ifdef Q_OS_ANDROID
    // NOTE android: This platform is using reversed RGB for camera frames.
    if (frame->pixelFormat() == QVideoFrame::Format_ABGR32)
    {
        if (frame->map(QAbstractVideoBuffer::ReadOnly) == false) return *frame;

        // NOTE: We need to copy the QVideoFrame buffer.
        image = QImage(frame->bits(), frame->width(), frame->height(), QImage::Format_ARGB32)
                .copy();

        frame->unmap();

        // NOTE android: Swapping rgb probably does not matter for QR codes and this call impacts
        //               performances.
        //image = image.rgbSwapped();
    }
#elif QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    if (frame->map(QAbstractVideoBuffer::ReadOnly) == false) return *frame;

    image = imageFromFrame(*frame);

    frame->unmap();
#else
    image = frame->image();
#endif

#ifdef Q_OS_ANDROID
    // NOTE ZXing Pre-2.0: We need to mirror the image before scanning it.
    p->reader.startRead(image, WBarcodeReader::Any, filter, SLOT(onLoaded(const QString &)),
                        p->target, true);
#else
    p->reader.startRead(image, WBarcodeReader::Any, filter, SLOT(onLoaded(const QString &)),
                        p->target);
#endif

    timer->start();

    return *frame;
}

#endif

//=================================================================================================
// WFilterBarcodePrivate
//=================================================================================================

WFilterBarcodePrivate::WFilterBarcodePrivate(WFilterBarcode * p) : WPrivate(p) {}

void WFilterBarcodePrivate::init()
{
#ifdef QT_6
    videoSink = NULL;
#endif

    interval = FILTERBARCODE_INTERVAL;

    loading = false;

#ifdef QT_6
    timer.setInterval(interval);

    timer.setSingleShot(true);
#endif
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

#ifdef QT_6

void WFilterBarcodePrivate::onUpdated(const QVideoFrame & frame)
{
    // NOTE: We wait for the last run to finish before starting a new one.
    if (loading || timer.isActive()) return;

    Q_Q(WFilterBarcode);

    loading = true;

    reader.startRead(frame.toImage(), WBarcodeReader::Any, q, SLOT(onLoaded(const QString &)),
                     target);

    timer.start();
}

#endif

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
#ifdef QT_5
    : QAbstractVideoFilter(parent), WPrivatable(new WFilterBarcodePrivate(this))
#else
    : QObject(parent), WPrivatable(new WFilterBarcodePrivate(this))
#endif
{
    Q_D(WFilterBarcode); d->init();
}

//-------------------------------------------------------------------------------------------------
// Static functions
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QPoint WFilterBarcode::mapPointToSource(const QRect  & source,
                                                                 const QRect  & content,
                                                                 const QPoint & point,
                                                                 int            orientation)
{
    qreal x = (qreal) (point.x() - content.left()) / content.width ();
    qreal y = (qreal) (point.y() - content.top ()) / content.height();

    //---------------------------------------------------------------------------------------------
    // NOTE: We normalize the orientation.

    orientation %= 360;

    if (orientation < 0) orientation += 360;

    //---------------------------------------------------------------------------------------------

    if (orientation == 90)
    {
        qreal temp = x;

        x = 1.0 - y;
        y = temp;
    }
    else if (orientation == 180)
    {
        x = 1.0 - x;
        y = 1.0 - y;
    }
    else if (orientation == 270)
    {
        qreal temp = x;

        x = y;
        y = 1.0 - temp;
    }

    return QPoint(x * source.width(), y * source.height());
}

/* Q_INVOKABLE static */ QRect WFilterBarcode::mapRectToSource(const QRect & source,
                                                               const QRect & content,
                                                               const QRect & target,
                                                               int           orientation)
{
    return QRect(mapPointToSource(source, content, target.topLeft    (), orientation),
                 mapPointToSource(source, content, target.bottomRight(), orientation))
           .normalized();
}

#ifdef QT_5

//-------------------------------------------------------------------------------------------------
// QAbstractVideoFilter implementation
//-------------------------------------------------------------------------------------------------

/* virtual */ QVideoFilterRunnable * WFilterBarcode::createFilterRunnable()
{
    return new WFilterRunnable(this);
}

#endif

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

#ifdef QT_6

QVideoSink * WFilterBarcode::videoSink() const
{
    Q_D(const WFilterBarcode); return d->videoSink;
}

void WFilterBarcode::setVideoSink(QVideoSink * videoSink)
{
    Q_D(WFilterBarcode);

    if (d->videoSink == videoSink) return;

    if (d->videoSink) disconnect(d->videoSink, 0, this, 0);

    d->videoSink = videoSink;

    if (videoSink)
    {
        connect(d->videoSink, SIGNAL(videoFrameChanged(const QVideoFrame &)),
                this,         SLOT  (onUpdated        (const QVideoFrame &)));
    }

    emit videoSinkChanged();
}

#endif

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

bool WFilterBarcode::interval() const
{
    Q_D(const WFilterBarcode); return d->interval;
}

void WFilterBarcode::setInterval(bool interval)
{
    Q_D(WFilterBarcode);

    if (d->interval == interval) return;

    d->interval = interval;

    emit intervalChanged();
}

#endif // SK_NO_FILTERBARCODE
