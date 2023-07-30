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

Item
{
    id: scroller

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    /* mandatory */ property variant scrollArea

    property bool isHoveredA: (mouseAreaUp.hoverActive
                               ||
                               (pMouseY != -1 && pMouseY < size))

    property bool isHoveredB: (mouseAreaDown.hoverActive
                               ||
                               (pMouseY != -1 && pMouseY >= height - size))

    /* read */ property int scrollDirection: -1

    //---------------------------------------------------------------------------------------------
    // Style

    property real scroll: st.scroller_scroll

    property int durationOpacity: st.scroller_durationOpacity

    property int intervalA: st.scroller_intervalA
    property int intervalB: st.scroller_intervalB

    property int size: st.scroller_size

    property real opacityA: st.scroller_opacityA
    property real opacityB: st.scroller_opacityB

    property ImageFilterColor filter: st.scroller_filter

    //---------------------------------------------------------------------------------------------
    // Private

    property int pMouseY: pGetMouseY()

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias mouseAreaUp  : mouseAreaUp
    property alias mouseAreaDown: mouseAreaDown

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------
    // Private

    function pGetMouseY()
    {
        if (visible && scrollArea.dragAccepted)
        {
            return window.mapToItem(scroller, -1, window.mouseY).y;
        }
        else return -1;
    }

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
    // Children
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

        cursor: Qt.PointingHandCursor

        Behavior on opacity
        {
            PropertyAnimation
            {
                duration: durationOpacity

                easing.type: st.easing
            }
        }

        RectangleShadow
        {
            anchors.fill: parent

            opacity: (isHoveredA) ? opacityB : opacityA

            direction: Sk.Down

            filter: scroller.filter

            Behavior on opacity
            {
                PropertyAnimation
                {
                    duration: durationOpacity

                    easing.type: st.easing
                }
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

        cursor: Qt.PointingHandCursor

        Behavior on opacity
        {
            PropertyAnimation
            {
                duration: durationOpacity

                easing.type: st.easing
            }
        }

        RectangleShadow
        {
            anchors.fill: parent

            opacity: (isHoveredB) ? opacityB : opacityA

            direction: Sk.Up

            filter: scroller.filter

            Behavior on opacity
            {
                PropertyAnimation
                {
                    duration: durationOpacity

                    easing.type: st.easing
                }
            }
        }
    }
}
