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
    id: baseScrollBar

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    /* read */ property bool isActive: (pMaximumA > 0)

    /* mandatory */ property variant view

    property int minimumSize: st.scrollBar_minimumSize

    //---------------------------------------------------------------------------------------------
    // Private

    // NOTE: We want to take Flickable.originY into account.
    property int pMaximumA: view.originY + view.contentHeight - height

    property int pMaximumB: height - handle.height

    property bool pUpdate: true

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias handle: handle

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    height: view.height

    visible: isActive

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    // NOTE: This handles 'page up' and 'page down' mouse interactions.
    onPressed:
    {
        var originY = view.originY;

        if (mouseY < handle.y)
        {
            var position = originY + view.contentY - view.height;

            if (position < originY)
            {
                 view.contentY = originY;
            }
            else view.contentY = position;
        }
        else
        {
            /* var */ position = originY + view.contentY + view.height;

            if (position > pMaximumA)
            {
                 view.contentY = pMaximumA;
            }
            else view.contentY = position;
        }
    }

    //---------------------------------------------------------------------------------------------

    onViewChanged: pUpdateHandle()

    onPMaximumAChanged: pUpdateHandle()
    onPMaximumBChanged: pUpdateHandle()

    //---------------------------------------------------------------------------------------------
    // Connections
    //---------------------------------------------------------------------------------------------

    Connections
    {
        target: view

        onContentYChanged: pUpdateHandle()
    }

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------
    // Private

    function pUpdateHandle()
    {
        if (pUpdate == false) return;

        var ratio = view.contentY / pMaximumA;

        // NOTE: We don't want the scrollBar position to go under 0.
        if (ratio < 1.0)
        {
             handle.y = Math.max(0, pMaximumB * ratio);
        }
        else handle.y = Math.max(0, pMaximumB);
    }

    //---------------------------------------------------------------------------------------------

    function pApplyPosition()
    {
        pUpdate = false;

        var ratio = handle.y / pMaximumB;

        if (ratio < 1.0)
        {
             view.contentY = pMaximumA * ratio;
        }
        else view.contentY = pMaximumA;

        pUpdate = true;
    }

    //---------------------------------------------------------------------------------------------

    function pGetSize()
    {
        var size = height * (height / view.contentHeight);

        if (size < minimumSize)
        {
            return minimumSize;
        }
        else if (size < height)
        {
            return size;
        }
        else return height;
    }

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    MouseArea
    {
        id: handle

        anchors.left : parent.left
        anchors.right: parent.right

        height: pGetSize()

        visible: (baseScrollBar.enabled && parent.height > minimumSize)

        hoverEnabled: true

        drag.target: handle
        drag.axis  : Drag.YAxis

        drag.minimumY: 0
        drag.maximumY: parent.height - height

//#QT_5
        drag.threshold: 0
//#END

        onYChanged: if (drag.active) pApplyPosition()
    }
}
