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
// WBarcodeRead, WBarcodeFile and WBarcodeReadReply
//=================================================================================================

class WBarcodeRead : public WAbstractThreadAction
{
    Q_OBJECT

public:
    WBarcodeRead(const QImage & image, const QRect & target, WBarcodeReader::Formats formats)
    {
        this->image  = image;
        this->target = target;

        this->formats = formats;
    }

protected: // WAbstractThreadAction reimplementation
    /* virtual */ WAbstractThreadReply * createReply() const;

protected: // WAbstractThreadAction implementation
    /* virtual */ bool run();

public: // Variables
    QImage image;
    QRect  target;

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

class WBarcodeReadReply : public WAbstractThreadReply
{
    Q_OBJECT

protected: // WAbstractThreadReply reimplementation
    /* virtual */ void onCompleted(bool ok);

signals:
    void loaded(const QByteArray & data);

public: // Variables
    QByteArray data;
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

/* virtual */ bool WBarcodeRead::run()
{
    WBarcodeReadReply * reply = qobject_cast<WBarcodeReadReply *> (this->reply());

    if (target.isValid())
    {
         reply->data = WBarcodeReader::read(image.copy(target), formats);
    }
    else reply->data = WBarcodeReader::read(image, formats);

    return true;
}

/* virtual */ bool WBarcodeFile::run()
{
    WBarcodeReadReply * reply = qobject_cast<WBarcodeReadReply *> (this->reply());

    reply->data = WBarcodeReader::read(QImage(fileName), formats);

    return true;
}

/* virtual */ void WBarcodeReadReply::onCompleted(bool)
{
    emit loaded(data);
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

protected:
    W_DECLARE_PUBLIC(WBarcodeReader)
};

//-------------------------------------------------------------------------------------------------

WBarcodeReaderPrivate::WBarcodeReaderPrivate(WBarcodeReader * p) : WPrivate(p) {}

void WBarcodeReaderPrivate::init() {}

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

/* Q_INVOKABLE static */ QByteArray WBarcodeReader::read(const QImage & image, Formats formats)
{
    if (image.isNull()) return QByteArray();

    QImage::Format format = image.format();

    ImageFormat imageFormat;

    if (format == QImage::Format_RGB32 || format == QImage::Format_ARGB32)
    {
#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
        imageFormat = ImageFormat::BGRX;
#else
        imageFormat = ImageFormat::XRGB;
#endif
    }
    else if (format == QImage::Format_RGB888)
    {
        imageFormat = ImageFormat::RGB;
    }
#ifdef QT_NEW
    else if (format == QImage::Format_RGBX8888 || format == QImage::Format_RGBA8888)
    {
        imageFormat = ImageFormat::RGBX;
    }
    else if (format == QImage::Format_Grayscale8)
    {
        imageFormat = ImageFormat::Lum;
    }
#endif
    // NOTE: We try image conversion to deal with 'exotic' formats.
    else return read(image.convertToFormat(QImage::Format_RGB32), formats);

    ImageView imageView(image.bits(), image.width(), image.height(), imageFormat);

    DecodeHints hints;

    hints.setFormats(static_cast<BarcodeFormats> (formats));

    Result result = ReadBarcode(imageView, hints);

    if (result.isValid())
    {
        return QString::fromWCharArray(result.text().c_str()).toUtf8();
    }
    else return QByteArray();
}

/* Q_INVOKABLE static */ QByteArray WBarcodeReader::readFile(const QString & fileName,
                                                             Formats         formats)
{
    return read(QImage(fileName), formats);
}

/* Q_INVOKABLE static */
WAbstractThreadAction * WBarcodeReader::startRead(const QImage & image,
                                                  Formats        formats,
                                                  QObject      * receiver,
                                                  const char   * method,
                                                  const QRect  & target)
{
    WBarcodeRead * action = new WBarcodeRead(image, target, formats);

    WBarcodeReadReply * reply = qobject_cast<WBarcodeReadReply *>
                                (wControllerFile->startReadAction(action));

    if (receiver) connect(reply, SIGNAL(loaded(const QByteArray &)), receiver, method);

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

    if (receiver) connect(reply, SIGNAL(loaded(const QByteArray &)), receiver, method);

    return action;
}

#endif // SK_NO_BARCODEREADER

#include "WBarcodeReader.moc"
