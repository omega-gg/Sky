//=================================================================================================
/*
    Copyright (C) 2015-2016 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkyPresentation module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

import QtQuick 1.1
import Sky     1.0

TextSvgScale
{
    id: slideText

    //---------------------------------------------------------------------------------------------
    // Properties style
    //---------------------------------------------------------------------------------------------

    property int durationAnimation: sp.slideText_durationAnimation

    property int easing: Easing.Linear

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    marginWidth : ds(textHeight / 4)
    marginHeight: ds(textHeight / 8)

    zoom: (slides) ? slides.ratio : 1.0

    horizontalAlignment: Text.AlignHCenter
    verticalAlignment  : Text.AlignVCenter

    loadMode: TextSvg.LoadAlways

    color: st.text_color

    style  : TextSvg.Glow
    outline: TextSvg.OutlineRound

    styleColor: (style == Text.Sunken) ? st.text_colorSunken
                                       : st.text_colorShadow

    styleSize: sp.slideText_styleSize

    scaleDelay: sp.slideText_scaleDelay

    font.family   : st.text_fontFamily
    font.pixelSize: sp.slideText_pixelSize
    font.bold     : true

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
