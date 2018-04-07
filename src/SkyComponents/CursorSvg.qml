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

BaseCursor
{
    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias source: image.source

    property alias loadMode: image.loadMode
    property alias fillMode: image.fillMode

    property alias scaling: image.scaling

    //---------------------------------------------------------------------------------------------

    property alias image: image

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

//#QT_4
    ImageSvgScale
//#ELSE
    ImageSvg
//#END
    {
        id: image

        anchors.fill: parent

//#QT_4
        scaleDelay: 0
//#END
    }
}
