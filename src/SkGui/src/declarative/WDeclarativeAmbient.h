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

#ifndef WDECLARATIVEAMBIENT_H
#define WDECLARATIVEAMBIENT_H

// Sk includes
#include <WDeclarativePlayer>

#ifndef SK_NO_DECLARATIVEAMBIENT

class WDeclarativeAmbientPrivate;

class SK_GUI_EXPORT WDeclarativeAmbient : public WDeclarativePlayer
{
    Q_OBJECT

public:
#ifdef QT_4
    explicit WDeclarativeAmbient(QDeclarativeItem * parent = NULL);
#else
    explicit WDeclarativeAmbient(QQuickItem * parent = NULL);
#endif

private:
    W_DECLARE_PRIVATE(WDeclarativeAmbient)
};

#include <private/WDeclarativeAmbient_p>

#endif // SK_NO_DECLARATIVEAMBIENT
#endif // WDECLARATIVEAMBIENT_H
