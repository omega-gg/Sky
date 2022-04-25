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
    // Children
    //---------------------------------------------------------------------------------------------

    CheckBox
    {
        id: checkBox

        anchors.verticalCenter: parent.verticalCenter

        enabled: checkBoxText.enabled

        isHovered: (checkBoxText.hoverActive || hoverActive)

        cursor: Qt.PointingHandCursor

        onCheckClicked: checkBoxText.checkClicked()
    }

    TextBase
    {
        id: itemText

        anchors.left  : checkBox.right
        anchors.top   : parent.top
        anchors.bottom: parent.bottom

        anchors.leftMargin: st.dp4

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
