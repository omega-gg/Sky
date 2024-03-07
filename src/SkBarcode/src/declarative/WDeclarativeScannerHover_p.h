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

#ifndef WDECLARATIVESCANNERHOVER_P_H
#define WDECLARATIVESCANNERHOVER_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

// Private includes
#ifdef QT_4
#include <private/WDeclarativeItem_p>
#else
#include <private/WDeclarativeItemPaint_p>
#endif

#ifndef SK_NO_DECLARATIVESCANNERHOVER

#ifdef QT_4
class SK_BARCODE_EXPORT WDeclarativeScannerHoverPrivate : public WDeclarativeItemPrivate
#else
class SK_BARCODE_EXPORT WDeclarativeScannerHoverPrivate : public WDeclarativeItemPaintPrivate
#endif
{
public:
    WDeclarativeScannerHoverPrivate(WDeclarativeScannerHover * p);

    void init();

public: // Variables
    bool active;

    QPointF topLeft;
    QPointF topRight;
    QPointF bottomLeft;
    QPointF bottomRight;

    QColor color;

protected:
    W_DECLARE_PUBLIC(WDeclarativeScannerHover)
};

#endif // SK_NO_DECLARATIVESCANNERHOVER
#endif // WDECLARATIVESCANNERHOVER_P_H
