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
    id: itemGrid

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property bool isFocused: activeFocus

    property bool isPressed: (pressed && (pressedButtons & Qt.LeftButton))
    property bool isHovered: hoverActive

    property bool isCurrent: false

    property int textMargin: st.dp8

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

    property alias text: itemText.text
    property alias font: itemText.font

    property alias textColor     : itemText.color
    property alias textStyle     : itemText.style
    property alias textStyleColor: itemText.styleColor

    //---------------------------------------------------------------------------------------------

    property alias background: background

    property alias itemImage: itemImage

    property alias bar     : bar
    property alias itemText: itemText

    property alias borders: borders

    //---------------------------------------------------------------------------------------------
    // Style

    property alias color   : background.color
    property alias gradient: background.gradient

    property alias colorBorder: borders.color

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    width : st.itemGrid_width
    height: st.itemGrid_height

    hoverEnabled: true
    hoverRetain : hoverEnabled

    cursor: Qt.PointingHandCursor

    color: st.itemGrid_color

    colorBorder:
    {
        if      (isCurrent) return colorBarSelectB;
        else if (isHovered) return colorBarHoverA;
        else                return st.border_color;
    }

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function setFocus() { forceActiveFocus() }

    function getIndex() { return index }

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

    Rectangle
    {
        id: background

        anchors.fill: parent

        color: st.panel_color
    }

    ImageScale
    {
        id: itemImage

        anchors.left  : parent.left
        anchors.right : parent.right
        anchors.top   : parent.top
        anchors.bottom: border.top

        anchors.leftMargin : borderLeft
        anchors.rightMargin: borderRight
        anchors.topMargin  : borderTop

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

        anchors.left  : parent.left
        anchors.right : parent.right
        anchors.bottom: parent.bottom

        anchors.leftMargin  : borderLeft
        anchors.rightMargin : borderRight
        anchors.bottomMargin: borderBottom

        height: st.itemGrid_barHeight

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

        TextBase
        {
            id: itemText

            anchors.fill: parent

            anchors.leftMargin : textMargin
            anchors.rightMargin: textMargin

            verticalAlignment: Text.AlignVCenter

            opacity: (itemGrid.enabled) ? 1.0 : st.text_opacityDisable

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
