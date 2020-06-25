//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkyComponents.

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

Loader
{
    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    anchors.left : parent.left
    anchors.right: parent.right

    height: (type == ContextualPage.Category) ? st.contextualCategory_height
                                              : st.contextualItem_height

    sourceComponent:
    {
        if      (type == ContextualPage.Item)        return item;
        else if (type == ContextualPage.ItemCover)   return itemCover;
        else if (type == ContextualPage.ItemConfirm) return itemConfirm;
        else /* (type == ContextualPage.Category) */ return category;
    }

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function getIndex() { return index; }
    function getId   () { return id;    }

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    Component { id: category;    ContextualCategory    {} }
    Component { id: item;        ContextualItem        {} }
    Component { id: itemCover;   ContextualItemCover   {} }
    Component { id: itemConfirm; ContextualItemConfirm {} }
}
