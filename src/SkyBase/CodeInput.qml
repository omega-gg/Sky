//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkyBase.

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
    id: codeInput

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property int pixelSize: st.dp16

    property int itemWidth : st.dp64
    property int itemHeight: st.dp48

    property int radius: st.dp8

    //---------------------------------------------------------------------------------------------
    // Style

    property color color          : st.codeNumber_color
    property color colorBackground: st.codeNumber_colorBackground

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias spacing: row.spacing

    //---------------------------------------------------------------------------------------------

    property alias textInput: textInput
    property alias row      : row

    //---------------------------------------------------------------------------------------------
    // Signal
    //---------------------------------------------------------------------------------------------

    signal requestHide

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    width : itemWidth * 4 + spacing * 3
    height: itemHeight

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    Component.onCompleted: textInput.forceActiveFocus()

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

    TextInput
    {
        id: textInput

        anchors.fill: parent

        opacity: 0.0

//#QT_4
        validator: IntValidator { bottom: 0 }
//#ELSE
        inputMethodHints: Qt.ImhDigitsOnly
//#END

        onActiveFocusChanged: if (activeFocus == false) codeInput.requestHide()

        /* QML_EVENT */ Keys.onPressed: function(event)
        {
            // NOTE: The user should not be able to move the selection cursor.
            if (event.key == Qt.Key_Escape || event.key == Qt.Key_Space
                ||
                event.key == Qt.Key_Left || event.key == Qt.Key_Right)
            {
                event.accepted = true;

                codeInput.requestHide();
            }
        }
    }

    Row
    {
        id: row

        spacing: st.dp8

        Repeater
        {
            id: repeater

            model: 4

            Item
            {
                width : itemWidth
                height: itemHeight

                Rectangle
                {
                    anchors.fill: parent

                    radius: codeInput.radius

                    opacity: 0.4

                    color: colorBackground
                }

                TextBase
                {
                    anchors.centerIn: parent

                    color: codeInput.color

                    font.pixelSize: pixelSize
                }
            }
        }
    }
}
