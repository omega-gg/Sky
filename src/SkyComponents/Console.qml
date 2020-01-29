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

ScrollArea
{
    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property string text

    property int maximumLength: st.console_maximumLength

    //---------------------------------------------------------------------------------------------
    // Private

    property bool pUpdate: true

    property bool pAtBottom: true

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    contentHeight: itemText.height

    onContentHeightChanged: if (pAtBottom) scrollToBottom()

    onValueChanged: pAtBottom = atBottom

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    onVisibleChanged: if (visible) itemText.text = text

    onTextChanged: if (visible && pUpdate) itemText.text = text

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function append(string)
    {
//#QT_4
        pSetText(text + string);

        var length = text.length;

        if (length > maximumLength)
        {
            pSetText(text.substring(length - maximumLength));
        }

        if (visible) itemText.text = text;
//#ELSE
        if (visible)
        {
            var length = text.length;

            pSetText(text + string);

            itemText.insert(length, string);

            var count = itemText.length - maximumLength;

            if (count > 0)
            {
                pSetText(text.substring(count));

                itemText.remove(0, count);
            }
        }
        else
        {
            pSetText(text + string);

            /* var */ length = text.length;

            if (length > maximumLength)
            {
                pSetText(text.substring(length - maximumLength));
            }
        }
//#END
    }

    //---------------------------------------------------------------------------------------------
    // Private

    function pSetText(string)
    {
        pUpdate = false;

        text = string;

        pUpdate = true;
    }

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    BaseTextEdit
    {
        id: itemText

        anchors.left : parent.left
        anchors.right: parent.right

        wrapMode: Text.Wrap

        color      : st.text_color
        colorCursor: color

        font.family   : "consolas"
        font.pixelSize: st.dp14
        font.bold     : false
    }
}
