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

BaseEdit
{
    id: editBox

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property bool enableClearFocus: true

    //---------------------------------------------------------------------------------------------
    // Style

    property real focusSize: st.editBox_focusSize

    property color colorA: st.editBox_colorA
    property color colorB: st.editBox_colorB

    property color colorFocusA: st.editBox_colorFocusA
    property color colorFocusB: st.editBox_colorFocusB

    property color colorFocusHoverA: st.editBox_colorFocusHoverA
    property color colorFocusHoverB: st.editBox_colorFocusHoverB

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias background: background
    property alias shadow    : shadow
    property alias itemFocus : itemFocus

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    textInput.font.bold: true

    //---------------------------------------------------------------------------------------------
    // Style

    colorDefault: st.editBox_colorDefault

    //---------------------------------------------------------------------------------------------
    // Functions events
    //---------------------------------------------------------------------------------------------

    function onClear()
    {
        if (enableClearFocus)
        {
            text = "";

            window.clearFocus();
        }
        else if (text)
        {
            text = "";
        }
        else window.clearFocus();
    }

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    Rectangle
    {
        id: itemFocus

        anchors.fill: parent

        z: -1

        visible: (window.isActive && isFocused)

        gradient: Gradient
        {
            GradientStop
            {
                position: 0.0

                color: (isHovered) ? colorFocusHoverA
                                   : colorFocusA
            }

            GradientStop
            {
                position: 1.0

                color: (isHovered) ? colorFocusHoverB
                                   : colorFocusB
            }
        }
    }

    Item
    {
        anchors.fill: parent

        anchors.margins: focusSize

        z: -1

        clip: itemFocus.visible

        Rectangle
        {
            id: background

            anchors.fill: parent

            anchors.margins: -focusSize

            gradient: Gradient
            {
                GradientStop { position: 0.0; color: colorA }
                GradientStop { position: 1.0; color: colorB }
            }
        }
    }

    RectangleShadow
    {
        id: shadow

        anchors.left : parent.left
        anchors.right: parent.right

        height: st.editBox_shadowHeight

        visible: (itemFocus.visible == false)

        opacity: (isHovered) ? st.baseEdit_shadowOpacityB
                             : st.baseEdit_shadowOpacityA

        direction: Sk.Down
    }

    RectangleBorders
    {
        anchors.fill: parent

        anchors.margins: focusSize

        visible: itemFocus.visible

        color: colorDefault
    }

    ButtonPianoIcon
    {
        id: button

        anchors.right: parent.right
        anchors.top  : parent.top

        anchors.rightMargin: focusSize
        anchors.topMargin  : focusSize

        width : st.lineEditClear_buttonSize + borderSizeWidth
        height: st.lineEditClear_buttonSize + borderSizeHeight

        borderLeft  : borderSize
        borderTop   : borderSize
        borderBottom: borderSize

        visible: editBox.isFocused

        icon          : st.icon16x16_close
        iconSourceSize: st.size16x16

        onClicked: onClear()
    }
}
