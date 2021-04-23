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

Item
{
    id: areaContextual

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    /* read */ property bool isActive: false

    /* read */ property variant currentPanel: null

    /* read */ property variant parentContextual: null

    //---------------------------------------------------------------------------------------------
    // Private

    property variant pLastPanel : null
    property variant pLastParent: null

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias mouseWatcher: mouseWatcher

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    onWidthChanged : hidePanels()
    onHeightChanged: hidePanels()

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function showPanel(panel, item, position, x, y, marginX, marginY, isCursorChild)
    {
        if (checkPanel(panel, item))
        {
            hidePanels();

            return false;
        }

        for (var i = 0; i < children.length; i++)
        {
            var child = children[i];

            if (panel == child)
            {
                if (pLastParent == item)
                {
                    if (child.visible == false)
                    {
                        pSetPanel(panel);

                        pUpdatePanel(item, position, x, y, marginX, marginY, isCursorChild);

                        pLastParent = item;

                        return true;
                    }
                    else
                    {
                        hidePanels();

                        return false;
                    }
                }
                else
                {
                    pClearParentContextual();

                    pSetPanel(panel);

                    pUpdatePanel(item, position, x, y, marginX, marginY, isCursorChild);

                    pLastParent = item;

                    return true;
                }
            }
        }
    }

    //---------------------------------------------------------------------------------------------

    function showPanelFrom(panel, item)
    {
        return showPanel(panel, item, Sk.BottomLeft, -1, -1, 0, 0, false);
    }

    //---------------------------------------------------------------------------------------------

    function showPanelPosition(panel, item, position)
    {
        return showPanel(panel, item, position, -1, -1, 0, 0, false);
    }

    function showPanelPositionMargins(panel, item, position, marginX, marginY)
    {
        return showPanel(panel, item, position, -1, -1, marginX, marginY, false);
    }

    function showPanelPositionAt(panel, item, position, x, y, isCursorChild)
    {
        return showPanel(panel, item, position, x, y, 0, 0, isCursorChild);
    }

    //---------------------------------------------------------------------------------------------

    function showPanelMargins(panel, item, marginX, marginY)
    {
        return showPanel(panel, item, Sk.BottomLeft, -1, -1, marginX, marginY, false);
    }

    //---------------------------------------------------------------------------------------------

    function showPanelAt(panel, item, x, y, isCursorChild)
    {
        return showPanel(panel, item, Sk.BottomLeft, x, y, 0, 0, isCursorChild);
    }

    //---------------------------------------------------------------------------------------------

    function hidePanels()
    {
        if (currentPanel == null) return;

        pClearParentContextual();

        pSetPanel(null);

        pClearLastPanel();

        isActive = false;
    }

    //---------------------------------------------------------------------------------------------

    function checkPanel(panel, item)
    {
        return (pLastPanel == panel && pLastParent == item);
    }

    //---------------------------------------------------------------------------------------------

    function clearLastParent()
    {
        pLastParent = null;
    }

    //---------------------------------------------------------------------------------------------
    // Private

    function pSetPanel(panel)
    {
        if (currentPanel == panel) return;

        if (currentPanel)
        {
            currentPanel.pSetActive(false);

            currentPanel.pSetItem(null);
        }

        currentPanel = panel;

        if (currentPanel)
        {
            for (var i = 0; i < children.length; i++)
            {
                var child = children[i];

                if (child != mouseWatcher && child != currentPanel)
                {
                    child.visible = false;
                }
            }
        }
    }

    function pUpdatePanel(item, position, x, y, marginX, marginY, isCursorChild)
    {
        if (currentPanel.item == item
            &&
            currentPanel.position == position
            &&
            currentPanel.posX == x && currentPanel.posY == y
            &&
            currentPanel.marginX == marginX && currentPanel.marginY == marginY
            &&
            currentPanel.isCursorChild == isCursorChild)
        {
            return;
        }

        currentPanel.pSetItem(item);

        currentPanel.position = position;

        currentPanel.posX = x;
        currentPanel.posY = y;

        currentPanel.marginX = marginX;
        currentPanel.marginY = marginY;

        currentPanel.isCursorChild = isCursorChild;

        currentPanel.updatePosition();

        currentPanel.pSetActive(true);

        pLastPanel = currentPanel;

        isActive = true;
    }

    //---------------------------------------------------------------------------------------------

    function pClearParentContextual()
    {
        if (parentContextual == null) return;

        parentContextual.onContextualClear();

        parentContextual = null;
    }

    function pClearLastPanel()
    {
        if (pLastPanel == null) return;

        pLastPanel.lastItem = null;

        pLastPanel = null;
    }

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    MouseWatcher
    {
        id: mouseWatcher

        anchors.fill: parent

        acceptedButtons: (isActive) ? Qt.LeftButton | Qt.RightButton
                                    : Qt.NoButton

        onPressed:
        {
            pClearParentContextual();

            pSetPanel(null);
        }

        onReleased:
        {
            if (currentPanel) return;

            pClearLastPanel();

            pLastParent = null;

            isActive = false;
        }
    }
}
