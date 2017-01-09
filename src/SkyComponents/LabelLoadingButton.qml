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

LabelLoadingText
{
    id: labelLoading

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias button: button

    //---------------------------------------------------------------------------------------------
    // Signal
    //---------------------------------------------------------------------------------------------

    signal pressed(variant mouse)
    signal clicked(variant mouse)

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    width: paddingLeft + itemIcon.width + spacing + itemText.width + button.width + paddingRight

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    ButtonRound
    {
        id: button

        anchors.left: itemText.right

        anchors.leftMargin: spacing

        anchors.verticalCenter: parent.verticalCenter

        width : st.labelLoadingButton_buttonWidth
        height: st.labelLoadingButton_buttonHeight

        icon          : st.icon16x16_close
        iconSourceSize: st.size16x16

        onPressed: labelLoading.pressed(mouse)
        onClicked: labelLoading.clicked(mouse)
    }
}
