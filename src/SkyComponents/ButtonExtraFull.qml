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
    id: buttonExtraFull

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias isHighlighted: buttonFull.isHighlighted

    property alias radius: buttonFull.radius

    property alias enableFilter: buttonFull.enableFilter

    property alias margins: buttonFull.margins

    property alias padding: buttonFull.padding
    property alias spacing: buttonFull.spacing

    property alias checkable: buttonText.checkable
    property alias checked  : buttonFull.checked

    //---------------------------------------------------------------------------------------------

    property alias isSourceDefault: buttonFull.isSourceDefault

    property alias icon       : buttonFull.icon
    property alias iconDefault: buttonFull.iconDefault

    property alias iconWidth : buttonFull.iconWidth
    property alias iconHeight: buttonFull.iconHeight

    property alias iconSourceSize : buttonFull.iconSourceSize
    property alias iconDefaultSize: buttonFull.iconDefaultSize

    property alias iconSourceArea: buttonFull.iconSourceArea

    property alias iconLoadMode: buttonFull.iconLoadMode
    property alias iconFillMode: buttonFull.iconFillMode

    property alias iconAsynchronous: buttonFull.iconAsynchronous
    property alias iconCache       : buttonFull.iconCache

    property alias iconScaling: buttonFull.iconScaling

    property alias text: buttonFull.text

    property alias font: buttonFull.font

    //---------------------------------------------------------------------------------------------

    property alias itemIcon: buttonFull.itemIcon
    property alias itemText: buttonFull.itemText

    property alias buttonFull: buttonFull
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
    // Functions
    //---------------------------------------------------------------------------------------------
    // Virtual

    /* virtual */ function getFilter()
    {
        return getFilterDefault();
    }

    /* virtual */ function getFilterDefault()
    {
        return st.getButtonFilter(buttonFull);
    }

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

    ButtonPush
    {
        id: buttonFull

        anchors.left : parent.left

        anchors.right: (buttonIcon.visible) ? buttonIcon.left
                                            : parent.right

        enabled: buttonExtraFull.enabled

        /* QML_EVENT */ onPressed: function(mouse) { buttonExtraFull.pressed(mouse) }

        /* QML_EVENT */ onClicked: function(mouse) { buttonExtraFull.clicked(mouse) }

        /* QML_EVENT */ onDoubleClicked: function(mouse) { buttonExtraFull.doubleClicked(mouse) }
    }

    ButtonPushIcon
    {
        id: buttonIcon

        anchors.right: parent.right

        enabled: buttonExtraFull.enabled
    }
}
