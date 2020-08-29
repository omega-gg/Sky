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

BaseButtonTouch
{
    id: buttonTouchFull

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property bool enableFilter: true

    property int margins: 0

    property int padding: st.buttonTouch_padding
    property int spacing: st.buttonTouch_spacing

    property int minimumWidth: -1
    property int maximumWidth: -1

    //---------------------------------------------------------------------------------------------
    // Private

    property int pIconWidth: Math.max(itemIcon.width, background.height)

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias isSourceDefault: itemIcon.isSourceDefault

    property alias icon       : itemIcon.source
    property alias iconDefault: itemIcon.sourceDefault

    property alias iconWidth : itemIcon.width
    property alias iconHeight: itemIcon.height

    property alias iconSourceSize: itemIcon.sourceSize
    property alias iconSourceArea: itemIcon.sourceArea

    property alias iconLoadMode: itemIcon.loadMode
    property alias iconFillMode: itemIcon.fillMode

    property alias iconAsynchronous: itemIcon.asynchronous
    property alias iconCache       : itemIcon.cache

    property alias iconScaling: itemIcon.scaling

    property alias text: itemText.text

    property alias font: itemText.font

    //---------------------------------------------------------------------------------------------

    property alias itemIcon: itemIcon
    property alias itemText: itemText

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    width: getWidth()

    height: st.buttonTouch_size

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function getWidth()
    {
        var size;

        if (pIconWidth)
        {
            if (text)
            {
                size = Math.max(pIconWidth, height) + sk.textWidth(font, text) + padding + spacing;
            }
            else size = Math.max(pIconWidth, height);
        }
        else if (text)
        {
            size = sk.textWidth(font, text) + padding * 2;
        }

        if (minimumWidth != -1)
        {
            size = Math.max(minimumWidth, size);
        }

        if (maximumWidth != -1)
        {
            size = Math.min(size, maximumWidth);
        }

        return size;
    }

    //---------------------------------------------------------------------------------------------

    function getSourceHeight()
    {
        return Math.round(height - margins * 2)
    }

    function getFilterDefault()
    {
        if (isHighlighted || checked)
        {
             return st.button_filterIconB;
        }
        else return st.button_filterIconA;
    }

    //---------------------------------------------------------------------------------------------
    // Virtual

    /* virtual */ function getFilter()
    {
        return getFilterDefault();
    }

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    ImageScale
    {
        id: itemIcon

        anchors.left: parent.left

        anchors.leftMargin: Math.round((pIconWidth - width) / 2)

        anchors.verticalCenter: parent.verticalCenter

        sourceSize.height: getSourceHeight()

        clip: (fillMode == Image.PreserveAspectCrop)

        opacity: (buttonTouchFull.enabled) ? 1.0 : st.icon_opacityDisable

        filter: (enableFilter) ? getFilter() : null
    }

    TextBase
    {
        id: itemText

        anchors.fill: parent

        anchors.leftMargin: (pIconWidth) ? pIconWidth + spacing
                                         : buttonTouchFull.padding

        anchors.rightMargin: buttonTouchFull.padding

        verticalAlignment: Text.AlignVCenter

        opacity: (buttonTouchFull.enabled) ? 1.0 : st.icon_opacityDisable

        color: (isHighlighted || checked) ? st.text2_color
                                          : st.text1_color
    }
}
