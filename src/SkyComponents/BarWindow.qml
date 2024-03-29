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

ViewDrag
{
    id: barWindow

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------
    // Style

    property color colorA: st.barWindow_colorA
    property color colorB: st.barWindow_colorB

    property color colorDisableA: st.barWindow_colorDisableA
    property color colorDisableB: st.barWindow_colorDisableB

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias borderSize: border.size

    property alias buttonApplicationMaximum: buttonApplication.maximumWidth

    //---------------------------------------------------------------------------------------------

    property alias buttonApplication: buttonApplication

    property alias buttonIconify : buttonIconify
    property alias buttonMaximize: buttonMaximize
    property alias buttonClose   : buttonClose

    property alias border    : border
    property alias borderLine: borderLine

    //---------------------------------------------------------------------------------------------
    // Style

    property alias colorBorder: border.color

    //---------------------------------------------------------------------------------------------
    // Signals
    //---------------------------------------------------------------------------------------------

    signal buttonPressed

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    height: st.barWindow_height + borderSize

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------
    // Private

    function pIconify()
    {
        window.minimized = true;
    }

    function pMaximize()
    {
        window.maximized = !(window.maximized);
    }

    function pClose()
    {
        window.close();
    }

    //---------------------------------------------------------------------------------------------

    function pDoubleClicked(mouse)
    {
        pMaximize();
    }

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

    onPressed: window.clearFocus()

    /* QML_EVENT */ onDoubleClicked: function(mouse)
    {
        if (window.hoverCount() == 0)
        {
            pDoubleClicked(mouse);
        }
    }

    Rectangle
    {
        anchors.left  : parent.left
        anchors.right : parent.right
        anchors.top   : parent.top
        anchors.bottom: border.top

        gradient: Gradient
        {
            GradientStop
            {
                position: 0.0

                color: (window.isActive) ? colorA
                                         : colorDisableA
            }

            GradientStop
            {
                position: 1.0

                color: (window.isActive) ? colorB
                                         : colorDisableB
            }
        }

        BorderHorizontal
        {
            id: borderLine

            color: st.barWindow_colorBorderLine

            visible: (colorA != colorB)
        }
    }

    ButtonPianoFull
    {
        id: buttonApplication

        anchors.top   : parent.top
        anchors.bottom: border.top

        maximumWidth: buttonIconify.x - st.dp32

        spacing: st.dp6

        checkable: true

        icon: st.icon

        iconSourceSize: Qt.size(height, height)

        enableFilter: false

        text: sk.name

        font.pixelSize: st.dp14

        onPressed: buttonPressed()
    }

    ButtonPianoWindow
    {
        id: buttonIconify

        anchors.right : (buttonMaximize.visible) ? buttonMaximize.left
                                                 : buttonClose   .left

        anchors.top   : buttonClose.top
        anchors.bottom: buttonClose.bottom

        borderLeft: borderSize

        icon          : st.icon12x12_iconify
        iconSourceSize: st.size12x12

        onClicked: pIconify()
    }

    ButtonPianoWindow
    {
        id: buttonMaximize

        anchors.right : buttonClose.left
        anchors.top   : buttonClose.top
        anchors.bottom: buttonClose.bottom

        highlighted: window.maximized

        icon: (window.maximized) ? st.icon12x12_minimize
                                 : st.icon12x12_maximize

        iconSourceSize: st.size16x16

        onClicked: pMaximize()
    }

    ButtonPianoWindow
    {
        id: buttonClose

        anchors.right : parent.right
        anchors.top   : parent.top
        anchors.bottom: border.top

        borderRight: 0

        icon          : st.icon12x12_close
        iconSourceSize: st.size12x12

        colorHoverA: st.button_colorConfirmHoverA
        colorHoverB: st.button_colorConfirmHoverB

        colorPressA: st.button_colorConfirmPressA
        colorPressB: st.button_colorConfirmPressB

        filterIcon: (isHovered || isPressed) ? st.button_filterIconB
                                             : st.button_filterIconA

        onClicked: pClose()
    }

    BorderHorizontal
    {
        id: border

        anchors.bottom: parent.bottom

        color: st.border_color
    }
}
