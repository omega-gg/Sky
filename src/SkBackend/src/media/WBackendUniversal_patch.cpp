//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkBackend.

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

#include "WBackendUniversal.h"

#ifndef SK_NO_BACKENDUNIVERSAL

// Sk includes
#include <WControllerApplication>
#include <WControllerPlaylist>

//-------------------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------------------

void WBackendUniversal_patch(QString & data, const QString & api)
{
    qWarning("WBackendUniversal_patch: Patching.");

    if (Sk::versionIsLower(api, "1.0.1"))
    {
        // String
        data.replace("PREPEND_CHAR", "PREPEND");
        data.replace("APPEND_CHAR",  "APPEND");
        data.replace("REMOVE_CHAR",  "REMOVE");

        // List
        data.replace("GET_LIST",     "LIST_GET");
        data.replace("SET_LIST",     "LIST_SET");
        data.replace("TAKE_LIST_AT", "LIST_TAKE_AT");
        data.replace("APPEND_LIST",  "LIST_APPEND");
        data.replace("REMOVE_LIST",  "LIST_REMOVE"); // And LIST_REMOVE_AT
        data.replace("INDEX_LIST",   "LIST_INDEX");
        data.replace("CONTAIN_LIST", "LIST_CONTAIN");

        // Hash
        data.replace("GET_HASH",    "HASH_GET");
        data.replace("SET_HASH",    "HASH_SET");
        data.replace("REMOVE_HASH", "HASH_REMOVE");
    }

    // NOTE: We replace the first occurence after the 'api' key.
    Sk::replaceFirst(&data, api, WControllerPlaylist::versionApi(), data.indexOf("api"));
}

#endif // SK_NO_BACKENDUNIVERSAL
