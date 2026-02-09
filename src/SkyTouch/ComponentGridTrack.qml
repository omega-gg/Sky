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
    id: componentGridTrack

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    default property alias content: buttonText.data

    property alias checked: buttonCover.checked

    property alias itemIcon: buttonCover.itemIcon
    property alias itemText: buttonText .itemText

    //---------------------------------------------------------------------------------------------

    property alias buttonCover     : buttonCover
    property alias buttonBackground: buttonBackground

    property alias buttonText: buttonText

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    width : GridView.view.cellWidth  - GridView.view.spacing
    height: GridView.view.cellHeight - GridView.view.spacingBottom

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------
    // Events

    function onClick()
    {
        GridView.view.currentIndex = index;
    }

    //---------------------------------------------------------------------------------------------
    // Virtual

    /* virtual */ function getFilterMask()
    {
        return st.icon_filterGrid;
    }

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

    RectangleButton
    {
        anchors.left  : buttonCover.left
        anchors.right : buttonCover.right
        anchors.top   : parent.top
        anchors.bottom: buttonText.bottom
    }

    ButtonTouchTrack
    {
        id: buttonCover

        anchors.left  : parent.left
        anchors.right : parent.right
        anchors.top   : parent.top
        anchors.bottom: buttonText.top

        iconWidth : componentGridTrack.GridView.view.coverWidth
        iconHeight: componentGridTrack.GridView.view.coverHeight

        iconSourceSize.height: iconHeight

        opacityBackground: 0.0

        icon: cover

        iconFillMode: SkyImage.PreserveAspectFit

        iconAsynchronous: gui.asynchronous

        trackType: type

        enableFilter: (isSourceDefault == false)

        // NOTE: We want the button background behind the cover background.
        background.z: -1

        onClicked: onClick()

        //-----------------------------------------------------------------------------------------
        // ButtonTouchIcon reimplementation

        /* virtual */ function getFilter()
        {
            return componentGridTrack.getFilterMask();
        }

        //-----------------------------------------------------------------------------------------

        Rectangle
        {
            id: buttonBackground

            anchors.fill: parent

            anchors.margins: componentGridTrack.GridView.view.padding

            z: -1

            radius: st.radius

            color: "black"
        }
    }

    ButtonTouchLeft
    {
        id: buttonText

        anchors.left  : parent.left
        anchors.right : parent.right
        anchors.bottom: parent.bottom

        opacityBackground: 0.0

        text: st.getTrackTitle(title, loadState, source)

        padding: st.componentGridTrack_padding

        font.pixelSize: st.componentGridTrack_pixelSize
    }
}
