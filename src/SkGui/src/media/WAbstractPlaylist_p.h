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

#ifndef WABSTRACTPLAYLIST_P_H
#define WABSTRACTPLAYLIST_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

#include <private/WLibraryItem_p>

#ifndef SK_NO_ABSTRACTPLAYLIST

class SK_GUI_EXPORT WAbstractPlaylistPrivate : public WLibraryItemPrivate
{
public:
    WAbstractPlaylistPrivate(WAbstractPlaylist * p);

    /* virtual */ ~WAbstractPlaylistPrivate();

    void init();

public: // Function
    void setPrevious(bool cycle);
    void setNext    (bool cycle);

    bool hasPrevious(int index) const;
    bool hasNext    (int index) const;

    bool insertSelected(const QList<int> & indexes, const WAbstractTrack * track, int index);

    QList<int> getSelected() const;

    void currentIndexChanged();

    void emitSelectedTracksChanged(const QList<int> & indexes);

public: // Variables
    const WAbstractTrack * currentTrack;

    int currentIndex;
    int currentTime;

    qreal scrollValue;

    int maxCount;

    QList<const WAbstractTrack *> selectedTracks;

    QList<WAbstractPlaylistWatcher *> watchers;

protected:
    W_DECLARE_PUBLIC(WAbstractPlaylist)
};

#endif // SK_NO_ABSTRACTPLAYLIST
#endif // WABSTRACTPLAYLIST_P_H
