//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkyComponents module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
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

    property int currentIndex : -1
    property int currentActive: -1

    property int elide: Text.ElideRight

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias model   : repeater.model
    property alias delegate: repeater.delegate

    //---------------------------------------------------------------------------------------------
    // Signals
    //---------------------------------------------------------------------------------------------

    signal pressed

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    height: st.buttonPush_height

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function pGetX(index)
    {
        if (index == 0)
        {
             return index * buttonWidth;
        }
        else return index * buttonWidth + extra;
    }

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

    //---------------------------------------------------------------------------------------------

    function pressAt(index)
    {
        if (currentIndex == index) return;

        currentIndex = index;

        pressed();
    }

    //---------------------------------------------------------------------------------------------
    // Childs
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

                    onPressed: pressAt(index)
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

                    onPressed: pressAt(index)
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

                    onPressed: pressAt(index)
                }
            }
        }
    }
}
