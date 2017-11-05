//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkyComponents module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

import QtQuick 1.1
import Sky     1.0

ComponentTab
{
    id: componentTabBrowser

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property bool isContextualHovered: (index == indexHover && index == indexContextual)

    //---------------------------------------------------------------------------------------------
    // Style

    property color colorContextualHoverA: st.itemTab_colorContextualHoverA
    property color colorContextualHoverB: st.itemTab_colorContextualHoverB

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    iconWidth: st.componentTabBrowser_iconWidth

    x: getItemX(index)

    z: (index == indexTop)

    clip: (componentTabBrowser == pClipItem)

    isHovered: (index == indexHover || index == indexContextual)

    isCurrent: (item == currentTab)

    icon: (item.cover == "") ? item.coverShot
                             : item.cover

    iconDefault: tabsBrowser.iconDefault

    text:
    {
        if (item.title)
        {
            return item.title;
        }
        else if (item.isLoading)
        {
            return qsTr("Loading...");
        }
        else return qsTr("New Tab");
    }

    iconFillMode: Image.PreserveAspectCrop

    iconCache: (item.cover != "" && item.coverShot != "")

    acceptedButtons: Qt.NoButton

    textMargin: getItemMargin(index)

    background.gradient: Gradient
    {
        GradientStop
        {
            position: 0.0

            color:
            {
                if      (isCurrent)           return colorSelectA;
                else if (isContextualHovered) return colorContextualHoverA;
                else if (isHovered)           return colorHoverA;
                else                          return colorA;
            }
        }

        GradientStop
        {
            position: 1.0

            color:
            {
                if      (isCurrent)           return colorSelectB;
                else if (isContextualHovered) return colorContextualHoverB;
                else if (isHovered)           return colorHoverB;
                else                          return colorB;
            }
        }
    }

    //---------------------------------------------------------------------------------------------
    // Animations
    //---------------------------------------------------------------------------------------------

    Behavior on x
    {
        enabled: (isAnimated && index != indexStatic)

        PropertyAnimation { duration: durationAnimation }
    }

    Behavior on width
    {
        enabled: (isAnimated && index != indexStatic)

        PropertyAnimation { duration: durationAnimation }
    }
}
