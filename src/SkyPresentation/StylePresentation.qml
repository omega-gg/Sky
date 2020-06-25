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

Item
{
    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------
    // Tempo

    property real velocity: st.speed

    property real ratioVelocity: (st.animate && velocity) ? 1 / velocity : 0.0

    property int tempo: 128

    property real ratioTempo: (tempo) ? 60000 / tempo : 0.0

    //---------------------------------------------------------------------------------------------
    // TimerTempo

    property int timerTempo_interval: 16

    //---------------------------------------------------------------------------------------------
    // BaseLogo

    property real baseLogo_borderRatio: 32.0

    property color baseLogo_colorA: "#484848"
    property color baseLogo_colorB: baseLogo_colorA

    //---------------------------------------------------------------------------------------------
    // ButtonBox

    property int buttonBox_size: st.dp256

    property int buttonBox_pixelSize: st.dp32

    property real buttonBox_opacity: 0.6

    property color buttonBox_color     : st.text_colorLink
    property color buttonBox_colorHover: st.text_colorLinkHover

    //---------------------------------------------------------------------------------------------
    // Slides

    property int slides_borderSize: st.dp8

    property int slides_duration: bpm(1)

    //---------------------------------------------------------------------------------------------
    // Slide

    property int slide_durationAnimation: bpm(2)

    property int slide_width : st.dp1920
    property int slide_height: st.dp1080

    //---------------------------------------------------------------------------------------------
    // SlideGradient

    property color slideGradient_colorA: "#323232"
    property color slideGradient_colorB: "#969696"

    property color slideGradient_colorBack: "#242424"

    //---------------------------------------------------------------------------------------------
    // SlideBackground

    property real slideBackground_opacity: 0.6

    property color slideBackground_color: "black"

    //---------------------------------------------------------------------------------------------
    // SlideFrame

    property color slideFrame_color: "#242424"

    //---------------------------------------------------------------------------------------------
    // SlideText

    property int slideText_durationAnimation: bpm(1)

    property int slideText_scaleDelay: bpm(4)

    property int slideText_pixelSize: st.dp128
    property int slideText_styleSize: st.dp8

    //---------------------------------------------------------------------------------------------
    // SlidePlayer

    property int slidePlayer_durationFadeIn : slide_durationAnimation
    property int slidePlayer_durationFadeOut: bpm(4)

    property int slidePlayer_interval: 10000

    //---------------------------------------------------------------------------------------------
    // Pulse

    property int pulse_durationA: bpm(0.5)
    property int pulse_durationB: bpm(8)

    property real pulse_pulseOpacity: 1.0

    //---------------------------------------------------------------------------------------------
    // PulseColor

    property color pulseColor_color: "white"

    //---------------------------------------------------------------------------------------------
    // PulseGradient

    property color pulseGradient_color    : "white"
    property color pulseGradient_colorBack: "black"

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function bpm(value)
    {
        return value * ratioTempo * ratioVelocity;
    }
}
