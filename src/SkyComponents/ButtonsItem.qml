//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkyComponents.

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

import QtQuick 1.0
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

        icon          : st.icon12x12_contextualDown
        iconSourceSize: st.size12x12

        cursor: Qt.PointingHandCursor

        onPressed: contextual()
    }

    ButtonPushRightIcon
    {
        id: buttonClose

        anchors.left: buttonContextual.right

        width : st.buttonsItem_buttonWidth
        height: st.buttonsItem_buttonHeight

        icon          : st.icon12x12_close
        iconSourceSize: st.size12x12

        cursor: Qt.PointingHandCursor

        colorHoverA: st.button_colorConfirmHoverA
        colorHoverB: st.button_colorConfirmHoverB

        colorPressA: st.button_colorConfirmPressA
        colorPressB: st.button_colorConfirmPressB

        filterIcon: (isHovered || isPressed) ? st.button_filterIconB
                                             : st.button_filterIconA

        onClicked: close()
    }
}
