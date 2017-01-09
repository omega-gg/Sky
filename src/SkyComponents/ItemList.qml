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
    //-----------------------------------------------------------------------------------------
    // Properties
    //-----------------------------------------------------------------------------------------

    property bool isEnabled: true

    property bool isFocused: activeFocus

    property bool isPressed: (pressed && (pressedButtons & Qt.LeftButton))
    property bool isHovered: containsMouse

    property bool isDefault   : false
    property bool isSelected  : false
    property bool isCurrent   : false
    property bool isContextual: false

    //-----------------------------------------------------------------------------------------
    // Aliases
    //-----------------------------------------------------------------------------------------

    property alias isIconDefault: itemIcon.isSourceDefault

    property alias borderSize: border.size

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

    property alias iconStyle: itemIcon.style

    property alias text: itemText.text
    property alias font: itemText.font

    property alias textSpacing: itemText.leftMargin
    property alias textMargin : itemText.rightMargin

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

    cursor: MouseArea.PointingHandCursor

    //-----------------------------------------------------------------------------------------
    // Functions
    //-----------------------------------------------------------------------------------------

    function focus() { return forceActiveFocus(); }

    //-----------------------------------------------------------------------------------------
    // Childs
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

            iconWidth : st.itemList_iconWidth
            iconHeight: st.itemList_iconHeight

            sourceSize: Qt.size(iconWidth, iconHeight)

            clip: (fillMode == Image.PreserveAspectCrop)

            style: (itemText.style == Text.Raised) ? Sk.IconRaised
                                                   : Sk.IconSunken

            enableFilter: isSourceDefault

            filterDefault:
            {
                if      (isSelected) return st.icon_filter;
                else if (isCurrent)  return st.icon_filterActive;
                else                 return st.icon_filter;
            }
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
                    if (isFocused) return Text.Raised;
                    else           return Text.Sunken;
                }
                else if (isCurrent)
                {
                     return Text.Raised;
                }
                else return Text.Sunken;
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
