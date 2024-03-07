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

#include "WBarcodeReader.h"

#ifndef SK_NO_BARCODEREADER

// Qt includes
#include <QImage>

// ZXing includes
#include <ReadBarcode.h>

// Sk includes
#include <WControllerFile>

// Namespaces
using namespace ZXing;

//=================================================================================================
// WBarcodeRead, WBarcodeFile, WBarcodeScan, WBarcodeReadReply and WBarcodeScanReply
//=================================================================================================

class WBarcodeRead : public WAbstractThreadAction
{
    Q_OBJECT

public:
    WBarcodeRead(const QImage & image, const QRect & target, bool mirror,
                 WBarcodeReader::Formats formats)
    {
        this->image  = image;
        this->target = target;

        this->mirror = mirror;

        this->formats = formats;
    }

protected: // WAbstractThreadAction reimplementation
    /* virtual */ WAbstractThreadReply * createReply() const;

protected: // WAbstractThreadAction implementation
    /* virtual */ bool run();

public: // Variables
    QImage image;
    QRect  target;

    bool mirror;

    WBarcodeReader::Formats formats;
};

class WBarcodeFile : public WAbstractThreadAction
{
    Q_OBJECT

public:
    WBarcodeFile(const QString & fileName, WBarcodeReader::Formats formats)
    {
        this->fileName = fileName;
        this->formats  = formats;
    }

protected: // WAbstractThreadAction reimplementation
    /* virtual */ WAbstractThreadReply * createReply() const;

protected: // WAbstractThreadAction implementation
    /* virtual */ bool run();

public: // Variables
    QString fileName;

    WBarcodeReader::Formats formats;
};

class WBarcodeScan : public WAbstractThreadAction
{
    Q_OBJECT

public:
    WBarcodeScan(const QImage & image, int x, int y, int size, int precision,
                 WBarcodeReader::Formats formats)
    {
        this->image = image;

        this->x = x;
        this->y = y;

        this->size = size;

        this->precision = precision;

        this->formats = formats;
    }

protected: // WAbstractThreadAction reimplementation
    /* virtual */ WAbstractThreadReply * createReply() const;

protected: // WAbstractThreadAction implementation
    /* virtual */ bool run();

public: // Variables
    QImage image;

    int x;
    int y;

    int size;

    int precision;

    WBarcodeReader::Formats formats;
};

class WBarcodeReadReply : public WAbstractThreadReply
{
    Q_OBJECT

protected: // WAbstractThreadReply reimplementation
    /* virtual */ void onCompleted(bool ok);

signals:
    void loaded(const QString & text);

public: // Variables
    QString text;
};

class WBarcodeScanReply : public WAbstractThreadReply
{
    Q_OBJECT

protected: // WAbstractThreadReply reimplementation
    /* virtual */ void onCompleted(bool ok);

signals:
    void loaded(const WBarcodeResult & result);

public: // Variables
    WBarcodeResult result;
};

//-------------------------------------------------------------------------------------------------

/* virtual */ WAbstractThreadReply * WBarcodeRead::createReply() const
{
    return new WBarcodeReadReply;
}

/* virtual */ WAbstractThreadReply * WBarcodeFile::createReply() const
{
    return new WBarcodeReadReply;
}

/* virtual */ WAbstractThreadReply * WBarcodeScan::createReply() const
{
    return new WBarcodeScanReply;
}

/* virtual */ bool WBarcodeRead::run()
{
    WBarcodeReadReply * reply = qobject_cast<WBarcodeReadReply *> (this->reply());

    if (target.isValid())
    {
        image = image.copy(target);
    }

    if (mirror) image = image.mirrored();

    reply->text = WBarcodeReader::read(image, formats);

    return true;
}

/* virtual */ bool WBarcodeFile::run()
{
    WBarcodeReadReply * reply = qobject_cast<WBarcodeReadReply *> (this->reply());

    reply->text = WBarcodeReader::read(QImage(fileName), formats);

    return true;
}


/* virtual */ bool WBarcodeScan::run()
{
    WBarcodeScanReply * reply = qobject_cast<WBarcodeScanReply *> (this->reply());

    reply->result = WBarcodeReader::scan(image, x, y, size, formats, precision);

    return true;
}

/* virtual */ void WBarcodeReadReply::onCompleted(bool)
{
    emit loaded(text);
}


/* virtual */ void WBarcodeScanReply::onCompleted(bool)
{
    emit loaded(result);
}

//=================================================================================================
// WBarcodeReaderPrivate
//=================================================================================================

#include <private/Sk_p>

class SK_BARCODE_EXPORT WBarcodeReaderPrivate : public WPrivate
{
public:
    WBarcodeReaderPrivate(WBarcodeReader * p);

    void init();

public: // Static functions
    static ImageFormat getFormat(QImage::Format format);

    static QRect getSlice(int width, int height, int x, int y, int size);

    static bool getMatch(WBarcodeResult * result,
                         const Position & position, int x, int y, int size);

protected:
    W_DECLARE_PUBLIC(WBarcodeReader)
};

