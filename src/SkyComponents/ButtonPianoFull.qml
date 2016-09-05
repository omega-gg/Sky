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

BaseButtonPiano
{
    id: buttonPianoFull

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property real padding: st.buttonPiano_padding

    property real paddingLeft : padding
    property real paddingRight: padding

    property real spacing: st.buttonPiano_spacing

    property real minimumWidth: -1
    property real maximumWidth: -1

    //---------------------------------------------------------------------------------------------
    // Style

    property ImageColorFilter filterIconCheck: st.buttonPiano_filterIconCheck

    //---------------------------------------------------------------------------------------------
    // Private

    property int pIconWidth: Math.max(itemIcon.width, background.height)

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

    width: getPreferredWidth()

    height: st.buttonPiano_height + borderSizeHeight

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function getPreferredWidth()
    {
        var size;

        if (iconWidth)
        {
            if (text)
            {
                size = Math.max(iconWidth, height) + sk.textWidth(text, font) + borderSizeWidth
                       +
                       paddingRight + spacing;
            }
            else size = Math.max(iconWidth, height) + borderSizeWidth;
        }
        else if (text)
        {
            size = sk.textWidth(text, font) + borderSizeWidth + paddingLeft + paddingRight;
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
    // Childs
    //---------------------------------------------------------------------------------------------

    Icon
    {
        id: itemIcon

        anchors.left: parent.left

        anchors.leftMargin: Math.round((pIconWidth - width) / 2)

        anchors.verticalCenter: parent.verticalCenter

        sourceSize.height: (scaling) ? -1 : parent.height

        opacity: (buttonPianoFull.enabled) ? 1.0 : st.icon_opacityDisable

        iconStyle: (checked) ? Sk.IconRaised
                             : Sk.IconSunken

        filter:
        {
            if (enableFilter)
            {
                if (checked && checkIcon) return filterIconCheck;
                else                      return filterIcon;
            }
            else return null;
        }

        filterDefault: st.buttonPiano_filterIcon
        filterShadow : st.buttonPiano_filterIconShadow
        filterSunken : st.buttonPiano_filterIconSunken
    }

    TextBase
    {
        id: itemText

        anchors.fill: parent

        leftMargin: (iconWidth) ? pIconWidth + spacing
                                : paddingLeft

        rightMargin: paddingRight

        verticalAlignment: Text.AlignVCenter

        opacity: (buttonPianoFull.enabled) ? 1.0 : st.text_opacityDisable

        style: (checked) ? Text.Raised
                         : Text.Sunken

        Behavior on opacity
        {
            PropertyAnimation { duration: st.duration_fast }
        }
    }
}
