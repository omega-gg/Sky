//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkyPresentation.

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

Player
{
    id: slidePlayer

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property bool standalone: false

    property int timeA:  0
    property int timeB: -1

    property bool autoPlay  : st.animate
    property bool autoRepeat: true

    property bool fade: false

    //---------------------------------------------------------------------------------------------
    // Style

    property int durationAnimation: sp.slide_durationAnimation

    property int durationFadeIn : sp.slidePlayer_durationFadeIn
    property int durationFadeOut: sp.slidePlayer_durationFadeOut

    property int easing: sp.easing

    //---------------------------------------------------------------------------------------------
    // Private

    property bool pReady : false
    property bool pVolume: false

    property int pDuration: 0

    //---------------------------------------------------------------------------------------------

    property url pSource

    property int pTimeA: -1
    property int pTimeB: -1

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    backend: BackendVlc {}

    volume: (fade == false || (slides && pVolume)) ? slides.volume : 0.0

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    onEnded: if (autoRepeat) pPlay()

    onSourceChanged:
    {
        if (pReady == false) return;

        stop();

        pPlay();
    }

    onTimeAChanged: if (pReady) seek(timeA)

    // NOTE: This is useful if we want to change the fade duration during playback.
    onDurationFadeInChanged: pDuration = durationFadeIn

    onCurrentTimeChanged:
    {
        if (autoRepeat)
        {
            if (timeB == -1 || currentTime < timeB) return;

            seek(timeA);
        }
        else if (timeB != -1)
        {
            var time = currentTime;

            if (fade)
            {
                if (pVolume == false) return;

                time -= durationFadeOut;

                if (time >= timeB)
                {
                    pApplyVolume(false);
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

        onClear: if (fade) pApplyVolume(false)

        onPlay: togglePlay()

        onBackward: seek(Math.max(0, currentTime - sp.slidePlayer_interval))

        onForward: seek(Math.min(currentTime + sp.slidePlayer_interval, duration))
    }

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function playSource(source, at /* 0 */, duration /* -1 */)
    {
        pFadeOut();

        slidePlayer.source = source;

        if (at == undefined) at = 0;

        timeA = at;

        if (duration == undefined || duration == -1)
        {
             timeB = -1;
        }
        else timeB = at + duration;

        pPlay();
    }

    function stopSource()
    {
        pFadeOut();

        stop();
    }

    //---------------------------------------------------------------------------------------------
    // Private

    function pPlay()
    {
        seek(timeA);

        play();

        if (autoPlay == false)
        {
            pause();
        }

        while (isLoading)
        {
            sk.processEvents();
        }

        if (fade) pApplyVolume(true);
    }

    //---------------------------------------------------------------------------------------------

    function pFadeOut()
    {
        if (fade && volume)
        {
            pApplyVolume(false);

            while (volume)
            {
                sk.processEvents();
            }
        }
    }

    function pApplyVolume(volume)
    {
        if (pVolume == volume) return;

        if (volume)
        {
             pDuration = durationFadeIn;
        }
        else pDuration = durationFadeOut;

        pVolume = volume;
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
                duration: (st.animate && fade) ? pDuration : 0

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
