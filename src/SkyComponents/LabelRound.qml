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

BaseLabelRound
{
    id: labelRound

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property real padding: st.labelRound_padding

    property real paddingLeft : padding
    property real paddingRight: padding

    property real minimumWidth: -1
    property real maximumWidth: -1

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias text: itemText.text
    property alias font: itemText.font

    //---------------------------------------------------------------------------------------------

    property alias itemText: itemText

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    width: getPreferredWidth()

    height: st.labelRound_height

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function getPreferredWidth()
    {
        var size = sk.textWidth(font, text) + paddingLeft + paddingRight;

        if (minimumWidth != -1)
        {
            size = Math.max(minimumWidth, size);
        }

        if (maximumWidth != -1)
        {
            size = Math.min(size, maximumWidth);
        }

        return size;
    }

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    TextBase
    {
        id: itemText

        anchors.fill: parent

        leftMargin : paddingLeft
        rightMargin: paddingRight

        horizontalAlignment: Text.AlignHCenter
        verticalAlignment  : Text.AlignVCenter

        style: (labelRound.enabled) ? Text.Raised
                                    : Text.Sunken

        Behavior on opacity
        {
            PropertyAnimation { duration: st.duration_fast }
        }
    }
}
