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

Item
{
    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    default property alias contents: flickable.data

//#QT_LATEST
    property alias originY: flickable.originY
//#END

    property alias contentHeight: flickable.contentHeight
    property alias contentY     : flickable.contentY

    //---------------------------------------------------------------------------------------------

    property alias flickable: flickable

    property alias scrollBar: scrollBar

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function scrollToTop()
    {
//#QT_4
        contentY = 0;
//#ELSE
        contentY = originY;
//#END
    }

    function scrollToBottom()
    {
//#QT_4
        contentY = contentHeight - height;
//#ELSE
        contentY = originY + contentHeight - height;
//#END
    }

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    Flickable
    {
        id: flickable

        anchors.fill: parent

        anchors.rightMargin: (scrollBar.isActive) ? scrollBar.width : 0

        clip: true

        // FIXME Qt5.14.2: We have to reparent the second item manually if we want scrolling
        //                 to work.
        onChildrenChanged:
        {
            var item = flickable.children[1];

            if (item) item.parent = flickable.contentItem;
        }

        // NOTE: When we have a contextual area we hide its panels when scrolling.
        onMovementStarted: window.checkContextual(areaContextual, flickable)
    }

    ScrollBar
    {
        id: scrollBar

        anchors.right: parent.right

        view: flickable
    }
}
