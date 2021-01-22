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

Item
{
    id: componentTrack

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    default property alias content: buttonText.data

    property alias checked: buttonText.checked

    property alias itemText: buttonText.itemText

    //---------------------------------------------------------------------------------------------

    property alias buttonIcon: buttonIcon
    property alias buttonText: buttonText

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

//#QT_4
    width: ListView.view.width
//#ELSE
    anchors.left : parent.left
    anchors.right: parent.right
//#END

    height: st.buttonTouch_size

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------
    // Events

    function onClick()
    {
        ListView.view.currentIndex = index;
    }

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    ButtonTouchIcon
    {
        id: buttonIcon

        margins: (isSourceDefault) ? st.componentTrack_marginsDefault
                                   : st.componentTrack_margins

        iconWidth: (isSourceDefault) ? st.componentTrack_iconWidth
                                     : st.icon_filterRound.width

        iconSourceSize.height: (isSourceDefault) ? getSourceHeight()
                                                 : st.icon_filterRound.height

        icon: cover

        iconDefault: st.icon_feed

        iconFillMode: (isSourceDefault) ? Image.PreserveAspectFit
                                        : Image.PreserveAspectCrop

        iconAsynchronous: gui.asynchronous

        //-----------------------------------------------------------------------------------------
        // Functions
        //-----------------------------------------------------------------------------------------
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

    ButtonTouchLeft
    {
        id: buttonText

        anchors.left : buttonIcon.right
        anchors.right: parent.right

        anchors.leftMargin: st.margins

        checked: (index == componentTrack.ListView.view.currentIndex)

        text: st.getTrackTitle(title, loadState, source)

        onClicked: onClick()
    }
}
