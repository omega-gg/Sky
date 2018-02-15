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

ItemWall
{
    id: componentWall

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

//#QT_4
    width : getItemWidth (index)
    height: getItemHeight(index)

    x: getItemX(index)
    y: getItemY(index)
//#ELSE
    width : parent.getItemWidth (index)
    height: parent.getItemHeight(index)

    x: parent.getItemX(index)
    y: parent.getItemY(index)
//#END

    z:
    {
//#QT_4
        if (index == indexTop) return 1;
//#ELSE
        if (index == parent.indexTop) return 1;
//#END
        else if (opacity == 1.0) return  0;
        else                     return -1;
    }

//#QT_4
    visible: getItemVisible(index)

    opacity: (index != indexRemove)

    isHovered: (index == indexHover)
//#ELSE
    visible: parent.getItemVisible(index)

    opacity: (index != parent.indexRemove)

    isHovered: (index == parent.indexHover)
//#END

    isCurrent: (componentWall == currentItem)

    image: sourceImage

    icon       : sourceIcon
    iconDefault: sourceIconDefault

//#QT_4
    iconAsynchronous: asynchronous
//#ELSE
    iconAsynchronous: parent.asynchronous
//#END

    text: title

//#QT_4
    textMargin: getItemBarMargin(index)

    itemImage.asynchronous: asynchronous
//#ELSE
    textMargin: parent.getItemBarMargin(index)

    itemImage.asynchronous: parent.asynchronous
//#END

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    onEntered: parent.setItemHovered  (componentWall)
    onExited : parent.clearItemHovered()
}
