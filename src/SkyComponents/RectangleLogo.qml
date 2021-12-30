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

Rectangle
{
    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------
    // Style

    property real logoRatio: st.logoRatio

    property color colorA: st.logo_colorA
    property color colorB: st.logo_colorB

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias source: image.source

    //---------------------------------------------------------------------------------------------

    property alias image: image

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

    gradient: Gradient
    {
        GradientStop { position: 0.0; color: colorA }
        GradientStop { position: 1.0; color: colorB }
    }

    ImageScale
    {
        id: image

        anchors.fill: parent

        anchors.leftMargin : Math.round(parent.width / logoRatio)
        anchors.rightMargin: anchors.leftMargin

        source: st.logo

        fillMode: Image.PreserveAspectFit
    }
}
