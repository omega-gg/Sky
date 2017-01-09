//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkyPresentation module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

import QtQuick 1.1
import Sky     1.0

Player
{
    id: slidePlayer

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property bool standalone: false

    property int timeA:  0
    property int timeB: -1

    property bool autoPlay  : true
    property bool autoRepeat: true

    property bool fade: false

    //---------------------------------------------------------------------------------------------
    // Style

    property int durationAnimation: sp.slide_durationAnimation

    property int easing: Easing.Linear

    //---------------------------------------------------------------------------------------------
    // Private

    property bool pReady : false
    property bool pVolume: false

    //---------------------------------------------------------------------------------------------

    property url pSource

    property int pTimeA: -1
    property int pTimeB: -1

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    backend: BackendVlc {}

    volume: (slides && pVolume) ? slides.volume : 0.0

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    onEnded:
    {
        if (autoRepeat == false) return;

        seekTo(timeA);

        play();
    }

    onCurrentTimeChanged:
    {
        if (autoRepeat)
        {
            if (timeB == -1 || currentTime < timeB) return;

            seekTo(timeA);
        }
        else if (timeB != -1)
        {
            var time = currentTime;

            if (fade)
            {
                if (pVolume == false) return;

                time -= durationAnimation;

                if (time >= timeB)
                {
                    pVolume = false;
                }
            }
            else if (time >= timeB)
            {
                pause();
            }
        }
    }

    //---------------------------------------------------------------------------------------------
    // Connections
    //---------------------------------------------------------------------------------------------

    Connections
    {
        target: (standalone) ? null : slides

        onSlide:
        {
            if (pReady) return;

            pReady = true;

            pPlay();
        }

        onClear: if (fade) pVolume = false
    }

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function playSource(source, timeA /* 0 */, timeB /* -1 */)
    {
        if (fade && volume)
        {
            pVolume = false;

            while (volume)
            {
                sk.processEvents();
            }
        }

        slidePlayer.source = source;

        if (timeA == undefined)
        {
             slidePlayer.timeA = 0;
        }
        else slidePlayer.timeA = timeA;

        if (timeB == undefined)
        {
             slidePlayer.timeB = -1;
        }
        else slidePlayer.timeB = timeB;

        pPlay();
    }

    //---------------------------------------------------------------------------------------------
    // Private

    function pPlay()
    {
        seekTo(timeA);

        play();

        while (isLoading)
        {
            sk.processEvents();
        }

        if (autoPlay == false)
        {
            pause();
        }

        if (fade) pVolume = true;
    }

    //---------------------------------------------------------------------------------------------
    // Animations
    //---------------------------------------------------------------------------------------------

    Behavior on opacity
    {
        PropertyAnimation
        {
            duration: (st.animate) ? durationAnimation : 0

            easing.type: slidePlayer.easing
        }
    }

    Behavior on volume
    {
        SequentialAnimation
        {
            PropertyAnimation
            {
                duration: (st.animate && fade) ? durationAnimation : 0

                easing.type: slidePlayer.easing
            }

            ScriptAction
            {
                script:
                {
                    if (fade && pVolume == false)
                    {
                        slidePlayer.pause();
                    }
                }
            }
        }
    }
}
