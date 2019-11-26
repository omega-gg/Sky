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

ComponentTabBrowser
{
    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property bool isHighlighted: (item == highlightedTab)

    //---------------------------------------------------------------------------------------------
    // Style

    property color colorHighlightA: st.itemTab_colorHighlightA
    property color colorHighlightB: st.itemTab_colorHighlightB

    property color colorHighlightHoverA: st.itemTab_colorHighlightHoverA
    property color colorHighlightHoverB: st.itemTab_colorHighlightHoverB

    property color colorHighlightContextualA: st.itemTab_colorHighlightContextualA
    property color colorHighlightContextualB: st.itemTab_colorHighlightContextualB

    //---------------------------------------------------------------------------------------------
    // Private

    property bool pCurrent: (isCurrent || isHighlighted)

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    textColor: (pCurrent) ? st.text2_color
                          : st.text1_color

    textStyle: (isCurrent) ? st.text_raised
                           : st.text_sunken

    iconStyle: (isCurrent) ? st.icon_raised
                           : st.icon_sunken

    textStyleColor: (isCurrent) ? st.text1_colorShadow
                                : st.text1_colorSunken

    filterIcon: (pCurrent) ? st.icon2_filter
                           : st.icon1_filter

    filterIconShadow: st.icon1_filterShadow

    background.gradient: Gradient
    {
        GradientStop
        {
            position: 0.0

            color:
            {
                if (isHighlighted)
                {
                    if      (isContextualHovered) return colorHighlightContextualA;
                    else if (isHovered)           return colorHighlightHoverA;
                    else                          return colorHighlightA;
                }
                else if (isCurrent)           return colorSelectA;
                else if (isContextualHovered) return colorContextualHoverA;
                else if (isHovered)           return colorHoverA;
                else                          return colorA;
            }
        }

        GradientStop
        {
            position: 1.0

            color:
            {
                if (isHighlighted)
                {
                    if      (isContextualHovered) return colorHighlightContextualB;
                    else if (isHovered)           return colorHighlightHoverB;
                    else                          return colorHighlightB;
                }
                else if (isCurrent)           return colorSelectB;
                else if (isContextualHovered) return colorContextualHoverB;
                else if (isHovered)           return colorHoverB;
                else                          return colorB;
            }
        }
    }

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    TabBarProgress
    {
        anchors.left  : itemIcon.right
        anchors.bottom: parent.bottom

        width: (item) ? item.currentTime * (parent.width - itemIcon.width) / item.duration : 0

        visible: (isCurrent == false && isHighlighted == false && item.currentTime > 0)

        enabled: player.isPlaying
    }
}
