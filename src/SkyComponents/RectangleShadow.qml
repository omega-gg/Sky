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

Item
{
    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property int direction: Sk.Left

    //---------------------------------------------------------------------------------------------
    // Style

    property alias filter: image.filter

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    Image
    {
        id: image

        anchors.fill: parent

        source:
        {
            if      (direction == Sk.Left)  return st.rectangleShadow_sourceLeft;
            else if (direction == Sk.Right) return st.rectangleShadow_sourceRight;
            else if (direction == Sk.Up)    return st.rectangleShadow_sourceUp;
            else                            return st.rectangleShadow_sourceDown;
        }

        filter: st.rectangleShadow_filter
    }
}
