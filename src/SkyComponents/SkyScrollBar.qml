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

Item
{
    id: scrollBar

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property int handleSize: getHandleSize()

//#QT_6
    property int radius: 0
//#END

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

//#QT_OLD
    // NOTE: A Rectangle gradient can only be vertical so we filter an image instead.
    property ImageFilterColor filterHandle     : st.scrollBar_filterHorizontalHandle
    property ImageFilterColor filterHandleHover: st.scrollBar_filterHorizontalHandleHover
    property ImageFilterColor filterHandlePress: st.scrollBar_filterHorizontalHandlePress
//#ELSE
    property color colorHandlePressA: st.scrollBar_colorHandlePressA
    property color colorHandlePressB: st.scrollBar_colorHandlePressB
//#END

    //---------------------------------------------------------------------------------------------
    // Private

    property bool pPagePressed: false

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias background      : background
    property alias backgroundHandle: backgroundHandle

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

//#QT_OLD
    property alias filterDefault: background.filter
//#END

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
    // Children
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

        running: pPagePressed

        onTriggered:
        {
            if (handleArea.pressed)
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

    SkyMouseArea
    {
        id: handleArea

        anchors.left  : border.right
        anchors.right : parent.right
        anchors.top   : parent.top
        anchors.bottom: parent.bottom

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

        /* QML_EVENT */ onWheeled: function(steps)
        {
            if (visible) model.scroll(-steps * 3);
        }

//#QT_OLD
        SkyImage
        {
            id: background

            anchors.fill: parent

            sourceSize: st.size16x16

            source: st.picture_blank

            filter: st.scrollBar_filterHorizontalDefault
        }
//#ELSE
        Rectangle
        {
            id: background

            anchors.fill: parent

            radius: scrollBar.radius

            gradient: Gradient
            {
                orientation: Gradient.Horizontal

                GradientStop { position: 0.0; color: colorA }
                GradientStop { position: 1.0; color: colorB }
            }
        }
//#END

        SkyMouseArea
        {
            id: handle

            anchors.left : parent.left
            anchors.right: parent.right

            height: handleSize

            visible: (handleArea.height > handleMinimumSize)

            hoverEnabled: true

            drag.target: handle
            drag.axis  : Drag.YAxis

            drag.minimumY: 0
            drag.maximumY: handleArea.height - height

//#QT_NEW
            drag.threshold: 0
//#END

            onYChanged: if (drag.active) position = y

//#QT_OLD
            SkyImage
            {
                id: backgroundHandle

                anchors.fill: parent

                sourceSize: st.size16x16

                source: st.picture_blank

                filter:
                {
                    if      (handle.pressed)     return filterHandlePress;
                    else if (handle.hoverActive) return filterHandleHover;
                    else                         return filterHandle;
                }
            }
//#ELSE
            Rectangle
            {
                id: backgroundHandle

                anchors.fill: parent

                radius: scrollBar.radius

                gradient: Gradient
                {
                    orientation: Gradient.Horizontal

                    GradientStop
                    {
                        position: 0.0

                        color:
                        {
                            if      (handle.pressed)     return colorHandlePressA;
                            else if (handle.hoverActive) return colorHandleHoverA;
                            else                         return colorHandleA;
                        }
                    }

                    GradientStop
                    {
                        position: 1.0

                        color:
                        {
                            if      (handle.pressed)     return colorHandlePressB;
                            else if (handle.hoverActive) return colorHandleHoverB;
                            else                         return colorHandleB;
                        }
                    }
                }
            }
//#END
        }

        BorderHorizontal
        {
            anchors.bottom: handle.top

//#QT_6
            visible: border.visible
//#END

            color: colorBorder
        }

        BorderHorizontal
        {
            anchors.top: handle.bottom

//#QT_6
            visible: border.visible
//#END

            color: colorBorder
        }
    }

    BorderVertical
    {
        id: border

//#QT_6
        // NOTE: A rounded handle does not want a border on its edges.
        visible: (scrollBar.radius == 0)
//#END

        color: colorBorder
    }
}
