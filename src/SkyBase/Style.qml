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

    property int radius: 0

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

    property real text_opacityDisable: 0.6

    property string text_fontFamily: "Arial"

    property int text_pixelSize: dp12

    property bool text_bold: false

    property int text_raised: Text.Normal
    property int text_sunken: text_raised

//#QT_NEW
    property int text_renderType: Text.QtRendering
//#END

    property color text_color         : text1_color
    property color text_colorCurrent  : color_highlight
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
    property color border_colorFocus: color_highlight

    property alias border_filter     : border_filter
    property alias border_filterFocus: border_filterFocus

    ImageFilterColor
    {
        id: border_filter

        color: border_color
    }

    ImageFilterColor
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
    property color icon_colorActiveA: color_highlight
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

    ImageFilterColor
    {
        id: icon_filter

        gradient: ScaleGradient
        {
            ScaleGradientStop { position: 0.0; color: icon_colorA }
            ScaleGradientStop { position: 1.0; color: icon_colorB }
        }
    }

    ImageFilterColor
    {
        id: icon_filterActive

        gradient: ScaleGradient
        {
            ScaleGradientStop { position: 0.0; color: icon_colorActiveA }
            ScaleGradientStop { position: 1.0; color: icon_colorActiveB }
        }
    }

    ImageFilterColor
    {
        id: icon_filterShadow

        color: icon_colorShadow
    }

    ImageFilterColor
    {
        id: icon_filterSunken

        color: icon_colorSunken
    }

    //---------------------------------------------------------------------------------------------

    ImageFilterColor
    {
        id: icon2_filter

        gradient: ScaleGradient
        {
            ScaleGradientStop { position: 0.0; color: icon2_colorA }
            ScaleGradientStop { position: 1.0; color: icon2_colorB }
        }
    }

    ImageFilterColor
    {
        id: icon2_filterShadow

        color: icon2_colorShadow
    }

    ImageFilterColor
    {
        id: icon2_filterSunken

        color: icon2_colorSunken
    }

//#WindowSky
    //---------------------------------------------------------------------------------------------
    // Window

    property int window_durationAnimation: duration_fast

    property int window_intervalTouch: 3000

    property int window_borderSize     : border_size
    property int window_borderSizeTouch: dp8

    property int window_resizerSize     : dp4
    property int window_resizerSizeTouch: dp16

    property color window_color: "#b4b4b4"
//#END

//#ItemScan
    //---------------------------------------------------------------------------------------------
    // ItemScan

//#DESKTOP
    property real itemScan_intervalA:  16
    property real itemScan_intervalB: 500
//#END

    property real itemScan_ratioTouch: 10.0
//#END

//#RectangleBorders
    //---------------------------------------------------------------------------------------------
    // RectangleBorders

    property int rectangleBorders_size: border_size
//#END

//#RectangleShadow
    //---------------------------------------------------------------------------------------------
    // RectangleShadow

    property url rectangleShadow_sourceLeft : "pictures/shadowLeft.png"
    property url rectangleShadow_sourceRight: "pictures/shadowRight.png"
    property url rectangleShadow_sourceUp   : "pictures/shadowUp.png"
    property url rectangleShadow_sourceDown : "pictures/shadowDown.png"

    property color rectangleShadow_color: "#161616"

    property ImageFilterColor rectangleShadow_filter: rectangleShadow_filter

    ImageFilterColor
    {
        id: rectangleShadow_filter

        color: rectangleShadow_color
    }
//#END

//#RectangleShadowClick
    //---------------------------------------------------------------------------------------------
    // RectangleShadowClick

    property real rectangleShadowClick_ratio: 4.0
//#END

//#RectangleTag
    //---------------------------------------------------------------------------------------------
    // RectangleTag

    property real rectangleTag_opacity: 0.4
//#END

//#ImageBarcode:ImageTag
    //---------------------------------------------------------------------------------------------
    // ImageBarcode

    property real barcode_ratio: 0.890625
//#END

//#ImageBarcode
    //---------------------------------------------------------------------------------------------
    // ImageBarcode

    property color imageBarcode_color: "white"
//#END

//#Cursor*
    //---------------------------------------------------------------------------------------------
    // Cursor

    property int cursor_size: dp48
//#END

//#CursorTouch
    //---------------------------------------------------------------------------------------------
    // CursorTouch

    property int cursorTouch_durationOpacity: duration_normal

    property real cursorTouch_opacityA: 0.6
    property real cursorTouch_opacityB: 0.8

    property color cursorTouch_color      : "#161616"
    property color cursorTouch_colorBorder: "#c8c8c8"
//#END

//#TextSubtitle
    //---------------------------------------------------------------------------------------------
    // TextSubtitle

    property int textSubtitle_sizeMinimum: dp18
    property int textSubtitle_sizeMaximum: dp48

    property bool textSubtitle_bold: true

    property color textSubtitle_color     : "#dcdcdc"
    property color textSubtitle_colorStyle: "#242424"
//#END

