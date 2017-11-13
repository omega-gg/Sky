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

#ifndef WMODELPLAYLIST_H
#define WMODELPLAYLIST_H

// Qt includes
#include <QAbstractListModel>

// Sk includes
#include <WPlaylist>

#ifndef SK_NO_MODELPLAYLIST

// Forward declarations
class WModelPlaylistPrivate;

class SK_GUI_EXPORT WModelPlaylist : public QAbstractListModel, public WPlaylistWatcher,
                                     public WPrivatable
{
    Q_OBJECT

    Q_ENUMS(TrackRoles)

    Q_PROPERTY(WPlaylist * playlist READ playlist WRITE setPlaylist NOTIFY playlistChanged)

public: // Enums
    enum TrackRoles
    {
        RoleState = Qt::UserRole + 1,
        RoleSource,
        RoleTitle,
        RoleCover,
        RoleSelected,
        RoleCurrent
    };

public:
    explicit WModelPlaylist(QObject * parent = NULL);

public: // QAbstractItemModel reimplementation
    /* virtual */ QHash<int, QByteArray> roleNames() const;

    /* virtual */ int rowCount(const QModelIndex & parent = QModelIndex()) const;

    /* virtual */ QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

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
