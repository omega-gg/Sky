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

MouseArea
{
    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    /* mandatory */ property variant handle

    property bool isHovered: containsMouse

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------
    // Model

    property alias model: model

    property alias value   : model.value
    property alias position: model.position

    property alias minimum: model.minimum
    property alias maximum: model.maximum

    property alias pageStep  : model.pageStep
    property alias singleStep: model.singleStep

    //---------------------------------------------------------------------------------------------
    // Signal
    //---------------------------------------------------------------------------------------------

    signal handleReleased

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    hoverEnabled: true
    wheelEnabled: true

    drag.target: handle
    drag.axis  : Drag.XAxis

    drag.minimumX: model.handleMinimum
    drag.maximumX: model.handleMaximum

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    onPressed:
    {
        var pos = mouseX - (handle.width / 2);

        pos = Math.max(model.handleMinimum, pos);
        pos = Math.min(pos, model.handleMaximum);

        handle.x = pos;
    }

    onReleased:
    {
        position = handle.x;

        handleReleased();
    }

    QML_EVENT onWheeled: function(steps)
    {
        if (visible) model.scroll(steps * 2);
    }

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function moveTo(pos)
    {
        pos = Math.max(0, pos);
        pos = Math.min(pos, maximum);

        value = pos;

        handleReleased();
    }

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    ModelRange
    {
        id: model

        handleMaximum: width - handle.width

        onPositionChanged:
        {
            if (width > 0 && handle.pressed == false)
            {
                handle.x = position;
            }
        }
    }
}
