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

#include "WModelPlaylist.h"

#ifndef SK_NO_MODELPLAYLIST

// Qt includes
#include <QDateTime>

// Sk includes
#include <WPlaylist>

//=================================================================================================
// WModelPlaylistPrivate
//=================================================================================================

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

//=================================================================================================
// WModelPlaylist
//=================================================================================================

/* explicit */ WModelPlaylist::WModelPlaylist(QObject * parent)
    : QAbstractListModel(parent), WPrivatable(new WModelPlaylistPrivate(this))
{
    Q_D(WModelPlaylist); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ int WModelPlaylist::idAt(int index) const
{
    Q_D(const WModelPlaylist);

    if (d->playlist)
    {
        return d->playlist->idAt(index);
    }
    else return -1;
}

/* Q_INVOKABLE */ int WModelPlaylist::indexAt(int index) const
{
    return index;
}

/* Q_INVOKABLE */ int WModelPlaylist::indexFromId(int id) const
{
    Q_D(const WModelPlaylist);

    if (d->playlist)
    {
        return d->playlist->indexFromId(id);
    }
    else return -1;
}

/* Q_INVOKABLE */ int WModelPlaylist::indexFromIndex(int index) const
{
    return index;
}

/* Q_INVOKABLE */ int WModelPlaylist::indexFromRole(int role,
                                                    const QVariant & value) const
{
    QModelIndex index = this->index(0, 0);

    QModelIndexList indexes = match(index, role, value, 1, Qt::MatchExactly);

    if (indexes.isEmpty())
    {
        return -1;
    }
    else return indexes.first().row();
}

//-------------------------------------------------------------------------------------------------
// QAbstractItemModel implementation
//-------------------------------------------------------------------------------------------------

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

    int row = index.row();

    if (row < 0 || row >= d->playlist->count())
    {
        return QVariant();
    }

    const WTrack * track = static_cast<const WTrack *> (d->playlist->trackPointerAt(row));

    if      (role == RoleId)       return track->id();
    else if (role == RoleType)     return track->type();
    else if (role == RoleState)    return track->state();
    else if (role == RoleSource)   return track->source();
    else if (role == RoleTitle)    return track->title();
    else if (role == RoleCover)    return track->cover();
    else if (role == RoleDate)     return track->date();
    else if (role == RoleSelected) return d->playlist->indexSelected(row);
    else if (role == RoleCurrent)  return (d->playlist->currentIndex() == row);
    else                           return QVariant();
}

//-------------------------------------------------------------------------------------------------
// QAbstractItemModel reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ QHash<int, QByteArray> WModelPlaylist::roleNames() const
{
    QHash<int, QByteArray> roles;

    roles.insert(WModelPlaylist::RoleId,       "id");
    roles.insert(WModelPlaylist::RoleType,     "type");
    roles.insert(WModelPlaylist::RoleState,    "loadState");
    roles.insert(WModelPlaylist::RoleSource,   "source");
    roles.insert(WModelPlaylist::RoleTitle,    "title");
    roles.insert(WModelPlaylist::RoleCover,    "cover");
    roles.insert(WModelPlaylist::RoleDate,     "date");
    roles.insert(WModelPlaylist::RoleSelected, "selected");
    roles.insert(WModelPlaylist::RoleCurrent,  "current");

    return roles;
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

//=================================================================================================
// WModelPlaylistFilteredPrivate
//=================================================================================================

class SK_GUI_EXPORT WModelPlaylistFilteredPrivate : public WPrivate
{
public:
    WModelPlaylistFilteredPrivate(WModelPlaylistFiltered * p);

    void init();

public: // Variables
    WModelPlaylist * model;

    Qt::SortOrder sortOrder;

protected:
    W_DECLARE_PUBLIC(WModelPlaylistFiltered)
};

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

WModelPlaylistFilteredPrivate::
WModelPlaylistFilteredPrivate(WModelPlaylistFiltered * p) : WPrivate(p) {}

void WModelPlaylistFilteredPrivate::init()
{
    Q_Q(WModelPlaylistFiltered);

    model = NULL;

    sortOrder = Qt::AscendingOrder;

    q->setDynamicSortFilter(true);
}

//=================================================================================================
// WModelPlaylistFiltered
//=================================================================================================

/* explicit */ WModelPlaylistFiltered::WModelPlaylistFiltered(QObject * parent)
    : QSortFilterProxyModel(parent), WPrivatable(new WModelPlaylistFilteredPrivate(this))
{
    Q_D(WModelPlaylistFiltered); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ int WModelPlaylistFiltered::idAt(int index) const
{
    QModelIndex modelIndex = this->index(index, 0);

    if (modelIndex.isValid())
    {
        return data(modelIndex, WModelPlaylist::RoleId).toInt();
    }
    else return -1;
}

/* Q_INVOKABLE */ int WModelPlaylistFiltered::indexAt(int index) const
{
    WPlaylist * playlist = this->playlist();

    if (playlist)
    {
        return playlist->indexFromId(idAt(index));
    }
    else return -1;
}

/* Q_INVOKABLE */ int WModelPlaylistFiltered::indexFromId(int id) const
{
    return indexFromRole(WModelPlaylist::RoleId, id);
}

/* Q_INVOKABLE */ int WModelPlaylistFiltered::indexFromIndex(int index) const
{
    WPlaylist * playlist = this->playlist();

    if (playlist)
    {
        return indexFromId(playlist->idAt(index));
    }
    else return -1;
}

/* Q_INVOKABLE */ int WModelPlaylistFiltered::indexFromRole(int role,
                                                            const QVariant & value) const
{
    QModelIndex index = this->index(0, 0);

    QModelIndexList indexes = match(index, role, value, 1, Qt::MatchExactly);

    if (indexes.isEmpty())
    {
        return -1;
    }
    else return indexes.first().row();
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

WModelPlaylist * WModelPlaylistFiltered::model() const
{
    Q_D(const WModelPlaylistFiltered); return d->model;
}

void WModelPlaylistFiltered::setModel(WModelPlaylist * model)
{
    Q_D(WModelPlaylistFiltered);

    if (d->model == model) return;

    if (d->model) disconnect(d->model, 0, this, 0);

    d->model = model;

    if (model)
    {
        setSourceModel(model);

        connect(model, SIGNAL(playlistChanged()), this, SIGNAL(playlistChanged()));

        if (sortRole()) sort(0, d->sortOrder);
    }

    emit modelChanged();
}

WPlaylist * WModelPlaylistFiltered::playlist() const
{
    Q_D(const WModelPlaylistFiltered);

    if (d->model)
    {
        return d->model->playlist();
    }
    else return NULL;
}

void WModelPlaylistFiltered::setPlaylist(WPlaylist * playlist)
{
    Q_D(WModelPlaylistFiltered);

    if (d->model) d->model->setPlaylist(playlist);
}

Qt::SortOrder WModelPlaylistFiltered::sortOrder() const
{
    Q_D(const WModelPlaylistFiltered); return d->sortOrder;
}

void WModelPlaylistFiltered::setSortOrder(Qt::SortOrder order)
{
    Q_D(WModelPlaylistFiltered);

    if (d->sortOrder == order) return;

    d->sortOrder = order;

    if (sortRole()) sort(0, order);

    emit sortOrderChanged();
}

#endif // SK_NO_MODELPLAYLIST
