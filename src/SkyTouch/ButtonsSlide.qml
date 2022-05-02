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

BaseButton
{
    id: buttonSlide

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    /* read */ property int count: model.count

    property int size: st.buttonTouch_size

    property int margins: st.buttonsSlides_margins

    property int currentIndex: -1

    property bool enableFilter: true

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias model: repeater.model

    //---------------------------------------------------------------------------------------------

    property alias background: background
    property alias foreground: foreground

    property alias repeater: repeater

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    width : size * count
    height: size

    cursor: Qt.PointingHandCursor

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function getSourceHeight(margins)
    {
        return st.getSizeMargins(height, margins);
    }

    //---------------------------------------------------------------------------------------------
    // Virtual

    /* virtual */ function getFilter(checked)
    {
        return getFilterDefault(checked);
    }

    /* virtual */ function getFilterDefault(checked)
    {
        if (checked)
        {
             return st.button_filterIconB;
        }
        else return st.button_filterIconA;
    }

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

    Rectangle
    {
        id: background

        anchors.fill: parent

        radius: height

        opacity: (isHovered) ? st.buttonTouch_opacityHover
                             : st.buttonTouch_opacity

        color: st.buttonTouch_color

//#QT_4
        smooth: true
//#END
    }

    Rectangle
    {
        id: foreground

        width : size - margins * 2
        height: width

        x: size * currentIndex + margins
        y: margins

        radius: height

        visible: (currentIndex >= 0 && currentIndex < count - 1)

        color: st.buttonTouch_colorHighlight
    }

    Repeater
    {
        id: repeater

        anchors.fill: parent

        model: ListModel {}

        delegate: Item
        {
            width : size
            height: width

            x: size * index

            ImageScale
            {
                id: itemIcon

                anchors.centerIn: parent

                sourceSize.height: getSourceHeight(margins)

                clip: (fillMode == Image.PreserveAspectCrop)

                source: cover

                filter: (enableFilter) ? getFilter(currentIndex == index) : null
            }
        }
    }
}
