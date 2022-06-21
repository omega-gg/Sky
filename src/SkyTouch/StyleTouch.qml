//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkyTouch.

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
    id: styleTouch

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------
    // Global

    property int margins: dp1

    property bool isTight: (window.contentWidth < dp480)

//#Panel
    //---------------------------------------------------------------------------------------------
    // Panel

    property int panel_durationAnimation: duration_fast

    property real panel_opacity: 0.8

    property color panel_color: "#c8c8c8"
//#END

//#PanelContextual
    //---------------------------------------------------------------------------------------------
    // PanelContextual

    property int panelContextual_preferredWidth: dp256

    property int panelContextual_margins: dp4

    property color panelContextual_color: border_color
//#END

//#BaseButton
    //---------------------------------------------------------------------------------------------
    // Button

    property ImageFilterColor button_filterIconA: icon1_filter
    property ImageFilterColor button_filterIconB: icon2_filter
//#END

//#BaseButtonTouch:ButtonMask
    //---------------------------------------------------------------------------------------------
    // ButtonTouch

    property int buttonTouch_size: dp48

    property int buttonTouch_padding: (isTight) ? dp12
                                                : dp16

    property int buttonTouch_spacing: dp8

    property real buttonTouch_opacity     : 0.8
    property real buttonTouch_opacityHover: 1.0
    property real buttonTouch_opacityPress: 0.6

    property color buttonTouch_color         : "#dcdcdc"
    property color buttonTouch_colorHighlight: color_highlight
//#END

//#ButtonTouchTrack
    //---------------------------------------------------------------------------------------------
    // ButtonTouchTrack

    property int buttonTouchTrack_size: dp14

    property color buttonTouchTrack_color      : "red"
    property color buttonTouchTrack_colorBorder: "#161616"
//#END

//#ButtonTouchTiny
    //---------------------------------------------------------------------------------------------
    // ButtonTouchTiny

    property int buttonTouchTiny_size: dp32
//#END

//#ButtonWide*
    //---------------------------------------------------------------------------------------------
    // ButtonWide

    property int buttonWide_margins: dp12
//#END

//#ButtonMask
    //---------------------------------------------------------------------------------------------
    // ButtonMask

    property int buttonMask_margins: dp10

    property real buttonMask_opacity     : buttonTouch_opacityHover
    property real buttonMask_opacityHover: buttonTouch_opacity
    property real buttonMask_opacityPress: buttonTouch_opacityPress
//#END

//#ButtonsWindow
    //---------------------------------------------------------------------------------------------
    // ButtonsWindow

    property int buttonsWindow_width : dp40
    property int buttonsWindow_height: dp26
//#END

//#ButtonSlide
    //---------------------------------------------------------------------------------------------
    // ButtonSlide

    property int buttonSlide_margins: dp4

    property real buttonSlide_opacityPress: 0.3

    property color buttonSlide_colorHandle     : buttonTouch_colorHighlight
    property color buttonSlide_colorHandlePress: icon1_colorA
//#END

//#BaseLabel
    //---------------------------------------------------------------------------------------------
    // Label

    property int label_size: buttonTouch_size

    property int label_padding: buttonTouch_padding

    property real label_opacity: buttonTouch_opacity

    property color label_color: "#f0f0f0"
//#END

//#LabelTiny
    //---------------------------------------------------------------------------------------------
    // LabelTiny

    property int labelTiny_size: buttonTouchTiny_size
//#END

//#LabelIcon
    //---------------------------------------------------------------------------------------------
    // LabelIcon

    property ImageFilterColor label_filterIcon: icon1_filter
//#END

//#LabelLoading
    //---------------------------------------------------------------------------------------------
    // LabelLoading

    property int labelLoading_durationAnimation: ms800
//#END

//#LabelStream
    //---------------------------------------------------------------------------------------------
    // LabelStream

    property int labelStream_padding: dp8
//#END

//#Popup
    //---------------------------------------------------------------------------------------------
    // Popup

    property int popup_interval: 5000
//#END

//#LineEdit
    //---------------------------------------------------------------------------------------------
    // LineEdit

    property int lineEdit_size: buttonTouch_size

    property int lineEdit_padding: buttonTouch_padding

    property real lineEdit_opacity     : buttonTouch_opacity
    property real lineEdit_opacityHover: buttonTouch_opacityHover

    property color lineEdit_color: label_color
//#END

//#ListLoading
    //---------------------------------------------------------------------------------------------
    // ListLoading

    property int listLoading_duration: duration_slower

    property real listLoading_opacityA: 0.2
    property real listLoading_opacityB: 1.0

    property int listLoading_minimumCount: 1

    property color listLoading_color: lineEdit_color
//#END

//#ListPlaylist
    //---------------------------------------------------------------------------------------------
    // ListPlaylist

    property int listPlaylist_intervalLoad  :   200
    property int listPlaylist_intervalReload: 60000 // 1 minute
//#END

//#ScrollBar
    //---------------------------------------------------------------------------------------------
    // ScrollBar

    property int scrollBar_size: (isTight) ? dp12
                                           : dp20

    property int scrollBar_margins: margins

    property real scrollBar_opacity     : 0.8
    property real scrollBar_opacityHover: 0.9
    property real scrollBar_opacityPress: 1.0

    property color scrollBar_color: icon1_colorA
//#END

//#Slider
    //---------------------------------------------------------------------------------------------
    // Slider

    property int slider_size: buttonTouch_size

    property int slider_margins      : dp8
    property int slider_marginsHandle: dp4

    property real slider_opacity     : scrollBar_opacity
    property real slider_opacityHover: scrollBar_opacityHover

    property color slider_color     : label_color
    property color slider_colorFront: icon1_colorA
