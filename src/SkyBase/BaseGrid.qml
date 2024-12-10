//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkyBase.

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

GridView
{
    id: baseGrid

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property bool autoScroll: true

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    maximumFlickVelocity: st.flickable_maximumVelocity

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

//#QT_4
    // NOTE Qt4: We update the contentHeight to 1 because it's not updated properly when it's on 0.
    onCountChanged: if (count == 0) contentHeight = 1
//#END

    onCurrentIndexChanged: if (autoScroll) positionViewAtIndex(currentIndex, GridView.Contain)

    // NOTE: When we have a contextual area we hide its panels when scrolling.
    onMovementStarted: window.checkContextual(areaContextual, baseGrid)

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function selectPrevious()
    {
        if (count == 0 || currentIndex < 1) return;

        currentIndex--;
    }

    function selectNext()
    {
        if (count == 0 || currentIndex == count - 1) return;

        currentIndex++;
    }

    //---------------------------------------------------------------------------------------------

    function scrollToY(y)
    {
//#QT_4
        contentY = 0;
//#ELSE
        contentY = originY + y;
//#END
    }

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

    function getPreferredWidth(width)
    {
        return Math.floor(width / cellWidth) * cellWidth;
    }

    function getY()
    {
//#QT_4
        return contentY;
//#ELSE
        return originY + contentY;
//#END
    }

    function getReloadIndex()
    {
        var count = Math.round(width / cellWidth);

//#QT_4
        return Math.floor(contentY / cellHeight) * count;
//#ELSE
        // NOTE: We substract origin because we want to ignore it.
        return Math.floor((contentY - originY) / cellHeight) * count;
//#END
    }

    function getReloadCount()
    {
        var count = Math.round(width / cellWidth);

        // NOTE: We add 1 to cover the entire region when half a track is exposed at the top and
        //       the bottom of the list.
        return (Math.ceil(height / cellHeight) + 1) * count;
    }
}
