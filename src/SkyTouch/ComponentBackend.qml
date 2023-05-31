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
    id: componentBackend

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property int iconWidth : st.componentBackend_iconWidth
    property int iconHeight: iconWidth

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias checked: buttonIcon.checked

    property alias itemIcon: buttonIcon.itemIcon
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
    // NOTE Qt5.15: sometimes we get an undefined parent.
    anchors.left : (parent) ? parent.left  : undefined
    anchors.right: (parent) ? parent.right : undefined
//#END

    height: st.buttonTouch_size

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------
    // Events

    function onClickIcon()
    {
        ListView.view.currentIndex = index;
    }

    function onClickText() { onClickIcon(); }

    //-----------------------------------------------------------------------------------------
    // Virtual

    /* virtual */ function getFilter()
    {
        if (buttonIcon.isSourceDefault)
        {
             return getFilterDefault();
        }
        else return getFilterMask();
    }

    /* virtual */ function getFilterDefault()
    {
        return buttonIcon.getFilterDefault();
    }

    /* virtual */ function getFilterMask()
    {
        return st.icon_filterRound;
    }

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

    ButtonTouchIcon
    {
        id: buttonIcon

        width: parent.height

        iconWidth: (isSourceDefault) ? componentBackend.iconWidth
                                     : itemIcon.filter.width

        iconHeight: (isSourceDefault) ? componentBackend.iconHeight
                                      : itemIcon.filter.height

        // NOTE: We have to provide the width and height otherwise we might have incorrect sizes
        //       for cropped portrait covers.
        iconSourceSize:
        {
            var mask = getFilterMask();

            return Qt.size(mask.width, mask.height);
        }

        iconDefaultSize.height: getSourceHeight()

        icon: cover

        iconDefault: st.icon_feed

        iconFillMode: (isSourceDefault) ? Image.PreserveAspectFit
                                        : Image.PreserveAspectCrop

        iconAsynchronous: gui.asynchronous

        onClicked: onClickIcon()

        //-----------------------------------------------------------------------------------------
        // ButtonTouchIcon reimplementation

        /* virtual */ function getFilter()
        {
            return componentBackend.getFilter();
        }
    }

    ButtonTouchLeft
    {
        id: buttonText

        anchors.left  : buttonIcon.right
        anchors.right : parent.right
        anchors.top   : parent.top
        anchors.bottom: parent.bottom

        anchors.leftMargin: st.margins

        checked: buttonIcon.checked

        text: title

        onClicked: onClickText()
    }
}
