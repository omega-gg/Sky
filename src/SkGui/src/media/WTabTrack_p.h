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

#ifndef WTABTRACK_P_H
#define WTABTRACK_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

// Qt includes
#include <QDateTime>
#include <QPixmap>

// Sk includes
#include <WListId>

// Private includes
#include <private/WAbstractTab_p>

#ifndef SK_NO_TABTRACK

//-------------------------------------------------------------------------------------------------
// WTabTrackDataBookmark
//-------------------------------------------------------------------------------------------------

struct WTabTrackDataBookmark
{
    int id;

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
    int  currentTime;
};

//-------------------------------------------------------------------------------------------------
// WTabTrackPrivate
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WTabTrackPrivate : public WAbstractTabPrivate
{
public:
    WTabTrackPrivate(WTabTrack * p);

    /* virtual */ ~WTabTrackPrivate();

    void init();

public: // Functions
    bool checkId(const QList<int> & idA, const QList<int> & idB);

    void loadBookmarks(const QList<WTabTrackDataBookmark> & bookmarks);

    void removeBookmark(int index);

    WBookmarkTrack * bookmarkFromId(int id);

    void setCurrentBookmark(WBookmarkTrack * bookmark);

    bool setPlaylist(WPlaylist * playlist);

    void applyPlaylist (WBookmarkTrack * bookmark);
    void updatePlaylist(WBookmarkTrack * bookmark);

    void clearPlaylist();

    void setTrack(const WTrack * track);

    void setVideoShot(WBookmarkTrack * bookmark, const QUrl & url);

    void saveState();

public: // Slots
    void onFocusChanged();

    void onCurrentTrackChanged();

    void onRegisterItemId(WLibraryItem * item, const QList<int> & idFull);

    void onFilesUpdated(const QList<int> & idFull);

    void onPlaylistDestroyed();

public: // Variables
    QList<WBookmarkTrack> bookmarks;

    WListId                      ids;
    QHash<int, WBookmarkTrack *> idHash;

    QHash<int, QPair<QString, QPixmap> > videoShots;

    WPlaylist          * playlist;
    WDeclarativePlayer * player;

    WBookmarkTrack * currentBookmark;

    bool stackEnabled;

    int maxCount;

protected:
    W_DECLARE_PUBLIC(WTabTrack)

    friend class WTabTrackReadReply;
};

#endif // SK_NO_TABTRACK
#endif // WTABTRACK_P_H
