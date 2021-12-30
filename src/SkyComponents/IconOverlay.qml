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
    id: iconOverlay

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------
    // Style

    property color colorA: st.iconOverlay_colorA
    property color colorB: st.iconOverlay_colorB

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias source       : itemIcon.source
    property alias sourceDefault: itemIcon.sourceDefault

    property alias sourceSize : itemIcon.sourceSize
    property alias defaultSize: itemIcon.defaultSize

    property alias sourceArea: itemIcon.sourceArea

    property alias loadMode: itemIcon.loadMode
    property alias fillMode: itemIcon.fillMode

    property alias asynchronous: itemIcon.asynchronous
    property alias cache       : itemIcon.cache

    property alias scaling: itemIcon.scaling

    property alias style: itemIcon.style

    property alias backgroundOpacity: background.opacity

    //---------------------------------------------------------------------------------------------

    property alias background: background
    property alias itemIcon  : itemIcon

    //---------------------------------------------------------------------------------------------
    // Style

    property alias filter: itemIcon.filter

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

    Rectangle
    {
        id: background

        anchors.fill: parent

        opacity: st.iconOverlay_opacity

        gradient: Gradient
        {
            GradientStop { position: 0.0; color: colorA }
            GradientStop { position: 1.0; color: colorB }
        }
    }

    Icon
    {
        id: itemIcon

        anchors.centerIn: parent

        sourceSize.height: (scaling) ? -1 : parent.height

        style: st.icon_raised

        filter: st.iconOverlay_filter
    }
}
