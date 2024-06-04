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
    id: labelStream

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    /* mandatory */ property variant slider

    /* read */ property int position: pGetPosition()

    /* read */ property int time: (visible) ? st.getSliderValue(slider, position) : -1

    //---------------------------------------------------------------------------------------------
    // Style

    property color color: st.buttonStream_color

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias itemTitle: itemTitle
    property alias itemTime : itemTime

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    height: pGetHeight()

    visible: (slider.enabled && slider.isHovered)

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------
    // Private

    function pGetHeight()
    {
        if (itemTitle.visible)
        {
            return itemTitle.height + itemTime.height;
        }
        else return itemTime.height;
    }

    function pGetPosition()
    {
        if (visible)
        {
             return window.mapToItem(slider.slider, window.mouseX, 0).x;
        }
        else return -1;
    }

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

    ButtonPiano
    {
        id: itemTitle

        height: st.labelStream_heightTitle + borderSizeHeight

        maximumWidth: labelStream.width

        x: st.getSliderX(slider.slider, width, position)

        borderLeft  : borderSize
        borderTop   : borderSize
        borderBottom: borderSize

        enabled: false

        visible: (text != "")

        text: st.getChapterTitle(slider, time)

        colorA: labelStream.color
        colorB: labelStream.color

        itemText.opacity: 1.0

        itemText.style: st.text_raised

//#DESKTOP
        // FIXME Qt5.14: Sometimes sk.textWidth() is too short.
        itemText.elide: (width == maximumWidth) ? Text.ElideRight
                                                : Text.ElideNone
//#END
    }

    ButtonPiano
    {
        id: itemTime

        anchors.bottom: parent.bottom

        height: st.labelStream_heightTime

        x: st.getSliderX(slider.slider, width, position)

        borderLeft: borderSize

        enabled: false

        text: controllerPlaylist.getPlayerTime(time, 8)

        colorA: labelStream.color
        colorB: labelStream.color

        itemText.opacity: 1.0

        itemText.style: st.text_raised

        // FIXME Qt5.14: Sometimes sk.textWidth() is too short.
        itemText.elide: Text.ElideNone
    }
}
