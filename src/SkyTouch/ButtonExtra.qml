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
    id: buttonExtra

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property int padding: buttonText.padding

    property alias checked: buttonText.checked

    //---------------------------------------------------------------------------------------------

    property alias text: buttonText.text

    property alias font: buttonText.font

    //---------------------------------------------------------------------------------------------

    property alias itemText: buttonText.itemText

    property alias buttonText: buttonText
    property alias buttonIcon: buttonIcon

    //---------------------------------------------------------------------------------------------
    // Signals
    //---------------------------------------------------------------------------------------------

    signal clicked
    signal doubleClicked

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    width : st.buttonExtra_width
    height: st.buttonTouch_size

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

    ButtonTouchLeft
    {
        id: buttonText

        anchors.left: parent.left

        anchors.right: (buttonIcon.visible) ? buttonIcon.left
                                            : parent.right

        anchors.rightMargin: (buttonIcon.visible) ? st.margins : 0

        onClicked: buttonExtra.clicked()

        onDoubleClicked: buttonExtra.doubleClicked()
    }

    ButtonTouchIcon
    {
        id: buttonIcon

        anchors.right: parent.right
    }
}
