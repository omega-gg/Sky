//=================================================================================================
/*
    Copyright (C) 2015-2016 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkyComponents module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

import QtQuick 1.1
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

    property int pIconWidth: Math.max(itemIcon.width, itemContent.height)

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias isIconDefault: itemIcon.isSourceDefault

    property alias icon       : itemIcon.source
    property alias iconDefault: itemIcon.sourceDefault

    property alias iconWidth : itemIcon.iconWidth
    property alias iconHeight: itemIcon.iconHeight

    property alias iconSourceSize: itemIcon.sourceSize
    property alias iconSourceArea: itemIcon.sourceArea

    property alias iconLoadMode: itemIcon.loadMode
    property alias iconFillMode: itemIcon.fillMode

    property alias iconAsynchronous: itemIcon.asynchronous
    property alias iconCache       : itemIcon.cache

    property alias iconScaling: itemIcon.scaling

    property alias iconStyle: itemIcon.iconStyle

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

    function show(text, iconDefault, iconWidth, iconHeight)
    {
        itemIcon.iconWidth  = iconWidth;
        itemIcon.iconHeight = iconHeight;

        itemIcon.sourceSize = Qt.size(iconWidth, iconHeight);

        iconFillMode = Image.Stretch;

        toolTip.text = text;

        toolTip.icon        = "";
        toolTip.iconDefault = iconDefault;

        pActivate();
    }

    function showIcon(text, iconDefault, icon, iconWidth, iconHeight)
    {
        itemIcon.iconWidth  = iconWidth;
        itemIcon.iconHeight = iconHeight;

        itemIcon.sourceSize = Qt.size(iconWidth, iconHeight);

        iconFillMode = Image.PreserveAspectCrop;

        toolTip.text = text;

        toolTip.icon        = icon;
        toolTip.iconDefault = iconDefault;

        pActivate();
    }

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    Icon
    {
        id: itemIcon

        anchors.left: parent.left

        anchors.leftMargin: Math.round((pIconWidth - width) / 2)

        anchors.verticalCenter: parent.verticalCenter

        clip: (fillMode == Image.PreserveAspectCrop)

        enableFilter: isSourceDefault
    }

    TextBase
    {
        id: itemText

        anchors.fill: parent

        leftMargin: (iconWidth) ? pIconWidth + spacing
                                : paddingLeft

        rightMargin: paddingRight

        verticalAlignment: Text.AlignVCenter

        style: Text.Raised
    }
}
