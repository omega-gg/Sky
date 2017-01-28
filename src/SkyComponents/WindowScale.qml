//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkyComponents module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

import QtQuick 1.1
import Sky     1.0

Window
{
    id: window

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property bool realTime: false

    //---------------------------------------------------------------------------------------------
    // Private

    property variant pTime

    property real pScale: -1
    property real pRatio: -1

    property int pX: -1
    property int pY: -1

    property int pDuration: durationAnimation

    property int pDurationAnimation: (st.animate) ? pDuration : 0

    property int pEasing: -1

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    transformOrigin: Item.TopLeft

    //---------------------------------------------------------------------------------------------
    // Animations
    //---------------------------------------------------------------------------------------------

    Behavior on scale
    {
        enabled: pAnimate

        PropertyAnimation
        {
            duration: pDurationAnimation

            easing.type: pEasing

            onRunningChanged:
            {
                if (scale != pRatio) return;

                pRatio = -1;

                var animate = st.animate;

                var hover = hoverEnabled;

                st.animate = false;

                hoverEnabled = false;

                zoom = pScale;

                scale = 1.0;

                hoverEnabled = hover;

                st.animate = animate;
            }
        }
    }

    Behavior on originX
    {
        enabled: pAnimate

        PropertyAnimation { duration: pDurationAnimation; easing.type: pEasing }
    }

    Behavior on originY
    {
        enabled: pAnimate

        PropertyAnimation { duration: pDurationAnimation; easing.type: pEasing }
    }

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function zoomTo(scale, x, y, duration /* 0 */,
                                 easing   /* Easing.Linear */, zoomAfter /* false */)
    {
        if (window.scale != 1.0 || (zoom == scale && pX == x && pY == y)) return;

        pX = x;
        pY = y;

        if (duration == undefined)
        {
             pDuration = st.duration_fast;
        }
        else pDuration = durationAnimation;

        if (easing == undefined)
        {
             pEasing = Easing.Linear;
        }
        else pEasing = easing;

        if (zoom == scale)
        {
            if (realTime)
            {
                pTime = sk.getTime();
            }

            timer.restart();
        }
        else if (zoomAfter)
        {
            pScale = scale;
            pRatio = scale / zoom;

            pAnimate = true;

            window.scale = pRatio;

            originX = pX * scale;
            originY = pY * scale;

            pAnimate = false;
        }
        else
        {
            if (realTime)
            {
                pTime = sk.getTime();
            }

            pRatio = -1;

            var animate = st.animate;

            var hover = hoverEnabled;

            var z = zoom;

            st.animate = false;

            hoverEnabled = false;

            zoom = scale;

            window.scale = z * (1.0 / scale);

            hoverEnabled = hover;

            st.animate = animate;

            timer.restart();
        }
    }

    //---------------------------------------------------------------------------------------------

    function moveOriginX(x, animate)
    {
        pDuration = durationAnimation;

        pAnimate = animate;

        window.originX = x;

        pAnimate = false;
    }

    function moveOriginY(y, animate)
    {
        pDuration = durationAnimation;

        pAnimate = animate;

        window.originY = y;

        pAnimate = false;
    }

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    Timer
    {
        id: timer

        interval: st.ms1

        onTriggered:
        {
            if (realTime && pDuration)
            {
                var elapsed = sk.getElapsed(pTime);

                if (pDuration > elapsed)
                {
                     pDuration -= elapsed;
                }
                else pDuration = 0;
            }

            pAnimate = true;

            scale = 1.0;

            originX = pX * zoom;
            originY = pY * zoom;

            pAnimate = false;
        }
    }
}
