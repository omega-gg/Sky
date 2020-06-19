//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of Sky kit.

    - GNU Lesser General Public License Usage:
    This file may be used under the terms of the GNU Lesser General Public License version 3 as
    published by the Free Software Foundation and appearing in the LICENSE.md file included in the
    packaging of this file. Please review the following information to ensure the GNU Lesser
    General Public License requirements will be met: https://www.gnu.org/licenses/lgpl.html.
*/
//=================================================================================================

import QtQuick 1.0
import Sky     1.0

Text
{
    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    color: st.text_color

    styleColor: (style == st.text_sunken) ? st.text_colorSunken
                                          : st.text_colorShadow

    textFormat: Text.PlainText

    elide: Text.ElideRight

//#QT_5
    renderType: st.text_renderType
//#END

    font.family   : st.text_fontFamily
    font.pixelSize: st.text_pixelSize
    font.bold     : st.text_bold
}
