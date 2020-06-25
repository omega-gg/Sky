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

ScrollArea
{
    id: scrollList

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias model   : list.model
    property alias delegate: list.delegate

    /* read */ property alias count: list.count

    property alias currentIndex: list.currentIndex

    //---------------------------------------------------------------------------------------------

    property alias list: list

    //---------------------------------------------------------------------------------------------
    // Signals
    //---------------------------------------------------------------------------------------------

    signal itemPressed (int index)
    signal itemReleased(int index)

    signal itemClicked      (int index)
    signal itemDoubleClicked(int index)

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    contentHeight: list.height

    singleStep     : list.itemSize
    wheelMultiplier: 1

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function updateView()
    {
        updateListHeight(list);
    }

    //---------------------------------------------------------------------------------------------
    // Events

    function onRangeUpdated()
    {
        updateView();
    }

    function onValueUpdated()
    {
        updateView();
    }

    //---------------------------------------------------------------------------------------------
    // List reimplementation

    function scrollToItem(index)
    {
        list.scrollToItem(index);
    }

    function scrollToItemTop(index)
    {
        list.scrollToItemTop(index);
    }

    //---------------------------------------------------------------------------------------------

    function selectPrevious()
    {
        list.selectPrevious();
    }

    function selectNext()
    {
        list.selectNext();
    }

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    List
    {
        id: list

        anchors.left : parent.left
        anchors.right: parent.right

        scrollArea: scrollList

        onItemPressed : scrollList.itemPressed (index)
        onItemReleased: scrollList.itemReleased(index)

        onItemClicked      : scrollList.itemClicked      (index)
        onItemDoubleClicked: scrollList.itemDoubleClicked(index)

        onCurrentIndexChanged: scrollToItem(currentIndex)
    }
}
