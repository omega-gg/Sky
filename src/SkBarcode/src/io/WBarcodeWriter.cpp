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

#include "WBarcodeWriter.h"

#ifndef SK_NO_BARCODEWRITER

// ZXing includes
#include <TextUtfEncoding.h>
#include <CharacterSet.h>
#include <qrcode/QREncoder.h>
#include <qrcode/QREncodeResult.h>

// Sk includes
#include <WControllerFile>
#include <WZipper>

// Namespaces
using namespace ZXing;

//-------------------------------------------------------------------------------------------------
// Static variables

static const int BARCODEWRITER_MARGINS = 1;

//=================================================================================================
// WBarcodeWrite and WBarcodeWriteReply
//=================================================================================================

class WBarcodeWrite : public WAbstractThreadAction
{
    Q_OBJECT

public:
    WBarcodeWrite(const QString & text, WBarcodeWriter::Type type)
    {
        this->text = text;
        this->type = type;
    }

protected: // WAbstractThreadAction reimplementation
    /* virtual */ WAbstractThreadReply * createReply() const;

protected: // WAbstractThreadAction implementation
    /* virtual */ bool run();

public: // Variables
    QString              text;
    WBarcodeWriter::Type type;
};

class WBarcodeWriteReply : public WAbstractThreadReply
{
    Q_OBJECT

protected: // WAbstractThreadReply reimplementation
    /* virtual */ void onCompleted(bool ok);

signals:
    void complete(const QImage & image);

public: // Variables
    QImage image;
};

//-------------------------------------------------------------------------------------------------

/* virtual */ WAbstractThreadReply * WBarcodeWrite::createReply() const
{
    return new WBarcodeWriteReply;
}

/* virtual */ bool WBarcodeWrite::run()
{
    WBarcodeWriteReply * reply = qobject_cast<WBarcodeWriteReply *> (this->reply());

    reply->image = WBarcodeWriter::write(text, type);

    return true;
}

/* virtual */ void WBarcodeWriteReply::onCompleted(bool)
{
    emit complete(image);
}

//=================================================================================================
// WBarcodeWriterPrivate
//=================================================================================================

#include <private/Sk_p>

class SK_BARCODE_EXPORT WBarcodeWriterPrivate : public WPrivate
{
public:
    WBarcodeWriterPrivate(WBarcodeWriter * p);

    void init();

protected:
    W_DECLARE_PUBLIC(WBarcodeWriter)
};

//-------------------------------------------------------------------------------------------------

WBarcodeWriterPrivate::WBarcodeWriterPrivate(WBarcodeWriter * p) : WPrivate(p) {}

void WBarcodeWriterPrivate::init() {}

//=================================================================================================
// WBarcodeWriter
//=================================================================================================

/* explicit */ WBarcodeWriter::WBarcodeWriter(QObject * parent)
    : QObject(parent), WPrivatable(new WBarcodeWriterPrivate(this))
{
    Q_D(WBarcodeWriter); d->init();
}

//-------------------------------------------------------------------------------------------------
// Static functions
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QImage WBarcodeWriter::write(const QString & text, Type type)
{
    if (text.isEmpty()) return QImage();

    std::wstring string;

    if (type == Vbml)
    {
        QByteArray data = WZipper::compress(text.toUtf8());

#ifdef QT_4
        // FIXME Qt4: This version does not support encoding flags.
        data = data.toBase64();
#else
        data = data.toBase64(QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals);
#endif

        string = TextUtfEncoding::FromUtf8("vbml:" + data.toStdString());
    }
    else string = TextUtfEncoding::FromUtf8(text.toStdString());

    QRCode::EncodeResult code = QRCode::Encode(string, QRCode::ErrorCorrectionLevel::Low,
                                               CharacterSet::Unknown, 0, false, -1);

    const BitMatrix & matrix = code.matrix;

    int width  = matrix.width ();
    int height = matrix.height();

    int margins = BARCODEWRITER_MARGINS * 2;

    QImage image(width + margins, height + margins, QImage::Format_Grayscale8);

    image.fill(255);

    for (int y = 0; y < height; y++)
    {
        uchar * line = image.scanLine(y + BARCODEWRITER_MARGINS);

        line += BARCODEWRITER_MARGINS;

        for (int x = 0; x < width; x++)
        {
            if (matrix.get(x, y)) *line = 0;

            line++;
        }
    }

    return image;
}

/* Q_INVOKABLE static */
WAbstractThreadAction * WBarcodeWriter::startWrite(const QString & text,
                                                   QObject       * receiver,
                                                   const char    * method, Type type)
{
    WBarcodeWrite * action = new WBarcodeWrite(text, type);

    WBarcodeWriteReply * reply = qobject_cast<WBarcodeWriteReply *>
                                 (wControllerFile->startWriteAction(action));

    QObject::connect(reply, SIGNAL(complete(const QImage &)), receiver, method);

    return action;
}

#endif // SK_NO_BARCODEWRITER

#include "WBarcodeWriter.moc"
