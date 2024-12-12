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

Item
{
    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------
    // Private

    property bool pUpdate: true

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    /* read */ property bool isScrollable: false

    property alias playlist: grid.playlist

    property alias contentHeight: grid.contentHeight
    property alias contentY     : grid.contentY

    property alias model   : grid.model
    property alias delegate: grid.delegate

    /* read */ property alias count: grid.count

    property alias currentIndex: grid.currentIndex

    //---------------------------------------------------------------------------------------------

    property alias grid     : grid
    property alias scrollBar: scrollBar

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function showTrack(index)
    {
        grid.positionViewAtIndex(index, GridView.Contain);
    }

    function showTrackBegin(index)
    {
        grid.positionViewAtIndex(index, GridView.Beginning);
    }

    //---------------------------------------------------------------------------------------------
    // Private

    function pUpdateRange()
    {
        if (contentHeight > height)
        {
            scrollBar.pageStep = height;

            scrollBar.model.setRange(0, contentHeight - height);

            isScrollable = true;
        }
        else
        {
            scrollBar.value    = 0;
            scrollBar.pageStep = 0;

            scrollBar.model.setRange(0, 0);

            isScrollable = false;
        }
    }

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

    GridPlaylist
    {
        id: grid

        anchors.fill: parent

        anchors.rightMargin: (isScrollable) ? scrollBar.width : 0

        delegate: ComponentGridTrack {}

        onHeightChanged: pUpdateRange()

        onContentHeightChanged: pUpdateRange()

        onContentYChanged:
        {
            pUpdate = false;

            scrollBar.value = contentY;

            pUpdate = true;
        }
    }

    ScrollBar
    {
        id: scrollBar

        anchors.right : parent.right
        anchors.top   : parent.top
        anchors.bottom: parent.bottom

        visible: isScrollable

        onValueChanged:
        {
            if (pUpdate == false) return;

            // FIXME Qt5: This might make the content blurry due to the float value.
            if (value == maximum)
            {
                 grid.scrollToY(value);
            }
            else grid.scrollToY(intValue);
        }
    }
}
