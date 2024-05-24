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
#ifndef SK_NO_PLAYER
#include <QPixmap>
#endif

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

    WTrack::Type type;

    WTrack::State state;

    QString source;

    QString title;
    QString cover;

    QString author;
    QString feed;

    int duration;

    QDateTime date;

    QString videoShot;

    int currentTime;

    QString subtitle;
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

    void updatePlaylistTrack();

    void checkPlaylistTrack();

    void setVideoShot(WBookmarkTrack * bookmark, const QString & url);

    void saveState();

public: // Slots
    void onFocusChanged();

    void onCurrentTrackChanged();

    void onTrackLoaded();

    void onRegisterItemId(WLibraryItem * item, const QList<int> & idFull);

    void onFilesUpdated(const QList<int> & idFull);

    void onPlaylistDestroyed();

public: // Variables
    QList<WBookmarkTrack> bookmarks;

    WListId                      ids;
    QHash<int, WBookmarkTrack *> idHash;

#ifndef SK_NO_PLAYER
    QHash<int, QPair<QString, QPixmap> > videoShots;
#endif

    WPlaylist * playlist;
    WPlaylist * playlistTrack;

#ifndef SK_NO_PLAYER
    WPlayer * player;
#endif

    WBookmarkTrack * currentBookmark;

    bool stackEnabled;

    int maxCount;

protected:
    W_DECLARE_PUBLIC(WTabTrack)

    friend class WTabTrackReadReply;
};

#endif // SK_NO_TABTRACK
#endif // WTABTRACK_P_H
