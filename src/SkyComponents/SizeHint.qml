//=================================================================================================
/*
    Copyright (C) 2015-2016 Sky kit authors united with omega. <http://omega.gg/about>

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

Item {
    //---------------------------------------------------------------------------------------------
    // Properties

    property real minimumWidth : -1
    property real minimumHeight: -1

    property real preferredWidth : -1
    property real preferredHeight: -1

    property real maximumWidth : -1
    property real maximumHeight: -1

    //---------------------------------------------------------------------------------------------
    // Functions

    function getMinimumWidth()
    {
        if (maximumWidth != -1)
        {
             return Math.min(minimumWidth, maximumWidth);
        }
        else return minimumWidth;
    }

    function getMinimumHeight()
    {
        if (maximumHeight != -1)
        {
             return Math.min(minimumHeight, maximumHeight);
        }
        else return minimumHeight;
    }

    //---------------------------------------------------------------------------------------------

    function getPreferredWidth()
    {
        var preferred;

        if (minimumWidth != -1) preferred = Math.max(minimumWidth, preferredWidth);
        if (maximumWidth != -1) preferred = Math.min(preferred,    maximumWidth);

        return preferred;
    }

    function getPreferredHeight()
    {
        var preferred;

        if (minimumHeight != -1) preferred = Math.max(minimumHeight, preferredHeight);
        if (maximumHeight != -1) preferred = Math.min(preferred,     maximumHeight);

        return preferred;
    }

    //---------------------------------------------------------------------------------------------

    function getMaximumWidth()
    {
        if (minimumWidth != -1)
        {
             return Math.max(minimumWidth, maximumWidth);
        }
        else return maximumWidth;
    }

    function getMaximumHeight()
    {
        if (minimumHeight != -1)
        {
             return Math.max(minimumHeight, maximumHeight);
        }
        else return maximumHeight;
    }
}
