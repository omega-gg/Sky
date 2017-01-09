//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkyPresentation module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

import QtQuick       1.1
import Sky           1.0
import SkyComponents 1.0

Rectangle
{
    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property int margin: height / sp.baseLogo_marginRatio

    property color colorA: sp.baseLogo_colorA
    property color colorB: sp.baseLogo_colorB

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias sourceHighlight: highlight.source

    //---------------------------------------------------------------------------------------------

    property alias shadow: shadow

    property alias rectangle: rectangle
    property alias highlight: highlight

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    color: sp.baseLogo_colorBack

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    BorderImageShadow { id: shadow }

    Rectangle
    {
        id: rectangle

        anchors.fill: parent

        anchors.margins: margin

        gradient: Gradient
        {
            GradientStop { position: 0.0; color: colorA }
            GradientStop { position: 1.0; color: colorB }
        }

        ImageSvgScale
        {
            id: highlight

            anchors.fill: parent

            source: sp.baseLogo_sourceHighlight
        }
    }
}
