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

MouseArea
{
    id: itemTab

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property bool isHovered: containsMouse
    property bool isCurrent: false

    //---------------------------------------------------------------------------------------------
    // Style

    property color colorA: st.itemTab_colorA
    property color colorB: st.itemTab_colorB

    property color colorHoverA: st.itemTab_colorHoverA
    property color colorHoverB: st.itemTab_colorHoverB

    property color colorSelectA: st.itemTab_colorSelectA
    property color colorSelectB: st.itemTab_colorSelectB

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    default property alias content: background.data

    property alias isIconDefault: itemIcon.isSourceDefault

    property alias borderLeft : lineLeft .size
    property alias borderRight: lineRight.size

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

    property alias background: background

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

    height: st.itemTab_height

    hoverEnabled: true
    hoverRetain : hoverEnabled

    cursor: MouseArea.PointingHandCursor

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    Rectangle
    {
        id: background

        anchors.left  : lineLeft.right
        anchors.right : lineRight.left
        anchors.top   : parent.top
        anchors.bottom: parent.bottom

        gradient: Gradient
        {
            GradientStop
            {
                position: 0.0

                color:
                {
                    if      (isCurrent) return colorSelectA;
                    else if (isHovered) return colorHoverA;
                    else                return colorA;
                }
            }

            GradientStop
            {
                position: 1.0

                color:
                {
                    if      (isCurrent) return colorSelectB;
                    else if (isHovered) return colorHoverB;
                    else                return colorB;
                }
            }
        }

        Icon
        {
            id: itemIcon

            anchors.verticalCenter: parent.verticalCenter

            iconWidth : st.itemTab_iconWidth
            iconHeight: st.itemTab_iconHeight

            sourceSize: Qt.size(iconWidth, iconHeight)

            opacity: (itemTab.enabled) ? 1.0 : st.icon_opacityDisable

            clip: (fillMode == Image.PreserveAspectCrop)

            iconStyle: (isCurrent) ? Sk.IconRaised
                                   : Sk.IconSunken

            enableFilter: isSourceDefault
        }

        TextBase
        {
            id: itemText

            anchors.left  : itemIcon.right
            anchors.right : parent.right
            anchors.top   : parent.top
            anchors.bottom: parent.bottom

            leftMargin : st.dp8
            rightMargin: st.dp8

            verticalAlignment: Text.AlignVCenter

            opacity: (itemTab.enabled) ? 1.0 : st.text_opacityDisable

            style: (isCurrent) ? Text.Raised
                               : Text.Sunken
        }
    }

    LineVertical
    {
        id: lineLeft

        anchors.left: parent.left

        size: st.itemTab_borderSize
    }

    LineVertical
    {
        id: lineRight

        anchors.right: parent.right

        size: st.itemTab_borderSize
    }
}
