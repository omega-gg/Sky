//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

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

ScrollAreaVertical
{
    id: scrollCompletion

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property string textDefault

    property bool hasNoResults: false

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias model   : list.model
    property alias delegate: list.delegate

    /* read */ property alias count: list.count

    property alias completion: list.completion

    property alias currentIndex: list.currentIndex

    //---------------------------------------------------------------------------------------------

    property alias list: list

    property alias itemText: itemText

    //---------------------------------------------------------------------------------------------
    // ListCompletion

    property alias query: list.query

    //---------------------------------------------------------------------------------------------
    // Signals
    //---------------------------------------------------------------------------------------------

    signal itemClicked      (int index)
    signal itemDoubleClicked(int index)

    signal queryCompleted

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    contentHeight: (itemText.visible) ? itemText.height
                                      : list.height

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

    //---------------------------------------------------------------------------------------------
    // ListCompletion reimplementation

    function addQueryItem(key, value)
    {
        list.addQueryItem(key, value);
    }

    function runQuery()
    {
        list.runQuery();
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

    TextListDefault
    {
        id: itemText

        anchors.left : parent.left
        anchors.right: parent.right
        anchors.top  : parent.top

        anchors.topMargin: st.dp20

        horizontalAlignment: Text.AlignHCenter

        text: (hasNoResults) ? qsTr("No completion")
                             : textDefault

        visible: (count == 0)
    }

    ListCompletion
    {
        id: list

        anchors.left : parent.left
        anchors.right: parent.right

        scrollArea: scrollCompletion

        onItemClicked      : scrollCompletion.itemClicked      (index)
        onItemDoubleClicked: scrollCompletion.itemDoubleClicked(index)

        onQueryCompleted:
        {
            if (query != "" && count == 0)
            {
                 hasNoResults = true;
            }
            else hasNoResults = false;

            scrollCompletion.queryCompleted();
        }
    }
}
