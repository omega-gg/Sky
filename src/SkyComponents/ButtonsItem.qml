//=================================================================================================
/*
    Copyright (C) 2015-2016 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkyComponents module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

import QtQuick 1.1
import Sky     1.0

Item
{
    id: buttonsItem

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias checked: buttonContextual.checked

    //---------------------------------------------------------------------------------------------

    property alias buttonContextual: buttonContextual
    property alias buttonClose     : buttonClose

    //---------------------------------------------------------------------------------------------
    // Signals
    //---------------------------------------------------------------------------------------------

    signal contextual
    signal close

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    width : buttonContextual.width + buttonClose.width
    height: st.buttonsItem_buttonHeight

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    ButtonPushLeftIcon
    {
        id: buttonContextual

        width : st.buttonsItem_buttonWidth
        height: st.buttonsItem_buttonHeight

        icon          : st.icon16x16_contextualDown
        iconSourceSize: st.size16x16

        cursor: MouseArea.PointingHandCursor

        onPressed: contextual()
    }

    ButtonPushRightIcon
    {
        id: buttonClose

        anchors.left: buttonContextual.right

        width : st.buttonsItem_buttonWidth
        height: st.buttonsItem_buttonHeight

        icon          : st.icon16x16_close
        iconSourceSize: st.size16x16

        cursor: MouseArea.PointingHandCursor

        filterHover: st.buttonPushConfirm_filterHover
        filterPress: st.buttonPushConfirm_filterPress

        onClicked: close()
    }
}
