//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

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

    property real progress: 0.0

    //---------------------------------------------------------------------------------------------
    // Style

    property int durationAnimation: st.sliderStream_durationAnimation

    property int intervalProgress: st.sliderStream_intervalProgress

    property real opacityProgressA: st.sliderStream_opacityProgressA
    property real opacityProgressB: st.sliderStream_opacityProgressB

    property ImageColorFilter filterBar            : st.sliderStream_filterBar
    property ImageColorFilter filterBarHover       : st.sliderStream_filterBarHover
    property ImageColorFilter filterBarDisable     : st.sliderStream_filterBarDisable
    property ImageColorFilter filterBarDisableHover: st.sliderStream_filterBarDisableHover
    property ImageColorFilter filterBarProgress    : st.sliderStream_filterBarProgress

    //---------------------------------------------------------------------------------------------
    // Private

    property int pState: 0

    property int pProgressWidth: slider.width * pProgress

    property real pProgress: 0.0

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

    onProgressChanged: pUpdateProgress()

    onPStateChanged: pUpdateProgress()

    //---------------------------------------------------------------------------------------------
    // Animations
    //---------------------------------------------------------------------------------------------

    Behavior on pProgress
    {
        id: behaviorProgress

        enabled: false

        PropertyAnimation { duration: durationAnimation }
    }

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

    function pUpdateProgress()
    {
        if (progress == 0.0)
        {
            pState = 0;

            if (pProgress != 1.0)
            {
                pProgress = progress;
            }

            return;
        }
        else if (progress == -1.0)
        {
            pState = 1;

            pProgress = 1.0;

            return;
        }

        if (progress == 1.0)
        {
             pState = 0;
        }
        else pState = 2;

        behaviorProgress.enabled = true;

        pProgress = progress;

        behaviorProgress.enabled = false;
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

        background.z: -1

        foreground.opacity: (pState) ? opacityProgressA
                                     : opacityProgressB

        onHandleReleased: sliderStream.handleReleased()

        BorderImageScale
        {
            id: imageProgress

            anchors.top   : parent.top
            anchors.bottom: parent.bottom

            width: Math.max(st.dp32, pProgressWidth)

            z: -1

            visible: (opacity != 0.0)
            opacity: 0.0

            source: st.slider_sourceForeground

            border
            {
                left : slider.borderForeground.x;     top   : slider.borderForeground.y
                right: slider.borderForeground.width; bottom: slider.borderForeground.height
            }

            filter: filterBarProgress

            states:
            [
                State { name: "buffering"; when: (pState == 1) },
                State { name: "loading";   when: (pState == 2) }
            ]

            transitions: Transition
            {
                SequentialAnimation
                {
                    PauseAnimation { duration: durationAnimation }

                    ScriptAction
                    {
                        script:
                        {
                            if (pState == 0)
                            {
                                behaviorOpacity.enabled = true;

                                imageProgress.opacity = 0.0;

                                behaviorOpacity.enabled = false;
                            }
                            else if (pState == 1)
                            {
                                animationBuffer.running = true;
                            }
                            else // if (pState == 2)
                            {
                                animationLoad.running = true;
                            }
                        }
                    }
                }
            }

            onStateChanged:
            {
                animationBuffer.running = false;
                animationLoad  .running = false;

                if (pState)
                {
                    behaviorOpacity.enabled = true;

                    imageProgress.opacity = opacityProgressB;

                    behaviorOpacity.enabled = false;
                }
                else if (pProgress == 0)
                {
                    imageProgress.opacity = 0;
                }
            }

            Behavior on opacity
            {
                id: behaviorOpacity

                enabled: false

                PropertyAnimation { duration: durationAnimation }
            }

            SequentialAnimation on opacity
            {
                id: animationBuffer

                running: false

                loops: Animation.Infinite

                PropertyAnimation
                {
                    to: 0.0

                    duration: intervalProgress
                }

                PropertyAnimation
                {
                    to: opacityProgressB

                    duration: intervalProgress
                }
            }

            SequentialAnimation on opacity
            {
                id: animationLoad

                running: false

                loops: Animation.Infinite

                PropertyAnimation
                {
                    to: opacityProgressA

                    duration: intervalProgress
                }

                PropertyAnimation
                {
                    to: opacityProgressB

                    duration: intervalProgress
                }
            }
        }
    }
}
