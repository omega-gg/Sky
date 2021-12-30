//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkyTouch.

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

BaseButtonTouch
{
    id: buttonTouch

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property int padding: st.buttonTouch_padding

    property int minimumWidth: -1
    property int maximumWidth: -1

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias text: itemText.text

    property alias font: itemText.font

    //---------------------------------------------------------------------------------------------

    property alias itemText: itemText

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    width: st.getSize(sk.textWidth(itemText.font, text) + padding * 2,
                      minimumWidth, maximumWidth)

    height: st.buttonTouch_size

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

    TextBase
    {
        id: itemText

        anchors.fill: parent

        anchors.leftMargin : buttonTouch.padding
        anchors.rightMargin: buttonTouch.padding

        horizontalAlignment: Text.AlignHCenter
        verticalAlignment  : Text.AlignVCenter

        opacity: (buttonTouch.enabled) ? 1.0 : st.icon_opacityDisable

        wrapMode: Text.Wrap

        maximumLineCount: 2

        color: (isHighlighted || checked) ? st.text2_color
                                          : st.text1_color
    }
}
