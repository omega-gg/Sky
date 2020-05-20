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

Item
{
    id: slideParagraph

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    /* read */ property int count: model.count

    //---------------------------------------------------------------------------------------------

    /* read */ property int textHeight: pGetTextHeight()

    property int marginWidth : ds(textHeight / 4)
    property int marginHeight: ds(textHeight / 8)

    property int horizontalAlignment: Text.AlignHCenter
    property int verticalAlignment  : Text.AlignVCenter

    property string text

    property color   color   : st.text_color
    property variant gradient: null

    property int style  : TextSvg.Glow
    property int outline: TextSvg.OutlineRound

    property color styleColor: (style == st.text_sunken) ? st.text_colorSunken
                                                         : st.text_colorShadow

    property int styleSize: sp.slideText_styleSize

    property string family   : st.text_fontFamily
    property int    pixelSize: sp.slideText_pixelSize
    property bool   bold     : true

    //---------------------------------------------------------------------------------------------
    // Style

    property int durationAnimation: sp.slideText_durationAnimation

    property int easing: Easing.Linear

    //---------------------------------------------------------------------------------------------
    // Private

    property bool pReady: false

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias model: repeater.model

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    width : pGetWidth ()
    height: pGetHeight()

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    Component.onCompleted:
    {
        pReady = true;

        pUpdateModel();
    }

    onTextChanged: if (pReady) pUpdateModel()

    //---------------------------------------------------------------------------------------------
    // Animations
    //---------------------------------------------------------------------------------------------

    Behavior on opacity
    {
        PropertyAnimation
        {
            duration: (st.animate) ? durationAnimation : 0

            easing.type: slideParagraph.easing
        }
    }

    //---------------------------------------------------------------------------------------------
    // Functions private
    //---------------------------------------------------------------------------------------------

    function pUpdateModel()
    {
        model.clear();

        var list = text.split("\n");

        for (var i = 0; i < list.length; i++)
        {
            model.append({ "line": list[i] });
        }
    }

    //---------------------------------------------------------------------------------------------

    function pItemAt(index)
    {
        return repeater.itemAt(index);
    }

    //---------------------------------------------------------------------------------------------

    function pGetWidth()
    {
        var width = 0;

        for (var i = 0; i < count; i++)
        {
            var size = pItemAt(i).implicitWidth;

            if (width < size)
            {
                width = size;
            }
        }

        return width;
    }

    function pGetHeight()
    {
        var height = marginHeight * 2;

        for (var i = 0; i < count; i++)
        {
            height += pItemAt(i).height;
        }

        return height;
    }

    //---------------------------------------------------------------------------------------------

    function pGetTextHeight()
    {
        if (count)
        {
             return pItemAt(0).textHeight;
        }
        else return 0;
    }

    //---------------------------------------------------------------------------------------------

    function pGetY(index)
    {
        if (index < 1)
        {
            return marginHeight;
        }

        var item = pItemAt(index - 1);

        return Math.round(item.y + item.height);
    }

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    Repeater
    {
        id: repeater

        anchors.fill: parent

        model: ListModel {}

        delegate: SlideText
        {
            width: repeater.width

            marginWidth : slideParagraph.marginWidth
            marginHeight: 0

            horizontalAlignment: slideParagraph.horizontalAlignment
            verticalAlignment  : slideParagraph.verticalAlignment

            y: pGetY(index)

            text: line

            color: slideParagraph.color

            gradient: (slideParagraph.gradient) ? slideParagraph.gradient : null

            style  : slideParagraph.style
            outline: slideParagraph.outline

            styleColor: slideParagraph.styleColor
            styleSize : slideParagraph.styleSize

            font.family   : slideParagraph.family
            font.pixelSize: slideParagraph.pixelSize
            font.bold     : slideParagraph.bold
        }
    }
}
