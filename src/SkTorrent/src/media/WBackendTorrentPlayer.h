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

#ifndef WBACKENDTORRENTPLAYER_H
#define WBACKENDTORRENTPLAYER_H

// Sk includes
#include <WAbstractBackend>

#ifndef SK_NO_BACKENDTORRENTPLAYER

class WBackendTorrentPlayerPrivate;

class SK_TORRENT_EXPORT WBackendTorrentPlayer : public WAbstractBackend
{
    Q_OBJECT

public:
    WBackendTorrentPlayer();

protected: // WAbstractBackend implementation
    /* virtual */ bool backendSetSource(const QUrl & url);

    /* virtual */ bool backendPlay  ();
    /* virtual */ bool backendReplay();

    /* virtual */ bool backendPause();
    /* virtual */ bool backendStop ();

    /* virtual */ void backendSetVolume(int percent);

    /* virtual */ bool backendDelete();

private:
    W_DECLARE_PRIVATE(WBackendTorrentPlayer)
};

#include <private/WBackendTorrentPlayer_p>

#endif // SK_NO_BACKENDTORRENTPLAYER
#endif // WBACKENDTORRENTPLAYER_H
