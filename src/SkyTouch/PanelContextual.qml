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

// NOTE: This is mostly a copy paste of the Panel item.
BasePanelContextual
{
    id: panel

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property bool animate: st.animate

    property int margins: st.panelContextual_margins

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    default property alias content: content.data

    property alias contentWidth : content.width
    property alias contentHeight: content.height

    //---------------------------------------------------------------------------------------------

    property alias clip: content.clip

    //---------------------------------------------------------------------------------------------

    property alias background: background

    property alias itemContent: content

    //---------------------------------------------------------------------------------------------
    // Style

    property int durationAnimation: st.panel_durationAnimation

    property alias radius: background.radius

    property alias backgroundOpacity: background.opacity

    property alias color: background.color

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    preferredWidth: st.panelContextual_preferredWidth

    //---------------------------------------------------------------------------------------------
    // Animations
    //---------------------------------------------------------------------------------------------

    Behavior on opacity
    {
        id: behaviorOpacity

        enabled: false

        PropertyAnimation
        {
            duration: durationAnimation

            easing.type: st.easing
        }
    }

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------
    // BasePanelContextual events

    function onActive(active)
    {
        st.animateShow(panel, active, behaviorOpacity, animate);
    }

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

    Rectangle
    {
        id: background

        anchors.fill: parent

        radius: st.radius

        opacity: st.panelContextual_opacity

        color: st.panelContextual_color

//#QT_4
        smooth: true
//#END
    }

    Item
    {
        id: content

        anchors.fill: parent

        anchors.margins: margins
    }
}
