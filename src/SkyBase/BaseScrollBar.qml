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

    /* read */ property bool isActive: (pMaximumY > 0)

    property variant view

    property int minimumSize: st.scrollBar_minimumSize

    //---------------------------------------------------------------------------------------------
    // Private

    property int pMaximumY: (view) ? view.contentHeight - height : -1

    property int pMaximumHandle: height - handle.height

//#QT_4
    property int pOriginY: 0
//#ELSE
    property int pOriginY: (view) ? view.originY : -1
//#END

    property bool pUpdate: isActive

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias handle: handle

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    height: (view) ? view.height : 0

    visible: isActive

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    // NOTE: This handles 'page up' and 'page down' mouse interactions.
    onPressed:
    {
        if (mouseY < handle.y)
        {
            var position = pOriginY + view.contentY - view.height;

            if (position < pOriginY)
            {
                 view.contentY = pOriginY;
            }
            else view.contentY = position;
        }
        else
        {
            /* var */ position = pOriginY + view.contentY + view.height;

            var maximum = pOriginY + pMaximumY;

            if (position > maximum)
            {
                 view.contentY = maximum;
            }
            else view.contentY = position;
        }
    }

    //---------------------------------------------------------------------------------------------

    onViewChanged: pUpdateHandle()

    onPMaximumYChanged     : pUpdateHandle()
    onPMaximumHandleChanged: pUpdateHandle()

    //---------------------------------------------------------------------------------------------
    // Connections
    //---------------------------------------------------------------------------------------------

    Connections
    {
        target: view

        /* QML_CONNECTION */ function onContentYChanged() { pUpdateHandle(); }
    }

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------
    // Private

    function pUpdateHandle()
    {
        if (pUpdate == false) return;

        handle.y = st.getHandlePosition(view, pMaximumY, pMaximumHandle);
    }

    //---------------------------------------------------------------------------------------------

    function pApplyPosition()
    {
        pUpdate = false;

        view.contentY = st.getHandleY(view, handle, pMaximumY, pMaximumHandle);

        pUpdate = true;
    }

    //---------------------------------------------------------------------------------------------

    function pGetSize()
    {
        // NOTE Qt6.6: We need to check the view to avoid a warning.
        if (view == null) return 0;

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
    // Children
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

//#QT_NEW
        drag.threshold: 0
//#END

        onYChanged: if (drag.active) pApplyPosition()
    }
}
