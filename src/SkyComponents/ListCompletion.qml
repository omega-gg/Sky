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
import Sky     1.0

List
{
    id: listCompletion

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    /* read */ property string completion

    property int currentIndex: -1

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias query: model.query

    //---------------------------------------------------------------------------------------------
    // Signals
    //---------------------------------------------------------------------------------------------

    signal itemClicked      (int index)
    signal itemDoubleClicked(int index)

    signal queryCompleted

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    model: ModelCompletionGoogle
    {
        id: model

        onQueryCompleted:
        {
            pUpdateCompletion();

            listCompletion.queryCompleted();
        }
    }

    delegate: ComponentCompletion {}

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    onCurrentIndexChanged: pUpdateCompletion()

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function addQueryItem(key, value)
    {
        model.addQueryItem(key, value);
    }

    function runQuery() { model.runQuery(); }

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

    //---------------------------------------------------------------------------------------------
    // Private

    function pUpdateCompletion()
    {
        if (currentIndex == -1)
        {
             completion = "";
        }
        else completion = model.getCompletionAt(currentIndex);
    }
}
