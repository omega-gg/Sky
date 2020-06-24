//=================================================================================================
/*
    Copyright (C) 2015-2020 HelloSky authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

import QtQuick 1.0
import Sky     1.0

BaseButtonTouch
{
    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property int padding: st.buttonTouch_padding

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

    width: sk.textWidth(itemText.font, text) + padding * 2

    height: st.buttonTouch_size

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    TextBase
    {
        id: itemText

        anchors.fill: parent

        anchors.leftMargin : label.padding
        anchors.rightMargin: label.padding

        horizontalAlignment: Text.AlignHCenter
        verticalAlignment  : Text.AlignVCenter
    }
}
