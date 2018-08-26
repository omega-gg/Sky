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

import QtQuick 1.1
import Sky     1.0

Item
{
    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------
    // Global

    property real scale: 1.0
    property real zoom : 1.0

    /* read */ property real ratio: scale * zoom

    property int dp1: 1 * ratio
    property int dp2: 2 * ratio
    property int dp3: 3 * ratio
    property int dp4: 4 * ratio
    property int dp5: 5 * ratio
    property int dp6: 6 * ratio
    property int dp7: 7 * ratio
    property int dp8: 8 * ratio
    property int dp9: 9 * ratio

    property int dp10: 10 * ratio
    property int dp11: 11 * ratio
    property int dp12: 12 * ratio
    property int dp13: 13 * ratio
    property int dp14: 14 * ratio
    property int dp15: 15 * ratio
    property int dp16: 16 * ratio
    property int dp17: 17 * ratio
    property int dp18: 18 * ratio
    property int dp19: 19 * ratio

    property int dp20: 20 * ratio
    property int dp22: 22 * ratio
    property int dp24: 24 * ratio
    property int dp26: 26 * ratio
    property int dp28: 28 * ratio

    property int dp30: 30 * ratio
    property int dp32: 32 * ratio
    property int dp34: 34 * ratio
    property int dp38: 38 * ratio

    property int dp40: 40 * ratio
    property int dp42: 42 * ratio
    property int dp44: 44 * ratio
    property int dp46: 46 * ratio
    property int dp48: 48 * ratio

    property int dp60: 60 * ratio
    property int dp64: 64 * ratio

    property int dp70: 70 * ratio

    property int dp80: 80 * ratio

    property int dp96: 96 * ratio

    property int dp120: 120 * ratio
    property int dp128: 128 * ratio
    property int dp150: 150 * ratio
    property int dp180: 180 * ratio

    property int dp200: 200 * ratio
    property int dp240: 240 * ratio

    property int dp300: 300 * ratio

    property variant size16x16: size(16, 16)
    property variant size24x24: size(24, 24)
    property variant size28x28: size(28, 28)
    property variant size32x32: size(32, 32)
    property variant size48x48: size(48, 48)

    property variant size128x128: size(128, 128)

    //---------------------------------------------------------------------------------------------

    property url icon

    property url  logo
    property real logoRatio: 8.0

    property color logo_colorA: "#323232"
    property color logo_colorB: "#646464"

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

    property int duration_slower: ms300
    property int duration_slow  : ms250
    property int duration_normal: ms200
    property int duration_fast  : ms150
    property int duration_faster: ms100

    property int ms1: 1 * ratioSpeed

    property int ms20: 20 * ratioSpeed
    property int ms50: 50 * ratioSpeed

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

    property string text_fontFamily: (sk.osMac) ? "" : "Arial"

    property real text_opacityDisable: 0.6

    property color text_color         : text1_color
    property color text_colorCurrent  : "#00b4ff"
    property color text_colorShadow   : text1_colorShadow
    property color text_colorSunken   : text1_colorSunken
    property color text_colorLink     : "#008cdc"
    property color text_colorLinkHover: "#00b4ff"

    property color text1_color      : "white"
    property color text1_colorShadow: "#161616"
    property color text1_colorSunken: "#323232"

    property color text2_color      : "black"
    property color text2_colorShadow: "#dcdcdc"
    property color text2_colorSunken: "#f0f0f0"

    property color text3_color      : "#c8c8c8"
    property color text3_colorShadow: "#161616"
    property color text3_colorSunken: "#323232"

    property color text4_color      : "#484848"
    property color text4_colorShadow: "#dcdcdc"
    property color text4_colorSunken: "#f0f0f0"

    //---------------------------------------------------------------------------------------------
    // TextDate

    property color textDate_color     : "#a0a0a0"
    property color textDate_colorHover: text_color

    //---------------------------------------------------------------------------------------------
    // TextListDefault

    property int textListDefault_pixelSize: dp14

    //---------------------------------------------------------------------------------------------
    // Line

    property int line_size: dp1

    //---------------------------------------------------------------------------------------------
    // LineHorizontalDrop

    property int lineHorizontalDrop_size: border_size

    //---------------------------------------------------------------------------------------------
    // Border

    property int border_size: dp2

    property real border_opacityDisable: 0.8

    property color border_color      : "#161616"
    property color border_colorLight : "#242424"
    property color border_colorShadow: "#323232"
    property color border_colorFocus : "#008cdc"

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
    // Separator

    property color separator_colorA: border_color
    property color separator_colorB: border_colorShadow

    //---------------------------------------------------------------------------------------------
    // Window

    property int window_durationAnimation: duration_fast

    property int window_resizerSize: dp4
    property int window_borderSize : dp3

    property color window_color: "#323232"

    //---------------------------------------------------------------------------------------------
    // RectangleBorders

    property int rectangleBorders_size: border_size

    //---------------------------------------------------------------------------------------------
    // RectangleShadow

    property url rectangleShadow_sourceLeft : "pictures/shadowLeft.png"
    property url rectangleShadow_sourceRight: "pictures/shadowRight.png"
    property url rectangleShadow_sourceUp   : "pictures/shadowUp.png"
    property url rectangleShadow_sourceDown : "pictures/shadowDown.png"

    property color rectangleShadow_color: "black"

    property alias rectangleShadow_filter: rectangleShadow_filter

    ImageColorFilter
    {
        id: rectangleShadow_filter

        color: rectangleShadow_color
    }

    //---------------------------------------------------------------------------------------------
    // BorderImageShadow

    property real borderImageShadow_ratio: 4.0

    property int borderImageShadow_adjust: dp2

    property real borderImageShadow_shadowOpacity: 1.0

    property url borderImageShadow_source: "pictures/shadow.png"

    property int borderImageShadow_borderSize: 128

    //---------------------------------------------------------------------------------------------
    // Icon

    property int icon_styleSize: dp1

    property real icon_opacityDisable: 0.5

    property color icon_colorA      : icon1_colorA
    property color icon_colorB      : icon1_colorB
    property color icon_colorActiveA: "#008cdc"
    property color icon_colorActiveB: icon_colorActiveA
    property color icon_colorShadow : icon1_colorShadow
    property color icon_colorSunken : icon1_colorSunken

    property color icon1_colorA     : "#dcdcdc"
    property color icon1_colorB     : icon1_colorA
    property color icon1_colorShadow: "#161616"
    property color icon1_colorSunken: "#323232"

    property color icon2_colorA     : "#323232"
    property color icon2_colorB     : icon2_colorA
    property color icon2_colorShadow: "#dcdcdc"
    property color icon2_colorSunken: "#f0f0f0"

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

    //---------------------------------------------------------------------------------------------
    // IconOverlay

    property real iconOverlay_opacity: 0.8

    property color iconOverlay_colorA: icon_colorActiveA
    property color iconOverlay_colorB: icon_colorActiveB

    //---------------------------------------------------------------------------------------------
    // IconLoading

    property int iconLoading_durationAnimation: ms800

    //---------------------------------------------------------------------------------------------
    // Panel

    property color panel_color: "#404040"

    //---------------------------------------------------------------------------------------------
    // PanelContextual

    property real panelContextual_backgroundOpacity: 0.94

    //---------------------------------------------------------------------------------------------
    // PanelImage

    property color panelImage_color: "black"

    //---------------------------------------------------------------------------------------------
    // BaseToolTip

    property int baseToolTip_minimumWidth : dp32
    property int baseToolTip_minimumHeight: dp32

    property int baseToolTip_marginWidth : cursor_width
    property int baseToolTip_marginHeight: cursor_height - dp8

    property real baseToolTip_opacityHover: 0.8

    property real baseToolTip_backgroundOpacity: 0.94

    property color baseToolTip_colorA: barTitle_colorA
    property color baseToolTip_colorB: barTitle_colorB

    //---------------------------------------------------------------------------------------------
    // ToolTip

    property int toolTip_maximumWidth: dp300

    property int toolTip_padding: dp8
    property int toolTip_spacing: dp8

    //---------------------------------------------------------------------------------------------
    // BarWindow

    property int barWindow_height: dp24

    property color barWindow_colorA: "#787878"
    property color barWindow_colorB: "#484848"

    property color barWindow_colorDisableA: "#484848"
    property color barWindow_colorDisableB: "#787878"

    property color barWindow_colorBorderLine: "#969696"

    //---------------------------------------------------------------------------------------------
    // BarTitle

    property int barTitle_height: dp28

    property int barTitle_borderSize: border_size

    property color barTitle_colorA: "#787878"
    property color barTitle_colorB: "#484848"

    property color barTitle_colorBorderLine: "#969696"

    //---------------------------------------------------------------------------------------------
    // BarTitleSmall

    property int barTitleSmall_height: dp26

    property color barTitleSmall_colorA: "#646464"
    property color barTitleSmall_colorB: "#484848"

    property color barTitleSmall_colorBorderLine: "#787878"

    //---------------------------------------------------------------------------------------------
    // BarTitleText

    property int barTitleText_leftMargin : dp8
    property int barTitleText_rightMargin: dp8

    property int barTitleText_pixelSize: dp14

    //---------------------------------------------------------------------------------------------
    // Button

    property color button_colorBorder: "#161616"

    property ImageColorFilter button_filterBorder    : border_filter
    property ImageColorFilter button_filterIcon      : icon_filter
    property ImageColorFilter button_filterIconShadow: icon_filterShadow
    property ImageColorFilter button_filterIconSunken: icon_filterSunken

    //---------------------------------------------------------------------------------------------
    // ButtonPush

    property int buttonPush_height: dp40

    property int buttonPush_margins: dp3
    property int buttonPush_padding: dp14
    property int buttonPush_spacing: dp6

    property url buttonPush_sourceBackground: "pictures/buttonPushMask.svg"
    property url buttonPush_sourceBorder    : "pictures/buttonPushBorder.svg"

    property variant buttonPush_borderBackground: Qt.rect(46, 46, 46, 46)

    property color buttonPush_colorA: "#646464"
    property color buttonPush_colorB: "#323232"

    property color buttonPush_colorHoverA: "#848484"
    property color buttonPush_colorHoverB: "#424242"

    property color buttonPush_colorPressA: "#242424"
    property color buttonPush_colorPressB: "#484848"

    property color buttonPush_colorPressHoverA: "#242424"
    property color buttonPush_colorPressHoverB: "#646464"

    property color buttonPush_colorHighlightA: "#008cdc"
    property color buttonPush_colorHighlightB: "#003ca0"

    property color buttonPush_colorHighlightHoverA: "#00a0f0"
    property color buttonPush_colorHighlightHoverB: "#0050b4"

    property color buttonPush_colorCheckA: "#0050b4"
    property color buttonPush_colorCheckB: "#008cdc"

    property color buttonPush_colorCheckHoverA: "#0050b4"
    property color buttonPush_colorCheckHoverB: "#00b4ff"

    property ImageColorFilter buttonPush_filterBorder    : button_filterBorder
    property ImageColorFilter buttonPush_filterIcon      : icon_filter
    property ImageColorFilter buttonPush_filterIconShadow: icon_filterShadow
    property ImageColorFilter buttonPush_filterIconSunken: icon_filterSunken

    property alias buttonPush_filterDefault       : buttonPush_filterDefault
    property alias buttonPush_filterHover         : buttonPush_filterHover
    property alias buttonPush_filterPress         : buttonPush_filterPress
    property alias buttonPush_filterPressHover    : buttonPush_filterPressHover
    property alias buttonPush_filterHighlight     : buttonPush_filterHighlight
    property alias buttonPush_filterHighlightHover: buttonPush_filterHighlightHover
    property alias buttonPush_filterCheck         : buttonPush_filterCheck
    property alias buttonPush_filterCheckHover    : buttonPush_filterCheckHover
    property alias buttonPush_filterFocus         : border_filterFocus

    ImageColorFilter
    {
        id: buttonPush_filterDefault

        gradient: ScaleGradient
        {
            ScaleGradientStop { position: 0.0; color: buttonPush_colorA }
            ScaleGradientStop { position: 1.0; color: buttonPush_colorB }
        }
    }

    ImageColorFilter
    {
        id: buttonPush_filterHover

        gradient: ScaleGradient
        {
            ScaleGradientStop { position: 0.0; color: buttonPush_colorHoverA }
            ScaleGradientStop { position: 1.0; color: buttonPush_colorHoverB }
        }
    }

    ImageColorFilter
    {
        id: buttonPush_filterPress

        gradient: ScaleGradient
        {
            ScaleGradientStop { position: 0.0; color: buttonPush_colorPressA }
            ScaleGradientStop { position: 1.0; color: buttonPush_colorPressB }
        }
    }

    ImageColorFilter
    {
        id: buttonPush_filterPressHover

        gradient: ScaleGradient
        {
            ScaleGradientStop { position: 0.0; color: buttonPush_colorPressHoverA }
            ScaleGradientStop { position: 1.0; color: buttonPush_colorPressHoverB }
        }
    }

    ImageColorFilter
    {
        id: buttonPush_filterHighlight

        gradient: ScaleGradient
        {
            ScaleGradientStop { position: 0.0; color: buttonPush_colorHighlightA }
            ScaleGradientStop { position: 1.0; color: buttonPush_colorHighlightB }
        }
    }

    ImageColorFilter
    {
        id: buttonPush_filterHighlightHover

        gradient: ScaleGradient
        {
            ScaleGradientStop { position: 0.0; color: buttonPush_colorHighlightHoverA }
            ScaleGradientStop { position: 1.0; color: buttonPush_colorHighlightHoverB }
        }
    }

    ImageColorFilter
    {
        id: buttonPush_filterCheck

        gradient: ScaleGradient
        {
            ScaleGradientStop { position: 0.0; color: buttonPush_colorCheckA }
            ScaleGradientStop { position: 1.0; color: buttonPush_colorCheckB }
        }
    }

    ImageColorFilter
    {
        id: buttonPush_filterCheckHover

        gradient: ScaleGradient
        {
            ScaleGradientStop { position: 0.0; color: buttonPush_colorCheckHoverA }
            ScaleGradientStop { position: 1.0; color: buttonPush_colorCheckHoverB }
        }
    }

    //---------------------------------------------------------------------------------------------
    // ButtonPushIcon

    property int buttonPushIcon_width : buttonPush_height
    property int buttonPushIcon_height: buttonPush_height

    //---------------------------------------------------------------------------------------------
    // ButtonPushLeft

    property int buttonPushLeft_extra: dp6

    property url buttonPushLeft_sourceBackground: "pictures/buttonPushLeftMask.svg"
    property url buttonPushLeft_sourceBorder    : "pictures/buttonPushLeftBorder.svg"

    //---------------------------------------------------------------------------------------------
    // ButtonPushCenter

    property url buttonPushCenter_sourceBackground: "pictures/buttonPushCenterMask.svg"
    property url buttonPushCenter_sourceBorder    : "pictures/buttonPushCenterBorder.svg"

    //---------------------------------------------------------------------------------------------
    // ButtonPushRight

    property int buttonPushRight_extra: dp6

    property url buttonPushRight_sourceBackground: "pictures/buttonPushRightMask.svg"
    property url buttonPushRight_sourceBorder    : "pictures/buttonPushRightBorder.svg"

    //---------------------------------------------------------------------------------------------
    // ButtonPushLeftIcon

    property int buttonPushLeftIcon_padding: dp4

    //---------------------------------------------------------------------------------------------
    // ButtonPushRightIcon

    property int buttonPushRightIcon_padding: dp4

    //---------------------------------------------------------------------------------------------
    // ButtonPushConfirm

    property color buttonPushConfirm_colorHoverA: "#c80000"
    property color buttonPushConfirm_colorHoverB: "#960000"

    property color buttonPushConfirm_colorPressA: "#960000"
    property color buttonPushConfirm_colorPressB: "#c80000"

    property alias buttonPushConfirm_filterHover: buttonPushConfirm_filterHover
    property alias buttonPushConfirm_filterPress: buttonPushConfirm_filterPress

    ImageColorFilter
    {
        id: buttonPushConfirm_filterHover

        gradient: ScaleGradient
        {
            ScaleGradientStop { position: 0.0; color: buttonPushConfirm_colorHoverA }
            ScaleGradientStop { position: 1.0; color: buttonPushConfirm_colorHoverB }
        }
    }

    ImageColorFilter
    {
        id: buttonPushConfirm_filterPress

        gradient: ScaleGradient
        {
            ScaleGradientStop { position: 0.0; color: buttonPushConfirm_colorPressA }
            ScaleGradientStop { position: 1.0; color: buttonPushConfirm_colorPressB }
        }
    }

    //---------------------------------------------------------------------------------------------
    // ButtonOverlay

    property real buttonOverlay_opacityDefault: 0.8
    property real buttonOverlay_opacityHover  : 1.0

    property color buttonOverlay_colorA: "#323232"
    property color buttonOverlay_colorB: "black"

    property color buttonOverlay_colorHoverA: buttonPush_colorHighlightHoverA
    property color buttonOverlay_colorHoverB: buttonPush_colorHighlightHoverB

    property color buttonOverlay_colorPressA: buttonPush_colorCheckA
    property color buttonOverlay_colorPressB: buttonPush_colorCheckB

    property color buttonOverlay_colorBorderA: icon_colorA
    property color buttonOverlay_colorBorderB: icon_colorB

    property alias buttonOverlay_filterDefault: buttonOverlay_filterDefault
    property alias buttonOverlay_filterHover  : buttonOverlay_filterHover
    property alias buttonOverlay_filterPress  : buttonOverlay_filterPress
    property alias buttonOverlay_filterBorder : buttonOverlay_filterBorder

    ImageColorFilter
    {
        id: buttonOverlay_filterDefault

        gradient: ScaleGradient
        {
            ScaleGradientStop { position: 0.0; color: buttonOverlay_colorA }
            ScaleGradientStop { position: 1.0; color: buttonOverlay_colorB }
        }
    }

    ImageColorFilter
    {
        id: buttonOverlay_filterHover

        gradient: ScaleGradient
        {
            ScaleGradientStop { position: 0.0; color: buttonOverlay_colorHoverA }
            ScaleGradientStop { position: 1.0; color: buttonOverlay_colorHoverB }
        }
    }

    ImageColorFilter
    {
        id: buttonOverlay_filterPress

        gradient: ScaleGradient
        {
            ScaleGradientStop { position: 0.0; color: buttonOverlay_colorPressA }
            ScaleGradientStop { position: 1.0; color: buttonOverlay_colorPressB }
        }
    }

    ImageColorFilter
    {
        id: buttonOverlay_filterBorder

        gradient: ScaleGradient
        {
            ScaleGradientStop { position: 0.0; color: buttonOverlay_colorBorderA }
            ScaleGradientStop { position: 1.0; color: buttonOverlay_colorBorderB }
        }
    }

    //---------------------------------------------------------------------------------------------
    // ButtonPiano

    property int buttonPiano_height: dp32

    property int buttonPiano_padding: dp8
    property int buttonPiano_spacing: dp4

    property color buttonPiano_colorA: buttonPush_colorA
    property color buttonPiano_colorB: buttonPush_colorB

    property color buttonPiano_colorHoverA: buttonPush_colorHoverA
    property color buttonPiano_colorHoverB: buttonPush_colorHoverB

    property color buttonPiano_colorPressA: buttonPush_colorPressA
    property color buttonPiano_colorPressB: buttonPush_colorPressB

    property color buttonPiano_colorPressHoverA: buttonPush_colorPressHoverA
    property color buttonPiano_colorPressHoverB: buttonPush_colorPressHoverB

    property color buttonPiano_colorHighlightA: buttonPush_colorHighlightA
    property color buttonPiano_colorHighlightB: buttonPush_colorHighlightB

    property color buttonPiano_colorHighlightHoverA: buttonPush_colorHighlightHoverA
    property color buttonPiano_colorHighlightHoverB: buttonPush_colorHighlightHoverB

    property color buttonPiano_colorCheckA: buttonPush_colorCheckA
    property color buttonPiano_colorCheckB: buttonPush_colorCheckB

    property color buttonPiano_colorCheckHoverA: buttonPush_colorCheckHoverA
    property color buttonPiano_colorCheckHoverB: buttonPush_colorCheckHoverB

    property color buttonPiano_colorFocus: border_colorFocus

    property ImageColorFilter buttonPiano_filterIcon      : icon_filter
    property ImageColorFilter buttonPiano_filterIconShadow: icon_filterShadow
    property ImageColorFilter buttonPiano_filterIconSunken: icon_filterSunken

    //---------------------------------------------------------------------------------------------
    // ButtonPianoIcon

    property int buttonPianoIcon_width : buttonPiano_height
    property int buttonPianoIcon_height: buttonPiano_height

    //---------------------------------------------------------------------------------------------
    // ButtonPianoConfirm

    property color buttonPianoConfirm_colorHoverA: "#c80000"
    property color buttonPianoConfirm_colorHoverB: "#780000"

    property color buttonPianoConfirm_colorPressA: "#780000"
    property color buttonPianoConfirm_colorPressB: "#c80000"

    property color buttonPianoConfirm_colorFocus: "#ff0000"

    //---------------------------------------------------------------------------------------------
    // ButtonPianoReset

    property int buttonPianoReset_durationOpacity: duration_normal

    //---------------------------------------------------------------------------------------------
    // ButtonRound

    property int buttonRound_width : dp48
    property int buttonRound_height: dp48

    property int buttonRound_margins: dp3

    property url buttonRound_sourceBackground: "pictures/buttonRoundMask.svg"
    property url buttonRound_sourceBorder    : "pictures/buttonRoundBorder.svg"

    property ImageColorFilter buttonRound_filterBorder    : buttonPush_filterBorder
    property ImageColorFilter buttonRound_filterIcon      : buttonPush_filterIcon
    property ImageColorFilter buttonRound_filterIconShadow: buttonPush_filterIconShadow
    property ImageColorFilter buttonRound_filterIconSunken: buttonPush_filterIconSunken

    property alias buttonRound_filterDefault       : buttonPush_filterDefault
    property alias buttonRound_filterHover         : buttonPush_filterHover
    property alias buttonRound_filterPress         : buttonPush_filterPress
    property alias buttonRound_filterPressHover    : buttonPush_filterPressHover
    property alias buttonRound_filterHighlight     : buttonPush_filterHighlight
    property alias buttonRound_filterHighlightHover: buttonPush_filterHighlightHover
    property alias buttonRound_filterCheck         : buttonPush_filterCheck
    property alias buttonRound_filterCheckHover    : buttonPush_filterCheckHover
    property alias buttonRound_filterFocus         : border_filterFocus

    //---------------------------------------------------------------------------------------------
    // ButtonCheck

    property int buttonCheck_height: buttonPush_height

    property int buttonCheck_buttonLeftMargin: -dp28

    property int buttonCheck_buttonPaddingLeft : dp30
    property int buttonCheck_buttonPaddingRight: dp18

    //---------------------------------------------------------------------------------------------
    // ButtonCheckLabel

    property int buttonCheckLabel_height: buttonCheck_height

    property int buttonCheckLabel_buttonLeftMargin: -dp32

    property int buttonCheckLabel_labelPaddingLeft : dp16
    property int buttonCheckLabel_labelPaddingRight: dp34

    //---------------------------------------------------------------------------------------------
    // ButtonImageBorders

    property color buttonImageBorders_colorBackground: "#242424"

    property color buttonImageBorders_colorBorder     : border_color
    property color buttonImageBorders_colorBorderHover: border_colorFocus

    //---------------------------------------------------------------------------------------------
    // ButtonMask

    property int buttonMask_width : dp16
    property int buttonMask_height: dp16

    property int buttonMask_styleSize: dp2

    property color buttonMask_colorHoverA: "white"
    property color buttonMask_colorHoverB: "white"

    property color buttonMask_colorPressA: "white"
    property color buttonMask_colorPressB: "#646464"

    property ImageColorFilter buttonMask_filterDefault: icon_filter
    property ImageColorFilter buttonMask_filterShadow : icon_filterShadow
    property ImageColorFilter buttonMask_filterSunken : icon_filterSunken

    property alias buttonMask_filterHover: buttonMask_filterHover
    property alias buttonMask_filterPress: buttonMask_filterPress

    ImageColorFilter
    {
        id: buttonMask_filterHover

        gradient: ScaleGradient
        {
            ScaleGradientStop { position: 0.0; color: buttonMask_colorHoverA }
            ScaleGradientStop { position: 1.0; color: buttonMask_colorHoverB }
        }
    }

    ImageColorFilter
    {
        id: buttonMask_filterPress

        gradient: ScaleGradient
        {
            ScaleGradientStop { position: 0.0; color: buttonMask_colorPressA }
            ScaleGradientStop { position: 1.0; color: buttonMask_colorPressB }
        }
    }

    //---------------------------------------------------------------------------------------------
    // ButtonStream

    property int buttonStream_width : dp70
    property int buttonStream_height: slider_height

    property url buttonStream_sourceBackground: "pictures/sliderMask.svg"
    property url buttonStream_sourceBorder    : "pictures/sliderBorder.svg"

    property variant buttonStream_borderBackground: slider_borderBackground

    property color buttonStream_color: scrollBar_colorA

    property ImageColorFilter buttonStream_filterDefault: buttonPush_filterDefault
    property ImageColorFilter buttonStream_filterHover  : slider_filterBarHover
    property ImageColorFilter buttonStream_filterPress  : buttonPush_filterPress
    property ImageColorFilter buttonStream_filterBorder : button_filterBorder

    property alias buttonStream_filterDisable: buttonStream_filterDisable

    ImageColorFilter
    {
        id: buttonStream_filterDisable

        color: buttonStream_color
    }

    //---------------------------------------------------------------------------------------------
    // ButtonsCheck

    property int buttonsCheck_extra: dp6

    //---------------------------------------------------------------------------------------------
    // ButtonsItem

    property int buttonsItem_buttonWidth : dp28
    property int buttonsItem_buttonHeight: dp30

    //---------------------------------------------------------------------------------------------
    // LabelRound

    property int labelRound_height: dp40

    property int labelRound_padding: dp14
    property int labelRound_spacing: dp6

    property url labelRound_sourceBackground: "pictures/labelRoundMask.svg"
    property url labelRound_sourceBorder    : "pictures/labelRoundBorder.svg"

    property variant labelRound_borderBackground: Qt.rect(64, 64, 64, 64)

    property color labelRound_colorA: buttonPush_colorCheckA
    property color labelRound_colorB: buttonPush_colorCheckB

    property color labelRound_colorDisableA: "#787878"
    property color labelRound_colorDisableB: "#484848"

    property color labelRound_colorText: text_color

    property ImageColorFilter labelRound_filterBorder    : button_filterBorder
    property ImageColorFilter labelRound_filterIcon      : icon_filter
    property ImageColorFilter labelRound_filterIconShadow: icon_filterShadow
    property ImageColorFilter labelRound_filterIconSunken: icon_filterSunken

    property alias labelRound_filterDefault: labelRound_filterDefault
    property alias labelRound_filterDisable: labelRound_filterDisable

    ImageColorFilter
    {
        id: labelRound_filterDefault

        gradient: ScaleGradient
        {
            ScaleGradientStop { position: 0.0; color: labelRound_colorA }
            ScaleGradientStop { position: 1.0; color: labelRound_colorB }
        }
    }

    ImageColorFilter
    {
        id: labelRound_filterDisable

        gradient: ScaleGradient
        {
            ScaleGradientStop { position: 0.0; color: labelRound_colorDisableA }
            ScaleGradientStop { position: 1.0; color: labelRound_colorDisableB }
        }
    }

    //---------------------------------------------------------------------------------------------
    // LabelRoundIcon

    property int labelRoundIcon_width : labelRound_height
    property int labelRoundIcon_height: labelRound_height

    //---------------------------------------------------------------------------------------------
    // LabelRoundInfo

    property ImageColorFilter labelRoundInfo_filterBorder: border_filter

    property alias labelRoundInfo_filterDefault: labelRoundInfo_filterDefault

    ImageColorFilter
    {
        id: labelRoundInfo_filterDefault

        color: "#323232"
    }

    //---------------------------------------------------------------------------------------------
    // LabelLoading

    property int labelLoading_width : dp48
    property int labelLoading_height: dp48

    property real labelLoading_opacity: 0.8

    property url labelLoading_sourceBackground: "pictures/buttonRoundMask.svg"

    //---------------------------------------------------------------------------------------------
    // LabelLoadingText

    property int labelLoadingText_height: dp44

    property int labelLoadingText_padding: dp6
    property int labelLoadingText_spacing: dp2

    property real labelLoadingText_opacity: 0.6

    property url labelLoadingText_sourceBackground: "pictures/labelRoundMask.svg"

    property variant labelLoadingText_borderBackground: Qt.rect(64, 64, 64, 64)

    //---------------------------------------------------------------------------------------------
    // LabelLoadingButton

    property int labelLoadingButton_buttonWidth : dp38
    property int labelLoadingButton_buttonHeight: dp38

    //---------------------------------------------------------------------------------------------
    // LabelStream

    property int labelStream_height: dp16

    property int labelStream_padding: dp4

    property int labelStream_gapX: dp5
    property int labelStream_gapY: dp7

    property color labelStream_colorA: scrollBar_colorA
    property color labelStream_colorB: scrollBar_colorB

    //---------------------------------------------------------------------------------------------
    // CheckBox

    property int checkBox_width : dp22
    property int checkBox_height: checkBox_width

    property int checkBox_margins: lineEdit_margins

    property url checkBox_sourceDefault: "pictures/checkBoxMask.svg"
    property url checkBox_sourceBorder : "pictures/checkBoxBorder.svg"
    property url checkBox_sourceCheck  : "pictures/checkBoxCheck.png"

    property ImageColorFilter checkBox_filterBorder: border_filter

    property alias checkBox_filterDefault: lineEdit_filterDefault
    property alias checkBox_filterHover  : lineEdit_filterHover
    property alias checkBox_filterFocus  : border_filterFocus

    //---------------------------------------------------------------------------------------------
    // CheckBoxText

    property int checkBoxText_height: dp28

    property int checkBoxText_padding: dp6
    property int checkBoxText_spacing: dp4

    //---------------------------------------------------------------------------------------------
    // BaseLineEdit

    property int baseLineEdit_durationCursor: ms500

    property int baseLineEdit_height: dp32

    property int baseLineEdit_cursorWidth: dp2

    property int baseLineEdit_maximumLength: 1000

    property color baseLineEdit_colorCursor: "#161616"

    property color baseLineEdit_colorSelection    : "#008cdc"
    property color baseLineEdit_colorSelectionText: "white"

    property color baseLineEdit_colorDefault: text4_color

    //---------------------------------------------------------------------------------------------
    // LineEdit

    property int lineEdit_width : dp200
    property int lineEdit_height: dp40

    property int lineEdit_margins: dp3
    property int lineEdit_padding: dp13

    property url lineEdit_sourceDefault: "pictures/lineEditMask.svg"
    property url lineEdit_sourceBorder : "pictures/lineEditBorder.svg"

    property variant lineEdit_borderBackground: Qt.rect(32, 32, 32, 32)

    property color lineEdit_color     : "#dcdcdc"
    property color lineEdit_colorHover: "white"

    property ImageColorFilter lineEdit_filterBorder: border_filter

    property alias lineEdit_filterDefault: lineEdit_filterDefault
    property alias lineEdit_filterHover  : lineEdit_filterHover
    property alias lineEdit_filterFocus  : border_filterFocus

    ImageColorFilter
    {
        id: lineEdit_filterDefault

        color: lineEdit_color
    }

    ImageColorFilter
    {
        id: lineEdit_filterHover

        color: lineEdit_colorHover
    }

    //---------------------------------------------------------------------------------------------
    // LineEditLabel

    property int lineEditLabel_lineEditLeftMargin: -dp26

    property int lineEditLabel_labelPaddingLeft : dp16
    property int lineEditLabel_labelPaddingRight: dp30

    //---------------------------------------------------------------------------------------------
    // LineEditBox

    property int lineEditBox_focusSize: dp2

    property color lineEditBox_colorA: "#c8c8c8"
    property color lineEditBox_colorB: "white"

    //---------------------------------------------------------------------------------------------
    // List

    property int list_itemSize: dp32 + border_size

    //---------------------------------------------------------------------------------------------
    // ListContextual

    property int listContextual_spacing: dp2

    property ImageColorFilter listContextual_filterIcon: icon_filter

    //---------------------------------------------------------------------------------------------
    // ScrollBar

    property int scrollBar_intervalPress : 300
    property int scrollBar_intervalRepeat: 50

    property int scrollBar_singleStep: dp8
    property int scrollBar_pageStep  : dp16

    property int scrollBar_wheelMultiplier: 3

    property int scrollBar_width: dp16

    property int scrollBar_buttonSize: dp19

    property int scrollBar_handleMinimumSize: dp32

    property color scrollBar_colorA: buttonPush_colorPressA
    property color scrollBar_colorB: buttonPush_colorPressB

    property color scrollBar_colorBorder: border_color

    property color scrollBar_colorHandleA: buttonPush_colorA
    property color scrollBar_colorHandleB: buttonPush_colorB

    property color scrollBar_colorHandleHoverA: buttonPush_colorHoverA
    property color scrollBar_colorHandleHoverB: buttonPush_colorHoverB

    property color scrollBar_colorHandlePressA: buttonPush_colorPressHoverA
    property color scrollBar_colorHandlePressB: buttonPush_colorPressHoverB

    property alias scrollBar_filterHorizontalDefault    : scrollBar_filterHorizontalDefault
    property alias scrollBar_filterHorizontalHandle     : scrollBar_filterHorizontalHandle
    property alias scrollBar_filterHorizontalHandleHover: scrollBar_filterHorizontalHandleHover
    property alias scrollBar_filterHorizontalHandlePress: scrollBar_filterHorizontalHandlePress

    ImageColorFilter
    {
        id: scrollBar_filterHorizontalDefault

        gradient: ScaleGradient
        {
            type: ScaleGradient.LinearHorizontal

            ScaleGradientStop { position: 0.0; color: scrollBar_colorA }
            ScaleGradientStop { position: 1.0; color: scrollBar_colorB }
        }
    }

    ImageColorFilter
    {
        id: scrollBar_filterHorizontalHandle

        gradient: ScaleGradient
        {
            type: ScaleGradient.LinearHorizontal

            ScaleGradientStop { position: 0.0; color: scrollBar_colorHandleA }
            ScaleGradientStop { position: 1.0; color: scrollBar_colorHandleB }
        }
    }

    ImageColorFilter
    {
        id: scrollBar_filterHorizontalHandleHover

        gradient: ScaleGradient
        {
            type: ScaleGradient.LinearHorizontal

            ScaleGradientStop { position: 0.0; color: scrollBar_colorHandleHoverA }
            ScaleGradientStop { position: 1.0; color: scrollBar_colorHandleHoverB }
        }
    }

    ImageColorFilter
    {
        id: scrollBar_filterHorizontalHandlePress

        gradient: ScaleGradient
        {
            type: ScaleGradient.LinearHorizontal

            ScaleGradientStop { position: 0.0; color: scrollBar_colorHandlePressA }
            ScaleGradientStop { position: 1.0; color: scrollBar_colorHandlePressB }
        }
    }

    //---------------------------------------------------------------------------------------------
    // Scroller

    property real scroller_scroll: 1.0

    property int scroller_durationOpacity: duration_faster

    property int scroller_intervalA: 300
    property int scroller_intervalB: 20

    property int scroller_size: dp38

    property real scroller_opacityA: 0.6
    property real scroller_opacityB: 0.8

    property ImageColorFilter scroller_filter: rectangleShadow_filter

    //---------------------------------------------------------------------------------------------
    // ScrollerList

    property real scrollerList_scroll: 0.1

    //---------------------------------------------------------------------------------------------
    // Slider

    property int slider_width : dp120
    property int slider_height: dp26

    property int slider_handleWidth : dp26
    property int slider_handleHeight: dp26

    property url slider_sourceBackground  : "pictures/sliderMask.svg"
    property url slider_sourceForeground  : "pictures/sliderHandleMask.svg"
    property url slider_sourceBorder      : "pictures/sliderBorder.svg"
    property url slider_sourceHandle      : "pictures/sliderHandleMask.svg"
    property url slider_sourceHandleBorder: "pictures/sliderHandleBorder.svg"

    property variant slider_borderBackground: Qt.rect(39, 39, 39, 39)
    property variant slider_borderForeground: Qt.rect(39, 39, 39, 39)

    property color slider_colorA: scrollBar_colorA
    property color slider_colorB: scrollBar_colorB

    property color slider_colorHandleA: "#dcdcdc"
    property color slider_colorHandleB: "#8c8c8c"

    property color slider_colorHandleHoverA: "#f0f0f0"
    property color slider_colorHandleHoverB: "#c8c8c8"

    property color slider_colorHandlePressA: "#646464"
    property color slider_colorHandlePressB: "#f0f0f0"

    property color slider_colorBarA: buttonPush_colorA
    property color slider_colorBarB: buttonPush_colorB

    property color slider_colorBarHoverA: buttonPush_colorHoverA
    property color slider_colorBarHoverB: buttonPush_colorHoverB

    property ImageColorFilter slider_filterBorder: border_filter

    property alias slider_filterBackground : slider_filterBackground
    property alias slider_filterHandle     : slider_filterHandle
    property alias slider_filterHandleHover: slider_filterHandleHover
    property alias slider_filterHandlePress: slider_filterHandlePress
    property alias slider_filterBar        : slider_filterBar
    property alias slider_filterBarHover   : slider_filterBarHover

    ImageColorFilter
    {
        id: slider_filterBackground

        gradient: ScaleGradient
        {
            ScaleGradientStop { position: 0.0; color: slider_colorA }
            ScaleGradientStop { position: 1.0; color: slider_colorB }
        }
    }

    ImageColorFilter
    {
        id: slider_filterHandle

        gradient: ScaleGradient
        {
            ScaleGradientStop { position: 0.0; color: slider_colorHandleA }
            ScaleGradientStop { position: 1.0; color: slider_colorHandleB }
        }
    }

    ImageColorFilter
    {
        id: slider_filterHandleHover

        gradient: ScaleGradient
        {
            ScaleGradientStop { position: 0.0; color: slider_colorHandleHoverA }
            ScaleGradientStop { position: 1.0; color: slider_colorHandleHoverB }
        }
    }

    ImageColorFilter
    {
        id: slider_filterHandlePress

        gradient: ScaleGradient
        {
            ScaleGradientStop { position: 0.0; color: slider_colorHandlePressA }
            ScaleGradientStop { position: 1.0; color: slider_colorHandlePressB }
        }
    }

    ImageColorFilter
    {
        id: slider_filterBar

        gradient: ScaleGradient
        {
            ScaleGradientStop { position: 0.0; color: slider_colorBarA }
            ScaleGradientStop { position: 1.0; color: slider_colorBarB }
        }
    }

    ImageColorFilter
    {
        id: slider_filterBarHover

        gradient: ScaleGradient
        {
            ScaleGradientStop { position: 0.0; color: slider_colorBarHoverA }
            ScaleGradientStop { position: 1.0; color: slider_colorBarHoverB }
        }
    }

    //---------------------------------------------------------------------------------------------
    // SliderVolume

    property int sliderVolume_width : dp150
    property int sliderVolume_height: dp32

    //---------------------------------------------------------------------------------------------
    // SliderStream

    property int sliderStream_durationAnimation: duration_slower

    property int sliderStream_intervalProgress: ms1000

    property int sliderStream_height: dp32

    property int sliderStream_textWidth: dp46

    property real sliderStream_opacityProgressA: 0.5
    property real sliderStream_opacityProgressB: 1.0

    property color sliderStream_colorBarA: "#00a0f0"
    property color sliderStream_colorBarB: "#0050b4"

    property color sliderStream_colorBarHoverA: "#00b4ff"
    property color sliderStream_colorBarHoverB: "#0064c8"

    property color sliderStream_colorBarDisableA: "#dcdcdc"
    property color sliderStream_colorBarDisableB: "#646464"

    property color sliderStream_colorBarDisableHoverA: "#f0f0f0"
    property color sliderStream_colorBarDisableHoverB: "#787878"

    property color sliderStream_colorBarProgressA: "#646464"
    property color sliderStream_colorBarProgressB: "#dcdcdc"

    property alias sliderStream_filterBar            : sliderStream_filterBar
    property alias sliderStream_filterBarHover       : sliderStream_filterBarHover
    property alias sliderStream_filterBarDisable     : sliderStream_filterBarDisable
    property alias sliderStream_filterBarDisableHover: sliderStream_filterBarDisableHover
    property alias sliderStream_filterBarProgress    : sliderStream_filterBarProgress

    ImageColorFilter
    {
        id: sliderStream_filterBar

        gradient: ScaleGradient
        {
            ScaleGradientStop { position: 0.0; color: sliderStream_colorBarA }
            ScaleGradientStop { position: 1.0; color: sliderStream_colorBarB }
        }
    }

    ImageColorFilter
    {
        id: sliderStream_filterBarHover

        gradient: ScaleGradient
        {
            ScaleGradientStop { position: 0.0; color: sliderStream_colorBarHoverA }
            ScaleGradientStop { position: 1.0; color: sliderStream_colorBarHoverB }
        }
    }

    ImageColorFilter
    {
        id: sliderStream_filterBarDisable

        gradient: ScaleGradient
        {
            ScaleGradientStop { position: 0.0; color: sliderStream_colorBarDisableA }
            ScaleGradientStop { position: 1.0; color: sliderStream_colorBarDisableB }
        }
    }

    ImageColorFilter
    {
        id: sliderStream_filterBarDisableHover

        gradient: ScaleGradient
        {
            ScaleGradientStop { position: 0.0; color: sliderStream_colorBarDisableHoverA }
            ScaleGradientStop { position: 1.0; color: sliderStream_colorBarDisableHoverB }
        }
    }

    ImageColorFilter
    {
        id: sliderStream_filterBarProgress

        gradient: ScaleGradient
        {
            ScaleGradientStop { position: 0.0; color: sliderStream_colorBarProgressA }
            ScaleGradientStop { position: 1.0; color: sliderStream_colorBarProgressB }
        }
    }

    //---------------------------------------------------------------------------------------------
    // BaseTabs

    property int baseTabs_height: dp32

    property int baseTabs_tabMinimum: dp64
    property int baseTabs_tabMaximum: dp200

    property int baseTabs_borderSize: border_size

    property int baseTabs_spacing: dp4

    //---------------------------------------------------------------------------------------------
    // TabsBrowser

    property int tabsBrowser_intervalResize: ms300

    property int tabsBrowser_tabMaximum: dp300

    property int tabsBrowser_spacing: dp8

    //---------------------------------------------------------------------------------------------
    // TabsPlayer

    property int tabsPlayer_overlayWidth : componentTabBrowser_iconWidth
    property int tabsPlayer_overlayHeight: itemTab_iconHeight

    //---------------------------------------------------------------------------------------------
    // TabBarProgress

    property int tabBarProgress_height: dp3

    property color tabBarProgress_colorA: sliderStream_colorBarB
    property color tabBarProgress_colorB: sliderStream_colorBarA

    property color tabBarProgress_colorDisableA: sliderStream_colorBarDisableB
    property color tabBarProgress_colorDisableB: sliderStream_colorBarDisableA

    //---------------------------------------------------------------------------------------------
    // BaseWall

    property int baseWall_intervalUpdate: ms300

    property int baseWall_itemPreferredWidth : dp240
    property int baseWall_itemPreferredHeight: dp180

    property real baseWall_itemRatioWidth : 1.333
    property real baseWall_itemRatioHeight: 0.75

    property int baseWall_itemExtraWidth : dp4
    property int baseWall_itemExtraHeight: dp38

    property int baseWall_marginsMinSize: dp32

    //---------------------------------------------------------------------------------------------
    // WallBookmarkTrack

    property int wallBookmarkTrack_fixedDuration: ms1000

    //---------------------------------------------------------------------------------------------
    // WallVideo

    property int wallVideo_overlayWidth : componentWallBookmarkTrack_iconWidth
    property int wallVideo_overlayHeight: itemWall_iconHeight

    property real wallVideo_opacityLoader: 0.6

    property color wallVideo_colorPlayer: "black"

    //---------------------------------------------------------------------------------------------
    // PlayerBrowser

    property int playerBrowser_interval: 200

    property real playerBrowser_opacityBar   : 0.6
    property real playerBrowser_opacityShadow: 0.6

    property color playerBrowser_colorA: "black"
    property color playerBrowser_colorB: "black"

    property color playerBrowser_colorBar: "black"

    property color playerBrowser_colorTextA: text_color
    property color playerBrowser_colorTextB: "#c8c8c8"

    property color playerBrowser_colorShadow: "#008cdc"

    property alias playerBrowser_filterShadow: playerBrowser_filterShadow

    ImageColorFilter
    {
        id: playerBrowser_filterShadow

        color: playerBrowser_colorShadow
    }

    //---------------------------------------------------------------------------------------------
    // ItemList

    property int itemList_height: list_itemSize

    property int itemList_iconWidth : dp32
    property int itemList_iconHeight: dp32

    property color itemList_colorA: "#383838"
    property color itemList_colorB: "#303030"

    property color itemList_colorDefaultA: "#282828"
    property color itemList_colorDefaultB: "#202020"

    property color itemList_colorHoverA: "#646464"
    property color itemList_colorHoverB: "#424242"

    property color itemList_colorPressA: "#787878"
    property color itemList_colorPressB: "#505050"

    property color itemList_colorContextualHoverA: "#787878"
    property color itemList_colorContextualHoverB: "#505050"

    property color itemList_colorSelectA: buttonPush_colorHighlightA
    property color itemList_colorSelectB: buttonPush_colorHighlightB

    property color itemList_colorSelectHoverA: buttonPush_colorHighlightHoverA
    property color itemList_colorSelectHoverB: buttonPush_colorHighlightHoverB

    property color itemList_colorSelectFocusA: buttonPush_colorCheckA
    property color itemList_colorSelectFocusB: buttonPush_colorCheckB

    property color itemList_colorCurrentA: "#161616"
    property color itemList_colorCurrentB: "#282828"

    property color itemList_colorCurrentHoverA: "#161616"
    property color itemList_colorCurrentHoverB: "#484848"

    property color itemList_colorBorder       : "#202020"
    property color itemList_colorBorderDefault: "#161616"

    property color itemList_colorText        : "#dcdcdc"
    property color itemList_colorTextHover   : text_color
    property color itemList_colorTextSelected: text_color
    property color itemList_colorTextCurrent : text_colorCurrent

    //---------------------------------------------------------------------------------------------
    // ItemTab

    property int itemTab_height: dp32

    property int itemTab_borderSize: border_size

    property int itemTab_iconWidth : dp32
    property int itemTab_iconHeight: dp32

    property color itemTab_colorA: buttonPiano_colorA
    property color itemTab_colorB: buttonPiano_colorB

    property color itemTab_colorHoverA: buttonPiano_colorHoverA
    property color itemTab_colorHoverB: buttonPiano_colorHoverB

    property color itemTab_colorContextualHoverA: "#a0a0a0"
    property color itemTab_colorContextualHoverB: "#505050"

    property color itemTab_colorSelectA: buttonPush_colorCheckA
    property color itemTab_colorSelectB: buttonPush_colorCheckB

    property color itemTab_colorHighlightA: buttonPush_colorHighlightA
    property color itemTab_colorHighlightB: buttonPush_colorHighlightB

    property color itemTab_colorHighlightHoverA: buttonPush_colorHighlightHoverA
    property color itemTab_colorHighlightHoverB: buttonPush_colorHighlightHoverB

    property color itemTab_colorHighlightContextualA: "#00b4ff"
    property color itemTab_colorHighlightContextualB: "#0064c8"

    //---------------------------------------------------------------------------------------------
    // ItemWall

    property int itemWall_width : baseWall_itemPreferredWidth
    property int itemWall_height: baseWall_itemPreferredHeight

    property int itemWall_barHeight: dp32

    property int itemWall_iconWidth : itemTab_iconWidth
    property int itemWall_iconHeight: itemTab_iconHeight

    //---------------------------------------------------------------------------------------------
    // ComponentCompletion

    property int componentCompletion_height: itemList_height

    //---------------------------------------------------------------------------------------------
    // ComponentTabBrowser

    property int componentTabBrowser_iconWidth: dp42

    //---------------------------------------------------------------------------------------------
    // ComponentWallBookmarkTrack

    property int componentWallBookmarkTrack_iconWidth: dp42

    property color componentWallBookmarkTrack_colorA: "black"
    property color componentWallBookmarkTrack_colorB: "black"

    //---------------------------------------------------------------------------------------------
    // ContextualCategory

    property int contextualCategory_height: barTitle_height + border_size

    property int contextualCategory_padding: dp8

    property int contextualCategory_pixelSize: barTitleText_pixelSize

    property color contextualCategory_colorA: barTitle_colorA
    property color contextualCategory_colorB: barTitle_colorB

    property color contextualCategory_colorBorderLine: barTitle_colorBorderLine

    //---------------------------------------------------------------------------------------------
    // ContextualItem

    property int contextualItem_height: itemList_height

    property int contextualItem_padding: dp8

    //---------------------------------------------------------------------------------------------
    // ContextualItemCover

    property int contextualItemCover_spacing: dp8

    //---------------------------------------------------------------------------------------------
    // ContextualItemConfirm

    property int contextualItemConfirm_height: itemList_height

    property int contextualItemConfirm_paddingRight: dp2

    //---------------------------------------------------------------------------------------------
    // Images

    property url image_blank: "pictures/blank.png"

    //---------------------------------------------------------------------------------------------
    // Icons

    property url icon_iconify : "pictures/icons/scale/iconify.svg"
    property url icon_maximize: "pictures/icons/scale/maximize.svg"
    property url icon_minimize: "pictures/icons/scale/minimize.svg"
    property url icon_close   : "pictures/icons/scale/close.svg"

    property url icon_previous: "pictures/icons/scale/previous.svg"
    property url icon_next    : "pictures/icons/scale/next.svg"

    property url icon_slideLeft : "pictures/icons/scale/slideLeft.svg"
    property url icon_slideRight: "pictures/icons/scale/slideRight.svg"
    property url icon_slideUp   : "pictures/icons/scale/slideUp.svg"
    property url icon_slideDown : "pictures/icons/scale/slideDown.svg"

    property url icon_extend: "pictures/icons/scale/extend.svg"
    property url icon_shrink: "pictures/icons/scale/shrink.svg"

    property url icon_contextualDown: "pictures/icons/scale/contextualDown.svg"

    property url icon_add: "pictures/icons/scale/add.svg"

    property url icon_external: "pictures/icons/scale/external.svg"
    property url icon_related : "pictures/icons/scale/related.svg"
    property url icon_hd      : "pictures/icons/scale/hd.svg"

    //---------------------------------------------------------------------------------------------

    property url icon_scrollLeft : "pictures/icons/scale/scrollLeft.svg"
    property url icon_scrollRight: "pictures/icons/scale/scrollRight.svg"
    property url icon_scrollUp   : "pictures/icons/scale/scrollUp.svg"
    property url icon_scrollDown : "pictures/icons/scale/scrollDown.svg"

    //---------------------------------------------------------------------------------------------

    property url icon_play : "pictures/icons/scale/play.svg"
    property url icon_pause: "pictures/icons/scale/pause.svg"

    property url icon_backward: "pictures/icons/scale/backward.svg"
    property url icon_forward : "pictures/icons/scale/forward.svg"

    property url icon_audioMin: "pictures/icons/scale/audioMin.svg"
    property url icon_audioMax: "pictures/icons/scale/audioMax.svg"

    //---------------------------------------------------------------------------------------------

    property url icon_loading: "pictures/icons/scale/loading.svg"

    //---------------------------------------------------------------------------------------------
    // 16x16

    property url icon16x16_iconify : vector("pictures/icons/16x16/iconify.png",  icon_iconify)
    property url icon16x16_maximize: vector("pictures/icons/16x16/maximize.png", icon_maximize)
    property url icon16x16_minimize: vector("pictures/icons/16x16/minimize.png", icon_minimize)
    property url icon16x16_close   : vector("pictures/icons/16x16/close.png",    icon_close)

    property url icon16x16_previous: vector("pictures/icons/16x16/previous.png", icon_previous)
    property url icon16x16_next    : vector("pictures/icons/16x16/next.png",     icon_next)

    property url icon16x16_contextualDown: vector("pictures/icons/16x16/contextualDown.png",
                                                  icon_contextualDown)

    property url icon16x16_add: vector("pictures/icons/16x16/add.png", icon_add)

    property url icon16x16_external: vector("pictures/icons/16x16/external.png", icon_external)

    //---------------------------------------------------------------------------------------------

    property url icon16x16_scrollLeft: vector("pictures/icons/16x16/scrollLeft.png",
                                              icon_scrollLeft)

    property url icon16x16_scrollRight: vector("pictures/icons/16x16/scrollRight.png",
                                               icon_scrollRight)

    property url icon16x16_scrollUp: vector("pictures/icons/16x16/scrollUp.png",
                                            icon_scrollUp)

    property url icon16x16_scrollDown: vector("pictures/icons/16x16/scrollDown.png",
                                              icon_scrollDown)

    //---------------------------------------------------------------------------------------------

    property url icon16x16_audioMin: vector("pictures/icons/16x16/audioMin.png", icon_audioMin)
    property url icon16x16_audioMax: vector("pictures/icons/16x16/audioMax.png", icon_audioMax)

    //---------------------------------------------------------------------------------------------
    // 24x24

    property url icon24x24_slideLeft: vector("pictures/icons/24x24/slideLeft.png",
                                             icon_slideLeft)

    property url icon24x24_slideRight: vector("pictures/icons/24x24/slideRight.png",
                                              icon_slideRight)

    property url icon24x24_slideUp: vector("pictures/icons/24x24/slideUp.png",
                                           icon_slideUp)

    property url icon24x24_slideDown: vector("pictures/icons/24x24/slideDown.png",
                                             icon_slideDown)

    property url icon24x24_extend: vector("pictures/icons/24x24/extend.png", icon_extend)
    property url icon24x24_shrink: vector("pictures/icons/24x24/shrink.png", icon_shrink)

    property url icon24x24_related: vector("pictures/icons/24x24/related.png", icon_related)

    //---------------------------------------------------------------------------------------------

    property url icon24x24_play : vector("pictures/icons/24x24/play.png",  icon_play)
    property url icon24x24_pause: vector("pictures/icons/24x24/pause.png", icon_pause)

    property url icon24x24_backward: vector("pictures/icons/24x24/backward.png", icon_backward)
    property url icon24x24_forward : vector("pictures/icons/24x24/forward.png",  icon_forward)

    //---------------------------------------------------------------------------------------------
    // 28x28

    property url icon28x28_hd: vector("pictures/icons/28x28/hd.png", icon_hd)

    //---------------------------------------------------------------------------------------------
    // 32x32

    property url icon32x32_loading: icon_loading

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
