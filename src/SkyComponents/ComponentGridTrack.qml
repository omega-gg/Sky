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

ComponentGrid
{
    id: componentGridTrack

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property bool isContextualHovered: (index == view.indexHover
                                        &&
                                        index == view.indexContextual)

    property bool isSourceDefault: itemImage.isSourceDefault

    /* read */ property int time    : -1
    /* read */ property int duration: -1

    property int logoMargin: background.width / view.logoRatio

    //---------------------------------------------------------------------------------------------
    // Private

    // NOTE: This is required for the onPSourceChanged event.
    property string pSource: source

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    onPSourceChanged: pUpdateTime()

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    isHovered: (index == view.indexHover || index == view.indexContextual)

    isContextual: (index == view.indexContextual)

    width : GridView.view.cellWidth  - GridView.view.spacing
    height: GridView.view.cellHeight - GridView.view.spacingBottom

    image: cover

    text: st.getTrackTitle(title, loadState, source)

    acceptedButtons: Qt.LeftButton | Qt.RightButton | Qt.MiddleButton

    itemImage.anchors.leftMargin: (isSourceDefault) ? logoMargin : 0

    itemImage.anchors.rightMargin: itemImage.anchors.leftMargin

    itemImage.sourceSize: Qt.size(view.coverWidth, view.coverHeight)

    itemImage.sourceDefault: view.logo

    itemImage.fillMode: Image.PreserveAspectFit

    itemImage.scaling: isSourceDefault

    //---------------------------------------------------------------------------------------------
    // Style

    gradient: Gradient
    {
        GradientStop
        {
            position: 0.0

            color: (isSourceDefault) ? view.defaultColorA
                                     : st.itemGrid_color
        }

        GradientStop
        {
            position: 1.0

            color: (isSourceDefault) ? view.defaultColorB
                                     : st.itemGrid_color
        }
    }

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------
    // Private

    function pUpdateTime()
    {
        if (playlist == null) return;

        duration = playlist.trackDuration(index);

        if (duration < 1)
        {
             componentGridTrack.time = -1;
        }

        var time = controllerPlaylist.extractTime(source);

        if (time == 0 || time > duration)
        {
             componentGridTrack.time = -1;
        }
        else componentGridTrack.time = time;
    }

    function pGetBarWidth()
    {
        if (barProgress.visible == false) return 0;

        if (time < duration)
        {
            return time * background.width / duration;
        }
        else return width;
    }

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

    BarProgress
    {
        id: barProgress

        anchors.left  : parent.left
        anchors.bottom: parent.bottom

        anchors.leftMargin  : borderLeft
        anchors.bottomMargin: borderBottom

        width: pGetBarWidth()

        visible: (time > 0)
    }

    RectangleLive
    {
        anchors.right : parent.right
        anchors.bottom: barProgress.bottom

        anchors.rightMargin : borderRight
        anchors.bottomMargin: componentGridTrack.border.size + bar.height

        width: st.componentGridTrack_sizeLive

        trackType: type
    }
}
