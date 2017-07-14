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

#ifndef WCONTROLLERTORRENT_P_H
#define WCONTROLLERTORRENT_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

// Sk includes
#include <WListId>

// Private includes
#include <private/WController_p>

#ifndef SK_NO_CONTROLLERTORRENT

// Forward declarations
class WRemoteData;

class SK_TORRENT_EXPORT WControllerTorrentPrivate : public WControllerPrivate
{
public:
    WControllerTorrentPrivate(WControllerTorrent * p);

    /* virtual */ ~WControllerTorrentPrivate();

    void init(const QString & path, qint64 sizeMax);

public: // Functions
    void loadTorrent(WTorrentReply * reply, const QUrl & url, WTorrent::Mode mode);

    void removeTorrent(WTorrent * torrent, WTorrentReply * reply);

public: // Slots
    void onLoaded(WRemoteData * data);

public: // Variables
    QThread * thread;

    WTorrentEngine * engine;

    QHash<WRemoteData *, WTorrent *> jobs;

    QList<WTorrent *> downloads;

    WListId ports;

    int port;

protected:
    W_DECLARE_PUBLIC(WControllerTorrent)
};

#endif // SK_NO_CONTROLLERTORRENT
#endif // WCONTROLLERTORRENT_P_H
