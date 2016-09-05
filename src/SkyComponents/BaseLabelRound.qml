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

Item
{
    id: baseLabelRound

    //---------------------------------------------------------------------------------------------
    // Properties style
    //---------------------------------------------------------------------------------------------

    property variant borderBackground: st.labelRound_borderBackground

    property ImageColorFilter filterDefault: st.labelRound_filterDefault
    property ImageColorFilter filterDisable: st.labelRound_filterDisable

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias background : background
    property alias imageBorder: imageBorder

    //---------------------------------------------------------------------------------------------
    // Style

    property alias sourceBackground: background .source
    property alias sourceBorder    : imageBorder.source

    property alias filterBorder: imageBorder.filter

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    BorderImageScale
    {
        id: background

        anchors.fill: parent

        source: st.labelRound_sourceBackground

        border
        {
            left : borderBackground.x;     top   : borderBackground.y
            right: borderBackground.width; bottom: borderBackground.height
        }

        filter: (baseLabelRound.enabled) ? filterDefault
                                         : filterDisable
    }

    BorderImageScale
    {
        id: imageBorder

        anchors.fill: parent

        opacity: (baseLabelRound.enabled) ? 1.0 : st.border_opacityDisable

        source: st.labelRound_sourceBorder

        border
        {
            left : borderBackground.x;     top   : borderBackground.y
            right: borderBackground.width; bottom: borderBackground.height
        }

        filter: st.labelRound_filterBorder

        Behavior on opacity
        {
            PropertyAnimation { duration: st.duration_fast }
        }
    }
}
