//=================================================================================================
/*
    Copyright (C) 2015-2016 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkyComponents module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

import QtQuick 1.1
import Sky     1.0

ComponentWall
{
    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property bool isContextualHovered: (index == indexHover && index == indexContextual)

    property bool isHighlighted: (item == highlightedTab)

    property int logoMargin: itemContent.width / logoRatio

    //---------------------------------------------------------------------------------------------
    // Style

    property color colorBarContextualHoverA: st.itemTab_colorContextualHoverA
    property color colorBarContextualHoverB: st.itemTab_colorContextualHoverB

    property color colorHighlightA: st.itemTab_colorHighlightA
    property color colorHighlightB: st.itemTab_colorHighlightB

    property color colorHighlightHoverA: st.itemTab_colorHighlightHoverA
    property color colorHighlightHoverB: st.itemTab_colorHighlightHoverB

    property color colorHighlightContextualA: st.itemTab_colorHighlightContextualA
    property color colorHighlightContextualB: st.itemTab_colorHighlightContextualB

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    iconWidth: st.componentWallBookmarkTrack_iconWidth

    z:
    {
        if      (index == indexTop)      return  3;
        else if (isHighlighted)          return  2;
        else if (isCurrent || isHovered) return  1;
        else if (opacity == 1.0)         return  0;
        else                             return -1;
    }

    isHovered: (index == indexHover || index == indexContextual)

    isCurrent: (item == currentTab)

    image: item.coverShot

    icon       : item.cover
    iconDefault: baseWall.iconDefault

    text:
    {
        if (item.title)
        {
            return item.title;
        }
        else if (item.isLoading)
        {
            return qsTr("Loading...");
        }
        else return qsTr("New Tab");
    }

    iconFillMode: Image.PreserveAspectCrop

    acceptedButtons: Qt.NoButton

    itemImage.anchors.leftMargin: (itemImage.isSourceDefault) ? logoMargin : 0

    itemImage.anchors.rightMargin: itemImage.anchors.leftMargin

    itemImage.sourceDefault: logo

    itemImage.loadMode: (isCurrent) ? Image.LoadVisible
                                    : Image.LoadAlways

    itemImage.cache: false

    itemImage.scaling: itemImage.isSourceDefault

    itemBar.gradient: Gradient
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
                else if (isCurrent)           return colorBarSelectA;
                else if (isContextualHovered) return colorBarContextualHoverA;
                else if (isHovered)           return colorBarHoverA;
                else                          return colorBarA;
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
                else if (isCurrent)           return colorBarSelectB;
                else if (isContextualHovered) return colorBarContextualHoverB;
                else if (isHovered)           return colorBarHoverB;
                else                          return colorBarB;
            }
        }
    }

    //---------------------------------------------------------------------------------------------
    // Style

    gradient: Gradient
    {
        GradientStop
        {
            position: 0.0

            color: (itemImage.isSourceDefault) ? defaultColorA
                                               : st.componentWallBookmarkTrack_colorA
        }

        GradientStop
        {
            position: 1.0

            color: (itemImage.isSourceDefault) ? defaultColorB
                                               : st.componentWallBookmarkTrack_colorB
        }
    }

    colorBorder:
    {
        if (isHighlighted)
        {
            if      (isContextualHovered) return colorBarSelectB;
            else if (isHovered)           return colorBarSelectA;
            else                          return st.border_color;
        }
        else if (isCurrent)           return colorBarSelectB;
        else if (isContextualHovered) return colorBarContextualHoverA;
        else if (isHovered)           return colorBarHoverA;
        else                          return st.border_color;
    }

    textColor: st.text1_color

    textStyle: (isCurrent) ? Text.Raised
                           : Text.Sunken

    iconStyle: (isCurrent) ? Sk.IconRaised
                           : Sk.IconSunken

    textStyleColor: (isCurrent) ? st.text1_colorShadow
                                : st.text1_colorSunken

    filterIcon      : st.icon1_filter
    filterIconShadow: st.icon1_filterShadow

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    TabBarProgress
    {
        anchors.bottom: parent.bottom

        width: (item.currentTime * (parent.width - itemIcon.width)) / item.duration

        x: itemIcon.width

        visible: (isCurrent == false && isHighlighted == false && item.currentTime > 0)

        enabled: player.isPlaying
    }
}
