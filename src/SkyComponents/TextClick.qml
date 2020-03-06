//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors united with omega. <http://omega.gg/about>

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

TextBase
{
    id: textClick

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property bool isFocused: activeFocus

    property bool isHovered: mouseArea.containsMouse
    property bool isPressed: mouseArea.pressed

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias mouseArea: mouseArea

    //---------------------------------------------------------------------------------------------
    // Signals
    //---------------------------------------------------------------------------------------------

    signal entered
    signal exited

    signal pressed(variant mouse)
    signal clicked(variant mouse)

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    MouseArea
    {
        id: mouseArea

        anchors.right: (horizontalAlignment == Text.AlignRight) ? parent.right : undefined

        anchors.bottom: (verticalAlignment == Text.AlignBottom) ? parent.bottom : undefined

        anchors.horizontalCenter: (horizontalAlignment
                                   ==
                                   Text.AlignHCenter) ? parent.horizontalCenter : undefined

        anchors.verticalCenter: (verticalAlignment
                                 ==
                                 Text.AlignVCenter) ? parent.verticalCenter : undefined

        width: Math.min(sk.textWidth(font, text), parent.width)

        height: Math.min(sk.textHeight(font), parent.height)

        hoverEnabled: textClick.enabled

        cursor: Qt.PointingHandCursor

        onEntered: textClick.entered()
        onExited : textClick.exited ()

        onPressed: textClick.pressed(mouse)
        onClicked: textClick.clicked(mouse)
    }
}
