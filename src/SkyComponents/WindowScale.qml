//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkyComponents.

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

    property int pEasing: Easing.Linear

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
                scaling = true;

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
             pDuration = durationAnimation;
        }
        else pDuration = duration;

        if (easing == undefined)
        {
             pEasing = Easing.Linear;
        }
        else pEasing = easing;

        if (zoom == scale)
        {
            if (realTime) pTime = sk.getTime();

            timer.restart();
        }
        else if (zoomAfter)
        {
            pScale = scale;
            pRatio = scale / zoom;

            pAnimate = true;

            scaling = false;

            window.scale = pRatio;

            originX = pX * scale;
            originY = pY * scale;

            pAnimate = false;
        }
        else
        {
            if (realTime) pTime = sk.getTime();

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
        pEasing   = Easing.Linear;

        pAnimate = animate;

        originX = x;

        pAnimate = false;
    }

    function moveOriginY(y, animate)
    {
        pDuration = durationAnimation;
        pEasing   = Easing.Linear;

        pAnimate = animate;

        originY = y;

        pAnimate = false;
    }

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    Timer
    {
        id: timer

        interval: 1

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

            scaling = false;

            scale = 1.0;

            originX = pX * zoom;
            originY = pY * zoom;

            pAnimate = false;
        }
    }
}
