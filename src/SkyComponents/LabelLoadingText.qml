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

Item
{
    id: labelLoadingText

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property int margins: height / 8

    property int radius: background.height

    property int paddingLeft : st.labelLoadingText_padding
    property int paddingRight: st.labelLoadingText_padding

    property int spacing: st.labelLoadingText_spacing

    //---------------------------------------------------------------------------------------------
    // Style

    property color colorA: st.labelLoading_colorA
    property color colorB: st.labelLoading_colorB

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias isIconDefault: itemIcon.isSourceDefault

    property alias icon       : itemIcon.source
    property alias iconDefault: itemIcon.sourceDefault

    property alias iconWidth : itemIcon.width
    property alias iconHeight: itemIcon.height

    property alias iconSourceSize : itemIcon.sourceSize
    property alias iconDefaultSize: itemIcon.defaultSize

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
    // Children
    //---------------------------------------------------------------------------------------------

    Rectangle
    {
        id: background

        anchors.fill: parent

        anchors.margins: margins

        radius: labelLoadingText.radius

        opacity: st.labelLoading_opacity

        gradient: Gradient
        {
            GradientStop { position: 0.0; color: colorA }
            GradientStop { position: 1.0; color: colorB }
        }

//#QT_4
        smooth: true
//#END
    }

    IconLoading
    {
        id: itemIcon

        anchors.left: parent.left

        anchors.leftMargin: paddingLeft

        anchors.verticalCenter: parent.verticalCenter

        filterDefault: st.labelRound_filterIcon
    }

    TextBase
    {
        id: itemText

        anchors.left: itemIcon.right

        anchors.leftMargin: spacing

        anchors.verticalCenter: parent.verticalCenter

        color: st.labelLoadingText_colorText

        style: st.text_raised

        font.pixelSize: st.dp14
    }
}
