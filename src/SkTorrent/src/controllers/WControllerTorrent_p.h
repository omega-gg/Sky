//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

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

    void init(const QString & path, int port, qint64 sizeMax);

public: // Functions
    void loadTorrent(WTorrentReply * reply, const QString & url, WTorrent::Mode mode);
    void loadMagnet (WMagnetReply  * reply, const QString & url);

    void removeTorrent(WTorrent * torrent, WTorrentReply * reply);
    void removeMagnet (WMagnet  * magnet,  WMagnetReply  * reply);

    int extractIndex(const QString & url) const;

public: // Static functions
    static void applyMagnet(WMagnet * magnet, const QByteArray & data);

public: // Slots
    void onLoaded      (WRemoteData * data);
    void onMagnetLoaded(WRemoteData * data);

public: // Variables
    QThread * thread;

    WTorrentEngine * engine;

    QHash<WBackendNetQuery::Type, WAbstractLoader *> loaders;

    QHash<WRemoteData *, WTorrent *> jobs;
    QHash<WRemoteData *, WMagnet  *> jobsMagnets;

    QList<WTorrent *> torrents;
    QList<WMagnet  *> magnets;

    WListId ports;

    int port;

protected:
    W_DECLARE_PUBLIC(WControllerTorrent)
};

#endif // SK_NO_CONTROLLERTORRENT
#endif // WCONTROLLERTORRENT_P_H
