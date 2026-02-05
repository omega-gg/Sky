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

TextEdit
{
    id: baseTextEdit

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property bool isFocused: activeFocus
    property bool isHovered: mouseArea.hoverActive

    property bool autoSelect: false

    property int cursorWidth: st.baseLineEdit_cursorWidth

    property int cursorHeight: sk.textHeight(font)

    //---------------------------------------------------------------------------------------------
    // Style

    property int durationCursor: st.baseLineEdit_durationCursor

    property color colorCursor: st.baseLineEdit_colorCursor

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias textDefault: itemTextDefault.text

    property alias itemTextDefault: itemTextDefault

    //---------------------------------------------------------------------------------------------
    // Style

    property alias colorText         : baseTextEdit.color
    property alias colorTextSelection: baseTextEdit.selectionColor
    property alias colorTextSelected : baseTextEdit.selectedTextColor

    property alias colorDefault: itemTextDefault.color

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    textMargin: st.baseTextEdit_margin

//#QT_NEW
    renderType: st.text_renderType
//#END

    selectByMouse: true

    wrapMode: Text.Wrap

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
    font.bold     : st.text_bold

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

    /* QML_EVENT */ Keys.onPressed: function(event)
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

    /* QML_EVENT */ Keys.onReleased: function(event)
    {
        onKeyReleased(event);
    }

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function setFocus()
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
    // Children
    //---------------------------------------------------------------------------------------------

    MouseArea
    {
        id: mouseArea

        anchors.fill: parent

        acceptedButtons: Qt.NoButton

        hoverEnabled: true

        cursor: Qt.IBeamCursor
    }

    TextBase
    {
        id: itemTextDefault

        anchors.fill: parent

        anchors.margins: baseTextEdit.textMargin

        verticalAlignment: Text.AlignTop

        maximumLineCount: 1

        visible: (isFocused == false && baseTextEdit.text == "")

        color: st.baseLineEdit_colorDefault

        font: baseTextEdit.font
    }
}