//-------------------------------------------------------------------------------------------------

WBarcodeReaderPrivate::WBarcodeReaderPrivate(WBarcodeReader * p) : WPrivate(p) {}

void WBarcodeReaderPrivate::init() {}

//-------------------------------------------------------------------------------------------------
// Private static functions
//-------------------------------------------------------------------------------------------------

/* static */ ImageFormat WBarcodeReaderPrivate::getFormat(QImage::Format format)
{
    if (format == QImage::Format_RGB32 || format == QImage::Format_ARGB32)
    {
#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
        return ImageFormat::BGRX;
#else
        return ImageFormat::XRGB;
#endif
    }
    else if (format == QImage::Format_RGB888)
    {
        return ImageFormat::RGB;
    }
#ifdef QT_NEW
    else if (format == QImage::Format_RGBX8888 || format == QImage::Format_RGBA8888)
    {
        return ImageFormat::RGBX;
    }
    else if (format == QImage::Format_Grayscale8)
    {
        return ImageFormat::Lum;
    }
#endif
    else return ImageFormat::None;
}

/* static */ QRect WBarcodeReaderPrivate::getSlice(int width, int height, int x, int y, int size)
{
    int sizeHalf = size / 2;

    int rectX = x - sizeHalf;
    int rectY = y - sizeHalf;

    if (rectX > 0)
    {
        int position = x + sizeHalf;

        if (position > width)
        {
            position -= width;

            rectX -= position;
        }
    }
    else rectX = 0;

    if (rectY > 0)
    {
        int position = y + sizeHalf;

        if (position > height)
        {
            position -= height;

            rectY -= position;
        }
    }
    else rectY = 0;

    return QRect(rectX, rectY, size, size);
}

/* static */ bool WBarcodeReaderPrivate::getMatch(WBarcodeResult * result,
                                                  const Position & position,
                                                  int x, int y, int size)
{
    const PointI & topLeft     = position.topLeft    ();
    const PointI & topRight    = position.topRight   ();
    const PointI & bottomLeft  = position.bottomLeft ();
    const PointI & bottomRight = position.bottomRight();

    //---------------------------------------------------------------------------------------------
    // NOTE: We map a square rectangle based on the result position.

    int rectX = qMin(topLeft.x, bottomLeft.x);
    int rectY = qMin(topLeft.y, topRight  .y);

    int width  = qMax(topRight  .x, bottomRight.x) - rectX;
    int height = qMax(bottomLeft.y, bottomRight.y) - rectY;

    //---------------------------------------------------------------------------------------------

    QRect rect = QRect(rectX, rectY, width, height);

    if (size > 1)
    {
        int sizeHalf = size / 2;

        if (rect.intersects(QRect(x - sizeHalf,
                                  y - sizeHalf, size, size)) == false) return false;
    }
    else if (rect.contains(x, y) == false) return false;

    result->rect = rect;

    result->topLeft     = QPoint(topLeft.x,     topLeft.y);
    result->topRight    = QPoint(topRight.x,    topRight.y);
    result->bottomLeft  = QPoint(bottomLeft.x,  bottomLeft.y);
    result->bottomRight = QPoint(bottomRight.x, bottomRight.y);

    return true;
}

//=================================================================================================
// WBarcodeReader
//=================================================================================================

/* explicit */ WBarcodeReader::WBarcodeReader(QObject * parent)
    : QObject(parent), WPrivatable(new WBarcodeReaderPrivate(this))
{
    Q_D(WBarcodeReader); d->init();
}

//-------------------------------------------------------------------------------------------------
// Static functions
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WBarcodeReader::read(const QImage & image, Formats formats)
{
    if (image.isNull()) return QString();

    ImageFormat format = WBarcodeReaderPrivate::getFormat(image.format());

    if (format == ImageFormat::None)
    {
        // NOTE: We try image conversion to deal with 'exotic' formats.
        return read(image.convertToFormat(QImage::Format_RGB32), formats);
    }

    ImageView imageView(image.bits(), image.width(), image.height(), format);

    DecodeHints hints;

    hints.setFormats(static_cast<BarcodeFormats> (formats));

    Result output = ReadBarcode(imageView, hints);

    if (output.isValid())
    {
        return QString::fromWCharArray(output.text().c_str());
    }
    else return QString();
}

/* Q_INVOKABLE static */ QStringList WBarcodeReader::reads(const QImage & image, Formats formats)
{
    if (image.isNull()) return QStringList();

    ImageFormat format = WBarcodeReaderPrivate::getFormat(image.format());

    if (format == ImageFormat::None)
    {
        // NOTE: We try image conversion to deal with 'exotic' formats.
        return reads(image.convertToFormat(QImage::Format_RGB32), formats);
    }

    ImageView imageView(image.bits(), image.width(), image.height(), format);

    DecodeHints hints;

    hints.setFormats(static_cast<BarcodeFormats> (formats));

    Results outputs = ReadBarcodes(imageView, hints);

    QStringList list;

    foreach (const Result & output, outputs)
    {
        if (output.isValid() == false) continue;

        list.append(QString::fromWCharArray(output.text().c_str()));
    }

    return list;
}

