//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkyComponents module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

import QtQuick 1.0
import Sky     1.0

LineEditBox
{
    id: lineEditBoxClear

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    paddingRight: (buttonMask.visible) ? buttonMask.width
                                       : padding

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function onClear()
    {
        text = "";
    }

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    ButtonMask
    {
        id: buttonMask

        anchors.right : parent.right
        anchors.top   : parent.top
        anchors.bottom: parent.bottom

        width: height

        visible: (lineEditBoxClear.isFocused == false && text != "")

        icon          : st.icon16x16_close
        iconSourceSize: st.size16x16

        onClicked: onClear()
    }
}
