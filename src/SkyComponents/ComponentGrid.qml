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

ItemGrid
{
    id: componentGrid

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

//#QT_4
    width : getItemWidth (index)
    height: getItemHeight(index)
//#ELSE
    width : parent.getItemWidth (index)
    height: parent.getItemHeight(index)
//#END

//#QT_4
    isHovered: (index == indexHover)
//#ELSE
    isHovered: (index == parent.indexHover)
//#END

    isCurrent: (componentGrid == currentItem)

    image: sourceImage

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

//#QT_4
    onHoverEntered: setItemHovered  (componentGrid)
    onHoverExited : clearItemHovered()
//#ELSE
    onHoverEntered: parent.setItemHovered  (componentGrid)
    onHoverExited : parent.clearItemHovered()
//#END
}
