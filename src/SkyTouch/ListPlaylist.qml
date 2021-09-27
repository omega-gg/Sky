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

    /* mandatory */ property variant playlist

    property int sizeTrack: st.buttonTouch_size + spacing

    //---------------------------------------------------------------------------------------------
    // Private

    property bool pReload: false

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    onHeightChanged: loadTracks()

    onContentYChanged: loadTracks()

    onVisibleChanged: loadTracks()

    //---------------------------------------------------------------------------------------------

    onPlaylistChanged: reloadTracks()

    onCountChanged: loadTracks()

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function loadTracks()
    {
        if (visible == false) return;

        timerLoad.restart();
    }

    function reloadTracks()
    {
        if (visible == false) return;

        pReload = true;

        timerLoad.restart();
    }

    //---------------------------------------------------------------------------------------------
    // Private

    function pApplyLoad()
    {
        if (playlist == null) return;

        if (pReload)
        {
            pReload = false;

            playlist.reloadTracks(pGetIndex(), pGetCount());
        }
        else playlist.loadTracks(pGetIndex(), pGetCount());
    }

    function pApplyReload()
    {
        if (playlist == null) return;

        // NOTE: We are reloading so we don't need to load anymore.
        timerLoad.stop();

        pReload = false;

        playlist.reloadTracks(pGetIndex(), pGetCount());
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

        onTriggered: pApplyLoad()
    }

    // NOTE: We want to reload each tracks periodically.
    Timer
    {
        id: timerReload

        interval: st.listPlaylist_intervalReload

        repeat: true

        onTriggered: pApplyReload()
    }
}
