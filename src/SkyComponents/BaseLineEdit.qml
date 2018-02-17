//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

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
//#QT_5
    id: baseLineEdit
//#END

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property bool isFocused: textInput.activeFocus
    property bool isHovered: containsMouse

    property int padding: Math.round((height - cursorHeight) / 2)

    property int paddingLeft : padding
    property int paddingRight: padding

    property bool autoSelect: true

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

    property alias durationCursor: textInput.durationCursor

    property alias cursorWidth : textInput.cursorWidth
    property alias cursorHeight: textInput.cursorHeight

    property alias colorCursor: textInput.colorCursor

    property alias colorSelection    : textInput.selectionColor
    property alias colorSelectionText: textInput.selectedTextColor

    property alias colorDefault: itemTextDefault.color

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    height: st.baseLineEdit_height

    acceptedButtons: Qt.LeftButton | Qt.RightButton

    hoverEnabled: true

    cursor: Qt.IBeamCursor

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

        interval: 1

        onTriggered: textInput.selectAll()
    }

    TextInput
    {
        id: textInput

        property int durationCursor: st.baseLineEdit_durationCursor

        property int cursorWidth: st.baseLineEdit_cursorWidth

        property int cursorHeight: sk.textHeight(font)

        property color colorCursor: st.baseLineEdit_colorCursor

        anchors.fill: parent

        anchors.leftMargin  : paddingLeft
        anchors.rightMargin : paddingRight
        anchors.topMargin   : padding
        anchors.bottomMargin: padding

//#QT_5
        verticalAlignment: TextInput.AlignVCenter

        enabled: baseLineEdit.enabled

        clip: true

        renderType: Text.NativeRendering
//#END

        selectByMouse: true

        maximumLength: st.baseLineEdit_maximumLength

        selectionColor   : st.baseLineEdit_colorSelection
        selectedTextColor: st.baseLineEdit_colorSelectionText

        font.family   : st.text_fontFamily
        font.pixelSize: st.text_pixelSize
        font.bold     : true

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

        anchors.left : textInput.left
        anchors.right: textInput.right

//#QT_4
        anchors.top: textInput.top
//#ELSE
        anchors.top: parent.top

        anchors.topMargin: padding
//#END

        visible: (isFocused == false && textInput.text == "")

        color: st.baseLineEdit_colorDefault
    }
}
