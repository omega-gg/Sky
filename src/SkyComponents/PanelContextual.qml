//=================================================================================================
/*
    Copyright (C) 2015-2016 Sky kit authors united with omega. <http://omega.gg/about>

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

Panel
{
    id: panelContextual

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    /* read */ property bool isActive: false

    /* read */ property bool isCursorChild: false

    /* read */ property variant item: null

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

    property int topMargin   : -(st.window_borderSize)
    property int bottomMargin: -(st.window_borderSize)
    property int leftMargin  : -(st.window_borderSize)
    property int rightMargin : -(st.window_borderSize)

    //---------------------------------------------------------------------------------------------

    property int panelWidth : -1
    property int panelHeight: -1

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    visible: false
    opacity: 0.0

    enableFocus: false

    backgroundOpacity: st.panelContextual_backgroundOpacity

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
    // Animations
    //---------------------------------------------------------------------------------------------

    Behavior on opacity
    {
        enabled: areaContextual.pAnimate

        PropertyAnimation { duration: st.duration_faster }
    }

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function updatePosition()
    {
        if (item == null) return;

        //-----------------------------------------------------------------------------------------
        // Position

        var x;
        var y;

        var parentWidth;
        var parentHeight;

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

            x = Math.min(x, areaContextual.width - parentWidth - rightMargin);
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

            y = Math.min(y, areaContextual.height - parentHeight - bottomMargin);
        }

        //-----------------------------------------------------------------------------------------
        // Size

        var width  = pGetWidth ();
        var height = pGetHeight();

        //-----------------------------------------------------------------------------------------
        // Checking size

        var widthBefore = x + parentWidth - leftMargin;

        var widthAfter = areaContextual.width - x - parentWidth - rightMargin;

        var heightBefore = y - topMargin;

        var heightAfter = areaContextual.height - y - parentHeight - bottomMargin;

        var panelLeft;
        var panelBottom;

        if (position == Sk.BottomLeft)
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
        else if (position == Sk.TopLeft)
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
        else // if (position == Sk.TopRight)
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

        //-----------------------------------------------------------------------------------------

        if (panelLeft) width = Math.min(width, widthBefore);
        else           width = Math.min(width, widthAfter);

        if (panelBottom) height = Math.min(height, heightAfter);
        else             height = Math.min(height, heightBefore);

        width  = Math.max(minimumWidth,  width);
        height = Math.max(minimumHeight, height);

        //-----------------------------------------------------------------------------------------

        if (panelLeft)
        {
             panelContextual.x = Math.round(x - width + parentWidth);
        }
        else panelContextual.x = Math.round(x);

        if (panelBottom)
        {
             panelContextual.y = Math.round(y + parentHeight);
        }
        else panelContextual.y = Math.round(y - height);

        panelWidth  = width;
        panelHeight = height;

        panelContextual.width  = width;
        panelContextual.height = height;
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

            panelContextual.width = width;
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

            panelContextual.height = height;
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
