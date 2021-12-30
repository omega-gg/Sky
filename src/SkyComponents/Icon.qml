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

ImageScale
{
    id: itemIcon

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property int style: st.icon_sunken

    property int styleSize: st.icon_styleSize

    //---------------------------------------------------------------------------------------------
    // Style

    property bool enableFilter: true

    property ImageFilterColor filterDefault: st.icon_filter
    property ImageFilterColor filterShadow : st.icon_filterShadow
    property ImageFilterColor filterSunken : st.icon_filterSunken

    //---------------------------------------------------------------------------------------------
    // Private

    property bool pStyleVisible: (enableFilter && style != Sk.IconNormal)

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias imageStyle: imageStyle

    //---------------------------------------------------------------------------------------------
    // Style

    property alias filter: itemIcon.filter

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    filter: (enableFilter) ? filterDefault : null

    scaling: false

    //---------------------------------------------------------------------------------------------
    // Animations
    //---------------------------------------------------------------------------------------------

    Behavior on opacity
    {
        PropertyAnimation
        {
            duration: st.duration_fast

            easing.type: st.easing
        }
    }

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

    ImageScale
    {
        id: imageStyle

        anchors.fill: parent

        anchors.topMargin   : (style == st.icon_raised) ?  styleSize : -styleSize
        anchors.bottomMargin: (style == st.icon_raised) ? -styleSize :  styleSize

        sourceSize : itemIcon.sourceSize
        defaultSize: itemIcon.defaultSize

        sourceArea: itemIcon.sourceArea

        z: -1

        visible: pStyleVisible

        source       : (pStyleVisible) ? itemIcon.source        : ""
        sourceDefault: (pStyleVisible) ? itemIcon.sourceDefault : ""

        loadMode: itemIcon.loadMode
        fillMode: itemIcon.fillMode

        asynchronous: itemIcon.asynchronous
        cache       : itemIcon.cache

        scaling: itemIcon.scaling

        filter: (style == st.icon_sunken) ? filterSunken
                                          : filterShadow
    }
}
