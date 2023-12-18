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

Item
{
    id: paragraphEdit

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------
    // Style

    property real opacityBackground: st.lineEdit_opacity

    //---------------------------------------------------------------------------------------------
    // Private

    property bool pAtBottom: true

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property bool isFocused: textEdit.isFocused
    property bool isHovered: textEdit.isHovered

    //---------------------------------------------------------------------------------------------

    property alias background: background

    property alias scrollArea: scrollArea
    property alias textEdit  : textEdit

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function setFocus()
    {
        textEdit.forceActiveFocus();
    }

    //---------------------------------------------------------------------------------------------
    // Private

    function pScrollToBottom()
    {
        if (pAtBottom) scrollArea.scrollToBottom();
    }

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

    Rectangle
    {
        id: background

        anchors.fill: parent

        radius: st.radius

        opacity: (isFocused || isHovered) ? st.lineEdit_opacityHover
                                          : opacityBackground

        color: st.lineEdit_color

//#QT_4
        smooth: true
//#END
    }

    ScrollArea
    {
        id: scrollArea

        anchors.fill: parent

//#QT_4
        contentHeight: textEdit.height
//#ELSE
        contentHeight: textEdit.height
//#END

        onHeightChanged: pScrollToBottom()

        onContentHeightChanged: pScrollToBottom()

        onContentYChanged: pAtBottom = flickable.atYEnd

        BaseTextEdit
        {
            id: textEdit

            anchors.left : parent.left
            anchors.right: parent.right

//#QT_4
            height: Math.max(paragraphEdit.height, paintedHeight)
//#ELSE
            height: Math.max(paragraphEdit.height, contentHeight)
//#END
        }
    }
}
