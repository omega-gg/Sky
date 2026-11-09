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

#ifndef WDOWNLOADHOOKTORRENT_P_H
#define WDOWNLOADHOOKTORRENT_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

// Qt includes
#include <QHash>

// Private includes
#include <private/WMediaDownload_p>

#ifndef SK_NO_DOWNLOADHOOKTORRENT

class SK_TORRENT_EXPORT WDownloadHookTorrentPrivate : public WAbstractDownloadHookPrivate
{
public:
    WDownloadHookTorrentPrivate(WDownloadHookTorrent * p);

    void init();

public: // Functions
    void clearReply(QObject * reply);

public: // Slots
    void onAdded (WTorrentReply * reply);
    void onLoaded(WTorrentReply * reply);

    void onBuffer(qint64 pieces);

    void onDestroyed(QObject * object);

    void onAboutToQuit();

public: // Variables
    // NOTE: The download of each reply. A reply keeps its torrent in the cache.
    QHash<QObject *, WMediaDownload *> downloads;

protected:
    W_DECLARE_PUBLIC(WDownloadHookTorrent)
};

#endif // SK_NO_DOWNLOADHOOKTORRENT
#endif // WDOWNLOADHOOKTORRENT_P_H
