//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkyPresentation.

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

Slides
{
    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    /* mandatory */ property variant pages

    property variant indexes: getIndexes()

    property int indexStart: 0

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    steps: indexes[indexes.length - 1]

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function fromPage(index, indexPage)
    {
        return from(index, pages[indexPage]);
    }

    //---------------------------------------------------------------------------------------------

    function getIndexes()
    {
        var indexes = new Array;

        var at = indexStart;

        indexes.push(at);

        var length = pages.length;

        for (var i = 0; i < length; i++)
        {
            at += pages[i];

            indexes.push(at);
        }

        // NOTE: We take the 'indexStart' into account for the first page length.
        if (length)
        {
            pages[0] += indexStart;
        }

        return indexes;
    }
}
