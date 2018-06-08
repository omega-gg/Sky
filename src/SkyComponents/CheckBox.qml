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
    id: checkBox

    //---------------------------------------------------------------------------------------------
    // Properties private
    //---------------------------------------------------------------------------------------------

    property variant pSourceSize: Qt.size(width, height)

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------
    // Style

    property alias filterDefault: background .filter
    property alias filterBorder : imageBorder.filter

    //---------------------------------------------------------------------------------------------
    // Signals
    //---------------------------------------------------------------------------------------------

    signal checkClicked

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    width : st.checkBox_width
    height: st.checkBox_height

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    onClicked: pClick()

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function pClick()
    {
        checked = !(checked);

        checkClicked();
    }

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    Image
    {
        anchors.fill: parent

        anchors.margins: -st.checkBox_margins

        sourceSize: pSourceSize

        opacity: (window.isActive && isFocused)

        source: st.checkBox_sourceBorder

        filter: st.checkBox_filterFocus

        Behavior on opacity
        {
            PropertyAnimation { duration: st.duration_fast }
        }
    }

    Image
    {
        id: background

        anchors.fill: parent

        sourceSize: pSourceSize

        source: st.checkBox_sourceDefault

        filter: (isFocused || isHovered) ? st.checkBox_filterHover
                                         : st.checkBox_filterDefault
    }

    Image
    {
        anchors.fill: parent

        sourceSize: pSourceSize

        visible: checked

        opacity: (checkBox.enabled) ? 1.0 : st.icon_opacityDisable

        source: st.checkBox_sourceCheck

        Behavior on opacity
        {
            PropertyAnimation { duration: st.duration_fast }
        }
    }

    Image
    {
        id: imageBorder

        anchors.fill: parent

        sourceSize: pSourceSize

        source: st.checkBox_sourceBorder

        filter: st.checkBox_filterBorder
    }
}
