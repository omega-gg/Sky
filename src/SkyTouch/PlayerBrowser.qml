//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkyTouch.

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

BasePlayerBrowser
{
    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias buttonPrevious: buttonPrevious
    property alias buttonNext    : buttonNext

    property alias buttonPlay: buttonPlay

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    anchors.margins: st.margins

    areaBackward.visible: buttonPrevious.visible
    areaForward .visible: buttonNext    .visible

    areaBackward.hoverEnabled: buttonPrevious.visible
    areaForward .hoverEnabled: buttonNext    .visible

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------
    // Private

    function pGetSize()
    {
        var size = Math.round(width / 12);

        size = Math.max(st.dp48, size);

        return Math.min(size, st.dp64);
    }

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    ButtonTouchIcon
    {
        id: buttonPrevious

        anchors.left: parent.left

        anchors.verticalCenter: parent.verticalCenter

        width : pGetSize()
        height: width

        margins: Math.round(width / 6)

        visible: player.hasPreviousTrack

        isHovered: (containsMouse || areaBackward.containsMouse)

        isPressed: (pressed || isReturnPressed || areaBackward.pressed || pBackward)

        icon: st.icon_backward

        onPressed : scrollPrevious()
        onReleased: scrollClear   ()
    }

    ButtonTouchIcon
    {
        id: buttonNext

        anchors.right: parent.right

        anchors.verticalCenter: parent.verticalCenter

        width : buttonPrevious.width
        height: buttonPrevious.height

        margins: buttonPrevious.margins

        visible: player.hasNextTrack

        isHovered: (containsMouse || areaForward.containsMouse)

        isPressed: (pressed || isReturnPressed || areaForward.pressed || pForward)

        icon: st.icon_forward

        onPressed : scrollNext ()
        onReleased: scrollClear()
    }

    ButtonTouchIcon
    {
        id: buttonPlay

        anchors.centerIn: parent

        width : buttonPrevious.width
        height: buttonPrevious.height

        margins: buttonPrevious.margins

        visible: (player.source != "")

        icon: st.icon_play

        cursor: Qt.PointingHandCursor

        onClicked: player.play()
    }
}
