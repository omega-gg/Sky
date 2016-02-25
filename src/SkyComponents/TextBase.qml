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

Text
{
    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property real leftMargin  : 0
    property real rightMargin : 0
    property real topMargin   : 0
    property real bottomMargin: 0

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    anchors.leftMargin : leftMargin  + st.text_fontMarginWidth
    anchors.rightMargin: rightMargin - st.text_fontMarginWidth

    anchors.topMargin   : topMargin    + st.text_fontMarginHeight
    anchors.bottomMargin: bottomMargin - st.text_fontMarginHeight

    color: st.text_color

    styleColor: (style == Text.Sunken) ? st.text_colorSunken
                                       : st.text_colorShadow

    styleSize: st.ratio

    textFormat: Text.PlainText

    elide: Text.ElideRight

    font.family   : st.text_fontFamily
    font.pixelSize: st.text_pixelSize
    font.bold     : true
}
