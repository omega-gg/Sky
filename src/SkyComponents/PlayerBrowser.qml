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

BasePlayerBrowser
{
    id: playerBrowser

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property bool loading: false

    property bool enableTitle : true
    property bool enableAuthor: true

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias title: itemTitle.text

    //---------------------------------------------------------------------------------------------

    property alias barTitle  : barTitle
    property alias barDetails: barDetails

    property alias buttonPrevious: buttonPrevious
    property alias buttonNext    : buttonNext

    property alias buttonPlay: buttonPlay

    property alias areaPlay: areaPlay

    //---------------------------------------------------------------------------------------------
    // Signals
    //---------------------------------------------------------------------------------------------

    signal contextual

    signal titleClicked (variant mouse)
    signal authorClicked(variant mouse)

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    // NOTE: This is for the 'contextual' signal.
    acceptedButtons: Qt.RightButton

    areaBackward.anchors.top   : (barTitle  .visible) ? barTitle.bottom : playerBrowser.top
    areaBackward.anchors.bottom: (barDetails.visible) ? barDetails.top  : playerBrowser.bottom

    areaBackward.visible: buttonPrevious.visible
    areaForward .visible: buttonNext    .visible

    areaBackward.hoverEnabled: buttonPrevious.visible
    areaForward .hoverEnabled: buttonNext    .visible

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    onPressed: contextual()

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function play()
    {
        if (tabs.highlightedTab)
        {
            tabs.highlightedTab = null;
        }

        player.play();

        window.clearFocus();
    }

    //---------------------------------------------------------------------------------------------
    // Private

    function pGetSize()
    {
        var size = Math.round(width / 6);

        size = Math.max(st.dp48, size);

        return Math.min(size, st.dp80);
    }

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

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

    ButtonPushOverlay
    {
        id: buttonPrevious

        anchors.left: parent.left

        anchors.leftMargin: st.dp8

        anchors.verticalCenter: parent.verticalCenter

        width : pGetSize()
        height: width

        iconWidth : Math.round(width / 3)
        iconHeight: iconWidth

        borderSize: st.border_sizeFocus

        visible: (playerTab == tab) ? player.hasPreviousTrack
                                    : tab   .hasPreviousTrack

        isHovered: (containsMouse || areaBackward.containsMouse)

        isPressed: (pressed || isReturnPressed || areaBackward.pressed || pBackward)

        icon       : st.icon_backward
        iconScaling: true

        cursor: Qt.PointingHandCursor

        onPressed : scrollPrevious()
        onReleased: scrollClear   ()
    }

    ButtonPushOverlay
    {
        id: buttonNext

        anchors.right: parent.right

        anchors.rightMargin: st.dp8

        anchors.verticalCenter: parent.verticalCenter

        width : buttonPrevious.width
        height: buttonPrevious.height

        iconWidth : buttonPrevious.iconWidth
        iconHeight: buttonPrevious.iconHeight

        borderSize: buttonPrevious.borderSize

        visible: (playerTab == tab) ? player.hasNextTrack
                                    : tab   .hasNextTrack

        isHovered: (containsMouse || areaForward.containsMouse)

        isPressed: (pressed || isReturnPressed || areaForward.pressed || pForward)

        icon       : st.icon_forward
        iconScaling: true

        cursor: Qt.PointingHandCursor

        onPressed : scrollNext ()
        onReleased: scrollClear()
    }

    ButtonPushOverlay
    {
        id: buttonPlay

        anchors.centerIn: parent

        width : buttonPrevious.width
        height: buttonPrevious.height

        iconWidth : buttonPrevious.iconWidth
        iconHeight: buttonPrevious.iconHeight

        borderSize: buttonPrevious.borderSize

        visible: (loading || enabled)

        enabled: (tab != null && tab.isValid)

        isHovered: (containsMouse || areaPlay.containsMouse)

        isPressed: (pressed || isReturnPressed || areaPlay.pressed)

        icon       : st.icon_play
        iconScaling: true

        cursor: Qt.PointingHandCursor

        itemIcon.visible: (loading == false)

        onClicked: play()

        IconLoading
        {
            id: itemLoading

            anchors.centerIn: parent

            width : Math.round(buttonPlay.width / 1.6)
            height: width

            visible: loading

            scaling: true

            filterDefault: st.labelRound_filterIcon
        }
    }

    Item
    {
        id: barTitle

        anchors.left : parent.left
        anchors.right: parent.right

        height: st.dp32

        visible: (buttonPlay.visible && title)

        Behavior on opacity
        {
            PropertyAnimation
            {
                duration: st.duration_fast

                easing.type: st.easing
            }
        }

        Rectangle
        {
            anchors.fill: parent

            opacity: st.playerBrowser_opacity

            color: st.playerBrowser_colorBar
        }

        TextLink
        {
            id: itemTitle

            anchors.fill: parent

            anchors.leftMargin : st.dp8
            anchors.rightMargin: st.dp8

            verticalAlignment: Text.AlignVCenter

            enabled: enableTitle

            text: (tab) ? tab.title : ""

            color: (isHovered) ? st.playerBrowser_colorTextHover
                               : st.playerBrowser_colorText

            style: st.text_raised

            font.pixelSize: st.dp16

            /* QML_EVENT */ onClicked: function(mouse) { titleClicked(mouse); }
        }
    }

    LabelLoadingText
    {
        anchors.bottom: parent.bottom

        anchors.horizontalCenter: parent.horizontalCenter

        visible: (tab && tab.isLoading)

        text: qsTr("Loading...")
    }

    Item
    {
        id: barDetails

        anchors.left  : parent.left
        anchors.right : parent.right
        anchors.bottom: parent.bottom

        height: st.dp32

        visible: (tab && tab.isLoaded)

        Behavior on opacity
        {
            PropertyAnimation
            {
                duration: st.duration_fast

                easing.type: st.easing
            }
        }

        Rectangle
        {
            anchors.fill: parent

            opacity: st.playerBrowser_opacity

            color: st.playerBrowser_colorBar
        }

        TextBase
        {
            id: itemDuration

            anchors.left  : parent.left
            anchors.top   : parent.top
            anchors.bottom: parent.bottom

            anchors.leftMargin: st.dp8

            verticalAlignment: Text.AlignVCenter

            text: (tab) ? gui.getTrackDuration(tab.duration) : ""

            color: st.playerBrowser_colorText

            style: st.text_raised

            font.pixelSize: st.dp14
        }

        TextDate
        {
            anchors.left  : itemDuration.right
            anchors.top   : parent.top
            anchors.bottom: parent.bottom

            anchors.leftMargin: st.dp4
            anchors.topMargin : st.dp1

            horizontalAlignment: Text.AlignRight
            verticalAlignment  : Text.AlignVCenter

            width: Math.max(0, itemAuthor.width - itemAuthor.mouseArea.width - st.dp8)

            date: (tab) ? tab.date : null

            color: (isHovered) ? st.playerBrowser_colorTextHover
                               : st.playerBrowser_colorText

            style: st.text_raised
        }

        TextLink
        {
            id: itemAuthor

            anchors.left  : itemDuration.right
            anchors.right : parent.right
            anchors.top   : parent.top
            anchors.bottom: parent.bottom

            anchors.leftMargin : st.dp4
            anchors.rightMargin: st.dp8

            horizontalAlignment: Text.AlignRight
            verticalAlignment  : Text.AlignVCenter

            enabled: enableAuthor

            text: (tab) ? gui.getTrackAuthor(tab.author, tab.feed) : ""

            color: (isHovered) ? st.playerBrowser_colorTextHover
                               : st.playerBrowser_colorText

            style: st.text_raised

            font.pixelSize: st.dp14

            /* QML_EVENT */ onClicked: function(mouse) { authorClicked(mouse); }
        }
    }
}
