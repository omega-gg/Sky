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

//#BaseButton
    //---------------------------------------------------------------------------------------------
    // Button

    property ImageColorFilter button_filterIconA: icon1_filter
    property ImageColorFilter button_filterIconB: icon2_filter

//#BaseButtonTouch
    //---------------------------------------------------------------------------------------------
    // ButtonTouch

    property int buttonTouch_size: dp48

    property int buttonTouch_radius: dp8

    property int buttonTouch_padding: dp48

    property int buttonTouch_borderSize: dp2

    property real buttonTouch_opacityA: 0.8
    property real buttonTouch_opacityB: 1.0
    property real buttonTouch_opacityC: 0.6

    property color buttonTouch_color         : "#161616"
    property color buttonTouch_colorHighlight: color_highlight

    property color buttonTouch_colorBorder: "#969696"
//#END

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

    text1_color: "#dcdcdc"

    //---------------------------------------------------------------------------------------------
    // Icon

    icon1_colorA: text1_color
}
