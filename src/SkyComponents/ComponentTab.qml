//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkyComponents.

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

ItemTab
{
    id: componentTab

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

//#QT_4
    width: getItemWidth(index) + borderSize

    height: baseTabs.height

    x: getItemX(index)

    borderLeft : borderSize
    borderRight: borderSize

    isHovered: (index == indexHover)
    isCurrent: (index == indexCurrent)
//#ELSE
    width: parent.getItemWidth(index) + parent.borderSize

    height: parent.height

    x: parent.getItemX(index)

    borderLeft : parent.borderSize
    borderRight: parent.borderSize

    isHovered: (index == parent.indexHover)
    isCurrent: (index == parent.indexCurrent)
//#END

    icon       : source
    iconDefault: sourceDefault

//#QT_4
    iconAsynchronous: asynchronous
//#ELSE
    iconAsynchronous: parent.asynchronous
//#END

    text: title

//#QT_4
    textSpacing: spacing
//#ELSE
    textSpacing: parent.spacing
//#END

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

//#QT_4
    onEntered: setItemHovered  (componentTab)
    onExited : clearItemHovered()

    onPressed: selectTab(index)
//#ELSE
    onEntered: parent.setItemHovered  (componentTab)
    onExited : parent.clearItemHovered()

    onPressed: parent.selectTab(index)
//#END

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function getIndex() { return index; }
}
