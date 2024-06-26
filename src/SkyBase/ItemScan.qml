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
    id: itemScan

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

//#DESKTOP
    property bool isHoverable: (sk.cursorVisible && mouseArea.containsMouse)
//#END

    property real ratioTouch: st.itemScan_ratioTouch

//#DESKTOP
    property int cursor: Qt.ArrowCursor
//#END

    //---------------------------------------------------------------------------------------------
    // Private

    property bool pClick: false

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

//#DESKTOP
    property alias isHovered: scannerHover.visible
//#END

    property alias isClicking: rectangleTag.visible

//#DESKTOP
    property alias containsMouse: mouseArea.containsMouse
//#END

    //---------------------------------------------------------------------------------------------

//#DESKTOP
    property alias mouseArea: mouseArea
//#END

    property alias rectangleTag: rectangleTag

    //---------------------------------------------------------------------------------------------
    // Style

    property alias durationAnimation: rectangleTag.durationAnimation

    //---------------------------------------------------------------------------------------------
    // Signals
    //---------------------------------------------------------------------------------------------

//#DESKTOP
    signal positionChanged(variant mouse)
//#END

    signal clicked(string text)

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    visible: ((player && player.visible) || (cover && cover.visible))

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    onLoaded:
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

                rectangleTag.clickStart();

                clicked(text);

                rectangleTag.clickEnd();
            }
//#DESKTOP
            else scannerHover.apply(topLeft, topRight, bottomLeft, bottomRight);
//#END
        }
        else if (pClick)
        {
            pClick = false;

            clicked("");
        }
//#DESKTOP
        else scannerHover.clear();
//#END
    }

//#DESKTOP
    onWidthChanged : pRestart()
    onHeightChanged: pRestart()

    onIsHoverableChanged:
    {
        if (isHoverable)
        {
            scan();
        }
        else clearHover();
    }
//#END

    //---------------------------------------------------------------------------------------------
    // Connections
    //---------------------------------------------------------------------------------------------

//#DESKTOP
    Connections
    {
        target: (visible && window.isDragged == false) ? window : null

        /* QML_CONNECTION */ function onMousePosChanged() { pRestartHover() }
    }

    Connections
    {
        target: (visible) ? cover : null

        /* QML_CONNECTION */ function onLoaded() { pRestart() }

        // NOTE: Clear the rectangle when we switch between the player and the cover.
        /* QML_CONNECTION */ function onVisibleChanged() { clearHover() }
    }
//#END

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function scan()
    {
        var position = window.mapToItem(itemScan, window.mouseX, window.mouseY);

        scanFrame(position.x, position.y);
    }

    function click()
    {
        var position = window.mapToItem(itemScan, window.mouseX, window.mouseY);

        return clickAt(position.x, position.y);
    }

    function clickAt(x, y)
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

//#DESKTOP
    function clearHover()
    {
        timer.stop();

        pClick = false;

        scannerHover.clear();
    }

    function getTextHovered()
    {
        if (isHovered == false) return "";

        var position = window.mapToItem(rectangleTag, window.mouseX, window.mouseY);

        var x = position.x;
        var y = position.y;

        if (x < 0 || y < 0
            ||
            x >= rectangleTag.width || y >= rectangleTag.height) return "";

        return text;
    }
//#END

    //---------------------------------------------------------------------------------------------
    // Private

//#DESKTOP
    function pRestart()
    {
        if (isHoverable == false) return;

        timer.interval = st.itemScan_intervalA;

        timer.restart();
    }

    function pRestartHover()
    {
        if (isHoverable == false) return;

        timer.interval = st.itemScan_intervalB;

        timer.restart();
    }
//#END

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

//#DESKTOP
    Timer
    {
        id: timer

        onTriggered: scan()
    }

    Timer
    {
        interval: st.itemScan_intervalA

        repeat: true

        running: (player && player.isPlaying && rectangleTag.isAnimated == false)

        onTriggered: scan()
    }

    MouseArea
    {
        id: mouseArea

        anchors.fill: parent

        acceptedButtons: Qt.NoButton

        hoverEnabled: true

        cursor: (isHovered || isClicking) ? Qt.PointingHandCursor
                                          : itemScan.cursor

        // NOTE: 'onContainsMouseChanged' does not work when the cursor leaves the window.
        onHoverActiveChanged: if (hoverActive == false) clearHover()

        /* QML_EVENT */ onPositionChanged: function(mouse) { itemScan.positionChanged(mouse); }
    }
//#END

//#DESKTOP
    ScannerHover
    {
        id: scannerHover

        anchors.fill: parent

        // NOTE: We don't want to hover a tag while clicking.
        visible: (isActive && isClicking == false)

        opacity: st.rectangleTag_opacity

        color: st.color_highlight
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
