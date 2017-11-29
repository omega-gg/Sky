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

WallBookmarkTrack
{
    id: wall

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    /* read */ property bool isExposed: false

    property int logoMargin: browserBack.width / logoRatio

    property bool enablePress: false

    /* read */ property int split: -1

    /* read */ property variant itemActive: pGetItemActive()

    //---------------------------------------------------------------------------------------------
    // Style

    property color defaultColorA: st.logo_colorA
    property color defaultColorB: st.logo_colorB

    //---------------------------------------------------------------------------------------------
    // Private

    property int pWidthHalf : (width - st.dp2) / 2
    property int pWidthRight: width - pWidthHalf - st.dp2

    property variant pHighlightedTab: null

    property variant pItemA: null
    property variant pItemB: null

    property TabTrack pHoveredTab: tabs.tabAt(indexHover)

    property bool pExpanded: true

    property bool pButtonsVisible: (isExposed && indexRemove == -1 && indexDrag < 0)

    property bool pAudio: (player.output == AbstractBackend.OutputAudio
                           ||
                           player.outputActive == AbstractBackend.OutputAudio
                           ||
                           player.isAudio)

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias loading: playerBrowser.loading

    property alias enableTitle : playerBrowser.enableTitle
    property alias enableAuthor: playerBrowser.enableAuthor

    //---------------------------------------------------------------------------------------------

    property alias player         : player
    property alias playerBrowser  : playerBrowser
    property alias playerMouseArea: playerMouseArea

    property alias buttonsItem: buttonsItem

    //---------------------------------------------------------------------------------------------
    // Signals
    //---------------------------------------------------------------------------------------------

    signal titleClicked (variant mouse)
    signal authorClicked(variant mouse)

    signal playerPressed(variant mouse)

    signal contextual
    signal contextualBrowser

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    isActive: false

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    Component.onCompleted:
    {
        isActive = isExposed;

        if (isActive) pExpanded = false;
    }

    //---------------------------------------------------------------------------------------------
    // Connections
    //---------------------------------------------------------------------------------------------

    Connections
    {
        target: tabs

        onCurrentTabChanged:
        {
            pUpdatePlayerItems(isExposed);

            pUpdateSplit();
        }

        onHighlightedTabChanged:
        {
            pUpdatePlayerItems(isExposed);

            pCheckSplit();
        }

        onTabsMoved: pUpdateSplit()
    }

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function expose()
    {
        if (isExposed) return;

        pUpdatePlayerItems(true);

        pItemA = player.item;
        pItemB = playerBrowser.item;

        isActive  = true;
        isExposed = true;

        if (pItemA) pItemA.itemImage.visible = false;
        if (pItemB) pItemB.itemImage.visible = false;

        hideTimer.restart();
    }

    function restore()
    {
        if (isExposed == false) return;

        clearDrag();

        isExposed = false;

        pItemA = player.item;
        pItemB = playerBrowser.item;

        pUpdatePlayerItems(false);

        pExpanded = true;

        if (pItemA) pItemA.itemImage.visible = false;
        if (pItemB) pItemB.itemImage.visible = false;

        hideTimer.restart();
    }

    function toggleExpose()
    {
        if (isExposed) restore();
        else           expose ();
    }

    //---------------------------------------------------------------------------------------------
    // WallBookmarkTrack reimplementation

    function getItemBarMargin(index)
    {
        if (pButtonsVisible && indexActive == index)
        {
             return st.dp40;
        }
        else return st.dp8;
    }

    //---------------------------------------------------------------------------------------------
    // Private

    function pGetItemActive()
    {
        if      (itemContextual) return itemContextual;
        else if (itemHovered)    return itemHovered;
        else                     return null;
    }

    //---------------------------------------------------------------------------------------------

    function pUpdatePlayerItems(isExposed)
    {
        if (isExposed)
        {
            if (tabs.highlightedTab)
            {
                player.item        = getHighlightedItem();
                playerBrowser.item = getCurrentItem    ();
            }
            else
            {
                player.item        = getCurrentItem();
                playerBrowser.item = player.item;
            }
        }
        else
        {
            player.item        = null;
            playerBrowser.item = null;
        }
    }

    //---------------------------------------------------------------------------------------------

    function pUpdateSplit()
    {
        if (tabs.highlightedTab == null || tabs.currentIndex == tabs.highlightedIndex) return;

        if (tabs.currentIndex > tabs.highlightedIndex)
        {
             split = 0;
        }
        else split = 1;
    }

    function pCheckSplit()
    {
        if (tabs.highlightedTab)
        {
            splitTimer.stop();

            pResetShot();

            pHighlightedTab = tabs.highlightedTab;

            playerBrowser.posB = true;

            if (tabs.currentIndex > tabs.highlightedIndex)
            {
                 split = 0;
            }
            else split = 1;
        }
        else if (split != -1)
        {
            if (isExposed == false)
            {
                if (pHighlightedTab != tabs.currentTab)
                {
                    var videoShot = pHighlightedTab.videoShot;

                    if (videoShot == "")
                    {
                        itemShot.setItemShot(playerBack);
                    }
                    else itemShot.loadSource(videoShot, true);

                    player.posB = true;
                }
                else if (browserCover.isSourceDefault)
                {
                     itemShot.setItemShot(browserBack);
                }
                else itemShot.setItemShot(browserCover);

                if (player.x == 0)
                {
                    rectangleShot.anchors.left = player.right;

                    rectangleShot.anchors.leftMargin = st.dp2;
                }
                else
                {
                    rectangleShot.anchors.right = player.left;

                    rectangleShot.anchors.rightMargin = st.dp2;
                }

                rectangleShot.visible = true;

                splitTimer.start();
            }

            playerBrowser.posB = false;

            split = -1;

            player.posB = false;

            pHighlightedTab = null;
        }
    }

    //---------------------------------------------------------------------------------------------

    function pResetShot()
    {
        rectangleShot.visible = false;

        rectangleShot.anchors.left  = undefined;
        rectangleShot.anchors.right = undefined;

        rectangleShot.anchors.leftMargin  = 0;
        rectangleShot.anchors.rightMargin = st.dp2;
    }

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    Timer
    {
        id: hideTimer

        interval: st.duration_normal

        onTriggered:
        {
            if (isExposed)
            {
                pExpanded = false;
            }
            else isActive = false;

            if (pItemA) pItemA.itemImage.visible = true;
            if (pItemB) pItemB.itemImage.visible = true;
        }
    }

    Timer
    {
        id: splitTimer

        interval: st.duration_normal

        onTriggered: pResetShot()
    }

    Rectangle
    {
        id: rectangleShot

        width : pWidthHalf
        height: parent.height

        visible: false

        color: st.wallVideo_colorPlayer

        Image
        {
            id: itemShot

            anchors.fill: parent

            fillMode: Image.PreserveAspectFit

            cache: false
        }
    }

    MouseArea
    {
        id: playerMouseArea

        anchors.fill: player

        z: player.z

        visible: enablePress

        hoverEnabled: true
        hoverRetain : true

        cursor: MouseArea.PointingHandCursor

        onPressed: playerPressed(mouse)
    }

    Player
    {
        id: player

        //-----------------------------------------------------------------------------------------
        // Properties
        //-----------------------------------------------------------------------------------------

        property variant item: null

        property bool posB: false

        //-----------------------------------------------------------------------------------------
        // Settings
        //-----------------------------------------------------------------------------------------

        width : parent.width
        height: parent.height

        z: 3

        visible:
        {
            if (isExposed && (item == null || item.visible == false))
            {
                if (split == -1)
                {
                     return false;
                }
                else return true;
            }
            else return hasStarted;
        }

        backend: BackendVlc {}

        tabs: wall.tabs

        volume: 0.0

        //-----------------------------------------------------------------------------------------
        // States
        //-----------------------------------------------------------------------------------------

        states:
        [
            State
            {
                name: "exposed"; when: isExposed

                PropertyChanges
                {
                    target: player

                    width: (item) ? item.itemImage.width
                                    + item.itemImage.anchors.leftMargin
                                    + item.itemImage.anchors.rightMargin : parent.width

                    height: (item) ? item.itemImage.height
                                     + item.itemImage.anchors.topMargin
                                     + item.itemImage.anchors.bottomMargin : parent.height

                    x: (item) ? item.x + item.borderLeft : 0
                    y: (item) ? item.y + item.borderTop  : 0

                    z: (item) ? item.z : 0
                }
            },
            State
            {
                name: "splittedLeft"; when: (split == 0)

                PropertyChanges
                {
                    target: player

                    width: pWidthHalf

                    x: (posB) ? playerBrowser.x : 0

                    z: 4
                }
            },
            State
            {
                name: "splittedRight"; when: (split == 1)

                PropertyChanges
                {
                    target: player

                    width: pWidthRight

                    x: (posB) ? playerBrowser.x
                              : pWidthHalf + 2

                    z: 4
                }
            }
        ]

        transitions: Transition
        {
            NumberAnimation
            {
                properties: "x, y, width, height"

                duration: (enableAnimation) ? st.duration_normal : 0
            }
        }
    }

    Rectangle
    {
        id: playerBack

        anchors.fill: player

        z: player.z

        visible: (player.isResuming || (player.visible && pAudio))

        gradient: Gradient
        {
            GradientStop
            {
                position: 0.0

                color: (playerCover.isSourceDefault) ? defaultColorA
                                                     : st.wallVideo_colorPlayer
            }

            GradientStop
            {
                position: 1.0

                color: (playerCover.isSourceDefault) ? defaultColorB
                                                     : st.wallVideo_colorPlayer
            }
        }

        ImageScale
        {
            id: playerCover

            anchors.fill: parent

            anchors.leftMargin: (isSourceDefault) ? logoMargin : 0

            anchors.rightMargin: anchors.leftMargin

            visible: (isSourceDefault == false || pAudio)

            source:
            {
                if (pAudio)
                {
                    if (pHighlightedTab)
                    {
                         return pHighlightedTab.cover;
                    }
                    else return playerTab.cover;
                }
                else return playerTab.videoShot;
            }

            sourceDefault: logo

            fillMode: Image.PreserveAspectFit

            cache: false

            scaling: isSourceDefault

            onVisibleChanged: if (visible) applyScale()
        }
    }

    LabelLoading
    {
        anchors.centerIn: player

        width : st.dp64
        height: st.dp64

        z: player.z

        visible: (player.isPlaying && player.isLoading)

        icon          : st.icon_loading
        iconSourceSize: st.size48x48

        background.opacity: st.wallVideo_opacityLoader
    }

    Rectangle
    {
        id: browserBack

        anchors.fill: playerBrowser

        z: playerBrowser.z

        visible: (pExpanded && (player.visible == false || split != -1))

        gradient: Gradient
        {
            GradientStop
            {
                position: 0.0

                color: (browserCover.isSourceDefault) ? defaultColorA
                                                      : st.wallVideo_colorPlayer
            }

            GradientStop
            {
                position: 1.0

                color: (browserCover.isSourceDefault) ? defaultColorB
                                                      : st.wallVideo_colorPlayer
            }
        }

        ImageScale
        {
            id: browserCover

            anchors.fill: parent

            anchors.leftMargin: (isSourceDefault) ? logoMargin : 0

            anchors.rightMargin: anchors.leftMargin

            source: (currentTab) ? currentTab.coverShot : ""

            sourceDefault: logo

            loadMode: (pExpanded) ? Image.LoadVisible
                                  : Image.LoadAlways

            fillMode: Image.PreserveAspectFit

            asynchronous: wall.asynchronous

            cache: false

            scaling: isSourceDefault
        }
    }

    PlayerBrowser
    {
        id: playerBrowser

        //-----------------------------------------------------------------------------------------
        // Properties
        //-----------------------------------------------------------------------------------------

        property variant item: null

        property bool posB: false

        //-----------------------------------------------------------------------------------------
        // Settings
        //-----------------------------------------------------------------------------------------

        width: (posB) ? pWidthHalf
                      : parent.width

        height: parent.height

        x:
        {
            if (posB)
            {
                if (tabs.currentIndex > tabs.highlightedIndex)
                {
                    return parent.width;
                }
                else return -pWidthHalf;
            }
            else return 0;
        }

        y: parent.y

        z: 3

        visible:
        {
            if (isExposed)
            {
                if (item == null || item.visible == false)
                {
                     return false;
                }
                else return (player.isPlaying == false || item != player.item);
            }
            else if (split != -1)
            {
                 return true;
            }
            else return (player.isPlaying == false);
        }

        opacity: (visible)

        acceptedButtons: (isExposed) ? Qt.NoButton
                                     : Qt.RightButton

        player: player

        tab: currentTab

        barTitle.visible: (isActive == false && buttonPlay.visible && title)

        barDetails.visible: (barTitle.visible && tab && tab.isLoaded)

        barTitle  .opacity: (isActive == false)
        barDetails.opacity: (isActive == false && barDetails.visible)

        areaForward.anchors.top: barTitle.bottom

        //-----------------------------------------------------------------------------------------
        // States
        //-----------------------------------------------------------------------------------------

        states:
        [
            State
            {
                name: "exposed"; when: isExposed

                PropertyChanges
                {
                    target: playerBrowser

                    width: (item) ? item.itemImage.width
                                    + item.itemImage.anchors.leftMargin
                                    + item.itemImage.anchors.rightMargin : 0

                    height: (item) ? item.itemImage.height
                                     + item.itemImage.anchors.topMargin
                                     + item.itemImage.anchors.bottomMargin : 0

                    x: (item) ? item.x + item.borderLeft : 0
                    y: (item) ? item.y + item.borderTop  : 0

                    z: (item) ? item.z : 0
                }
            },
            State
            {
                name: "splittedRight"; when: (split == 0)

                PropertyChanges
                {
                    target: playerBrowser

                    width: pWidthRight

                    x: pWidthHalf + st.dp2
                }
            },
            State
            {
                name: "splittedLeft"; when: (split == 1)

                PropertyChanges
                {
                    target: playerBrowser

                    width: pWidthHalf

                    x: 0
                }
            }
        ]

        //-----------------------------------------------------------------------------------------
        // Transitions
        //-----------------------------------------------------------------------------------------

        transitions: Transition
        {
            NumberAnimation
            {
                properties: "x, y, width, height"

                duration: (enableAnimation) ? st.duration_normal : 0
            }
        }

        //-----------------------------------------------------------------------------------------
        // Animations
        //-----------------------------------------------------------------------------------------

        Behavior on opacity
        {
            PropertyAnimation
            {
                duration: (visible) ? st.duration_fast : 0
            }
        }

        //-----------------------------------------------------------------------------------------
        // Events
        //-----------------------------------------------------------------------------------------

        onTitleClicked : wall.titleClicked (mouse)
        onAuthorClicked: wall.authorClicked(mouse)

        onContextual: contextualBrowser()
    }

    ButtonsItem
    {
        id: buttonsItem

        anchors.right: (itemActive) ? itemActive.right : undefined
        anchors.top  : (itemActive) ? itemActive.top   : undefined

        anchors.rightMargin: st.dp4
        anchors.topMargin  : st.dp4

        z: (itemActive) ? itemActive.z : 0

        visible: (pButtonsVisible && itemActive != null && itemActive.visible)

        checked: (indexContextual != -1)

        buttonClose.enabled: (count > 1 || currentTab.isValid)

        onContextual: wall.contextual()

        onClose: closeItem(indexHover)
    }

    ButtonPianoIcon
    {
        anchors.right : (itemActive) ? itemActive.right  : undefined
        anchors.bottom: (itemActive) ? itemActive.bottom : undefined

        anchors.rightMargin : st.dp2
        anchors.bottomMargin: st.dp2

        width : st.wallVideo_overlayHeight
        height: st.wallVideo_overlayHeight

        borderLeft  : borderSize
        borderTop   : borderSize
        borderBottom: borderSize

        z: buttonsItem.z

        visible: buttonsItem.visible

        icon          : st.icon24x24_extend
        iconSourceSize: st.size24x24

        onClicked:
        {
            selectItem(indexHover);

            restore();
        }
    }

    IconOverlay
    {
        width : st.wallVideo_overlayWidth
        height: st.wallVideo_overlayHeight

        x: (itemHovered) ? itemHovered.x + itemHovered.borderLeft + itemHovered.itemBar.x : 0
        y: (itemHovered) ? itemHovered.y + itemHovered.borderTop  + itemHovered.itemBar.y : 0

        z: buttonsItem.z

        visible: (buttonsItem.visible && player.isPlaying
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
