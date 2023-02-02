//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkyBase.

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

Rectangle
{
    id: rectangleTag

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property int borderSize: st.border_size

    property bool hovered: false

    //---------------------------------------------------------------------------------------------
    // Private

    property bool pAnimate: false

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias isAnimated: timer.running

    //---------------------------------------------------------------------------------------------

    property alias timer     : timer
    property alias itemBorder: itemBorder

    //---------------------------------------------------------------------------------------------
    // Style

    property alias durationAnimation: timer.interval

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    radius: Math.round(height / 16)

    visible: hovered

    opacity: st.rectangleTag_opacity

    color: st.color_highlight

//#QT_4
    smooth: true
//#END

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    onHoveredChanged: visible = hovered

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function click()
    {
        visible = true;

        opacity = 1.0;

        pAnimate = true;

        scale = 2.0;

        opacity = 0.0;

        pAnimate = false;

        timer.interval = durationAnimation;

        timer.restart();
    }

    //---------------------------------------------------------------------------------------------
    // Private

    function pReset()
    {
        if (hovered == false) visible = false;

        scale = 1.0;

        opacity = st.rectangleTag_opacity;
    }

    //---------------------------------------------------------------------------------------------
    // Animations
    //---------------------------------------------------------------------------------------------

    Behavior on scale
    {
        enabled: pAnimate

        PropertyAnimation
        {
            duration: durationAnimation

            easing.type: st.easing
        }
    }

    Behavior on opacity
    {
        enabled: pAnimate

        PropertyAnimation
        {
            duration: durationAnimation

            easing.type: st.easing
        }
    }

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

    Timer
    {
        id: timer

        interval: st.ms500

        onTriggered: pReset()
    }

    Rectangle
    {
        id: itemBorder

        anchors.fill: parent

        radius: rectangleTag.radius

        color: "transparent"

//#QT_4
        smooth: true
//#END

        border.width: borderSize
        border.color: rectangleTag.color
    }
}
