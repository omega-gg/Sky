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

TextEdit
{
    id: baseTextEdit

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property bool isFocused: activeFocus
    property bool isHovered: mouseArea.containsMouse

    property bool autoSelect: false

    property int cursorWidth: st.baseLineEdit_cursorWidth

    property int cursorHeight: sk.textHeight(font)

    //---------------------------------------------------------------------------------------------
    // Style

    property int durationCursor: st.baseLineEdit_durationCursor

    property alias colorText         : baseTextEdit.color
    property alias colorTextSelection: baseTextEdit.selectionColor
    property alias colorTextSelected : baseTextEdit.selectedTextColor

    property color colorCursor: st.baseLineEdit_colorCursor

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    textMargin: st.baseTextEdit_margin

//#QT_5
    renderType: Text.NativeRendering
//#END

    selectByMouse: true

    cursorDelegate: Component
    {
        Rectangle
        {
            id: cursor

            width : parent.cursorWidth
            height: parent.cursorHeight

            visible: parent.cursorVisible

            color: parent.colorCursor

            SequentialAnimation
            {
                running: cursor.visible

                loops: Animation.Infinite

                PropertyAction { target: cursor; property: "opacity"; value: 1.0 }

                PauseAnimation { duration: parent.durationCursor }

                PropertyAction { target: cursor; property: "opacity"; value: 0.0 }

                PauseAnimation { duration: parent.durationCursor }
            }
        }
    }

    //---------------------------------------------------------------------------------------------
    // Style

    color            : st.baseLineEdit_colorText
    selectionColor   : st.baseLineEdit_colorTextSelection
    selectedTextColor: st.baseLineEdit_colorTextSelected

    font.family   : st.text_fontFamily
    font.pixelSize: st.text_pixelSize
    font.bold     : true

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    onActiveFocusChanged:
    {
        if (activeFocus)
        {
            if (autoSelect)
            {
                textEdit.selectAll();
            }
        }
        else baseTextEdit.deselect();
    }

    //---------------------------------------------------------------------------------------------
    // Keys
    //---------------------------------------------------------------------------------------------

    Keys.onPressed:
    {
        if (event.key == Qt.Key_Left
            &&
            cursorPosition == 0 && selectionStart == selectionEnd)
        {
            event.accepted = true;
        }
        else if (event.key == Qt.Key_Right
                 &&
                 cursorPosition == text.length && selectionStart == selectionEnd)
        {
            event.accepted = true;
        }

        onKeyPressed(event);
    }

    Keys.onReleased: onKeyReleased(event)

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function focus()
    {
        forceActiveFocus();
    }

    //---------------------------------------------------------------------------------------------

    function moveCursorAt(position)
    {
        cursorPosition = position;
    }

    function moveCursorAtStart()
    {
        cursorPosition = 0;
    }

    function moveCursorAtEnd()
    {
        cursorPosition = text.length;
    }

    //---------------------------------------------------------------------------------------------
    // Events

    function onKeyPressed (event) {}
    function onKeyReleased(event) {}

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    MouseArea
    {
        id: mouseArea

        anchors.fill: parent

        acceptedButtons: Qt.NoButton

        hoverEnabled: true

        cursor: Qt.IBeamCursor
    }
}
