//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkyPresentation module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

import QtQuick 1.0
import Sky     1.0

Rectangle
{
    id: slideGradient

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------
    // Style

    property int durationAnimation: sp.slide_durationAnimation

    property int easing: Easing.Linear

    property color colorA: sp.slideGradient_colorA
    property color colorB: sp.slideGradient_colorB

    property color colorBack: sp.slideGradient_colorBack

    //---------------------------------------------------------------------------------------------
    // Private

    property bool pReady: false

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    gradient: Gradient
    {
        GradientStop
        {
            position: 0.0

            color: (pReady) ? colorA : colorBack

            Behavior on color
            {
                ColorAnimation
                {
                    duration: (st.animate) ? durationAnimation : 0

                    easing.type: slideGradient.easing
                }
            }
        }

        GradientStop
        {
            position: 1.0

            color: (pReady) ? colorB : colorBack

            Behavior on color
            {
                ColorAnimation
                {
                    duration: (st.animate) ? durationAnimation : 0

                    easing.type: slideGradient.easing
                }
            }
        }
    }

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    Component.onCompleted: pReady = true
}
