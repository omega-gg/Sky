//=================================================================================================
/*
    Copyright (C) 2015-2016 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkTorrent module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#ifndef WBACKENDTORRENTPLAYER_P_H
#define WBACKENDTORRENTPLAYER_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

// Private includes
#include <private/WAbstractBackend_p>

#ifndef SK_NO_BACKENDTORRENTPLAYER

class SK_TORRENT_EXPORT WBackendTorrentPlayerPrivate : public WAbstractBackendPrivate
{
public:
    WBackendTorrentPlayerPrivate(WBackendTorrentPlayer * p);

    void init();

protected:
    W_DECLARE_PUBLIC(WBackendTorrentPlayer)
};

#endif // SK_NO_BACKENDTORRENTPLAYER
#endif // WBACKENDTORRENTPLAYER_P_H
