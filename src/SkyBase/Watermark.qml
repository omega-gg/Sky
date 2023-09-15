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

ImageScale
{
    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    /* mandatory */ property variant player

    /* read */ property int marginRight: 0
    /* read */ property int marginTop  : 0

    //---------------------------------------------------------------------------------------------
    // Private

    property variant pRect

    property int pHeight: 0

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    anchors.right: player.right
    anchors.top  : player.top

    anchors.rightMargin: marginRight
    anchors.topMargin  : marginTop

    height: pHeight

    visible: (player.visible && isSourceDefault == false)

    source: player.watermark

    fillMode: AbstractBackend.PreserveAspectFit

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    onVisibleChanged: if (visible) onUpdateGeometry()

    //---------------------------------------------------------------------------------------------
    // Connections
    //---------------------------------------------------------------------------------------------

    Connections
    {
        target: (visible) ? player : null

        /* QML_CONNECTION */ function onWidthChanged () { onUpdateGeometry() }
        /* QML_CONNECTION */ function onHeightChanged() { onUpdateGeometry() }

        /* QML_CONNECTION */ function onFillModeChanged() { onUpdateGeometry() }

        /* QML_CONNECTION */ function onStateLoadChanged()
        {
            // NOTE: We want to update the geometry after the player has finished buffering.
            if (player.isDefault) onUpdateGeometry();
        }
    }

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------
    // Events

    function onUpdateGeometry()
    {
        var rect = player.getGeometry();

        if (pRect == rect) return;

        pRect = rect;

        pHeight = rect.height / 24;

        marginRight = rect.x;
        marginTop   = rect.y;
    }
}
