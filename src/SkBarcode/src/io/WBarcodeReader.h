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

#ifndef WBARCODEREADER_H
#define WBARCODEREADER_H

// Qt includes
#include <QObject>
#ifdef QT_4
#include <QImage>
#endif
#include <QRect>

// Sk includes
#include <Sk>

// Forward declarations
class WBarcodeReaderPrivate;
class WAbstractThreadAction;

//-------------------------------------------------------------------------------------------------
// WBarcodeResult
//-------------------------------------------------------------------------------------------------

struct WBarcodeResult
{
    QString text;
    QRect   rect;
};

//-------------------------------------------------------------------------------------------------
// WBarcodeReader
//-------------------------------------------------------------------------------------------------

class SK_BARCODE_EXPORT WBarcodeReader : public QObject, public WPrivatable
{
    Q_OBJECT

    Q_ENUMS(Format)

public: // Enums
    // NOTE: These are copied from BarcodeFormat.h. We have to make a modification in
    //       zxing-cpp/Flags.h to make the conversion work.
    enum Format
    {
        None            = 0,         ///< Used as a return value
        Aztec           = (1 << 0),  ///< Aztec (2D)
        Codabar         = (1 << 1),  ///< Codabar (1D)
        Code39          = (1 << 2),  ///< Code39 (1D)
        Code93          = (1 << 3),  ///< Code93 (1D)
        Code128         = (1 << 4),  ///< Code128 (1D)
        DataBar         = (1 << 5),  ///< GS1 DataBar, formerly known as RSS 14
        DataBarExpanded = (1 << 6),  ///< GS1 DataBar Expanded, formerly known as RSS EXPANDED
        DataMatrix      = (1 << 7),  ///< DataMatrix (2D)
        EAN8            = (1 << 8),  ///< EAN-8 (1D)
        EAN13           = (1 << 9),  ///< EAN-13 (1D)
        ITF             = (1 << 10), ///< ITF (Interleaved Two of Five) (1D)
        MaxiCode        = (1 << 11), ///< MaxiCode (2D)
        PDF417          = (1 << 12), ///< PDF417 (1D) or (2D)
        QRCode          = (1 << 13), ///< QR Code (2D)
        UPCA            = (1 << 14), ///< UPC-A (1D)
        UPCE            = (1 << 15), ///< UPC-E (1D)

        OneDCodes = Codabar | Code39 | Code93 | Code128 | EAN8 | EAN13 | ITF | DataBar | \
                    DataBarExpanded | UPCA | UPCE,
        TwoDCodes = Aztec | DataMatrix | MaxiCode | PDF417 | QRCode,
        Any       = OneDCodes | TwoDCodes,
    };
    Q_DECLARE_FLAGS(Formats, Format)

public:
    explicit WBarcodeReader(QObject * parent = NULL);

public: // Static functions
    Q_INVOKABLE static QString     read (const QImage & image, Formats formats = Any);
    Q_INVOKABLE static QStringList reads(const QImage & image, Formats formats = Any);

    Q_INVOKABLE static QString readFile(const QString & fileName, Formats formats = Any);

    // NOTE: The 'precision' parameter is the number of scan iterations on the image. This function
    //       returns the matched barcode rectangle, if any.
    Q_INVOKABLE static WBarcodeResult scan(const QImage & image, int x, int y,
                                           int     size      = 1,
                                           Formats formats   = Any,
                                           int     precision = 5);

    // NOTE: The 'method' format is loaded(const QString &). A target can be specified for
    //       precise image scanning.
    Q_INVOKABLE static WAbstractThreadAction * startRead(const QImage & image,
                                                         Formats        formats  = Any,
                                                         QObject      * receiver = NULL,
                                                         const char   * method   = NULL,
                                                         const QRect  & target   = QRect(),
                                                         bool           mirror   = false);

    // NOTE: The 'method' format is loaded(const QString &).
    Q_INVOKABLE static WAbstractThreadAction * startReadFile(const QString & fileName,
                                                             Formats         formats  = Any,
                                                             QObject       * receiver = NULL,
                                                             const char    * method   = NULL);

    // NOTE: The 'method' format is loaded(const WBarcodeResult &).
    Q_INVOKABLE static WAbstractThreadAction * startScan(const QImage & image,
                                                         int            x,
                                                         int            y,
                                                         int            size      = 1,
                                                         Formats        formats   = Any,
                                                         QObject      * receiver  = NULL,
                                                         const char   * method    = NULL,
                                                         int            precision = 5);

private:
    W_DECLARE_PRIVATE(WBarcodeReader)
};

#endif // WBARCODEREADER_H
