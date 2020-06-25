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

BaseTabs
{
    id: tabsBrowser

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    /* mandatory */ property variant tabs

    property variant currentTab: tabs.currentTab

    /* read */ property bool isAnimated: false

    property url iconDefault

    /* read */ property variant itemActive    : pGetItemActive()
    /* read */ property variant itemContextual: null

    /* read */ property int indexAdd       : -1
    /* read */ property int indexRemove    : -1
    /* read */ property int indexPress     : -1
    /* read */ property int indexDrag      : -1
    /* read */ property int indexMove      : -1
    /* read */ property int indexTop       : -1
    /* read */ property int indexStatic    : -1
    /* read */ property int indexContextual: -1

    /* read */ property int indexActive: (indexContextual != -1) ? indexContextual
                                                                 : indexHover

    /* read */ property real currentTabWidth: pGetCurrentWidth()

    /* read */ property int dragX     : 0
    /* read */ property int dragMargin: 0

    //---------------------------------------------------------------------------------------------
    // Style

    property int durationAnimation: st.duration_normal

    //---------------------------------------------------------------------------------------------
    // Private

    property variant pClipItem: null

    property real pFixedTabWidth: -1

    property int pMaximumX: 0

    property bool pButtonsVisible: (pClipItem == null && indexDrag < 0)

    property int pButtonsX: (itemActive) ? itemActive.x + itemActive.width : 0

    //---------------------------------------------------------------------------------------------
    // Signals
    //---------------------------------------------------------------------------------------------

    signal tabPressed(int index)

    signal tabClicked      (int index)
    signal tabDoubleClicked(int index)

    signal contextual

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    height: st.baseTabs_height

    model: ModelTabs { tabs: tabsBrowser.tabs }

    delegate: ComponentTabBrowser {}

    //---------------------------------------------------------------------------------------------

    acceptedButtons: Qt.LeftButton | Qt.RightButton | Qt.MiddleButton

    cursor: Qt.PointingHandCursor

    //---------------------------------------------------------------------------------------------

    tabMinimum: st.tabsBrowser_tabMinimum
    tabMaximum: st.tabsBrowser_tabMaximum

    spacing: st.tabsBrowser_spacing

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    onPressed:
    {
        window.clearFocus();

        if (isAnimated || indexDrag != -1 || indexHover == -1) return;

        indexPress = indexHover;

        if (mouse.button & Qt.LeftButton)
        {
            hoverEnabled = true;

            tabPressed(indexHover);

            if (tabs.currentIndex == indexHover) return;

            selectTab(indexHover);
        }
        else if (mouse.button & Qt.RightButton)
        {
            contextual();
        }
    }

    onReleased: clearDrag()

    onPositionChanged:
    {
        if (indexDrag == -1 && hoverEnabled && count > 1 && indexHover != -1)
        {
            var item = itemAt(indexHover);

            if (item == null) return;

            dragMargin = mouseX - item.x;

            dragX = pGetDragX();

            pMaximumX = (count - 1) * currentTabWidth;

            indexDrag   = indexHover;
            indexTop    = indexDrag;
            indexStatic = indexDrag;
        }
        else if (indexDrag >= 0)
        {
            dragX = pGetDragX();

            var x = dragX + (tabWidth / 2);

            var index = Math.floor(x / currentTabWidth);

            if (indexMove != index)
            {
                startAnimation();

                indexMove = index;
            }
        }
    }

    onClicked:
    {
        if (indexPress == -1 || indexPress != indexHover) return;

        if (mouse.button & Qt.LeftButton)
        {
            tabClicked(indexHover);
        }
        else if (mouse.button & Qt.MiddleButton)
        {
            closeTab(indexHover);
        }
    }

    onDoubleClicked:
    {
        if (indexPress == -1 || indexPress != indexHover) return;

        if (mouse.button & Qt.LeftButton)
        {
            tabDoubleClicked(indexHover);
        }
    }

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function openTabAt(index)
    {
        if (tabs.isFull || isAnimated || onBeforeTabOpen(index) == false) return false;

        if (index < 0 || index >= count)
        {
            index = count;

            indexAdd = index;
        }

        startAnimation();

        tabs.insertTab(index);

        pClipItem = itemAt(index);

        if (count > 1)
        {
            if (index == (count - 1))
            {
                 indexTop = index - 1;
            }
            else indexTop = index + 1;
        }
        else indexTop = -1;

        indexAdd = -1;

        pFixedTabWidth = -1;

        return true;
    }

    function openTab()
    {
        return openTabAt(count);
    }

    //---------------------------------------------------------------------------------------------

    function closeTab(index)
    {
        if (isAnimated || index < 0 || index >= count || onBeforeTabClose(index) == false)
        {
            return false;
        }

        if (count == 1)
        {
            currentTab.clearBookmarks();

            return true;
        }

        if (itemHovered && pFixedTabWidth == -1 && index != (count - 1))
        {
            pFixedTabWidth = tabWidth;
        }

        window.clearContextual();

        startAnimation();

        indexRemove = index;

        if (index == tabs.currentIndex && count > 1)
        {
            var newIndex = index + 1;

            if (newIndex < count) tabs.currentIndex = newIndex;
            else                  tabs.currentIndex = newIndex - 2;
        }

        pClipItem = itemAt(indexRemove);

        return true;
    }

    function closeCurrentTab()
    {
        return closeTab(tabs.currentIndex);
    }

    //---------------------------------------------------------------------------------------------

    function removeTab(index)
    {
        if (index < 0 || index >= count) return;

        model.remove(index);
    }

    function selectTab(index)
    {
        if (index < 0 || index >= count || indexRemove == index
            ||
            onBeforeTabSelect(index) == false) return;

        tabs.currentIndex = index;
    }

    function moveTab(from, to)
    {
        tabs.moveTab(from, to);
    }

    //---------------------------------------------------------------------------------------------

    function startAnimation()
    {
        animationTimer.restart();

        isAnimated = st.animate;
    }

    function stopAnimation()
    {
        animationTimer.stop();

        isAnimated = false;
    }

    function isAnimated()
    {
        return animationTimer.running;
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

            moveTab(indexDrag, indexMove);
        }

        indexDrag = -1;
        indexMove = -1;

        dragX      = 0;
        dragMargin = 0;
    }

    //---------------------------------------------------------------------------------------------
    // Events

    function onBeforeTabOpen (index) { return true; }
    function onBeforeTabClose(index) { return true; }

    function onBeforeTabSelect(index) { return true; }

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
    // BaseTabs reimplementation

    function onAnimationTimeout()
    {
        if (indexMove != -1) return;

        stopAnimation();

        if (indexRemove != -1)
        {
            tabs.deleteAt(indexRemove);

            indexRemove = -1;
        }

        pClipItem = null;

        indexTop = -1;
    }

    //---------------------------------------------------------------------------------------------

    function setItemHovered(item)
    {
        if (itemHovered == item) return;

        itemHovered = item;

        resizeTimer.stop();
    }

    function clearItemHovered()
    {
        if (itemHovered == null) return;

        itemHovered = null;

        if (pFixedTabWidth != -1)
        {
            resizeTimer.start();
        }
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

        if (indexRemove != -1 && index > indexRemove) index--;

        return Math.round(index * currentTabWidth);
    }

    function getItemWidth(index)
    {
        if      (indexAdd    == index) return 0;
        else if (indexRemove == index) return 0;
        else                           return currentTabWidth;
    }

    //---------------------------------------------------------------------------------------------

    function getItemMargin(index)
    {
        if (pButtonsVisible && indexActive == index)
        {
            if (currentTabWidth > st.dp128) return st.dp60;
            else                            return st.dp28;
        }
        else return st.dp8;
    }

    //---------------------------------------------------------------------------------------------
    // Private

    function pGetCurrentWidth()
    {
        if      (pFixedTabWidth != -1) return pFixedTabWidth;
        else if (indexRemove    != -1) return calculateTabWidth(count - 1);
        else                           return tabWidth;
    }

    //---------------------------------------------------------------------------------------------

    function pGetItemActive()
    {
        if      (itemContextual) return itemContextual;
        else if (itemHovered)    return itemHovered;
        else                     return null;
    }

    //---------------------------------------------------------------------------------------------

    function pGetDragX()
    {
        var x = Math.max(0, mouseX - dragMargin);

        return Math.min(x, pMaximumX);
    }

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    Timer
    {
        id: animationTimer

        interval: durationAnimation

        onTriggered: onAnimationTimeout()
    }

    Timer
    {
        id: resizeTimer

        interval: st.tabsBrowser_intervalResize

        onTriggered:
        {
            startAnimation();

            pFixedTabWidth = -1;
        }
    }

    ButtonsItem
    {
        id: buttonsItem

        anchors.verticalCenter: parent.verticalCenter

        x: (itemActive) ? pButtonsX - width - st.dp4 : 0

        z: (itemActive) ? itemActive.z : 0

        visible: (pButtonsVisible && itemActive != null && currentTabWidth > st.dp128)

        checked: (indexContextual != -1)

        buttonClose.enabled: (count > 1 || currentTab.isValid)

        onContextual: tabsBrowser.contextual()

        onClose: closeTab(indexHover)
    }

    ButtonPianoIcon
    {
        id: buttonCornerClose

        width : st.dp20 + borderSizeWidth
        height: st.dp20 + borderSizeHeight

        x: (itemActive) ? pButtonsX - width : 0

        z: buttonsItem.z

        borderLeft  : borderSize
        borderBottom: borderSize

        visible: (pButtonsVisible && itemActive != null
                  &&
                  (itemActive.isCurrent || tabWidth > tabMinimum) && buttonsItem.visible == false)

        enabled: buttonsItem.enabled

        icon          : st.icon16x16_close
        iconSourceSize: st.size16x16

        colorHoverA: st.button_colorConfirmHoverA
        colorHoverB: st.button_colorConfirmHoverB

        colorPressA: st.button_colorConfirmPressA
        colorPressB: st.button_colorConfirmPressB

        filterIcon: (isHovered || isPressed) ? st.button_filterIconB
                                             : st.button_filterIconA

        onClicked: closeTab(indexHover)
    }
}
