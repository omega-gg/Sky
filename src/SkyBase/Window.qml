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

BaseWindow
{
    id: window

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    /* mandatory */ property variant st: null

    property variant areaContextual: null

    property bool resizable: true

    /* read */ property bool isTouchActive: false

    //---------------------------------------------------------------------------------------------
    // Style

    property int durationAnimation: st.window_durationAnimation

    //---------------------------------------------------------------------------------------------
    // Private

    property bool pAnimate: false

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    default property alias content: content.data

    property alias contentWidth : content.width
    property alias contentHeight: content.height

    property alias resizerSize: resizer.size

    //---------------------------------------------------------------------------------------------

    property alias borderSize : borders.size
    property alias borderColor: borders.color

    property alias borderLeft  : borders.borderLeft
    property alias borderRight : borders.borderRight
    property alias borderTop   : borders.borderTop
    property alias borderBottom: borders.borderBottom

    property alias borderSizeWidth : borders.sizeWidth
    property alias borderSizeHeight: borders.sizeHeight

    //---------------------------------------------------------------------------------------------

    property alias borders: borders
    property alias resizer: resizer

    //---------------------------------------------------------------------------------------------
    // Style

    property alias color   : content.color
    property alias gradient: content.gradient

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    onPressed: clearFocus()

    onActiveChanged: if (isActive == false) clearContextual()

    onResizingChanged:
    {
        if (isResizing)
        {
            if (isTouching) pTouchShow();
        }
        else if (isTouchActive)
        {
            timer.restart();
        }
    }

    //---------------------------------------------------------------------------------------------
    // Animations
    //---------------------------------------------------------------------------------------------

    Behavior on width
    {
        enabled: pAnimate

        PropertyAnimation
        {
            duration: durationAnimation

            easing.type: st.easing
        }
    }

    Behavior on height
    {
        enabled: pAnimate

        PropertyAnimation
        {
            duration: durationAnimation

            easing.type: st.easing
        }
    }

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function resize(width, height, animate)
    {
        pAnimate = animate;

        window.width  = width;
        window.height = height;

        pAnimate = false;
    }

    //---------------------------------------------------------------------------------------------

    function resizeWidth(width, animate)
    {
        pAnimate = animate;

        window.width = width;

        pAnimate = false;
    }

    function resizeHeight(height, animate)
    {
        pAnimate = animate;

        window.height = height;

        pAnimate = false;
    }

    //---------------------------------------------------------------------------------------------

    function showTouch()
    {
        pTouchShow();

        timer.restart();
    }

    function hideTouch()
    {
        timer.stop();

        pTouchClear();
    }

    function toggleTouch()
    {
        if (isTouchActive)
        {
            hideTouch();
        }
        else showTouch();
    }

    //---------------------------------------------------------------------------------------------

    function clearContextual()
    {
        if (areaContextual) areaContextual.hidePanels();
    }

    //---------------------------------------------------------------------------------------------

    function contentMouseX()
    {
        return (mouseX + originX - borderLeft) / scale;
    }

    function contentMouseY()
    {
        return (mouseY + originY - borderTop) / scale;
    }

    //---------------------------------------------------------------------------------------------

    function saveShotContent(fileName)
    {
        return saveShot(fileName, borderLeft, borderTop, contentWidth, contentHeight);
    }

    function saveShotItem(fileName, item)
    {
        var pos = mapFromItem(item, 0, 0);

        return saveShot(fileName, pos.x, pos.y, item.width, item.height);
    }

    //---------------------------------------------------------------------------------------------
    // Virtual

    /* virtual */ function getMargin(size)
    {
        if (size)
        {
             return st.window_borderSize;
        }
        else return 0;
    }

    /* virtual */ function getBorderSize()
    {
        if (isTouchActive)
        {
             return st.window_borderSizeTouch;
        }
        else return st.window_borderSize;
    }

    //---------------------------------------------------------------------------------------------
    // Private

    function pTouchShow()
    {
        pAnimate = true;

        isTouchActive = true;

        pAnimate = false;
    }

    function pTouchClear()
    {
        if (isResizing) return;

        pAnimate = true;

        isTouchActive = false;

        pAnimate = false;
    }

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    Timer
    {
        id: timer

        interval: st.window_intervalTouch

        onTriggered: pTouchClear()
    }

    Rectangle
    {
        id: content

        anchors.fill: parent

        anchors.leftMargin  : getMargin(borderLeft)
        anchors.rightMargin : getMargin(borderRight)
        anchors.topMargin   : getMargin(borderTop)
        anchors.bottomMargin: getMargin(borderBottom)

        color: st.window_color
    }

    RectangleBorders
    {
        id: borders

        anchors.fill: parent

//#DESKTOP
        size: (maximized == false && fullScreen == false) ? getBorderSize() : 0
//#ELSE
        size: 0
//#END

        color: (isTouchActive) ? st.border_colorFocus
                               : st.border_color

        Behavior on size
        {
            enabled: pAnimate

            PropertyAnimation
            {
                duration: durationAnimation

                easing.type: st.easing
            }
        }
    }

    ViewResizer
    {
        id: resizer

        anchors.fill: parent

        size: (isTouchActive) ? st.window_resizerSizeTouch
                              : st.window_resizerSize

        visible: (resizable && maximized == false && fullScreen == false)
    }
}
