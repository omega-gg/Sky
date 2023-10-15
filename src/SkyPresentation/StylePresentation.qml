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
    // Animation

    property int easing: st.easing

    //---------------------------------------------------------------------------------------------
    // Tempo

    property real velocity: st.speed

    property real ratioVelocity: (st.animate && velocity) ? 1 / velocity : 0.0

    property int tempo: 128

    property real ratioTempo: (tempo) ? 60000 / tempo : 0.0

//#TimerTempo
    //---------------------------------------------------------------------------------------------
    // TimerTempo

    property int timerTempo_interval: 16
//#END

//#BaseLogo
    //---------------------------------------------------------------------------------------------
    // BaseLogo

    property real baseLogo_borderRatio: 32.0

    property color baseLogo_colorA: "#484848"
    property color baseLogo_colorB: baseLogo_colorA
//#END

//#ButtonBox
    //---------------------------------------------------------------------------------------------
    // ButtonBox

    property int buttonBox_size: st.dp256

    property int buttonBox_pixelSize: st.dp32

    property real buttonBox_opacity: 0.6

    property color buttonBox_color     : st.text_colorLink
    property color buttonBox_colorHover: st.text_colorLinkHover
//#END

//#Slides
    //---------------------------------------------------------------------------------------------
    // Slides

    property int slides_borderSize: 0

    property int slides_bpm: 1

    property int slides_duration: bpm(slides_bpm)
//#END

//#Slide*
    //---------------------------------------------------------------------------------------------
    // Slide

    property int slide_durationAnimation: bpm(2)

    property int slide_width : st.dp1920
    property int slide_height: st.dp1080
//#END

//#SlideGradient*
    //---------------------------------------------------------------------------------------------
    // SlideGradient

    property color slideGradient_colorA: "#323232"
    property color slideGradient_colorB: "#969696"

    property color slideGradient_colorBack: "#242424"
//#END

//#SlideBack
    //---------------------------------------------------------------------------------------------
    // SlideBack

    property real slideBack_opacity: 0.6

    property color slideBack_color: "black"
//#END

//#SlideFrame
    //---------------------------------------------------------------------------------------------
    // SlideFrame

    property color slideFrame_color: "#242424"
//#END

//#SlideText
    //---------------------------------------------------------------------------------------------
    // SlideText

    property int slideText_durationAnimation: bpm(1)

    property int slideText_scaleDelay: bpm(4)

    property int slideText_pixelSize: st.dp128
    property int slideText_styleSize: 8
//#END

//#SlidePlayer
    //---------------------------------------------------------------------------------------------
    // SlidePlayer

    property int slidePlayer_durationFadeIn : slide_durationAnimation
    property int slidePlayer_durationFadeOut: bpm(4)

    property int slidePlayer_interval: 10000
//#END

//#Pulse
    //---------------------------------------------------------------------------------------------
    // Pulse

    property int pulse_durationA: bpm(0.5)
    property int pulse_durationB: bpm(8)

    property real pulse_pulseOpacity: 1.0
//#END

//#PulseColor
    //---------------------------------------------------------------------------------------------
    // PulseColor

    property color pulseColor_color: "white"
//#END

//#PulseGradient
    //---------------------------------------------------------------------------------------------
    // PulseGradient

    property color pulseGradient_color    : pulseColor_color
    property color pulseGradient_colorBack: "black"
//#END

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function bpm(value)
    {
        return value * ratioTempo * ratioVelocity;
    }

    function toBpm(msecs)
    {
        return msecs / ratioTempo / ratioVelocity;
    }

    //---------------------------------------------------------------------------------------------

    // NOTE: This can be useful to check a bpm interval.
    function bpmCheck(bpm, bpmAt, duration)
    {
        return (bpm >= bpmAt && bpm < bpmAt + duration);
    }
}
