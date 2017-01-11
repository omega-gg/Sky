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

BaseButton
{
    id: buttonMask

    //---------------------------------------------------------------------------------------------
    // Properties style
    //---------------------------------------------------------------------------------------------

    property ImageColorFilter filterDefault: st.buttonMask_filterDefault
    property ImageColorFilter filterHover  : st.buttonMask_filterHover
    property ImageColorFilter filterPress  : st.buttonMask_filterPress

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias isIconDefault: itemIcon.isSourceDefault

    property alias icon       : itemIcon.source
    property alias iconDefault: itemIcon.sourceDefault

    property alias iconWidth : itemIcon.width
    property alias iconHeight: itemIcon.height

    property alias iconSourceSize: itemIcon.sourceSize
    property alias iconSourceArea: itemIcon.sourceArea

    property alias iconLoadMode: itemIcon.loadMode
    property alias iconFillMode: itemIcon.fillMode

    property alias iconAsynchronous: itemIcon.asynchronous
    property alias iconCache       : itemIcon.cache

    property alias iconScaling: itemIcon.scaling

    property alias iconStyle: itemIcon.style

    //---------------------------------------------------------------------------------------------

    property alias itemIcon: itemIcon

    //---------------------------------------------------------------------------------------------
    // Style

    property alias filterShadow: itemIcon.filterShadow
    property alias filterSunken: itemIcon.filterSunken

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    width : st.buttonMask_width
    height: st.buttonMask_height

    cursor: MouseArea.PointingHandCursor

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    Icon
    {
        id: itemIcon

        anchors.fill: parent

        sourceSize.height: (scaling) ? -1 : parent.height

        style    : Sk.IconRaised
        styleSize: st.buttonMask_styleSize

        filter:
        {
            if      (isPressed) return buttonMask.filterPress;
            else if (isHovered) return buttonMask.filterHover;
            else                return buttonMask.filterDefault;
        }
    }
}
