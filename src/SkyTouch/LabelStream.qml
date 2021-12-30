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

BaseLabel
{
    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    /* mandatory */ property variant slider

    property int padding: st.labelStream_padding

    /* read */ property int position: pGetPosition()

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias itemText: itemText

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    width: sk.textWidth(itemText.font, itemText.text) + padding * 2

    height: st.labelTiny_size

    x: st.getSliderX(slider, width, position)

    visible: (slider.enabled && slider.isHovered)

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------
    // Private

    function pGetPosition()
    {
        if (visible)
        {
             return window.mapToItem(slider, window.mouseX, 0).x;
        }
        else return -1;
    }

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

    TextBase
    {
        id: itemText

        anchors.fill: parent

        horizontalAlignment: Text.AlignHCenter
        verticalAlignment  : Text.AlignVCenter

        text: controllerPlaylist.getPlayerTime(st.getSliderValue(slider, position), 8)
    }
}
