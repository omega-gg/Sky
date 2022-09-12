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

MouseArea
{
    id: buttonCheck

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property int margins: height / 5

    property int radius: background.height

    property int borderSize: st.border_size

    property bool checked: false

    //---------------------------------------------------------------------------------------------
    // Style

    property color colorA: st.buttonCheck_colorA
    property color colorB: st.buttonCheck_colorB

    property color colorActiveA: st.buttonCheck_colorActiveA
    property color colorActiveB: st.buttonCheck_colorActiveB

    property color colorHandleA: st.buttonCheck_colorHandleA
    property color colorHandleB: st.buttonCheck_colorHandleB

    property color colorHandleHoverA: st.buttonCheck_colorHandleHoverA
    property color colorHandleHoverB: st.buttonCheck_colorHandleHoverB

    property color colorHandlePressA: st.buttonCheck_colorHandlePressA
    property color colorHandlePressB: st.buttonCheck_colorHandlePressB

    //---------------------------------------------------------------------------------------------
    // Private

    property bool pUpdate: true

    //---------------------------------------------------------------------------------------------
    // Signals
    //---------------------------------------------------------------------------------------------

    signal checkClicked

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    width : st.buttonCheck_width
    height: st.buttonCheck_height

    hoverEnabled: enabled

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    onClicked: pToggleChecked()

    onWidthChanged: pUpdatePosition()

    onCheckedChanged: if (pUpdate) pUpdatePosition()

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------
    // Private

    function pToggleChecked()
    {
        checked = !checked;

        checkClicked();
    }

    function pApplyX(x)
    {
        var from = handle.x;
        var to;

        pUpdate = false;

        if (x < width / 2)
        {
            to = 0;

            checked = false;
        }
        else
        {
            to = width - handle.width;

            checked = true;
        }

        pUpdate = true;

        if (from == to) return;

        animation.from = from;
        animation.to   = to;

        animation.restart();
    }

    function pUpdatePosition()
    {
        animation.stop();

        if (checked) handle.x = width - handle.width;
        else         handle.x = 0;
    }

    //---------------------------------------------------------------------------------------------
    // Animations
    //---------------------------------------------------------------------------------------------

    NumberAnimation
    {
        id: animation

        target: handle

        property: "x"

        duration: st.duration_faster

        easing.type: st.easing
    }

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

    Rectangle
    {
        id: background

        anchors.fill: parent

        anchors.margins: margins

        radius: buttonCheck.radius

        gradient: Gradient
        {
            GradientStop
            {
                position: 0.0

                color: (checked) ? colorActiveA : colorA
            }

            GradientStop
            {
                position: 1.0

                color: (checked) ? colorActiveB : colorB
            }
        }

//#QT_4
        smooth: true
//#END

        border.width: borderSize
        border.color: st.border_color
    }

    MouseArea
    {
        id: handle

        width : parent.height
        height: width

        hoverEnabled: parent.enabled

        drag.target: handle
        drag.axis  : Drag.XAxis

        drag.minimumX: 0
        drag.maximumX: parent.width - width

        onClicked: pToggleChecked()

        // NOTE: We update the position when the drag has ended.
        drag.onActiveChanged: if (drag.active == false) pApplyX(x + width / 2)

        Rectangle
        {
            anchors.fill: parent

            anchors.margins: margins

            radius: buttonCheck.radius

            gradient: Gradient
            {
                GradientStop
                {
                    position: 0.0

                    color:
                    {
                        if (buttonCheck.pressed)
                        {
                            return colorHandlePressA;
                        }
                        else if (handle.hoverActive)
                        {
                            return colorHandleHoverA;
                        }
                        else return colorHandleA;
                    }
                }

                GradientStop
                {
                    position: 1.0

                    color:
                    {
                        if (buttonCheck.pressed)
                        {
                            return colorHandlePressB;
                        }
                        else if (handle.hoverActive)
                        {
                            return colorHandleHoverB;
                        }
                        else return colorHandleB;
                    }
                }
            }

//#QT_4
            smooth: true
//#END

            border.width: borderSize
            border.color: st.border_color
        }
    }
}
