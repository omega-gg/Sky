//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

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
    id: iconOverlay

    //---------------------------------------------------------------------------------------------
    // Properties style
    //---------------------------------------------------------------------------------------------

    property color colorA: st.iconOverlay_colorA
    property color colorB: st.iconOverlay_colorB

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias source       : itemIcon.source
    property alias sourceDefault: itemIcon.sourceDefault

    property alias sourceSize: itemIcon.sourceSize
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
    // Childs
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

        style: Sk.IconRaised

        filter: st.icon_filter
    }
}
