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

LabelTiny
{
    id: popup

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    /* read */ property bool isActive: false

    property bool animate: st.animate

    //---------------------------------------------------------------------------------------------
    // Style

    property int durationAnimation: st.panel_durationAnimation

    property int interval: st.popup_interval

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    maximumWidth: parent.width - st.marginsDouble

    visible: false
    opacity: 0.0

    backgroundOpacity: st.popup_opacity

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    onIsActiveChanged: st.animateShow(popup, isActive, behaviorOpacity, animate)

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

    function showText(text)
    {
        popup.text = text;

        show();
    }

    function show()
    {
        if (isActive == true)
        {
            timer.restart();

            return;
        }

        isActive = true;

        timer.start();
    }

    function hide()
    {
        timer.stop();

        isActive = false;
    }

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

    Timer
    {
        id: timer

        interval: popup.interval

        onTriggered: isActive = false
    }
}
