//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the Sky kit runtime.

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

Application
{
    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function load()
    {
        var argument = core.argument;

        if (argument == "")
        {
             loader.source = "Gui.qml";
        }
        else loader.source = argument;

        loader.item.forceActiveFocus();
    }

    function toggleMaximized()
    {
        window.maximized = !(window.maximized);
    }

    //---------------------------------------------------------------------------------------------
    // Children
    //---------------------------------------------------------------------------------------------

    WindowSky
    {
        id: window

//#QT_4
        antialias: true
//#END

        vsync: true

        st: StyleComponents
        {
            id: st

            // NOTE: We apply the night color right away to avoid the flickering before onFadeIn.
            window_color: "#404040"
        }

        //-----------------------------------------------------------------------------------------
        // Events
        //-----------------------------------------------------------------------------------------

        onFadeIn:
        {
            st.applyNight();

            core.load();

            load();
        }

        viewport.onActiveFocusChanged:
        {
            if (loader.item)
            {
                loader.item.forceActiveFocus();
            }
        }

        /* QML_EVENT */ onKeyPressed: function(event)
        {
            if (event.key == Qt.Key_Escape)
            {
                event.accepted = true;

                close();
            }
            else if (event.key == Qt.Key_P && event.modifiers == Qt.ControlModifier)
            {
                event.accepted = true;

                if (event.isAutoRepeat) return;

                window.writeShot(core.pathShots);
            }
            else if (event.key == Qt.Key_F11)
            {
                event.accepted = true;

                fullScreen = !(fullScreen);
            }
        }

        //-----------------------------------------------------------------------------------------
        // Children
        //-----------------------------------------------------------------------------------------

        ViewDrag
        {
            id: viewDrag

            anchors.fill: parent

            visible: (window.fullScreen == false)

            onDoubleClicked: toggleMaximized()
        }

        Loader
        {
            id: loader

            anchors.fill: parent
        }
    }
}
