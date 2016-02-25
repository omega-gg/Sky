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

ItemWall
{
    id: componentWall

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    width : getItemWidth (index)
    height: getItemHeight(index)

    x: getItemX(index)
    y: getItemY(index)

    z:
    {
        if      (index == indexTop) return 1;
        else if (opacity == 1)      return 0;
        else                        return -1;
    }

    visible: getItemVisible(index)

    opacity: (index != indexRemove)

    isHovered: (index == indexHover)

    isCurrent: (componentWall == currentItem)

    image: sourceImage

    icon       : sourceIcon
    iconDefault: sourceIconDefault

    iconAsynchronous: asynchronous

    text: title

    textMargin: getItemBarMargin(index)

    itemImage.asynchronous: asynchronous

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    onEntered: setItemHovered  (componentWall)
    onExited : clearItemHovered()
}
