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

BaseSlider
{
    id: slider

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property int margins      : height / 5
    property int marginsHandle: height / 8

    property int radius: background.height

    property int borderSize: st.border_size

    //---------------------------------------------------------------------------------------------
    // Style

    property color colorA: st.slider_colorA
    property color colorB: st.slider_colorB

    property color colorHandleA: st.slider_colorHandleA
    property color colorHandleB: st.slider_colorHandleB

    property color colorHandleHoverA: st.slider_colorHandleHoverA
    property color colorHandleHoverB: st.slider_colorHandleHoverB

    property color colorHandlePressA: st.slider_colorHandlePressA
    property color colorHandlePressB: st.slider_colorHandlePressB

    property color colorBarA: st.slider_colorBarA
    property color colorBarB: st.slider_colorBarB

    property color colorBarHoverA: st.slider_colorBarHoverA
    property color colorBarHoverB: st.slider_colorBarHoverB

    //---------------------------------------------------------------------------------------------
    // Private

    property bool pHovered: (enabled && isHovered)

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias background: background
    property alias foreground: foreground

    property alias handleBackground: handleBackground

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    width : st.slider_width
    height: st.slider_height

    handle: itemHandle

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

    Rectangle
    {
        id: background

        anchors.fill: parent

        anchors.margins: margins

        radius: slider.radius

        gradient: Gradient
        {
            GradientStop { position: 0.0; color: colorA }
            GradientStop { position: 1.0; color: colorB }
        }

//#QT_4
        smooth: true
//#END

        border.width: borderSize
        border.color: st.border_color
    }

    Rectangle
    {
        id: foreground

        anchors.left  : parent.left
        anchors.top   : parent.top
        anchors.bottom: parent.bottom

        anchors.margins: margins

        width: handle.x + handleBackground.width - margins

        radius: slider.radius

        visible: (slider.enabled && value > -1)

        gradient: Gradient
        {
            GradientStop
            {
                position: 0.0

                color: (pHovered) ? colorBarHoverA
                                  : colorBarA
            }

            GradientStop
            {
                position: 1.0

                color: (pHovered) ? colorBarHoverB
                                  : colorBarB
            }
        }

//#QT_4
        smooth: true
//#END

        border.width: borderSize
        border.color: st.border_color
    }

    MouseArea
    {
        id: itemHandle

        width : parent.height
        height: width

        visible: slider.enabled

        acceptedButtons: Qt.NoButton

        hoverEnabled: true

        onXChanged: position = x

        Rectangle
        {
            id: handleBackground

            anchors.fill: parent

            anchors.margins: marginsHandle

            radius: slider.radius

            gradient: Gradient
            {
                GradientStop
                {
                    position: 0.0

                    color:
                    {
                        if (slider.pressed)
                        {
                            return colorHandlePressA;
                        }
                        else if (handle.hoverActive)
                        {
                            return colorHandleHoverA;
                        }
                        else return colorHandleA;
                    }
                }

                GradientStop
                {
                    position: 1.0

                    color:
                    {
                        if (slider.pressed)
                        {
                            return colorHandlePressB;
                        }
                        else if (handle.hoverActive)
                        {
                            return colorHandleHoverB;
                        }
                        else return colorHandleB;
                    }
                }
            }

//#QT_4
            smooth: true
//#END

            border.width: borderSize
            border.color: st.border_color
        }
    }
}
