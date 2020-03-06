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

MouseArea
{
    id: checkBoxText

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property int padding: st.checkBoxText_padding
    property int spacing: st.checkBoxText_spacing

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias isHovered: checkBox.isHovered

    property alias checked: checkBox.checked

    property alias text: itemText.text

    //---------------------------------------------------------------------------------------------

    property alias checkBox: checkBox
    property alias itemText: itemText

    //---------------------------------------------------------------------------------------------
    // Signals
    //---------------------------------------------------------------------------------------------

    signal checkClicked

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    width: (text) ? checkBox.width + spacing + itemText.width + padding
                  : checkBox.width

    height: st.checkBoxText_height

    hoverEnabled: true

    cursor: Qt.PointingHandCursor

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    onClicked: checkBox.onClick()

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    CheckBox
    {
        id: checkBox

        anchors.verticalCenter: parent.verticalCenter

        enabled: checkBoxText.enabled

        isHovered: (checkBoxText.containsMouse || containsMouse)

        cursor: Qt.PointingHandCursor

        onCheckClicked: checkBoxText.checkClicked()
    }

    TextBase
    {
        id: itemText

        anchors.left  : checkBox.right
        anchors.top   : parent.top
        anchors.bottom: parent.bottom

        leftMargin: st.dp4

        verticalAlignment: Text.AlignVCenter

        opacity: (checkBoxText.enabled) ? 1.0 : st.text_opacityDisable

        style: st.text_raised

        font.underline: isHovered

        Behavior on opacity
        {
            PropertyAnimation
            {
                duration: st.duration_fast

                easing.type: st.easing
            }
        }
    }
}