/* Q_INVOKABLE static */ QString WBarcodeReader::readFile(const QString & fileName,
                                                          Formats         formats)
{
    return read(QImage(fileName), formats);
}

/* Q_INVOKABLE static */ WBarcodeResult WBarcodeReader::scan(const QImage & image, int x, int y,
                                                             int size, Formats formats,
                                                             int precision)
{
    if (image.isNull() || precision == 0) return WBarcodeResult();

    ImageFormat format = WBarcodeReaderPrivate::getFormat(image.format());

    if (format == ImageFormat::None)
    {
        // NOTE: We try image conversion to deal with 'exotic' formats.
        return scan(image.convertToFormat(QImage::Format_RGB32), x, y, size, formats, precision);
    }

    DecodeHints hints;

    hints.setFormats(static_cast<BarcodeFormats> (formats));

    int width  = image.width ();
    int height = image.height();

    int ratio = height / precision;

    int currentSize = ratio;

    precision--;

    WBarcodeResult result;

    // NOTE: We call ReadBarcode multiple times because it's faster than ReadBarcodes.
    for (int i = 0; i < precision; i++)
    {
        QRect rect = WBarcodeReaderPrivate::getSlice(width, height, x, y, currentSize);

        QImage view = image.copy(rect);

        ImageView imageView(view.bits(), view.width(), view.height(), format);

        Result output = ReadBarcode(imageView, hints);

        if (output.isValid())
        {
            int rectX = rect.x();
            int rectY = rect.y();

            if (WBarcodeReaderPrivate::getMatch(&result,
                                                output.position(), x - rectX, y - rectY, size))
            {
                result.text = QString::fromWCharArray(output.text().c_str());

                rect = result.rect;

                result.rect = QRect(rect.x() + rectX,
                                    rect.y() + rectY, rect.width(), rect.height());

                QPoint topLeft     = result.topLeft;
                QPoint topRight    = result.topRight;
                QPoint bottomLeft  = result.bottomLeft;
                QPoint bottomRight = result.bottomRight;

                result.topLeft     = QPoint(topLeft    .x() + rectX, topLeft    .y() + rectY);
                result.topRight    = QPoint(topRight   .x() + rectX, topRight   .y() + rectY);
                result.bottomLeft  = QPoint(bottomLeft .x() + rectX, bottomLeft .y() + rectY);
                result.bottomRight = QPoint(bottomRight.x() + rectX, bottomRight.y() + rectY);

                return result;
            }
        }

        currentSize += ratio;
    }

    ImageView imageView(image.bits(), image.width(), image.height(), format);

    Result output = ReadBarcode(imageView, hints);

    if (output.isValid())
    {
        if (WBarcodeReaderPrivate::getMatch(&result, output.position(), x, y, size))
        {
            result.text = QString::fromWCharArray(output.text().c_str());

            return result;
        }
    }

    return WBarcodeResult();
}

/* Q_INVOKABLE static */
WAbstractThreadAction * WBarcodeReader::startRead(const QImage & image,
                                                  Formats        formats,
                                                  QObject      * receiver,
                                                  const char   * method,
                                                  const QRect  & target,
                                                  bool           mirror)
{
    WBarcodeRead * action = new WBarcodeRead(image, target, mirror, formats);

    WBarcodeReadReply * reply = qobject_cast<WBarcodeReadReply *>
                                (wControllerFile->startReadAction(action));

    if (receiver) connect(reply, SIGNAL(loaded(const QString &)), receiver, method);

    return action;
}

/* Q_INVOKABLE static */
WAbstractThreadAction * WBarcodeReader::startReadFile(const QString & fileName,
                                                      Formats         formats,
                                                      QObject       * receiver,
                                                      const char    * method)
{
    WBarcodeFile * action = new WBarcodeFile(fileName, formats);

    WBarcodeReadReply * reply = qobject_cast<WBarcodeReadReply *>
                                (wControllerFile->startReadAction(action));

    if (receiver) connect(reply, SIGNAL(loaded(const QString &)), receiver, method);

    return action;
}

/* Q_INVOKABLE static */
WAbstractThreadAction * WBarcodeReader::startScan(const QImage & image,
                                                  int            x,
                                                  int            y,
                                                  int            size,
                                                  Formats        formats,
                                                  QObject      * receiver,
                                                  const char   * method,
                                                  int            precision)
{
    WBarcodeScan * action = new WBarcodeScan(image, x, y, size, precision, formats);

    WBarcodeScanReply * reply = qobject_cast<WBarcodeScanReply *>
                                (wControllerFile->startReadAction(action));

    if (receiver) connect(reply, SIGNAL(loaded(const WBarcodeResult &)), receiver, method);

    return action;
}

#endif // SK_NO_BARCODEREADER

#include "WBarcodeReader.moc"
