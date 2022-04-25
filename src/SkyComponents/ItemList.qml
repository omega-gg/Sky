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
    //-----------------------------------------------------------------------------------------
    // Properties
    //-----------------------------------------------------------------------------------------

    property bool isEnabled: true

    property bool isFocused: activeFocus

    property bool isPressed: (pressed && (pressedButtons & Qt.LeftButton))
    property bool isHovered: hoverActive

    property bool isDefault   : false
    property bool isSelected  : false
    property bool isCurrent   : false
    property bool isContextual: false

    property int textSpacing: st.dp8
    property int textMargin : st.dp8

    //-----------------------------------------------------------------------------------------
    // Aliases
    //-----------------------------------------------------------------------------------------

    property alias isIconDefault: itemIcon.isSourceDefault

    property alias borderSize: border.size

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

    //-----------------------------------------------------------------------------------------

    property alias background: background

    property alias itemIcon: itemIcon
    property alias itemText: itemText

    //---------------------------------------------------------------------------------------------
    // Style

    property alias enableFilter: itemIcon.enableFilter

    property alias filterIcon      : itemIcon.filterDefault
    property alias filterIconShadow: itemIcon.filterShadow
    property alias filterIconSunken: itemIcon.filterSunken

    //-----------------------------------------------------------------------------------------
    // Settings
    //-----------------------------------------------------------------------------------------

    anchors.left : parent.left
    anchors.right: parent.right

    height: st.itemList_height

    hoverEnabled: true
    hoverRetain : hoverEnabled

    cursor: Qt.PointingHandCursor

    //-----------------------------------------------------------------------------------------
    // Functions
    //-----------------------------------------------------------------------------------------

    function setFocus() { return forceActiveFocus(); }

    //-----------------------------------------------------------------------------------------
    // Children
    //-----------------------------------------------------------------------------------------

    Rectangle
    {
        id: background

        anchors.left  : parent.left
        anchors.right : parent.right
        anchors.top   : parent.top
        anchors.bottom: border.top

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

        Icon
        {
            id: itemIcon

            anchors.verticalCenter: parent.verticalCenter

            width : st.itemList_iconWidth
            height: st.itemList_iconHeight

            sourceSize: Qt.size(width, height)
            sourceArea: Qt.size(width, height)

            clip: (fillMode == Image.PreserveAspectCrop)

            style: (itemText.style == st.text_raised) ? st.icon_raised
                                                      : st.icon_sunken

            enableFilter: isSourceDefault

            filterDefault:
            {
                if      (isSelected) return st.icon2_filter;
                else if (isCurrent)  return st.icon_filterActive;
                else                 return st.icon1_filter;
            }
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

            opacity: (isEnabled) ? 1.0 : st.text_opacityDisable

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

    BorderHorizontal
    {
        id: border

        anchors.bottom: parent.bottom

        color: (isDefault || isSelected || isCurrent) ? st.itemList_colorBorderDefault
                                                      : st.itemList_colorBorder
    }
}
