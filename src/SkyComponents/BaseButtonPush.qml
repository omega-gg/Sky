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
    id: baseButtonPush

    //---------------------------------------------------------------------------------------------
    // Properties style
    //---------------------------------------------------------------------------------------------

    property variant borderBackground: st.buttonPush_borderBackground

    property ImageColorFilter filterDefault: (highlighted) ? st.buttonPush_filterHighlight
                                                           : st.buttonPush_filterDefault

    property ImageColorFilter filterHover: (highlighted) ? st.buttonPush_filterHighlightHover
                                                         : st.buttonPush_filterHover

    property ImageColorFilter filterPress: (highlighted || checkable) ? st.buttonPush_filterCheck
                                                                      : st.buttonPush_filterPress

    property ImageColorFilter filterPressHover: (highlighted
                                                 ||
                                                 checkable) ? st.buttonPush_filterCheckHover
                                                            : st.buttonPush_filterPressHover

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias background : background
    property alias imageBorder: imageBorder
    property alias imageFocus : imageFocus

    //---------------------------------------------------------------------------------------------
    // Style

    property alias sourceBackground: background .source
    property alias sourceBorder    : imageBorder.source
    property alias sourceFocus     : imageFocus .source

    property alias filterBorder: imageBorder.filter
    property alias filterFocus : imageFocus .filter

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    cursor: Qt.ArrowCursor

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    BorderImageScale
    {
        id: imageFocus

        anchors.fill: parent

        source: st.buttonPush_sourceFocus

        border
        {
            left : borderBackground.x;     top   : borderBackground.y
            right: borderBackground.width; bottom: borderBackground.height
        }

        opacity: (window.isActive && isFocused)

        Behavior on opacity
        {
            PropertyAnimation { duration: st.duration_fast }
        }
    }

    BorderImageScale
    {
        id: background

        anchors.fill: parent

        source: st.buttonPush_sourceBackground

        border
        {
            left : borderBackground.x;     top   : borderBackground.y
            right: borderBackground.width; bottom: borderBackground.height
        }

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

    BorderImageScale
    {
        id: imageBorder

        anchors.fill: parent

        opacity: (baseButtonPush.enabled) ? 1.0 : st.border_opacityDisable

        source: st.buttonPush_sourceBorder

        border
        {
            left : borderBackground.x;     top   : borderBackground.y
            right: borderBackground.width; bottom: borderBackground.height
        }

        filter: st.buttonPush_filterBorder

        Behavior on opacity
        {
            PropertyAnimation { duration: st.duration_fast }
        }
    }
}
