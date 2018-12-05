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

import QtQuick 1.0
import Sky     1.0

Item
{
    id: itemWipe

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    /* read */ property bool isAnimated: false

    property int borderSize: st.border_sizeFocus

    //---------------------------------------------------------------------------------------------
    // Style

    property int durationAnimation: st.duration_normal

    //---------------------------------------------------------------------------------------------
    // Private

    property bool pReady: false

    property int pDirectionA: -1
    property int pDirectionB: -1

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    default property alias content: content.data

    //---------------------------------------------------------------------------------------------
    // Style

    property alias color: border.color

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function startLeft () { pStart(Sk.Left);  }
    function startRight() { pStart(Sk.Right); }
    function startUp   () { pStart(Sk.Up);    }
    function startDown () { pStart(Sk.Down);  }

    //---------------------------------------------------------------------------------------------

    function init()
    {
        if (isAnimated) return;

        itemImage.setItemShot(itemWipe);

        pReady = true;
    }

    //---------------------------------------------------------------------------------------------
    // Private

    function pStart(direction)
    {
        if (isAnimated) return;

        if (pReady == false) init();

        clip = true;

        itemImage.width  = width;
        itemImage.height = height;

        pDirectionA = direction;
        pDirectionB = direction;

        isAnimated = st.animate;

        pDirectionA = -1;
    }

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    Item
    {
        id: backgroundA

        anchors.left  : parent.left
        anchors.right : parent.right
        anchors.top   : parent.top
        anchors.bottom: parent.bottom

        clip: itemWipe.clip

        states:
        [
            State
            {
                name: "left"; when: (pDirectionB == Sk.Left)

                AnchorChanges
                {
                    target: backgroundA

                    anchors.left: border.left
                }
            },
            State
            {
                name: "right"; when: (pDirectionB == Sk.Right)

                AnchorChanges
                {
                    target: backgroundA

                    anchors.right: border.right
                }
            },
            State
            {
                name: "up"; when: (pDirectionB == Sk.Up)

                AnchorChanges
                {
                    target: backgroundA

                    anchors.top: border.top
                }
            },
            State
            {
                name: "down"; when: (pDirectionB == Sk.Down)

                AnchorChanges
                {
                    target: backgroundA

                    anchors.bottom: border.bottom
                }
            }
        ]

        Item
        {
            id: content

            width : itemWipe.width
            height: itemWipe.height

            x: -(backgroundA.x)
            y: -(backgroundA.y)
        }
    }

    Item
    {
        id: backgroundB

        anchors.left  : parent.left
        anchors.right : parent.right
        anchors.top   : parent.top
        anchors.bottom: parent.bottom

        visible: itemWipe.clip

        clip: visible

        states:
        [
            State
            {
                name: "left"; when: (pDirectionB == Sk.Left)

                AnchorChanges
                {
                    target: backgroundB

                    anchors.right: border.left
                }
            },
            State
            {
                name: "right"; when: (pDirectionB == Sk.Right)

                AnchorChanges
                {
                    target: backgroundB

                    anchors.left: border.right
                }
            },
            State
            {
                name: "up"; when: (pDirectionB == Sk.Up)

                AnchorChanges
                {
                    target: backgroundB

                    anchors.bottom: border.top
                }
            },
            State
            {
                name: "down"; when: (pDirectionB == Sk.Down)

                AnchorChanges
                {
                    target: backgroundB

                    anchors.top: border.bottom
                }
            }
        ]

        Image
        {
            id: itemImage

            x: -(backgroundB.x)
            y: -(backgroundB.y)
        }
    }

    RectangleBorders
    {
        id: borders

        anchors.fill: parent

        size: borderSize

        visible: (opacity != 0.0)

        opacity: (itemWipe.clip) ? 1.0 : 0.0

        color: border.color

        Behavior on opacity
        {
            NumberAnimation
            {
                duration: durationAnimation / 2

                easing.type: st.easing
            }
        }
    }

    Rectangle
    {
        id: border

        anchors.margins: borderSize

        width: (pDirectionB == Sk.Left || pDirectionB == Sk.Right) ? borderSize : 0

        height: (pDirectionB == Sk.Up || pDirectionB == Sk.Down) ? borderSize : 0

        visible: itemWipe.clip

        opacity: borders.opacity

        color: st.border_colorFocus

        states:
        [
            State
            {
                name: "leftB"; when: (pDirectionA == -1 && pDirectionB == Sk.Left)

                AnchorChanges
                {
                    target: border

                    anchors.right : parent.left
                    anchors.top   : parent.top
                    anchors.bottom: parent.bottom
                }
            },
            State
            {
                name: "rightB"; when: (pDirectionA == -1 && pDirectionB == Sk.Right)

                AnchorChanges
                {
                    target: border

                    anchors.left  : parent.right
                    anchors.top   : parent.top
                    anchors.bottom: parent.bottom
                }
            },
            State
            {
                name: "upB"; when: (pDirectionA == -1 && pDirectionB == Sk.Up)

                AnchorChanges
                {
                    target: border

                    anchors.left  : parent.left
                    anchors.right : parent.right
                    anchors.bottom: parent.top
                }
            },
            State
            {
                name: "downB"; when: (pDirectionA == -1 && pDirectionB == Sk.Down)

                AnchorChanges
                {
                    target: border

                    anchors.left : parent.left
                    anchors.right: parent.right
                    anchors.top  : parent.bottom
                }
            },
            State
            {
                name: "leftA"; when: (pDirectionB == Sk.Left)

                AnchorChanges
                {
                    target: border

                    anchors.left  : parent.right
                    anchors.top   : parent.top
                    anchors.bottom: parent.bottom
                }
            },
            State
            {
                name: "rightA"; when: (pDirectionB == Sk.Right)

                AnchorChanges
                {
                    target: border

                    anchors.right : parent.left
                    anchors.top   : parent.top
                    anchors.bottom: parent.bottom
                }
            },
            State
            {
                name: "upA"; when: (pDirectionB == Sk.Up)

                AnchorChanges
                {
                    target: border

                    anchors.left : parent.left
                    anchors.right: parent.right
                    anchors.top  : parent.bottom
                }
            },
            State
            {
                name: "downA"; when: (pDirectionB == Sk.Down)

                AnchorChanges
                {
                    target: border

                    anchors.left  : parent.left
                    anchors.right : parent.right
                    anchors.bottom: parent.top
                }
            }
        ]

        transitions: Transition
        {
            SequentialAnimation
            {
                AnchorAnimation
                {
                    duration: (isAnimated) ? durationAnimation : 0

                    easing.type: st.easing
                }

                ScriptAction
                {
                    script:
                    {
                        if (pDirectionA != -1 || pDirectionB == -1) return;

                        itemWipe.clip = false;

                        pReady = false;

                        pDirectionB = -1;

                        isAnimated = false;
                    }
                }
            }
        }
    }
}
