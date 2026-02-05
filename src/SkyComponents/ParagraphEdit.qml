//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkyComponents.

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

    property int borderSize     : st.border_size
    property int borderSizeFocus: st.border_sizeFocus

    //---------------------------------------------------------------------------------------------
    // Style

    property color color     : st.lineEdit_color
    property color colorHover: st.lineEdit_colorHover

    //---------------------------------------------------------------------------------------------
    // Private

    property bool pAtBottom: true

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias isFocused: textEdit.isFocused
    property alias isHovered: textEdit.isHovered

    property alias radius: background.radius

    property alias text: textEdit.text

    //---------------------------------------------------------------------------------------------

    property alias padding: textEdit.padding

    property alias background: background
    property alias itemFocus : itemFocus

    property alias scrollArea: scrollArea
    property alias textEdit  : textEdit

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function setFocus()
    {
        textEdit.setFocus();
    }

    //---------------------------------------------------------------------------------------------

    function moveCursorAt(index)
    {
        textEdit.moveCursorAt(index);
    }

    function moveCursorAtStart()
    {
        textEdit.moveCursorAtStart();
    }

    function moveCursorAtEnd()
    {
        textEdit.moveCursorAtEnd();
    }

    //---------------------------------------------------------------------------------------------

    function select(start, end)
    {
        textEdit.select(start, end);
    }

    function selectAll()
    {
        textEdit.selectAll();
    }

    function deselect()
    {
        textEdit.deselect();
    }

    //---------------------------------------------------------------------------------------------

    function clear()
    {
        textEdit.clear();
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
        id: itemFocus

        anchors.fill: background

//#QT_4
        anchors.margins: -borderSize
//#ELSE
        anchors.margins: -borderSizeFocus
//#END

        z: (radius) ? 1 : -1

        radius: background.radius

        opacity: (window.isActive && isFocused)

        color: "transparent"

//#QT_4
        smooth: true
//#END

        border.width: borderSize + borderSizeFocus
        border.color: textEdit.colorTextSelection
    }

    Rectangle
    {
        id: background

        anchors.fill: parent

        z: -1

        color: (paragraphEdit.enabled && (isFocused || isHovered)) ? colorHover
                                                                   : paragraphEdit.color

//#QT_4
        smooth: true
//#END

        border.width: borderSize
        border.color: st.border_color
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

        onContentYChanged: pAtBottom = scrollBar.atMaximum

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
