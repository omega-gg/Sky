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
        var height;

        var playerHeight = player.height;

        // NOTE: When the player is hidden we display the subtitles on the cover.
        if (player.outputActive == AbstractBackend.OutputAudio || player.hasOutput)
        {
            height = cover.paintedHeight;
        }
        else height = Math.min(player.getRect().height, playerHeight);

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
        var size = width / 32;

        if (size < sizeMinimum)
        {
            return sizeMinimum;
        }
        else if (size > sizeMaximum)
        {
            return sizeMaximum;
        }
        else return size;
    }

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    BackendSubtitle
    {
        id: backend

        enabled: textSubtitle.visible

        QML_EVENT onLoaded: function(ok) { textSubtitle.loaded(ok) }
    }
}
