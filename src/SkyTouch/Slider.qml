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

BaseSlider
{
    id: slider

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    /* read */ property int margins: itemHandle.width / 2

    property int marginsHandle: st.slider_marginsHandle

    //---------------------------------------------------------------------------------------------
    // Style

    property real opacityBackground: st.slider_opacity

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias background: background
    property alias foreground: foreground

    property alias handleForeground: handleForeground

    //---------------------------------------------------------------------------------------------
    // Style

    property alias radius: background.radius

    property alias backgroundOpacity: background.opacity

    property alias color     : background.color
    property alias colorFront: foreground.color

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    width : st.slider_size
    height: st.slider_size

    handle: itemHandle

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

    Rectangle
    {
        id: background

        anchors.fill: parent

        radius: height

        opacity: (isHovered) ? st.slider_opacityHover
                             : opacityBackground

        color: st.slider_color

//#QT_4
        smooth: true
//#END
    }

    Rectangle
    {
        id: foreground

        anchors.left: parent.left

        anchors.verticalCenter: parent.verticalCenter

        anchors.margins: margins

        // NOTE: We add an extra pixel to go past the handle aliasing.
        width: handle.x - (margins - marginsHandle) + st.dp1

        height: st.slider_sizeHandle

        visible: (slider.enabled && value > -1)

        color: st.slider_colorFront

//#QT_4
        smooth: true
//#END
    }

    MouseArea
    {
        id: itemHandle

        width : parent.height
        height: width

        visible: slider.enabled

        acceptedButtons: Qt.NoButton

        hoverEnabled: true

        onXChanged: position = x

        Rectangle
        {
            id: handleForeground

            anchors.fill: parent

            anchors.margins: marginsHandle

            radius: height

            color: (pressed) ? foreground.color
                             : "transparent"

//#QT_4
            smooth: true
//#END

            border.width: st.slider_sizeHandle
            border.color: foreground.color
        }
    }
}
