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

Item
{
    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias value: slider.value

    //---------------------------------------------------------------------------------------------

    property alias slider: slider

    property alias buttonMinimum: buttonMinimum
    property alias buttonMaximum: buttonMaximum

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    width : st.sliderVolume_width
    height: st.sliderVolume_height

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function volumeUp()
    {
        value = Math.min(value + 0.1, 1.0);
    }

    function volumeDown()
    {
        value = Math.max(0.0, value - 0.1);
    }

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    ButtonMask
    {
        id: buttonMinimum

        anchors.verticalCenter: parent.verticalCenter

        icon: st.icon16x16_audioMin

        onPressed: slider.value = 0
    }

    Slider
    {
        id: slider

        anchors.left : buttonMinimum.right
        anchors.right: buttonMaximum.left

        anchors.verticalCenter: parent.verticalCenter

        value: 1.0
    }

    ButtonMask
    {
        id: buttonMaximum

        anchors.right: parent.right

        anchors.verticalCenter: parent.verticalCenter

        icon: st.icon16x16_audioMax

        onPressed: slider.value = 1.0
    }
}
