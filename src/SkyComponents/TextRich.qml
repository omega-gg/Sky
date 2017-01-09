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

import QtQuick 1.1
import Sky     1.0

TextBase
{
    id: textRich

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------
    // Style

    property color colorLink     : st.textRich_colorLink
    property color colorLinkHover: st.textRich_colorLinkHover

    //---------------------------------------------------------------------------------------------
    // Private

    property variant pPos: null

    property string pLink

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    textFormat: Text.AutoText

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
    // Private

    function pClearPos()
    {
        if (pPos == null) return;

        setBlockForeground(pPos, colorLink);

        pPos = null;

        mouseArea.cursor = MouseArea.ArrowCursor;
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
            var pos = Qt.point(mouse.x, mouse.y);

            if (textRich.linkAt(pos))
            {
                pPos = pos;

                if (pressed == false)
                {
                    textRich.setBlockForeground(pos, colorLinkHover);
                }

                cursor = MouseArea.PointingHandCursor;
            }
            else pClearPos();
        }

        onExited: pClearPos()

        onPressed:
        {
            if (pPos == null) return;

            pLink = textRich.linkAt(pPos);

            textRich.setBlockForeground(pPos, colorLink)
        }

        onReleased:
        {
            if (pPos == null) return;

            var link = textRich.linkAt(pPos);

            if (pLink == link)
            {
                onLinkClicked(link);
            }
        }
    }
}
