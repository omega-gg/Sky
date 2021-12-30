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
    // Settings
    //---------------------------------------------------------------------------------------------

//#QT_NEW
    // FIXME Qt5: The cursor gets cropped on the right if we keep the TextInput clip.
    clip: true

    textInput.clip: false
//#END

    //---------------------------------------------------------------------------------------------
    // Style

    colorText: (isFocused) ? st.baseLineEdit_colorText : colorDefault

    colorDefault: (enabled && isHovered) ? st.lineEditBox_colorDefaultHover
                                         : st.lineEditBox_colorDefault

    //---------------------------------------------------------------------------------------------
    // Children
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
