//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of Sky kit.

    - GNU Lesser General Public License Usage:
    This file may be used under the terms of the GNU Lesser General Public License version 3 as
    published by the Free Software Foundation and appearing in the LICENSE.md file included in the
    packaging of this file. Please review the following information to ensure the GNU Lesser
    General Public License requirements will be met: https://www.gnu.org/licenses/lgpl.html.
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

//#BaseButton
    //---------------------------------------------------------------------------------------------
    // Button

    property ImageColorFilter button_filterIconA: icon1_filter
    property ImageColorFilter button_filterIconB: icon2_filter

//#BaseButtonTouch
    //---------------------------------------------------------------------------------------------
    // ButtonTouch

    property int buttonTouch_size: dp48

    property int buttonTouch_radius: radius

    property int buttonTouch_padding: buttonTouch_size

    property real buttonTouch_opacityA: 0.8
    property real buttonTouch_opacityB: 1.0
    property real buttonTouch_opacityC: 0.4

    property color buttonTouch_color         : "#dcdcdc"
    property color buttonTouch_colorHighlight: color_highlight

//#BaseLabel
    //---------------------------------------------------------------------------------------------
    // Label

    property int label_size: buttonTouch_size

    property int label_padding: buttonTouch_padding

    property real label_opacity: buttonTouch_opacityA

    property color label_color: "#f0f0f0"

    //---------------------------------------------------------------------------------------------
    // Icons

//#ButtonsWindow
    property url icon_close   : "icons/times.svg"
    property url icon_maximize: "icons/window-maximize.svg"
    property url icon_restore : "icons/window-restore.svg"
    property url icon_minimize: "icons/window-minimize.svg"
//#END

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------
    // Text

    text_pixelSize: dp20

    //---------------------------------------------------------------------------------------------
    // Functions
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
