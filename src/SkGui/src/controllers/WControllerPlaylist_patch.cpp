//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkGui.

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

#include "WControllerPlaylist.h"

#ifndef SK_NO_CONTROLLERPLAYLIST

// Sk includes
#include <WControllerApplication>
#include <WYamlReader>
#include <WRegExp>

void WControllerPlaylist_patch(QString & data, const QString & api)
{
    qWarning("WControllerPlaylist_patch: Patching.");

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

    if (Sk::versionIsLower(api, "1.0.4"))
    {
        data.replace("\nbackend_", "\nbackends_");

        WControllerPlaylist::Type type = WControllerPlaylist::vbmlType(data);

        if (type == WControllerPlaylist::Index || type == WControllerPlaylist::Backend)
        {
            data.replace("\nsource", "\norigin");
        }
    }

    if (Sk::versionIsLower(api, "1.0.6"))
    {
        WControllerPlaylist::Type type = WControllerPlaylist::vbmlType(data);

        if (WControllerPlaylist::vbmlTypeTrack(type))
        {
            WYamlReader reader(data.toUtf8());

            if (data.contains("\norigin") == false)
            {
                const WYamlNode * node = reader.at("source");

                if (node)
                {
                    // NOTE: A single source is the equivalent of the origin property.
                    if (node->children.isEmpty())
                    {
                        data.replace("\nsource", "\norigin");
                    }
                }
                else data.replace("\nmedia", "\nsource");
            }
        }
        else if (WControllerPlaylist::vbmlTypePlaylist(type))
        {
            WYamlReader reader(data.toUtf8());

            if (reader.at("origin") == NULL)
            {
                data.replace("\nsource", "\norigin");
            }
        }
    }

    //---------------------------------------------------------------------------------------------
    // NOTE: We replace the VBML comment with the right 'api' key.

    Sk::removeLine(&data, QString('#'), WControllerPlaylist::vbmlHeader(data));

    data.prepend("# VBML " + WControllerPlaylist::versionApi() + '\n');
}

#endif // SK_NO_CONTROLLERPLAYLIST
