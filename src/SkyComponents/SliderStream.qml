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
    id: sliderStream

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property bool isHovered: slider.containsMouse

    property bool active: true

    property int currentTime: -1
    property int duration   : -1

    //---------------------------------------------------------------------------------------------
    // Style

    property ImageColorFilter filterBar            : st.sliderStream_filterBar
    property ImageColorFilter filterBarHover       : st.sliderStream_filterBarHover
    property ImageColorFilter filterBarDisable     : st.sliderStream_filterBarDisable
    property ImageColorFilter filterBarDisableHover: st.sliderStream_filterBarDisableHover

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias enabled: slider.enabled

    property alias model: slider.model

    property alias value   : slider.value
    property alias position: slider.position

    property alias minimum: slider.minimum
    property alias maximum: slider.maximum

    property alias pageStep  : slider.pageStep
    property alias singleStep: slider.singleStep

    //---------------------------------------------------------------------------------------------

    property alias slider: slider

    property alias background: slider.background
    property alias foreground: slider.foreground

    property alias handle: slider.handle

    //---------------------------------------------------------------------------------------------
    // Style

    property alias filterHandle     : slider.filterHandle
    property alias filterHandleHover: slider.filterHandleHover

    //---------------------------------------------------------------------------------------------
    // Signal
    //---------------------------------------------------------------------------------------------

    signal handleReleased

    signal reset

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    height: st.sliderStream_height

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    onCurrentTimeChanged: pUpdate()
    onDurationChanged   : pUpdate()

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function moveTo(pos)
    {
        slider.moveTo(pos);
    }

    //---------------------------------------------------------------------------------------------
    // Private

    function pUpdate()
    {
        maximum = duration;

        if (slider.pressed == false && slider.drag.active == false)
        {
            value = currentTime;
        }
    }

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    ButtonStream
    {
        id: buttonLeft

        anchors.verticalCenter: parent.verticalCenter

        paddingRight: st.dp9

        enabled: foreground.visible

        text: controllerPlaylist.getPlayerTime(value, 7)

        itemText.visible: (duration != -1)

        onClicked: reset()
    }

    ButtonStream
    {
        id: buttonRight

        anchors.right: parent.right

        anchors.verticalCenter: parent.verticalCenter

        paddingLeft: st.dp9

        enabled: false

        text: controllerPlaylist.getPlayerTime(duration, 7)

        itemText.visible: (duration != -1)
    }

    Slider
    {
        id: slider

        anchors.left : buttonLeft.right
        anchors.right: buttonRight.left

        anchors.leftMargin : -st.dp24
        anchors.rightMargin: -st.dp24

        anchors.verticalCenter: parent.verticalCenter

        hoverRetain: true

        wheelEnabled: false

        filterBar: (active) ? sliderStream.filterBar
                            : filterBarDisable

        filterBarHover: (active) ? sliderStream.filterBarHover
                                 : filterBarDisableHover

        onHandleReleased: sliderStream.handleReleased()
    }
}
