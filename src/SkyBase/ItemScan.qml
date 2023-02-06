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

    /* mandatory */ property Player player
    /* mandatory */ property Image  cover

    property real ratioTouch: st.itemScan_ratioTouch

    //---------------------------------------------------------------------------------------------
    // Private

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

//#DESKTOP
    onWidthChanged : pRestart()
    onHeightChanged: pRestart()
//#END

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

    //---------------------------------------------------------------------------------------------
    // Connections
    //---------------------------------------------------------------------------------------------

//#DESKTOP
    Connections
    {
        target: (visible && window.isDragged == false) ? window : null

        onMousePosChanged: pRestart()
    }

    Connections
    {
        target: (visible) ? cover : null

        onLoaded: pRestart()
    }
//#END

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function click(x, y)
    {
        pClick = true;

        // NOTE: We check a larger surface on touch interactions.
        if (window.isTouching)
        {
            var ratio = Math.min(player.width, player.height) / ratioTouch;

            return scanFrame(player, cover, x, y, ratio);
        }
        else return scanFrame(player, cover, x, y);
    }

    //---------------------------------------------------------------------------------------------
    // Private

//#DESKTOP
    function pRestart()
    {
        // NOTE: We don't want to update the rectangle position while clicking.
        if (rectangleTag.isAnimated) return;

        timer.restart();
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

        interval: st.itemScan_intervalA

        onTriggered: scanFrame(player, cover, window.mouseX, window.mouseY)
    }

    Timer
    {
        interval: st.itemScan_intervalB

        repeat: true

        running: (player.isPlaying && rectangleTag.isAnimated == false)

        onTriggered: scanFrame(player, cover, window.mouseX, window.mouseY)
    }

    MouseArea
    {
        id: mouseArea

        anchors.fill: parent

        hoverEnabled: true

        cursor: Qt.PointingHandCursor

        /* QML_EVENT */ onPressed: function(mouse) { mouse.accepted = false }

        onHoverActiveChanged: pClearHover()
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
