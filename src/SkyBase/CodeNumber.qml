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

Item
{
    id: codeNumber

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    /* mandatory */ property string text

    property int pixelSize: st.text_pixelSize

    property int itemWidth : pixelSize * 2.2
    property int itemHeight: pixelSize * 1.3

    //---------------------------------------------------------------------------------------------
    // Style

    property color colorBackground: st.codeNumber_colorBackground

    //---------------------------------------------------------------------------------------------
    // Private

    property int pSpacing: spacing * 2

    property variant pList: null

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias radius: background.radius

    property alias spacing: row.spacing

    //---------------------------------------------------------------------------------------------

    property alias background: background

    //---------------------------------------------------------------------------------------------
    // Style

    property alias color: background.color

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    width : row.width  + pSpacing
    height: row.height + pSpacing

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    onTextChanged: pList = text.split(' ')

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

    Rectangle
    {
        id: background

        anchors.fill: parent

        radius: Math.round(pixelSize / 3)

        opacity: st.codeNumber_opacity

        color: st.codeNumber_color
    }

    Row
    {
        id: row

        anchors.centerIn: parent

        spacing: Math.round(pixelSize / 4)

        Repeater
        {
            model: 4

            Rectangle
            {
                width : itemWidth
                height: itemHeight

                radius: codeNumber.radius

                color: colorBackground

                TextBase
                {
                    anchors.centerIn: parent

                    text: (pList) ? pList[index] : ""

                    color: codeNumber.color

                    font.pixelSize: pixelSize
                }
            }
        }
    }
}
