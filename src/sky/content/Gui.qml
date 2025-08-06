//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the Sky kit runtime.

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
    id: gui

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property bool ui: false

    //---------------------------------------------------------------------------------------------
    // Connections
    //---------------------------------------------------------------------------------------------

    Connections
    {
        target: window

        /* QML_CONNECTION */ function onKeyPressed (event) { gui.onKeyPressed (event) }
        /* QML_CONNECTION */ function onKeyReleased(event) { gui.onKeyReleased(event) }
    }

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------
    // Keys

    function onKeyPressed(event)
    {
        if (event.key == Qt.Key_F1)
        {
            ui = !ui;
        }
    }

    function onKeyReleased(event)
    {
        if (event.isAutoRepeat) return;
    }

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

    TextBase
    {
        id: itemText

        anchors.centerIn: parent

        text: qsTr("Welcome to Sky kit")

        font.pixelSize: st.dp32
    }

    TextBase
    {
        anchors.top: itemText.bottom

        anchors.topMargin: st.dp16

        anchors.horizontalCenter: parent.horizontalCenter

        text: qsTr("Drop a .sky file to begin")

        color: st.text3_color

        font.pixelSize: st.dp20
    }

    TextBase
    {
        anchors.left: parent.left
        anchors.top : parent.top

        anchors.margins: st.dp8

        visible: (opacity != 0.0)

        opacity: (ui == false)

        text: qsTr("Press F1 for UI")

        color: st.text3_color

        font.pixelSize: st.dp16

        Behavior on opacity
        {
            PropertyAnimation
            {
                duration: st.duration_fast

                easing.type: st.easing
            }
        }
    }

    Item
    {
        anchors.fill: parent

        visible: (opacity != 0.0)
        opacity: (ui)

        Behavior on opacity
        {
            PropertyAnimation
            {
                duration: st.duration_fast

                easing.type: st.easing
            }
        }

        ButtonsWindow
        {
            anchors.right: parent.right
        }
    }
}
