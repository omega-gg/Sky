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

#ifndef WDECLARATIVESCANNER_P_H
#define WDECLARATIVESCANNER_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

#include <private/WDeclarativeItem_p>

#ifndef SK_NO_DECLARATIVESCANNER

// Forward declarations
class WAbstractThreadAction;

//-------------------------------------------------------------------------------------------------
// WDeclarativeScannerData
//-------------------------------------------------------------------------------------------------

struct WDeclarativeScannerData
{
    WAbstractThreadAction * action;

    qreal ratioX;
    qreal ratioY;

    qreal rectX;
    qreal rectY;
};

//-------------------------------------------------------------------------------------------------
// WDeclarativeScannerPrivate
//-------------------------------------------------------------------------------------------------

class SK_BARCODE_EXPORT WDeclarativeScannerPrivate : public WDeclarativeItemPrivate
{
public:
    WDeclarativeScannerPrivate(WDeclarativeScanner * p);

    void init();

public: // Fuctions
    bool scan();

    void stopTimer();

    void clearCount();
    void clearData ();
    void clearItem ();

public: // Slots
    void onLoaded(const WBarcodeResult & result);

    void onClearPlayer();
    void onClearCover ();

public: // Variables
    WDeclarativePlayer * player;
    WDeclarativeImage  * cover;

    int x;
    int y;

    int size;

    int timerId;

    int count;
    int currentCount;

    int interval;

    QList<WDeclarativeScannerData> datas;

protected:
    W_DECLARE_PUBLIC(WDeclarativeScanner)
};

#endif // SK_NO_DECLARATIVESCANNER
#endif // WDECLARATIVESCANNER_P_H
