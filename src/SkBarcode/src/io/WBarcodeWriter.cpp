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

// Qt includes
#include <QPainter>

// ZXing includes
#include <TextUtfEncoding.h>
#include <CharacterSet.h>
#include <qrcode/QREncoder.h>
#include <qrcode/QREncodeResult.h>

// Sk includes
#include <WControllerApplication>
#include <WControllerFile>
#include <WZipper>
#include <WRegExp>

// Namespaces
using namespace ZXing;

//-------------------------------------------------------------------------------------------------
// Static variables

static const int BARCODEWRITER_MARGINS = 1;

static const int BARCODEWRITER_MAX = 4296;

static const int BARCODEWRITER_MAX_LENGTH = 50;

static const int BARCODEWRITER_COLOR_THRESHOLD = 30;

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

class WBarcodeWriteTag : public WAbstractThreadAction
{
    Q_OBJECT

public:
    WBarcodeWriteTag(const QString & text,
                     const QString & background, const QString & cover, int size, int margin)
    {
        this->text = text;

        this->background = background;
        this->cover      = cover;

        this->size   = size;
        this->margin = margin;
    }

protected: // WAbstractThreadAction reimplementation
    /* virtual */ WAbstractThreadReply * createReply() const;

protected: // WAbstractThreadAction implementation
    /* virtual */ bool run();

public: // Variables
    QString text;

    QString background;
    QString cover;

    int size;
    int margin;
};

class WBarcodeWriteTagFile : public WAbstractThreadAction
{
    Q_OBJECT

public:
    WBarcodeWriteTagFile(const QString & fileName, const QString & text,
                         const QString & background, const QString & cover,
                         int size, int margin, const QString & format)
    {
        this->fileName = fileName;

        this->text = text;

        this->background = background;
        this->cover      = cover;

        this->size   = size;
        this->margin = margin;

        this->format = format;
    }

protected: // WAbstractThreadAction reimplementation
    /* virtual */ WAbstractThreadReply * createReply() const;

protected: // WAbstractThreadAction implementation
    /* virtual */ bool run();

public: // Variables
    QString fileName;

    QString text;

    QString background;
    QString cover;

    int size;
    int margin;

    QString format;
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

/* virtual */ WAbstractThreadReply * WBarcodeWriteTag::createReply() const
{
    return new WBarcodeWriteReply;
}

/* virtual */ WAbstractThreadReply * WBarcodeWriteTagFile::createReply() const
{
    return new WControllerFileReply;
}

/* virtual */ bool WBarcodeWrite::run()
{
    WBarcodeWriteReply * reply = qobject_cast<WBarcodeWriteReply *> (this->reply());

    reply->image = WBarcodeWriter::write(text, type);

    return true;
}

/* virtual */ bool WBarcodeWriteTag::run()
{
    WBarcodeWriteReply * reply = qobject_cast<WBarcodeWriteReply *> (this->reply());

    reply->image = WBarcodeWriter::writeTag(text, background, cover, size, margin);

    return true;
}

/* virtual */ bool WBarcodeWriteTagFile::run()
{
    return WBarcodeWriter::writeTagFile(fileName, text, background, cover, size, margin, format);
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

        string = TextUtfEncoding::FromUtf8("vbml:" + QString(data).toStdString());
#else
        data = data.toBase64(QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals);

        string = TextUtfEncoding::FromUtf8("vbml:" + data.toStdString());
#endif
    }
    else string = TextUtfEncoding::FromUtf8(text.toStdString());

    if (string.size() > BARCODEWRITER_MAX)
    {
        qWarning("WBarcodeWriter::write: The text is too long (%d).", (int) string.size());

        return QImage();
    }

    QRCode::EncodeResult code = QRCode::Encode(string, QRCode::ErrorCorrectionLevel::Low,
                                               CharacterSet::Unknown, 0, false, -1);

    const BitMatrix & matrix = code.matrix;

    int width  = matrix.width ();
    int height = matrix.height();

    int margins = BARCODEWRITER_MARGINS * 2;

#ifdef QT_4
    // NOTE Qt4: This version does not support Format_Grayscale8 and Format_Indexed8 cannot be
    //           drawn into a QImage.
    QImage image(width + margins, height + margins, QImage::Format_RGB32);

    image.fill(Qt::white);

    for (int y = 0; y < height; y++)
    {
        QRgb * line = (QRgb *) image.scanLine(y + BARCODEWRITER_MARGINS);

        line += BARCODEWRITER_MARGINS;

        for (int x = 0; x < width; x++)
        {
            if (matrix.get(x, y)) *line = Qt::black;

            line++;
        }
    }
#else
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
#endif

    return image;
}

