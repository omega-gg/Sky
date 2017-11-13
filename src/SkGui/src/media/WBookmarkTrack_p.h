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

#ifndef WBOOKMARKTRACK_P_H
#define WBOOKMARKTRACK_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

#include <private/Sk_p>

#ifndef SK_NO_BOOKMARKTRACK

class SK_GUI_EXPORT WBookmarkTrackPrivate : public WPrivate
{
public:
    WBookmarkTrackPrivate(WBookmarkTrack * p);

    /* virtual */ ~WBookmarkTrackPrivate();

    void init();

public: // Functions
    void setParentTab(WTabTrack * parent);

    void setPlaylist  (WPlaylist * playlist);
    void clearPlaylist();

    void setTrack  (const WTrack & track);
    void clearTrack();

    void updateTrackPointer();

    void emitUpdated();

public: // Variables
    int id;

    WTabTrack * parentTab;

    WPlaylist    * playlist;
    const WTrack * track;

    QList<int> idPlaylist;
    int        idTrack;

    WTrack::State state;

    QUrl source;

    QString title;
    QUrl    cover;

    QString author;
    QString feed;

    int duration;

    QDateTime date;

    WAbstractBackend::Quality quality;

    QUrl videoShot;

    int currentTime;

protected:
    W_DECLARE_PUBLIC(WBookmarkTrack)
};

#endif // SK_NO_BOOKMARKTRACK
#endif // WBOOKMARKTRACK_P_H
