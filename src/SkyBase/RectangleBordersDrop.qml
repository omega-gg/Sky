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

RectangleBorders
{
    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------
    // Private

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
            pApplyPosition();

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
        if (pItem) pApplyPosition();
    }

    //---------------------------------------------------------------------------------------------
    // Private

    function pApplyPosition()
    {
        var position = parent.mapFromItem(pItem, 0, 0);

        x = position.x;
        y = position.y;
    }
}
