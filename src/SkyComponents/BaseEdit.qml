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

MouseArea
{
    id: baseEdit

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property bool isFocused: textInput.activeFocus
    property bool isHovered: containsMouse

    property real paddingLeft : st.baseEdit_padding
    property real paddingRight: st.baseEdit_padding

    property bool autoSelect: true

    //---------------------------------------------------------------------------------------------
    // Private

    property bool pTextEvent: true

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias text       : textInput      .text
    property alias textDefault: itemTextDefault.text

    property alias selectedText: textInput.selectedText

    property alias cursorPosition: textInput.cursorPosition

    property alias selectionStart: textInput.selectionStart
    property alias selectionEnd  : textInput.selectionEnd

    property alias maximumLength: textInput.maximumLength

    property alias echoMode: textInput.echoMode

    //---------------------------------------------------------------------------------------------

    property alias textInput      : textInput
    property alias itemTextDefault: itemTextDefault

    //---------------------------------------------------------------------------------------------
    // Style

    property alias cursorDuration: textInput.cursorDuration

    property alias cursorWidth : textInput.cursorWidth
    property alias cursorHeight: textInput.cursorHeight

    property alias cursorColor: textInput.cursorColor

    property alias colorSelection    : textInput.selectionColor
    property alias colorSelectionText: textInput.selectedTextColor

    property alias colorDefault: itemTextDefault.color

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    height: st.baseEdit_height

    acceptedButtons: Qt.LeftButton | Qt.RightButton

    hoverEnabled: true

    cursor: MouseArea.IBeamCursor

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    onPressed:
    {
        if (mouse.button & Qt.LeftButton)
        {
            textInput.forceActiveFocus();
        }
    }

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function focus()
    {
        textInput.forceActiveFocus();
    }

    //---------------------------------------------------------------------------------------------

    function moveCursorAt(position)
    {
        textInput.cursorPosition = position;
    }

    function moveCursorAtStart()
    {
        textInput.cursorPosition = 0;
    }

    function moveCursorAtEnd()
    {
        textInput.cursorPosition = text.length;
    }

    //---------------------------------------------------------------------------------------------

    function select(start, end)
    {
        timer.stop();

        textInput.select(start, end);
    }

    function selectAll()
    {
        timer.stop();

        textInput.selectAll();
    }

    function deselect()
    {
        timer.stop();

        textInput.deselect();
    }

    //---------------------------------------------------------------------------------------------

    function clear()
    {
        text = "";
    }

    //---------------------------------------------------------------------------------------------
    // Events

    function onKeyPressed (event) {}
    function onKeyReleased(event) {}

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    Timer
    {
        id: timer

        interval: 10

        onTriggered: textInput.selectAll()
    }

    TextInput
    {
        id: textInput

        property int cursorDuration: st.baseEdit_durationAnimation

        property real cursorWidth: st.baseEdit_cursorWidth

        property real cursorHeight: sk.textHeight(font)

        property color cursorColor: st.baseEdit_colorCursor

        anchors.fill: parent

        anchors.leftMargin : paddingLeft
        anchors.rightMargin: paddingRight
        anchors.topMargin  : st.baseEdit_padding

        selectByMouse: true

        maximumLength: st.baseEdit_maximumLength

        selectionColor   : st.baseEdit_colorSelection
        selectedTextColor: st.baseEdit_colorSelectionText

        font.family   : st.text_fontFamily
        font.pixelSize: st.text_pixelSize

        cursorDelegate: Component
        {
            Rectangle
            {
                id: cursor

                width : parent.cursorWidth
                height: parent.cursorHeight

                visible: parent.cursorVisible

                color: parent.cursorColor

                SequentialAnimation
                {
                    running: cursor.visible

                    loops: Animation.Infinite

                    PropertyAction { target: cursor; property: "opacity"; value: 1.0 }

                    PauseAnimation { duration: parent.cursorDuration }

                    PropertyAction { target: cursor; property: "opacity"; value: 0.0 }

                    PauseAnimation { duration: parent.cursorDuration }
                }
            }
        }

        onActiveFocusChanged:
        {
            if (activeFocus)
            {
                if (autoSelect)
                {
                    timer.start();
                }
            }
            else deselect();
        }

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
    }

    TextBase
    {
        id: itemTextDefault

        anchors.left  : textInput.left
        anchors.right : textInput.right
        anchors.top   : parent.top
        anchors.bottom: parent.bottom

        verticalAlignment: Text.AlignVCenter

        visible: (isFocused == false && textInput.text == "")

        color: st.baseEdit_colorDefault
    }
}
