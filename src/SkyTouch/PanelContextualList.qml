//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkyTouch.

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

PanelContextual
{
    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property int activeIndex: -1

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias currentIndex: list.currentIndex

    property alias model: list.model

    property alias delegate: list.delegate

    //---------------------------------------------------------------------------------------------

    property alias list: list

    property alias scrollBar: scrollBar

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    preferredHeight: list.contentHeight + margins * 2

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    List
    {
        id: list

        anchors.fill: parent

        anchors.rightMargin: (scrollBar.isActive) ? scrollBar.width : 0

        model: ListModel {}

        delegate: ButtonWideFull
        {
            checked: (currentIndex == index)

            iconDefault: cover

            text: title
        }

        clip: true

        onVisibleChanged: if (visible) positionViewAtIndex(currentIndex, ListView.Contain)
    }

    ScrollBar
    {
        id: scrollBar

        anchors.right: parent.right

        view: list
    }
}
