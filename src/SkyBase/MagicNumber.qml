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

Row
{
    id: magicNumber

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    /* mandatory */ property string text

    property int pixelSize: st.text_pixelSize

    property int itemWidth : pixelSize * 2.32
    property int itemHeight: pixelSize * 1.4

    property int radius: pixelSize / 3

    //---------------------------------------------------------------------------------------------
    // Style

    property color color          : st.magicNumber_color
    property color colorBackground: st.magicNumber_colorBackground

    //---------------------------------------------------------------------------------------------
    // Private

    property variant pList

    //---------------------------------------------------------------------------------------------
    // Spacing
    //---------------------------------------------------------------------------------------------

    spacing: Math.round(pixelSize / 4)

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    onTextChanged: pList = text.split(' ')

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

    Repeater
    {
        model: 4

        Rectangle
        {
            width : itemWidth
            height: itemHeight

            radius: magicNumber.radius

            color: colorBackground

            TextBase
            {
                id: itemText

                anchors.centerIn: parent

                text: pList[index]

                color: magicNumber.color

                font.pixelSize: pixelSize
            }
        }
    }
}
