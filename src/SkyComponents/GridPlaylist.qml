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

BaseGrid
{
    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    // FIXME Qt6: Strangely, when using a var property we might get a changed events several times
    //            for the same playlist.
    /* mandatory */ property Playlist playlist

    // NOTE: The minimum tracks to load.
    property int minimumLoad: 8

    property url logo     : st.logo
    property int logoRatio: st.logoRatio

    property bool asynchronous: true

    //---------------------------------------------------------------------------------------------
    // Style

    property color defaultColorA: st.logo_colorA
    property color defaultColorB: st.logo_colorB

    //---------------------------------------------------------------------------------------------
    // Private

    // NOTE: We avoid loading tracks when the item is not loaded.
    property bool pReady: false

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    //cellWidth : coverWidth  + padding2x + spacing
    //cellHeight: coverHeight + padding2x + st.buttonTouch_size + spacingBottom

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    Component.onCompleted:
    {
        pReady = true;

        pUpdateVisible();
    }

    onHeightChanged: reloadTracks()

    onContentYChanged: reloadTracks()

    onVisibleChanged: if (pReady) pUpdateVisible()

    //---------------------------------------------------------------------------------------------

    onPlaylistChanged: reloadTracks()

    onCountChanged: reloadTracks()

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    Connections
    {
        target: playlist

        /* QML_CONNECTION */ function onTracksMoved() { reloadTracks() }
    }

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function reloadTracks()
    {
        if (pReady == false || visible == false) return;

        timerLoad.restart();
    }

    //---------------------------------------------------------------------------------------------
    // Private

    function pUpdateVisible()
    {
        if (visible)
        {
            reloadTracks();

            timerReload.start();
        }
        else timerReload.stop();
    }

    function pApplyReload()
    {
        if (playlist == null) return;

        var count = getReloadCount();

        if (count < minimumLoad)
        {
            var index = Math.max(0, getReloadIndex() - Math.round((minimumLoad - count) / 2));

            // NOTE: We skip tracks that were reloaded less than 1 minute ago.
            playlist.reloadTracks(index, minimumLoad, 60000);
        }
        // NOTE: We skip tracks that were reloaded less than 1 minute ago.
        else playlist.reloadTracks(getReloadIndex(), count, 60000);
    }

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

    Timer
    {
        id: timerLoad

        interval: st.gridPlaylist_intervalLoad

        onTriggered: pApplyReload()
    }

    // NOTE: We want to reload each track periodically.
    Timer
    {
        id: timerReload

        interval: st.gridPlaylist_intervalReload

        repeat: true

        onTriggered:
        {
            // NOTE: We are reloading so we don't need to load anymore.
            timerLoad.stop();

            pApplyReload();
        }
    }
}
