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
    id: baseButtonPush

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property int margins: height / 8

//#QT_4
    property int radius: (height - margins * 2) / 3.75
//#ELSE
    property int radius: background.height / 3.75
//#END

    property int borderSize     : st.border_size
    property int borderSizeFocus: st.border_sizeFocus

    //---------------------------------------------------------------------------------------------
    // Style

    property color colorA: (highlighted) ? st.buttonPush_colorHighlightA
                                         : st.buttonPush_colorA

    property color colorB: (highlighted) ? st.buttonPush_colorHighlightB
                                         : st.buttonPush_colorB

    property color colorHoverA: (highlighted) ? st.buttonPush_colorHighlightHoverA
                                              : st.buttonPush_colorHoverA

    property color colorHoverB: (highlighted) ? st.buttonPush_colorHighlightHoverB
                                              : st.buttonPush_colorHoverB

    property color colorPressA: (pHighlighted) ? st.buttonPush_colorCheckA
                                               : st.buttonPush_colorPressA

    property color colorPressB: (pHighlighted) ? st.buttonPush_colorCheckB
                                               : st.buttonPush_colorPressB

    property color colorPressHoverA: (pHighlighted) ? st.buttonPush_colorCheckHoverA
                                                    : st.buttonPush_colorPressHoverA

    property color colorPressHoverB: (pHighlighted) ? st.buttonPush_colorCheckHoverB
                                                    : st.buttonPush_colorPressHoverB

    //---------------------------------------------------------------------------------------------
    // Private

    property bool pHighlighted: (highlighted || checkable)

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias background: background
    property alias itemFocus : itemFocus

    //---------------------------------------------------------------------------------------------
    // Style

    property alias colorFocus: itemFocus.color

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    cursor: Qt.ArrowCursor

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    Rectangle
    {
        id: itemFocus

        anchors.fill: background

//#QT_4
        anchors.margins: -borderSize
//#ELSE
        anchors.margins: -borderSizeFocus
//#END

        radius: Math.round(baseButtonPush.radius * (height / background.height))

        opacity: (window.isActive && isFocused)

        color: "transparent"

//#QT_4
        smooth: true
//#END

        border.width: borderSize + borderSizeFocus
        border.color: st.button_colorFocus
    }

    Rectangle
    {
        id: background

        anchors.fill: parent

        anchors.margins: margins

        radius: baseButtonPush.radius

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
                        if (isHovered && checkHover)
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
                        if (isHovered && checkHover)
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

//#QT_4
        smooth: true
//#END

        border.width: borderSize
        border.color: st.border_color
    }
}
