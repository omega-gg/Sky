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

#include "WBackendIndex.h"

#ifndef SK_NO_BACKENDINDEX

// Sk includes
#include <WControllerApplication>
#include <WControllerPlaylist>

//-------------------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------------------

void WBackendIndex_patch(QString & data, const QString & api)
{
    qWarning("WBackendIndex_patch: Patching.");

    // NOTE: We replace the first occurence after the 'api' key.
    Sk::replaceFirst(&data, api, WControllerPlaylist::versionApi(), data.indexOf("api"));
}

#endif // SK_NO_BACKENDINDEX
