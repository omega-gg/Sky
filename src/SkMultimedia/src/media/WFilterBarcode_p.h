//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkMultimedia.

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

#ifndef WFILTERBARCODE_P_H
#define WFILTERBARCODE_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

#ifdef QT_6
// Qt includes
#include <QTimer>
#endif

// Sk includes
#include <WBarcodeReader>

// Private includes
#include <private/Sk_p>

#ifndef SK_NO_FILTERBARCODE

class SK_MULTIMEDIA_EXPORT WFilterBarcodePrivate : public WPrivate
{
public:
    WFilterBarcodePrivate(WFilterBarcode * p);

    void init();

public: // Functions
#ifdef QT_6
    void applyRotation(const QVideoFrame & frame);
#endif

public: // Slots
#ifdef QT_6
    void onUpdated(const QVideoFrame & frame);
#endif

    void onLoaded(const QString & text);

public: // Variables
    WBarcodeReader reader;

#ifdef QT_6
    QVideoSink * videoSink;
#endif

    QRect target;

    int orientation;

    int interval;

    bool loading;

#ifdef QT_6
    QTimer timer;
#endif

protected:
    W_DECLARE_PUBLIC(WFilterBarcode)
};

#endif // SK_NO_FILTERBARCODE
#endif // WFILTERBARCODE_P_H
