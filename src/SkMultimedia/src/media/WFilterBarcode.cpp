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

#ifdef QT_6
// Qt includes
#include <QVideoFrame>
#endif

#ifdef QT_5

//=================================================================================================
// WFilterRunnable
//=================================================================================================

class WFilterRunnable : public QVideoFilterRunnable
{
public:
    explicit WFilterRunnable(WFilterBarcode * filter);

#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
public: // Functions
    QImage imageFromFrame(const QVideoFrame & frame) const;
#endif

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

#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)

extern void QT_FASTCALL qt_convert_BGRA32_to_ARGB32 (const QVideoFrame &, uchar *);
extern void QT_FASTCALL qt_convert_BGR24_to_ARGB32  (const QVideoFrame &, uchar *);
extern void QT_FASTCALL qt_convert_BGR565_to_ARGB32 (const QVideoFrame &, uchar *);
extern void QT_FASTCALL qt_convert_BGR555_to_ARGB32 (const QVideoFrame &, uchar *);
extern void QT_FASTCALL qt_convert_AYUV444_to_ARGB32(const QVideoFrame &, uchar *);
extern void QT_FASTCALL qt_convert_YUV444_to_ARGB32 (const QVideoFrame &, uchar *);
extern void QT_FASTCALL qt_convert_YUV420P_to_ARGB32(const QVideoFrame &, uchar *);
extern void QT_FASTCALL qt_convert_YV12_to_ARGB32   (const QVideoFrame &, uchar *);
extern void QT_FASTCALL qt_convert_UYVY_to_ARGB32   (const QVideoFrame &, uchar *);
extern void QT_FASTCALL qt_convert_YUYV_to_ARGB32   (const QVideoFrame &, uchar *);
extern void QT_FASTCALL qt_convert_NV12_to_ARGB32   (const QVideoFrame &, uchar *);
extern void QT_FASTCALL qt_convert_NV21_to_ARGB32   (const QVideoFrame &, uchar *);

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

/* virtual */ QVideoFrame WFilterRunnable::run(QVideoFrame * input, const QVideoSurfaceFormat &,
                                               QVideoFilterRunnable::RunFlags)
{
    WFilterBarcodePrivate * p = filter->d_func();

    // NOTE: We wait for the last run to finish before starting a new one.
    if (p->loading) return *input;

    p->loading = true;

#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    input->map(QAbstractVideoBuffer::ReadOnly);

    QImage image = imageFromFrame(*input);

    input->unmap();
#else
    QImage image = input->image();
#endif

    p->reader.startRead(image, WBarcodeReader::Any, filter, SLOT(onLoaded(const QString &)),
                        p->target);

    return *input;
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

    loading = false;
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

#ifdef QT_6

void WFilterBarcodePrivate::onUpdated(const QVideoFrame & frame)
{
    // NOTE: We wait for the last run to finish before starting a new one.
    if (loading) return;

    Q_Q(WFilterBarcode);

    loading = true;

    reader.startRead(frame.toImage(), WBarcodeReader::Any, q, SLOT(onLoaded(const QString &)),
                     target);
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
                                                                 const QPoint & point)
{
    qreal x = (qreal) (point.x() - content.left()) / content.width ();
    qreal y = (qreal) (point.y() - content.top ()) / content.height();

    return QPoint(x * source.width(), y * source.height());
}

/* Q_INVOKABLE static */ QRect WFilterBarcode::mapRectToSource(const QRect & source,
                                                               const QRect & content,
                                                               const QRect & target)
{
    return QRect(mapPointToSource(source, content, target.topLeft    ()),
                 mapPointToSource(source, content, target.bottomRight()));
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

#endif // SK_NO_FILTERBARCODE
