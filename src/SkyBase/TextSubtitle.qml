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

TextBase
{
    id: textSubtitle

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property int pixelSize: sizeMinimum

    property int sizeMinimum: st.textSubtitle_sizeMinimum
    property int sizeMaximum: st.textSubtitle_sizeMaximum

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias enabled: backend.enabled

    property alias source: backend.source

    property alias currentTime: backend.currentTime

    //---------------------------------------------------------------------------------------------

    property alias backend: backend

    //---------------------------------------------------------------------------------------------
    // Signals
    //---------------------------------------------------------------------------------------------

    signal loaded(bool ok)

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    horizontalAlignment: Text.AlignHCenter

    text: backend.text

    color: st.textSubtitle_color

    style: Text.Outline

    styleColor: st.textSubtitle_colorStyle

    textFormat: Text.RichText

    wrapMode: Text.WordWrap

    font.pixelSize: pGetSize()

    font.bold: st.textSubtitle_bold

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    // NOTE: This function applies the proper subtitle size and returns the bottom margin.
    function applySize(player, cover)
    {
        var width;
        var height;

        var playerHeight = player.height;

        if (player.outputActive == AbstractBackend.OutputAudio)
        {
            width  = cover.paintedWidth;
            height = cover.paintedHeight;
        }
        else
        {
            var rect = player.getRect();

            width  = Math.min(rect.width,  player.width);
            height = Math.min(rect.height, playerHeight);
        }

        pixelSize = width / 32;

        if (height > 0)
        {
             return (playerHeight - height) / 2 + height / 64;
        }
        else return playerHeight / 64;
    }

    //---------------------------------------------------------------------------------------------
    // Private

    function pGetSize()
    {
        if (pixelSize < sizeMinimum)
        {
            return sizeMinimum;
        }
        else if (pixelSize > sizeMaximum)
        {
            return sizeMaximum;
        }
        else return pixelSize;
    }

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    BackendSubtitle
    {
        id: backend

        enabled: textSubtitle.visible

        onLoaded: textSubtitle.loaded(ok)
    }
}
