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
    // Events
    //---------------------------------------------------------------------------------------------

    onHeightChanged: loadTracks()

    onContentYChanged: loadTracks()

    onVisibleChanged: loadTracks()

    //---------------------------------------------------------------------------------------------

    onPlaylistChanged: loadTracks()

    onCountChanged: loadTracks()

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function loadTracks()
    {
        if (visible == false) return;

        timer.restart();
    }

    //---------------------------------------------------------------------------------------------
    // Private

    function pApplyTracks()
    {
        if (playlist == null) return;

        var index = Math.floor(contentY / sizeTrack);

        // NOTE: We add 1 to cover the entire region when half a track is exposed at the top and
        //       the bottom of the list.
        var count = Math.ceil(height / sizeTrack) + 1;

        playlist.loadTracks(index, count);
    }

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    Timer
    {
        id: timer

        interval: st.listPlaylist_interval

        onTriggered: pApplyTracks()
    }
}
