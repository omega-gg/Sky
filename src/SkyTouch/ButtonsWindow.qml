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

    spacing: st.margins

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------
    // Events

    function onMaximize()
    {
        if (window.fullScreen)
        {
            window.fullScreen = false;
//#!MAC+!MOBILE
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

        width : st.buttonsWindow_width
        height: st.buttonsWindow_height

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
