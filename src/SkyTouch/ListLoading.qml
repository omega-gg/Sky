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

    property int size: st.label_size

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    spacing: st.margins

    //---------------------------------------------------------------------------------------------
    // Animations
    //---------------------------------------------------------------------------------------------

    SequentialAnimation on opacity
    {
        running: (st.animate && visible)

        loops: Animation.Infinite

        PropertyAnimation
        {
            to: 0.4

            duration: 500

            easing.type: st.easing
        }

        PropertyAnimation
        {
            to: 1.0

            duration: 500

            easing.type: st.easing
        }
    }

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    Repeater
    {
        model: Math.ceil(listLoading.height / (size + spacing))

        Rectangle
        {
            id: rectangle

            width: listLoading.width

            height: size

            radius: st.radius

            color: st.lineEdit_color

//#QT_4
            smooth: true
//#END
        }
    }
}
