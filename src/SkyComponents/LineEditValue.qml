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

LineEditLabel
{    
    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias value: labelValue.text

    property alias valueWidth: labelValue.width

    //---------------------------------------------------------------------------------------------

    property alias labelValue: labelValue

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    lineEdit.anchors.right: labelValue.left

    lineEdit.anchors.rightMargin: lineEdit.anchors.leftMargin

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    LabelRoundInfo
    {
        id: labelValue

        anchors.right : parent.right
        anchors.top   : parent.top
        anchors.bottom: parent.bottom

        paddingLeft : label.paddingRight
        paddingRight: label.paddingLeft

        z: -1
    }
}
