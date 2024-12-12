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
    id: componentGridTrack

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property bool isContextualHovered: (index == view.indexHover
                                        &&
                                        index == view.indexContextual)

    property bool isSourceDefault: itemImage.isSourceDefault

    /* read */ property int time    : -1
    /* read */ property int duration: -1

    property int logoMargin: background.width / view.logoRatio

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
    // Private

    // NOTE: This is required for the onPSourceChanged event.
    property string pSource: source

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    onPSourceChanged: pUpdateTime()

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    isHovered: (index == view.indexHover || index == view.indexContextual)

    spacing: view.spacingBottom

    //isCurrent: (index == indexPlayer)

    image: cover

    text: st.getTrackTitle(title, loadState, source)

    acceptedButtons: Qt.LeftButton | Qt.RightButton | Qt.MiddleButton

    itemImage.anchors.leftMargin: (isSourceDefault) ? logoMargin : 0

    itemImage.anchors.rightMargin: itemImage.anchors.leftMargin

    itemImage.sourceDefault: view.logo

    itemImage.fillMode: Image.PreserveAspectFit

    itemImage.scaling: isSourceDefault

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

            color: (isSourceDefault) ? view.defaultColorA
                                     : st.itemGrid_color
        }

        GradientStop
        {
            position: 1.0

            color: (isSourceDefault) ? view.defaultColorB
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

    function pUpdateTime()
    {
        duration = playlist.trackDuration(index);

        if (duration < 1)
        {
             componentGridTrack.time = -1;
        }

        var time = controllerPlaylist.extractTime(source);

        if (time == 0 || time > duration)
        {
             componentGridTrack.time = -1;
        }
        else componentGridTrack.time = time;
    }

    function pGetBarWidth()
    {
        if (barProgress.visible == false) return 0;

        if (time < duration)
        {
            return time * background.width / duration;
        }
        else return width;
    }

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

    BarProgress
    {
        id: barProgress

        anchors.left  : parent.left
        anchors.bottom: parent.bottom

        anchors.leftMargin  : borderLeft
        anchors.bottomMargin: spacing + borderBottom

        width: pGetBarWidth()

        visible: (time > 0)
    }

    RectangleLive
    {
        anchors.right : parent.right
        anchors.bottom: barProgress.bottom

        anchors.rightMargin : borderRight
        anchors.bottomMargin: componentGridTrack.border.size + bar.height

        trackType: type
    }
}
