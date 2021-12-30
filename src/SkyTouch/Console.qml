//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkyTouch.

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

ScrollArea
{
    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------
    // Private

    property bool pAtBottom: true

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias log: itemConsole.log

    //---------------------------------------------------------------------------------------------

    property alias itemConsole: itemConsole

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

//#QT_4
    contentHeight: itemConsole.paintedHeight
//#ELSE
    contentHeight: itemConsole.contentHeight
//#END

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    onHeightChanged: pScrollToBottom()

    onContentHeightChanged: pScrollToBottom()

    onContentYChanged: pAtBottom = flickable.atYEnd

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function append(string)
    {
        itemConsole.append(string);
    }

    //---------------------------------------------------------------------------------------------
    // Private

    function pScrollToBottom()
    {
        if (pAtBottom) scrollToBottom();
    }

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

    BaseConsole
    {
        id: itemConsole

        anchors.left : parent.left
        anchors.right: parent.right
    }
}
