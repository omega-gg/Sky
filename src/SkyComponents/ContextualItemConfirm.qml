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
    id: contextualItemConfirm

    //---------------------------------------------------------------------------------------------
    // Properties private
    //---------------------------------------------------------------------------------------------

    property bool pCurrent: (isCurrent || mouseArea.pressed)

    property int pIconWidth: Math.max(itemIcon.width, background.height)

    property bool pConfirm: false

    property bool pAutoClear: true

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    anchors.left : parent.left
    anchors.right: parent.right

    height: st.contextualItemConfirm_height

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function press() { mouseArea.press(); }
    function click() { mouseArea.click(); }

    //---------------------------------------------------------------------------------------------
    // Private

    function pSelectPrevious()
    {
        pConfirm = false;

        setCurrentId(id);

        selectPrevious();
    }

    function pSelectNext()
    {
        pConfirm = false;

        setCurrentId(id);

        selectNext();
    }

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    MouseArea
    {
        id: mouseArea

        anchors.top   : parent.top
        anchors.bottom: parent.bottom

        width: parent.width

        enabled: isEnabled

        hoverEnabled: isEnabled

        cursor: Qt.PointingHandCursor

        onClicked:
        {
            window.clearFocus();

            if (list.getCurrentId() != id)
            {
                list.clearCurrentId();
            }

            pConfirm = true;
        }

        onStateChanged:
        {
            contextualItemConfirm.clip = true;

            if (pConfirm)
            {
                itemConfirm.visible = true;
            }
            else mouseArea.visible = true;
        }

        states: State
        {
            name: "confirm"; when: pConfirm

            AnchorChanges
            {
                target: mouseArea

                anchors.right: parent.left
            }
        }

        transitions: Transition
        {
            SequentialAnimation
            {
                AnchorAnimation
                {
                    duration: st.duration_normal

                    easing.type: st.easing
                }

                ScriptAction
                {
                    script:
                    {
                        if (pConfirm)
                        {
                            mouseArea.visible = false;

                            buttonConfirm.focus();
                        }
                        else
                        {
                            itemConfirm.visible = false;

                            list.focus();
                        }

                        contextualItemConfirm.clip = false;
                    }
                }
            }
        }

        Rectangle
        {
            id: background

            anchors.fill: parent

            anchors.bottomMargin: border.size

            visible: (pCurrent || mouseArea.containsMouse)

            gradient: Gradient
            {
                GradientStop
                {
                    position: 0.0

                    color:
                    {
                        if (isCurrent)
                        {
                            if (mouseArea.pressed || isReturnPressed)
                            {
                                return st.itemList_colorSelectFocusA;
                            }
                            else if (mouseArea.containsMouse)
                            {
                                return st.itemList_colorSelectHoverA;
                            }
                            else return st.itemList_colorSelectA;
                        }
                        else if (mouseArea.pressed || pConfirm)
                        {
                            return st.itemList_colorSelectFocusA;
                        }
                        else return st.itemList_colorHoverA;
                    }
                }

                GradientStop
                {
                    position: 1.0

                    color:
                    {
                        if (isCurrent)
                        {
                            if (mouseArea.pressed || isReturnPressed)
                            {
                                return st.itemList_colorSelectFocusB;
                            }
                            else if (mouseArea.containsMouse)
                            {
                                return st.itemList_colorSelectHoverB;
                            }
                            else return st.itemList_colorSelectB;
                        }
                        else if (mouseArea.pressed || pConfirm)
                        {
                            return st.itemList_colorSelectFocusB;
                        }
                        else return st.itemList_colorHoverB;
                    }
                }
            }
        }

        Icon
        {
            id: itemIcon

            anchors.left: parent.left

            anchors.leftMargin: Math.round((pIconWidth - width) / 2)

            anchors.verticalCenter: background.verticalCenter

            opacity: (isEnabled) ? 1.0 : st.icon_opacityDisable

            source    : icon
            sourceSize: iconSize

            style: st.icon_sunken

            filter: (pCurrent) ? st.icon2_filter
                               : st.icon1_filter
        }

        TextBase
        {
            anchors.fill: background

            leftMargin: (itemIcon.width) ? pIconWidth
                                         : st.contextualItem_padding

            rightMargin: st.contextualItem_padding

            verticalAlignment: Text.AlignVCenter

            opacity: (isEnabled) ? 1.0 : st.text_opacityDisable

            text: title

            color:
            {
                if (pCurrent || pConfirm)
                {
                    return st.itemList_colorTextSelected;
                }
                else if (mouseArea.containsMouse)
                {
                    return st.itemList_colorTextHover;
                }
                else return st.itemList_colorText;
            }

            style: st.text_sunken
        }

        Icon
        {
            id: itemIconRight

            anchors.right: parent.right

            anchors.rightMargin: st.contextualItemConfirm_paddingRight

            anchors.verticalCenter: background.verticalCenter

            opacity: (isEnabled) ? 1.0 : st.icon_opacityDisable

            source    : st.icon24x24_slideRight
            sourceSize: st.size24x24

            style: st.icon_sunken

            filter: (pCurrent) ? st.icon2_filter
                               : st.icon1_filter
        }
    }

    Item
    {
        id: itemConfirm

        anchors.left: mouseArea.right

        anchors.top   : parent.top
        anchors.bottom: border.top

        width: parent.width

        visible: false

        ButtonPianoIcon
        {
            id: buttonCancel

            anchors.top   : parent.top
            anchors.bottom: parent.bottom

            width: height + borderSizeWidth

            borderRight: borderSize

            icon          : st.icon24x24_slideLeft
            iconSourceSize: st.size24x24

            onActiveFocusChanged:
            {
                if (activeFocus == false && pAutoClear)
                {
                    pConfirm = false;
                }
            }

            onClicked: pConfirm = false

            Keys.onPressed:
            {
                if ((event.key == Qt.Key_Right
                    ||
                    event.key == Qt.Key_Tab || event.key == Qt.Key_Backtab)
                    &&
                    event.modifiers == Qt.NoModifier)
                {
                    event.accepted = true;

                    pAutoClear = false;

                    buttonConfirm.focus();

                    pAutoClear = true;
                }
                else if (event.key == Qt.Key_Up && event.modifiers == Qt.NoModifier)
                {
                    event.accepted = true;

                    pSelectPrevious();
                }
                else if (event.key == Qt.Key_Down && event.modifiers == Qt.NoModifier)
                {
                    event.accepted = true;

                    pSelectNext();
                }
            }
        }

        ButtonPiano
        {
            id: buttonConfirm

            anchors.left  : buttonCancel.right
            anchors.right : parent.right
            anchors.top   : parent.top
            anchors.bottom: parent.bottom

            borderRight: 0

            enabled: isEnabled

            text: title

            colorHoverA: st.button_colorConfirmHoverA
            colorHoverB: st.button_colorConfirmHoverB

            colorPressA: st.button_colorConfirmPressA
            colorPressB: st.button_colorConfirmPressB

            itemText.color: (isHovered || isPressed) ? st.button_colorTextB
                                                     : st.button_colorTextA

            onActiveFocusChanged:
            {
                if (activeFocus == false && pAutoClear)
                {
                    pConfirm = false;
                }
            }

            onClicked: itemClicked(id)

            Keys.onPressed:
            {
                if ((event.key == Qt.Key_Left
                    ||
                    event.key == Qt.Key_Tab || event.key == Qt.Key_Backtab)
                    &&
                    event.modifiers == Qt.NoModifier)
                {
                    event.accepted = true;

                    pAutoClear = false;

                    buttonCancel.focus();

                    pAutoClear = true;
                }
                else if (event.key == Qt.Key_Up && event.modifiers == Qt.NoModifier)
                {
                    event.accepted = true;

                    pSelectPrevious();
                }
                else if (event.key == Qt.Key_Down && event.modifiers == Qt.NoModifier)
                {
                    event.accepted = true;

                    pSelectNext();
                }
            }
        }
    }

    BorderHorizontal
    {
        id: border

        anchors.bottom: parent.bottom
    }
}
