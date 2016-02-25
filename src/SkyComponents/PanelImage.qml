//=================================================================================================
/*
    Copyright (C) 2015-2016 Sky kit authors united with omega. <http://omega.gg/about>

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

Panel
{
    id: panelImage

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias source    : itemImage.source
    property alias sourceSize: itemImage.sourceSize

    property alias fillMode: itemImage.fillMode

    //---------------------------------------------------------------------------------------------

    property alias backgroundImage: backgroundImage
    property alias itemImage      : itemImage

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    width : itemImage.width  + borderSizeWidth
    height: itemImage.height + borders.sizeHeight

    opacity: (visible)

    acceptedButtons: Qt.NoButton

    hoverEnabled: false

    color: st.panelImage_color

    //---------------------------------------------------------------------------------------------
    // Animations
    //---------------------------------------------------------------------------------------------

    Behavior on opacity
    {
        PropertyAnimation { duration: st.duration_faster }
    }

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    RectangleLogo
    {
        id: backgroundImage

        anchors.fill: itemImage

        visible: (itemImage.visible == false)
    }

    Image
    {
        id: itemImage

        anchors.fill: parent

        visible: (isSourceDefault == false)
    }
}
