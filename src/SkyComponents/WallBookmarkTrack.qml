//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkyComponents.

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

BaseWall
{
    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    /* mandatory */ property variant itemTabs

    property variant tabs: itemTabs.tabs

    property variant currentTab    : itemTabs.tabs.currentTab
    property variant highlightedTab: itemTabs.tabs.highlightedTab

    property url iconDefault: itemTabs.iconDefault

    property url logo     : st.logo
    property int logoRatio: st.logoRatio

    /* read */ property variant itemHovered   : null
    /* read */ property variant itemContextual: null

    /* read */ property int indexHover     : (itemHovered) ? itemHovered.getIndex() : -1
    /* read */ property int indexRemove    : -1
    /* read */ property int indexPress     : -1
    /* read */ property int indexDrag      : -1
    /* read */ property int indexMove      : -1
    /* read */ property int indexTop       : -1
    /* read */ property int indexStatic    : -1
    /* read */ property int indexContextual: -1

    /* read */ property int indexActive: (indexContextual != -1) ? indexContextual
                                                                 : indexHover

    /* read */ property int dragX: 0
    /* read */ property int dragY: 0

    /* read */ property int dragXMargin: 0
    /* read */ property int dragYMargin: 0

    //---------------------------------------------------------------------------------------------
    // Private

    property bool pSilentAnimation: false

    property int pMaximumX: 0
    property int pMaximumY: 0

    //---------------------------------------------------------------------------------------------
    // Signals
    //---------------------------------------------------------------------------------------------

    signal itemClicked      (int index)
    signal itemDoubleClicked(int index)

    signal contextual

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    model: itemTabs.model

    delegate: ComponentWallBookmarkTrack {}

    //---------------------------------------------------------------------------------------------

    acceptedButtons: Qt.LeftButton | Qt.RightButton | Qt.MiddleButton

    cursor: Qt.PointingHandCursor

    //---------------------------------------------------------------------------------------------

    durationAnimation: itemTabs.durationAnimation

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    QML_EVENT onPressed: function(mouse)
    {
        window.clearFocus();

        if (isAnimated || indexDrag != -1 || indexHover == -1) return;

        indexPress = indexHover;

        if (mouse.button & Qt.LeftButton)
        {
            hoverEnabled = true;

            if (tabs.currentIndex == indexHover) return;

            selectItem(indexHover);
        }
        else if (mouse.button & Qt.RightButton)
        {
            contextual();
        }
    }

    onReleased: clearDrag()

    onPositionChanged:
    {
        if (indexDrag == -1 && hoverEnabled && pageItemCount > 1 && indexHover != -1)
        {
            var item = itemAt(indexHover);

            if (item == null) return;

            var itemMouseX = mouseX - item.x;
            var itemMouseY = mouseY - item.y;

            tabs.currentIndex = indexHover;

            dragXMargin = itemMouseX;
            dragYMargin = itemMouseY;

            dragX = pGetDragX();
            dragY = pGetDragY();

            pMaximumX = (countHorizontal - 1) * (itemWidth  + marginsHorizontal)
                        +
                        marginsHorizontal;

            pMaximumY = (countVertical - 1) * (itemHeight + marginsVertical)
                        +
                        marginsVertical;

            indexDrag   = indexHover;
            indexTop    = indexDrag;
            indexStatic = indexDrag;
        }
        else if (indexDrag >= 0)
        {
            dragX = pGetDragX();
            dragY = pGetDragY();

            var x = dragX - marginsHorizontal;
            var y = dragY - marginsVertical;

            x = Math.round(x / (itemWidth  + marginsHorizontal));
            y = Math.round(y / (itemHeight + marginsVertical));

            var index = calculateGlobalIndex(x, y);

            if (index >= count)
            {
                indexMove = count - 1;
            }
            else if (indexMove != index)
            {
                startAnimation();

                indexMove = index;
            }
        }
    }

    QML_EVENT onClicked: function(mouse)
    {
        if (indexPress == -1 || indexPress != indexHover) return;

        if (mouse.button & Qt.LeftButton)
        {
            itemClicked(indexHover);
        }
        else if (mouse.button & Qt.MiddleButton)
        {
            closeItem(indexHover);
        }
    }

    QML_EVENT onDoubleClicked: function(mouse)
    {
        if (indexPress == -1 || indexPress != indexHover) return;

        if (mouse.button & Qt.LeftButton)
        {
            itemDoubleClicked(indexHover);
        }
    }

    //---------------------------------------------------------------------------------------------

    onIsAnimatedChanged:
    {
        if (pSilentAnimation == false)
        {
            itemTabs.isAnimated = isAnimated;
        }
    }

    onIndexTopChanged: itemTabs.indexTop = indexTop

    onIndexRemoveChanged: itemTabs.indexRemove = indexRemove

    //---------------------------------------------------------------------------------------------
    // Connections
    //---------------------------------------------------------------------------------------------

    Connections
    {
        target: itemTabs

        QML_CONNECTION function onIsAnimatedChanged()
        {
            if (pActive == false) return;

            isAnimated = itemTabs.isAnimated;
        }

        QML_CONNECTION function onIndexTopChanged()
        {
            indexTop = itemTabs.indexTop
        }

        QML_CONNECTION function onIndexRemoveChanged()
        {
            if (itemTabs.indexRemove == indexRemove) return;

            if (itemTabs.indexRemove != -1) startFixedSize();

            indexRemove = itemTabs.indexRemove;
        }
    }

    Connections
    {
        target: tabs

        QML_CONNECTION function onCurrentIndexChanged() { updateCurrentPage() }
    }

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function closeItem(index)
    {
        if (isAnimated || index < 0 || index >= count || onBeforeItemClose(index) == false)
        {
            return false;
        }

        if (count == 1)
        {
            currentTab.clearBookmarks();

            return true;
        }

        clearItemHovered();

        window.clearContextual();

        startFixedSize();
        startAnimation();

        indexRemove = index;

        if (tabs.currentIndex == index && count > 1)
        {
            var newIndex = index + 1;

            if (newIndex < count) tabs.currentIndex = newIndex;
            else                  tabs.currentIndex = newIndex - 2;
        }

        return true;
    }

    //---------------------------------------------------------------------------------------------

    function removeItem(index)
    {
        itemTabs.removeTab(index);
    }

    function selectItem(index)
    {
        itemTabs.selectTab(index);
    }

    function moveItem(from, to)
    {
        itemTabs.moveTab(from, to);
    }

    //---------------------------------------------------------------------------------------------

    function startFixedSize()
    {
        enableFixedSize(st.wallBookmarkTrack_fixedDuration);
    }

    //---------------------------------------------------------------------------------------------

    function getCurrentItem()
    {
        return itemAt(tabs.currentIndex);
    }

    function getHighlightedItem()
    {
        return itemAt(tabs.highlightedIndex);
    }

    //---------------------------------------------------------------------------------------------

    function getItemVisible(index)
    {
        if (isActive == false)
        {
            return false;
        }
        else if (isAnimated || isChangingPage || isFixedSize)
        {
            return true;
        }
        else
        {
            var pageIndex = pageFromIndex(index);

            if (pageIndex == currentPage)
            {
                 return true;
            }
            else return false;
        }
    }

    //---------------------------------------------------------------------------------------------

    function setIndexContextual(index)
    {
        var item = itemAt(index);

        if (item)
        {
            indexContextual = index;

            itemContextual = item;
        }
    }

    function clearIndexContextual()
    {
        indexContextual = -1;

        itemContextual = null;
    }

    //---------------------------------------------------------------------------------------------

    function clearDrag()
    {
        if (hoverEnabled == false) return;

        itemTabs.clearDrag();

        hoverEnabled = false;

        if (indexDrag < 0)
        {
            indexDrag = -1;

            return;
        }

        indexStatic = -1;

        startAnimation();

        if (indexMove != -1 && indexMove != indexDrag)
        {
            indexTop = indexMove;

            moveItem(indexDrag, indexMove);
        }

        indexDrag = -1;
        indexMove = -1;

        dragX = 0;
        dragY = 0;

        dragXMargin = 0;
        dragYMargin = 0;
    }

    //---------------------------------------------------------------------------------------------
    // Events

    function onBeforeItemClose(index) { return true; }

    //---------------------------------------------------------------------------------------------

    function onContextualClear()
    {
        if (indexContextual != indexHover)
        {
            window.areaContextual.clearLastParent();
        }

        clearIndexContextual();
    }

    //---------------------------------------------------------------------------------------------
    // BaseWall reimplementation

    function onAnimationTimeout()
    {
        if (indexMove != -1) return;

        stopAnimation();

        if (indexRemove != -1)
        {
            tabs.deleteAt(indexRemove);

            indexRemove = -1;
        }

        indexTop = -1;
    }

    function onUpdateTimeout()
    {
        pSilentAnimation = true;

        startAnimation();

        pSilentAnimation = false;

        updateView();
    }

    function onFixedSizeTimeout()
    {
        isFixedSize = false;

        pSilentAnimation = true;

        startAnimation();

        pSilentAnimation = false;

        updateView();
    }

    //---------------------------------------------------------------------------------------------

    function setItemHovered(item)
    {
        itemHovered = item;
    }

    function clearItemHovered()
    {
        itemHovered = null;
    }

    //---------------------------------------------------------------------------------------------

    function updateCurrentPage()
    {
        if (isFixedSize) return;

        pSilentAnimation = true;

        setPageFromIndex(tabs.currentIndex);

        pSilentAnimation = false;
    }

    //---------------------------------------------------------------------------------------------

    function getItemX(index)
    {
        if (indexMove != -1)
        {
            if (index != indexDrag)
            {
                if (index >  indexDrag) index--;
                if (index >= indexMove) index++;
            }
            else return dragX;
        }

        if (indexRemove != -1 && index > indexRemove)
        {
            index--;
        }

        return calculateItemX(index);
    }

    function getItemY(index)
    {
        if (indexMove != -1)
        {
            if (index != indexDrag)
            {
                if (index >  indexDrag) index--;
                if (index >= indexMove) index++;
            }
            else return dragY;
        }

        if (indexRemove != -1 && index > indexRemove)
        {
            index--;
        }

        return calculateItemY(index);
    }

    //---------------------------------------------------------------------------------------------
    // Private

    function pGetDragX()
    {
        var x = Math.max(marginsHorizontal, mouseX - dragXMargin);

        return Math.min(x, pMaximumX);
    }

    function pGetDragY()
    {
        var y = Math.max(marginsVertical, mouseY - dragYMargin);

        return Math.min(y, pMaximumY);
    }
}
