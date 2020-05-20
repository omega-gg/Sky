//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of Sky kit.

    - GNU Lesser General Public License Usage:
    This file may be used under the terms of the GNU Lesser General Public License version 3 as
    published by the Free Software Foundation and appearing in the LICENSE.md file included in the
    packaging of this file. Please review the following information to ensure the GNU Lesser
    General Public License requirements will be met: https://www.gnu.org/licenses/lgpl.html.
*/
//=================================================================================================

import QtQuick 1.0
import Sky     1.0

ItemSlide
{
    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias item: loader.item

    property alias source         : loader.source
    property alias sourceComponent: loader.sourceComponent

    property alias progress: loader.progress

    property alias status: loader.status

    //---------------------------------------------------------------------------------------------

    property alias loader: loader

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function load(source)
    {
        loader.source = source;
    }

    function reload()
    {
        var source = loader.source;

        if (source)
        {
            loader.source = "";
            loader.source = source;
        }
    }

    //---------------------------------------------------------------------------------------------

    function loadLeft (source) { pLoad(source, Sk.Left);  }
    function loadRight(source) { pLoad(source, Sk.Right); }
    function loadUp   (source) { pLoad(source, Sk.Up);    }
    function loadDown (source) { pLoad(source, Sk.Down);  }

    //---------------------------------------------------------------------------------------------
    // Private

    function pLoad(source, direction)
    {
        if (loader.source == source) return;

        init();

        loader.source = source;

        pStart(direction);
    }

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    Loader
    {
        id: loader

        anchors.fill: parent
    }
}
