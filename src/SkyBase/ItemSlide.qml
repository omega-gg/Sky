//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkyBase.

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
    id: itemSlide

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    /* read */ property bool isAnimated: false

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

        itemImage.setItemShot(content);

        //itemImage.width  = width;
        //itemImage.height = height;

        pReady = true;
    }

    //---------------------------------------------------------------------------------------------
    // Private

    function pStart(direction)
    {
        if (isAnimated) return;

        if (pReady == false) init();

        clip = true;

        itemImage.visible = true;

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
        id: content

        width : parent.width
        height: parent.height

        states:
        [
            State
            {
                name: "left"; when: (pDirectionA == Sk.Left)

                AnchorChanges
                {
                    target: content

                    anchors.left: parent.right
                }
            },
            State
            {
                name: "right"; when: (pDirectionA == Sk.Right)

                AnchorChanges
                {
                    target: content

                    anchors.right: parent.left
                }
            },
            State
            {
                name: "up"; when: (pDirectionA == Sk.Up)

                AnchorChanges
                {
                    target: content

                    anchors.top: parent.bottom
                }
            },
            State
            {
                name: "down"; when: (pDirectionA == Sk.Down)

                AnchorChanges
                {
                    target: content

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
                        if (pDirectionA != -1) return;

                        itemImage.visible = false;

                        itemSlide.clip = false;

                        pReady = false;

                        pDirectionB = -1;

                        isAnimated = false;
                    }
                }
            }
        }
    }

    Image
    {
        id: itemImage

        visible: false

        states:
        [
            State
            {
                name: "left"; when: (pDirectionB == Sk.Left)

                AnchorChanges
                {
                    target: itemImage

                    anchors.right: content.left
                }
            },
            State
            {
                name: "right"; when: (pDirectionB == Sk.Right)

                AnchorChanges
                {
                    target: itemImage

                    anchors.left: content.right
                }
            },
            State
            {
                name: "up"; when: (pDirectionB == Sk.Up)

                AnchorChanges
                {
                    target: itemImage

                    anchors.bottom: content.top
                }
            },
            State
            {
                name: "down"; when: (pDirectionB == Sk.Down)

                AnchorChanges
                {
                    target: itemImage

                    anchors.top: content.bottom
                }
            }
        ]
    }
}
