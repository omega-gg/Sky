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
    // Aliases
    //---------------------------------------------------------------------------------------------

    property int margins      : st.slider_margins
    property int marginsHandle: st.slider_marginsHandle

    //---------------------------------------------------------------------------------------------
    // Style

    property color colorHandle     : st.slider_colorHandle
    property color colorHandleHover: st.slider_colorHandleHover
    property color colorHandlePress: st.slider_colorHandlePress

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias background: background

    //---------------------------------------------------------------------------------------------
    // Style

    property alias radius: background.radius

    property alias backgroundOpacity: background.opacity

    property alias color: background.color

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    anchors.margins: st.margins

    width : st.slider_size
    height: st.slider_size

    handle: itemHandle

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    Rectangle
    {
        id: background

        anchors.fill: parent

        anchors.margins: margins

        radius: height

        opacity: (isHovered) ? st.slider_opacityHover
                             : st.slider_opacity

        color: st.slider_color

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
            id: handleBackground

            anchors.fill: parent

            anchors.margins: marginsHandle

            radius: height

            color: (handle.containsMouse && slider.pressed == false) ? colorHandleHover
                                                                     : colorHandle

//#QT_4
            smooth: true
//#END
        }
    }
}
