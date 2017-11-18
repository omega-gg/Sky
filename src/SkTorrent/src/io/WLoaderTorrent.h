//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkTorrent module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#ifndef WLOADERTORRENT_H
#define WLOADERTORRENT_H

// Sk includes
#include <WAbstractLoader>

#ifndef SK_NO_LOADERTORRENT

class WLoaderTorrentPrivate;

class SK_TORRENT_EXPORT WLoaderTorrent : public WAbstractLoader
{
    Q_OBJECT

public:
    explicit WLoaderTorrent(QObject * parent = NULL);

protected: // WAbstractLoader implementation
    /* virtual */ QIODevice * load(WRemoteData * data);

protected: // WAbstractLoader reimplementation
    /* virtual */ void abort(QIODevice * reply);

private:
    W_DECLARE_PRIVATE(WLoaderTorrent)

    Q_PRIVATE_SLOT(d_func(), void onLoaded(WMagnetReply *))

    Q_PRIVATE_SLOT(d_func(), void onDestroyed())
};

#include <private/WLoaderTorrent_p>

#endif // SK_NO_LOADERTORRENT
#endif // WLOADERTORRENT_H
