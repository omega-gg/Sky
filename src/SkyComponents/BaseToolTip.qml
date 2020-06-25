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

Panel
{
    id: toolTip

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    /* read */ property bool isActive: false

    //---------------------------------------------------------------------------------------------

    property int minimumWidth : st.baseToolTip_minimumWidth  + borderSizeWidth
    property int minimumHeight: st.baseToolTip_minimumHeight + borderSizeHeight

    property int maximumWidth : -1
    property int maximumHeight: -1

    property int preferredWidth : -1
    property int preferredHeight: -1

    property int marginWidth : st.baseToolTip_marginWidth
    property int marginHeight: st.baseToolTip_marginHeight

    property int position: 1

    //---------------------------------------------------------------------------------------------
    // Private

    property int pMarginWidthHalf : marginWidth  / 2
    property int pMarginHeightHalf: marginHeight / 2

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    visible: false
    opacity: 0.0

    acceptedButtons: Qt.NoButton

    hoverEnabled: false

    backgroundOpacity: st.baseToolTip_backgroundOpacity

    gradient: Gradient
    {
        GradientStop { position: 0.0; color: st.baseToolTip_colorA }
        GradientStop { position: 1.0; color: st.baseToolTip_colorB }
    }

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    onMinimumWidthChanged : updatePosition()
    onMinimumHeightChanged: updatePosition()

    onMaximumWidthChanged : updatePosition()
    onMaximumHeightChanged: updatePosition()

    onPreferredWidthChanged : updatePosition()
    onPreferredHeightChanged: updatePosition()

    //---------------------------------------------------------------------------------------------
    // Connections
    //---------------------------------------------------------------------------------------------

    Connections
    {
        target: (isActive && window.isDragged == false) ? window : null

        onScaleChanged: pUpdatePosition()

        onOriginXChanged: pUpdatePosition()
        onOriginYChanged: pUpdatePosition()

        onMousePosChanged: pUpdatePosition()

        onContentWidthChanged : pUpdatePosition()
        onContentHeightChanged: pUpdatePosition()
    }

    //---------------------------------------------------------------------------------------------
    // Animations
    //---------------------------------------------------------------------------------------------

    Behavior on opacity
    {
        PropertyAnimation
        {
            duration: st.duration_faster

            easing.type: st.easing
        }
    }

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function show()
    {
        pActivate();
    }

    function hide()
    {
        if (isActive == false) return;

        isActive = false;

        visible = false;
        opacity = 0.0;
    }

    //---------------------------------------------------------------------------------------------

    function updatePosition()
    {
        if (isActive) pUpdatePosition();
    }

    //---------------------------------------------------------------------------------------------
    // Private

    function pActivate()
    {
        if (isActive) return;

        isActive = true;

        pUpdatePosition();
    }

    //---------------------------------------------------------------------------------------------

    function pUpdatePosition()
    {
        if (window.isResizing || (window.mouseX < 0 || window.mouseX > window.width
                                  ||
                                  window.mouseY < 0 || window.mouseY > window.height))
        {
            visible = false;
            opacity = 0.0;

            return;
        }

        var width  = Math.min(pGetWidth (), window.contentWidth);
        var height = Math.min(pGetHeight(), window.contentHeight);

        width  = Math.max(minimumWidth,  width);
        height = Math.max(minimumHeight, height);

        var x;
        var y;

        if (position == 0 || position == 1)
        {
             x = Math.round(window.contentMouseX() + marginWidth);
        }
        else x = Math.round(window.contentMouseX() - width - pMarginWidthHalf);

        if (position == 1 || position == 2)
        {
             y = Math.round(window.contentMouseY() + marginHeight);
        }
        else y = Math.round(window.contentMouseY() - height - pMarginHeightHalf);

        var oldX = x;
        var oldY = y;

        x = Math.max(0, x);
        y = Math.max(0, y);

        toolTip.x = Math.min(x, window.contentWidth  - width);
        toolTip.y = Math.min(y, window.contentHeight - height);

        toolTip.width  = width;
        toolTip.height = height;

        visible = true;

        if (toolTip.x == oldX || toolTip.y == oldY)
        {
             opacity = 1.0;
        }
        else opacity = st.baseToolTip_opacityHover;
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
}
