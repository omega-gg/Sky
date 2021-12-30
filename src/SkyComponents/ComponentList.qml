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
    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    /* read */ property bool isCurrent: (index == currentIndex)

    property bool checkHover: true

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias text: itemText.text

    //---------------------------------------------------------------------------------------------

    property alias background: background

    property alias itemText: itemText

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    width: parent.width

    height: st.componentList_height

    hoverEnabled: true

    cursor: Qt.PointingHandCursor

    //---------------------------------------------------------------------------------------------
    // Event
    //---------------------------------------------------------------------------------------------

    onPressed:
    {
        onPress();

        itemPressed(index);
    }

    onReleased: itemReleased(index)

    onClicked: itemClicked(index)

    onDoubleClicked: itemDoubleClicked(index)

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------
    // Events

    function onPress()
    {
        currentIndex = index;
    }

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

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
                    if (isCurrent)
                    {
                        if (checkHover && containsMouse)
                        {
                             return st.itemList_colorSelectFocusA;
                        }
                        else return st.itemList_colorSelectA;
                    }
                    else if (pressed)       return st.itemList_colorPressA;
                    else if (containsMouse) return st.itemList_colorHoverA;
                    else                    return st.itemList_colorA;
                }
            }

            GradientStop
            {
                position: 1.0

                color:
                {
                    if (isCurrent)
                    {
                        if (checkHover && containsMouse)
                        {
                             return st.itemList_colorSelectFocusB;
                        }
                        else return st.itemList_colorSelectB;
                    }
                    else if (pressed)       return st.itemList_colorPressB;
                    else if (containsMouse) return st.itemList_colorHoverB;
                    else                    return st.itemList_colorB;
                }
            }
        }

        TextBase
        {
            id: itemText

            anchors.fill: parent

            anchors.leftMargin : st.dp8
            anchors.rightMargin: st.dp8

            verticalAlignment: Text.AlignVCenter

            text: title

            color:
            {
                if (isCurrent)
                {
                    return st.itemList_colorTextSelected;
                }
                else if (containsMouse)
                {
                    return st.itemList_colorTextHover;
                }
                else return st.itemList_colorText
            }

            style: (isCurrent) ? st.text_raised
                               : st.text_sunken
        }
    }

    BorderHorizontal
    {
        id: border

        anchors.bottom: parent.bottom

        color: st.itemList_colorBorder
    }
}
