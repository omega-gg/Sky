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

TextBase
{
    id: textRich

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------
    // Style

    property color colorLink     : st.text_colorLink
    property color colorLinkHover: st.text_colorLinkHover

    //---------------------------------------------------------------------------------------------
    // Private

    property string pLink

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    textFormat: Text.RichText

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function getStyle(style)
    {
        return "<style>"
               +
               "p { margin: 0 0 " + st.dp8 + "px 0; }"
               +
               "p.br { margin-bottom: " + st.dp20 + "px; }"
               +
               "a { text-decoration: none; color: " + colorLink + "; }"
               +
               style + "</style>";
    }

    //---------------------------------------------------------------------------------------------
    // Events

    function onLinkClicked(link)
    {
        Qt.openUrlExternally(link)
    }

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    MouseArea
    {
        id: mouseArea

        anchors.fill: parent

        hoverEnabled: true
        hoverRetain : false

        onPositionChanged:
        {
            pLink = textRich.linkAt(mouseX, mouseY);

            if (pLink)
            {
                 cursor = Qt.PointingHandCursor;
            }
            else cursor = Qt.ArrowCursor;
        }

        onExited: cursor = Qt.ArrowCursor

        onReleased:
        {
            if (pLink == "") return;

            var link = textRich.linkAt(mouseX, mouseY);

            if (pLink == link)
            {
                onLinkClicked(link);
            }
        }
    }
}
