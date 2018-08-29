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

import QtQuick 1.1
import Sky     1.0

List
{
    id: listContextual

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property bool isReturnPressed: false

    property int currentId: -1

    property int spacing: st.listContextual_spacing

    //---------------------------------------------------------------------------------------------
    // Style

    property ImageColorFilter filterIcon: st.listContextual_filterIcon

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
            event.modifiers == Qt.NoModifier)
        {
            event.accepted = true;
        }
        else if (event.key == Qt.Key_Up && event.modifiers == Qt.NoModifier)
        {
            event.accepted = true;

            selectPrevious();
        }
        else if (event.key == Qt.Key_Down && event.modifiers == Qt.NoModifier)
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
