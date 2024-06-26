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

Item
{
    id: sliderStream

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property bool isHovered: slider.hoverActive

    property int sizeChapter: st.sliderStream_sizeChapter

    /* read */ property int handleRange: handleMaximum - handleMinimum

    property bool active: true
    property bool live  : false

    property int currentTime: -1
    property int duration   : -1

    property real progress: 0.0

    property variant chapters: null

    /* read */ property variant times: []

    //---------------------------------------------------------------------------------------------
    // Style

    property int durationAnimation: st.sliderStream_durationAnimation

    property int intervalProgress: st.sliderStream_intervalProgress

    property real opacityProgressA: st.sliderStream_opacityProgressA
    property real opacityProgressB: st.sliderStream_opacityProgressB

    property color colorBarA: st.sliderStream_colorBarA
    property color colorBarB: st.sliderStream_colorBarB

    property color colorBarHoverA: st.sliderStream_colorBarHoverA
    property color colorBarHoverB: st.sliderStream_colorBarHoverB

    property color colorBarDisableA: st.sliderStream_colorBarDisableA
    property color colorBarDisableB: st.sliderStream_colorBarDisableB

    property color colorBarDisableHoverA: st.sliderStream_colorBarDisableHoverA
    property color colorBarDisableHoverB: st.sliderStream_colorBarDisableHoverB

    property color colorBarProgressA: st.sliderStream_colorBarProgressA
    property color colorBarProgressB: st.sliderStream_colorBarProgressB

    property color colorChapter       : st.sliderStream_colorChapter
    property color colorChapterActive : st.sliderStream_colorChapterActive
    property color colorChapterDisable: st.sliderStream_colorChapterDisable

    //---------------------------------------------------------------------------------------------
    // Private

    property int pChapterSize: (handle.width - sizeChapter) / 2

    property int pChapterX: handleMinimum + pChapterSize

    property int pHandleX: position + pChapterSize + st.dp1

    property int pState: 0

    property int pProgressWidth: background.width * pProgress

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

    property alias handleMinimum: slider.handleMinimum
    property alias handleMaximum: slider.handleMaximum

    property alias pageStep  : slider.pageStep
    property alias singleStep: slider.singleStep

    //---------------------------------------------------------------------------------------------

    property alias slider: slider

    property alias background: slider.background
    property alias foreground: slider.foreground

    property alias handle: slider.handle

    //---------------------------------------------------------------------------------------------
    // Signal
    //---------------------------------------------------------------------------------------------

    signal handlePressed
    signal handleReleased

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

    onChaptersChanged:
    {
        var array = new Array;

        for (var i = 0; i < chapters.length; i++)
        {
            var time = chapters[i].time;

            if (time >= duration) continue;

            array.push(time);
        }

        times = array;
    }

    onPStateChanged: pUpdateProgress()

    //---------------------------------------------------------------------------------------------
    // Animations
    //---------------------------------------------------------------------------------------------

    Behavior on pProgress
    {
        id: behaviorProgress

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

    function moveTo(pos)
    {
        slider.moveTo(pos);
    }

    //---------------------------------------------------------------------------------------------
    // Private

    function pUpdate()
    {
        maximum = Math.max(0, duration);

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

    function pGetX(index)
    {
        return handleRange * (times[index] / duration) + pChapterX;
    }

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

    ButtonStream
    {
        id: buttonLeft

        anchors.verticalCenter: parent.verticalCenter

        paddingRight: (sk.osLinux) ? st.dp10 : st.dp9

        enabled: false

        text: controllerPlaylist.getPlayerTime(value, 7)

        itemText.visible: (duration > 0)
    }

    ButtonStream
    {
        id: buttonRight

        anchors.right: parent.right

        anchors.verticalCenter: parent.verticalCenter

        paddingLeft: (sk.osLinux) ? st.dp10 : st.dp9

        enabled: false

        text: (live) ? qsTr("L I V E")
                     : controllerPlaylist.getPlayerTime(duration, 7)

        itemText.visible: (buttonLeft.visible || live)
    }

    Slider
    {
        id: slider

        anchors.left : buttonLeft.right
        anchors.right: buttonRight.left

        anchors.leftMargin : -st.dp24
        anchors.rightMargin: -st.dp24

        anchors.verticalCenter: parent.verticalCenter

        enabled: (duration > 0)

        hoverRetain: true

        wheelEnabled: false

        colorBarA: (active) ? sliderStream.colorBarA
                            : colorBarDisableA

        colorBarB: (active) ? sliderStream.colorBarB
                            : colorBarDisableB

        colorBarHoverA: (active) ? sliderStream.colorBarHoverA
                                 : colorBarDisableHoverA

        colorBarHoverB: (active) ? sliderStream.colorBarHoverB
                                 : colorBarDisableHoverB

        // NOTE: We want the background to be behind the itemProgress.
        background.z: -2

        // NOTE: We want the foreground to be behind the chapters.
        foreground.z: -1

        foreground.opacity: (pState) ? opacityProgressA
                                     : opacityProgressB

        onHandlePressed : sliderStream.handlePressed ()
        onHandleReleased: sliderStream.handleReleased()

        Rectangle
        {
            id: itemProgress

            anchors.left  : background.left
            anchors.top   : background.top
            anchors.bottom: background.bottom

            width: Math.max(st.dp32, pProgressWidth)

            radius: parent.radius

            z: -2

            visible: (opacity != 0.0)
            opacity: 0.0

            gradient: Gradient
            {
                GradientStop { position: 0.0; color: colorBarProgressA }
                GradientStop { position: 1.0; color: colorBarProgressB }
            }

//#QT_4
            smooth: true
//#END

            border.width: slider.borderSize
            border.color: st.border_color

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

                                itemProgress.opacity = 0.0;

                                behaviorOpacity.enabled = false;
                            }
                            else if (pState == 1)
                            {
                                animationBuffer.start();
                            }
                            else // if (pState == 2)
                            {
                                animationLoad.start();
                            }
                        }
                    }
                }
            }

            onStateChanged:
            {
                animationBuffer.stop();
                animationLoad  .stop();

                if (pState)
                {
                    behaviorOpacity.enabled = true;

                    itemProgress.opacity = opacityProgressB;

                    behaviorOpacity.enabled = false;
                }
                else if (pProgress == 0)
                {
                    itemProgress.opacity = 0;
                }
            }

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

            SequentialAnimation on opacity
            {
                id: animationBuffer

                running: false

                loops: Animation.Infinite

                PropertyAnimation
                {
                    to: 0.0

                    duration: intervalProgress

                    easing.type: st.easing
                }

                PropertyAnimation
                {
                    to: opacityProgressB

                    duration: intervalProgress

                    easing.type: st.easing
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

                    easing.type: st.easing
                }

                PropertyAnimation
                {
                    to: opacityProgressB

                    duration: intervalProgress

                    easing.type: st.easing
                }
            }
        }

        Repeater
        {
            model: times.length

            Rectangle
            {
                anchors.verticalCenter: slider.verticalCenter

                width : sizeChapter
                height: sizeChapter

                radius: height

                x: pGetX(index)

                z: foreground.z

                visible: foreground.visible

                color:
                {
                    if (x > pHandleX)
                    {
                        return colorChapter;
                    }
                    else if (active)
                    {
                        return colorChapterActive;
                    }
                    else return colorChapterDisable;
                }

//#QT_4
                smooth: true
//#END
            }
        }
    }
}
