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

BaseToolTip
{
    id: toolTip

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property int padding: st.toolTip_padding

    property int paddingLeft : padding
    property int paddingRight: padding

    property int spacing: st.toolTip_spacing

    //---------------------------------------------------------------------------------------------
    // Private

    property int pIconWidth: Math.max(iconWidth, itemContent.height)

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

    property alias itemText: itemText
    property alias itemIcon: itemIcon

    //---------------------------------------------------------------------------------------------
    // Style

    property alias enableFilter: itemIcon.enableFilter

    property alias filterIcon      : itemIcon.filterDefault
    property alias filterIconShadow: itemIcon.filterShadow
    property alias filterIconSunken: itemIcon.filterSunken

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    maximumWidth: st.toolTip_maximumWidth

    preferredWidth:
    {
        var size;

        if (iconWidth)
        {
            if (text)
            {
                size = Math.max(iconWidth, height) + sk.textWidth(font, text) + borderSizeWidth
                       +
                       paddingRight + spacing;
            }
            else size = Math.max(iconWidth, height) + borderSizeWidth;
        }
        else if (text)
        {
            size = sk.textWidth(font, text) + borderSizeWidth + paddingLeft + paddingRight;
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
    // Functions
    //---------------------------------------------------------------------------------------------

    function show(text, iconDefault, width, height)
    {
        itemIcon.width  = width;
        itemIcon.height = height;

        iconDefaultSize = Qt.size(width, height);

        iconFillMode = Image.Stretch;

        toolTip.text = text;

        toolTip.icon        = "";
        toolTip.iconDefault = iconDefault;

        pActivate();
    }

    function showIcon(text, iconDefault, widthDefault, heightDefault, icon, width, height)
    {
        itemIcon.width  = width;
        itemIcon.height = height;

        iconDefaultSize = Qt.size(widthDefault, heightDefault);

        iconFillMode = Image.PreserveAspectCrop;

        toolTip.text = text;

        toolTip.icon        = icon;
        toolTip.iconDefault = iconDefault;

        pActivate();
    }

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

    Icon
    {
        id: itemIcon

        anchors.left: parent.left

        anchors.leftMargin: Math.round((pIconWidth - width) / 2)

        anchors.verticalCenter: parent.verticalCenter

        sourceSize: Qt.size(width, height)
        sourceArea: Qt.size(width, height)

        clip: (fillMode == Image.PreserveAspectCrop)

        enableFilter: isSourceDefault
    }

    TextBase
    {
        id: itemText

        anchors.fill: parent

        anchors.leftMargin: (iconWidth) ? pIconWidth + spacing
                                        : paddingLeft

        anchors.rightMargin: paddingRight

        verticalAlignment: Text.AlignVCenter

        style: st.text_raised
    }
}
