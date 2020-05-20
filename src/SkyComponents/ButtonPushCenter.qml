//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of Sky kit.

    - GNU Lesser General Public License Usage:
    This file may be used under the terms of the GNU Lesser General Public License version 3 as
    published by the Free Software Foundation and appearing in the LICENSE.md file included in the
    packaging of this file. Please review the following information to ensure the GNU Lesser
    General Public License requirements will be met: https://www.gnu.org/licenses/lgpl.html.
*/
//=================================================================================================

import QtQuick 1.0
import Sky     1.0

ButtonPush
{
    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    radius: 0

    clip: true

    padding: st.buttonPush_padding - margins

    background.anchors.leftMargin: -(borderSize / 2)

    background.anchors.rightMargin: background.anchors.leftMargin

// FIXME Qt4: Something is wrong with clipping and margins.
//#QT_4
    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    RectangleBorders
    {
        anchors.fill: background

        borderTop   : 0
        borderBottom: 0
    }
//#END
}
