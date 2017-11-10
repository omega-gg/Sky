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

#include "WModelPlaylist.h"

#ifndef SK_NO_MODELPLAYLIST

// Qt includes
#include <QDateTime>

// Sk includes
#include <WPlaylist>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include "WModelPlaylist_p.h"

WModelPlaylistPrivate::WModelPlaylistPrivate(WModelPlaylist * p) : WPrivate(p) {}

/* virtual */ WModelPlaylistPrivate::~WModelPlaylistPrivate()
{
    Q_Q(WModelPlaylist);

    if (playlist) playlist->unregisterWatcher(q);
}

//-------------------------------------------------------------------------------------------------

void WModelPlaylistPrivate::init()
{
    playlist = NULL;

    oldTrack = NULL;

#ifdef QT_4
    Q_Q(WModelPlaylist);

    q->setRoleNames(q->roleNames());
#endif
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

/* explicit */ WModelPlaylist::WModelPlaylist(QObject * parent)
    : QAbstractListModel(parent), WPrivatable(new WModelPlaylistPrivate(this))
{
    Q_D(WModelPlaylist); d->init();
}

//-------------------------------------------------------------------------------------------------
// QAbstractItemModel reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ QHash<int, QByteArray> WModelPlaylist::roleNames() const
{
    QHash<int, QByteArray> roles;

    roles.insert(WModelPlaylist::RoleState,    "loadState");
    roles.insert(WModelPlaylist::RoleSource,   "source");
    roles.insert(WModelPlaylist::RoleTitle,    "title");
    roles.insert(WModelPlaylist::RoleCover,    "cover");
    roles.insert(WModelPlaylist::RoleSelected, "selected");
    roles.insert(WModelPlaylist::RoleCurrent,  "current");

    return roles;
}

/* virtual */ int WModelPlaylist::rowCount(const QModelIndex &) const
{
    Q_D(const WModelPlaylist);

    if (d->playlist)
    {
         return d->playlist->count();
    }
    else return 0;
}

/* virtual */ QVariant WModelPlaylist::data(const QModelIndex & index, int role) const
{
    Q_D(const WModelPlaylist);

    if (d->playlist == NULL) return QVariant();

    if (index.row() < 0 || index.row() >= d->playlist->count()) return QVariant();

    const WTrack * track = static_cast<const WTrack *> (d->playlist->trackPointerAt(index.row()));

    if      (role == RoleState)    return track->state();
    else if (role == RoleSource)   return track->source();
    else if (role == RoleTitle)    return track->title();
    else if (role == RoleCover)    return track->cover();
    else if (role == RoleSelected) return d->playlist->indexSelected(index.row());
    else if (role == RoleCurrent)  return (d->playlist->currentIndex() == index.row());
    else                           return QVariant();
}

//-------------------------------------------------------------------------------------------------
// WPlaylistWatcher implementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WModelPlaylist::beginTracksInsert(int first, int last)
{
    beginInsertRows(QModelIndex(), first, last);
}

/* virtual */ void WModelPlaylist::endTracksInsert()
{
    endInsertRows();
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WModelPlaylist::beginTracksMove(int first, int last, int to)
{
    beginMoveRows(QModelIndex(), first, last, QModelIndex(), to);
}

/* virtual */ void WModelPlaylist::endTracksMove()
{
    endMoveRows();
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WModelPlaylist::beginTracksRemove(int first, int last)
{
    beginRemoveRows(QModelIndex(), first, last);
}

/* virtual */ void WModelPlaylist::endTracksRemove()
{
    endRemoveRows();
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WModelPlaylist::trackUpdated(int index)
{
    QModelIndex modelIndex = this->index(index);

    if (modelIndex.isValid()) emit dataChanged(modelIndex, modelIndex);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WModelPlaylist::beginTracksClear()
{
    beginResetModel();
}

/* virtual */ void WModelPlaylist::endTracksClear()
{
    endResetModel();
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WModelPlaylist::selectedTracksChanged(const QList<int> & updatedIndexes)
{
    Q_ASSERT(updatedIndexes.count());

    QModelIndex modelIndexA = this->index(updatedIndexes.first());
    QModelIndex modelIndexB = this->index(updatedIndexes.last());

    if (modelIndexA.isValid() && modelIndexB.isValid())
    {
        emit dataChanged(modelIndexA, modelIndexB);
    }
}

/* virtual */ void WModelPlaylist::currentIndexChanged(int index)
{
    Q_D(WModelPlaylist);

    if (d->oldTrack)
    {
        int index = d->playlist->indexOf(d->oldTrack);

        if (index != -1) trackUpdated(index);
    }

    if (index != -1) trackUpdated(index);

    d->oldTrack = d->playlist->currentTrackPointer();
}

/* virtual */ void WModelPlaylist::playlistDestroyed()
{
    setPlaylist(NULL);
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

WPlaylist * WModelPlaylist::playlist() const
{
    Q_D(const WModelPlaylist); return d->playlist;
}

void WModelPlaylist::setPlaylist(WPlaylist * playlist)
{
    Q_D(WModelPlaylist);

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

#endif // SK_NO_MODELPLAYLIST
