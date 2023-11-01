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

Item
{
    id: buttonsCheck

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    /* read */ property int count: (model) ? model.count : 0

    /* read */ property int buttonWidth: (width - extra * 2) / count

    property int padding: st.buttonPush_padding

    property int extra: height / 8 + st.border_size / 2

    property bool checkable: true

    property int currentIndex : -1
    property int currentActive: -1

    property int elide: Text.ElideRight

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias model   : repeater.model
    property alias delegate: repeater.delegate

    //---------------------------------------------------------------------------------------------

    property alias repeater: repeater

    //---------------------------------------------------------------------------------------------
    // Signals
    //---------------------------------------------------------------------------------------------

    signal pressed(int index)
    signal clicked(int index)

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    height: st.buttonPush_height

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function itemAt(index)
    {
        return repeater.itemAt(index).item;
    }

    function pressAt(index)
    {
        if (checkable)
        {
            if (currentIndex == index) return;

            currentIndex = index;
        }

        pressed(index);
    }

    function clickAt(index)
    {
        clicked(index);
    }

    //---------------------------------------------------------------------------------------------
    // Private

    function pGetWidth(index, x)
    {
        if (index == 0)
        {
            return buttonWidth + extra;
        }
        else if (index == count - 1)
        {
            return width - x;
        }
        else return buttonWidth;
    }

    function pGetX(index)
    {
        if (index == 0)
        {
             return index * buttonWidth;
        }
        else return index * buttonWidth + extra;
    }

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

    Repeater
    {
        id: repeater

        anchors.fill: parent

        delegate: Loader
        {
            anchors.top   : parent.top
            anchors.bottom: parent.bottom

            width: pGetWidth(index, x)

            x: pGetX(index)

            sourceComponent:
            {
                if      (index == 0)         return left;
                else if (index == count - 1) return right;
                else                         return center;
            }

            Component
            {
                id: left

                ButtonPushLeft
                {
                    anchors.fill: parent

                    padding: buttonsCheck.padding

                    highlighted: (index == currentActive)

                    checked   : (index == currentIndex)
                    checkHover: false

                    text: title

                    itemText.elide: buttonsCheck.elide

                    /* QML_EVENT */ onPressed: function(mouse) { pressAt(index) }
                    /* QML_EVENT */ onClicked: function(mouse) { clickAt(index) }
                }
            }

            Component
            {
                id: center

                ButtonPushCenter
                {
                    anchors.fill: parent

                    padding: buttonsCheck.padding - margins

                    highlighted: (index == currentActive)

                    checked   : (index == currentIndex)
                    checkHover: false

                    text: title

                    itemText.elide: buttonsCheck.elide

                    /* QML_EVENT */ onPressed: function(mouse) { pressAt(index) }
                    /* QML_EVENT */ onClicked: function(mouse) { clickAt(index) }
                }
            }

            Component
            {
                id: right

                ButtonPushRight
                {
                    anchors.fill: parent

                    padding: buttonsCheck.padding

                    highlighted: (index == currentActive)

                    checked   : (index == currentIndex)
                    checkHover: false

                    text: title

                    itemText.elide: buttonsCheck.elide

                    /* QML_EVENT */ onPressed: function(mouse) { pressAt(index) }
                    /* QML_EVENT */ onClicked: function(mouse) { clickAt(index) }
                }
            }
        }
    }
}
