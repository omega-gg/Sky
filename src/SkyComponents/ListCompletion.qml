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

List
{
    id: listCompletion

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    /* read */ property string completion

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias query: model.query

    //---------------------------------------------------------------------------------------------
    // Signals
    //---------------------------------------------------------------------------------------------

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
