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

import QtQuick 1.0
import Sky     1.0

ViewDrag
{
    id: barWindow

    //---------------------------------------------------------------------------------------------
    // Properties style
    //---------------------------------------------------------------------------------------------

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

    visible: (window.fullScreen == false)

    //---------------------------------------------------------------------------------------------
    // Functions private
    //---------------------------------------------------------------------------------------------

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
    // Childs
    //---------------------------------------------------------------------------------------------

    onPressed: window.clearFocus()

    onDoubleClicked:
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

    ButtonPianoIcon
    {
        id: buttonIconify

        anchors.right : (buttonMaximize.visible) ? buttonMaximize.left
                                                 : buttonClose   .left

        anchors.top   : buttonClose.top
        anchors.bottom: buttonClose.bottom

        borderLeft: borderSize

        icon          : st.icon16x16_iconify
        iconSourceSize: st.size16x16

        onClicked: pIconify()
    }

    ButtonPianoIcon
    {
        id: buttonMaximize

        anchors.right : buttonClose.left
        anchors.top   : buttonClose.top
        anchors.bottom: buttonClose.bottom

        highlighted: window.maximized

        icon: (window.maximized) ? st.icon16x16_minimize
                                 : st.icon16x16_maximize

        iconSourceSize: st.size16x16

        onClicked: pMaximize()
    }

    ButtonPianoIcon
    {
        id: buttonClose

        anchors.right : parent.right
        anchors.top   : parent.top
        anchors.bottom: border.top

        anchors.rightMargin: st.dp16

        icon          : st.icon16x16_close
        iconSourceSize: st.size16x16

        colorHoverA: st.buttonPiano_colorHoverA
        colorHoverB: st.buttonPiano_colorHoverB

        colorPressA: st.buttonPiano_colorPressA
        colorPressB: st.buttonPiano_colorPressB

        colorFocus: st.buttonPiano_colorFocus

        onClicked: pClose()
    }

    BorderHorizontal
    {
        id: border

        anchors.bottom: parent.bottom

        color: st.border_color
    }
}
