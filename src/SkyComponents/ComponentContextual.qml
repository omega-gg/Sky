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
