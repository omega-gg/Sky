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

Label
{
    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    /* read */ property bool isTiny: st.checkTiny(preferredSize, maximumWidth)

    property int preferredSize: sk.textWidth(itemText.font, text) + padding * 2

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    width: st.getSize(preferredSize, minimumWidth, maximumWidth)

    height: (isTiny) ? st.labelTiny_size
                     : st.label_size

    itemText.horizontalAlignment: Text.AlignLeft

    itemText.wrapMode: (isTiny) ? Text.NoWrap
                                : Text.Wrap

    itemText.maximumLineCount: 2

//#DESKTOP
    // FIXME Qt5.14: Sometimes sk.textWidth() is too short.
    itemText.elide: (width == maximumWidth) ? Text.ElideRight
                                            : Text.ElideNone
//#END
}
