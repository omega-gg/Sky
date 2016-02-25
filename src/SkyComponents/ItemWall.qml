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

Panel
{
    id: itemWall

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property bool isHovered: containsMouse
    property bool isCurrent: false

    //---------------------------------------------------------------------------------------------
    // Style

    property color colorBarA: st.itemTab_colorA
    property color colorBarB: st.itemTab_colorB

    property color colorBarHoverA: st.itemTab_colorHoverA
    property color colorBarHoverB: st.itemTab_colorHoverB

    property color colorBarSelectA: st.itemTab_colorSelectA
    property color colorBarSelectB: st.itemTab_colorSelectB

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias isIconDefault: itemIcon.isSourceDefault

    property alias image: itemImage.source

    property alias icon       : itemIcon.source
    property alias iconDefault: itemIcon.sourceDefault

    property alias iconWidth : itemIcon.iconWidth
    property alias iconHeight: itemIcon.iconHeight

    property alias iconSourceSize: itemIcon.sourceSize
    property alias iconSourceArea: itemIcon.sourceArea

    property alias iconLoadMode: itemIcon.loadMode
    property alias iconFillMode: itemIcon.fillMode

    property alias iconAsynchronous: itemIcon.asynchronous
    property alias iconCache       : itemIcon.cache

    property alias iconScaling: itemIcon.scaling

    property alias iconStyle: itemIcon.iconStyle

    property alias text: itemText.text
    property alias font: itemText.font

    property alias textColor     : itemText.color
    property alias textStyle     : itemText.style
    property alias textStyleColor: itemText.styleColor

    property alias textSpacing: itemText.leftMargin
    property alias textMargin : itemText.rightMargin

    //---------------------------------------------------------------------------------------------

    property alias itemImage: itemImage

    property alias itemBar: bar

    property alias itemIcon: itemIcon
    property alias itemText: itemText

    //---------------------------------------------------------------------------------------------
    // Style

    property alias enableFilter: itemIcon.enableFilter

    property alias filterIcon      : itemIcon.filterDefault
    property alias filterIconShadow: itemIcon.filterShadow
    property alias filterIconSunken: itemIcon.filterSunken

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    width : st.itemWall_width
    height: st.itemWall_height

    cursor: MouseArea.PointingHandCursor

    color: st.border_color

    colorBorder:
    {
        if      (isCurrent) return colorBarSelectB;
        else if (isHovered) return colorBarHoverA;
        else                return st.border_color;
    }

    //---------------------------------------------------------------------------------------------
    // Animations
    //---------------------------------------------------------------------------------------------

    Behavior on x
    {
        enabled: (isAnimated && index != indexStatic)

        PropertyAnimation { duration: durationAnimation }
    }

    Behavior on y
    {
        enabled: (isAnimated && index != indexStatic)

        PropertyAnimation { duration: durationAnimation }
    }

    //---------------------------------------------------------------------------------------------

    Behavior on width
    {
        enabled: (isAnimated && index != indexStatic)

        PropertyAnimation { duration: durationAnimation }
    }

    Behavior on height
    {
        enabled: (isAnimated && index != indexStatic)

        PropertyAnimation { duration: durationAnimation }
    }

    //---------------------------------------------------------------------------------------------

    Behavior on opacity
    {
        enabled: (isAnimated && index != indexStatic)

        PropertyAnimation { duration: durationAnimation }
    }

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function getIndex() { return index; }

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    ImageScale
    {
        id: itemImage

        anchors.left : parent.left
        anchors.right: parent.right

        anchors.top   : parent.top
        anchors.bottom: border.top

        fillMode: Image.PreserveAspectFit
    }

    BorderHorizontal
    {
        id: border

        anchors.bottom: bar.top

        color: colorBorder
    }

    Rectangle
    {
        id: bar

        anchors.left : parent.left
        anchors.right: parent.right

        anchors.bottom: parent.bottom

        height: st.itemWall_barHeight

        gradient: Gradient
        {
            GradientStop
            {
                position: 0.0

                color:
                {
                    if      (isCurrent) return colorBarSelectA;
                    else if (isHovered) return colorBarHoverA;
                    else                return colorBarA;
                }
            }

            GradientStop
            {
                position: 1.0

                color:
                {
                    if      (isCurrent) return colorBarSelectB;
                    else if (isHovered) return colorBarHoverB;
                    else                return colorBarB;
                }
            }
        }

        Icon
        {
            id: itemIcon

            iconWidth : st.itemWall_iconWidth
            iconHeight: st.itemWall_iconHeight

            sourceSize.height: iconHeight

            opacity: (itemWall.enabled) ? 1 : st.icon_opacityDisable

            clip: (fillMode == Image.PreserveAspectCrop)

            iconStyle: (isCurrent) ? Sk.IconRaised
                                   : Sk.IconSunken

            enableFilter: isSourceDefault
        }

        TextBase
        {
            id: itemText

            anchors.left : itemIcon.right
            anchors.right: parent.right

            anchors.top   : parent.top
            anchors.bottom: parent.bottom

            leftMargin : st.dp8
            rightMargin: st.dp8

            verticalAlignment: Text.AlignVCenter

            opacity: (itemWall.enabled) ? 1 : st.text_opacityDisable

            style: (isCurrent) ? Text.Raised
                               : Text.Sunken
        }
    }
}
