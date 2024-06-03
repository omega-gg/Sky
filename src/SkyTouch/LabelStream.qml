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
    id: labelStream

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    /* mandatory */ property variant slider

    /* read */ property int position: pGetPosition()

    /* read */ property int time: (visible) ? st.getSliderValue(slider, position) : -1

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias labelTitle: labelTitle
    property alias labelTime : labelTime

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    anchors.left : parent.left
    anchors.right: parent.right

    height: pGetHeight()

    visible: (slider.enabled && slider.isHovered)

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------
    // Private

    function pGetHeight()
    {
        if (labelTitle.visible)
        {
            return labelTitle.height + st.margins + labelTime.height;
        }
        else return labelTime.height;
    }

    function pGetPosition()
    {
        if (visible)
        {
            return window.viewport.mapToItem(slider, window.mouseX, 0).x;
        }
        else return -1;
    }

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

    Label
    {
        id: labelTitle

        height: st.labelTiny_size

        maximumWidth: labelStream.width

        x: st.getSliderX(slider, width, position)

        radius: height

        visible: (text != "")

        text: st.getChapterTitle(slider, time)

        itemText.wrapMode: Text.NoWrap

        itemText.maximumLineCount: 1

//#DESKTOP
        // FIXME Qt5.14: Sometimes sk.textWidth() is too short.
        itemText.elide: (width == maximumWidth) ? Text.ElideRight
                                                : Text.ElideNone
//#END
    }

    Label
    {
        id: labelTime

        anchors.bottom: parent.bottom

        height: st.labelTiny_size

        x: st.getSliderX(slider, width, position)

        radius: height

        text: controllerPlaylist.getPlayerTime(time, 8)
    }
}
