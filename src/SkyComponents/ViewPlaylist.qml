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

    property string textDefault: qsTr("Playlist is empty")

    //---------------------------------------------------------------------------------------------
    // Private

    property bool pUpdate: true

    property int pMaximumY: grid.contentHeight - height

    property int pMaximumHandle: height - handle.height

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

    property alias singleStep: scrollBar.singleStep
    property alias pageStep  : scrollBar.pageStep

    property alias wheelMultiplier: scrollBar.wheelMultiplier

    //---------------------------------------------------------------------------------------------

    property alias coverWidth : grid.coverWidth
    property alias coverHeight: grid.coverHeight

    property alias spacing      : grid.spacing
    property alias spacingBottom: grid.spacingBottom

    property alias grid: grid

    property alias itemText   : itemText
    property alias itemLoading: itemLoading

    property alias scrollBar: scrollBar
    property alias handle   : scrollBar.handle

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    acceptedButtons: Qt.NoButton

    wheelEnabled: true

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    /* QML_EVENT */ onWheeled: function(steps)
    {
        if (scrollBar.visible == false || scrollBar.handle.pressed) return;

        scrollBar.model.scroll(-steps * wheelMultiplier);
    }

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
            scrollBar.pageStep = 0;

            scrollBar.model.setRange(0, 0);

            isScrollable = false;
        }

        pUpdateHandle();
    }

    function pUpdateHandle()
    {
        if (pUpdate == false) return;

        handle.y = st.getHandlePosition(grid, pMaximumY, pMaximumHandle);
    }

    function pApplyPosition()
    {
        pUpdate = false;

        grid.contentY = st.getHandleY(grid, handle, pMaximumY, pMaximumHandle);

        pUpdate = true;
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

        onContentYChanged: pUpdateHandle()
    }

    TextListDefault
    {
        id: itemText

        anchors.left : grid.left
        anchors.right: grid.right
        anchors.top  : parent.top

        anchors.topMargin: st.dp20

        horizontalAlignment: Text.AlignHCenter

        visible: (playlist != null && playlist.queryIsLoading == false && count == 0)

        text: qsTr("Playlist is empty")
    }

    LabelLoadingButton
    {
        id: itemLoading

        anchors.top: parent.top

        anchors.topMargin: st.dp16

        anchors.horizontalCenter: grid.horizontalCenter

        visible: (playlist != null && playlist.queryIsLoading && count == 0)

        text: qsTr("Loading Tracks...")

        onClicked: playlist.abortQuery()
    }

    ScrollBar
    {
        id: scrollBar

        anchors.right : parent.right
        anchors.top   : parent.top
        anchors.bottom: parent.bottom

        visible: isScrollable

        singleStep     : grid.cellHeight
        wheelMultiplier: 1

        onPositionChanged: pApplyPosition()
    }
}
