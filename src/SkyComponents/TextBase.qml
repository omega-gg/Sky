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

import QtQuick 1.0
import Sky     1.0

Text
{
    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property int leftMargin  : 0
    property int rightMargin : 0
    property int topMargin   : 0
    property int bottomMargin: 0

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    anchors.leftMargin  : leftMargin
    anchors.rightMargin : rightMargin
    anchors.topMargin   : topMargin
    anchors.bottomMargin: bottomMargin

    color: st.text_color

    styleColor: (style == st.text_sunken) ? st.text_colorSunken
                                          : st.text_colorShadow

    textFormat: Text.PlainText

    elide: Text.ElideRight

//#QT_5
    renderType: Text.NativeRendering
//#END

    font.family   : st.text_fontFamily
    font.pixelSize: st.text_pixelSize
    font.bold     : true
}
