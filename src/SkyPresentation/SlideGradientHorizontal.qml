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

Item
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
    // Events
    //---------------------------------------------------------------------------------------------

    Component.onCompleted: pReady = true

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    Rectangle
    {
        anchors.centerIn: parent

        width : parent.height
        height: parent.width

        rotation: 270

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
    }
}
