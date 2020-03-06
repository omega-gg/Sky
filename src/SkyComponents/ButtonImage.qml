//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors united with omega. <http://omega.gg/about>

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

BaseButton
{
    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property bool enableFilter: true

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias isSourceDefault: itemImage.isSourceDefault

    property alias source       : itemImage.source
    property alias sourceDefault: itemImage.sourceDefault

    property alias imageWidth : itemImage.width
    property alias imageHeight: itemImage.height

    property alias sourceSize: itemImage.sourceSize
    property alias sourceArea: itemImage.sourceArea

    property alias loadMode: itemImage.loadMode
    property alias fillMode: itemImage.fillMode

    property alias asynchronous: itemImage.asynchronous
    property alias cache       : itemImage.cache

    property alias scaling: itemImage.scaling

    //---------------------------------------------------------------------------------------------
    // Style

    property alias filter: itemImage.filter

    //---------------------------------------------------------------------------------------------

    property alias itemImage: itemImage

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    width : itemImage.width
    height: itemImage.height

    cursor: Qt.PointingHandCursor

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    ImageScale { id: itemImage }
}
