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

BaseButtonPush
{
    id: buttonPushFull

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property int padding: st.buttonPush_padding

    property int paddingLeft : padding
    property int paddingRight: padding

    property int spacing: st.buttonPush_spacing

    property int minimumWidth: -1
    property int maximumWidth: -1

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias isIconDefault: itemIcon.isSourceDefault

    property alias icon       : itemIcon.source
    property alias iconDefault: itemIcon.sourceDefault

    property alias iconWidth : itemIcon.width
    property alias iconHeight: itemIcon.height

    property alias iconSourceSize : itemIcon.sourceSize
    property alias iconDefaultSize: itemIcon.defaultSize

    property alias iconSourceArea: itemIcon.sourceArea

    property alias iconLoadMode: itemIcon.loadMode
    property alias iconFillMode: itemIcon.fillMode

    property alias iconAsynchronous: itemIcon.asynchronous
    property alias iconCache       : itemIcon.cache

    property alias iconScaling: itemIcon.scaling

    property alias iconStyle: itemIcon.style

    property alias text: itemText.text
    property alias font: itemText.font

    //---------------------------------------------------------------------------------------------

    property alias itemIcon: itemIcon
    property alias itemText: itemText

    //---------------------------------------------------------------------------------------------
    // Style

    property alias enableFilter: itemIcon.enableFilter

    property alias filterIcon      : itemIcon.filterDefault
    property alias filterIconShadow: itemIcon.filterShadow
    property alias filterIconSunken: itemIcon.filterSunken

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    width: getWidth()

    height: st.buttonPush_height

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function getWidth()
    {
        var size;

        if (iconWidth)
        {
            if (text)
            {
                size = iconWidth + sk.textWidth(font, text) + paddingLeft + paddingRight + spacing;
            }
            else size = itemIcon.width + paddingLeft + paddingRight;
        }
        else if (text)
        {
            size = sk.textWidth(font, text) + paddingLeft + paddingRight;
        }

        return st.getSize(size, minimumWidth, maximumWidth);
    }

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

    Icon
    {
        id: itemIcon

        anchors.left: parent.left

        anchors.leftMargin: paddingLeft

        anchors.verticalCenter: parent.verticalCenter

        sourceSize.height: (scaling) ? -1 : parent.height

        opacity: (buttonPushFull.enabled) ? 1.0 : st.icon_opacityDisable

        style: (checked) ? st.icon_raised
                         : st.icon_sunken

        filterDefault: (isHighlighted) ? st.button_filterIconB
                                       : st.button_filterIconA

        filterShadow : st.button_filterIconShadow
        filterSunken : st.button_filterIconSunken
    }

    TextBase
    {
        id: itemText

        anchors.left  : itemIcon.right
        anchors.right : parent.right
        anchors.top   : parent.top
        anchors.bottom: parent.bottom

        anchors.leftMargin: (iconWidth) ? spacing : 0

        anchors.rightMargin: paddingRight

        verticalAlignment: Text.AlignVCenter

        opacity: (buttonPushFull.enabled) ? 1.0 : st.text_opacityDisable

        color: (isHighlighted) ? st.button_colorTextB
                               : st.button_colorTextA

        style: (checked) ? st.text_raised
                         : st.text_sunken

        Behavior on opacity
        {
            PropertyAnimation
            {
                duration: st.duration_fast

                easing.type: st.easing
            }
        }
    }
}
