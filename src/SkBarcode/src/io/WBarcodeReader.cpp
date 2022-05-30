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
#include <WAbstractThreadAction>
#include <WAbstractThreadReply>

// Namespaces
using namespace ZXing;

//=================================================================================================
// WBarcodeRead, WBarcodeFile and WBarcodeReply
//=================================================================================================

class WBarcodeRead : public WAbstractThreadAction
{
    Q_OBJECT

public:
    WBarcodeRead(const QImage & image, BarcodeFormats formats)
    {
        this->image   = image;
        this->formats = formats;
    }

protected: // WAbstractThreadAction reimplementation
    /* virtual */ WAbstractThreadReply * createReply() const;

protected: // WAbstractThreadAction implementation
    /* virtual */ bool run();

public: // Variables
    QImage         image;
    BarcodeFormats formats;
};

class WBarcodeFile : public WAbstractThreadAction
{
    Q_OBJECT

public:
    WBarcodeFile(const QString & fileName, BarcodeFormats formats)
    {
        this->fileName = fileName;
        this->formats  = formats;
    }

protected: // WAbstractThreadAction reimplementation
    /* virtual */ WAbstractThreadReply * createReply() const;

protected: // WAbstractThreadAction implementation
    /* virtual */ bool run();

public: // Variables
    QString        fileName;
    BarcodeFormats formats;
};

class WBarcodeReply : public WAbstractThreadReply
{
    Q_OBJECT

protected: // WAbstractThreadReply reimplementation
    /* virtual */ void onCompleted(bool ok);

signals:
    void loaded(const QString & text);

public: // Variables
    QString text;
};

//-------------------------------------------------------------------------------------------------

/* virtual */ WAbstractThreadReply * WBarcodeRead::createReply() const
{
    return new WBarcodeReply;
}

/* virtual */ WAbstractThreadReply * WBarcodeFile::createReply() const
{
    return new WBarcodeReply;
}

/* virtual */ bool WBarcodeRead::run()
{
    WBarcodeReply * reply = qobject_cast<WBarcodeReply *> (this->reply());

    reply->text = WBarcodeReader::read(image, formats);

    return true;
}

/* virtual */ bool WBarcodeFile::run()
{
    WBarcodeReply * reply = qobject_cast<WBarcodeReply *> (this->reply());

    reply->text = WBarcodeReader::read(QImage(fileName), formats);

    return true;
}

/* virtual */ void WBarcodeReply::onCompleted(bool)
{
    emit loaded(text);
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

/* Q_INVOKABLE static */ QString WBarcodeReader::read(const QImage & image, BarcodeFormats formats)
{
    if (image.isNull()) return QString();

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
    else imageFormat = ImageFormat::None;

    ImageView imageView(image.bits(), image.width(), image.height(), imageFormat);

    DecodeHints hints;

    hints.setFormats(formats);

    Result result = ReadBarcode(imageView, hints);

    if (result.isValid() == false) return QString();

    return QString::fromWCharArray(result.text().c_str());
}

/* Q_INVOKABLE static */ QString WBarcodeReader::readFile(const QString & fileName,
                                                          BarcodeFormats  formats)
{
    return read(QImage(fileName), formats);
}

/* Q_INVOKABLE static */
WAbstractThreadAction * WBarcodeReader::startRead(const QImage   & image,
                                                  BarcodeFormats   formats,
                                                  QObject        * receiver,
                                                  const char     * method)
{
    WBarcodeRead * action = new WBarcodeRead(image, formats);

    WBarcodeReply * reply = qobject_cast<WBarcodeReply *>
                            (wControllerFile->startReadAction(action));

    QObject::connect(reply, SIGNAL(loaded(const QString &)), receiver, method);

    return action;
}

/* Q_INVOKABLE static */
WAbstractThreadAction * WBarcodeReader::startReadFile(const QString  & fileName,
                                                      BarcodeFormats   formats,
                                                      QObject        * receiver,
                                                      const char     * method)
{
    WBarcodeFile * action = new WBarcodeFile(fileName, formats);

    WBarcodeReply * reply = qobject_cast<WBarcodeReply *>
                            (wControllerFile->startReadAction(action));

    QObject::connect(reply, SIGNAL(loaded(const QString &)), receiver, method);

    return action;
}

#endif // SK_NO_BARCODEREADER

#include "WBarcodeReader.moc"
