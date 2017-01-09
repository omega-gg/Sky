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

Item
{
    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property int paddingLeft : st.labelLoadingText_padding
    property int paddingRight: st.labelLoadingText_padding

    property int spacing: st.labelLoadingText_spacing

    //---------------------------------------------------------------------------------------------
    // Style

    property variant borderBackground: st.labelLoadingText_borderBackground

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias isIconDefault: itemIcon.isSourceDefault

    property alias icon       : itemIcon.source
    property alias iconDefault: itemIcon.sourceDefault

    property alias iconWidth : itemIcon.iconWidth
    property alias iconHeight: itemIcon.iconHeight

    property alias iconSourceSize: itemIcon.sourceSize
    property alias iconSourceArea: itemIcon.sourceArea

    property alias iconLoadMode: itemIcon.loadMode
    property alias iconFillMode: itemIcon.fillMode

    property alias iconAsynchronous: itemIcon.asynchronous
    property alias iconCache       : itemIcon.cache

    property alias iconScaling: itemIcon.scaling

    property alias iconStyle: itemIcon.style

    property alias text: itemText.text
    property alias font: itemText.font

    //---------------------------------------------------------------------------------------------

    property alias background: background

    property alias itemIcon: itemIcon
    property alias itemText: itemText

    //---------------------------------------------------------------------------------------------
    // Style

    property alias durationAnimation: itemIcon.durationAnimation

    property alias enableFilter: itemIcon.enableFilter

    property alias filterIcon      : itemIcon.filterDefault
    property alias filterIconShadow: itemIcon.filterShadow
    property alias filterIconSunken: itemIcon.filterSunken

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    width: paddingLeft + itemIcon.width + spacing + itemText.width + paddingRight + st.dp8

    height: st.labelLoadingText_height

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    BorderImageScale
    {
        id: background

        anchors.fill: parent

        opacity: st.labelLoadingText_opacity

        source: st.labelLoadingText_sourceBackground

        border
        {
            left : borderBackground.x;     top   : borderBackground.y;
            right: borderBackground.width; bottom: borderBackground.height
        }
    }

    IconLoading
    {
        id: itemIcon

        anchors.left: parent.left

        anchors.leftMargin: paddingLeft

        anchors.verticalCenter: parent.verticalCenter
    }

    TextBase
    {
        id: itemText

        anchors.left: itemIcon.right

        leftMargin: spacing

        anchors.verticalCenter: parent.verticalCenter

        style: Text.Raised

        font.pixelSize: st.dp14
    }
}
