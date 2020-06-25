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

Animated
{
    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property real duration: 0.0

    //---------------------------------------------------------------------------------------------
    // Private

    property variant pTime: null

    property int pElapsed: 0

    property real pExtra: 0.0

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    running: false

    interval: 0

    //---------------------------------------------------------------------------------------------
    // Functions events
    //---------------------------------------------------------------------------------------------

    function onStep() {}

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    onRunningChanged:
    {
        if (running == false) return;

        pTime = sk.getTime();

        pElapsed = 0;

        pExtra = 0.0;

        interval = 0;
    }

    onStepDirectionChanged: if (step != -1) onStep()

    onStepChanged:
    {
        onStep();

        var msec  = Math.floor(duration);
        var extra = Math.floor(pExtra);

        var elapsed = sk.getElapsed(pTime);

        pExtra += (duration - (msec + extra)) - (elapsed - (pElapsed + interval));

        pElapsed = elapsed;

        msec += extra;

        if (msec < 0)
        {
            pExtra += msec;

            msec = 0;
        }

        interval = msec;
    }
}
