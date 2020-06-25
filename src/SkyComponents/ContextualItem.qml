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
    id: contextualItem

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property int textMargin: (itemIcon.width) ? pIconWidth
                                              : st.contextualItem_padding

    //---------------------------------------------------------------------------------------------
    // Private

    property bool pCurrent: (isCurrent || pressed)

    property int pIconWidth: Math.max(itemIcon.width, background.height)

    //---------------------------------------------------------------------------------------------
    // Aliases
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

        anchors.leftMargin: textMargin

        anchors.rightMargin: st.contextualItem_padding

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
