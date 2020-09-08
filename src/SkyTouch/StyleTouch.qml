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
    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------
    // Global

    property int margins: dp2
    property int radius : dp4

    property bool isTight: (window.contentWidth < st.dp480)

//#Panel
    //---------------------------------------------------------------------------------------------
    // Panel

    property real panel_opacity: 0.8

    property color panel_color: "#c8c8c8"
//#END

//#BaseButton
    //---------------------------------------------------------------------------------------------
    // Button

    property ImageFilterColor button_filterIconA: icon1_filter
    property ImageFilterColor button_filterIconB: icon2_filter
//#END

//#BaseButtonTouch
    //---------------------------------------------------------------------------------------------
    // ButtonTouch

    property int buttonTouch_size: dp48

    property int buttonTouch_padding: dp16
    property int buttonTouch_spacing: dp8

    property real buttonTouch_opacity     : 0.8
    property real buttonTouch_opacityHover: 1.0
    property real buttonTouch_opacityPress: 0.6

    property color buttonTouch_color         : "#dcdcdc"
    property color buttonTouch_colorHighlight: color_highlight
//#END

//#BaseLabel
    //---------------------------------------------------------------------------------------------
    // Label

    property int label_size: buttonTouch_size

    property int label_padding: buttonTouch_padding

    property real label_opacity: buttonTouch_opacity

    property color label_color: buttonTouch_color
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

//#LineEdit
    //---------------------------------------------------------------------------------------------
    // LineEdit

    property int lineEdit_size: buttonTouch_size

    property int lineEdit_padding: buttonTouch_padding

    property real lineEdit_opacity     : buttonTouch_opacity
    property real lineEdit_opacityHover: buttonTouch_opacityHover

    property color lineEdit_color: "#f0f0f0"
//#END

//#ScrollBar
    //---------------------------------------------------------------------------------------------
    // ScrollBar

    property int scrollBar_size: st.dp24

    property int scrollBar_margins: margins

    property real scrollBar_opacity     : buttonTouch_opacity
    property real scrollBar_opacityHover: buttonTouch_opacityHover

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

    //---------------------------------------------------------------------------------------------
    // Icons

//#icons_window:ButtonsWindow
    property url icon_close   : "icons/times.svg"
    property url icon_maximize: "icons/window-maximize.svg"
    property url icon_restore : "icons/window-restore.svg"
    property url icon_minimize: "icons/window-minimize.svg"
//#END

//#icons_loading:LabelLoading
    property url icon_loading: "icons/loading.svg"
//#END

//#icons_playback:PlayerBrowser
    property url icon_backward: "icons/backward.svg"
    property url icon_forward : "icons/forward.svg"
    property url icon_play    : "icons/play.svg"
//#END

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------
    // Text

    text_pixelSize: dp18

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function grid(count)
    {
        if (count < 2)
        {
             return buttonTouch_size;
        }
        else return buttonTouch_size * count + st.margins * (count - 1);
    }

    //---------------------------------------------------------------------------------------------

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

//#BaseButtonTouch
        //-----------------------------------------------------------------------------------------
        // ButtonTouch

        buttonTouch_color = "#dcdcdc";
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

        window_color = "#646464";

//#BaseButtonTouch
        //-----------------------------------------------------------------------------------------
        // ButtonTouch

        buttonTouch_color = "#161616";
//#END
    }
}
