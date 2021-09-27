//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkGui.

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

#ifndef WPLAYLIST_P_H
#define WPLAYLIST_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

#include <private/WLibraryItem_p>

#ifndef SK_NO_PLAYLIST

class SK_GUI_EXPORT WPlaylistPrivate : public WLibraryItemPrivate
{
public:
    WPlaylistPrivate(WPlaylist * p);

    /* virtual */ ~WPlaylistPrivate();

    void init();

public: // Function
    const WTrack * itemFromId(int id)    const;
    const WTrack * itemAt    (int index) const;

    WTrack * getTrack(int index);

    void loadTracks(const QList<WTrack> & tracks);

    bool loadTrack(int index);

    void applyTrack(WTrack * track, int index);

    bool loadCover(WTrack * track);

    void setPrevious(bool cycle);
    void setNext    (bool cycle);

    bool hasPrevious(int index) const;
    bool hasNext    (int index) const;

    bool insertSelected(const QList<int> & indexes, const WTrack * track, int index);

    QList<int> getSelected() const;

    void currentIndexChanged();

    void emitSelectedTracksChanged(const QList<int> & indexes);

public: // Variables
    QList<WTrack> tracks;

    WListId ids;

    const WTrack * currentTrack;

    int currentIndex;
    int currentTime;

    qreal scrollValue;

    int maxCount;

    QList<const WTrack *> selectedTracks;

    QList<WPlaylistWatcher *> watchers;

protected:
    W_DECLARE_PUBLIC(WPlaylist)

    friend class WPlaylistReadReply;
};

#endif // SK_NO_PLAYLIST
#endif // WPLAYLIST_P_H
