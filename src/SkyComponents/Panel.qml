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

MouseArea
{
    id: panel

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property bool isFocused: activeFocus
    property bool isHovered: containsMouse

    property bool animate: true

    property bool enableFocus: true

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    default property alias content: content.data

    property alias contentWidth : content.width
    property alias contentHeight: content.height

    //---------------------------------------------------------------------------------------------

    property alias clip: content.clip

    property alias borderSize : borders.size
    property alias borderColor: borders.color

    property alias borderLeft  : borders.borderLeft
    property alias borderRight : borders.borderRight
    property alias borderTop   : borders.borderTop
    property alias borderBottom: borders.borderBottom

    property alias borderSizeWidth : borders.sizeWidth
    property alias borderSizeHeight: borders.sizeHeight

    property alias backgroundOpacity: background.opacity

    //---------------------------------------------------------------------------------------------

    property alias background: background

    property alias borders: borders

    property alias itemContent: content

    //---------------------------------------------------------------------------------------------
    // Style

    property alias color   : background.color
    property alias gradient: background.gradient

    property alias colorBorder: borders.color

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    acceptedButtons: Qt.LeftButton | Qt.RightButton | Qt.MiddleButton

    hoverEnabled: true
    hoverRetain : hoverEnabled

    wheelEnabled: hoverEnabled

    cursor: Qt.ArrowCursor

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    onPressed:
    {
        if (enableFocus && (mouse.button & Qt.LeftButton))
        {
            forceActiveFocus();
        }
    }

    onVisibleChanged: if (visible == false) window.clearFocusItem(panel)

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function focus()
    {
        forceActiveFocus();
    }

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    Rectangle
    {
        id: background

        anchors.fill: parent

        color: st.panel_color

        Behavior on opacity
        {
            PropertyAnimation
            {
                duration: (animate) ? st.duration_normal : 0
            }
        }
    }

    Item
    {
        id: content

        anchors.fill: parent

        anchors.leftMargin  : borderLeft
        anchors.rightMargin : borderRight
        anchors.topMargin   : borderTop
        anchors.bottomMargin: borderBottom
    }

    RectangleBorders
    {
        id: borders

        anchors.fill: parent

        color: st.border_color
    }
}
