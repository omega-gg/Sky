//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkyPresentation.

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

Animated
{
    id: slides

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    /* read */ property bool isAnimated: false

    /* read */ property real ratio: width / slide_width

    /* read */ property variant item: (pLoader == loaderA) ? loaderB.item
                                                           : loaderA.item

    property bool animate: st.animate

    property real volume: 1.0

    //---------------------------------------------------------------------------------------------
    // Style

    property int durationAnimation: sp.slides_duration

    property int easing: sp.easing

    property int slide_width : sp.slide_width
    property int slide_height: sp.slide_height

    //---------------------------------------------------------------------------------------------
    // Private

    property int pState: -1

    property variant pLoader: null

    property bool pLoad      : false
    property bool pTransition: false

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias borderSize : border.size
    property alias borderColor: border.color

    //---------------------------------------------------------------------------------------------
    // Signals
    //---------------------------------------------------------------------------------------------

    signal slide
    signal clear

    signal play
    signal backward
    signal forward

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    running: false

    stepMode: Animated.StepManual

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    Component.onCompleted: if (step != -1) slide()

    onStepDirectionChanged: if (step != -1) start()

    onStepChanged: pUpdateStep()

    //---------------------------------------------------------------------------------------------
    // Keys
    //---------------------------------------------------------------------------------------------

    /* QML_EVENT */ Keys.onPressed: function(event)
    {
        if (event.key == Qt.Key_Left)
        {
            event.accepted = true;

            if (pLoad) return;

            if (event.modifiers == sk.keypad(Qt.ControlModifier))
            {
                backward();
            }
            else stepBackward();
        }
        else if (event.key == Qt.Key_Right)
        {
            event.accepted = true;

            if (pLoad) return;

            if (event.modifiers == sk.keypad(Qt.ControlModifier))
            {
                forward();
            }
            else stepForward();
        }
        else if (event.key == Qt.Key_Space)
        {
            event.accepted = true;

            if (pLoad == false)
            {
                play();
            }
        }
    }

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function ds(size)
    {
        return st.dp(size * ratio);
    }

    //---------------------------------------------------------------------------------------------

    function before(index)
    {
        return (step < index);
    }

    function after(index)
    {
        return (step > index);
    }

    //---------------------------------------------------------------------------------------------

    function between(stepA, stepB)
    {
        return (step > stepA && step <= stepB);
    }

    //---------------------------------------------------------------------------------------------

    function from(index, count)
    {
        return (step >= index && step < (index + count));
    }

    //---------------------------------------------------------------------------------------------
    // Events

    function onSlide(step, item) { return null; }

    //---------------------------------------------------------------------------------------------
    // Private

    function pUpdateStep()
    {
        if (pLoader == null)
        {
            pLoad = true;

            clear();

            loaderA.sourceComponent = onSlide(step, null);

            pLoader = loaderB;

            slide();

            pLoad = false;
        }
        else if (pLoader == loaderA)
        {
            var component = onSlide(step, loaderB.sourceComponent);

            if (loaderB.sourceComponent == component) return;

            pLoad = true;

            clear();

            loaderA.sourceComponent = component;

            pLoader = loaderB;

            if (stepDirection == Animated.StepForward)
            {
                 pSlide(loaderB, 0, 2, 0, 1);
            }
            else pSlide(loaderA, 1, 0, 1, 0);
        }
        else
        {
            /* var */ component = onSlide(step, loaderA.sourceComponent);

            if (loaderA.sourceComponent == component) return;

            pLoad = true;

            clear();

            loaderB.sourceComponent = component;

            pLoader = loaderA;

            if (stepDirection == Animated.StepForward)
            {
                 pSlide(loaderA, 0, 1, 1, 0);
            }
            else pSlide(loaderB, 2, 0, 0, 1);
        }
    }

    //---------------------------------------------------------------------------------------------

    function pSlide(loader, stateA, stateB, zA, zB)
    {
        isAnimated = false;

        pState = -1;
        pState = stateA;

        loaderA.z = zA;
        loaderB.z = zB;

        // NOTE: We clip to avoid overlapping the other page during the transition.
        loader.clip = true;

        slide();

        pBorder(loader);

        isAnimated = animate;

        pTransition = true;

        pState = stateB;
    }

    function pBorder(item)
    {
        border.anchors.left = item.right;

        border.visible = true;
    }

    //---------------------------------------------------------------------------------------------

    function pCompleteTransition()
    {
        if (pTransition == false) return;

        pLoad       = false;
        pTransition = false;

        isAnimated = false;

        border.visible = false;

        pLoader.sourceComponent = null;

        loaderA.clip = false;
        loaderB.clip = false;
    }

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

    Loader
    {
        id: loaderA

        anchors.top   : parent.top
        anchors.bottom: parent.bottom

        width: parent.width

        states:
        [
            State
            {
                name: "left"; when: (pState == 1)

                PropertyChanges
                {
                    target: loaderA

                    x: -width - borderSize
                }
            },
            State
            {
                name: "hidden"; when: (pState == 3)

                PropertyChanges
                {
                    target: loaderA

                    opacity: 0.0
                }
            }
        ]

        transitions: Transition
        {
            SequentialAnimation
            {
                NumberAnimation
                {
                    properties: "x, opacity"

                    duration: (isAnimated) ? durationAnimation : 0

                    easing.type: slides.easing
                }

                ScriptAction { script: pCompleteTransition() }
            }
        }
    }

    Loader
    {
        id: loaderB

        anchors.top   : parent.top
        anchors.bottom: parent.bottom

        width: parent.width

        states:
        [
            State
            {
                name: "left"; when: (pState == 2)

                PropertyChanges
                {
                    target: loaderB

                    x: -width - borderSize
                }
            },
            State
            {
                name: "hidden"; when: (pState == 4)

                PropertyChanges
                {
                    target: loaderB

                    opacity: 0.0
                }
            }
        ]

        transitions: Transition
        {
            SequentialAnimation
            {
                NumberAnimation
                {
                    properties: "x, opacity"

                    duration: (isAnimated) ? durationAnimation : 0

                    easing.type: slides.easing
                }

                ScriptAction { script: pCompleteTransition() }
            }
        }
    }

    BorderVertical
    {
        id: border

        size: sp.slides_borderSize

        visible: false
    }
}