//#END

//#SliderStream
    //---------------------------------------------------------------------------------------------
    // SliderStream

    property int sliderStream_durationAnimation: duration_slower

    property int sliderStream_intervalProgress: ms1000

    property real sliderStream_opacityProgressA: 0.5
    property real sliderStream_opacityProgressB: 1.0

    property color sliderStream_colorActive: color_highlight
//#END

//#PlayerBrowser
    //---------------------------------------------------------------------------------------------
    // PlayerBrowser

    property int playerBrowser_durationAnimation: panel_durationAnimation
//#END

//#ComponentCompletion
    //---------------------------------------------------------------------------------------------
    // ComponentCompletion

    property int componentCompletion_margins: dp12
//#END

//#ComponentBackend*
    //---------------------------------------------------------------------------------------------
    // ComponentBackend

    property int componentBackend_iconWidth: dp24
//#END

//#ComponentTrack
    //---------------------------------------------------------------------------------------------
    // ComponentTrack

    property int componentTrack_iconWidth: dp24
//#END

//#ComponentTrackHistory
    //---------------------------------------------------------------------------------------------
    // ComponentTrackHistory

    property real componentTrackHistory_opacity: 0.2

    property color componentTrackHistory_color: slider_colorFront
//#END

    //---------------------------------------------------------------------------------------------
    // Filters

//#filter_round:ComponentBackend:ComponentTrack
    property int filterRound_margins: dp4

    property alias icon_filterRound: icon_filterRound

    ImageFilterMask
    {
        id: icon_filterRound

        width : buttonTouch_size - filterRound_margins * 2
        height: width

        radius: styleTouch.radius
    }
//#END

    //---------------------------------------------------------------------------------------------
    // Icons

//#icons_window:ButtonsWindow:LineEditClear
    property url icon_close: "icons/times.svg"
//#END

//#icons_window:ButtonsWindow
    property url icon_maximize: "icons/window-maximize.svg"
    property url icon_restore : "icons/window-restore.svg"
    property url icon_minimize: "icons/window-minimize.svg"
//#END

//#icons_direction:ButtonWideAction
    property url icon_down: "icons/chevron-down.svg"
//#END

//#icons_direction:ButtonWideAction:ComponentCompletion
    property url icon_right: "icons/chevron-right.svg"
//#END

//#icons_loading:LabelLoading
    property url icon_loading: "icons/loading.svg"
//#END

//#icons_playback:PlayerBrowser
    property url icon_backward: "icons/backward.svg"
    property url icon_forward : "icons/forward.svg"
    property url icon_play    : "icons/play.svg"
//#END

//#icons_feed:ComponentBackend:ComponentTrack
    property url icon_feed: "icons/rss.svg"
//#END

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------
    // Global

    radius: dp6

    //---------------------------------------------------------------------------------------------
    // Text

    text_pixelSize: dp18

    // NOTE: The third text color is the same than the first.
    text3_color: text_color

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function grid(count)
    {
        if (count < 2)
        {
             return buttonTouch_size;
        }
        else return buttonTouch_size * count + margins * (count - 1);
    }

    //---------------------------------------------------------------------------------------------

    function getButtonFilter(button)
    {
        if (button.isHighlighted || button.checked)
        {
             return button_filterIconB;
        }
        else return button_filterIconA;
    }

    //---------------------------------------------------------------------------------------------

    function getTextColor(highlighed, checked, active)
    {
        if (highlighed || checked)
        {
            return text2_color;
        }
        else if (active)
        {
            return text_colorCurrent;
        }
        else return text1_color;
    }

    //---------------------------------------------------------------------------------------------

    function applyStyle(index)
    {
        if (index == 0) // Light
        {
            margins = dp1;

            applyLight();
        }
        else if (index == 1) // Night
        {
            margins = dp1;

            applyNight();
        }
        else if (index == 2) // Light bold
        {
            margins = dp2;

            applyLight();
        }
        else // Night bold
        {
            margins = dp2;

            applyNight();
        }
    }

    function applyLight()
    {
        //-----------------------------------------------------------------------------------------
        // Border

        border_color = "#969696";

        //-----------------------------------------------------------------------------------------
        // Text

        text1_color = "#484848";

        //-----------------------------------------------------------------------------------------
        // Icon

        icon1_colorA = "#646464";

        //-----------------------------------------------------------------------------------------
        // Window

        window_color = "#b4b4b4";

//#Panel
        //---------------------------------------------------------------------------------------------
        // Panel

        panel_color = "#c8c8c8";
//#END

//#BaseButtonTouch:ButtonMask
        //-----------------------------------------------------------------------------------------
        // ButtonTouch

        buttonTouch_color = "#dcdcdc";
//#END

//#BaseLabel
        //---------------------------------------------------------------------------------------------
        // Label

        label_color = "#f0f0f0";
//#END
    }

    function applyNight()
    {
        //-----------------------------------------------------------------------------------------
        // Border

        border_color = "#161616";

        //-----------------------------------------------------------------------------------------
        // Text

        text1_color = "#dcdcdc";

        //-----------------------------------------------------------------------------------------
        // Icon

        icon1_colorA = text1_color;

        //-----------------------------------------------------------------------------------------
        // Window

        window_color = "#404040";

//#Panel
        //---------------------------------------------------------------------------------------------
        // Panel

        panel_color = "#484848";
//#END

//#BaseButtonTouch:ButtonMask
        //-----------------------------------------------------------------------------------------
        // ButtonTouch

        buttonTouch_color = "#323232";
//#END

//#BaseLabel
        //---------------------------------------------------------------------------------------------
        // Label

        label_color = "#242424";
//#END
    }
}