//#BaseLineEdit
    //---------------------------------------------------------------------------------------------
    // BaseLineEdit

    property int baseLineEdit_durationCursor: ms500

    property int baseLineEdit_height: dp32

    property int baseLineEdit_cursorWidth: dp2

    property int baseLineEdit_maximumLength: 1000

    property bool baseLineEdit_autoSelect: true

    property color baseLineEdit_colorCursor: baseLineEdit_colorText

    property color baseLineEdit_colorText         : text1_color
    property color baseLineEdit_colorTextSelection: border_colorFocus
    property color baseLineEdit_colorTextSelected : text2_color

    property color baseLineEdit_colorDefault: text3_color
//#END

//#BaseTextEdit
    //---------------------------------------------------------------------------------------------
    // BaseTextEdit

    property int baseTextEdit_margin: dp8
//#END

//#BaseConsole
    //---------------------------------------------------------------------------------------------
    // BaseConsole

    property int baseConsole_maximumLength: 4000

    property string baseConsole_fontFamily: "consolas"

    property int baseConsole_pixelSize: dp14

    property bool baseConsole_bold: false

    property color baseConsole_color: text_color
//#END

//#FlickablePages
    //---------------------------------------------------------------------------------------------
    // FlickablePages

    property int flickablePages_velocitySlide: dp400
//#END

//#BaseScrollBar
    //---------------------------------------------------------------------------------------------
    // BaseScrollBar

    property int scrollBar_minimumSize: dp48
//#END

//#BasePlayerBrowser
    //---------------------------------------------------------------------------------------------
    // BasePlayerBrowser

    property int playerBrowser_intervalA: 500
    property int playerBrowser_intervalB: 200

    property real playerBrowser_ratio: 5.0
//#END

//#AnimatedSlide
    //---------------------------------------------------------------------------------------------
    // AnimatedSlide

    property int animatedSlide_duration: ms(10000)
//#END

    //---------------------------------------------------------------------------------------------
    // Images

//#pictures_tag:ImageTag
    property url picture_tag: "pictures/tag.svg"
//#END

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

    function ms(value)
    {
        return value * ratioSpeed;
    }

    //---------------------------------------------------------------------------------------------

    function checkTiny(size, maximumSize)
    {
        return (maximumSize == -1 || size < maximumSize)
    }

    //---------------------------------------------------------------------------------------------

    function animateShow(item, visible, animation, animate)
    {
        if (visible)
        {
            if (animate)
            {
                item.visible = true;

                animation.enabled = true;

                item.opacity = 1.0;

                animation.enabled = false;
            }
            else
            {
                item.opacity = 1.0;
                item.visible = true;
            }
        }
        else
        {
            item.visible = false;

            if (animate)
            {
                animation.enabled = true;

                item.opacity = 0.0;

                animation.enabled = false;
            }
            else item.opacity = 0.0;
        }
    }

    //---------------------------------------------------------------------------------------------

    function getSize(size, minimum, maximum)
    {
        if (minimum != -1)
        {
            size = Math.max(minimum, size);
        }

        if (maximum != -1)
        {
            size = Math.min(size, maximum);
        }

        return size;
    }

    function getSizeMargins(size, margins)
    {
        size = Math.round(size - margins * 2);

        if (size < 1) return 0;
        else          return size;
    }

    //---------------------------------------------------------------------------------------------

    function getHeightColumn(column)
    {
        var y = -1;

        var height;

        for (var i = 0; i < column.children.length; i++)
        {
            var child = column.children[i];

            // NOTE: We check against opacity instead of visible because we want the proper height
            //       even when the parent is hidden.
            if (child.opacity && child.y > y)
            {
                y = child.y;

                height = child.height;
            }
        }

        if (y == -1)
        {
            return 0;
        }
        else return y + height;
    }

    //---------------------------------------------------------------------------------------------

    function getSliderX(slider, width, position)
    {
        var x = position - Math.round(width / 2);

        if (x > 0)
        {
            var maximum = slider.width - width;

            if (x > maximum)
            {
                 return slider.x + maximum;
            }
            else return slider.x + x;
        }
        else return slider.x;
    }

    function getSliderValue(slider, position)
    {
        var model = slider.model;

        var minimum = model.handleMinimum;
        var maximum = model.handleMaximum;

        var pos = position - Math.round(slider.handle.width / 2);

        if      (pos < minimum) pos = minimum;
        else if (pos > maximum) pos = maximum;

        return model.positionToValue(pos);
    }

    //---------------------------------------------------------------------------------------------

    function getTabTitle(title, state, source)
    {
        if (title)
        {
            return title;
        }
        else if (state == LocalObject.Loading)
        {
            return qsTr("Loading...");
        }
        else if (source != "")
        {
            return getUrlTitle(source, qsTr("Track"));
        }
        else return qsTr("New Tab");
    }

    function getTrackTitle(title, state, source)
    {
        if (title)
        {
            return title;
        }
        else if (state == LocalObject.Loading)
        {
            return qsTr("Loading Track...");
        }
        else if (source != "")
        {
            return getUrlTitle(source, qsTr("Track"));
        }
        else return qsTr("Invalid Track");
    }

    function getUrlTitle(source, text)
    {
        var title = controllerNetwork.urlTitle(source);

        if (title)
        {
             return title + ' ' + text;
        }
        else return text;
    }
}
