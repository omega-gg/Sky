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
    id: buttonCheckLabel

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias checked: button.checked

    property alias text: label.text

    property alias buttonText: button.text

    //---------------------------------------------------------------------------------------------

    property alias label : label
    property alias button: button

    //---------------------------------------------------------------------------------------------
    // Signals
    //---------------------------------------------------------------------------------------------

    signal checkClicked

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    width: label.getPreferredWidth() + button.anchors.leftMargin + button.width

    height: st.buttonCheckLabel_height

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    LabelRoundInfo
    {
        id: label

        anchors.left  : parent.left
        anchors.right : parent.right
        anchors.top   : parent.top
        anchors.bottom: parent.bottom

        paddingLeft : st.buttonCheckLabel_labelPaddingLeft
        paddingRight: st.buttonCheckLabel_labelPaddingRight

        itemText.horizontalAlignment: Text.AlignLeft
    }

    ButtonCheck
    {
        id: button

        anchors.right : parent.right
        anchors.top   : parent.top
        anchors.bottom: parent.bottom

        anchors.leftMargin: st.buttonCheckLabel_buttonLeftMargin

        onCheckClicked: buttonCheckLabel.checkClicked()
    }
}
