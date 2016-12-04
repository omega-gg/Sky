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
    id: buttonCheck

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property bool checked: false

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias text: buttonOn.text

    //---------------------------------------------------------------------------------------------

    property alias buttonOn : buttonOn
    property alias buttonOff: buttonOff

    //---------------------------------------------------------------------------------------------
    // Signals
    //---------------------------------------------------------------------------------------------

    signal checkClicked

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    width: buttonOn.x + buttonOn.width

    height: st.buttonCheck_height

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function pClick()
    {
        checked = !(checked);

        checkClicked();
    }

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    ButtonPush
    {
        id: buttonOn

        anchors.left  : buttonOff.right
        anchors.top   : parent.top
        anchors.bottom: parent.bottom

        anchors.leftMargin: st.buttonCheck_buttonLeftMargin

        paddingLeft : st.buttonCheck_buttonPaddingLeft
        paddingRight: st.buttonCheck_buttonPaddingRight

        enabled: buttonCheck.enabled

        checkable: true
        checked  : buttonCheck.checked

        text: qsTr("ON")

        onClicked: pClick()
    }

    ButtonRound
    {
        id: buttonOff

        anchors.top   : parent.top
        anchors.bottom: parent.bottom

        width: buttonOn.height

        icon          : st.icon16x16_close
        iconSourceSize: st.size16x16

        hoverRetain: true

        enabled: buttonCheck.enabled

        checked   : (buttonCheck.checked == false)
        checkHover: false

        onClicked: pClick()
    }
}
