//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkGui.

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

#ifndef WDECLARATIVENOISE_P_H
#define WDECLARATIVENOISE_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

// Qt includes
#include <QBasicTimer>

// Sk includes
#ifdef QT_4
#include <private/WDeclarativeItem_p>
#else
#include <private/WDeclarativeTexture_p>
#endif

#ifndef SK_NO_DECLARATIVENOISE

#ifdef QT_4
class SK_GUI_EXPORT WDeclarativeNoisePrivate : public WDeclarativeItemPrivate
#else
class SK_GUI_EXPORT WDeclarativeNoisePrivate : public WDeclarativeTexturePrivate
#endif
{
protected:
    WDeclarativeNoisePrivate(WDeclarativeNoise * p);

    void init();

public: // Functions
    void updateSize();

    void resetColor();

public: // Variables
    QImage image;

#ifdef QT_NEW
    QPixmap pixmap;

    WDeclarativeNoise::FillMode fillMode;
#endif

    QSize density;

    int interval;

    int increment;

    QColor color;

    QBasicTimer timer;

protected:
    W_DECLARE_PUBLIC(WDeclarativeNoise)
};

#endif // SK_NO_DECLARATIVENOISE
#endif // WDECLARATIVENOISE_P_H
