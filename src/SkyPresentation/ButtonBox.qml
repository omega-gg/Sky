//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkyPresentation.

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
    id: buttonBox

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------
    // Style

    property color color     : sp.buttonBox_color
    property color colorHover: sp.buttonBox_colorHover

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias text: itemText.text

    //---------------------------------------------------------------------------------------------

    property alias background: background
    property alias borders   : borders

    property alias itemText: itemText

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    width : sp.buttonBox_size
    height: width

    cursor: Qt.PointingHandCursor

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

    Rectangle
    {
        id: background

        anchors.fill: parent

        anchors.margins: borders.size

        opacity: sp.buttonBox_opacity

        color: (isHovered) ? colorHover
                           : buttonBox.color
    }

    RectangleBorders
    {
        id: borders

        anchors.fill: parent

        color: background.color
    }

    TextBase
    {
        id: itemText

        anchors.fill: parent

        horizontalAlignment: Text.AlignHCenter
        verticalAlignment  : Text.AlignVCenter

        font.pixelSize: sp.buttonBox_pixelSize
    }
}
