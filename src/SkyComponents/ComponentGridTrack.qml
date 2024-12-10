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

ComponentGrid
{
    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

//#QT_4
    property bool isContextualHovered: (index == indexHover && index == indexContextual)

    property int logoMargin: itemContent.width / logoRatio
//#ELSE
    property bool isContextualHovered: (index == parent.indexHover
                                        &&
                                        index == parent.indexContextual)

    property int logoMargin: itemContent.width / parent.logoRatio
//#END

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

//#QT_4
    isHovered: (index == indexHover || index == indexContextual)
//#ELSE
    isHovered: (index == parent.indexHover || index == parent.indexContextual)
//#END

    isCurrent: (index == indexPlayer)

    image: item.cover

    text: st.getTrackTitle(title, loadState, source)

    acceptedButtons: Qt.LeftButton | Qt.RightButton | Qt.MiddleButton

    itemImage.anchors.leftMargin: (itemImage.isSourceDefault) ? logoMargin : 0

    itemImage.anchors.rightMargin: itemImage.anchors.leftMargin

//#QT_4
    itemImage.sourceDefault: logo
//#ELSE
    itemImage.sourceDefault: parent.logo
//#END

    itemImage.fillMode: Image.PreserveAspectFit

    itemImage.scaling: itemImage.isSourceDefault

    bar.gradient: Gradient
    {
        GradientStop
        {
            position: 0.0

            color:
            {
                if      (isCurrent)           return colorBarSelectA;
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
                if      (isCurrent)           return colorBarSelectB;
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

//#QT_4
            color: (itemImage.isSourceDefault) ? defaultColorA
//#ELSE
            color: (itemImage.isSourceDefault) ? parent.defaultColorA
//#END
                                               : st.itemGrid_color
        }

        GradientStop
        {
            position: 1.0

//#QT_4
            color: (itemImage.isSourceDefault) ? defaultColorB
//#ELSE
            color: (itemImage.isSourceDefault) ? parent.defaultColorB
//#END
                                               : st.itemGrid_color
        }
    }

    colorBorder:
    {
        if      (isCurrent)           return colorBarSelectB;
        else if (isContextualHovered) return colorBarContextualHoverA;
        else if (isHovered)           return colorBarHoverA;
        else                          return st.border_color;
    }

    textColor: (isCurrent) ? st.text2_color
                           : st.text1_color

    textStyle: (isCurrent) ? st.text_raised
                           : st.text_sunken

    textStyleColor: (isCurrent) ? st.text1_colorShadow
                                : st.text1_colorSunken

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

        anchors.bottom: parent.bottom

        width: pGetBarWidth()

        x: itemIcon.width

        visible: (isCurrent == false && item.currentTime > 0)

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
