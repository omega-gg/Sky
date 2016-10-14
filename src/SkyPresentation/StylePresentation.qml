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

Item
{
    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------
    // Global

    property int dp256: st.dp(256)

    property int dp1920: st.dp(1920)
    property int dp1080: st.dp(1080)

    //---------------------------------------------------------------------------------------------

    property real velocity: st.speed

    property real ratioVelocity: (st.animate && velocity) ? 1 / velocity : 0.0

    property int tempo: 128

    property real ratioTempo: (tempo) ? 60000 / tempo : 0.0

    //---------------------------------------------------------------------------------------------
    // TimerTempo

    property int timerTempo_interval: 16

    //---------------------------------------------------------------------------------------------
    // BaseLogo

    property real baseLogo_marginRatio: 32

    property url baseLogo_sourceHighlight: "pictures/highlight.svg"

    property color baseLogo_colorA: "#8c8c8c"
    property color baseLogo_colorB: "#2a2a2a"

    property color baseLogo_colorBack: "#161616"

    //---------------------------------------------------------------------------------------------
    // ButtonBox

    property real buttonBox_size: dp256

    property real buttonBox_pixelSize: st.dp32

    property real buttonBox_opacity: 0.6

    property color buttonBox_color     : st.textRich_colorLink
    property color buttonBox_colorHover: st.textRich_colorLinkHover

    //---------------------------------------------------------------------------------------------
    // Slides

    property real slides_borderSize: st.dp8

    property int slides_duration: bpm(1)

    //---------------------------------------------------------------------------------------------
    // Slide

    property int slide_durationAnimation: bpm(2)

    property real slide_width : dp1920
    property real slide_height: dp1080

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

    property real slideText_pixelSize: st.dp128
    property real slideText_styleSize: st.dp8

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
