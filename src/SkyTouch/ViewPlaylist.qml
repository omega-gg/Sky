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

Loader
{
    id: viewPlaylist

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    // FIXME Qt6: Strangely, when using a var property we might get a changed events several times
    //            for the same playlist.
    /* mandatory */ property Playlist playlist

    /* mandatory */ property ModelPlaylist model

    property Component delegateGrid: ComponentGridTrack {}
    property Component delegateList: ComponentTrack     {}

    property int type: 0 // Grid is 0, List is 1.

    property int sizeTrack: st.buttonTouch_size + st.margins

    property int currentIndex: -1

    //---------------------------------------------------------------------------------------------
    // Signals
    //---------------------------------------------------------------------------------------------

    signal movementStarted

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias scrollBar: scrollBar

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    sourceComponent: (type) ? componentList
                            : componentGrid

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function selectTrack(index)
    {
        if (type) item.positionViewAtIndex(index, ListView.Contain);
        else      item.positionViewAtIndex(index, GridView.Contain);
    }

    function getPreferredWidth(width)
    {
        if (type)
        {
            if (scrollBar.visible)
            {
                 return item.contentWidth + scrollBar.width;
            }
            else return item.contentWidth;
        }
        else if (scrollBar.visible)
        {
            var scrollWidth = scrollBar.width;

            return item.getPreferredWidth(width - scrollWidth) + scrollWidth;
        }
        else return item.getPreferredWidth(width - scrollBar.width);
    }

    function getContentHeight()
    {
        var size;

        var itemLoading = item.itemLoading;

        if (itemLoading.visible)
        {
            size = itemLoading.height;
        }
        else size = item.contentHeight;

        return Math.max(sizeTrack, size);
    }

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

    Component
    {
        id: componentGrid

        GridPlaylist
        {
            property alias itemLoading: itemLoading

            anchors.fill: parent

            anchors.rightMargin: (scrollBar.isActive) ? scrollBar.width : 0

            playlist: viewPlaylist.playlist

            currentIndex: viewPlaylist.currentIndex

            model: viewPlaylist.model

            delegate: viewPlaylist.delegateGrid

            onMovementStarted: viewPlaylist.movementStarted()

            onCurrentIndexChanged: viewPlaylist.currentIndex = currentIndex

            ListLoading
            {
                id: itemLoading

                z: -1

                active: (playlist != null && playlist.queryIsLoading)
            }
        }
    }

    Component
    {
        id: componentList

        ListPlaylist
        {
            property alias itemLoading: itemLoading

            anchors.fill: parent

            anchors.rightMargin: (scrollBar.isActive) ? scrollBar.width : 0

            sizeTrack: viewPlaylist.sizeTrack

            playlist: viewPlaylist.playlist

            currentIndex: viewPlaylist.currentIndex

            model: viewPlaylist.model

            delegate: viewPlaylist.delegateList

            onMovementStarted: viewPlaylist.movementStarted()

            onCurrentIndexChanged: viewPlaylist.currentIndex = currentIndex

            ListLoading
            {
                id: itemLoading

                z: -1

                size: sizeTrack

                active: (playlist != null && playlist.queryIsLoading)

                list: parent
            }
        }
    }

    ScrollBar
    {
        id: scrollBar

        anchors.right: parent.right

        // NOTE: We want our minimum to match the track height.
        minimumSize: sizeTrack

        view: item
    }
}
