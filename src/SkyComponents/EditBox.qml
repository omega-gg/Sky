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

BaseEdit
{
    id: editBox

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property bool enableClearFocus: true

    //---------------------------------------------------------------------------------------------
    // Style

    property real focusSize: st.editBox_focusSize

    property color colorA: st.editBox_colorA
    property color colorB: st.editBox_colorB

    property color colorFocus: st.editBox_colorFocus

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias background: background
    property alias shadow    : shadow
    property alias itemFocus : itemFocus

    //---------------------------------------------------------------------------------------------
    // Settings style
    //---------------------------------------------------------------------------------------------

    colorDefault: st.editBox_colorDefault

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    Rectangle
    {
        id: background

        anchors.fill: parent

        z: -1

        gradient: Gradient
        {
            GradientStop { position: 0.0; color: colorA }
            GradientStop { position: 1.0; color: colorB }
        }
    }

    RectangleShadow
    {
        id: shadow

        anchors.left : parent.left
        anchors.right: parent.right

        height: st.editBox_shadowHeight

        opacity: (isHovered) ? st.baseEdit_shadowOpacityB
                             : st.baseEdit_shadowOpacityA

        direction: Sk.Down
    }

    RectangleBorders
    {
        id: itemFocus

        anchors.fill: parent

        size: focusSize

        opacity: (window.isActive && isFocused)

        color: st.editBox_colorFocus

        Behavior on opacity
        {
            PropertyAnimation { duration: st.duration_fast }
        }

        RectangleBorders
        {
            anchors.fill: parent

            anchors.margins: focusSize
        }
    }
}
