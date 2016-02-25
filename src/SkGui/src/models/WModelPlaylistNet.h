//=================================================================================================
/*
    Copyright (C) 2015-2016 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkGui module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#ifndef WMODELPLAYLISTNET_H
#define WMODELPLAYLISTNET_H

// Qt includes
#include <QAbstractListModel>

// Sk includes
#include <WAbstractPlaylist>

#ifndef SK_NO_MODELPLAYLISTNET

class WModelPlaylistNetPrivate;
class WPlaylistNet;

class SK_GUI_EXPORT WModelPlaylistNet : public QAbstractListModel, public WAbstractPlaylistWatcher,
                                        public WPrivatable
{
    Q_OBJECT

    Q_ENUMS(TrackRoles)

    Q_PROPERTY(WPlaylistNet * playlist READ playlist WRITE setPlaylist NOTIFY playlistChanged)

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
    explicit WModelPlaylistNet(QObject * parent = NULL);

public: // QAbstractItemModel reimplementation
    /* virtual */ QHash<int, QByteArray> roleNames() const;

    /* virtual */ int rowCount(const QModelIndex & parent = QModelIndex()) const;

    /* virtual */ QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

signals:
    void playlistChanged();

protected: // WAbstractPlaylistWatcher implementation
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

public: // Properties
    WPlaylistNet * playlist() const;
    void           setPlaylist(WPlaylistNet * playlist);

private:
    W_DECLARE_PRIVATE(WModelPlaylistNet)

    friend class WAbstractPlaylist;
    friend class WAbstractPlaylistPrivate;
};

#include <private/WModelPlaylistNet_p>

#endif // SK_NO_MODELPLAYLISTNET
#endif // WMODELPLAYLISTNET_H
