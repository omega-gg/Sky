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

ComponentGridTrack
{
    id: componentGridHistory

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property int margins: st.margins

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias foreground: foreground

    //---------------------------------------------------------------------------------------------
    // Style

    property alias colorFront: foreground.color

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    // NOTE: We want the text on top of the foreground.
    itemText.z: 1

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------
    // Virtual

    /* virtual */ function getPlaylist() { return null; }

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

    Rectangle
    {
        id: foreground

        anchors.left  : parent.left
        anchors.top   : parent.top
        anchors.bottom: parent.bottom

        anchors.margins: margins

        width: st.getWidthHistory(getPlaylist(), componentGridHistory, source, index)

        radius: st.radius

        visible: (width != 0)

        opacity: st.componentHistory_opacity

        color: st.componentHistory_color

//#QT_4
        smooth: true
//#END
    }
}
