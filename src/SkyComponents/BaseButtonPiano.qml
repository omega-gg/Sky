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

BaseButton
{
    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property variant itemLeft  : null
    property variant itemRight : null
    property variant itemTop   : null
    property variant itemBottom: null

    //---------------------------------------------------------------------------------------------
    // Style

    property color colorA: (highlighted) ? st.buttonPiano_colorHighlightA
                                         : st.buttonPiano_colorA

    property color colorB: (highlighted) ? st.buttonPiano_colorHighlightB
                                         : st.buttonPiano_colorB

    property color colorHoverA: (highlighted) ? st.buttonPiano_colorHighlightHoverA
                                              : st.buttonPiano_colorHoverA

    property color colorHoverB: (highlighted) ? st.buttonPiano_colorHighlightHoverB
                                              : st.buttonPiano_colorHoverB

    property color colorPressA: (pHighlighted) ? st.buttonPiano_colorCheckA
                                               : st.buttonPiano_colorPressA

    property color colorPressB: (pHighlighted) ? st.buttonPiano_colorCheckB
                                               : st.buttonPiano_colorPressB

    property color colorPressHoverA: (pHighlighted) ? st.buttonPiano_colorCheckHoverA
                                                    : st.buttonPiano_colorPressHoverA

    property color colorPressHoverB: (pHighlighted) ? st.buttonPiano_colorCheckHoverB
                                                    : st.buttonPiano_colorPressHoverB

    //---------------------------------------------------------------------------------------------
    // Private

    property bool pHighlighted: (highlighted || checkable)

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    default property alias content: background.data

    property alias borderSize : borders.size
    property alias borderColor: borders.color

    property alias borderLeft  : borders.borderLeft
    property alias borderRight : borders.borderRight
    property alias borderTop   : borders.borderTop
    property alias borderBottom: borders.borderBottom

    property alias borderSizeWidth : borders.sizeWidth
    property alias borderSizeHeight: borders.sizeHeight

    //---------------------------------------------------------------------------------------------

    property alias background: background
    property alias itemFocus : itemFocus
    property alias borders   : borders

    //---------------------------------------------------------------------------------------------
    // Style

    property alias colorFocus: itemFocus.color

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    cursor: Qt.PointingHandCursor

    //---------------------------------------------------------------------------------------------
    // Keys
    //---------------------------------------------------------------------------------------------

    QML_EVENT Keys.onPressed: function(event)
    {
        if (event.key == Qt.Key_Left)
        {
            event.accepted = true;

            if (itemLeft) itemLeft.focus();
        }
        else if (event.key == Qt.Key_Right)
        {
            event.accepted = true;

            if (itemRight) itemRight.focus();
        }
        else if (event.key == Qt.Key_Up)
        {
            event.accepted = true;

            if (itemTop) itemTop.focus();
        }
        else if (event.key == Qt.Key_Down)
        {
            event.accepted = true;

            if (itemBottom) itemBottom.focus();
        }
    }

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    Rectangle
    {
        id: background

        anchors.fill: parent

        anchors.leftMargin  : borderLeft
        anchors.rightMargin : borderRight
        anchors.topMargin   : borderTop
        anchors.bottomMargin: borderBottom

        gradient: Gradient
        {
            GradientStop
            {
                position: 0.0

                color:
                {
                    if (isPressed)
                    {
                        return colorPressA;
                    }
                    else if (checked)
                    {
                        if (checkHover && isHovered)
                        {
                             return colorPressHoverA;
                        }
                        else return colorPressA;
                    }
                    else if (isHovered)
                    {
                         return colorHoverA;
                    }
                    else return colorA;
                }
            }

            GradientStop
            {
                position: 1.0

                color:
                {
                    if (isPressed)
                    {
                        return colorPressB;
                    }
                    else if (checked)
                    {
                        if (checkHover && isHovered)
                        {
                             return colorPressHoverB;
                        }
                        else return colorPressB;
                    }
                    else if (isHovered)
                    {
                         return colorHoverB;
                    }
                    else return colorB;
                }
            }
        }
    }

    RectangleBorders
    {
        id: itemFocus

        anchors.fill: background

        size: st.border_sizeFocus

        opacity: (window.isActive && isFocused)

        color: st.button_colorFocus
    }

    RectangleBorders
    {
        id: borders

        anchors.fill: parent

        borderLeft  : 0
        borderTop   : 0
        borderBottom: 0
    }
}
