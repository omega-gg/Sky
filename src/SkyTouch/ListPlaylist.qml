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

List
{
    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    // FIXME Qt6: Strangely, when using a var property we might get a changed events several times
    //            for the same playlist.
    /* mandatory */ property Playlist playlist

    property int sizeTrack: st.buttonTouch_size + spacing

    //---------------------------------------------------------------------------------------------
    // Private

    // NOTE: We need this to avoid loading tracks when the item is not loaded.
    property bool pReady: false

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

        // NOTE: We skip tracks that were reloaded less than 1 minute ago.
        playlist.reloadTracks(pGetIndex(), pGetCount(), 60000);
    }

    //---------------------------------------------------------------------------------------------

    function pGetIndex()
    {
        return Math.floor(getY() / sizeTrack);
    }

    function pGetCount()
    {
        // NOTE: We add 1 to cover the entire region when half a track is exposed at the top and
        //       the bottom of the list.
        return Math.ceil(height / sizeTrack) + 1;
    }

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    Timer
    {
        id: timerLoad

        interval: st.listPlaylist_intervalLoad

        onTriggered: pApplyReload()
    }

    // NOTE: We want to reload each track periodically.
    Timer
    {
        id: timerReload

        interval: st.listPlaylist_intervalReload

        repeat: true

        onTriggered:
        {
            // NOTE: We are reloading so we don't need to load anymore.
            timerLoad.stop();

            pApplyReload();
        }
    }
}
