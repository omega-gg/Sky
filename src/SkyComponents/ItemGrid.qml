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

SkyMouseArea
{
    id: itemGrid

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property bool isFocused: activeFocus

    property bool isPressed: (pressed && (pressedButtons & Qt.LeftButton))
    property bool isHovered: hoverActive

    property bool isDefault   : false
    property bool isSelected  : false
    property bool isCurrent   : false
    property bool isContextual: false

    property int textMargin: st.dp8

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

    property alias border: border

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

    acceptedButtons: Qt.LeftButton | Qt.RightButton | Qt.MiddleButton

    hoverEnabled: true
    hoverRetain : hoverEnabled

    cursor: Qt.PointingHandCursor

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

        anchors.leftMargin  : borderLeft
        anchors.rightMargin : borderRight
        anchors.topMargin   : borderTop
        anchors.bottomMargin: borderBottom

        color: st.itemGrid_color
    }

    ImageScale
    {
        id: itemImage

        anchors.left  : background.left
        anchors.right : background.right
        anchors.top   : background.top
        anchors.bottom: border.top

        fillMode: SkyImage.PreserveAspectFit
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

        height: st.itemGrid_barHeight

        gradient: Gradient
        {
            GradientStop
            {
                position: 0.0

                color:
                {
                    if (isSelected)
                    {
                        if (isFocused == false)
                        {
                            if (isHovered) return st.itemList_colorSelectHoverA;
                            else           return st.itemList_colorSelectA;
                        }
                        else return st.itemList_colorSelectFocusA;
                    }
                    else if (isCurrent)
                    {
                        if (isHovered) return st.itemList_colorCurrentHoverA;
                        else           return st.itemList_colorCurrentA;
                    }
                    else if (isContextual)
                    {
                        if (isHovered) return st.itemList_colorContextualHoverA;
                        else           return st.itemList_colorHoverA;
                    }
                    else if (isPressed)
                    {
                        return st.itemList_colorPressA;
                    }
                    else if (isHovered)
                    {
                        return st.itemList_colorHoverA;
                    }
                    else if (isDefault)
                    {
                        return st.itemList_colorDefaultA;
                    }
                    else return st.itemList_colorA;
                }
            }

            GradientStop
            {
                position: 1.0

                color:
                {
                    if (isSelected)
                    {
                        if (isFocused == false)
                        {
                            if (isHovered) return st.itemList_colorSelectHoverB;
                            else           return st.itemList_colorSelectB;
                        }
                        else return st.itemList_colorSelectFocusB;
                    }
                    else if (isCurrent)
                    {
                        if (isHovered) return st.itemList_colorCurrentHoverB;
                        else           return st.itemList_colorCurrentB;
                    }
                    else if (isContextual)
                    {
                        if (isHovered) return st.itemList_colorContextualHoverB;
                        else           return st.itemList_colorHoverB;
                    }
                    else if (isPressed)
                    {
                        return st.itemList_colorPressB;
                    }
                    else if (isHovered)
                    {
                        return st.itemList_colorHoverB;
                    }
                    else if (isDefault)
                    {
                        return st.itemList_colorDefaultB;
                    }
                    else return st.itemList_colorB;
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

            color:
            {
                if      (isSelected)                return st.itemList_colorTextSelected;
                else if (isCurrent)                 return st.itemList_colorTextCurrent;
                else if (isHovered || isContextual) return st.itemList_colorTextHover;
                else                                return st.itemList_colorText;
            }

            style:
            {
                if (isSelected)
                {
                    if (isFocused) return st.text_raised;
                    else           return st.text_sunken;
                }
                else if (isCurrent)
                {
                     return st.text_raised;
                }
                else return st.text_sunken;
            }
        }
    }

    RectangleBorders
    {
        id: borders

        anchors.fill: parent

        color:
        {
            if (isCurrent || isSelected)
            {
                if (isHovered) return st.itemTab_colorHighlightA;
                else           return st.itemTab_colorSelectB;
            }
            else if (isHovered) return st.itemTab_colorHoverA;
            else                return st.border_color;
        }
    }
}
