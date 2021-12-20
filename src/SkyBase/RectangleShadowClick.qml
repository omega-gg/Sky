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

RectangleShadow
{
    id: rectangleShadowClick

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property real ratio: st.rectangleShadowClick_ratio

    //---------------------------------------------------------------------------------------------
    // Signals
    //---------------------------------------------------------------------------------------------

    signal entered
    signal exited

    signal pressed(variant mouse)
    signal clicked(variant mouse)

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    opacity: getOpacity()

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function getOpacity()
    {
        if (mouseArea.containsMouse)
        {
            if (mouseArea.pressed) return 1.0;
            else                   return 0.5;
        }
        else return 0.0;
    }

    //---------------------------------------------------------------------------------------------
    // Animations
    //---------------------------------------------------------------------------------------------

    Behavior on opacity
    {
        PropertyAnimation
        {
            duration: st.duration_normal

            easing.type: st.easing
        }
    }

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    MouseArea
    {
        id: mouseArea

        anchors.left : parent.left
        anchors.right: parent.right

        anchors.verticalCenter: parent.verticalCenter

        height: Math.round(parent.height / ratio)

        hoverEnabled: true

        cursor: Qt.PointingHandCursor

        onEntered: rectangleShadowClick.entered()
        onExited : rectangleShadowClick.exited ()

        QML_EVENT onPressed: function(mouse) { rectangleShadowClick.pressed(mouse); }
        QML_EVENT onClicked: function(mouse) { rectangleShadowClick.clicked(mouse); }
    }
}
