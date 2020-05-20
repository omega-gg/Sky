//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of Sky kit.

    - GNU Lesser General Public License Usage:
    This file may be used under the terms of the GNU Lesser General Public License version 3 as
    published by the Free Software Foundation and appearing in the LICENSE.md file included in the
    packaging of this file. Please review the following information to ensure the GNU Lesser
    General Public License requirements will be met: https://www.gnu.org/licenses/lgpl.html.
*/
//=================================================================================================

import QtQuick 1.0
import Sky     1.0

ButtonPushIcon
{
    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    opacity: (isHovered || isPressed) ? st.buttonPushOverlay_opacityHover
                                      : st.buttonPushOverlay_opacityDefault

    //---------------------------------------------------------------------------------------------
    // Style

    colorA: (highlighted) ? st.buttonPush_colorHighlightA
                          : st.buttonPushOverlay_colorA

    colorB: (highlighted) ? st.buttonPush_colorHighlightB
                          : st.buttonPushOverlay_colorB

    colorHoverA: (highlighted) ? st.buttonPush_colorHighlightHoverA
                               : st.buttonPushOverlay_colorHoverA

    colorHoverB: (highlighted) ? st.buttonPush_colorHighlightHoverB
                               : st.buttonPushOverlay_colorHoverB

    colorPressA: (pHighlighted) ? st.buttonPush_colorCheckA
                                : st.buttonPushOverlay_colorPressA

    colorPressB: (pHighlighted) ? st.buttonPush_colorCheckB
                                : st.buttonPushOverlay_colorPressB

    colorPressHoverA: (pHighlighted) ? st.buttonPush_colorCheckHoverA
                                     : st.buttonPushOverlay_colorPressA

    colorPressHoverB: (pHighlighted) ? st.buttonPush_colorCheckHoverB
                                     : st.buttonPushOverlay_colorPressB

    filterIcon: st.buttonPushOverlay_filterIcon

    background.border.color: st.buttonPushOverlay_colorBorder

    //---------------------------------------------------------------------------------------------
    // Animations
    //---------------------------------------------------------------------------------------------

    Behavior on opacity
    {
        PropertyAnimation
        {
            duration: st.duration_faster

            easing.type: st.easing
        }
    }
}
