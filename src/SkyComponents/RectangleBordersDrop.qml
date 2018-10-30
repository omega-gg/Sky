//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkyComponents module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

import QtQuick 1.0
import Sky     1.0

RectangleBorders
{
    //---------------------------------------------------------------------------------------------
    // Properties private
    //---------------------------------------------------------------------------------------------

    property variant pItem: null

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    width : (pItem) ? pItem.width  : 0
    height: (pItem) ? pItem.height : 0

    size: st.border_sizeFocus

    visible: false

    color: st.border_colorFocus

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function setItem(item)
    {
        if (pItem == item) return;

        pItem = item;

        if (item)
        {
            x = parent.mapFromItem(item, 0, 0).x;
            y = parent.mapFromItem(item, 0, 0).y;

            visible = true;
        }
        else visible = false;
    }

    function clearItem(item)
    {
        if (pItem == item) clear();
    }

    function clear()
    {
        pItem = null;

        visible = false;
    }

    //---------------------------------------------------------------------------------------------

    function updatePosition()
    {
        if (pItem == null) return;

        x = parent.mapFromItem(pItem, 0, 0).x;
        y = parent.mapFromItem(pItem, 0, 0).y;
    }
}
