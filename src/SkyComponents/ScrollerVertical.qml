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

Item
{
    id: scroller

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    /* mandatory */ property variant scrollArea

    property bool isHoveredA: (mouseAreaUp.containsMouse
                               ||
                               (visible
                                &&
                                scrollArea.dragAccepted && scrollArea.mouseY < size))

    property bool isHoveredB: (mouseAreaDown.containsMouse
                               ||
                               (visible
                                &&
                                scrollArea.dragAccepted && scrollArea.mouseY >= height - size))

    /* read */ property int scrollDirection: -1

    //---------------------------------------------------------------------------------------------
    // Style

    property real scroll: st.scroller_scroll

    property int durationOpacity: st.scroller_durationOpacity

    property int intervalA: st.scroller_intervalA
    property int intervalB: st.scroller_intervalB

    property real size: st.scroller_size

    property real opacityA: st.scroller_opacityA
    property real opacityB: st.scroller_opacityB

    property ImageColorFilter filter: st.scroller_filter

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias mouseAreaUp  : mouseAreaUp
    property alias mouseAreaDown: mouseAreaDown

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    anchors.fill: scrollArea

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    onIsHoveredAChanged:
    {
        if (isHoveredA)
        {
            scrollDirection = Sk.Up;

            timer.interval = intervalA;

            timer.start();
        }
        else if (isHoveredB == false)
        {
            timer.stop();

            scrollDirection = -1;
        }
    }

    onIsHoveredBChanged:
    {
        if (isHoveredB)
        {
            scrollDirection = Sk.Down;

            timer.interval = intervalA;

            timer.start();
        }
        else if (isHoveredA == false)
        {
            timer.stop();

            scrollDirection = -1;
        }
    }

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    Timer
    {
        id: timer

        repeat: true

        onTriggered:
        {
            if (scrollDirection == Sk.Up)
            {
                 scrollArea.scrollUp(scroll);
            }
            else scrollArea.scrollDown(scroll);

            interval = intervalB;
        }
    }

    MouseArea
    {
        id: mouseAreaUp

        anchors.left : parent.left
        anchors.right: parent.right

        anchors.rightMargin: scrollArea.scrollBar.width

        height: size

        opacity: (scrollArea.atTop == false)

        acceptedButtons: Qt.NoButton

        hoverEnabled: true

        cursor: MouseArea.PointingHandCursor

        Behavior on opacity
        {
            PropertyAnimation { duration: durationOpacity }
        }

        RectangleShadow
        {
            anchors.fill: parent

            opacity: (isHoveredA) ? opacityB : opacityA

            direction: Sk.Down

            filter: scroller.filter

            Behavior on opacity
            {
                PropertyAnimation { duration: durationOpacity }
            }
        }
    }

    MouseArea
    {
        id: mouseAreaDown

        anchors.left  : parent.left
        anchors.right : parent.right
        anchors.bottom: parent.bottom

        anchors.rightMargin: scrollArea.scrollBar.width

        height: size

        opacity: (scrollArea.atBottom == false)

        acceptedButtons: Qt.NoButton

        hoverEnabled: true

        cursor: MouseArea.PointingHandCursor

        Behavior on opacity
        {
            PropertyAnimation { duration: durationOpacity }
        }

        RectangleShadow
        {
            anchors.fill: parent

            opacity: (isHoveredB) ? opacityB : opacityA

            direction: Sk.Up

            filter: scroller.filter

            Behavior on opacity
            {
                PropertyAnimation { duration: durationOpacity }
            }
        }
    }
}
