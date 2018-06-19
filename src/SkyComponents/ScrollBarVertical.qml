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
    id: scrollBar

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property int handleSize: getHandleSize()

    //---------------------------------------------------------------------------------------------
    // Style

    property int wheelMultiplier: st.scrollBar_wheelMultiplier

    property int handleMinimumSize: st.scrollBar_handleMinimumSize

    property color colorA: st.scrollBar_colorA
    property color colorB: st.scrollBar_colorB

    property color colorBorder: st.scrollBar_colorBorder

    property color colorHandleA: st.scrollBar_colorHandleA
    property color colorHandleB: st.scrollBar_colorHandleB

    property color colorHandleHoverA: st.scrollBar_colorHandleHoverA
    property color colorHandleHoverB: st.scrollBar_colorHandleHoverB

    property ImageColorFilter filterHandle     : st.scrollBar_filterHorizontalHandle
    property ImageColorFilter filterHandleHover: st.scrollBar_filterHorizontalHandleHover
    property ImageColorFilter filterHandlePress: st.scrollBar_filterHorizontalHandlePress

    //---------------------------------------------------------------------------------------------
    // Private

    property bool pPagePressed: false

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias handle: handle

    //---------------------------------------------------------------------------------------------
    // Model

    property alias model: model

    property alias value   : model.value
    property alias position: model.position

    property alias intValue   : model.intValue
    property alias intPosition: model.intPosition

    property alias minimum: model.minimum
    property alias maximum: model.maximum

    property alias singleStep: model.singleStep
    property alias pageStep  : model.pageStep

    property alias atMinimum: model.atMinimum
    property alias atMaximum: model.atMaximum

    //---------------------------------------------------------------------------------------------
    // Style

    property alias filterDefault: background.filter

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    width: st.scrollBar_width + border.size

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function scroll(steps)
    {
        model.scroll(steps);
    }

    //---------------------------------------------------------------------------------------------

    function scrollUp(steps)
    {
        model.scroll(-steps);
    }

    function scrollDown(steps)
    {
        model.scroll(steps);
    }

    //---------------------------------------------------------------------------------------------

    function scrollTo(value)
    {
        scrollBar.value = value;
    }

    function scrollToMinimum()
    {
        value = 0;
    }

    function scrollToMaximum()
    {
        value = scrollBar.maximum;
    }

    //---------------------------------------------------------------------------------------------

    function getHandleSize()
    {
        var size = height / (maximum + pageStep) * (handleArea.height);

        if (size < handleMinimumSize) size = handleMinimumSize;

        return size;
    }

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    ModelRange
    {
        id: model

        scale: st.ratio

        singleStep: st.scrollBar_singleStep
        pageStep  : st.scrollBar_pageStep

        handleMaximum: handleArea.height - handleSize

        onPositionChanged:
        {
            if (handle.drag.active == false)
            {
                handle.y = intPosition;
            }
        }
    }

    Timer
    {
        interval: st.scrollBar_intervalPress

        repeat: true

        running: (buttonUp.pressed || buttonDown.pressed || pPagePressed)

        onTriggered:
        {
            if (buttonUp.pressed)
            {
                model.scroll(-1);
            }
            else if (buttonDown.pressed)
            {
                model.scroll(1);
            }
            else if (handleArea.pressed)
            {
                if (handleArea.mouseY >= handle.y)
                {
                    if (handleArea.mouseY < (handle.y + handle.height))
                    {
                        pPagePressed = false;

                        stop();
                    }
                    else model.pageScroll(1);
                }
                else model.pageScroll(-1);
            }

            interval = st.scrollBar_intervalRepeat;
        }

        onRunningChanged: interval = st.scrollBar_intervalPress
    }

    MouseArea
    {
        id: handleArea

        anchors.left  : buttonUp.left
        anchors.right : buttonUp.right
        anchors.top   : buttonUp.bottom
        anchors.bottom: buttonDown.top

        anchors.topMargin   : -(buttonUp  .borderBottom)
        anchors.bottomMargin: -(buttonDown.borderTop)

        wheelEnabled: true

        onPressed:
        {
            if (mouseY < handle.y)
            {
                 model.pageScroll(-1);
            }
            else model.pageScroll(1);

            pPagePressed = true;
        }

        onReleased: pPagePressed = false

        onWheeled: if (visible) model.scroll(-steps * 3)

        Image
        {
            id: background

            anchors.fill: parent

            sourceSize: st.size16x16

            source: st.image_blank

            filter: st.scrollBar_filterHorizontalDefault
        }

        MouseArea
        {
            id: handle

            anchors.left : parent.left
            anchors.right: parent.right

            height: handleSize

            visible: (handleArea.height > handleMinimumSize)

            hoverEnabled: true

            drag.target   : handle
            drag.axis     : Drag.YAxis
            drag.threshold: 0

            drag.minimumY: 0
            drag.maximumY: handleArea.height - height

            onYChanged: if (drag.active) position = y

            Image
            {
                anchors.left  : parent.left
                anchors.right : parent.right
                anchors.top   : borderA.bottom
                anchors.bottom: borderB.top

                sourceSize: st.size16x16

                source: st.image_blank

                filter:
                {
                    if      (handle.pressed)       return filterHandlePress;
                    else if (handle.containsMouse) return filterHandleHover;
                    else                           return filterHandle;
                }
            }

            BorderHorizontal
            {
                id: borderA

                color: colorBorder
            }

            BorderHorizontal
            {
                id: borderB

                anchors.bottom: parent.bottom

                color: colorBorder
            }
        }
    }

    ButtonPianoIcon
    {
        id: buttonUp

        anchors.left : border.right
        anchors.right: parent.right

        height: st.scrollBar_buttonSize

        borderRight : 0
        borderBottom: borderSize

        icon          : st.icon16x16_scrollUp
        iconSourceSize: st.size16x16

        cursor: Qt.ArrowCursor

        onPressed: model.scroll(-1)
    }

    ButtonPianoIcon
    {
        id: buttonDown

        anchors.left  : border.right
        anchors.right : parent.right
        anchors.bottom: parent.bottom

        height: st.scrollBar_buttonSize

        borderRight: 0
        borderTop  : borderSize

        icon          : st.icon16x16_scrollDown
        iconSourceSize: st.size16x16

        cursor: Qt.ArrowCursor

        onPressed: model.scroll(1)
    }

    BorderVertical
    {
        id: border

        color: colorBorder
    }
}
