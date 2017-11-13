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

#ifndef WBOOKMARKTRACK_H
#define WBOOKMARKTRACK_H

// Sk includes
#include <WPlaylist>

#ifndef SK_NO_BOOKMARKTRACK

// Forward declarations
class WBookmarkTrackPrivate;

class SK_GUI_EXPORT WBookmarkTrack : public WPrivatable, public WPlaylistWatcher
{
public:
    explicit WBookmarkTrack(const WTrack & track);

    WBookmarkTrack();

public: // Interface
    void setTrack(const WTrack & track);

    WTrack      toTrack    () const;
    QVariantMap toTrackData() const;

    void save();

public: // Operators
    WBookmarkTrack(const WBookmarkTrack & other);

    bool operator==(const WBookmarkTrack & other) const;

    WBookmarkTrack & operator=(const WBookmarkTrack & other);

protected: // WPlaylistWatcher reimplementation
    /* virtual */ void endTracksInsert();

    /* virtual */ void beginTracksRemove(int first, int last);
    /* virtual */ void beginTracksClear();

    /* virtual */ void trackUpdated(int index);

    /* virtual */ void playlistDestroyed();

public: // Properties
    int id() const;

    bool isDefault() const;
    bool isLoading() const;
    bool isLoaded () const;

    bool isValid() const;

    bool isCurrent() const;

    WTabTrack * parentTab() const;

    QList<int> folderIds() const;

    WPlaylist    * playlist() const;
    const WTrack * track   () const;

    QList<int> idPlaylist  () const;
    int        idTrack     () const;
    int        idFolderRoot() const;

    WTrack::State state() const;

    QUrl source() const;

    QString title() const;
    QUrl    cover() const;

    QString author() const;
    QString feed  () const;

    int duration() const;

    QDateTime date() const;

    WAbstractBackend::Quality quality() const;

    QUrl videoShot  () const;
    int  currentTime() const;

private:
    W_DECLARE_PRIVATE_COPY(WBookmarkTrack)

    friend class WTabTrack;
    friend class WTabTrackPrivate;
};

#endif // SK_NO_BOOKMARKTRACK
#endif // WBOOKMARKTRACK_H
