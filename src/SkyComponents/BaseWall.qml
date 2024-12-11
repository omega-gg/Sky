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

MouseArea
{
    id: baseWall

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    /* read */ property int count: (model) ? model.count : 0

    /* read */ property bool isAnimated: false

    /* read */ property bool isActive: true

    /* read */ property bool isFixedSize: false

    /* read */ property bool isChangingPage: false

    property bool enableAnimation: true

    property bool asynchronous: true

    //---------------------------------------------------------------------------------------------
    // Item size

    property int itemPreferredWidth : st.baseWall_itemPreferredWidth
    property int itemPreferredHeight: st.baseWall_itemPreferredHeight

    property real itemRatioWidth : st.baseWall_itemRatioWidth
    property real itemRatioHeight: st.baseWall_itemRatioHeight

    property int itemExtraWidth : st.baseWall_itemExtraWidth
    property int itemExtraHeight: st.baseWall_itemExtraHeight

    /* read */ property int itemWidth : itemPreferredWidth
    /* read */ property int itemHeight: itemPreferredHeight

    //---------------------------------------------------------------------------------------------
    // Margin size

    property int marginsMinSize: st.baseWall_marginsMinSize

    property int marginsHorizontal: marginsMinSize
    property int marginsVertical  : marginsMinSize

    //---------------------------------------------------------------------------------------------
    // Pages

    /* read */ property int pageCount: 1

    /* read */ property int currentPage: 0

    /* read */ property int pageItemCount: 1

    /* read */ property int countHorizontal: 1
    /* read */ property int countVertical  : 1

    //---------------------------------------------------------------------------------------------
    // Style

    property int durationAnimation: st.duration_normal

    //---------------------------------------------------------------------------------------------
    // Private

    property bool pActive: isActive

    property bool pScale: ((itemPreferredHeight + pExtra) > height)

    property int pExtra: itemExtraHeight + marginsMinSize * 2

    property int pWidth: (pScale) ? Math.floor(pHeight * itemRatioWidth)
                                  : itemPreferredWidth

    property int pHeight: (pScale) ? Math.floor(height - pExtra)
                                   : itemPreferredHeight

    property int pPageLast     : -1
    property int pPageLastIndex:  0
    property int pPageGap      :  0

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias model   : repeater.model
    property alias delegate: repeater.delegate

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    acceptedButtons: Qt.NoButton

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    onWidthChanged : if (pActive) updateTimer.restart()
    onHeightChanged: if (pActive) updateTimer.restart()

    onCountChanged: if (pActive) updateView()

    onIsActiveChanged: pUpdateActive()
    onVisibleChanged : pUpdateActive()

    onEnableAnimationChanged: updateTimer.stop()

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function startAnimation()
    {
        if (pActive == false || enableAnimation == false) return;

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

    function enableFixedSize(duration)
    {
        if (pActive == false) return;

        fixedSizeTimer.interval = duration;

        fixedSizeTimer.restart();

        isFixedSize = true;
    }

    //---------------------------------------------------------------------------------------------

    function setPage(index)
    {
        if (currentPage == index
            ||
            index < 0 || index > (pageCount - 1)) return;

        if (pActive) isChangingPage = true;

        startAnimation();

        currentPage = index;
    }

    function setPageFromIndex(index)
    {
        setPage(pageFromIndex(index));
    }

    //---------------------------------------------------------------------------------------------

    function updateView()
    {
        if (isFixedSize) return;

        pUpdateItemCount();
        pUpdatePageCount();

        pUpdateItemSize();

        updateCurrentPage();
    }

    //---------------------------------------------------------------------------------------------

    function updateCurrentPage()
    {
        if (pPageLast != -1 && currentPage > pPageLast)
        {
            setPage(pPageLast);
        }
    }

    //---------------------------------------------------------------------------------------------

    function calculateItemX(index)
    {
        if (index >= count) index--;

        var pageIndex;

        if (currentPage == pPageLast && index >= pPageLastIndex)
        {
            pageIndex = Math.floor((index + pPageGap) / pageItemCount);

            index += pPageGap;
        }
        else pageIndex = Math.floor(index / pageItemCount);

        index = (index - (pageIndex * pageItemCount)) % countHorizontal;

        var x = Math.floor(index) * (itemWidth + marginsHorizontal) + marginsHorizontal;

        if (pageIndex == currentPage)
        {
            return x;
        }
        else if (pageIndex > currentPage)
        {
             return (x + (repeater.width * (pageIndex - currentPage)));
        }
        else return (x - (repeater.width * (currentPage - pageIndex)));
    }

    function calculateItemY(index)
    {
        if (index >= count) index--;

        var pageIndex;

        if (currentPage == pPageLast && index >= pPageLastIndex)
        {
            pageIndex = Math.floor((index + pPageGap) / pageItemCount);

            index += pPageGap;
        }
        else pageIndex = Math.floor(index / pageItemCount);

        index = (index - (pageIndex * pageItemCount)) / countHorizontal;

        return Math.floor(index) * (itemHeight + marginsVertical) + marginsVertical;
    }

    //---------------------------------------------------------------------------------------------

    function pageFromIndex(index)
    {
        if (currentPage == pPageLast && index >= pPageLastIndex)
        {
             return Math.floor((index + pPageGap) / pageItemCount);
        }
        else return Math.floor(index / pageItemCount);
    }

    //---------------------------------------------------------------------------------------------

    function itemAt(index)
    {
        for (var i = 0; i < children.length; i++)
        {
            var child = children[i];

            if (typeof child.getIndex == "function" && (child.getIndex()) == index)
            {
                return child;
            }
        }

        return null;
    }

    function indexFromPos(x, y)
    {
        if (x < 0 || x > repeater.width
            ||
            y < 0 || y > repeater.height) return -1;

        for (var i = 0; i < children.length; i++)
        {
            var child = children[i];

            if (typeof child.getIndex == "function" && child.visible)
            {
                if (x >= child.x && x < (child.x + child.width)
                    &&
                    y >= child.y && y < (child.y + child.height))
                {
                    return child.getIndex();
                }
            }
        }

        return -1;
    }

    //---------------------------------------------------------------------------------------------

    function calculateGlobalIndex(indexX, indexY)
    {
        var index = currentPage * pageItemCount;

        if (currentPage == pPageLast)
        {
             index += (indexY * countHorizontal) + indexX - pPageGap;
        }
        else index += (indexY * countHorizontal) + indexX;

        return index;
    }

    //---------------------------------------------------------------------------------------------

    function onAnimationTimeout()
    {
        stopAnimation();
    }

    function onUpdateTimeout()
    {
        startAnimation();

        updateView();
    }

    function onFixedSizeTimeout()
    {
        isFixedSize = false;

        startAnimation();

        updateView();
    }

    //---------------------------------------------------------------------------------------------
    // Virtual

    /* virtual */ function getItemX(index)
    {
        return calculateItemX(index);
    }

    /* virtual */ function getItemY(index)
    {
        return calculateItemY(index);
    }

    //---------------------------------------------------------------------------------------------

    /* virtual */ function getItemWidth(index)
    {
        return itemWidth;
    }

    /* virtual */ function getItemHeight(index)
    {
        return itemHeight;
    }

    /* virtual */ function getItemBarMargin(index) { return st.dp8; }

    //---------------------------------------------------------------------------------------------
    // Private

    function pUpdateItemCount()
    {
        var countHorizontal = Math.floor((width - marginsMinSize)
                                         / (pWidth + itemExtraWidth + marginsMinSize));

        var countVertical = Math.floor((height - marginsMinSize)
                                       / (pHeight + itemExtraHeight + marginsMinSize));

        countHorizontal = Math.max(1, countHorizontal);
        countVertical   = Math.max(1, countVertical);

        var pageItemCount = countHorizontal * countVertical;

        var horizontal = 1;
        var vertical   = 1;

        var count = 1;

        var ratio      = Math.round(countHorizontal / countVertical);
        var ratioIndex = 0;

        while (count < pageItemCount && count < baseWall.count)
        {
            if (ratioIndex < ratio && horizontal < countHorizontal)
            {
                horizontal++;

                ratioIndex++;
            }
            else if (vertical < countVertical)
            {
                vertical++;

                ratioIndex = 0;
            }
            else horizontal++;

            count = horizontal * vertical;
        }

        baseWall.countHorizontal = horizontal;
        baseWall.countVertical   = vertical;

        baseWall.pageItemCount = Math.max(1, count);
    }

    function pUpdatePageCount()
    {
        pageCount = Math.max(1, Math.ceil(count / pageItemCount));

        if (pageCount > 1)
        {
            pPageLast = pageCount - 1;

            pPageLastIndex = (count - pageItemCount);

            pPageGap = (pageItemCount * pageCount) - count;

            repeater.anchors.bottomMargin = st.dp8;
        }
        else
        {
            pPageLast      = -1;
            pPageLastIndex =  0;
            pPageGap       =  0;

            repeater.anchors.bottomMargin = 0;
        }
    }

    //---------------------------------------------------------------------------------------------

    function pUpdateItemSize()
    {
        if (pWidth == itemPreferredWidth)
        {
            var width  = repeater.width  - marginsMinSize - (marginsMinSize * countHorizontal);
            var height = repeater.height - marginsMinSize - (marginsMinSize * countVertical);

            width  = Math.round(width  / countHorizontal);
            height = Math.round(height / countVertical);

            var ratioWidth  = (width  - itemExtraWidth)  / pWidth;
            var ratioHeight = (height - itemExtraHeight) / pHeight;

            if (ratioWidth < ratioHeight)
            {
                itemWidth = width;

                itemHeight = Math.round((width - itemExtraWidth) * itemRatioHeight
                                        + itemExtraHeight);
            }
            else
            {
                itemWidth = Math.round((height - itemExtraHeight) * itemRatioWidth
                                       + itemExtraWidth);

                itemHeight = height;
            }
        }
        else
        {
            itemWidth  = pWidth  + itemExtraWidth;
            itemHeight = pHeight + itemExtraHeight;
        }

        marginsHorizontal = (repeater.width - (itemWidth * countHorizontal))
                            / (countHorizontal + 1);

        marginsVertical = (repeater.height - (itemHeight * countVertical))
                          / (countVertical + 1);
    }

    //---------------------------------------------------------------------------------------------

    function pUpdateActive()
    {
        var active;

        if (isActive && visible)
        {
             active = true;
        }
        else active = false;

        if (pActive == active) return;

        if (active == false)
        {
            isChangingPage = false;

            isAnimated = false;

            updateTimer.stop();
        }
        else updateView();

        pActive = active;
    }

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

    Timer
    {
        id: animationTimer

        interval: durationAnimation

        onTriggered:
        {
            isChangingPage = false;

            onAnimationTimeout();
        }
    }

    Timer
    {
        id: updateTimer

        interval: st.baseWall_intervalUpdate

        onTriggered: onUpdateTimeout()
    }

    Timer
    {
        id: fixedSizeTimer

        onTriggered: onFixedSizeTimeout()
    }

    Repeater
    {
        id: repeater

        anchors.fill: parent
    }

    Item
    {
        id: bar

        anchors.bottom: parent.bottom

        anchors.horizontalCenter: parent.horizontalCenter

        width : backward.width + forward.width
        height: st.dp26

        z: 1

        opacity: (pageCount > 1)

        visible: (pActive && enableAnimation)

        Behavior on opacity
        {
            enabled: bar.visible

            PropertyAnimation
            {
                duration: durationAnimation

                easing.type: st.easing
            }
        }

        ButtonPianoIcon
        {
            id: backward

            anchors.top   : parent.top
            anchors.bottom: parent.bottom

            width: st.dp28 + borderSizeWidth

            borderTop : borderSize
            borderLeft: borderSize

            enabled: (currentPage > 0)

            icon          : st.icon8x8_previous
            iconSourceSize: st.size8x8

            cursor: Qt.PointingHandCursor

            onClicked: setPage(currentPage - 1)
        }

        ButtonPianoIcon
        {
            id: forward

            anchors.left  : backward.right
            anchors.top   : parent.top
            anchors.bottom: parent.bottom

            width: st.dp28 + borderSizeWidth

            borderTop: borderSize

            enabled: (currentPage < (pageCount - 1))

            icon          : st.icon8x8_next
            iconSourceSize: st.size8x8

            cursor: Qt.PointingHandCursor

            onClicked: setPage(currentPage + 1)
        }
    }
}
