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

BaseButton
{
    id: buttonMask

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property bool enableFilter: true

    property int margins: 0

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias isSourceDefault: itemIcon.isSourceDefault

    property alias icon       : itemIcon.source
    property alias iconDefault: itemIcon.sourceDefault

    property alias iconWidth : itemIcon.width
    property alias iconHeight: itemIcon.height

    property alias iconSourceSize : itemIcon.sourceSize
    property alias iconDefaultSize: itemIcon.defaultSize

    property alias iconSourceArea: itemIcon.sourceArea

    property alias iconLoadMode: itemIcon.loadMode
    property alias iconFillMode: itemIcon.fillMode

    property alias iconAsynchronous: itemIcon.asynchronous
    property alias iconCache       : itemIcon.cache

    property alias iconScaling: itemIcon.scaling

    //---------------------------------------------------------------------------------------------

    property alias itemIcon: itemIcon

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    width : st.buttonTouch_size
    height: width

    cursor: Qt.PointingHandCursor

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function getSourceHeight()
    {
        return st.getSizeMargins(height, margins);
    }

    //---------------------------------------------------------------------------------------------
    // Virtual

    /* virtual */ function getOpacity()
    {
        if      (isPressed) return st.buttonMask_opacityPress;
        else if (isHovered) return st.buttonMask_opacityHover;
        else                return st.buttonMask_opacity;
    }

    //---------------------------------------------------------------------------------------------

    /* virtual */ function getFilter()
    {
        return getFilterDefault();
    }

    /* virtual */ function getFilterDefault()
    {
        return st.getButtonFilter(buttonMask);
    }

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

    ImageScale
    {
        id: itemIcon

        anchors.centerIn: parent

        sourceSize.height: getSourceHeight()

        clip: (fillMode == Image.PreserveAspectCrop)

        opacity: (buttonMask.enabled) ? getOpacity() : st.icon_opacityDisable

        filter: (enableFilter) ? getFilter() : null
    }
}
