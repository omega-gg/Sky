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

BaseLineEdit
{
    //---------------------------------------------------------------------------------------------
    // Properties style
    //---------------------------------------------------------------------------------------------

    property variant borderBackground: st.lineEdit_borderBackground

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias background : background
    property alias imageBorder: imageBorder
    property alias imageFocus : imageFocus

    //---------------------------------------------------------------------------------------------
    // Style

    property alias sourceBackground: background .source
    property alias sourceBorder    : imageBorder.source

    property alias filterDefault: background .filter
    property alias filterBorder : imageBorder.filter
    property alias filterFocus  : imageFocus .filter

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    width : st.lineEdit_width
    height: st.lineEdit_height

    padding: st.lineEdit_padding

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    BorderImageScale
    {
        id: imageFocus

        anchors.fill: parent

        anchors.margins: -st.lineEdit_marginFocus

        z: -1

        opacity: (window.isActive && isFocused)

        source: st.lineEdit_sourceBorder

        border
        {
            left : borderBackground.x;     top   : borderBackground.y
            right: borderBackground.width; bottom: borderBackground.height
        }

        filter: st.lineEdit_filterFocus

        scaleDelay: 0

        Behavior on opacity
        {
            PropertyAnimation { duration: st.duration_fast }
        }
    }

    BorderImageScale
    {
        id: background

        anchors.fill: parent

        z: -1

        source: st.lineEdit_sourceDefault

        border
        {
            left : borderBackground.x;     top   : borderBackground.y
            right: borderBackground.width; bottom: borderBackground.height
        }

        filter: (isFocused || isHovered) ? st.lineEdit_filterHover
                                         : st.lineEdit_filterDefault
    }

    BorderImageScale
    {
        id: imageBorder

        anchors.fill: parent

        source: st.lineEdit_sourceBorder

        border
        {
            left : borderBackground.x;     top   : borderBackground.y
            right: borderBackground.width; bottom: borderBackground.height
        }

        filter: st.lineEdit_filterBorder

        scaleDelay: 0
    }
}
