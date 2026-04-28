//=================================================================================================
/*
    Copyright (C) 2015-2026 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkTorrent.

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

#ifndef WBACKENDTORRENT_H
#define WBACKENDTORRENT_H

// Sk includes
#include <WBackendManager>

#ifndef SK_NO_BACKENDTORRENT

class WBackendTorrentPrivate;

class SK_TORRENT_EXPORT WBackendTorrent : public WBackendManager
{
    Q_OBJECT

public:
    WBackendTorrent(QObject * parent = NULL);
protected:
    WBackendTorrent(WBackendTorrentPrivate * p, QObject * parent = NULL);

protected: // WBackendManager implementation
    /* virtual */ WAbstractHook * createHook(WAbstractBackend * backend);

private:
    W_DECLARE_PRIVATE(WBackendTorrent)
};

#endif // SK_NO_BACKENDTORRENT
#endif // WBACKENDTORRENT_H
