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

BaseList
{
    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property int currentIndex: -1

    //---------------------------------------------------------------------------------------------
    // Signals
    //---------------------------------------------------------------------------------------------

    signal itemPressed (int index)
    signal itemReleased(int index)

    signal itemClicked      (int index)
    signal itemDoubleClicked(int index)

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function selectPrevious()
    {
        if (currentIndex <= 0)
        {
            currentIndex = -1;
        }
        else if (count)
        {
            currentIndex--;
        }
    }

    function selectNext()
    {
        if (currentIndex >= (count - 1))
        {
            currentIndex = -1;
        }
        else if (count)
        {
            currentIndex++;
        }
    }
}
