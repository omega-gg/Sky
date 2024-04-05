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
    // Functions
    //---------------------------------------------------------------------------------------------
    // Private

    function pGetBarWidth()
    {
        if (bar.visible == false) return 0;

        var time     = item.currentTime;
        var duration = item.duration;

        if (time < duration)
        {
            return time * (width - itemIcon.width) / duration;
        }
        else return width - itemIcon.x - itemIcon.width;
    }

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

    BarProgress
    {
        id: bar

        anchors.left  : itemIcon.right
        anchors.bottom: parent.bottom

        width: pGetBarWidth()

        visible: (isCurrent == false && isHighlighted == false && item.currentTime > 0)

        enabled: player.isPlaying
    }

    RectangleLive
    {
        anchors.left  : parent.left
        anchors.bottom: parent.bottom

        anchors.leftMargin: iconWidth - width

        trackType: item.type
    }
}
