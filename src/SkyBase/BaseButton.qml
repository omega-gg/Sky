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

MouseArea
{
    id: baseButton

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property bool isFocused: activeFocus

    property bool isHovered: containsMouse

    property bool isHighlighted: (highlighted || (checkable && (checked || isPressed)))

    property bool isPressed: (pressed || isReturnPressed)

    property bool isReturnPressed: false

    property bool highlighted: false

    property bool checkable: false
    property bool checked  : false

    property bool checkHover: true

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    hoverEnabled: enabled

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    onVisibleChanged: returnReleased()
    onEnabledChanged: returnReleased()

    onActiveFocusChanged: returnReleased()

    //---------------------------------------------------------------------------------------------
    // Keys
    //---------------------------------------------------------------------------------------------

    QML_EVENT Keys.onPressed: function(event)
    {
        if (event.key == Qt.Key_Return || event.key == Qt.Key_Enter
            ||
            event.key == Qt.Key_Space)
        {
            event.accepted = true;

            isReturnPressed = true;

            press();

            click();
        }
    }

    Keys.onReleased: returnReleased()

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function focus()
    {
        forceActiveFocus();
    }

    //---------------------------------------------------------------------------------------------

    function returnPressed()
    {
        if (enabled == false) return;

        isReturnPressed = true;

        press();

        click();
    }

    function returnReleased()
    {
        if (isReturnPressed == false) return;

        isReturnPressed = false;

        release();
    }

    //---------------------------------------------------------------------------------------------

    function triggerPressed()
    {
        if (enabled == false) return;

        isReturnPressed = true;

        press();
    }

    function triggerReleased()
    {
        if (isReturnPressed == false) return;

        click();

        isReturnPressed = false;

        release();
    }

    //---------------------------------------------------------------------------------------------

    function triggerClick()
    {
        triggerPressed ();
        triggerReleased();
    }
}
