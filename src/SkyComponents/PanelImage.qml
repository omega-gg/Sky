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

import QtQuick 1.0
import Sky     1.0

Panel
{
    id: panelImage

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias source    : imageFront.source
    property alias sourceSize: imageFront.sourceSize

    property alias fillMode: imageFront.fillMode

    //---------------------------------------------------------------------------------------------

    property alias imageBack : imageBack
    property alias imageFront: imageFront

    property alias imageBackground: imageBack.image

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    width : imageFront.width  + borderSizeWidth
    height: imageFront.height + borders.sizeHeight

    opacity: (visible)

    acceptedButtons: Qt.NoButton

    hoverEnabled: false

    color: st.panelImage_color

    //---------------------------------------------------------------------------------------------
    // Animations
    //---------------------------------------------------------------------------------------------

    Behavior on opacity
    {
        PropertyAnimation
        {
            duration: st.duration_faster

            easing.type: st.easing
        }
    }

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    RectangleLogo
    {
        id: imageBack

        anchors.fill: imageFront

        visible: (imageFront.visible == false)
    }

    Image
    {
        id: imageFront

        anchors.fill: parent

        visible: (isSourceDefault == false)
    }
}
