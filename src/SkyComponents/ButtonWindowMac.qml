//=================================================================================================
/*
    Copyright (C) 2015-2026 Sky kit authors. <http://omega.gg/Sky>

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

BaseButton
{
    id: buttonWindowMac

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property bool showIcon: isHovered

    property color color
    property color colorPress

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias icon: itemIcon.source

    property alias background: background

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    width : st.buttonWindowMac_width
    height: st.buttonWindowMac_height

    cursor: Qt.ArrowCursor

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

    Rectangle
    {
        id: background

        anchors.centerIn: parent

        width : st.buttonWindowMac_size
        height: width

        radius: height

        // NOTE macOS: The buttons are gray in an inactive window, until they get hovered.
        color:
        {
            if (window.isActive == false && showIcon == false)
            {
                return st.buttonWindowMac_colorDisable;
            }
            else if (isPressed)
            {
                return colorPress;
            }
            else return buttonWindowMac.color;
        }
    }

    ImageScale
    {
        id: itemIcon

        anchors.centerIn: parent

        sourceSize: st.size12x12

        visible: showIcon
    }
}
