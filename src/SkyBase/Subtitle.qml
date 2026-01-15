//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkyBase.

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

TextSubtitle
{
    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    /* mandatory */ property variant player

    property int margin: st.dp8

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    anchors.left  : player.left
    anchors.right : player.right
    anchors.bottom: player.bottom

    anchors.leftMargin  : st.dp8
    anchors.rightMargin : st.dp8
    anchors.bottomMargin: margin

    z: player.z

    // NOTE: The player has to be playing on the default output.
    visible: (player.visible
              &&
              player.hasStarted && player.hasOutput == false && player.isLoading == false)

    source: player.subtitle

    currentTime: player.currentTime

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    onVisibleChanged: onUpdateMargin()

    //---------------------------------------------------------------------------------------------
    // Connections
    //---------------------------------------------------------------------------------------------

    Connections
    {
        target: (visible) ? player : null

        /* QML_CONNECTION */ function onWidthChanged () { onUpdateMargin() }
        /* QML_CONNECTION */ function onHeightChanged() { onUpdateMargin() }

        /* QML_CONNECTION */ function onFillModeChanged() { onUpdateMargin() }
    }

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------
    // Events

    function onUpdateMargin() {}
}
