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

Panel
{
    id: panelImage

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias source: imageFront.source

    property alias sourceSize : imageFront.sourceSize
    property alias defaultSize: imageFront.defaultSize

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

    acceptedButtons: Qt.NoButton

    hoverEnabled: false

    color: st.panelImage_color

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

    RectangleLogo
    {
        id: imageBack

        anchors.fill: imageFront

        visible: (imageFront.visible == false)
    }

    SkyImage
    {
        id: imageFront

        anchors.fill: parent

        visible: (isSourceDefault == false)
    }
}
