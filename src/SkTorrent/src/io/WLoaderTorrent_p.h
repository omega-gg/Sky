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

#ifndef WLOADERTORRENT_P_H
#define WLOADERTORRENT_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

#include <private/WAbstractLoader_p>

#ifndef SK_NO_LOADERTORRENT

// Forward declarations
class WMagnetReply;

class SK_TORRENT_EXPORT WLoaderTorrentPrivate : public WAbstractLoaderPrivate
{
public:
    WLoaderTorrentPrivate(WLoaderTorrent * p);

    void init();

public: // Slots
    void onLoaded(WMagnetReply * reply);

    void onDestroyed();

public: // Variables
    QHash<QIODevice *, WMagnetReply *> replies;

protected:
    W_DECLARE_PUBLIC(WLoaderTorrent)
};

#endif // SK_NO_LOADERTORRENT
#endif // WLOADERTORRENT_P_H
