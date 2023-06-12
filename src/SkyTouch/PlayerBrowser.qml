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
    id: playerBrowser

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property bool active: true

    property bool animate: st.animate

    property bool scaling: (st.isTiny == false)

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias buttonPrevious: buttonPrevious
    property alias buttonNext    : buttonNext

    property alias buttonPlay: buttonPlay

    property alias areaPlay: areaPlay

    //---------------------------------------------------------------------------------------------
    // Style

    property int durationAnimation: st.playerBrowser_durationAnimation

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    areaBackward.visible: buttonPrevious.visible
    areaForward .visible: buttonNext    .visible

    areaBackward.hoverEnabled: buttonPrevious.visible
    areaForward .hoverEnabled: buttonNext    .visible

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    onActiveChanged: st.animateShow(playerBrowser, active, behaviorOpacity, animate)

    //---------------------------------------------------------------------------------------------
    // Animations
    //---------------------------------------------------------------------------------------------

    Behavior on opacity
    {
        id: behaviorOpacity

        enabled: false

        PropertyAnimation
        {
            duration: durationAnimation

            easing.type: st.easing
        }
    }

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function play() { player.play(); }

    //---------------------------------------------------------------------------------------------
    // Private

    function pGetSize()
    {
        if (scaling == false) return st.buttonTouch_size;

        var size = Math.round(width / 12);

        size = Math.max(st.buttonTouch_size, size);

        return Math.min(size, st.dp64);
    }

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

    ButtonTouchIcon
    {
        id: buttonPrevious

        anchors.left: parent.left

        anchors.verticalCenter: parent.verticalCenter

        width : buttonPlay.width
        height: buttonPlay.height

        radius: height

        margins: buttonPlay.margins

        visible: player.hasPreviousTrack

        isHovered: (hoverActive || areaBackward.hoverActive)

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

        width : buttonPlay.width
        height: buttonPlay.height

        radius: height

        margins: buttonPlay.margins

        visible: player.hasNextTrack

        isHovered: (hoverActive || areaForward.hoverActive)

        isPressed: (pressed || isReturnPressed || areaForward.pressed || pForward)

        icon: st.icon_forward

        onPressed : scrollNext ()
        onReleased: scrollClear()
    }

    MouseArea
    {
        id: areaPlay

        anchors.centerIn: parent

        width : Math.round(buttonPlay.width * 1.4)
        height: width

        visible: buttonPlay.visible

        hoverEnabled: buttonPlay.visible

        cursor: Qt.PointingHandCursor

        onClicked: play()
    }

    ButtonTouchIcon
    {
        id: buttonPlay

        anchors.centerIn: parent

        width : pGetSize()
        height: width

        radius: height

        margins: Math.round(width / 6)

        visible: (player.source != "")

        isHovered: (hoverActive || areaPlay.hoverActive)

        isPressed: (pressed || isReturnPressed || areaPlay.pressed)

        icon: st.icon_play

        //-----------------------------------------------------------------------------------------
        // NOTE: We want the play button slightly shifted to the right

        itemIcon.anchors.left: buttonPlay.left

        itemIcon.anchors.leftMargin: Math.round(width / 3.4)

        itemIcon.anchors.centerIn: undefined

        itemIcon.anchors.verticalCenter: buttonPlay.verticalCenter

        //-----------------------------------------------------------------------------------------

        cursor: Qt.PointingHandCursor

        onClicked: play()
    }
}
