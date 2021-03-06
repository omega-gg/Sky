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

Timer
{
    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    /* read */ property int count: 0

    /* read */ property real value: (tempo) ? 60000 / tempo * sp.ratioVelocity : 0.0

    /* read */ property real sum: 0.0

    property int tempo: sp.tempo

    //---------------------------------------------------------------------------------------------
    // Private

    property variant pTime: null

    property int pElapsed: 0

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    interval: sp.timerTempo_interval

    repeat: true

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    onRunningChanged:
    {
        count = 0;

        sum = 0.0;

        if (running)
        {
            // FIXME Qt5.14.2: We must use 'date' instead of 'time' to avoid conversions issues.
            pTime = sk.getDate();

            pElapsed = 0;
        }
    }

    onTriggered:
    {
        var elapsed = sk.getElapsed(pTime);

        sum += elapsed - pElapsed;

        pElapsed = elapsed;

        while (sum >= value)
        {
            sum -= value;

            count++;
        }
    }
}
