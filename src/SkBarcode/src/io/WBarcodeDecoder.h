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

#ifndef WBARCODEDECODER_H
#define WBARCODEDECODER_H

// Qt includes
#include <QObject>
#ifdef QT_4
#include <QImage>
#endif

// ZXing includes
#include <BarcodeFormat.h>

// Sk includes
#include <Sk>

// Forward declarations
class WBarcodeDecoderPrivate;

class SK_BARCODE_EXPORT WBarcodeDecoder : public QObject, public WPrivatable
{
    Q_OBJECT

public:
    explicit WBarcodeDecoder(QObject * parent = NULL);

public: // Static functions
    Q_INVOKABLE static QString decode(const QImage & image, ZXing::BarcodeFormats formats);

private:
    W_DECLARE_PRIVATE(WBarcodeDecoder)
};

#endif // WBARCODEDECODER_H
