//=================================================================================================
/*
    Copyright (C) 2015-2020 HelloSky authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

import QtQuick 1.0
import Sky     1.0

Row
{
    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias buttonWidth : buttonMinimize.width
    property alias buttonHeight: buttonMinimize.height

    //---------------------------------------------------------------------------------------------

    property alias buttonMinimize: buttonMinimize
    property alias buttonMaximize: buttonMaximize
    property alias buttonClose   : buttonClose

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    anchors.margins: spacing

    spacing: st.dp2

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------
    // Events

    function onMaximize()
    {
        if (window.fullScreen)
        {
            window.fullScreen = false;
//#!MAC
            // FIXME macOS: We can't go from full screen to normal window right away.
            //              This could be related to the animation.
            window.maximized = false;
//#END
        }
        else window.maximized = !(window.maximized);
    }

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    ButtonTouchIcon
    {
        id: buttonMinimize

        width : st.dp40
        height: st.dp26

        margins: st.dp6

        icon: st.icon_minimize

        onClicked: window.minimized = true
    }

    ButtonTouchIcon
    {
        id: buttonMaximize

        width : buttonWidth
        height: buttonHeight

        margins: st.dp6

        highlighted: (window.maximized || window.fullScreen)

        icon: (highlighted) ? st.icon_restore
                            : st.icon_maximize

        onClicked: onMaximize()
    }

    ButtonTouchIcon
    {
        id: buttonClose

        width : buttonWidth
        height: buttonHeight

        margins: st.dp4

        icon: st.icon_close

        onClicked: window.close()
    }
}
