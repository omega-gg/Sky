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

    /* read */ property bool isActive: (handle.pressed || animation.running)

    /* read */ property int count: model.count

    property int size: st.buttonTouch_size

    property int margins: st.buttonSlide_margins

    property int currentIndex: -1

    property bool enableFilter: true

    //---------------------------------------------------------------------------------------------
    // Private

    property bool pUdpate: true

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias model: repeater.model

    property alias position: handle.x

    //---------------------------------------------------------------------------------------------

    property alias background: background

    property alias handle    : handle
    property alias foreground: foreground

    property alias repeater: repeater

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    width : size * count
    height: size

    // NOTE: This is useful when we want a custom 'highlighted' behavior.
    highlighted: (isActive == false)

    cursor: Qt.PointingHandCursor

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    onPressed: pApplyX(mouseX)

    onSizeChanged: pApplyHandle()

    onCurrentIndexChanged: if (pUdpate) pApplyHandle()

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    // NOTE: Can be useful when we're working in tandem with a FlickablePages.
    function applyIndex(index)
    {
        pUdpate = false;

        currentIndex = index;

        pUdpate = true;
    }

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
    // Private

    function pApplyHandle()
    {
        handle.x = size * currentIndex;
    }

    function pApplyX(x)
    {
        var index = Math.floor(x / size);

        var from = handle.x;
        var to   = size * index;

        if (from != to)
        {
            animation.from = from;
            animation.to   = to;

            animation.start();
        }

        applyIndex(index);
    }

    function pGetFilter(index)
    {
        if (enableFilter)
        {
            if (currentIndex == index && highlighted)
            {
                 return getFilter(true);
            }
            else return getFilter(false);
        }
        else return null;
    }

    //---------------------------------------------------------------------------------------------
    // Animations
    //---------------------------------------------------------------------------------------------

    NumberAnimation
    {
        id: animation

        target: handle

        property: "x"

        duration: st.duration_normal

        easing.type: st.easing
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

    MouseArea
    {
        id: handle

        anchors.top   : parent.top
        anchors.bottom: parent.bottom

        width: size

        visible: (currentIndex >= 0 && currentIndex < count)

        hoverEnabled: true

        cursor: Qt.PointingHandCursor

        drag.target: handle
        drag.axis  : Drag.XAxis

        drag.minimumX: 0
        drag.maximumX: parent.width - width

        onReleased: pApplyX(x + width / 2)

        Rectangle
        {
            id: foreground

            anchors.fill: parent

            anchors.margins: margins

            radius: height

            opacity: (highlighted) ? 1.0 : st.buttonSlide_opacityPress

            color: (highlighted) ? st.buttonSlide_colorHandle
                                 : st.buttonSlide_colorHandlePress

//#QT_4
            smooth: true
//#END
        }
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

                // NOTE: Since 'margins' comes from a static model we apply the ratio manually.
                sourceSize.height: getSourceHeight(st.dp(margins))

                clip: (fillMode == Image.PreserveAspectCrop)

                source: cover

                filter: pGetFilter(index)
            }
        }
    }
}
