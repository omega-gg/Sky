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

Column
{
    id: listLoading

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    /* mandatory */ property ListView list

    property bool active: false

    property int size: st.label_size

    property int radius: st.radius

    property int minimumCount: st.listLoading_minimumCount

    //---------------------------------------------------------------------------------------------
    // Style

    property int duration: st.listLoading_duration

    property real opacityA: st.listLoading_opacityA
    property real opacityB: st.listLoading_opacityB

    property color color: st.listLoading_color

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias repeater: repeater

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    anchors.left : list.left
    anchors.right: list.right
    anchors.top  : list.top

//#QT_4
    anchors.topMargin: -(list.contentY)
//#ELSE
    anchors.topMargin: -(list.originY + list.contentY)
//#END

    height: list.contentHeight + pGetHeight(minimumCount)

    spacing: st.margins

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

    function pGetHeight(count)
    {
        // NOTE: We only want spacing when the list has more than one item.
        if (list.count > 1)
        {
             return (size + spacing) * count;
        }
        else return size;
    }

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    Repeater
    {
        id: repeater

        model: (visible) ? Math.ceil(listLoading.height / (size + spacing)) : 0

        Rectangle
        {
            id: rectangle

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
