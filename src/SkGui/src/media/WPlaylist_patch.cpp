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

#include "WPlaylist.h"

#ifndef SK_NO_PLAYLIST

// Sk includes
#include <WControllerApplication>

//-------------------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------------------

void WBackendPlaylist_patch(QString & data, const QString & api)
{
    qWarning("WBackendPlaylist_patch: Patching.");

    if (Sk::versionIsLower(api, "1.8.1-0"))
    {
        int index = data.indexOf("<tracks>");

        // NOTE: We've added a 'type' property for tracks.
        Sk::insertLines(&data, Sk::tabs(4) + "<type>0</type>", "<id>", index);

        // NOTE: We don't need 'quality' anymore.
        Sk::removeLines(&data, "<quality>", index);
    }

    // NOTE: We replace the first occurence after the 'version' key.
    Sk::replaceFirst(&data, api, Sk::versionSky(), data.indexOf("version"));
}

#endif // SK_NO_PLAYLIST
