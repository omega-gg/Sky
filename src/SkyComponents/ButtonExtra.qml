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
    id: buttonExtra

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias isHighlighted: buttonText.isHighlighted

    property alias radius: buttonText.radius

    property alias padding: buttonText.padding

    property alias checkable: buttonText.checkable
    property alias checked  : buttonText.checked

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

    signal pressed(variant mouse)

    signal clicked      (variant mouse)
    signal doubleClicked(variant mouse)

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    width : st.buttonExtra_width
    height: st.buttonPush_height

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

    ButtonPush
    {
        id: buttonText

        anchors.left: parent.left

        anchors.right: (buttonIcon.visible) ? buttonIcon.left
                                            : parent.right

        enabled: buttonExtra.enabled

        /* QML_EVENT */ onPressed: function(mouse) { buttonExtra.pressed(mouse) }

        /* QML_EVENT */ onClicked: function(mouse) { buttonExtra.clicked(mouse) }

        /* QML_EVENT */ onDoubleClicked: function(mouse) { buttonExtra.doubleClicked(mouse) }
    }

    ButtonPushIcon
    {
        id: buttonIcon

        anchors.right: parent.right

        enabled: buttonExtra.enabled
    }
}
