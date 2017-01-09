//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkGui module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#ifndef WPLAYLISTNET_P_H
#define WPLAYLISTNET_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

// Sk includes
#include <private/WAbstractPlaylist_p>

#ifndef SK_NO_PLAYLISTNET

// Forward declarations
class WTrackNet;
class WThreadActionWritePlaylist;
class WThreadActionReadPlaylist;

class SK_GUI_EXPORT WPlaylistNetPrivate : public WAbstractPlaylistPrivate
{
public:
    WPlaylistNetPrivate(WPlaylistNet * p);

    void init();

public: // Functions
    const WTrackNet * getTrackConst(int index) const;
    WTrackNet       * getTrack     (int index);

    bool containsTrackPointer(WTrackNet * track) const;

    void loadTracks(const QList<WTrackNet> & tracks);

public: // Variables
    QList<WTrackNet> tracks;

    WListId ids;

protected:
    W_DECLARE_PUBLIC(WPlaylistNet)

    friend class WPlaylistNetReadReply;
};

#endif // SK_NO_PLAYLISTNET
#endif // WPLAYLISTNET_P_H
