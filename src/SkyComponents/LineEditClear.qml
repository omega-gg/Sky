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

LineEdit
{
    id: lineEditClear

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property bool enableClearFocus: true

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    isHovered: (containsMouse || button.containsMouse)

    paddingRight: (button.visible) ? st.lineEditClear_paddingRight
                                   : st.lineEdit_padding

    shadow.anchors.rightMargin: (button.visible) ? button.width : 0

    //---------------------------------------------------------------------------------------------
    // Functions events
    //---------------------------------------------------------------------------------------------

    function onClear()
    {
        if (enableClearFocus)
        {
            text = "";

            window.clearFocus();
        }
        else if (text)
        {
            text = "";
        }
        else window.clearFocus();
    }

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    ButtonPianoIcon
    {
        id: button

        anchors.right: parent.right

        anchors.rightMargin: st.dp5

        anchors.verticalCenter: parent.verticalCenter

        width : st.lineEditClear_buttonSize + borderSizeWidth
        height: st.lineEditClear_buttonSize + borderSizeHeight

        borderLeft  : borderSize
        borderRight : st.line_size
        borderTop   : st.line_size
        borderBottom: st.line_size

        visible: lineEditClear.isFocused

        icon          : st.icon16x16_close
        iconSourceSize: st.size16x16

        onClicked: onClear()
    }
}
