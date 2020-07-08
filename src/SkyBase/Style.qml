//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkyBase.

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
    // Global

    property real scale: 1.0
    property real zoom : 1.0

    property real ratio: scale * zoom * window.ratio

    //---------------------------------------------------------------------------------------------

    property url icon
    property url logo

    property color color_highlight: "#008cdc"

    property int cursor_width : dp18
    property int cursor_height: dp24

    //---------------------------------------------------------------------------------------------
    // Speed

    property real speed: speed_normal

    property real ratioSpeed: (animate && speed) ? 1 / speed : 0.0

    property real speed_slower: 0.25
    property real speed_slow  : 0.5
    property real speed_normal: 1.0
    property real speed_fast  : 2.0
    property real speed_faster: 4.0

    //---------------------------------------------------------------------------------------------
    // Animation

    property bool animate: true

    property int easing: Easing.OutQuad

    property int duration_slower: ms300
    property int duration_slow  : ms250
    property int duration_normal: ms200
    property int duration_fast  : ms150
    property int duration_faster: ms100

    property int ms100: 100 * ratioSpeed
    property int ms150: 150 * ratioSpeed

    property int ms200: 200 * ratioSpeed
    property int ms250: 250 * ratioSpeed

    property int ms300: 300 * ratioSpeed
    property int ms500: 500 * ratioSpeed
    property int ms800: 800 * ratioSpeed

    property int ms1000: 1000 * ratioSpeed

    //---------------------------------------------------------------------------------------------
    // Text

    property int text_pixelSize: dp12

    property bool text_bold: false

    property real text_opacityDisable: 0.6

    property string text_fontFamily: (sk.osMac) ? "" : "Arial"

    property int text_raised: Text.Normal
    property int text_sunken: text_raised

//#QT_5
    property int text_renderType: Text.QtRendering
//#END

    property color text_color         : text1_color
    property color text_colorCurrent  : "#008cdc"
    property color text_colorShadow   : text1_colorShadow
    property color text_colorSunken   : text1_colorSunken
    property color text_colorLink     : text_colorCurrent
    property color text_colorLinkHover: "#00b4ff"

    property color text1_color      : "#484848"
    property color text1_colorShadow: "#161616"
    property color text1_colorSunken: "#323232"

    property color text2_color      : "white"
    property color text2_colorShadow: text1_colorShadow
    property color text2_colorSunken: text1_colorSunken

    property color text3_color      : "#646464"
    property color text3_colorShadow: text1_colorShadow
    property color text3_colorSunken: text1_colorSunken

    //---------------------------------------------------------------------------------------------
    // Border

    property int border_size     : dp1
    property int border_sizeFocus: border_size

    property color border_color     : "#969696"
    property color border_colorLight: "#b4b4b4"
    property color border_colorFocus: "#00a0f0"

    property alias border_filter     : border_filter
    property alias border_filterFocus: border_filterFocus

    ImageColorFilter
    {
        id: border_filter

        color: border_color
    }

    ImageColorFilter
    {
        id: border_filterFocus

        color: border_colorFocus
    }

    //---------------------------------------------------------------------------------------------
    // Icon

    property int icon_styleSize: dp1

    property real icon_opacityDisable: 0.5

    property int icon_raised: Sk.IconNormal
    property int icon_sunken: icon_raised

    property color icon_colorA      : icon1_colorA
    property color icon_colorB      : icon1_colorB
    property color icon_colorActiveA: "#008cdc"
    property color icon_colorActiveB: icon_colorActiveA
    property color icon_colorShadow : icon1_colorShadow
    property color icon_colorSunken : icon1_colorSunken

    property color icon1_colorA     : "#646464"
    property color icon1_colorB     : icon1_colorA
    property color icon1_colorShadow: "#161616"
    property color icon1_colorSunken: "#323232"

    property color icon2_colorA     : "white"
    property color icon2_colorB     : icon2_colorA
    property color icon2_colorShadow: icon1_colorShadow
    property color icon2_colorSunken: icon1_colorSunken

    property alias icon_filter      : icon_filter
    property alias icon_filterActive: icon_filterActive
    property alias icon_filterShadow: icon_filterShadow
    property alias icon_filterSunken: icon_filterSunken

    property alias icon1_filter      : icon_filter
    property alias icon1_filterShadow: icon_filterShadow
    property alias icon1_filterSunken: icon_filterSunken

    property alias icon2_filter      : icon2_filter
    property alias icon2_filterShadow: icon2_filterShadow
    property alias icon2_filterSunken: icon2_filterSunken

    //---------------------------------------------------------------------------------------------

    ImageColorFilter
    {
        id: icon_filter

        gradient: ScaleGradient
        {
            ScaleGradientStop { position: 0.0; color: icon_colorA }
            ScaleGradientStop { position: 1.0; color: icon_colorB }
        }
    }

    ImageColorFilter
    {
        id: icon_filterActive

        gradient: ScaleGradient
        {
            ScaleGradientStop { position: 0.0; color: icon_colorActiveA }
            ScaleGradientStop { position: 1.0; color: icon_colorActiveB }
        }
    }

    ImageColorFilter
    {
        id: icon_filterShadow

        color: icon_colorShadow
    }

    ImageColorFilter
    {
        id: icon_filterSunken

        color: icon_colorSunken
    }

    //---------------------------------------------------------------------------------------------

    ImageColorFilter
    {
        id: icon2_filter

        gradient: ScaleGradient
        {
            ScaleGradientStop { position: 0.0; color: icon2_colorA }
            ScaleGradientStop { position: 1.0; color: icon2_colorB }
        }
    }

    ImageColorFilter
    {
        id: icon2_filterShadow

        color: icon2_colorShadow
    }

    ImageColorFilter
    {
        id: icon2_filterSunken

        color: icon2_colorSunken
    }

//#Window
    //---------------------------------------------------------------------------------------------
    // Window

    property int window_durationAnimation: duration_fast

    property int window_intervalTouch: 3000

    property int window_borderSize     : border_size
    property int window_borderSizeTouch: dp8

    property int window_resizerSize     : dp4
    property int window_resizerSizeTouch: dp16

    property color window_color: "#b4b4b4"

//#RectangleBorders
    //---------------------------------------------------------------------------------------------
    // RectangleBorders

    property int rectangleBorders_size: border_size
//#END

//#BaseLineEdit
    //---------------------------------------------------------------------------------------------
    // BaseLineEdit

    property int baseLineEdit_durationCursor: ms500

    property int baseLineEdit_height: dp32

    property int baseLineEdit_cursorWidth: dp2

    property int baseLineEdit_maximumLength: 1000

    property bool baseLineEdit_autoSelect: true

    property color baseLineEdit_colorCursor: "#161616"

    property color baseLineEdit_colorText         : text1_color
    property color baseLineEdit_colorTextSelection: border_colorFocus
    property color baseLineEdit_colorTextSelected : text2_color

    property color baseLineEdit_colorDefault: text3_color

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function dp(value)
    {
        return Math.round(value * ratio);
    }

    function ab(value)
    {
        return Math.round(value / ratio);
    }

    //---------------------------------------------------------------------------------------------

    function size(valueA, valueB)
    {
        return Qt.size(dp(valueA), dp(valueB));
    }

    //---------------------------------------------------------------------------------------------

    function vector(pathA, pathB)
    {
        if (ratio == 1.0) return pathA;
        else              return pathB;
    }

    //---------------------------------------------------------------------------------------------

    function duration(value)
    {
        return value * ratioSpeed;
    }
}