/* Q_INVOKABLE static */ QImage WBarcodeWriter::writeTag(const QString & text,
                                                         const QString & background,
                                                         const QString & cover,
                                                         int size, int margin)
{
    QImage front = write(text, Vbml);

    if (front.isNull()) return QImage();

    QImage back(WControllerFile::toLocalFile(background));

    if (cover.isEmpty() == false)
    {
        QImage image(WControllerFile::toLocalFile(cover));

        if (image.isNull() == false)
        {
            //-------------------------------------------------------------------------------------
            // NOTE: We want to get rid of the black bars.
            image = getBackground(image);

            int sizeDouble = size * 2;

            image = image.scaled(sizeDouble, sizeDouble,
                                 Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

            QImage content(sizeDouble, sizeDouble, QImage::Format_ARGB32_Premultiplied);

            content.fill(Qt::black);

            QPainter painter(&content);

            painter.drawImage(QRect(0, 0, sizeDouble, sizeDouble), image,
                              QRect((image.width () - sizeDouble) / 2,
                                    (image.height() - sizeDouble) / 2, sizeDouble, sizeDouble));

            int sizeHalf = size / 2;

            painter.drawImage(QRect(sizeHalf, sizeHalf, size, size), back,
                              QRect(0, 0, back.width(), back.height()));

            sizeHalf += margin;

            size -= margin * 2;

            painter.drawImage(QRect(sizeHalf, sizeHalf, size, size), front,
                              QRect(0, 0, front.width(), front.height()));

            return content;
        }
    }

    QImage content(size, size, QImage::Format_ARGB32_Premultiplied);

    content.fill(Qt::transparent);

    QPainter painter(&content);

    painter.drawImage(QRect(0, 0, size, size), back,
                      QRect(0, 0, back.width(), back.height()));

    size -= margin * 2;

    painter.drawImage(QRect(margin, margin, size, size), front,
                      QRect(0, 0, front.width(), front.height()));

    return content;
}

/* Q_INVOKABLE static */ bool WBarcodeWriter::writeTagFile(const QString & fileName,
                                                           const QString & text,
                                                           const QString & background,
                                                           const QString & cover,
                                                           int             size,
                                                           int             margin,
                                                           const QString & format)
{
    QImage image = writeTag(text, background, cover, size, margin);

    return image.save(fileName, format.C_STR);
}

/* Q_INVOKABLE static */
WAbstractThreadAction * WBarcodeWriter::startWrite(const QString & text,
                                                   QObject       * receiver,
                                                   const char    * method, Type type)
{
    WBarcodeWrite * action = new WBarcodeWrite(text, type);

    WBarcodeWriteReply * reply = qobject_cast<WBarcodeWriteReply *>
                                 (wControllerFile->startWriteAction(action));

    if (method) connect(reply, SIGNAL(complete(const QImage &)), receiver, method);

    return action;
}

/* Q_INVOKABLE static */
WAbstractThreadAction * WBarcodeWriter::startWriteTag(const QString & text,
                                                      const QString & background,
                                                      QObject       * receiver,
                                                      const char    * method,
                                                      const QString & cover,
                                                      int             size,
                                                      int             margin)
{
    WBarcodeWriteTag * action = new WBarcodeWriteTag(text, background, cover, size, margin);

    WBarcodeWriteReply * reply = qobject_cast<WBarcodeWriteReply *>
                                 (wControllerFile->startWriteAction(action));

    if (method) connect(reply, SIGNAL(complete(const QImage &)), receiver, method);

    return action;
}

/* Q_INVOKABLE static */
WAbstractThreadAction * WBarcodeWriter::startWriteTagFile(const QString & fileName,
                                                          const QString & text,
                                                          const QString & background,
                                                          QObject       * receiver,
                                                          const char    * method,
                                                          const QString & cover,
                                                          int             size,
                                                          int             margin,
                                                          const QString & format)
{
    WBarcodeWriteTagFile * action = new WBarcodeWriteTagFile(fileName, text, background, cover,
                                                             size, margin, format);

    WControllerFileReply * reply = qobject_cast<WControllerFileReply *>
                                   (wControllerFile->startWriteAction(action));

    if (method) connect(reply, SIGNAL(complete(bool)), receiver, method);

    return action;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WBarcodeWriter::getTagName(const QString & title)
{
    QString name = title;

    name.remove(WRegExp("[^a-zA-Z0-9. ]"));

    name = name.simplified();

    int length = name.length();

    if (length > BARCODEWRITER_MAX_LENGTH)
    {
        name.chop(length - BARCODEWRITER_MAX_LENGTH);
    }

    name.replace(' ', '_');

    return "VideoTag-" + name + '-' + Sk::currentDateString();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QImage WBarcodeWriter::getBackground(const QImage & image)
{
    int top    = getBarTop   (image);
    int bottom = getBarBottom(image);

    top = qMin(top, bottom);

    if (top == 0) return image;

    int height = image.height();

    // NOTE: When the margin is too large we keep the bars.
    if (top < height / 2)
    {
         return image.copy(QRect(0, top, image.width(), height - top * 2));
    }
    else return image;
}

/* Q_INVOKABLE static */ int WBarcodeWriter::getBarTop(const QImage & image)
{
    int margin = 0;

    int width = image.width();

    for (int y = 0; y < image.height(); y++)
    {
        const QRgb * line = (QRgb *) image.scanLine(y);

        qint64 value = 0;

        for (int x = 0; x < width; x++)
        {
            value += qRed(*line) + qGreen(*line) + qBlue(*line);

            line++;
        }

        // NOTE: We use a threshold to compensate the pixel compression.
        if (value / width > BARCODEWRITER_COLOR_THRESHOLD) return margin;

        margin++;
    }

    return margin;
}

/* Q_INVOKABLE static */ int WBarcodeWriter::getBarBottom(const QImage & image)
{
    int margin = 0;

    int width = image.width();

    for (int y = image.height() - 1; y > -1; y--)
    {
        const QRgb * line = (QRgb *) image.scanLine(y);

        qint64 value = 0;

        for (int x = 0; x < width; x++)
        {
            value += qRed(*line) + qGreen(*line) + qBlue(*line);

            line++;
        }

        // NOTE: We use a threshold to compensate the pixel compression.
        if (value / width > BARCODEWRITER_COLOR_THRESHOLD) return margin;

        margin++;
    }

    return margin;
}

#endif // SK_NO_BARCODEWRITER

#include "WBarcodeWriter.moc"
