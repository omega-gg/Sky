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
    id: lineEditLabel

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias labelWidth: label.width

    property alias text: label.text

    property alias editText       : lineEdit.text
    property alias editTextDefault: lineEdit.text

    property alias selectedText: lineEdit.selectedText

    property alias cursorPosition: lineEdit.cursorPosition

    property alias selectionStart: lineEdit.selectionStart
    property alias selectionEnd  : lineEdit.selectionEnd

    property alias maximumLength: lineEdit.maximumLength

    property alias echoMode: lineEdit.echoMode

    //---------------------------------------------------------------------------------------------

    property alias label: label

    property alias lineEdit : lineEdit
    property alias textInput: lineEdit.textInput

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    width : st.lineEdit_width
    height: st.lineEdit_height

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    onActiveFocusChanged: if (activeFocus) lineEdit.focus()

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function focus()
    {
        lineEdit.focus();
    }

    //---------------------------------------------------------------------------------------------

    function moveCursorAt(index)
    {
        lineEdit.moveCursorAt(index);
    }

    function moveCursorAtStart()
    {
        lineEdit.moveCursorAtStart();
    }

    function moveCursorAtEnd()
    {
        lineEdit.moveCursorAtEnd();
    }

    //---------------------------------------------------------------------------------------------

    function select(start, end)
    {
        lineEdit.select(start, end);
    }

    function selectAll()
    {
        lineEdit.selectAll();
    }

    function deselect()
    {
        lineEdit.deselect();
    }

    //---------------------------------------------------------------------------------------------

    function clear()
    {
        lineEdit.clear();
    }

    //---------------------------------------------------------------------------------------------
    // Events

    function onKeyPressed (event) {}
    function onKeyReleased(event) {}

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

    LabelRoundInfo
    {
        id: label

        anchors.top   : parent.top
        anchors.bottom: parent.bottom

        paddingLeft : st.lineEditLabel_labelPaddingLeft
        paddingRight: st.lineEditLabel_labelPaddingRight
    }

    LineEdit
    {
        id: lineEdit

        anchors.left  : label.right
        anchors.right : parent.right
        anchors.top   : parent.top
        anchors.bottom: parent.bottom

        anchors.leftMargin: -st.lineEditLabel_lineEditLeftMargin

        function onKeyPressed(event)
        {
            lineEditLabel.onKeyPressed(event);
        }

        function onKeyReleased(event)
        {
            lineEditLabel.onKeyReleased(event);
        }
    }
}
