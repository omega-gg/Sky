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

public: // Virtual interface
    virtual QString toVbml() const;

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

    WTrack::Type type() const;

    WTrack::State state() const;

    QString source() const;

    QString title() const;
    QString cover() const;

    QString author() const;
    QString feed  () const;

    int duration() const;

    QDateTime date() const;

    QString videoShot() const;

    int currentTime() const;

    QString subtitle() const;

private:
    W_DECLARE_PRIVATE_COPY(WBookmarkTrack)

    friend class WTabTrack;
    friend class WTabTrackPrivate;
};

#endif // SK_NO_BOOKMARKTRACK
#endif // WBOOKMARKTRACK_H
