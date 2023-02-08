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

Scanner
{
    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property real ratioTouch: st.itemScan_ratioTouch

    //---------------------------------------------------------------------------------------------
    // Private

//#DESKTOP
    // NOTE: We don't want to update the rectangle position while clicking.
    property bool pHoverable: (mouseArea.containsMouse
                               &&
                               pClick == false && rectangleTag.isAnimated == false)
//#END

    property bool pClick: false

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

//#DESKTOP
    property alias mouseArea: mouseArea
//#END

    property alias rectangleTag: rectangleTag

    //---------------------------------------------------------------------------------------------
    // Signals
    //---------------------------------------------------------------------------------------------

    signal clicked(string text)

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    visible: (player.visible || cover.visible)

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    /* QML_EVENT */ onLoaded: function(text, rect)
    {
        if (text)
        {
            rectangleTag.x      = rect.x;
            rectangleTag.y      = rect.y;
            rectangleTag.width  = rect.width;
            rectangleTag.height = rect.height;

            if (pClick)
            {
                pClick = false;

                rectangleTag.click();

                clicked(text);
            }
//#DESKTOP
            else rectangleTag.hovered = true;
//#END
        }
        else if (pClick)
        {
            pClick = false;

            clicked("");
        }
//#DESKTOP
        else rectangleTag.hovered = false;
//#END
    }

//#DESKTOP
    onWidthChanged : pRestart()
    onHeightChanged: pRestart()
//#END

    //---------------------------------------------------------------------------------------------
    // Connections
    //---------------------------------------------------------------------------------------------

//#DESKTOP
    Connections
    {
        target: (visible && window.isDragged == false) ? window : null

        onMousePosChanged: pRestartHover()
    }

    Connections
    {
        target: (visible) ? cover : null

        onLoaded: pRestart()

        // NOTE: Clear the rectangle when we switch between the player and the cover.
        onVisibleChanged: pClearHover()
    }
//#END

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function click(x, y)
    {
        // NOTE: We check a larger surface on touch interactions.
        if (window.isTouching)
        {
            var ratio = Math.min(player.width, player.height) / ratioTouch;

            pClick = scanFrame(x, y, ratio, 5);
        }
        else pClick = scanFrame(x, y, 1, 5);

        return pClick;
    }

    //---------------------------------------------------------------------------------------------
    // Private

//#DESKTOP
    function pRestart()
    {
        if (pHoverable == false) return;

        timer.interval = st.itemScan_intervalA;

        timer.restart();
    }

    function pRestartHover()
    {
        if (pHoverable == false) return;

        timer.interval = st.itemScan_intervalB;

        timer.restart();
    }

    function pScan()
    {
        if (pHoverable) scanFrame(window.mouseX, window.mouseY);
    }

    function pClearHover()
    {
        timer.stop();

        rectangleTag.hovered = false;
    }
//#END

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

//#DESKTOP
    Timer
    {
        id: timer

        onTriggered: pScan()
    }

    Timer
    {
        interval: st.itemScan_intervalA

        repeat: true

        running: (player.isPlaying && rectangleTag.isAnimated == false)

        onTriggered: pScan()
    }

    MouseArea
    {
        id: mouseArea

        anchors.fill: parent

        hoverEnabled: true

        cursor: Qt.PointingHandCursor

        /* QML_EVENT */ onPressed: function(mouse) { mouse.accepted = false }

        onContainsMouseChanged: if (containsMouse == false) pClearHover()

        // NOTE: 'onContainsMouseChanged' does not work when the cursor leaves the window.
        onHoverActiveChanged: if (hoverActive == false) pClearHover()
    }
//#END

    RectangleTag
    {
        id: rectangleTag

//#DESKTOP
        // NOTE: Updating the rectangle position after clicking.
        onIsAnimatedChanged: if (isAnimated == false) pRestart()
//#END
    }
}
