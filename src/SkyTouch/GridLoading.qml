//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkyTouch.

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
    id: gridLoading

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    /* mandatory */ property GridView grid

    property bool active: false

    property int cellWidth : grid.cellWidth
    property int cellHeight: grid.cellHeight

    property int radius: st.radius

    property int spacing: grid.spacing

    //---------------------------------------------------------------------------------------------
    // Style

    property int duration: st.listLoading_duration

    property real opacityA: st.listLoading_opacityA
    property real opacityB: st.listLoading_opacityB

    property color color: st.listLoading_color

    //---------------------------------------------------------------------------------------------
    // Private

//#QT_4
    property real pContentY: (visible) ? grid.contentY : 0
//#ELSE
    property real pContentY: (visible) ? grid.originY + grid.contentY : 0
//#END

    property int pCountX: (visible) ? Math.floor(width     / cellWidth)  : 0
    property int pCountY: (visible) ? Math.floor(pContentY / cellHeight) : 0

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias spacingBottom: column.spacing

    property alias column  : column
    property alias repeater: repeater

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    anchors.left : grid.left
    anchors.right: grid.right
    anchors.top  : grid.top

    anchors.topMargin: -pContentY

    height: grid.contentHeight + cellHeight

    // NOTE: We want to hide this component when the opacity is at the lowest.
    visible: (active == true || opacity != opacityA)

    opacity: opacityA

    //---------------------------------------------------------------------------------------------
    // Animations
    //---------------------------------------------------------------------------------------------

    SequentialAnimation on opacity
    {
        running: (st.animate && visible)

        loops: Animation.Infinite

        PropertyAnimation
        {
            to: opacityA

            duration: gridLoading.duration

            easing.type: st.easing
        }

        PropertyAnimation
        {
            to: opacityB

            duration: gridLoading.duration

            easing.type: st.easing
        }
    }

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------
    // Private

    function pGetY()
    {
        if (visible && pContentY > 0)
        {
            return pCountY * cellHeight;
        }
        else return 0;
    }

    function pGetCount()
    {
        if (visible == false) return 0;

        var count = grid.count;

        if (count == 0) return 1;

        var size = Math.ceil((grid.height + pContentY - column.y) / cellHeight);

        return Math.min(size, Math.ceil(count / pCountX - pCountY + 1));
    }

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

    Column
    {
        id: column

        y: pGetY()

        spacing: gridLoading.spacing

        Repeater
        {
            id: repeater

            model: pGetCount()

            Row
            {
                spacing: gridLoading.spacing

                Repeater
                {
                    model: pCountX

                    Rectangle
                    {
                        width : cellWidth  - spacing
                        height: cellHeight - spacingBottom

                        radius: gridLoading.radius

                        color: gridLoading.color

//#QT_4
                        smooth: true
//#END
                    }
                }
            }
        }
    }
}
