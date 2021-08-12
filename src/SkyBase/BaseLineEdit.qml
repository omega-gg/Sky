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

MouseArea
{
    id: baseLineEdit

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property bool isFocused: textInput.activeFocus
    property bool isHovered: containsMouse

    property int padding: Math.round((height - cursorHeight) / 2)

    property int paddingLeft : padding
    property int paddingRight: paddingLeft

    property bool autoSelect: st.baseLineEdit_autoSelect

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias text       : textInput      .text
    property alias textDefault: itemTextDefault.text

//#QT_5
    // NOTE android: This is useful to get each 'text changed' events.
    property alias displayText: textInput.displayText
//#END

    property alias selectedText: textInput.selectedText

    property alias cursorPosition: textInput.cursorPosition

    property alias selectionStart: textInput.selectionStart
    property alias selectionEnd  : textInput.selectionEnd

    property alias maximumLength: textInput.maximumLength

    property alias echoMode: textInput.echoMode

    property alias font: textInput.font

    //---------------------------------------------------------------------------------------------

    property alias textInput      : textInput
    property alias itemTextDefault: itemTextDefault

    //---------------------------------------------------------------------------------------------
    // Style

    property alias durationCursor: textInput.durationCursor

    property alias cursorWidth : textInput.cursorWidth
    property alias cursorHeight: textInput.cursorHeight

    property alias colorCursor: textInput.colorCursor

    property alias colorText         : textInput.color
    property alias colorTextSelection: textInput.selectionColor
    property alias colorTextSelected : textInput.selectedTextColor

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
//#QT_4
        timer.stop();
//#END

        textInput.select(start, end);
    }

    function selectAll()
    {
//#QT_4
        // NOTE: If the timer is running we wait for the timeout.
        if (timer.running) return;
//#END

        textInput.selectAll();
    }

    function deselect()
    {
//#QT_4
        timer.stop();
//#END

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

//#QT_4
    Timer
    {
        id: timer

        interval: 1

        onTriggered: textInput.selectAll()
    }
//#END

    TextInput
    {
        id: textInput

        property int durationCursor: st.baseLineEdit_durationCursor

        property int cursorWidth: st.baseLineEdit_cursorWidth

        property int cursorHeight: sk.textHeight(font)

        property color colorCursor: st.baseLineEdit_colorCursor

        anchors.fill: parent

        anchors.leftMargin : paddingLeft
        anchors.rightMargin: paddingRight

//#QT_4
        anchors.topMargin   : padding
        anchors.bottomMargin: padding
//#ELSE
        verticalAlignment: TextInput.AlignVCenter

        clip: true

        renderType: st.text_renderType
//#END

        enabled: baseLineEdit.enabled

        selectByMouse: true

        maximumLength: st.baseLineEdit_maximumLength

        color            : st.baseLineEdit_colorText
        selectionColor   : st.baseLineEdit_colorTextSelection
        selectedTextColor: st.baseLineEdit_colorTextSelected

        font.family   : st.text_fontFamily
        font.pixelSize: st.text_pixelSize
        font.bold     : st.text_bold

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
//#QT_4
                    timer.start();
//#ELSE
                    textInput.selectAll();
//#END
                }
            }
            else baseLineEdit.deselect();
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

        anchors.fill: textInput

        verticalAlignment: Text.AlignVCenter

        visible: (isFocused == false && textInput.text == "")

        color: st.baseLineEdit_colorDefault

        font.pixelSize: textInput.font.pixelSize
    }
}
