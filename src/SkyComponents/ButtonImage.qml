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
