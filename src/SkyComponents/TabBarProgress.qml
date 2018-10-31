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

import QtQuick 1.0
import Sky     1.0

Rectangle
{
    //---------------------------------------------------------------------------------------------
    // Properties style
    //---------------------------------------------------------------------------------------------

    property color colorA: st.tabBarProgress_colorA
    property color colorB: st.tabBarProgress_colorB

    property color colorDisableA: st.tabBarProgress_colorDisableA
    property color colorDisableB: st.tabBarProgress_colorDisableB

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    height: st.tabBarProgress_height

    gradient: Gradient
    {
        GradientStop
        {
            position: 0.0

            color: (enabled) ? colorA
                             : colorDisableA
        }

        GradientStop
        {
            position: 1.0

            color: (enabled) ? colorB
                             : colorDisableB
        }
    }

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    BorderHorizontal
    {
        anchors.bottom: parent.top

        color: st.border_colorLight

        visible: (colorA != colorB)
    }
}
