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

BasePanel
{
    id: panel

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    /* read */ property bool isActive: false

    /* read */ property bool isCursorChild: false

    /* read */ property variant item: null

    // NOTE: This is useful when we want to avoid showing twice the same panel in a 'clicked' event.
    /* read */ property variant lastItem: null

    /* read */ property int position: -1

    /* read */ property int posX: -1
    /* read */ property int posY: -1

    /* read */ property int marginX: 0
    /* read */ property int marginY: 0

    //---------------------------------------------------------------------------------------------

    property int minimumWidth : st.dp32
    property int minimumHeight: st.dp32

    property int maximumWidth : -1
    property int maximumHeight: -1

    property int preferredWidth : -1
    property int preferredHeight: -1

    //---------------------------------------------------------------------------------------------

    property int margins: 0

    property int leftMargin  : margins
    property int rightMargin : margins
    property int topMargin   : margins
    property int bottomMargin: margins

    //---------------------------------------------------------------------------------------------

    /* read */ property int panelWidth : -1
    /* read */ property int panelHeight: -1

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    visible: false
    opacity: 0.0

    enableFocus: false

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    onMinimumWidthChanged : pUpdateWidth ()
    onMinimumHeightChanged: pUpdateHeight()

    onMaximumWidthChanged : pUpdateWidth ()
    onMaximumHeightChanged: pUpdateHeight()

    onPreferredWidthChanged : pUpdateWidth ()
    onPreferredHeightChanged: pUpdateHeight()

    //---------------------------------------------------------------------------------------------
    // Keys
    //---------------------------------------------------------------------------------------------

    Keys.onPressed:
    {
        if (event.key == Qt.Key_Escape)
        {
            event.accepted = true;

            hidePanels();
        }
    }

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function updatePosition()
    {
        if (item == null) return;

        //-----------------------------------------------------------------------------------------
        // Position

        var areaWidth  = areaContextual.width;
        var areaHeight = areaContextual.height;

        var parentWidth;
        var parentHeight;

        var x;
        var y;

        if (posX == -1)
        {
            parentWidth = item.width;

            x = areaContextual.mapFromItem(item, 0, 0).x + marginX;
        }
        else
        {
            if (isCursorChild) parentWidth = st.cursor_width;
            else               parentWidth = 0;

            x = Math.max(leftMargin, posX);

            x = Math.min(x, areaWidth - parentWidth - rightMargin);
        }

        if (posY == -1)
        {
            parentHeight = item.height;

            y = areaContextual.mapFromItem(item, 0, 0).y + marginY;
        }
        else
        {
            if (isCursorChild) parentHeight = st.cursor_height;
            else               parentHeight = 0;

            y = Math.max(topMargin, posY);

            y = Math.min(y, areaHeight - parentHeight - bottomMargin);
        }

        //-----------------------------------------------------------------------------------------
        // Size

        var width  = pGetWidth ();
        var height = pGetHeight();

        //-----------------------------------------------------------------------------------------
        // Checking size

        var widthBefore = x + parentWidth - leftMargin;

        var widthAfter = areaWidth - x - parentWidth - rightMargin;

        var heightBefore = y - topMargin;

        var heightAfter = areaHeight - y - parentHeight - bottomMargin;

        var panelLeft;
        var panelBottom;

        if (position == Sk.TopLeft)
        {
            panelLeft   = pGetPanelLeft(width,  widthBefore,  widthAfter);
            panelBottom = pGetPanelTop (height, heightBefore, heightAfter);

            if (posX == -1 && panelLeft == false)
            {
                x -= marginX;
            }

            if (posY == -1 && panelBottom)
            {
                y -= marginY;
            }
        }
        else if (position == Sk.TopRight)
        {
            panelLeft   = pGetPanelRight(width,  widthBefore,  widthAfter);
            panelBottom = pGetPanelTop  (height, heightBefore, heightAfter);

            if (posX == -1 && panelLeft)
            {
                x -= marginX;
            }

            if (posY == -1 && panelBottom)
            {
                y -= marginY;
            }
        }
        else if (position == Sk.BottomLeft)
        {
            panelLeft   = pGetPanelLeft  (width,  widthBefore,  widthAfter);
            panelBottom = pGetPanelBottom(height, heightBefore, heightAfter);

            if (posX == -1 && panelLeft == false)
            {
                x -= marginX;
            }

            if (posY == -1 && panelBottom == false)
            {
                y -= marginY;
            }
        }
        else if (position == Sk.BottomRight)
        {
            panelLeft   = pGetPanelRight (width,  widthBefore,  widthAfter);
            panelBottom = pGetPanelBottom(height, heightBefore, heightAfter);

            if (posX == -1 && panelLeft)
            {
                x -= marginX;
            }

            if (posY == -1 && panelBottom == false)
            {
                y -= marginY;
            }
        }
        else if (position == Sk.TopLeftCorner)
        {
            // NOTE: We need to take the item size into account for Corner positionning.
            widthBefore  -= parentWidth;
            heightBefore += parentHeight;
            heightAfter  += parentHeight;

            panelLeft   = pGetPanelLeft(width,  widthBefore,  widthAfter);
            panelBottom = pGetPanelTop (height, heightBefore, heightAfter);

            if (posX == -1)
            {
                if (panelLeft)
                {
                     x -= parentWidth;
                }
                else x += parentWidth - marginX * 2;
            }

            if (posY == -1)
            {
                if (panelBottom)
                {
                     y -= parentHeight + marginY * 2;
                }
                else y += parentHeight;
            }
        }
        else if (position == Sk.TopRightCorner)
        {
            // NOTE: We need to take the item size into account for Corner positionning.
            heightBefore += parentHeight;
            heightAfter  += parentHeight;

            panelLeft   = pGetPanelRight(width,  widthBefore,  widthAfter);
            panelBottom = pGetPanelTop  (height, heightBefore, heightAfter);

            if (posX == -1)
            {
                if (panelLeft)
                {
                     x -= parentWidth + marginX * 2;
                }
                else x += parentWidth;
            }

            if (posY == -1)
            {
                if (panelBottom)
                {
                     y -= parentHeight + marginY * 2;
                }
                else y += parentHeight;
            }
        }
        else if (position == Sk.BottomLeftCorner)
        {
            // NOTE: We need to take the item size into account for Corner positionning.
            widthBefore  -= parentWidth;
            heightBefore += parentHeight;
            heightAfter  += parentHeight;

            panelLeft   = pGetPanelLeft  (width,  widthBefore,  widthAfter);
            panelBottom = pGetPanelBottom(height, heightBefore, heightAfter);

            if (posX == -1)
            {
                if (panelLeft)
                {
                     x -= parentWidth;
                }
                else x += parentWidth - marginX * 2;
            }

            if (posY == -1)
            {
                if (panelBottom)
                {
                     y -= parentHeight;
                }
                else y += parentHeight - marginY * 2;
            }
        }
        else // if (position == Sk.BottomRightCorner)
        {
            // NOTE: We need to take the item size into account for Corner positionning.
            heightBefore += parentHeight;
            heightAfter  += parentHeight;

            panelLeft   = pGetPanelRight (width,  widthBefore,  widthAfter);
            panelBottom = pGetPanelBottom(height, heightBefore, heightAfter);

            if (posX == -1)
            {
                if (panelLeft)
                {
                     x -= parentWidth + marginX * 2;
                }
                else x += parentWidth;
            }

            if (posY == -1)
            {
                if (panelBottom)
                {
                     y -= parentHeight;
                }
                else y += parentHeight - marginY * 2;
            }
        }

        //-----------------------------------------------------------------------------------------

        if (panelLeft) width = Math.min(width, widthBefore);
        else           width = Math.min(width, widthAfter);

        if (panelBottom) height = Math.min(height, heightAfter);
        else             height = Math.min(height, heightBefore);

        width  = Math.max(minimumWidth,  width);
        height = Math.max(minimumHeight, height);

        width  = Math.min(width,  areaWidth);
        height = Math.min(height, areaHeight);

        //-----------------------------------------------------------------------------------------

        if (panelLeft)
        {
             x = Math.round(x - width + parentWidth);
        }
        else x = Math.round(x);

        if (panelBottom)
        {
             y = Math.round(y + parentHeight);
        }
        else y = Math.round(y - height);

        x = Math.max(0, x);
        y = Math.max(0, y);

        //-----------------------------------------------------------------------------------------

        panelWidth  = width;
        panelHeight = height;

        panel.width  = width;
        panel.height = height;

        panel.x = x;
        panel.y = y;
    }

    //---------------------------------------------------------------------------------------------
    // Private

    function pSetActive(active)
    {
        if (active)
        {
            visible = true;
            opacity = 1.0;

            isActive = true;
        }
        else
        {
            visible = false;
            opacity = 0.0;

            isActive = false;
        }
    }

    function pSetItem(item)
    {
        lastItem = panel.item;

        panel.item = item;
    }

    //---------------------------------------------------------------------------------------------

    function pUpdateWidth()
    {
        if (isCursorChild == false || posX == -1)
        {
            updatePosition();

            return;
        }

        if (item == null) return;

        var width = Math.max(minimumWidth, pGetWidth());

        if ((x + width) < (areaContextual.width - rightMargin))
        {
            panelWidth = width;

            panel.width = width;
        }
        else updatePosition();
    }

    function pUpdateHeight()
    {
        if (isCursorChild == false || posY == -1)
        {
            updatePosition();

            return;
        }

        if (item == null) return;

        var height = Math.max(minimumHeight, pGetHeight());

        if ((y + height) < (areaContextual.height - bottomMargin))
        {
            panelHeight = height;

            panel.height = height;
        }
        else updatePosition();
    }

    //---------------------------------------------------------------------------------------------

    function pGetWidth()
    {
        if (maximumWidth != -1)
        {
            return Math.min(preferredWidth, maximumWidth);
        }
        else return preferredWidth;
    }

    function pGetHeight()
    {
        if (maximumHeight != -1)
        {
            return Math.min(preferredHeight, maximumHeight);
        }
        else return preferredHeight;
    }

    //---------------------------------------------------------------------------------------------

    function pGetPanelLeft(width, before, after)
    {
        if (before >= width)
        {
            return true;
        }
        else if (after > before)
        {
            return false;
        }
        else return true;
    }

    function pGetPanelRight(width, before, after)
    {
        if (after >= width)
        {
            return false;
        }
        else if (before > after)
        {
            return true;
        }
        else return false;
    }

    //---------------------------------------------------------------------------------------------

    function pGetPanelTop(height, before, after)
    {
        if (before >= height)
        {
            return false;
        }
        else if (after > before)
        {
            return true;
        }
        else return false;
    }

    function pGetPanelBottom(height, before, after)
    {
        if (after >= height)
        {
            return true;
        }
        else if (before > after)
        {
            return false;
        }
        else return true;
    }
}
