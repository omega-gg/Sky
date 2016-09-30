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

Item
{
    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias itemText: itemText

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    anchors.left : parent.left
    anchors.right: parent.right

    height: st.contextualCategory_height

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    Rectangle
    {
        anchors.left  : parent.left
        anchors.right : parent.right
        anchors.top   : parent.top
        anchors.bottom: border.top

        gradient: Gradient
        {
            GradientStop { position: 0.0; color: st.contextualCategory_colorA }
            GradientStop { position: 1.0; color: st.contextualCategory_colorB }
        }

        BorderHorizontal
        {
            id: borderLine

            color: st.contextualCategory_colorBorderLine
        }

        TextBase
        {
            id: itemText

            anchors.fill: parent

            leftMargin : st.contextualCategory_padding
            rightMargin: st.contextualCategory_padding

            verticalAlignment: Text.AlignVCenter

            text: title

            style: Text.Sunken

            font.pixelSize: st.contextualCategory_pixelSize
        }
    }

    BorderHorizontal
    {
        id: border

        anchors.bottom: parent.bottom
    }
}
