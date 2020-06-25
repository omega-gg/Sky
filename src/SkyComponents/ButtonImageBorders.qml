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

ButtonImage
{
    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property int imageDefaultWidth : 0
    property int imageDefaultHeight: 0

    property int imageMaximumWidth : -1
    property int imageMaximumHeight: -1

    //---------------------------------------------------------------------------------------------
    // Style

    property color colorBorder     : st.buttonImageBorders_colorBorder
    property color colorBorderHover: st.buttonImageBorders_colorBorderHover

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias borderSize : borders.size
    property alias borderColor: borders.color

    property alias borderSizeWidth : borders.sizeWidth
    property alias borderSizeHeight: borders.sizeHeight

    //---------------------------------------------------------------------------------------------

    property alias background: background
    property alias borders   : borders

    //---------------------------------------------------------------------------------------------
    // Style

    property alias colorBackground: background.color

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    width : pGetWidth ()
    height: pGetHeight()

    itemImage.anchors.centerIn: itemImage.parent

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    itemImage.onLoaded:
    {
        if (imageMaximumWidth != -1)
        {
            itemImage.width = Math.min(itemImage.implicitWidth, imageMaximumWidth);
        }

        if (imageMaximumHeight != -1)
        {
            itemImage.height = Math.min(itemImage.implicitHeight, imageMaximumHeight);
        }
    }

    //---------------------------------------------------------------------------------------------
    // Functions private
    //---------------------------------------------------------------------------------------------

    function pGetWidth()
    {
        if (itemImage.isReady)
        {
             return itemImage.width + borderSizeWidth;
        }
        else return imageDefaultWidth + borderSizeWidth;
    }

    function pGetHeight()
    {
        if (itemImage.isReady)
        {
             return itemImage.height + borderSizeHeight;
        }
        else return imageDefaultHeight + borderSizeHeight;
    }

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    Rectangle
    {
        id: background

        anchors.fill: parent

        z: -1

        color: st.buttonImageBorders_colorBackground
    }

    RectangleBorders
    {
        id: borders

        anchors.fill: parent

        color: (isHovered && isPressed == false) ? colorBorderHover
                                                 : colorBorder
    }
}
