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

BaseWindow
{
    id: window

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    /* mandatory */ property variant st: null

    property variant areaContextual: null

    property bool resizable: true

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

    //---------------------------------------------------------------------------------------------
    // Style

    property alias color   : content.color
    property alias gradient: content.gradient

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    onPressed: clearFocus()

    onActiveChanged: if (isActive == false) clearContextual()

    //---------------------------------------------------------------------------------------------
    // Animations
    //---------------------------------------------------------------------------------------------

    Behavior on width
    {
        enabled: pAnimate

        PropertyAnimation { duration: durationAnimation }
    }

    Behavior on height
    {
        enabled: pAnimate

        PropertyAnimation { duration: durationAnimation }
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
    // Childs
    //---------------------------------------------------------------------------------------------

    Rectangle
    {
        id: content

        anchors.fill: parent

        anchors.leftMargin  : borderLeft
        anchors.rightMargin : borderRight
        anchors.topMargin   : borderTop
        anchors.bottomMargin: borderBottom

        color: st.window_color
    }

    RectangleBorders
    {
        id: borders

        anchors.fill: parent

        size: (maximized == false && fullScreen == false) ? st.window_borderSize : 0
    }

    ViewResizer
    {
        id: resizer

        anchors.fill: parent

        size: st.window_resizerSize

        visible: (resizable && maximized == false && fullScreen == false)
    }
}
