//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

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

BaseButton
{
    id: buttonRound

    //---------------------------------------------------------------------------------------------
    // Properties style
    //---------------------------------------------------------------------------------------------

    property ImageColorFilter filterDefault: (highlighted) ? st.buttonRound_filterHighlight
                                                           : st.buttonRound_filterDefault

    property ImageColorFilter filterHover: (highlighted) ? st.buttonRound_filterHighlightHover
                                                         : st.buttonRound_filterHover

    property ImageColorFilter filterPress: (highlighted || checkable) ? st.buttonRound_filterCheck
                                                                      : st.buttonRound_filterPress

    property ImageColorFilter filterPressHover: (highlighted
                                                 ||
                                                 checkable) ? st.buttonRound_filterCheckHover
                                                            : st.buttonRound_filterPressHover

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias isIconDefault: itemIcon.isSourceDefault

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

    property alias iconStyle: itemIcon.style

    //---------------------------------------------------------------------------------------------

    property alias background : background
    property alias imageBorder: imageBorder

    property alias itemIcon: itemIcon

    //---------------------------------------------------------------------------------------------
    // Style

    property alias enableFilter: itemIcon.enableFilter

    property alias filterBorder    : imageBorder.filter
    property alias filterIcon      : itemIcon.filterDefault
    property alias filterIconShadow: itemIcon.filterShadow
    property alias filterIconSunken: itemIcon.filterSunken

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    width : st.buttonRound_width
    height: st.buttonRound_height

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    ImageScale
    {
        anchors.fill: parent

        opacity: (window.isActive && isFocused)

        source: st.buttonRound_sourceFocus

        Behavior on opacity
        {
            PropertyAnimation { duration: st.duration_fast }
        }
    }

    ImageScale
    {
        id: background

        anchors.fill: parent

        source: st.buttonRound_sourceBackground

        filter:
        {
            if (isPressed)
            {
                return filterPress;
            }
            else if (checked)
            {
                if (isHovered && checkHover)
                {
                     return filterPressHover;
                }
                else return filterPress;
            }
            else if (isHovered)
            {
                 return filterHover;
            }
            else return filterDefault;
        }
    }

    ImageScale
    {
        id: imageBorder

        anchors.fill: parent

        opacity: (buttonRound.enabled) ? 1.0 : st.icon_opacityDisable

        source: st.buttonRound_sourceBorder

        filter: st.buttonRound_filterBorder

        Behavior on opacity
        {
            PropertyAnimation { duration: st.duration_fast }
        }
    }

    Icon
    {
        id: itemIcon

        anchors.centerIn: parent

        sourceSize.height: (scaling) ? -1 : parent.height

        sourceArea: (scaling) ? Qt.size(-1, -1) : Qt.size(parent.width, parent.height)

        opacity: (buttonRound.enabled) ? 1.0 : st.icon_opacityDisable

        style: (checked) ? Sk.IconRaised
                         : Sk.IconSunken

        filterDefault: st.buttonRound_filterIcon
        filterShadow : st.buttonRound_filterIconShadow
        filterSunken : st.buttonRound_filterIconSunken
    }
}
