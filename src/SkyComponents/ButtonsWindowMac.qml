//=================================================================================================
/*
    Copyright (C) 2015-2026 Sky kit authors. <http://omega.gg/Sky>

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

SkyMouseArea
{
    id: buttonsWindowMac

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    // NOTE macOS: Hovering the buttons shows the icons on all of them.
    /* read */ property bool isHovered: hoverActive

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias buttonClose   : buttonClose
    property alias buttonIconify : buttonIconify
    property alias buttonMaximize: buttonMaximize

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    width: buttonClose.width + buttonIconify.width + buttonMaximize.width
           + st.buttonsWindowMac_spacing * 2

    height: buttonClose.height

    acceptedButtons: Qt.NoButton

    hoverEnabled: true

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------
    // Events

    function onClose()
    {
        window.close()
    }

    function onIconify()
    {
        window.minimized = true;
    }

    function onMaximize()
    {
        if (window.fullScreen)
        {
            window.fullScreen = false;
        }
        else window.maximized = !(window.maximized);
    }

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

    ButtonWindowMac
    {
        id: buttonClose

        anchors.left: parent.left

        anchors.verticalCenter: parent.verticalCenter

        showIcon: buttonsWindowMac.isHovered

        icon: st.icon_macClose

        color     : st.buttonWindowMac_colorClose
        colorPress: st.buttonWindowMac_colorClosePress

        onClicked: onClose()
    }

    ButtonWindowMac
    {
        id: buttonIconify

        anchors.left: buttonClose.right

        anchors.leftMargin: st.buttonsWindowMac_spacing

        anchors.verticalCenter: buttonClose.verticalCenter

        showIcon: buttonsWindowMac.isHovered

        icon: st.icon_macIconify

        color     : st.buttonWindowMac_colorIconify
        colorPress: st.buttonWindowMac_colorIconifyPress

        onClicked: onIconify()
    }

    ButtonWindowMac
    {
        id: buttonMaximize

        anchors.left: buttonIconify.right

        anchors.leftMargin: st.buttonsWindowMac_spacing

        anchors.verticalCenter: buttonClose.verticalCenter

        showIcon: buttonsWindowMac.isHovered

        icon: st.icon_macMaximize

        color     : st.buttonWindowMac_colorMaximize
        colorPress: st.buttonWindowMac_colorMaximizePress

        onClicked: onMaximize()
    }
}
