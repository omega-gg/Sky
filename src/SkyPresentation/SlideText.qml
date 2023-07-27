//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkyPresentation.

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

//#QT_4
TextSvgScale
//#ELSE
TextSvg
//#END
{
    id: slideText

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property string family   : st.text_fontFamily
    property int    pixelSize: sp.slideText_pixelSize
    property bool   bold     : true
    property bool   italic   : false

    //---------------------------------------------------------------------------------------------
    // Style

    property int durationAnimation: sp.slideText_durationAnimation

    property int easing: sp.easing

    //---------------------------------------------------------------------------------------------
    // Private

    property bool pSlides: (typeof slides != "undefined")

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    marginWidth : (pSlides) ? ds(textHeight / 4) : 0
    marginHeight: (pSlides) ? ds(textHeight / 8) : 0

    zoom: (pSlides) ? slides.ratio : 1.0

    horizontalAlignment: Text.AlignHCenter
    verticalAlignment  : Text.AlignVCenter

    loadMode: TextSvg.LoadAlways

    color: st.text_color

    style  : TextSvg.Glow
    outline: TextSvg.OutlineRound

    styleColor: (style == st.text_sunken) ? st.text_colorSunken
                                          : st.text_colorShadow

    styleSize: sp.slideText_styleSize

    // NOTE: This makes sure the font width metric is accurate.
    multiplier: 1.01

    scaleDelay: sp.slideText_scaleDelay

    font.family   : family
    font.pixelSize: pixelSize
    font.bold     : bold
    font.italic   : italic

    //---------------------------------------------------------------------------------------------
    // Animations
    //---------------------------------------------------------------------------------------------

    Behavior on opacity
    {
        PropertyAnimation
        {
            duration: (st.animate) ? durationAnimation : 0

            easing.type: slideText.easing
        }
    }
}
