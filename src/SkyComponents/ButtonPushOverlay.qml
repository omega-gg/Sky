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

ButtonPushIcon
{
    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    opacity: (isHovered || isPressed) ? st.buttonOverlay_opacityHover
                                      : st.buttonOverlay_opacityDefault

    //---------------------------------------------------------------------------------------------
    // Style

    colorA: (highlighted) ? st.buttonPush_colorHighlightA
                          : st.buttonOverlay_colorA

    colorB: (highlighted) ? st.buttonPush_colorHighlightB
                          : st.buttonOverlay_colorB

    colorHoverA: (highlighted) ? st.buttonPush_colorHighlightHoverA
                               : st.buttonOverlay_colorHoverA

    colorHoverB: (highlighted) ? st.buttonPush_colorHighlightHoverB
                               : st.buttonOverlay_colorHoverB

    colorPressA: (highlighted || checkable) ? st.buttonPush_colorCheckA
                                            : st.buttonOverlay_colorPressA

    colorPressB: (highlighted || checkable) ? st.buttonPush_colorCheckB
                                            : st.buttonOverlay_colorPressB

    colorPressHoverA: (highlighted || checkable) ? st.buttonPush_colorCheckHoverA
                                                 : st.buttonOverlay_colorPressA

    colorPressHoverB: (highlighted || checkable) ? st.buttonPush_colorCheckHoverB
                                                 : st.buttonOverlay_colorPressB

    background.border.color: st.buttonOverlay_colorBorderA

    //---------------------------------------------------------------------------------------------
    // Animations
    //---------------------------------------------------------------------------------------------

    Behavior on opacity
    {
        PropertyAnimation { duration: st.duration_faster }
    }
}
