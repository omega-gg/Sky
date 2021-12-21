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

        width : height
        height: parent.height - st.labelLoadingText_padding

        icon          : st.icon12x12_close
        iconSourceSize: st.size12x12

        /* QML_EVENT */ onPressed: function(mouse) { labelLoading.pressed(mouse); }
        /* QML_EVENT */ onClicked: function(mouse) { labelLoading.clicked(mouse); }
    }
}
