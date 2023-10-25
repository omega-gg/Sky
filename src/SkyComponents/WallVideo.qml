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

WallBookmarkTrack
{
    id: wall

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    /* read */ property bool isExposed: false

    property int loaderHeight: player.height / 128

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

    property int pWidthHalf: (width - st.border_size) / 2

    property int pWidthRight: width - pWidthHalf - st.border_size

    property int pAreaWidth: player.width / 8

    property int pMargin: 0

    property variant pCurrentTab    : null
    property variant pHighlightedTab: null

    property variant pFillMode: player.fillMode

    property variant pPlayerTab: player.tab

    property variant pItemA: null
    property variant pItemB: null

    property TabTrack pHoveredTab: tabs.tabAt(indexHover)

    property bool pExpanded: true

    property bool pButtonsVisible: (isExposed && indexRemove == -1 && indexDrag < 0)

    property bool pAudio: (player.output == AbstractBackend.OutputAudio
                           ||
                           player.outputActive == AbstractBackend.OutputAudio
                           ||
                           player.isAudio || player.hasOutput)

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias loading: playerBrowser.loading

    property alias enableTitle : playerBrowser.enableTitle
    property alias enableAuthor: playerBrowser.enableAuthor

    //---------------------------------------------------------------------------------------------

    property alias player          : player
    property alias playerMouseArea : playerMouseArea
    property alias playerBackground: playerBackground
    property alias playerBrowser   : playerBrowser

    property alias itemText: itemText

    property alias itemLoader: itemLoader

    property alias scanner: scanner

    property alias buttonsItem: buttonsItem

    //---------------------------------------------------------------------------------------------
    // Signals
    //---------------------------------------------------------------------------------------------

    signal titleClicked (variant mouse)
    signal authorClicked(variant mouse)

    signal playerPressed      (variant mouse)
    signal playerDoubleClicked(variant mouse)

    signal tagClicked(string text)

    signal contextualBrowser

    signal subtitleLoaded(bool ok)

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

    // NOTE: We need to check tags at the root and in playerMouseArea.
    onClicked: if (scanner.visible) scanner.click()

    //---------------------------------------------------------------------------------------------
    // Connections
    //---------------------------------------------------------------------------------------------

    Connections
    {
        target: tabs

        /* QML_CONNECTION */ function onCurrentTabChanged()
        {
            pUpdatePlayerItems(isExposed);

            pUpdateSplit();
        }

        /* QML_CONNECTION */ function onHighlightedTabChanged()
        {
            pUpdatePlayerItems(isExposed);

            pCheckSplit();
        }

        /* QML_CONNECTION */ function onTabsMoved() { pUpdateSplit(); }
    }

    Connections
    {
        target: (itemText.visible) ? player : null

        /* QML_CONNECTION */ function onWidthChanged () { pUpdateText(); }
        /* QML_CONNECTION */ function onHeightChanged() { pUpdateText(); }

        /* QML_CONNECTION */ function onFillModeChanged() { pUpdateText(); }
    }

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function expose()
    {
        if (isExposed) return;

        pUpdatePlayerItems(true);

        pItemA = player       .item;
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

        pItemA = player       .item;
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

    function seekBackward()
    {
        player.seek(player.currentTime - st.wallVideo_interval);
    }

    function seekForward()
    {
        player.seek(player.currentTime + st.wallVideo_interval);
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

    function pUpdatePlayerItems(isExposed)
    {
        if (isExposed)
        {
            if (tabs.highlightedTab)
            {
                player       .item = getHighlightedItem();
                playerBrowser.item = getCurrentItem    ();
            }
            else
            {
                player       .item = getCurrentItem();
                playerBrowser.item = player.item;
            }
        }
        else
        {
            player       .item = null;
            playerBrowser.item = null;
        }
    }

    //---------------------------------------------------------------------------------------------

    function pUpdateSplit()
    {
        if (tabs.highlightedTab == null || tabs.currentIndex == tabs.highlightedIndex) return;

        pCurrentTab = tabs.currentTab;

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

            pCurrentTab     = tabs.currentTab;
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
                    player.updateHighlightedTab();

                    var videoShot = pHighlightedTab.videoShot;

                    if (videoShot == "")
                    {
                        itemShot.source = pHighlightedTab.cover;
                    }
                    else itemShot.loadSource(videoShot, true);

                    player.posB = true;
                }
                else itemShot.source = pCurrentTab.cover;

                if (player.x == 0)
                {
                    rectangleShot.anchors.left = player.right;

                    rectangleShot.anchors.leftMargin = st.border_size;
                }
                else
                {
                    rectangleShot.anchors.right = player.left;

                    rectangleShot.anchors.rightMargin = st.border_size;
                }

                rectangleShot.visible = true;

                splitTimer.start();
            }

            playerBrowser.posB = false;

            split = -1;

            player.posB = false;

            pCurrentTab     = null;
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
        rectangleShot.anchors.rightMargin = st.border_size;
    }

    //---------------------------------------------------------------------------------------------

    function pUpdateText()
    {
        pMargin = itemText.applySize(player, playerCover);
    }

    //---------------------------------------------------------------------------------------------

    function pGetItemActive()
    {
        if      (itemContextual) return itemContextual;
        else if (itemHovered)    return itemHovered;
        else                     return null;
    }

    //---------------------------------------------------------------------------------------------
    // Children
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

        gradient: Gradient
        {
            GradientStop
            {
                position: 0.0

                color: (itemShot.isSourceDefault) ? defaultColorA
                                                  : st.wallVideo_colorPlayer
            }

            GradientStop
            {
                position: 1.0

                color: (itemShot.isSourceDefault) ? defaultColorB
                                                  : st.wallVideo_colorPlayer
            }
        }

        ImageScale
        {
            id: itemShot

            anchors.fill: parent

            anchors.leftMargin: (isSourceDefault) ? logoMargin : 0

            anchors.rightMargin: anchors.leftMargin

            clip: (fillMode == Image.PreserveAspectCrop)

            sourceDefault: logo

            fillMode: (isSourceDefault) ? Image.PreserveAspectFit
                                        : pFillMode

            cache: false

            scaling: isSourceDefault
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

        cursor: Qt.PointingHandCursor

        /* QML_EVENT */ onPressed: function(mouse) { playerPressed(mouse); }

        /* QML_EVENT */ onDoubleClicked: function(mouse) { playerDoubleClicked(mouse); }

        // NOTE: We need to check tags at the root and in playerMouseArea.
        onClicked: if (scanner.visible) scanner.click()
    }

    Rectangle
    {
        id: playerBackground

        anchors.fill: player

        z: player.z

        visible: player.visible

        color: st.wallVideo_colorPlayer
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
            else return (hasStarted && pAudio == false);
        }

        // NOTE: When the wall is exposed we make sure to clip the player properly.
        clip: (isActive && fillMode == AbstractBackend.PreserveAspectCrop)

        tabs: wall.tabs

        volume: 0.0

        pauseTimeout: 600000 // 10 minutes

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
                }
            }
        ]

        transitions: Transition
        {
            NumberAnimation
            {
                properties: "x, y, width, height"

                duration: (enableAnimation) ? st.duration_normal : 0

                easing.type: st.easing
            }
        }
    }

    Rectangle
    {
        id: playerBack

        anchors.fill: player

        z: player.z

        visible: (player.isStarting || player.isResuming || (player.visible && pAudio))

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

            clip: (fillMode == Image.PreserveAspectCrop)

            source:
            {
                if (pAudio)
                {
                    if (pHighlightedTab)
                    {
                         return pHighlightedTab.cover;
                    }
                    else return pPlayerTab.cover;
                }
                else return pPlayerTab.coverShot;
            }

            sourceDefault: logo

            fillMode: (isSourceDefault) ? Image.PreserveAspectFit
                                        : pFillMode

            cache: false

            scaling: isSourceDefault

            onVisibleChanged: if (visible) applyScale()
        }
    }

    TextSubtitle
    {
        id: itemText

        anchors.left  : player.left
        anchors.right : player.right
        anchors.bottom: player.bottom

        anchors.leftMargin  : st.dp8
        anchors.rightMargin : st.dp8
        anchors.bottomMargin: pMargin

        z: player.z

        visible: (player.visible && player.isLoading == false && player.isPlaying)

        source: player.subtitle

        currentTime: player.currentTime

        /* QML_EVENT */ onLoaded: function(ok) { subtitleLoaded(ok); }

        onVisibleChanged: pUpdateText()
    }

    AnimatedLoader
    {
        id: itemLoader

        anchors.left  : player.left
        anchors.right : player.right
        anchors.bottom: player.bottom

        height: loaderHeight

        z: player.z

        opacity: (player.isPlaying && player.isLoading)

        // NOTE: When the wall is exposed we make sure to clip the loader properly.
        clip: isActive
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

            clip: (fillMode == Image.PreserveAspectCrop)

            source: (currentTab) ? currentTab.coverShot : ""

            sourceDefault: logo

            fillMode: (isSourceDefault) ? Image.PreserveAspectFit
                                        : pFillMode

            asynchronous: wall.asynchronous

            cache: false

            scaling: isSourceDefault

            onVisibleChanged: if (visible) loadNow()
        }
    }

    ItemScan
    {
        id: scanner

        anchors.fill: playerBrowser

        z: playerBrowser.z

        visible: (player.visible || playerBack.visible || browserBack.visible)

        player: player

        cover: (playerCover.visible) ? playerCover
                                     : browserCover

        /* QML_EVENT */ onClicked: function(text) { tagClicked(text) }
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
                if (item && item.visible
                    &&
                    (player.isPlaying == false || player.item != item))
                {
                     return true;
                }
                else return false;
            }
            else if (split != -1 || player.isPlaying == false)
            {
                 return true;
            }
            else return false;
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

                    x: pWidthHalf + st.border_size
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

                easing.type: st.easing
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

                easing.type: st.easing
            }
        }

        //-----------------------------------------------------------------------------------------
        // Events
        //-----------------------------------------------------------------------------------------

        /* QML_EVENT */ onTitleClicked : function(mouse) { wall.titleClicked (mouse); }
        /* QML_EVENT */ onAuthorClicked: function(mouse) { wall.authorClicked(mouse); }

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

        anchors.rightMargin : borderRight
        anchors.bottomMargin: borderBottom

        width : st.wallVideo_overlayHeight
        height: st.wallVideo_overlayHeight

        borderLeft  : borderSize
        borderTop   : borderSize
        borderBottom: borderSize

        z: buttonsItem.z

        visible: buttonsItem.visible

        icon          : st.icon20x20_extend
        iconSourceSize: st.size20x20

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

        source    : st.icon16x16_play
        sourceSize: st.size16x16
    }
}
