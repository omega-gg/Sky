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

TabsTrack
{
    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    /* mandatory */ property Player player

    //---------------------------------------------------------------------------------------------
    // Private

    property TabTrack pHoveredTab: tabs.tabAt(indexHover)

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias overlay: overlay

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function selectTab(index)
    {
        if (index < 0 || index >= count || indexRemove == index) return;

        if (highlightedTab)
        {
            var tab = tabs.tabAt(index);

            if (tab.currentTime != -1 && tabs.highlightedIndex != index)
            {
                 tabs.highlightedIndex = index;
            }
            else tabs.currentIndex = index;
        }
        else tabs.currentIndex = index;
    }

    //---------------------------------------------------------------------------------------------

    function selectPrevious()
    {
        selectTab(tabs.currentIndex - 1);
    }

    function selectNext()
    {
        selectTab(tabs.currentIndex + 1);
    }

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    IconOverlay
    {
        id: overlay

        width : st.tabsPlayer_overlayWidth
        height: st.tabsPlayer_overlayHeight

        x: (itemHovered != null) ? itemHovered.x + borderSize : 0

        visible: (player.isPlaying
                  &&
                  pHoveredTab != null
                  &&
                  pHoveredTab != currentTab && pHoveredTab != highlightedTab
                  &&
                  pHoveredTab.currentTime != -1)

        source    : st.icon24x24_play
        sourceSize: st.size24x24
    }
}
