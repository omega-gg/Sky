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
    id: itemWall

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property bool isFocused: activeFocus
    property bool isHovered: hoverActive

    property bool isCurrent: false

    property int textSpacing: st.dp8
    property int textMargin : st.dp8

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

    property alias borderSize : borders.size
    property alias borderColor: borders.color

    property alias borderLeft  : borders.borderLeft
    property alias borderRight : borders.borderRight
    property alias borderTop   : borders.borderTop
    property alias borderBottom: borders.borderBottom

    property alias borderSizeWidth : borders.sizeWidth
    property alias borderSizeHeight: borders.sizeHeight

    property alias backgroundOpacity: background.opacity

    property alias image: itemImage.source

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

    property alias itemImage: itemImage

    property alias border: border

    property alias bar     : bar
    property alias itemIcon: itemIcon
    property alias itemText: itemText

    property alias borders: borders

    //---------------------------------------------------------------------------------------------
    // Style

    property alias color   : background.color
    property alias gradient: background.gradient

    property alias colorBorder: borders.color

    property alias enableFilter: itemIcon.enableFilter

    property alias filterIcon      : itemIcon.filterDefault
    property alias filterIconShadow: itemIcon.filterShadow
    property alias filterIconSunken: itemIcon.filterSunken

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    width : st.itemWall_width
    height: st.itemWall_height

    acceptedButtons: Qt.LeftButton | Qt.RightButton | Qt.MiddleButton

    hoverEnabled: true
    hoverRetain : hoverEnabled

    wheelEnabled: hoverEnabled

    cursor: Qt.PointingHandCursor

    color: st.wallVideo_colorPlayer

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
//#QT_4
        enabled: (isAnimated && index != indexStatic)

        PropertyAnimation
        {
            duration: durationAnimation

            easing.type: st.easing
        }
//#ELSE
        enabled: (parent.isAnimated && index != parent.indexStatic)

        PropertyAnimation
        {
            duration: parent.durationAnimation

            easing.type: st.easing
        }
//#END
    }

    Behavior on y
    {
//#QT_4
        enabled: (isAnimated && index != indexStatic)

        PropertyAnimation
        {
            duration: durationAnimation

            easing.type: st.easing
        }
//#ELSE
        enabled: (parent.isAnimated && index != parent.indexStatic)

        PropertyAnimation
        {
            duration: parent.durationAnimation

            easing.type: st.easing
        }
//#END
    }

    //---------------------------------------------------------------------------------------------

    Behavior on width
    {
//#QT_4
        enabled: (isAnimated && index != indexStatic)

        PropertyAnimation
        {
            duration: durationAnimation

            easing.type: st.easing
        }
//#ELSE
        enabled: (parent.isAnimated && index != parent.indexStatic)

        PropertyAnimation
        {
            duration: parent.durationAnimation

            easing.type: st.easing
        }
//#END
    }

    Behavior on height
    {
//#QT_4
        enabled: (isAnimated && index != indexStatic)

        PropertyAnimation
        {
            duration: durationAnimation

            easing.type: st.easing
        }
//#ELSE
        enabled: (parent.isAnimated && index != parent.indexStatic)

        PropertyAnimation
        {
            duration: parent.durationAnimation

            easing.type: st.easing
        }
//#END
    }

    //---------------------------------------------------------------------------------------------

    Behavior on opacity
    {
//#QT_4
        enabled: (isAnimated && index != indexStatic)

        PropertyAnimation
        {
            duration: durationAnimation

            easing.type: st.easing
        }
//#ELSE
        enabled: (parent.isAnimated && index != parent.indexStatic)

        PropertyAnimation
        {
            duration: parent.durationAnimation

            easing.type: st.easing
        }
//#END
    }

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function getIndex() { return index; }

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

    Rectangle
    {
        id: background

        anchors.left  : parent.left
        anchors.right : parent.right
        anchors.top   : parent.top
        anchors.bottom: borders.bottom

        anchors.leftMargin  : borderLeft
        anchors.rightMargin : borderRight
        anchors.topMargin   : borderTop
        anchors.bottomMargin: borderBottom

        color: st.panel_color
    }

    ImageScale
    {
        id: itemImage

        anchors.left  : background.left
        anchors.right : background.right
        anchors.top   : background.top
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

        anchors.left  : background.left
        anchors.right : background.right
        anchors.bottom: background.bottom

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

            width : st.itemWall_iconWidth
            height: st.itemWall_iconHeight

            sourceSize: Qt.size(width, height)
            sourceArea: Qt.size(width, height)

            opacity: (itemWall.enabled) ? 1.0 : st.icon_opacityDisable

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

            opacity: (itemWall.enabled) ? 1.0 : st.text_opacityDisable

            color: (isCurrent) ? st.text2_color
                               : st.text1_color

            style: (isCurrent) ? st.text_raised
                               : st.text_sunken
        }
    }

    RectangleBorders
    {
        id: borders

        anchors.fill: parent

        color: st.border_color
    }
}
