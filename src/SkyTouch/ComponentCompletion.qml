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
    id: componentCompletion

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    default property alias content: buttonText.data

    property alias checked: buttonText.checked

    property alias itemText: buttonText.itemText

    //---------------------------------------------------------------------------------------------

    property alias buttonText: buttonText
    property alias buttonIcon: buttonIcon

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

//#QT_4
    width: ListView.view.width
//#ELSE
    // NOTE Qt5.15: sometimes we get an undefined parent.
    anchors.left : (parent) ? parent.left  : undefined
    anchors.right: (parent) ? parent.right : undefined
//#END

    height: st.buttonTouch_size

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------
    // Events

    function onSelect() {}
    function onClick () {}

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

    ButtonTouchLeft
    {
        id: buttonText

        anchors.left : parent.left
        anchors.right: buttonIcon.left

        anchors.rightMargin: st.margins

        checked: (index == componentCompletion.ListView.view.currentIndex)

        text: title

        onClicked: onClick()
    }

    ButtonTouchIcon
    {
        id: buttonIcon

        anchors.right: parent.right

        margins: st.componentCompletion_margins

        iconDefault: st.icon_right

        onClicked: onSelect()
    }
}
