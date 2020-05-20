//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of Sky kit.

    - GNU Lesser General Public License Usage:
    This file may be used under the terms of the GNU Lesser General Public License version 3 as
    published by the Free Software Foundation and appearing in the LICENSE.md file included in the
    packaging of this file. Please review the following information to ensure the GNU Lesser
    General Public License requirements will be met: https://www.gnu.org/licenses/lgpl.html.
*/
//=================================================================================================

import QtQuick 1.0
import Sky     1.0

MouseArea
{
    id: baseTabs

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    /* read */ property int count: (model) ? model.count : 0

    /* read */ property int tabsWidth: (borderRight.visible) ? width
                                                             : getItemX(count) + borderSize

    /* read */ property real tabWidth: calculateTabWidth(count)

    property int tabMinimum: st.baseTabs_tabMinimum
    property int tabMaximum: st.baseTabs_tabMaximum

    /* read */ property variant itemHovered: null

    /* read */ property int indexHover: (itemHovered) ? itemHovered.getIndex()
                                                      : -1

    /* read */ property int borderSize: st.baseTabs_borderSize

    /* read */ property int spacing: st.baseTabs_spacing

    property bool asynchronous: false

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias model   : repeater.model
    property alias delegate: repeater.delegate

    property alias repeater   : repeater
    property alias borderRight: borderRight

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    height: st.baseTabs_height

    clip: (repeater.width > width)

    acceptedButtons: Qt.NoButton

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function getItemX(index)
    {
        return Math.round(index * tabWidth);
    }

    function getItemWidth(index)
    {
        return tabWidth;
    }

    //---------------------------------------------------------------------------------------------

    function calculateTabWidth(count)
    {
        var size = Math.min((width - borderSize) / count, tabMaximum);

        return Math.max(tabMinimum, size);
    }

    //---------------------------------------------------------------------------------------------

    function itemAt(index)
    {
        for (var i = 0; i < children.length; i++)
        {
            var child = children[i];

            if (typeof child.getIndex == "function" && (child.getIndex()) == index)
            {
                return child;
            }
        }

        return null;
    }

    function indexFromPos(x, y)
    {
        if (x < 0 || x > width
            ||
            y < 0 || y > height) return -1;

        for (var i = 0; i < children.length; i++)
        {
            var child = children[i];

            if (typeof child.getIndex == "function" && child.visible)
            {
                if (x >= child.x && x < (child.x + child.width))
                {
                    return child.getIndex();
                }
            }
        }

        return -1;
    }

    //---------------------------------------------------------------------------------------------

    function setItemHovered(item)
    {
        itemHovered = item;
    }

    function clearItemHovered()
    {
        itemHovered = null;
    }

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    Repeater
    {
        id: repeater

        anchors.top   : parent.top
        anchors.bottom: parent.bottom

        width: (tabWidth * baseTabs.count) + borderSize
    }

    BorderVertical
    {
        id: borderRight

        anchors.right: parent.right

        size: borderSize

        z: 1

        visible: parent.clip
    }
}
