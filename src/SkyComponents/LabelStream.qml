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

Item
{
    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    /* mandatory */ property variant slider

    property int padding: st.labelStream_padding

    property int paddingLeft : padding
    property int paddingRight: padding

    /* read */ property int position: pGetPosition()

    //---------------------------------------------------------------------------------------------
    // Style

    property color colorA: st.labelStream_colorA
    property color colorB: st.labelStream_colorB

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    default property alias content: background.data

    property alias borderSize : borders.size
    property alias borderColor: borders.color

    property alias borderLeft  : borders.borderLeft
    property alias borderRight : borders.borderRight
    property alias borderTop   : borders.borderTop
    property alias borderBottom: borders.borderBottom

    property alias borderSizeWidth : borders.sizeWidth
    property alias borderSizeHeight: borders.sizeHeight

    /* read */ property alias text: itemText.text

    property alias font: itemText.font

    //---------------------------------------------------------------------------------------------

    property alias background: background
    property alias borders   : borders

    property alias itemText: itemText

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    width: paddingLeft + paddingRight + borderSizeWidth + sk.textWidth(font, text)

    height: st.labelStream_height + borderSizeHeight

    x: pGetX()

    visible: (slider.enabled && slider.isHovered)

    opacity: (visible)

    //---------------------------------------------------------------------------------------------
    // Functions private
    //---------------------------------------------------------------------------------------------

    function pGetX()
    {
        var area = slider.slider;

        var x = position - Math.round(width / 2);

        if (x > 0)
        {
            var maximum = area.width - width;

            if (x > maximum)
            {
                 return slider.x + area.x + maximum;
            }
            else return slider.x + area.x + x;
        }
        else return slider.x + area.x;
    }

    //---------------------------------------------------------------------------------------------

    function pGetPosition()
    {
        if (visible)
        {
             return window.mapToItem(slider.slider, window.mouseX, 0).x;
        }
        else return -1;
    }

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    Rectangle
    {
        id: background

        anchors.fill: parent

        anchors.leftMargin  : borderLeft
        anchors.rightMargin : borderRight
        anchors.topMargin   : borderTop
        anchors.bottomMargin: borderBottom

        gradient: Gradient
        {
            GradientStop { position: 0.0; color: colorA }
            GradientStop { position: 1.0; color: colorB }
        }

        TextBase
        {
            id: itemText

            anchors.fill: parent

            anchors.leftMargin : paddingLeft
            anchors.rightMargin: paddingRight

            horizontalAlignment: Text.AlignHCenter
            verticalAlignment  : Text.AlignVCenter

            text:
            {
                var model = slider.model;

                var minimum = model.handleMinimum;
                var maximum = model.handleMaximum;

                var pos = position - Math.round(slider.handle.width / 2);

                if      (pos < minimum) pos = minimum;
                else if (pos > maximum) pos = maximum;

                return controllerPlaylist.getPlayerTime(model.positionToValue(pos), 7);
            }

            style: st.text_raised

            font.pixelSize: st.dp11
        }
    }

    RectangleBorders
    {
        id: borders

        anchors.fill: parent
    }
}
