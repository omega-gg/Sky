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
    id: lineEdit

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property int margins: height / 8

//#QT_4
    property int radius: (height - margins * 2) / 5
//#ELSE
    property int radius: background.height / 5
//#END

    property int borderSize     : st.border_size
    property int borderSizeFocus: st.border_sizeFocus

    //---------------------------------------------------------------------------------------------
    // Style

    property color color     : st.lineEdit_color
    property color colorHover: st.lineEdit_colorHover

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias background: background
    property alias itemFocus : itemFocus

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    width : st.lineEdit_width
    height: st.lineEdit_height

    padding: st.lineEdit_padding

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    Rectangle
    {
        id: itemFocus

        anchors.fill: background

        anchors.margins: -borderSizeFocus

        radius: lineEdit.radius

        z: -1

        opacity: (window.isActive && isFocused)

        color: "transparent"

//#QT_4
        smooth: true
//#END

        border.width: borderSize + borderSizeFocus
        border.color: st.colorTextSelection
    }

    Rectangle
    {
        id: background

        anchors.fill: parent

        anchors.margins: margins

        radius: lineEdit.radius

        z: -1

        color: (lineEdit.enabled && (isFocused || isHovered)) ? colorHover
                                                              : lineEdit.color

//#QT_4
        smooth: true
//#END

        border.width: borderSize
        border.color: st.border_color
    }
}
