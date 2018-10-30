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

BaseLineEdit
{
    id: lineEditBox

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------
    // Style

    property color colorA: st.lineEditBox_colorA
    property color colorB: st.lineEditBox_colorB

    property color colorHoverA: st.lineEditBox_colorHoverA
    property color colorHoverB: st.lineEditBox_colorHoverB

    property color colorActive: st.lineEditBox_colorActive

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias background: background
    property alias itemFocus : itemFocus
    property alias borders   : borders

    //---------------------------------------------------------------------------------------------
    // Style

    property alias focusSize: itemFocus.size

    //---------------------------------------------------------------------------------------------
    // Settings style
    //---------------------------------------------------------------------------------------------

    colorText: (isFocused) ? st.baseLineEdit_colorText : colorDefault

    colorDefault: (enabled && isHovered) ? st.lineEditBox_colorDefaultHover
                                         : st.lineEditBox_colorDefault

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    Rectangle
    {
        id: background

        anchors.fill: parent

        z: -1

        gradient: Gradient
        {
            GradientStop
            {
                position: 0.0

                color:
                {
                    if (enabled)
                    {
                        if (isFocused)
                        {
                            return colorActive;
                        }
                        else if (isHovered)
                        {
                            return colorHoverA;
                        }
                    }

                    return colorA;
                }
            }

            GradientStop
            {
                position: 1.0

                color:
                {
                    if (enabled)
                    {
                        if (isFocused)
                        {
                            return colorActive;
                        }
                        else if (isHovered)
                        {
                            return colorHoverB;
                        }
                    }

                    return colorB;
                }
            }
        }
    }

    RectangleBorders
    {
        id: itemFocus

        anchors.fill: parent

        size: st.border_sizeFocus

        opacity: (window.isActive && isFocused)

        color: colorTextSelection

        RectangleBorders
        {
            id: borders

            anchors.fill: parent

            anchors.margins: focusSize
        }
    }
}
