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

Item
{
    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property int style: Sk.IconSunken

    property int shadowSize: st.icon_shadowSize

    //---------------------------------------------------------------------------------------------
    // Style

    property bool enableFilter: true

    property ImageColorFilter filterDefault: st.icon_filter
    property ImageColorFilter filterShadow : st.icon_filterShadow
    property ImageColorFilter filterSunken : st.icon_filterSunken

    //---------------------------------------------------------------------------------------------
    // Private

    property bool pShadowVisible: (enableFilter && style != Sk.IconNormal)

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias isSourceDefault: itemIcon.isSourceDefault

    property alias source       : itemIcon.source
    property alias sourceDefault: itemIcon.sourceDefault

    property alias iconWidth : itemIcon.width
    property alias iconHeight: itemIcon.height

    property alias sourceSize: itemIcon.sourceSize
    property alias sourceArea: itemIcon.sourceArea

    property alias ratioWidth : itemIcon.ratioWidth
    property alias ratioHeight: itemIcon.ratioHeight

    property alias loadMode: itemIcon.loadMode
    property alias fillMode: itemIcon.fillMode

    property alias asynchronous: itemIcon.asynchronous
    property alias cache       : itemIcon.cache

    property alias scaling: itemIcon.scaling

    //---------------------------------------------------------------------------------------------

    property alias shadow  : shadow
    property alias itemIcon: itemIcon

    //---------------------------------------------------------------------------------------------
    // Style

    property alias filter: itemIcon.filter

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    width : itemIcon.width
    height: itemIcon.height

    //---------------------------------------------------------------------------------------------
    // Animations
    //---------------------------------------------------------------------------------------------

    Behavior on opacity
    {
        PropertyAnimation { duration: st.duration_fast }
    }

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    ImageScale
    {
        id: shadow

        anchors.fill: parent

        anchors.topMargin   : (style == Sk.IconRaised) ?  shadowSize : -shadowSize
        anchors.bottomMargin: (style == Sk.IconRaised) ? -shadowSize :  shadowSize

        sourceSize: (itemIcon.isExplicitSize) ? itemIcon.sourceSize : undefined

        sourceArea: itemIcon.sourceArea

        visible: pShadowVisible

        source       : (pShadowVisible) ? itemIcon.source        : ""
        sourceDefault: (pShadowVisible) ? itemIcon.sourceDefault : ""

        loadMode: itemIcon.loadMode
        fillMode: itemIcon.fillMode

        asynchronous: itemIcon.asynchronous
        cache       : itemIcon.cache

        scaling: itemIcon.scaling

        filter: (style == Sk.IconSunken) ? filterSunken
                                         : filterShadow
    }

    ImageScale
    {
        id: itemIcon

        filter: (enableFilter) ? filterDefault : null

        scaling: false
    }
}
