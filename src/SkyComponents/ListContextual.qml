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

BaseList
{
    id: listContextual

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property bool isReturnPressed: false

    property int currentId: -1

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias currentPage: model.currentPage

    //---------------------------------------------------------------------------------------------
    // Signals
    //---------------------------------------------------------------------------------------------

    signal itemClicked(int id)

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    model: ModelContextual { id: model }

    delegate: ComponentContextual {}

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    onActiveFocusChanged: isReturnPressed = false

    //---------------------------------------------------------------------------------------------
    // Keys
    //---------------------------------------------------------------------------------------------

    Keys.onPressed:
    {
        if ((event.key == Qt.Key_Left || event.key == Qt.Key_Right)
            &&
            event.modifiers == sk.keypad(Qt.NoModifier))
        {
            event.accepted = true;
        }
        else if (event.key == Qt.Key_Up && event.modifiers == sk.keypad(Qt.NoModifier))
        {
            event.accepted = true;

            selectPrevious();
        }
        else if (event.key == Qt.Key_Down && event.modifiers == sk.keypad(Qt.NoModifier))
        {
            event.accepted = true;

            selectNext();
        }
        else if (event.key == Qt.Key_Return || event.key == Qt.Key_Enter)
        {
            event.accepted = true;

            if (event.isAutoRepeat) return;

            isReturnPressed = true;

            var id;

            if (currentPage) id = currentPage.currentId;
            else             id = -1;

            if (id == -1) return;

            var component = itemFromId(id);

            if (component == null) return;

            component.item.press();

            component.item.click();
        }
    }

    Keys.onReleased: isReturnPressed = false

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function getCurrentId()
    {
        if (currentPage)
        {
             return currentPage.currentId;
        }
        else return -1;
    }

    function setCurrentId(id)
    {
        if (currentPage) currentPage.currentId = id;
    }

    //---------------------------------------------------------------------------------------------

    function clearCurrentId()
    {
        if (currentPage) currentPage.currentId = -1;
    }

    //---------------------------------------------------------------------------------------------

    function selectPrevious()
    {
        if (currentPage) currentPage.selectPrevious();
    }

    function selectNext()
    {
        if (currentPage) currentPage.selectNext();
    }

    //---------------------------------------------------------------------------------------------

    function itemFromId(id)
    {
        for (var i = 0; i < children.length; i++)
        {
            if (typeof children[i].getId == "function" && children[i].getId() == id)
            {
                return children[i];
            }
        }

        return null;
    }
}
