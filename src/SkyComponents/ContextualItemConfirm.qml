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

Item
{
    id: contextualItemConfirm

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------
    // Private

    property bool pSelected: (isSelected || mouseArea.pressed)

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
    // Children
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

            if (list.getSelectedId() != id)
            {
                list.clearSelectedId();
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

                            buttonConfirm.setFocus();
                        }
                        else
                        {
                            itemConfirm.visible = false;

                            list.setFocus();
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

            visible: (isSelected || mouseArea.hoverActive)

            gradient: Gradient
            {
                GradientStop
                {
                    position: 0.0

                    color:
                    {
                        if (isSelected)
                        {
                            if (mouseArea.pressed || isReturnPressed)
                            {
                                return st.itemList_colorSelectFocusA;
                            }
                            else if (mouseArea.hoverActive)
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
                        if (isSelected)
                        {
                            if (mouseArea.pressed || isReturnPressed)
                            {
                                return st.itemList_colorSelectFocusB;
                            }
                            else if (mouseArea.hoverActive)
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

            filter:
            {
                if      (pSelected) return st.icon2_filter;
                else if (isCurrent) return st.icon_filterActive;
                else                return st.icon1_filter;
            }
        }

        TextBase
        {
            anchors.fill: background

            anchors.leftMargin: (itemIcon.width) ? pIconWidth
                                                 : st.contextualItem_padding

            anchors.rightMargin: st.contextualItem_padding

            verticalAlignment: Text.AlignVCenter

            opacity: (isEnabled) ? 1.0 : st.text_opacityDisable

            text: title

            color:
            {
                if      (pSelected || pConfirm) return st.itemList_colorTextSelected;
                else if (isCurrent)             return st.itemList_colorTextCurrent;
                else if (mouseArea.hoverActive) return st.itemList_colorTextHover;
                else                            return st.itemList_colorText;
            }

            style: st.text_sunken
        }

        Icon
        {
            id: itemIconRight

            anchors.right: parent.right

            anchors.verticalCenter: background.verticalCenter

            opacity: (isEnabled) ? 1.0 : st.icon_opacityDisable

            source    : st.icon12x12_slideRight
            sourceSize: st.size12x12

            sourceArea: Qt.size(parent.height, parent.height)

            style: st.icon_sunken

            filter: itemIcon.filter
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

            borderRight: borderSize

            icon          : st.icon12x12_slideLeft
            iconSourceSize: st.size12x12

            onActiveFocusChanged:
            {
                if (activeFocus == false && pAutoClear)
                {
                    pConfirm = false;
                }
            }

            onClicked: pConfirm = false

            /* QML_EVENT */ Keys.onPressed: function(event)
            {
                if ((event.key == Qt.Key_Right
                    ||
                    event.key == Qt.Key_Tab || event.key == Qt.Key_Backtab)
                    &&
                    event.modifiers == sk.keypad(Qt.NoModifier))
                {
                    event.accepted = true;

                    pAutoClear = false;

                    buttonConfirm.setFocus();

                    pAutoClear = true;
                }
                else if (event.key == Qt.Key_Up && event.modifiers == sk.keypad(Qt.NoModifier))
                {
                    event.accepted = true;

                    pSelectPrevious();
                }
                else if (event.key == Qt.Key_Down && event.modifiers == sk.keypad(Qt.NoModifier))
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

            /* QML_EVENT */ Keys.onPressed: function(event)
            {
                if ((event.key == Qt.Key_Left
                    ||
                    event.key == Qt.Key_Tab || event.key == Qt.Key_Backtab)
                    &&
                    event.modifiers == sk.keypad(Qt.NoModifier))
                {
                    event.accepted = true;

                    pAutoClear = false;

                    buttonCancel.setFocus();

                    pAutoClear = true;
                }
                else if (event.key == Qt.Key_Up && event.modifiers == sk.keypad(Qt.NoModifier))
                {
                    event.accepted = true;

                    pSelectPrevious();
                }
                else if (event.key == Qt.Key_Down && event.modifiers == sk.keypad(Qt.NoModifier))
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
