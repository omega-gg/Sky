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

MouseArea
{
    id: slider

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property bool isHovered: containsMouse

    //---------------------------------------------------------------------------------------------
    // Style

    property variant borderBackground: st.slider_borderBackground
    property variant borderForeground: st.slider_borderForeground

    property ImageColorFilter filterHandle     : st.slider_filterHandle
    property ImageColorFilter filterHandleHover: st.slider_filterHandleHover
    property ImageColorFilter filterHandlePress: st.slider_filterHandlePress
    property ImageColorFilter filterBar        : st.slider_filterBar
    property ImageColorFilter filterBarHover   : st.slider_filterBarHover

    //---------------------------------------------------------------------------------------------
    // Private

    property variant pSourceSize: Qt.size(handle.width, handle.height)

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias background: background
    property alias foreground: foreground

    property alias handle: handle

    //---------------------------------------------------------------------------------------------
    // Model

    property alias model: model

    property alias value   : model.value
    property alias position: model.position

    property alias minimum: model.minimum
    property alias maximum: model.maximum

    property alias pageStep  : model.pageStep
    property alias singleStep: model.singleStep

    //---------------------------------------------------------------------------------------------
    // Style

    property alias filterBackground: background .filter
    property alias filterBorder    : imageBorder.filter

    //---------------------------------------------------------------------------------------------
    // Signal
    //---------------------------------------------------------------------------------------------

    signal handleReleased

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    width : st.slider_width
    height: st.slider_height

    hoverEnabled: true
    wheelEnabled: true

    drag.target: handle
    drag.axis  : Drag.XAxis

    drag.minimumX: model.handleMinimum
    drag.maximumX: model.handleMaximum

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    onPressed:
    {
        var pos = mouseX - (handle.width / 2);

        pos = Math.max(model.handleMinimum, pos);
        pos = Math.min(pos, model.handleMaximum);

        handle.x = pos;
    }

    onReleased:
    {
        position = handle.x;

        handleReleased();
    }

    onWheeled: if (slider.visible) model.scroll(steps * 2)

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function moveTo(pos)
    {
        pos = Math.max(0, pos);
        pos = Math.min(pos, maximum);

        value = pos;

        handleReleased();
    }

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    ModelRange
    {
        id: model

        handleMaximum: width - handle.width

        onPositionChanged:
        {
            if (width > 0 && handle.pressed == false)
            {
                handle.x = position;
            }
        }
    }

    BorderImageScale
    {
        id: background

        anchors.fill: parent

        source: st.slider_sourceBackground

        border
        {
            left : borderBackground.x;     top   : borderBackground.y
            right: borderBackground.width; bottom: borderBackground.height
        }

        filter: st.slider_filterBackground
    }

    BorderImageScale
    {
        id: foreground

        anchors.top   : parent.top
        anchors.bottom: parent.bottom

        width: handle.x + handle.width

        visible: (value > 0)

        source: st.slider_sourceForeground

        border
        {
            left : borderForeground.x;     top   : borderForeground.y
            right: borderForeground.width; bottom: borderForeground.height
        }

        filter: (slider.enabled && isHovered) ? filterBarHover
                                              : filterBar

        scaling: false
    }

    MouseArea
    {
        id: handle

        width : st.slider_handleWidth
        height: st.slider_handleHeight

        visible: slider.enabled

        acceptedButtons: Qt.NoButton

        hoverEnabled: true

        onXChanged: position = x

        Image
        {
            id: handleBackground

            anchors.fill: parent

            sourceSize: pSourceSize

            source: st.slider_sourceHandle

            filter:
            {
                if (slider.pressed)
                {
                    return filterHandlePress;
                }
                else if (handle.containsMouse)
                {
                    return filterHandleHover;
                }
                else return filterHandle;
            }
        }

        Image
        {
            anchors.fill: parent

            sourceSize: pSourceSize

            source: st.slider_sourceHandleBorder
        }
    }

    BorderImageScale
    {
        id: imageBorder

        anchors.fill: parent

        source: st.slider_sourceBorder

        border
        {
            left : borderBackground.x;     top   : borderBackground.y
            right: borderBackground.width; bottom: borderBackground.height
        }

        filter: st.slider_filterBorder
    }
}
