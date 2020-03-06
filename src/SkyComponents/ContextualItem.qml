//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkyComponents module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

import QtQuick 1.0
import Sky     1.0

MouseArea
{
    id: contextualItem

    //---------------------------------------------------------------------------------------------
    // Properties private
    //---------------------------------------------------------------------------------------------

    property bool pCurrent: (isCurrent || pressed)

    property int pIconWidth: Math.max(itemIcon.width, background.height)

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias textMargin: itemText.leftMargin

    //---------------------------------------------------------------------------------------------

    property alias background: background

    property alias itemIcon: itemIcon
    property alias itemText: itemText

    //---------------------------------------------------------------------------------------------
    // Style

    property alias gradient: background.gradient

    property alias filter: itemIcon.filter

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    anchors.left : parent.left
    anchors.right: parent.right

    height: st.contextualItem_height

    enabled     : isEnabled
    hoverEnabled: isEnabled

    cursor: Qt.PointingHandCursor

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    onClicked: itemClicked(id)

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    Rectangle
    {
        id: background

        anchors.left  : parent.left
        anchors.right : parent.right
        anchors.top   : parent.top
        anchors.bottom: border.top

        visible: (isCurrent || pressed || containsMouse)

        gradient: Gradient
        {
            GradientStop
            {
                position: 0.0

                color:
                {
                    if (isCurrent)
                    {
                        if (pressed || isReturnPressed)
                        {
                            return st.itemList_colorSelectFocusA;
                        }
                        else if (containsMouse)
                        {
                            return st.itemList_colorSelectHoverA;
                        }
                        else return st.itemList_colorSelectA;
                    }
                    else if (pressed)
                    {
                        return st.itemList_colorSelectFocusA;
                    }
                    else return st.itemList_colorHoverA;
                }
            }

            GradientStop
            {
                position: 1.0

                color:
                {
                    if (isCurrent)
                    {
                        if (pressed || isReturnPressed)
                        {
                            return st.itemList_colorSelectFocusB;
                        }
                        else if (containsMouse)
                        {
                            return st.itemList_colorSelectHoverB;
                        }
                        else return st.itemList_colorSelectB;
                    }
                    else if (pressed)
                    {
                        return st.itemList_colorSelectFocusB;
                    }
                    else return st.itemList_colorHoverB;
                }
            }
        }
    }

    Icon
    {
        id: itemIcon

        anchors.left: parent.left

        anchors.leftMargin: Math.round((pIconWidth - width) / 2)

        anchors.verticalCenter: background.verticalCenter

        opacity: (isEnabled) ? 1.0 : st.icon_opacityDisable

        source    : icon
        sourceSize: iconSize

        style: st.icon_sunken

        filter: (pCurrent) ? st.icon2_filter
                           : st.icon1_filter
    }

    TextBase
    {
        id: itemText

        anchors.left  : parent.left
        anchors.right : parent.right
        anchors.top   : parent.top
        anchors.bottom: border.top

        leftMargin: (itemIcon.width) ? pIconWidth
                                     : st.contextualItem_padding

        rightMargin: st.contextualItem_padding

        verticalAlignment: Text.AlignVCenter

        opacity: (isEnabled) ? 1.0 : st.text_opacityDisable

        text: title

        color:
        {
            if (pCurrent)
            {
                return st.itemList_colorTextSelected;
            }
            else if (containsMouse)
            {
                return st.itemList_colorTextHover;
            }
            else return st.itemList_colorText;
        }

        style: st.text_sunken
    }

    BorderHorizontal
    {
        id: border

        anchors.bottom: parent.bottom
    }
}
