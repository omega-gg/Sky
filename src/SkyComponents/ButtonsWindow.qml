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
    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias buttonIconify : buttonIconify
    property alias buttonMaximize: buttonMaximize
    property alias buttonClose   : buttonClose

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    width: buttonIconify.width + buttonMaximize.width + buttonClose.width

    height: buttonClose.height

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------
    // Events

    function onIconify()
    {
        window.minimized = true;
    }

    function onMaximize()
    {
        window.maximized = !(window.maximized);
    }

    function onClose()
    {
        window.close()
    }

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

    ButtonPianoWindow
    {
        id: buttonIconify

        anchors.right : (buttonMaximize.visible) ? buttonMaximize.left
                                                 : buttonClose   .left

        anchors.top   : buttonClose.top
        anchors.bottom: buttonClose.bottom

        borderLeft  : borderSize
        borderRight : 0
        borderBottom: borderSize

        icon          : st.icon12x12_iconify
        iconSourceSize: st.size12x12

        onClicked: onIconify()
    }

    ButtonPianoWindow
    {
        id: buttonMaximize

        anchors.right : buttonClose.left
        anchors.top   : buttonClose.top
        anchors.bottom: buttonClose.bottom

        borderRight : 0
        borderBottom: borderSize

        highlighted: (window.maximized || window.fullScreen)

        icon: (highlighted) ? st.icon12x12_minimize
                            : st.icon12x12_maximize

        iconSourceSize: st.size12x12

        onClicked: onMaximize()
    }

    ButtonPianoWindow
    {
        id: buttonClose

        anchors.right: parent.right
        anchors.top  : parent.top

        height: st.barWindow_height + borderSizeHeight

        borderRight : 0
        borderBottom: borderSize

        icon          : st.icon12x12_close
        iconSourceSize: st.size12x12

        colorHoverA: st.button_colorConfirmHoverA
        colorHoverB: st.button_colorConfirmHoverB

        colorPressA: st.button_colorConfirmPressA
        colorPressB: st.button_colorConfirmPressB

        filterIcon: (isHovered || isPressed) ? st.button_filterIconB
                                             : st.button_filterIconA

        onClicked: onClose()
    }
}
