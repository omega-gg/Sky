//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkyComponents.

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

Style
{
    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------
    // Logo

    property real logoRatio: 8.0

    property color logo_colorA: "#c8c8c8"
    property color logo_colorB: logo_colorA

//#RectangleLive
    //---------------------------------------------------------------------------------------------
    // RectangleLive

    property int rectangleLive_size: dp10

    property color rectangleLive_colorBorder: "#161616"
//#END

//#TextDate
    //---------------------------------------------------------------------------------------------
    // TextDate

    property color textDate_color     : text3_color
    property color textDate_colorHover: text1_color
//#END

//#TextListDefault
    //---------------------------------------------------------------------------------------------
    // TextListDefault

    property int textListDefault_pixelSize: dp14
//#END

//#LineHorizontal:LineVertical
    //---------------------------------------------------------------------------------------------
    // Line

    property int line_size: dp1
//#END

//#BorderImageShadow
    //---------------------------------------------------------------------------------------------
    // BorderImageShadow

    property real borderImageShadow_ratio: 4.0

    property int borderImageShadow_adjust: dp2

    property real borderImageShadow_shadowOpacity: 0.8

    property url borderImageShadow_source: "pictures/shadow.png"

    property int borderImageShadow_borderSize: 128
//#END

//#IconOverlay
    //---------------------------------------------------------------------------------------------
    // IconOverlay

    property real iconOverlay_opacity: 0.8

    property color iconOverlay_colorA: "#00b4ff"
    property color iconOverlay_colorB: iconOverlay_colorA

    property ImageFilterColor iconOverlay_filter: icon2_filter
//#END

//#IconLoading
    //---------------------------------------------------------------------------------------------
    // IconLoading

    property int iconLoading_durationAnimation: ms800
//#END

//#Panel
    //---------------------------------------------------------------------------------------------
    // Panel

    property color panel_color: "#c8c8c8"
//#END

//#PanelContextual
    //---------------------------------------------------------------------------------------------
    // PanelContextual

    property real panelContextual_margins: -(window_borderSize)

    property real panelContextual_backgroundOpacity: 0.94
//#END

//#PanelImage
    //---------------------------------------------------------------------------------------------
    // PanelImage

    property color panelImage_color: "black"
//#END

//#BaseToolTip
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
//#END

//#ToolTip
    //---------------------------------------------------------------------------------------------
    // ToolTip

    property int toolTip_maximumWidth: dp288

    property int toolTip_padding: dp8
    property int toolTip_spacing: dp8
//#END

//#BarWindow
    //---------------------------------------------------------------------------------------------
    // BarWindow

    property int barWindow_height: dp24

    property color barWindow_colorA: panel_color
    property color barWindow_colorB: barWindow_colorA

    property color barWindow_colorDisableA: "#dcdcdc"
    property color barWindow_colorDisableB: barWindow_colorDisableA

    property color barWindow_colorBorderLine: "#969696"
//#END

//#BarTitle
    //---------------------------------------------------------------------------------------------
    // BarTitle

    property int barTitle_height: dp32

    property int barTitle_borderSize: border_size

    property color barTitle_colorA: barWindow_colorA
    property color barTitle_colorB: barWindow_colorB

    property color barTitle_colorBorderLine: barWindow_colorBorderLine
//#END

//#BarTitleSmall
    //---------------------------------------------------------------------------------------------
    // BarTitleSmall

    property int barTitleSmall_height: dp26

    property color barTitleSmall_colorA: barWindow_colorA
    property color barTitleSmall_colorB: barWindow_colorB

    property color barTitleSmall_colorBorderLine: "#808080"
//#END

//#BarTitleText
    //---------------------------------------------------------------------------------------------
    // BarTitleText

    property int barTitleText_leftMargin : dp8
    property int barTitleText_rightMargin: dp8

    property int barTitleText_pixelSize: dp14
//#END

//#BarProgress
    //---------------------------------------------------------------------------------------------
    // BarProgress

    property int barProgress_height: dp2

    property color barProgress_colorA: sliderStream_colorBarB
    property color barProgress_colorB: sliderStream_colorBarA

    property color barProgress_colorDisableA: "#969696"
    property color barProgress_colorDisableB: barProgress_colorDisableA
//#END

//#BaseButton
    //---------------------------------------------------------------------------------------------
    // Button

    property color button_colorBorder: border_color
    property color button_colorFocus : border_colorFocus

    property color button_colorTextA: text1_color
    property color button_colorTextB: text2_color

    property color button_colorConfirmHoverA: "#dc0000"
    property color button_colorConfirmHoverB: button_colorConfirmHoverA

    property color button_colorConfirmPressA: "#b40000"
    property color button_colorConfirmPressB: button_colorConfirmPressA

    property ImageFilterColor button_filterIconA: icon1_filter
    property ImageFilterColor button_filterIconB: icon2_filter

    property ImageFilterColor button_filterIconShadow: icon_filterShadow
    property ImageFilterColor button_filterIconSunken: icon_filterSunken
//#END

//#BaseButtonPush
    //---------------------------------------------------------------------------------------------
    // ButtonPush

    property int buttonPush_height: dp40

    property int buttonPush_padding: dp14
    property int buttonPush_spacing: dp6

    property color buttonPush_colorA: "#dcdcdc"
    property color buttonPush_colorB: buttonPush_colorA

    property color buttonPush_colorHoverA: "#f0f0f0"
    property color buttonPush_colorHoverB: buttonPush_colorHoverA

    property color buttonPush_colorPressA: "#b4b4b4"
    property color buttonPush_colorPressB: buttonPush_colorPressA

    property color buttonPush_colorPressHoverA: buttonPush_colorHoverA
    property color buttonPush_colorPressHoverB: buttonPush_colorHoverB

    property color buttonPush_colorHighlightA: color_highlight
    property color buttonPush_colorHighlightB: buttonPush_colorHighlightA

    property color buttonPush_colorHighlightHoverA: "#00a0f0"
    property color buttonPush_colorHighlightHoverB: buttonPush_colorHighlightHoverA

    property color buttonPush_colorCheckA: buttonPush_colorHighlightA
    property color buttonPush_colorCheckB: buttonPush_colorHighlightB

    property color buttonPush_colorCheckHoverA: buttonPush_colorHighlightHoverA
    property color buttonPush_colorCheckHoverB: buttonPush_colorHighlightHoverB
//#END

//#ButtonPushIcon
    //---------------------------------------------------------------------------------------------
    // ButtonPushIcon

    property int buttonPushIcon_width : buttonPush_height
    property int buttonPushIcon_height: buttonPush_height
//#END

//#ButtonPushOverlay
    //---------------------------------------------------------------------------------------------
    // ButtonPushOverlay

    property real buttonPushOverlay_opacityDefault: 0.8
    property real buttonPushOverlay_opacityHover  : 1.0

    property color buttonPushOverlay_colorBorder: icon2_colorA

    property color buttonPushOverlay_colorA: "#323232"
    property color buttonPushOverlay_colorB: buttonPushOverlay_colorA

    property color buttonPushOverlay_colorHoverA: buttonPush_colorHighlightA
    property color buttonPushOverlay_colorHoverB: buttonPush_colorHighlightB

    property color buttonPushOverlay_colorPressA: buttonPush_colorHighlightHoverA
    property color buttonPushOverlay_colorPressB: buttonPush_colorHighlightHoverB

    property ImageFilterColor buttonPushOverlay_filterIcon: icon2_filter
//#END

//#ButtonExtra*
    //---------------------------------------------------------------------------------------------
    // ButtonExtra

    property int buttonExtra_width: dp256
//#END

//#BaseButtonPiano
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
//#END

//#ButtonPianoIcon
    //---------------------------------------------------------------------------------------------
    // ButtonPianoIcon

    property int buttonPianoIcon_size: buttonPiano_height
//#END

//#ButtonPianoWindow
    //---------------------------------------------------------------------------------------------
    // ButtonPianoWindow

    property int buttonPianoWindow_width: dp40
//#END

//#ButtonPianoReset
    //---------------------------------------------------------------------------------------------
    // ButtonPianoReset

    property int buttonPianoReset_durationOpacity: duration_normal
//#END

//#ButtonRound
    //---------------------------------------------------------------------------------------------
    // ButtonRound

    property int buttonRound_width : dp48
    property int buttonRound_height: dp48
//#END

//#ButtonCheck
    //---------------------------------------------------------------------------------------------
    // ButtonCheck

    property int buttonCheck_width : dp64
    property int buttonCheck_height: buttonPush_height

    property color buttonCheck_colorA: buttonPush_colorPressA
    property color buttonCheck_colorB: buttonPush_colorPressB

    property color buttonCheck_colorActiveA: buttonPush_colorCheckA
    property color buttonCheck_colorActiveB: buttonPush_colorCheckB

    property color buttonCheck_colorHandleA: "#dcdcdc"
    property color buttonCheck_colorHandleB: buttonCheck_colorHandleA

    property color buttonCheck_colorHandleHoverA: "#f0f0f0"
    property color buttonCheck_colorHandleHoverB: buttonCheck_colorHandleHoverA

    property color buttonCheck_colorHandlePressA: buttonCheck_colorHandleA
    property color buttonCheck_colorHandlePressB: buttonCheck_colorHandleB
//#END

//#ButtonCheckLabel
    //---------------------------------------------------------------------------------------------
    // ButtonCheckLabel

    property int buttonCheckLabel_height: buttonCheck_height

    property int buttonCheckLabel_padding: dp4
//#END

//#ButtonImageBorders
    //---------------------------------------------------------------------------------------------
    // ButtonImageBorders

    property color buttonImageBorders_colorBackground: buttonPush_colorA

    property color buttonImageBorders_colorBorder     : border_color
    property color buttonImageBorders_colorBorderHover: border_colorFocus
//#END

//#ButtonMask
    //---------------------------------------------------------------------------------------------
    // ButtonMask

    property int buttonMask_size: dp16

    property int buttonMask_styleSize: dp2

    property color buttonMask_colorHoverA: "#323232"
    property color buttonMask_colorHoverB: buttonMask_colorHoverA

    property color buttonMask_colorPressA: icon_colorA
    property color buttonMask_colorPressB: icon_colorB

    property ImageFilterColor buttonMask_filterDefault: icon_filter
    property ImageFilterColor buttonMask_filterShadow : icon_filterShadow
    property ImageFilterColor buttonMask_filterSunken : icon_filterSunken

    property alias buttonMask_filterHover: buttonMask_filterHover
    property alias buttonMask_filterPress: buttonMask_filterPress

    ImageFilterColor
    {
        id: buttonMask_filterHover

        gradient: ScaleGradient
        {
            ScaleGradientStop { position: 0.0; color: buttonMask_colorHoverA }
            ScaleGradientStop { position: 1.0; color: buttonMask_colorHoverB }
        }
    }

    ImageFilterColor
    {
        id: buttonMask_filterPress

        gradient: ScaleGradient
        {
            ScaleGradientStop { position: 0.0; color: buttonMask_colorPressA }
            ScaleGradientStop { position: 1.0; color: buttonMask_colorPressB }
        }
    }
//#END

//#ButtonStream
    //---------------------------------------------------------------------------------------------
    // ButtonStream

    property int buttonStream_width : dp70
    property int buttonStream_height: slider_height

    property color buttonStream_color: scrollBar_colorA
//#END

//#ButtonsItem
    //---------------------------------------------------------------------------------------------
    // ButtonsItem

    property int buttonsItem_buttonWidth : dp28
    property int buttonsItem_buttonHeight: dp30
//#END

//#BaseLabelRound
    //---------------------------------------------------------------------------------------------
    // LabelRound

    property int labelRound_height: dp40

    property int labelRound_padding: dp16
    property int labelRound_spacing: buttonPush_spacing

    property color labelRound_colorA: buttonPush_colorHoverA
    property color labelRound_colorB: labelRound_colorA

    property color labelRound_colorDisableA: buttonPush_colorPressA
    property color labelRound_colorDisableB: labelRound_colorDisableA

    property color labelRound_colorText: text_color

    property ImageFilterColor labelRound_filterIcon      : icon2_filter
    property ImageFilterColor labelRound_filterIconShadow: icon2_filterShadow
    property ImageFilterColor labelRound_filterIconSunken: icon2_filterSunken
//#END

//#LabelRoundIcon
    //---------------------------------------------------------------------------------------------
    // LabelRoundIcon

    property int labelRoundIcon_width : labelRound_height
    property int labelRoundIcon_height: labelRound_height
//#END

//#LabelRoundInfo
    //---------------------------------------------------------------------------------------------
    // LabelRoundInfo

    property color labelRoundInfo_color: labelRound_colorDisableA

    property color labelRoundInfo_colorText: text_color
//#END

//#LabelLoading*
    //---------------------------------------------------------------------------------------------
    // LabelLoading

    property int labelLoading_size: dp48

    property real labelLoading_opacity: 0.6

    property color labelLoading_colorA: buttonPushOverlay_colorA
    property color labelLoading_colorB: labelLoading_colorA
//#END

//#LabelLoadingText
    //---------------------------------------------------------------------------------------------
    // LabelLoadingText

    property int labelLoadingText_height: dp44

    property int labelLoadingText_padding: dp4
    property int labelLoadingText_spacing: dp2

    property int labelLoadingText_iconMargins: dp6

    property color labelLoadingText_colorText: text2_color
//#END

//#LabelStream
    //---------------------------------------------------------------------------------------------
    // LabelStream

    property int labelStream_height: dp16

    property int labelStream_padding: dp4

    property color labelStream_colorA: scrollBar_colorA
    property color labelStream_colorB: scrollBar_colorB
//#END

//#Popup
    //---------------------------------------------------------------------------------------------
    // Popup

    property int popup_durationAnimation: duration_fast

    property int popup_interval: 5000
//#END

//#CheckBox
    //---------------------------------------------------------------------------------------------
    // CheckBox

    property int checkBox_width : dp22
    property int checkBox_height: checkBox_width

    property url checkBox_sourceCheck: "pictures/checkBoxCheck.png"
//#END

//#CheckBoxText
    //---------------------------------------------------------------------------------------------
    // CheckBoxText

    property int checkBoxText_height: dp28

    property int checkBoxText_padding: dp6
    property int checkBoxText_spacing: dp4
//#END

//#LineEdit*
    //---------------------------------------------------------------------------------------------
    // LineEdit

    property int lineEdit_width : dp192
    property int lineEdit_height: dp40

    property int lineEdit_padding: dp13

    property color lineEdit_color     : "#f0f0f0"
    property color lineEdit_colorHover: "white"
//#END

//#LineEditLabel
    //---------------------------------------------------------------------------------------------
    // LineEditLabel

    property int lineEditLabel_lineEditLeftMargin: dp26

    property int lineEditLabel_labelPaddingLeft : dp16
    property int lineEditLabel_labelPaddingRight: dp30
//#END

//#LineEditBox
    //---------------------------------------------------------------------------------------------
    // LineEditBox

    property color lineEditBox_colorA: lineEdit_color
    property color lineEditBox_colorB: lineEditBox_colorA

    property color lineEditBox_colorHoverA: lineEdit_colorHover
    property color lineEditBox_colorHoverB: lineEditBox_colorHoverA

    property color lineEditBox_colorActive: lineEdit_colorHover

    property color lineEditBox_colorDefault     : text3_color
    property color lineEditBox_colorDefaultHover: text1_color
//#END

//#BaseList
    //---------------------------------------------------------------------------------------------
    // List

    property int list_itemHeight: dp32
    property int list_itemSize  : list_itemHeight + border_size
//#END

//#ScrollBar
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

    property color scrollBar_colorHandlePressA: buttonPush_colorA
    property color scrollBar_colorHandlePressB: buttonPush_colorB

    property alias scrollBar_filterHorizontalDefault    : scrollBar_filterHorizontalDefault
    property alias scrollBar_filterHorizontalHandle     : scrollBar_filterHorizontalHandle
    property alias scrollBar_filterHorizontalHandleHover: scrollBar_filterHorizontalHandleHover
    property alias scrollBar_filterHorizontalHandlePress: scrollBar_filterHorizontalHandlePress

    ImageFilterColor
    {
        id: scrollBar_filterHorizontalDefault

        gradient: ScaleGradient
        {
            type: ScaleGradient.LinearHorizontal

            ScaleGradientStop { position: 0.0; color: scrollBar_colorA }
            ScaleGradientStop { position: 1.0; color: scrollBar_colorB }
        }
    }

    ImageFilterColor
    {
        id: scrollBar_filterHorizontalHandle

        gradient: ScaleGradient
        {
            type: ScaleGradient.LinearHorizontal

            ScaleGradientStop { position: 0.0; color: scrollBar_colorHandleA }
            ScaleGradientStop { position: 1.0; color: scrollBar_colorHandleB }
        }
    }

    ImageFilterColor
    {
        id: scrollBar_filterHorizontalHandleHover

        gradient: ScaleGradient
        {
            type: ScaleGradient.LinearHorizontal

            ScaleGradientStop { position: 0.0; color: scrollBar_colorHandleHoverA }
            ScaleGradientStop { position: 1.0; color: scrollBar_colorHandleHoverB }
        }
    }

    ImageFilterColor
    {
        id: scrollBar_filterHorizontalHandlePress

        gradient: ScaleGradient
        {
            type: ScaleGradient.LinearHorizontal

            ScaleGradientStop { position: 0.0; color: scrollBar_colorHandlePressA }
            ScaleGradientStop { position: 1.0; color: scrollBar_colorHandlePressB }
        }
    }
//#END

//#ScrollerVertical
    //---------------------------------------------------------------------------------------------
    // Scroller

    property real scroller_scroll: 1.0

    property int scroller_durationOpacity: duration_faster

    property int scroller_intervalA: 300
    property int scroller_intervalB: 20

    property int scroller_size: dp38

    property real scroller_opacityA: 0.6
    property real scroller_opacityB: 0.8

    property ImageFilterColor scroller_filter: border_filterFocus
//#END

//#ScrollerList
    //---------------------------------------------------------------------------------------------
    // ScrollerList

    property real scrollerList_scroll: 0.1
//#END

//#Slider
    //---------------------------------------------------------------------------------------------
    // Slider

    property int slider_width : dp128
    property int slider_height: dp26

    property color slider_colorA: scrollBar_colorA
    property color slider_colorB: scrollBar_colorB

    property color slider_colorHandleA: buttonCheck_colorHandleA
    property color slider_colorHandleB: buttonCheck_colorHandleB

    property color slider_colorHandleHoverA: buttonCheck_colorHandleHoverA
    property color slider_colorHandleHoverB: buttonCheck_colorHandleHoverB

    property color slider_colorHandlePressA: buttonCheck_colorHandlePressA
    property color slider_colorHandlePressB: buttonCheck_colorHandlePressB

    property color slider_colorBarA: buttonPush_colorA
    property color slider_colorBarB: buttonPush_colorB

    property color slider_colorBarHoverA: buttonPush_colorHoverA
    property color slider_colorBarHoverB: buttonPush_colorHoverB
//#END

//#SliderVolume
    //---------------------------------------------------------------------------------------------
    // SliderVolume

    property int sliderVolume_width : dp128
    property int sliderVolume_height: dp32

    property int sliderVolume_margins: dp4
//#END

//#SliderStream
    //---------------------------------------------------------------------------------------------
    // SliderStream

    property int sliderStream_durationAnimation: duration_slower

    property int sliderStream_intervalProgress: ms1000

    property int sliderStream_height: dp32

    property int sliderStream_textWidth: dp46

    property real sliderStream_opacityProgressA: 0.5
    property real sliderStream_opacityProgressB: 1.0

    property color sliderStream_colorBarA: color_highlight
    property color sliderStream_colorBarB: sliderStream_colorBarA

    property color sliderStream_colorBarHoverA: "#00a0f0"
    property color sliderStream_colorBarHoverB: sliderStream_colorBarHoverA

    property color sliderStream_colorBarDisableA: "#dcdcdc"
    property color sliderStream_colorBarDisableB: sliderStream_colorBarDisableA

    property color sliderStream_colorBarDisableHoverA: "#f0f0f0"
    property color sliderStream_colorBarDisableHoverB: sliderStream_colorBarDisableHoverA

    property color sliderStream_colorBarProgressA: sliderStream_colorBarDisableHoverA
    property color sliderStream_colorBarProgressB: sliderStream_colorBarDisableHoverB
//#END

//#BaseTabs
    //---------------------------------------------------------------------------------------------
    // BaseTabs

    property int baseTabs_height: dp32

    property int baseTabs_tabMinimum: dp64
    property int baseTabs_tabMaximum: dp192

    property int baseTabs_borderSize: border_size

    property int baseTabs_spacing: dp4
//#END

//#TabsBrowser
    //---------------------------------------------------------------------------------------------
    // TabsBrowser

    property int tabsBrowser_intervalResize: ms300

    property int tabsBrowser_tabMinimum: dp56
    property int tabsBrowser_tabMaximum: dp288

    property int tabsBrowser_spacing: dp8
//#END

//#TabsTrack
    //---------------------------------------------------------------------------------------------
    // TabsTrack

    property int tabsTrack_interval: 60000 // 1 minute
//#END

//#TabsPlayer
    //---------------------------------------------------------------------------------------------
    // TabsPlayer

    property int tabsPlayer_overlayWidth : componentTabBrowser_iconWidth
    property int tabsPlayer_overlayHeight: itemTab_iconHeight
//#END

//#BaseWall
    //---------------------------------------------------------------------------------------------
    // BaseWall

    property int baseWall_intervalUpdate: ms300

    property int baseWall_itemPreferredWidth : dp240
    property int baseWall_itemPreferredHeight: dp180

    property real baseWall_itemRatioWidth : 1.333
    property real baseWall_itemRatioHeight: 0.75

    property int baseWall_itemExtraWidth : border_size * 2
    property int baseWall_itemExtraHeight: border_size * 3 + dp32

    property int baseWall_marginsMinSize: dp24
//#END

//#WallBookmarkTrack
    //---------------------------------------------------------------------------------------------
    // WallBookmarkTrack

    property int wallBookmarkTrack_fixedDuration: ms1000
//#END

//#WallVideo
    //---------------------------------------------------------------------------------------------
    // WallVideo

    property int wallVideo_interval: 10000

    property int wallVideo_overlayWidth : componentWallBookmarkTrack_iconWidth
    property int wallVideo_overlayHeight: itemWall_iconHeight

    property color wallVideo_colorPlayer: "black"

    property color wallVideo_colorShadow: color_highlight

    property alias wallVideo_filterShadow: wallVideo_filterShadow

    ImageFilterColor
    {
        id: wallVideo_filterShadow

        color: wallVideo_colorShadow
    }
//#END

//#PlayerBrowser
    //---------------------------------------------------------------------------------------------
    // PlayerBrowser

    property real playerBrowser_opacity: 0.6

    property color playerBrowser_colorBar: buttonPushOverlay_colorA

    property color playerBrowser_colorText     : "#dcdcdc"
    property color playerBrowser_colorTextHover: text2_color
//#END

//#ItemList
    //---------------------------------------------------------------------------------------------
    // ItemList

    property int itemList_height: list_itemSize

    property int itemList_iconWidth : list_itemHeight
    property int itemList_iconHeight: list_itemHeight

    property color itemList_colorA: "#dcdcdc"
    property color itemList_colorB: itemList_colorA

    property color itemList_colorDefaultA: "#c8c8c8"
    property color itemList_colorDefaultB: itemList_colorDefaultA

    property color itemList_colorHoverA: "#f0f0f0"
    property color itemList_colorHoverB: itemList_colorHoverA

    property color itemList_colorPressA: "white"
    property color itemList_colorPressB: itemList_colorPressA

    property color itemList_colorContextualHoverA: itemList_colorPressA
    property color itemList_colorContextualHoverB: itemList_colorPressB

    property color itemList_colorSelectA: buttonPush_colorHighlightA
    property color itemList_colorSelectB: buttonPush_colorHighlightB

    property color itemList_colorSelectHoverA: buttonPush_colorHighlightHoverA
    property color itemList_colorSelectHoverB: buttonPush_colorHighlightHoverB

    property color itemList_colorSelectFocusA: buttonPush_colorHighlightHoverA
    property color itemList_colorSelectFocusB: itemList_colorSelectFocusA

    property color itemList_colorCurrentA: itemList_colorHoverA
    property color itemList_colorCurrentB: itemList_colorHoverB

    property color itemList_colorCurrentHoverA: itemList_colorPressA
    property color itemList_colorCurrentHoverB: itemList_colorPressB

    property color itemList_colorBorder       : border_colorLight
    property color itemList_colorBorderDefault: "#b4b4b4"
    property color itemList_colorBorderBar    : "#969696"

    property color itemList_colorText        : "#646464"
    property color itemList_colorTextHover   : text_color
    property color itemList_colorTextSelected: text2_color
    property color itemList_colorTextCurrent : text_colorCurrent
//#END

//#ItemTab
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

    property color itemTab_colorContextualHoverA: "white"
    property color itemTab_colorContextualHoverB: itemTab_colorContextualHoverA

    property color itemTab_colorSelectA: buttonPush_colorCheckA
    property color itemTab_colorSelectB: buttonPush_colorCheckB

    property color itemTab_colorHighlightA: buttonPush_colorHighlightA
    property color itemTab_colorHighlightB: buttonPush_colorHighlightB

    property color itemTab_colorHighlightHoverA: buttonPush_colorHighlightHoverA
    property color itemTab_colorHighlightHoverB: buttonPush_colorHighlightHoverB

    property color itemTab_colorHighlightContextualA: "#00b4ff"
    property color itemTab_colorHighlightContextualB: itemTab_colorHighlightContextualA
//#END

//#ItemWall
    //---------------------------------------------------------------------------------------------
    // ItemWall

    property int itemWall_width : baseWall_itemPreferredWidth
    property int itemWall_height: baseWall_itemPreferredHeight

    property int itemWall_barHeight: dp32

    property int itemWall_iconWidth : itemTab_iconWidth
    property int itemWall_iconHeight: itemTab_iconHeight
//#END

//#ComponentList
    //---------------------------------------------------------------------------------------------
    // ComponentList

    property int componentList_height: itemList_height
//#END

//#ComponentCompletion
    //---------------------------------------------------------------------------------------------
    // ComponentCompletion

    property int componentCompletion_pixelSize: dp14
//#END

//#ComponentTabBrowser
    //---------------------------------------------------------------------------------------------
    // ComponentTabBrowser

    property int componentTabBrowser_iconWidth: dp56
//#END

//#ComponentWallBookmarkTrack
    //---------------------------------------------------------------------------------------------
    // ComponentWallBookmarkTrack

    property int componentWallBookmarkTrack_iconWidth: dp56

    property color componentWallBookmarkTrack_colorA: wallVideo_colorPlayer
    property color componentWallBookmarkTrack_colorB: componentWallBookmarkTrack_colorA
//#END

//#ContextualCategory
    //---------------------------------------------------------------------------------------------
    // ContextualCategory

    property int contextualCategory_height: barTitle_height + border_size

    property int contextualCategory_padding: dp8

    property int contextualCategory_pixelSize: barTitleText_pixelSize

    property color contextualCategory_colorA: barTitle_colorA
    property color contextualCategory_colorB: barTitle_colorB

    property color contextualCategory_colorBorderLine: barTitle_colorBorderLine
//#END

//#ContextualItem
    //---------------------------------------------------------------------------------------------
    // ContextualItem

    property int contextualItem_height: itemList_height

    property int contextualItem_padding: dp8
//#END

//#ContextualItemCover
    //---------------------------------------------------------------------------------------------
    // ContextualItemCover

    property int contextualItemCover_spacing: dp8
//#END

//#ContextualItemConfirm
    //---------------------------------------------------------------------------------------------
    // ContextualItemConfirm

    property int contextualItemConfirm_height: contextualItem_height
//#END

    //---------------------------------------------------------------------------------------------
    // Images

//#pictures_blank:ScrollBar
    property url picture_blank: "pictures/blank.png"
//#END

    //---------------------------------------------------------------------------------------------
    // Icons

//#icons_window:BarWindow
    property url icon_iconify : "icons/window-minimize.svg"
    property url icon_maximize: "icons/window-maximize.svg"
    property url icon_minimize: "icons/window-restore.svg"

    property url icon12x12_iconify : icon_iconify
    property url icon12x12_maximize: icon_maximize
    property url icon12x12_minimize: icon_minimize
//#END

//#icons_close:BarWindow:ButtonsItem:LabelLoadingButton:LineEditBoxClear:TabsBrowser
    property url icon_close: "icons/times.svg"

    property url icon12x12_close: icon_close
//#END

//#icons_navigate:BaseWall
    property url icon_previous: "icons/caret-left.svg"
    property url icon_next    : "icons/caret-right.svg"

    property url icon8x8_previous: icon_previous
    property url icon8x8_next    : icon_next
//#END

//#icons_slide:ContextualItemConfirm
    property url icon_slideLeft : "icons/chevron-left.svg"
    property url icon_slideRight: "icons/chevron-right.svg"

    property url icon12x12_slideLeft : icon_slideLeft
    property url icon12x12_slideRight: icon_slideRight
//#END

//#icons_slide
    property url icon_slideUp   : "icons/chevron-up.svg"
    property url icon_slideDown : "icons/chevron-down.svg"

    property url icon16x16_slideUp  : icon_slideUp
    property url icon16x16_slideDown: icon_slideDown
//#END

//#icons_scale:WallVideo
    property url icon_extend: "icons/expand-arrows-alt.svg"

    property url icon20x20_extend: icon_extend
//#END

//#icons_scale:WallVideo
    property url icon_shrink: "icons/compress-arrows-alt.svg"

    property url icon20x20_shrink: icon_shrink
//#END

//#icons_contextual:ButtonsItem
    property url icon_contextualDown: "icons/caret-down.svg"

    property url icon10x10_contextualDown: icon_contextualDown
//#END

//#icons_external
    property url icon_external: "icons/external-link-alt.svg"

    property url icon16x16_external: icon_external
//#END

    //---------------------------------------------------------------------------------------------

//#icons_playback:TabsPlayer:PlayerBrowser
    property url icon_play: "icons/play.svg"

    property url icon16x16_play: icon_play
//#END

//#icons_playback
    property url icon_pause: "icons/pause.svg"

    property url icon16x16_pause: icon_pause
//#END

//#icons_playback:PlayerBrowser
    property url icon_backward: "icons/backward.svg"
    property url icon_forward : "icons/forward.svg"
//#END

//#icons_playback
    property url icon12x12_backward: icon_backward
    property url icon12x12_forward : icon_forward
//#END

//#icons_audio:SliderVolume
    property url icon_audioMin: "icons/volume-off.svg"
    property url icon_audioMax: "icons/volume-up.svg"

    property url icon16x16_audioMin: icon_audioMin
    property url icon16x16_audioMax: icon_audioMax
//#END

    //---------------------------------------------------------------------------------------------

//#icons_loading:IconLoading
    property url icon_loading: "icons/loading.svg"

    property url icon20x20_loading: icon_loading
//#END

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------
    // Text

    text_bold: true

//#QT_NEW
    //text_renderType: Text.NativeRendering
//#END

    //---------------------------------------------------------------------------------------------
    // Border

    border_colorFocus: "#00a0f0"

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

    function getTextColor(highlighed, active)
    {
        if (highlighed)
        {
            return button_colorTextB;
        }
        else if (active)
        {
            return text_colorCurrent;
        }
        else return button_colorTextA;
    }

    //---------------------------------------------------------------------------------------------

    function applyLight()
    {
        //-----------------------------------------------------------------------------------------
        // Global

        logo_colorA = "#c8c8c8";
        logo_colorB = logo_colorA;

        //-----------------------------------------------------------------------------------------
        // Text

        text_raised = Text.Normal;
        text_sunken = text_raised;

        text_colorCurrent = color_highlight;
        text_colorLink    = text_colorCurrent;

        text1_color = "#484848";
        text2_color = "white";
        text3_color = "#646464";

        //-----------------------------------------------------------------------------------------
        // Border

        border_color      = "#969696";
        border_colorLight = "#c8c8c8";
        border_colorFocus = "#00a0f0";

//#WindowSky
        //-----------------------------------------------------------------------------------------
        // Window

        window_color = "#b4b4b4";
//#END

        //-----------------------------------------------------------------------------------------
        // Icon

        icon_raised = Sk.IconNormal;
        icon_sunken = icon_raised;

        icon1_colorA = "#646464";
        icon2_colorA = "white";

//#IconOverlay
        //-----------------------------------------------------------------------------------------
        // IconOverlay

        iconOverlay_colorA = "#00b4ff";
        iconOverlay_colorB = iconOverlay_colorA;
//#END

//#Panel
        //-----------------------------------------------------------------------------------------
        // Panel

        panel_color = "#c8c8c8";
//#END

//#BarWindow
        //-----------------------------------------------------------------------------------------
        // BarWindow

        barWindow_colorA = panel_color;
        barWindow_colorB = barWindow_colorA;

        barWindow_colorDisableA = "#dcdcdc";
        barWindow_colorDisableB = barWindow_colorDisableA;
//#END

//#BarTitle
        //-----------------------------------------------------------------------------------------
        // BarTitle

        barTitleSmall_colorA = barWindow_colorA;
        barTitleSmall_colorB = barWindow_colorB;
//#END

//#BarProgress
        //-----------------------------------------------------------------------------------------
        // BarProgress

        barProgress_colorDisableA = "#969696";
        barProgress_colorDisableB = barProgress_colorDisableA;
//#END

//#BaseButton
        //-----------------------------------------------------------------------------------------
        // Button

        button_colorConfirmHoverA = "#dc0000";
        button_colorConfirmHoverB = button_colorConfirmHoverA;

        button_colorConfirmPressA = "#b40000";
        button_colorConfirmPressB = button_colorConfirmPressA;
//#END

//#BaseButtonPush
        //-----------------------------------------------------------------------------------------
        // ButtonPush

        buttonPush_colorA = "#dcdcdc";
        buttonPush_colorB = buttonPush_colorA;

        buttonPush_colorHoverA = "#f0f0f0";
        buttonPush_colorHoverB = buttonPush_colorHoverA;

        buttonPush_colorPressA = "#b4b4b4";
        buttonPush_colorPressB = buttonPush_colorPressA;

        buttonPush_colorPressHoverA = buttonPush_colorHoverA;
        buttonPush_colorPressHoverB = buttonPush_colorHoverB;

        buttonPush_colorHighlightA = color_highlight;
        buttonPush_colorHighlightB = buttonPush_colorHighlightA;

        buttonPush_colorHighlightHoverA = "#00a0f0";
        buttonPush_colorHighlightHoverB = buttonPush_colorHighlightHoverA;

        buttonPush_colorCheckA = buttonPush_colorHighlightA;
        buttonPush_colorCheckB = buttonPush_colorHighlightB;

        buttonPush_colorCheckHoverA = buttonPush_colorHighlightHoverA;
        buttonPush_colorCheckHoverB = buttonPush_colorHighlightHoverB;
//#END

//#ButtonPushOverlay
        //-----------------------------------------------------------------------------------------
        // ButtonPushOverlay

        buttonPushOverlay_colorBorder = icon2_colorA;

        buttonPushOverlay_colorA = "#323232";
        buttonPushOverlay_colorB = buttonPushOverlay_colorA;

        buttonPushOverlay_colorHoverA = buttonPush_colorHighlightA;
        buttonPushOverlay_colorHoverB = buttonPush_colorHighlightB;

        buttonPushOverlay_colorPressA = buttonPush_colorHighlightHoverA;
        buttonPushOverlay_colorPressB = buttonPush_colorHighlightHoverB;

        buttonPushOverlay_filterIcon = icon2_filter;
//#END

//#ButtonCheck
        //-----------------------------------------------------------------------------------------
        // ButtonCheck

        buttonCheck_colorHandleA = "#dcdcdc";
        buttonCheck_colorHandleB = buttonCheck_colorHandleA;

        buttonCheck_colorHandleHoverB = buttonCheck_colorHandleHoverA;

        buttonCheck_colorHandlePressA = buttonCheck_colorHandleA;
        buttonCheck_colorHandlePressB = buttonCheck_colorHandleB;
//#END

//#ButtonMask
        //-----------------------------------------------------------------------------------------
        // ButtonMask

        buttonMask_colorHoverA = "#323232";

        buttonMask_colorPressA = icon_colorA;
        buttonMask_colorPressB = icon_colorB;
//#END

//#BaseLabelRound
        //-----------------------------------------------------------------------------------------
        // LabelRound

        labelRound_colorA = buttonPush_colorHoverA;

        labelRound_filterIcon = icon2_filter;
//#END

//#LabelRoundInfo
        //-----------------------------------------------------------------------------------------
        // LabelRoundInfo

        labelRoundInfo_colorText = text_color;
//#END

//#LabelLoading*
        //-----------------------------------------------------------------------------------------
        // LabelLoading

        labelLoading_colorA = buttonPushOverlay_colorA;
//#END

//#LabelLoadingText
        //-----------------------------------------------------------------------------------------
        // LabelLoadingText

        labelLoadingText_colorText = text2_color;
//#END

//#BaseLineEdit
        //-----------------------------------------------------------------------------------------
        // BaseLineEdit

        baseLineEdit_colorText         = text1_color;
        baseLineEdit_colorTextSelected = text2_color;

        baseLineEdit_colorDefault = text3_color;
//#END

//#LineEdit*
        //-----------------------------------------------------------------------------------------
        // LineEdit

        lineEdit_color = "#f0f0f0";
//#END

//#LineEditBox
        //-----------------------------------------------------------------------------------------
        // LineEditBox

        lineEditBox_colorA = lineEdit_color;
        lineEditBox_colorB = lineEditBox_colorA;

        lineEditBox_colorHoverA = lineEdit_colorHover;
        lineEditBox_colorHoverB = lineEditBox_colorHoverA;
//#END

//#ScrollBar
        //-----------------------------------------------------------------------------------------
        // ScrollBar

        scrollBar_colorHandlePressA = buttonPush_colorA;
        scrollBar_colorHandlePressB = buttonPush_colorB;
//#END

//#SliderStream
        //-----------------------------------------------------------------------------------------
        // SliderStream

        sliderStream_colorBarA = color_highlight;
        sliderStream_colorBarB = sliderStream_colorBarA;

        sliderStream_colorBarHoverA = "#00a0f0";
        sliderStream_colorBarHoverB = sliderStream_colorBarHoverA;

        sliderStream_colorBarDisableA = "#dcdcdc";
        sliderStream_colorBarDisableB = sliderStream_colorBarDisableA;

        sliderStream_colorBarDisableHoverA = "#f0f0f0";
        sliderStream_colorBarDisableHoverB = sliderStream_colorBarDisableHoverA;

        sliderStream_colorBarProgressA = sliderStream_colorBarDisableHoverA;
        sliderStream_colorBarProgressB = sliderStream_colorBarDisableHoverB;
//#END

//#PlayerBrowser
        //-----------------------------------------------------------------------------------------
        // PlayerBrowser

        playerBrowser_colorText      = "#dcdcdc";
        playerBrowser_colorTextHover = text2_color;
//#END

//#ItemList
        //-----------------------------------------------------------------------------------------
        // ItemList

        itemList_colorA = "#dcdcdc";
        itemList_colorB = itemList_colorA;

        itemList_colorDefaultA = "#c8c8c8";
        itemList_colorDefaultB = itemList_colorDefaultA;

        itemList_colorHoverA = "#f0f0f0";
        itemList_colorHoverB = itemList_colorHoverA;

        itemList_colorPressA = "white";
        itemList_colorPressB = itemList_colorPressA;

        itemList_colorContextualHoverB = itemList_colorPressB;

        itemList_colorSelectFocusA = buttonPush_colorHighlightHoverA;
        itemList_colorSelectFocusB = itemList_colorSelectFocusA;

        itemList_colorCurrentA = itemList_colorHoverA;
        itemList_colorCurrentB = itemList_colorHoverB;

        itemList_colorCurrentHoverA = itemList_colorPressA;
        itemList_colorCurrentHoverB = itemList_colorPressB;

        itemList_colorBorder        = border_colorLight;
        itemList_colorBorderDefault = "#b4b4b4";
        itemList_colorBorderBar     = "#969696";

        itemList_colorText         = "#646464";
        itemList_colorTextSelected = text2_color;
//#END

//#ItemTab
        //-----------------------------------------------------------------------------------------
        // ItemTab

        itemTab_colorContextualHoverA = "white";
        itemTab_colorContextualHoverB = itemTab_colorContextualHoverA;

        itemTab_colorHighlightContextualA = "#00b4ff";
        itemTab_colorHighlightContextualB = itemTab_colorHighlightContextualA;
//#END
    }

    function applyNight()
    {
        //-----------------------------------------------------------------------------------------
        // Global

        logo_colorA = "#404040";
        logo_colorB = logo_colorA;

        //-----------------------------------------------------------------------------------------
        // Text

        text_raised = Text.Normal;
        text_sunken = text_raised;

        text_colorCurrent = "#00b4ff";
        text_colorLink    = color_highlight;

        text1_color = "white";
        text2_color = text1_color;
        text3_color = "#c8c8c8";

        //-----------------------------------------------------------------------------------------
        // Border

        border_color      = "#242424";
        border_colorLight = border_color;
        border_colorFocus = color_highlight;

//#WindowSky
        //-----------------------------------------------------------------------------------------
        // Window

        window_color = "#323232";
//#END

        //-----------------------------------------------------------------------------------------
        // Icon

        icon_raised = Sk.IconNormal;
        icon_sunken = icon_raised;

        icon1_colorA = "#dcdcdc";
        icon2_colorA = icon1_colorA;

//#IconOverlay
        //-----------------------------------------------------------------------------------------
        // IconOverlay

        iconOverlay_colorA = icon_colorActiveA;
        iconOverlay_colorB = iconOverlay_colorA;
//#END

//#Panel
        //-----------------------------------------------------------------------------------------
        // Panel

        panel_color = "#404040";
//#END

//#BarWindow
        //-----------------------------------------------------------------------------------------
        // BarWindow

        barWindow_colorA = "#565656";
        barWindow_colorB = barWindow_colorA;

        barWindow_colorDisableA = "#484848";
        barWindow_colorDisableB = barWindow_colorDisableA;
//#END

//#BarTitle
        //-----------------------------------------------------------------------------------------
        // BarTitle

        barTitleSmall_colorA = barWindow_colorA;
        barTitleSmall_colorB = barWindow_colorB;
//#END

//#BarProgress
        //-----------------------------------------------------------------------------------------
        // BarProgress

        barProgress_colorDisableA = sliderStream_colorBarDisableB;
        barProgress_colorDisableB = sliderStream_colorBarDisableA;
//#END

//#BaseButton
        //-----------------------------------------------------------------------------------------
        // Button

        button_colorConfirmHoverA = "#c80000";
        button_colorConfirmHoverB = button_colorConfirmHoverA;

        button_colorConfirmPressA = "#960000";
        button_colorConfirmPressB = button_colorConfirmPressA;
//#END

//#BaseButtonPush
        //-----------------------------------------------------------------------------------------
        // ButtonPush

        buttonPush_colorA = "#484848";
        buttonPush_colorB = buttonPush_colorA;

        buttonPush_colorHoverA = "#646464";
        buttonPush_colorHoverB = buttonPush_colorHoverA;

        buttonPush_colorPressA = "#323232";
        buttonPush_colorPressB = buttonPush_colorPressA;

        buttonPush_colorPressHoverA = buttonPush_colorHoverA;
        buttonPush_colorPressHoverB = buttonPush_colorHoverB;

        buttonPush_colorHighlightA = "#0078c8";
        buttonPush_colorHighlightB = buttonPush_colorHighlightA;

        buttonPush_colorHighlightHoverA = color_highlight;
        buttonPush_colorHighlightHoverB = buttonPush_colorHighlightHoverA;

        buttonPush_colorCheckA = buttonPush_colorHighlightA;
        buttonPush_colorCheckB = buttonPush_colorHighlightB;

        buttonPush_colorCheckHoverA = buttonPush_colorHighlightHoverA;
        buttonPush_colorCheckHoverB = buttonPush_colorHighlightHoverB;
//#END

//#ButtonPushOverlay
        //-----------------------------------------------------------------------------------------
        // ButtonPushOverlay

        buttonPushOverlay_colorBorder = icon_colorA;

        buttonPushOverlay_colorA = "#242424";
        buttonPushOverlay_colorB = buttonPushOverlay_colorA;

        buttonPushOverlay_colorHoverA = buttonPush_colorHighlightHoverA;
        buttonPushOverlay_colorHoverB = buttonPush_colorHighlightHoverB;

        buttonPushOverlay_colorPressA = buttonPush_colorCheckA;
        buttonPushOverlay_colorPressB = buttonPush_colorCheckB;

        buttonPushOverlay_filterIcon = button_filterIconA;
//#END

//#ButtonCheck
        //-----------------------------------------------------------------------------------------
        // ButtonCheck

        buttonCheck_colorHandleA = "#c8c8c8";
        buttonCheck_colorHandleB = buttonCheck_colorHandleA;

        buttonCheck_colorHandleHoverB = buttonCheck_colorHandleHoverA;

        buttonCheck_colorHandlePressA = buttonCheck_colorHandleA;
        buttonCheck_colorHandlePressB = buttonCheck_colorHandleB;
//#END

//#ButtonMask
        //-----------------------------------------------------------------------------------------
        // ButtonMask

        buttonMask_colorHoverA = "white";

        buttonMask_colorPressA = icon_colorA;
        buttonMask_colorPressB = icon_colorA;
//#END

//#BaseLabelRound
        //-----------------------------------------------------------------------------------------
        // LabelRound

        labelRound_colorA = buttonPush_colorHoverA;

        labelRound_filterIcon = icon_filter;
//#END

//#LabelRoundInfo
        //-----------------------------------------------------------------------------------------
        // LabelRoundInfo

        labelRoundInfo_colorText = labelRound_colorText;
//#END

//#LabelLoading*
        //-----------------------------------------------------------------------------------------
        // LabelLoading

        labelLoading_colorA = "black";
//#END

//#LabelLoadingText
        //-----------------------------------------------------------------------------------------
        // LabelLoadingText

        labelLoadingText_colorText = labelRound_colorText;
//#END

//#BaseLineEdit
        //-----------------------------------------------------------------------------------------
        // BaseLineEdit

        baseLineEdit_colorText         = "black";
        baseLineEdit_colorTextSelected = text1_color;

        baseLineEdit_colorDefault = "#323232";
//#END

//#LineEdit*
        //-----------------------------------------------------------------------------------------
        // LineEdit

        lineEdit_color = "#dcdcdc";
//#END

//#LineEditBox
        //-----------------------------------------------------------------------------------------
        // LineEditBox

        lineEditBox_colorA = "#323232";
        lineEditBox_colorB = lineEditBox_colorA;

        lineEditBox_colorHoverA = "#484848";
        lineEditBox_colorHoverB = lineEditBox_colorHoverA;
//#END

//#ScrollBar
        //-----------------------------------------------------------------------------------------
        // ScrollBar

        scrollBar_colorHandlePressA = buttonPush_colorA;
        scrollBar_colorHandlePressB = buttonPush_colorB;
//#END

//#SliderStream
        //-----------------------------------------------------------------------------------------
        // SliderStream

        sliderStream_colorBarA = color_highlight;
        sliderStream_colorBarB = sliderStream_colorBarA;

        sliderStream_colorBarHoverA = "#00a0f0";
        sliderStream_colorBarHoverB = sliderStream_colorBarHoverA;

        sliderStream_colorBarDisableA = "#969696";
        sliderStream_colorBarDisableB = sliderStream_colorBarDisableA;

        sliderStream_colorBarDisableHoverA = "#c8c8c8";
        sliderStream_colorBarDisableHoverB = sliderStream_colorBarDisableHoverA;

        sliderStream_colorBarProgressA = sliderStream_colorBarDisableHoverA;
        sliderStream_colorBarProgressB = sliderStream_colorBarDisableHoverB;
//#END

//#PlayerBrowser
        //-----------------------------------------------------------------------------------------
        // PlayerBrowser

        playerBrowser_colorText      = text3_color;
        playerBrowser_colorTextHover = "#dcdcdc";
//#END

//#ItemList
        //-----------------------------------------------------------------------------------------
        // ItemList

        itemList_colorA = "#323232";
        itemList_colorB = itemList_colorA;

        itemList_colorDefaultA = "#242424";
        itemList_colorDefaultB = itemList_colorDefaultA;

        itemList_colorHoverA = "#646464";
        itemList_colorHoverB = itemList_colorHoverA;

        itemList_colorPressA = "#808080";
        itemList_colorPressB = itemList_colorPressA;

        itemList_colorContextualHoverB = itemList_colorPressB;

        itemList_colorSelectFocusA = color_highlight;
        itemList_colorSelectFocusB = itemList_colorSelectFocusA;

        itemList_colorCurrentA = itemList_colorDefaultA;
        itemList_colorCurrentB = itemList_colorDefaultB;

        itemList_colorCurrentHoverA = "#484848";
        itemList_colorCurrentHoverB = itemList_colorCurrentHoverA;

        itemList_colorBorder        = border_colorLight;
        itemList_colorBorderDefault = "#161616";
        itemList_colorBorderBar     = "#646464";

        itemList_colorText         = "#dcdcdc";
        itemList_colorTextSelected = text_color;
//#END

//#ItemTab
        //-----------------------------------------------------------------------------------------
        // ItemTab

        itemTab_colorContextualHoverA = "#808080";
        itemTab_colorContextualHoverB = itemTab_colorContextualHoverA;

        itemTab_colorHighlightContextualA = color_highlight;
        itemTab_colorHighlightContextualB = itemTab_colorHighlightContextualA;
//#END
    }

    function applyClassic()
    {
        //-----------------------------------------------------------------------------------------
        // Global

        logo_colorA = "#323232";
        logo_colorB = "#646464";

        //-----------------------------------------------------------------------------------------
        // Text

        text_raised = Text.Raised;
        text_sunken = Text.Sunken;

        text_colorCurrent = "#00b4ff";
        text_colorLink    = color_highlight;

        text1_color = "white";
        text2_color = text1_color;
        text3_color = "#c8c8c8";

        //-----------------------------------------------------------------------------------------
        // Border

        border_color      = "#161616";
        border_colorLight = "#242424";
        border_colorFocus = color_highlight;

//#WindowSky
        //-----------------------------------------------------------------------------------------
        // Window

        window_color = "#323232";
//#END

        //-----------------------------------------------------------------------------------------
        // Icon

        icon_raised = Sk.IconRaised;
        icon_sunken = Sk.IconSunken;

        icon1_colorA = "#dcdcdc";
        icon2_colorA = icon1_colorA;

//#IconOverlay
        //-----------------------------------------------------------------------------------------
        // IconOverlay

        iconOverlay_colorA = icon_colorActiveA;
        iconOverlay_colorB = icon_colorActiveB;
//#END

//#Panel
        //-----------------------------------------------------------------------------------------
        // Panel

        panel_color = "#404040";
//#END

//#BarWindow
        //-----------------------------------------------------------------------------------------
        // BarWindow

        barWindow_colorA = "#808080";
        barWindow_colorB = "#484848";

        barWindow_colorDisableA = barWindow_colorB;
        barWindow_colorDisableB = barWindow_colorA;
//#END

//#BarTitle
        //-----------------------------------------------------------------------------------------
        // BarTitle

        barTitleSmall_colorA = "#646464";
        barTitleSmall_colorB = "#484848";
//#END

//#BarProgress
        //-----------------------------------------------------------------------------------------
        // BarProgress

        barProgress_colorDisableA = sliderStream_colorBarDisableB;
        barProgress_colorDisableB = sliderStream_colorBarDisableA;
//#END

//#BaseButton
        //-----------------------------------------------------------------------------------------
        // Button

        button_colorConfirmHoverA = "#c80000";
        button_colorConfirmHoverB = "#960000";

        button_colorConfirmPressA = "#960000";
        button_colorConfirmPressB = "#c80000";
//#END

//#BaseButtonPush
        //-----------------------------------------------------------------------------------------
        // ButtonPush

        buttonPush_colorA = "#646464";
        buttonPush_colorB = "#323232";

        buttonPush_colorHoverA = "#848484";
        buttonPush_colorHoverB = "#404040";

        buttonPush_colorPressA = "#242424";
        buttonPush_colorPressB = "#484848";

        buttonPush_colorPressHoverA = "#323232";
        buttonPush_colorPressHoverB = "#646464";

        buttonPush_colorHighlightA = color_highlight;
        buttonPush_colorHighlightB = "#003ca0";

        buttonPush_colorHighlightHoverA = "#00a0f0";
        buttonPush_colorHighlightHoverB = "#0050b4";

        buttonPush_colorCheckA = "#0050b4";
        buttonPush_colorCheckB = color_highlight;

        buttonPush_colorCheckHoverA = buttonPush_colorCheckA;
        buttonPush_colorCheckHoverB = "#00b4ff";
//#END

//#ButtonPushOverlay
        //-----------------------------------------------------------------------------------------
        // ButtonPushOverlay

        buttonPushOverlay_colorBorder = icon_colorA;

        buttonPushOverlay_colorA = "#323232";
        buttonPushOverlay_colorB = "black";

        buttonPushOverlay_colorHoverA = buttonPush_colorHighlightHoverA;
        buttonPushOverlay_colorHoverB = buttonPush_colorHighlightHoverB;

        buttonPushOverlay_colorPressA = buttonPush_colorCheckA;
        buttonPushOverlay_colorPressB = buttonPush_colorCheckB;

        buttonPushOverlay_filterIcon = button_filterIconA;
//#END

//#ButtonCheck
        //-----------------------------------------------------------------------------------------
        // ButtonCheck

        buttonCheck_colorHandleA = "#dcdcdc";
        buttonCheck_colorHandleB = "#8c8c8c";

        buttonCheck_colorHandleHoverB = "#c8c8c8";

        buttonCheck_colorHandlePressA = "#646464";
        buttonCheck_colorHandlePressB = "#f0f0f0";
//#END

//#ButtonMask
        //-----------------------------------------------------------------------------------------
        // ButtonMask

        buttonMask_colorHoverA = "white";

        buttonMask_colorPressA = buttonMask_colorHoverA;
        buttonMask_colorPressB = "#646464";
//#END

//#BaseLabelRound
        //-----------------------------------------------------------------------------------------
        // LabelRound

        labelRound_colorA = "#646464";

        labelRound_filterIcon = icon_filter;
//#END

//#LabelRoundInfo
        //-----------------------------------------------------------------------------------------
        // LabelRoundInfo

        labelRoundInfo_colorText = labelRound_colorText;
//#END

//#LabelLoading*
        //-----------------------------------------------------------------------------------------
        // LabelLoading

        labelLoading_colorA = "black";
//#END

//#LabelLoadingText
        //-----------------------------------------------------------------------------------------
        // LabelLoadingText

        labelLoadingText_colorText = labelRound_colorText;
//#END

//#BaseLineEdit
        //-----------------------------------------------------------------------------------------
        // BaseLineEdit

        baseLineEdit_colorText         = "black";
        baseLineEdit_colorTextSelected = text1_color;

        baseLineEdit_colorDefault = "#323232";
//#END

//#LineEdit*
        //-----------------------------------------------------------------------------------------
        // LineEdit

        lineEdit_color = "#dcdcdc";
//#END

//#LineEditBox
        //-----------------------------------------------------------------------------------------
        // LineEditBox

        lineEditBox_colorA = "#242424";
        lineEditBox_colorB = "#484848";

        lineEditBox_colorHoverA = "#323232";
        lineEditBox_colorHoverB = "#646464";
//#END

//#ScrollBar
        //-----------------------------------------------------------------------------------------
        // ScrollBar

        scrollBar_colorHandlePressA = buttonPush_colorPressHoverA;
        scrollBar_colorHandlePressB = buttonPush_colorPressHoverB;
//#END

//#SliderStream
        //-----------------------------------------------------------------------------------------
        // SliderStream

        sliderStream_colorBarA = "#00a0f0";
        sliderStream_colorBarB = "#0050b4";

        sliderStream_colorBarHoverA = "#00b4ff";
        sliderStream_colorBarHoverB = "#0064c8";

        sliderStream_colorBarDisableA = "#dcdcdc";
        sliderStream_colorBarDisableB = "#646464";

        sliderStream_colorBarDisableHoverA = "#f0f0f0";
        sliderStream_colorBarDisableHoverB = "#808080";

        sliderStream_colorBarProgressA = "#646464";
        sliderStream_colorBarProgressB = "#dcdcdc";
//#END

//#PlayerBrowser
        //-----------------------------------------------------------------------------------------
        // PlayerBrowser

        playerBrowser_colorText      = text3_color;
        playerBrowser_colorTextHover = "#dcdcdc";
//#END

//#ItemList
        //-----------------------------------------------------------------------------------------
        // ItemList

        itemList_colorA = "#383838";
        itemList_colorB = "#303030";

        itemList_colorDefaultA = "#282828";
        itemList_colorDefaultB = "#202020";

        itemList_colorHoverA = "#646464";
        itemList_colorHoverB = "#404040";

        itemList_colorPressA = "#808080";
        itemList_colorPressB = "#484848";

        itemList_colorContextualHoverB = "#484848";

        itemList_colorSelectFocusA = buttonPush_colorCheckA;
        itemList_colorSelectFocusB = buttonPush_colorCheckB;

        itemList_colorCurrentA = "#161616";
        itemList_colorCurrentB = "#282828";

        itemList_colorCurrentHoverA = itemList_colorCurrentA;
        itemList_colorCurrentHoverB = "#484848";

        itemList_colorBorder        = border_colorLight;
        itemList_colorBorderDefault = "#161616";
        itemList_colorBorderBar     = "#646464";

        itemList_colorText         = "#dcdcdc";
        itemList_colorTextSelected = text_color;
//#END

//#ItemTab
        //-----------------------------------------------------------------------------------------
        // ItemTab

        itemTab_colorContextualHoverA = "#a0a0a0";
        itemTab_colorContextualHoverB = "#484848";

        itemTab_colorHighlightContextualA = "#00b4ff";
        itemTab_colorHighlightContextualB = "#0064c8";
//#END
    }
}
