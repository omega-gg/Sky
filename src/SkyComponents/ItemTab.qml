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

MouseArea
{
    id: itemTab

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property bool isHovered: containsMouse
    property bool isCurrent: false

    property int textSpacing: st.dp8
    property int textMargin : st.dp8

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

    property alias iconWidth : itemIcon.width
    property alias iconHeight: itemIcon.height

    property alias iconSourceSize : itemIcon.sourceSize
    property alias iconDefaultSize: itemIcon.defaultSize

    property alias iconSourceArea: itemIcon.sourceArea

    property alias iconLoadMode: itemIcon.loadMode
    property alias iconFillMode: itemIcon.fillMode

    property alias iconAsynchronous: itemIcon.asynchronous
    property alias iconCache       : itemIcon.cache

    property alias iconScaling: itemIcon.scaling

    property alias iconStyle: itemIcon.style

    property alias text: itemText.text
    property alias font: itemText.font

    property alias textColor     : itemText.color
    property alias textStyle     : itemText.style
    property alias textStyleColor: itemText.styleColor

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

    cursor: Qt.PointingHandCursor

    //---------------------------------------------------------------------------------------------
    // Children
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

            width : st.itemTab_iconWidth
            height: st.itemTab_iconHeight

            sourceSize: Qt.size(width, height)
            sourceArea: Qt.size(width, height)

            opacity: (itemTab.enabled) ? 1.0 : st.icon_opacityDisable

            clip: (fillMode == Image.PreserveAspectCrop)

            style: (isCurrent) ? st.icon_raised
                               : st.icon_sunken

            enableFilter: isSourceDefault

            filterDefault: (isCurrent) ? st.icon2_filter
                                       : st.icon1_filter
        }

        TextBase
        {
            id: itemText

            anchors.left  : itemIcon.right
            anchors.right : parent.right
            anchors.top   : parent.top
            anchors.bottom: parent.bottom

            anchors.leftMargin : textSpacing
            anchors.rightMargin: textMargin

            verticalAlignment: Text.AlignVCenter

            opacity: (itemTab.enabled) ? 1.0 : st.text_opacityDisable

            color: (isCurrent) ? st.text2_color
                               : st.text1_color

            style: (isCurrent) ? st.text_raised
                               : st.text_sunken
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
