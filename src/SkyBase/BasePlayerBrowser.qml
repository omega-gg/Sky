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

MouseArea
{
    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    /* mandatory */ property Player player

    /* read */ property TabTrack playerTab: player.tab

    property TabTrack tab: playerTab

    property real ratio: st.playerBrowser_ratio

    //---------------------------------------------------------------------------------------------
    // Private

    property int pScroll: -1

    property bool pBackward: false
    property bool pForward : false

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias areaBackward: areaBackward
    property alias areaForward : areaForward

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    // NOTE: We only want the wheel events by default.
    acceptedButtons: Qt.NoButton

    wheelEnabled: true

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    /* QML_EVENT */ onWheeled: function(steps)
    {
        if (timerScroll.running) return;

        var i = steps;

        if (i > 0)
        {
            while (i)
            {
                onNext();

                i--;
            }

            pFlashNext();
        }
        else
        {
            while (i)
            {
                onPrevious();

                i++;
            }

            pFlashPrevious();
        }
    }

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function flashPrevious()
    {
        onPrevious();

        pFlashPrevious();
    }

    function flashNext()
    {
        onNext();

        pFlashNext();
    }

    //---------------------------------------------------------------------------------------------

    function scrollPrevious()
    {
        if (buttonPrevious.visible == false || (pScroll == 0 && timerScroll.running)) return;

        pScroll = 0;

        timerScroll.interval = st.playerBrowser_intervalA;

        timerScroll.restart();

        flashPrevious();
    }

    function scrollNext()
    {
        if (buttonNext.visible == false || (pScroll == 1 && timerScroll.running)) return;

        pScroll = 1;

        timerScroll.interval = st.playerBrowser_intervalA;

        timerScroll.restart();

        flashNext();
    }

    function scrollClear()
    {
        timerScroll.stop();
    }

    //---------------------------------------------------------------------------------------------

    function setPrevious()
    {
        if (playerTab == tab)
        {
            player.setPreviousTrack();
        }
        else tab.setPreviousTrack();
    }

    function setNext()
    {
        if (playerTab == tab)
        {
            player.setNextTrack();
        }
        else tab.setNextTrack();
    }

    //---------------------------------------------------------------------------------------------
    // Events

    function onPrevious()
    {
        setPrevious();
    }

    function onNext()
    {
        setNext();
    }

    //---------------------------------------------------------------------------------------------
    // Private

    function pFlashPrevious()
    {
        pBackward = true;

        timerBackward.restart();
    }

    function pFlashNext()
    {
        pForward = true;

        timerForward.restart();
    }

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

    Timer
    {
        id: timerScroll

        interval: st.playerBrowser_intervalA

        repeat: true

        onTriggered:
        {
            interval = st.playerBrowser_intervalB;

            if (pScroll)
            {
                 flashNext();
            }
            else flashPrevious();
        }
    }

    Timer
    {
        id: timerBackward

        interval: st.playerBrowser_intervalB

        onTriggered: pBackward = false
    }

    Timer
    {
        id: timerForward

        interval: st.playerBrowser_intervalB

        onTriggered: pForward = false
    }

    MouseArea
    {
        id: areaBackward

        anchors.top   : parent.top
        anchors.bottom: parent.bottom

        width: Math.round(parent.width / ratio)

        cursor: Qt.PointingHandCursor

        onPressed : scrollPrevious()
        onReleased: scrollClear   ()

        Behavior on visible
        {
            enabled: (areaBackward.visible)

            PropertyAnimation { duration: st.ms1000 }
        }
    }

    MouseArea
    {
        id: areaForward

        anchors.top   : areaBackward.top
        anchors.bottom: areaBackward.bottom
        anchors.right : parent.right

        width: Math.round(parent.width / ratio)

        cursor: Qt.PointingHandCursor

        onPressed : scrollNext ()
        onReleased: scrollClear()

        Behavior on visible
        {
            enabled: (areaForward.visible)

            PropertyAnimation { duration: st.ms1000 }
        }
    }
}
