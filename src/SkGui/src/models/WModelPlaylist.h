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

#ifndef WMODELPLAYLIST_H
#define WMODELPLAYLIST_H

// Qt includes
#include <QAbstractListModel>

// Sk includes
#include <WPlaylist>

#ifndef SK_NO_MODELPLAYLIST

// Forward declarations
class WModelPlaylistPrivate;

//-------------------------------------------------------------------------------------------------
// WModelPlaylist
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WModelPlaylist : public QAbstractListModel, public WPlaylistWatcher,
                                     public WPrivatable
{
    Q_OBJECT

    Q_ENUMS(TrackRoles)

    Q_PROPERTY(WPlaylist * playlist READ playlist WRITE setPlaylist NOTIFY playlistChanged)

public: // Enums
    enum TrackRoles
    {
        RoleType = Qt::UserRole + 1,
        RoleState,
        RoleSource,
        RoleTitle,
        RoleCover,
        RoleDate,
        RoleSelected,
        RoleCurrent
    };

public:
    explicit WModelPlaylist(QObject * parent = NULL);

public: // QAbstractItemModel implementation
    /* virtual */ int rowCount(const QModelIndex & parent = QModelIndex()) const;

    /* virtual */ QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

public: // QAbstractItemModel reimplementation
    /* virtual */ QHash<int, QByteArray> roleNames() const;

protected: // WPlaylistWatcher implementation
    /* virtual */ void beginTracksInsert(int first, int last);
    /* virtual */ void endTracksInsert  ();

    /* virtual */ void beginTracksMove(int first, int last, int to);
    /* virtual */ void endTracksMove  ();

    /* virtual */ void beginTracksRemove(int first, int last);
    /* virtual */ void endTracksRemove  ();

    /* virtual */ void trackUpdated(int index);

    /* virtual */ void beginTracksClear();
    /* virtual */ void endTracksClear  ();

    /* virtual */ void selectedTracksChanged(const QList<int> & updatedIndexes);

    /* virtual */ void currentIndexChanged(int index);

    /* virtual */ void playlistDestroyed();

signals:
    void playlistChanged();

public: // Properties
    WPlaylist * playlist() const;
    void        setPlaylist(WPlaylist * playlist);

private:
    W_DECLARE_PRIVATE(WModelPlaylist)

    friend class WPlaylist;
    friend class WPlaylistPrivate;
};

#include <private/WModelPlaylist_p>

#endif // SK_NO_MODELPLAYLIST
#endif // WMODELPLAYLIST_H
