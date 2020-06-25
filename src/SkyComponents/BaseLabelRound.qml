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

Item
{
    id: baseLabelRound

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property int margins: height / 8

    property int radius: background.height

    property int borderSize: st.border_size

    //---------------------------------------------------------------------------------------------
    // Style

    property color colorA: st.labelRound_colorA
    property color colorB: st.labelRound_colorB

    property color colorDisableA: st.labelRound_colorDisableA
    property color colorDisableB: st.labelRound_colorDisableB

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias background: background

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    Rectangle
    {
        id: background

        anchors.fill: parent

        anchors.margins: margins

        radius: baseLabelRound.radius

        gradient: Gradient
        {
            GradientStop
            {
                position: 0.0

                color: (baseLabelRound.enabled) ? colorA
                                                : colorDisableA
            }

            GradientStop
            {
                position: 1.0

                color: (baseLabelRound.enabled) ? colorB
                                                : colorDisableB
            }
        }

//#QT_4
        smooth: true
//#END

        border.width: borderSize
        border.color: st.border_color
    }
}
