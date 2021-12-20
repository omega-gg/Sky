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

MouseArea
{
    id: scrollArea

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    /* read */ property bool isScrollable: false

    property bool enableUpdateRange: true

    //---------------------------------------------------------------------------------------------
    // Private

    property int pHeight: -1

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    default property alias content: content.data

    property alias contentHeight: content.height

    property alias value   : scrollBar.value
    property alias position: scrollBar.position

    property alias minimum: scrollBar.minimum
    property alias maximum: scrollBar.maximum

    property alias singleStep: scrollBar.singleStep
    property alias pageStep  : scrollBar.pageStep

    property alias wheelMultiplier: scrollBar.wheelMultiplier

    property alias atTop   : scrollBar.atMinimum
    property alias atBottom: scrollBar.atMaximum

    //---------------------------------------------------------------------------------------------

    property alias container: scrollArea
    property alias scrollBar: scrollBar

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    clip: true

    acceptedButtons: Qt.NoButton

    wheelEnabled: true

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    onHeightChanged:
    {
        if (sk.fuzzyCompare(pHeight, height)) return;

        pHeight = height;

        pUpdateRange();
    }

    QML_EVENT onWheeled: function(steps)
    {
        if (scrollBar.visible && scrollBar.handle.pressed == false)
        {
            scrollBar.model.scroll(-steps * wheelMultiplier);

            onWheelUpdated();
        }
    }

    onEnableUpdateRangeChanged: pUpdateRange()

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function ensureVisible(y, height)
    {
        if (isScrollable == false) return;

        var minimum = scrollBar.value;
        var maximum = scrollBar.value + scrollArea.height;

        var itemMinimum = y;
        var itemMaximum = y + height;

        if (itemMinimum < minimum)
        {
            scrollBar.value -= minimum - itemMinimum;
        }
        else if (itemMaximum > maximum)
        {
            scrollBar.value += itemMaximum - maximum;
        }
    }

    //---------------------------------------------------------------------------------------------

    function scroll(steps)
    {
        scrollBar.scroll(steps);
    }

    //---------------------------------------------------------------------------------------------

    function scrollUp(steps)
    {
        scrollBar.scrollUp(steps);
    }

    function scrollDown(steps)
    {
        scrollBar.scrollDown(steps);
    }

    //---------------------------------------------------------------------------------------------

    function scrollTo(value)
    {
        scrollBar.scrollTo(value);
    }

    function scrollToTop()
    {
        scrollBar.scrollToMinimum();
    }

    function scrollToBottom()
    {
        scrollBar.scrollToMaximum();
    }

    //---------------------------------------------------------------------------------------------

    function checkVisible(x, y)
    {
        if (y >= value && y < (value + height))
        {
             return true;
        }
        else return false;
    }

    //---------------------------------------------------------------------------------------------

    function updateListHeight(list)
    {
        if ((value + height) < list.y || value > (list.y + list.height))
        {
            list.setRange(0, 0);

            return;
        }

        var y = value - list.y;

        if (y > 0)
        {
            if ((list.height - y) > height)
            {
                 list.setRange(y, y + height);
            }
            else list.setRange(y, list.height);
        }
        else if ((list.height + y) > height)
        {
             list.setRange(0, height);
        }
        else list.setRange(0, list.height);
    }

    //---------------------------------------------------------------------------------------------
    // Events

    function onRangeUpdated() {}
    function onValueUpdated() {}
    function onWheelUpdated() {}

    //---------------------------------------------------------------------------------------------
    // Private

    function pUpdateRange()
    {
        if (enableUpdateRange == false) return;

        if (contentHeight > height)
        {
            pageStep = height;

            scrollBar.model.setRange(0, contentHeight - height);

            isScrollable = true;
        }
        else
        {
            value    = 0;
            pageStep = 0;

            scrollBar.model.setRange(0, 0);

            isScrollable = false;
        }

        onRangeUpdated();
    }

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    Item
    {
        id: content

        anchors.left: parent.left

        anchors.right: (isScrollable) ? scrollBar.left
                                      : parent.right

        onHeightChanged: pUpdateRange()

        onYChanged: onValueUpdated()
    }

    ScrollBar
    {
        id: scrollBar

        anchors.right : parent.right
        anchors.top   : parent.top
        anchors.bottom: parent.bottom

        visible: isScrollable

        onValueChanged:
        {
            // FIXME Qt5: This might make the content blurry due to the float value.
            if (value == maximum)
            {
                 content.y = -value;
            }
            else content.y = -intValue;
        }
    }
}
