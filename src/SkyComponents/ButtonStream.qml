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

BaseButton
{
    id: buttonStream

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property int padding: 0

    property int paddingLeft : padding
    property int paddingRight: padding

    //---------------------------------------------------------------------------------------------
    // Style

    property variant borderBackground: st.buttonStream_borderBackground

    property ImageColorFilter filterDefault: st.buttonStream_filterDefault
    property ImageColorFilter filterHover  : st.buttonStream_filterHover
    property ImageColorFilter filterPress  : st.buttonStream_filterPress
    property ImageColorFilter filterDisable: st.buttonStream_filterDisable

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias text: itemText.text
    property alias font: itemText.font

    //---------------------------------------------------------------------------------------------

    property alias itemText: itemText

    //---------------------------------------------------------------------------------------------
    // Style

    property alias filterBorder: imageBorder.filter

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    width : st.buttonStream_width
    height: st.buttonStream_height

    cursor: MouseArea.ArrowCursor

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    BorderImageScale
    {
        id: background

        anchors.fill: parent

        source: st.buttonStream_sourceBackground

        border
        {
            left : borderBackground.x;     top   : borderBackground.y
            right: borderBackground.width; bottom: borderBackground.height
        }

        filter:
        {
            if (isPressed)
            {
                return filterPress;
            }
            else if (isHovered)
            {
                return filterHover;
            }
            else if (buttonStream.enabled)
            {
                return filterDefault;
            }
            else return filterDisable;
        }
    }

    BorderImageScale
    {
        id: imageBorder

        anchors.fill: parent

        source: st.buttonStream_sourceBorder

        border
        {
            left : borderBackground.x;     top   : borderBackground.y
            right: borderBackground.width; bottom: borderBackground.height
        }

        filter: st.buttonStream_filterBorder
    }

    TextBase
    {
        id: itemText

        anchors.fill: parent

        leftMargin : paddingLeft
        rightMargin: paddingRight

        horizontalAlignment: Text.AlignHCenter
        verticalAlignment  : Text.AlignVCenter

        style: Text.Raised

        font.pixelSize: st.dp11
    }
}
