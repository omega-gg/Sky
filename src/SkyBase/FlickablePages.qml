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

Flickable
{
    id: flickablePages

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

//#QT_4
    /* read */ property bool isActive: (moving || animation.running)
//#ELSE
    /* read */ property bool isActive: (dragging || animation.running)
//#END

    property int count: 1

    property int currentPage: 0

    property real ratioVelocity: st.flickablePages_ratioVelocity

    //---------------------------------------------------------------------------------------------
    // Private

    property bool pUdpate: true

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias animation: animation

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    contentWidth : width * count
    contentHeight: height

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

//#QT_4
    onWidthChanged: if (moving == false) pUpdateX()
//#ELSE
    onWidthChanged: if (dragging == false) pUpdateX()
//#END

    onCurrentPageChanged: if (pUdpate) pUpdateX()

//#QT_4
    onMovingChanged: if (moving == false) pApplyPosition()
//#ELSE
    onDraggingChanged: if (dragging == false) pApplyPosition()
//#END

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function setPage(index)
    {
        if (index < 0 || index >= count) return;

        pApplyPage(index);
    }

    // NOTE: Can be useful when we're working in tandem with a ButtonSlide.
    function applyPage(index)
    {
        pUdpate = false;

        currentPage = index;

        pUdpate = true;
    }

    //---------------------------------------------------------------------------------------------
    // Private

    function pApplyPosition()
    {
        var position = contentX - currentPage * width;

        if (position == 0) return;

        // NOTE: We consider resetting the page when the velocity is too low. If we're less than
        //       halfway there we reset the page.
        if (Math.abs(horizontalVelocity) / width < ratioVelocity
            &&
            Math.abs(position) < width / 2)
        {
            pApplyPage(currentPage);

            return;
        }

        if (position < 0)
        {
            var index = currentPage - 1;

            if (index < 0) return;

            pApplyPage(index);
        }
        else
        {
            /* var */ index = currentPage + 1;

            if (index >= count) return;

            pApplyPage(index);
        }
    }

    function pApplyPage(index)
    {
        var to = width * index;

        if (contentX != to)
        {
            animation.from = contentX;
            animation.to   = to;

            animation.running = true;
        }

        applyPage(index);
    }

    function pUpdateX()
    {
        contentX = currentPage * width;
    }

    //---------------------------------------------------------------------------------------------
    // Animations
    //---------------------------------------------------------------------------------------------

    NumberAnimation
    {
        id: animation

        target: flickablePages

        property: "contentX"

        duration: st.duration_normal

        easing.type: st.easing
    }
}