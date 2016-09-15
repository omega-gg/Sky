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

#include "WModelPlaylistNet.h"

#ifndef SK_NO_MODELPLAYLISTNET

// Qt includes
#include <QDateTime>

// Sk includes
#include <WPlaylistNet>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include "WModelPlaylistNet_p.h"

WModelPlaylistNetPrivate::WModelPlaylistNetPrivate(WModelPlaylistNet * p) : WPrivate(p) {}

/* virtual */ WModelPlaylistNetPrivate::~WModelPlaylistNetPrivate()
{
    Q_Q(WModelPlaylistNet);

    if (playlist) playlist->unregisterWatcher(q);
}

//-------------------------------------------------------------------------------------------------

void WModelPlaylistNetPrivate::init()
{
    playlist = NULL;

    oldTrack = NULL;

#ifndef QT_LATEST
    Q_Q(WModelPlaylistNet);

    q->setRoleNames(q->roleNames());
#endif
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

/* explicit */ WModelPlaylistNet::WModelPlaylistNet(QObject * parent)
    : QAbstractListModel(parent), WPrivatable(new WModelPlaylistNetPrivate(this))
{
    Q_D(WModelPlaylistNet); d->init();
}

//-------------------------------------------------------------------------------------------------
// QAbstractItemModel reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ QHash<int, QByteArray> WModelPlaylistNet::roleNames() const
{
    QHash<int, QByteArray> roles;

    roles.insert(WModelPlaylistNet::RoleState,    "loadState");
    roles.insert(WModelPlaylistNet::RoleSource,   "source");
    roles.insert(WModelPlaylistNet::RoleTitle,    "title");
    roles.insert(WModelPlaylistNet::RoleCover,    "cover");
    roles.insert(WModelPlaylistNet::RoleSelected, "selected");
    roles.insert(WModelPlaylistNet::RoleCurrent,  "current");

    return roles;
}

/* virtual */ int WModelPlaylistNet::rowCount(const QModelIndex &) const
{
    Q_D(const WModelPlaylistNet);

    if (d->playlist)
    {
         return d->playlist->count();
    }
    else return 0;
}

/* virtual */ QVariant WModelPlaylistNet::data(const QModelIndex & index, int role) const
{
    Q_D(const WModelPlaylistNet);

    if (d->playlist == NULL) return QVariant();

    if (index.row() < 0 || index.row() >= d->playlist->count()) return QVariant();

    const WTrackNet * track
                    = static_cast<const WTrackNet *> (d->playlist->trackPointerAt(index.row()));

    if      (role == RoleState)    return track->state();
    else if (role == RoleSource)   return track->source();
    else if (role == RoleTitle)    return track->title();
    else if (role == RoleCover)    return track->cover();
    else if (role == RoleSelected) return d->playlist->indexSelected(index.row());
    else if (role == RoleCurrent)  return (d->playlist->currentIndex() == index.row());
    else                           return QVariant();
}

//-------------------------------------------------------------------------------------------------
// WAbstractPlaylistWatcher implementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WModelPlaylistNet::beginTracksInsert(int first, int last)
{
    beginInsertRows(QModelIndex(), first, last);
}

/* virtual */ void WModelPlaylistNet::endTracksInsert()
{
    endInsertRows();
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WModelPlaylistNet::beginTracksMove(int first, int last, int to)
{
    beginMoveRows(QModelIndex(), first, last, QModelIndex(), to);
}

/* virtual */ void WModelPlaylistNet::endTracksMove()
{
    endMoveRows();
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WModelPlaylistNet::beginTracksRemove(int first, int last)
{
    beginRemoveRows(QModelIndex(), first, last);
}

/* virtual */ void WModelPlaylistNet::endTracksRemove()
{
    endRemoveRows();
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WModelPlaylistNet::trackUpdated(int index)
{
    QModelIndex modelIndex = this->index(index);

    if (modelIndex.isValid()) emit dataChanged(modelIndex, modelIndex);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WModelPlaylistNet::beginTracksClear()
{
    beginResetModel();
}

/* virtual */ void WModelPlaylistNet::endTracksClear()
{
    endResetModel();
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WModelPlaylistNet::selectedTracksChanged(const QList<int> & updatedIndexes)
{
    Q_ASSERT(updatedIndexes.count());

    QModelIndex modelIndexA = this->index(updatedIndexes.first());
    QModelIndex modelIndexB = this->index(updatedIndexes.last());

    if (modelIndexA.isValid() && modelIndexB.isValid())
    {
        emit dataChanged(modelIndexA, modelIndexB);
    }
}

/* virtual */ void WModelPlaylistNet::currentIndexChanged(int index)
{
    Q_D(WModelPlaylistNet);

    if (d->oldTrack)
    {
        int index = d->playlist->indexOf(d->oldTrack);

        if (index != -1) trackUpdated(index);
    }

    if (index != -1) trackUpdated(index);

    d->oldTrack = d->playlist->currentTrackPointer();
}

/* virtual */ void WModelPlaylistNet::playlistDestroyed()
{
    setPlaylist(NULL);
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

WPlaylistNet * WModelPlaylistNet::playlist() const
{
    Q_D(const WModelPlaylistNet); return d->playlist;
}

void WModelPlaylistNet::setPlaylist(WPlaylistNet * playlist)
{
    Q_D(WModelPlaylistNet);

    if (d->playlist == playlist) return;

    if (d->playlist) d->playlist->unregisterWatcher(this);

    beginResetModel();

    d->playlist = playlist;

    endResetModel();

    if (d->playlist)
    {
        d->playlist->registerWatcher(this);

        d->oldTrack = d->playlist->currentTrackPointer();
    }
    else d->oldTrack = NULL;

    emit playlistChanged();
}

#endif // SK_NO_MODELPLAYLISTNET
