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

#ifndef WHOOKOUTPUTBARCODE_P_H
#define WHOOKOUTPUTBARCODE_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

// Private includes
#include <private/WHookOutput_p>

#ifndef SK_NO_HOOKOUTPUTBARCODE

class SK_BARCODE_EXPORT WHookOutputBarcodePrivate : public WHookOutputPrivate
{
public:
    WHookOutputBarcodePrivate(WHookOutputBarcode * p);

    void init();

protected:
    W_DECLARE_PUBLIC(WHookOutputBarcode)
};

#endif // SK_NO_HOOKOUTPUTBARCODE
#endif // WHOOKOUTPUTBARCODE_P_H
