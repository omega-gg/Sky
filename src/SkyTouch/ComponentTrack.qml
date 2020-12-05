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

ButtonTouchFull
{
    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

//#QT_4
    width: ListView.view.width
//#ELSE
    anchors.left : parent.left
    anchors.right: parent.right
//#END

    iconWidth: (isSourceDefault) ? st.componentTrack_iconWidth
                                 : st.icon_filterRound.width

    iconSourceSize.height: (isSourceDefault) ? getSourceHeight()
                                             : st.icon_filterRound.height

    margins: (isSourceDefault) ? st.componentTrack_marginsDefault
                               : st.componentTrack_margins

    checked: (index == ListView.view.currentIndex)

    icon: cover

    iconDefault: st.icon_feed

    iconFillMode: (isSourceDefault) ? Image.PreserveAspectFit
                                    : Image.PreserveAspectCrop

    iconAsynchronous: gui.asynchronous

    text: st.getTrackTitle(title, loadState, source)

    itemText.wrapMode: Text.Wrap

    itemText.maximumLineCount: 2

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    onClicked: ListView.view.currentIndex = index

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------
    // ButtonTouchIcon reimplementation

    /* virtual */ function getFilter()
    {
        if (isSourceDefault)
        {
            return getFilterDefault();
        }
        else return st.icon_filterRound;
    }
}
