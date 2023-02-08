//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkyTouch.

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

ButtonWide
{
    id: buttonWideCheck

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property int margins: st.buttonWide_margins

    property int spacing: st.buttonTouch_spacing

    //---------------------------------------------------------------------------------------------
    // Private

    property int pMargins: margins / 2

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias backgroundCheck: backgroundCheck

    property alias itemCheck: itemCheck

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    // NOTE: We avoid highlighting when we're checked.
    isChecklighted: isHighlighted

    itemText.anchors.leftMargin: backgroundCheck.x + backgroundCheck.width + spacing

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    onClicked: onClick()

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------
    // Events

    function onClick()
    {
        checked = !(checked);
    }

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

    Rectangle
    {
        id: backgroundCheck

        width : parent.height - pMargins * 2
        height: width

        x: pMargins
        y: pMargins

        radius: buttonWideCheck.radius

        opacity: itemText.opacity

        color: st.buttonWideCheck_color

//#QT_4
        smooth: true
//#END

        border.width: st.buttonWideCheck_border
        border.color: st.buttonWideCheck_colorBorder
    }

    ImageScale
    {
        id: itemCheck

        anchors.centerIn: backgroundCheck

        visible: checked

        opacity: (buttonWideCheck.enabled) ? 1.0 : st.icon_opacityDisable

        sourceSize.height: st.getSizeMargins(buttonWideCheck.height, margins)

        source: st.icon_check

        filter: st.getButtonFilter(buttonWideCheck)
    }
}
