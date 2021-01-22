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

List
{
    id: listCompletion

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    /* read */ property bool hasNoResults: false

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
            hasNoResults = (query && count == 0);

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

    //---------------------------------------------------------------------------------------------

    function runCompletion(text)
    {
        currentIndex = -1;

        query = text;

        runQuery();
    }

    function runQuery() { model.runQuery(); }

    //---------------------------------------------------------------------------------------------
    // List reimplementation

    function selectPrevious()
    {
        var index = currentIndex;

        if (index < 1)
        {
             currentIndex = -1;
        }
        else currentIndex--;
    }

    function selectNext()
    {
        var index = currentIndex;

        if (index > count - 2)
        {
             currentIndex = -1;
        }
        else currentIndex++;
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
