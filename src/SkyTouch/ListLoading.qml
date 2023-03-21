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
    id: listLoading

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    /* mandatory */ property ListView list

    property int size: st.label_size

    property int radius: st.radius

    //---------------------------------------------------------------------------------------------
    // Style

    property int duration: st.listLoading_duration

    property real opacityA: st.listLoading_opacityA
    property real opacityB: st.listLoading_opacityB

    property color color: st.listLoading_color

    //---------------------------------------------------------------------------------------------
    // Private

//#QT_4
    property real pContentY: (visible) ? list.contentY : 0
//#ELSE
    property real pContentY: (visible) ? list.originY + list.contentY : 0
//#END

    property int pItemSize: size + column.spacing

    property int pCount: (visible) ? Math.floor(pContentY / pItemSize) : 0

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias column  : column
    property alias repeater: repeater

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    anchors.left : list.left
    anchors.right: list.right
    anchors.top  : list.top

    anchors.topMargin: -pContentY

    height: list.contentHeight + size

    visible: false

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

            duration: listLoading.duration

            easing.type: st.easing
        }

        PropertyAnimation
        {
            to: opacityB

            duration: listLoading.duration

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
            return pCount * pItemSize;
        }
        else return 0;
    }

    function pGetCount()
    {
        if (visible == false) return 0;

        var count = list.count;

        if (count == 0) return 1;

        var size = Math.ceil((list.height + pContentY - column.y) / pItemSize);

        return Math.min(size, count - pCount + 1);
    }

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

    Column
    {
        id: column

        y: pGetY()

        spacing: list.spacing

        Repeater
        {
            id: repeater

            model: pGetCount()

            Rectangle
            {
                width: listLoading.width

                height: size

                radius: listLoading.radius

                color: listLoading.color

//#QT_4
                smooth: true
//#END
            }
        }
    }
}
