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

#include "WModelLibraryFolder.h"

#ifndef SK_NO_MODELLIBRARYFOLDER

//=================================================================================================
// WModelLibraryFolderPrivate
//=================================================================================================

#include "WModelLibraryFolder_p.h"

WModelLibraryFolderPrivate::WModelLibraryFolderPrivate(WModelLibraryFolder * p) : WPrivate(p) {}

/* virtual */ WModelLibraryFolderPrivate::~WModelLibraryFolderPrivate()
{
    Q_Q(WModelLibraryFolder);

    if (folder) folder->unregisterWatcher(q);
}

//-------------------------------------------------------------------------------------------------

void WModelLibraryFolderPrivate::init()
{
    folder = NULL;

    oldItem = NULL;

#ifndef QT_LATEST
    Q_Q(WModelLibraryFolder);

    q->setRoleNames(q->roleNames());
#endif
}

//=================================================================================================
// WModelLibraryFolder
//=================================================================================================

/* explicit */ WModelLibraryFolder::WModelLibraryFolder(QObject * parent)
    : QAbstractListModel(parent), WPrivatable(new WModelLibraryFolderPrivate(this))
{
    Q_D(WModelLibraryFolder); d->init();
}

//-------------------------------------------------------------------------------------------------
// QAbstractItemModel reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ QHash<int, QByteArray> WModelLibraryFolder::roleNames() const
{
    QHash<int, QByteArray> roles;

    roles.insert(WModelLibraryFolder::RoleId,         "id");
    roles.insert(WModelLibraryFolder::RoleType,       "type");
    roles.insert(WModelLibraryFolder::RoleState,      "loadState");
    roles.insert(WModelLibraryFolder::RoleStateQuery, "loadStateQuery");
    roles.insert(WModelLibraryFolder::RoleSource,     "source");
    roles.insert(WModelLibraryFolder::RoleTitle,      "title");
    roles.insert(WModelLibraryFolder::RoleCover,      "cover");
    roles.insert(WModelLibraryFolder::RoleLabel,      "label");
    roles.insert(WModelLibraryFolder::RoleCurrent,    "current");

    return roles;
}

/* virtual */ int WModelLibraryFolder::rowCount(const QModelIndex &) const
{
    Q_D(const WModelLibraryFolder);

    if (d->folder)
    {
         return d->folder->count();
    }
    else return 0;
}

/* virtual */ QVariant WModelLibraryFolder::data(const QModelIndex & index, int role) const
{
    Q_D(const WModelLibraryFolder);

    if (d->folder == NULL) return QVariant();

    if (index.row() < 0 || index.row() >= d->folder->count()) return QVariant();

    const WLibraryFolderItem * item = d->folder->itemAt(index.row());

    if      (role == RoleId)         return item->id;
    else if (role == RoleType)       return item->type;
    else if (role == RoleState)      return item->state;
    else if (role == RoleStateQuery) return item->stateQuery;
    else if (role == RoleSource)     return item->source;
    else if (role == RoleTitle)      return item->title;
    else if (role == RoleCover)      return item->cover;
    else if (role == RoleLabel)      return item->label;
    else if (role == RoleCurrent)    return (d->folder->currentIndex() == index.row());
    else                             return QVariant();
}

//-------------------------------------------------------------------------------------------------
// WLibraryFolderWatcher implementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WModelLibraryFolder::beginItemsInsert(int first, int last)
{
    beginInsertRows(QModelIndex(), first, last);
}

/* virtual */ void WModelLibraryFolder::endItemsInsert()
{
    endInsertRows();
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WModelLibraryFolder::beginItemsMove(int first, int last, int to)
{
    beginMoveRows(QModelIndex(), first, last, QModelIndex(), to);
}

/* virtual */ void WModelLibraryFolder::endItemsMove()
{
    endMoveRows();
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WModelLibraryFolder::beginItemsRemove(int first, int last)
{
    beginRemoveRows(QModelIndex(), first, last);
}

/* virtual */ void WModelLibraryFolder::endItemsRemove()
{
    endRemoveRows();
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WModelLibraryFolder::itemUpdated(int index)
{
    QModelIndex modelIndex = this->index(index);

    if (modelIndex.isValid()) emit dataChanged(modelIndex, modelIndex);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WModelLibraryFolder::beginItemsClear()
{
    beginResetModel();
}

/* virtual */ void WModelLibraryFolder::endItemsClear()
{
    endResetModel();
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WModelLibraryFolder::currentIndexChanged(int index)
{
    Q_D(WModelLibraryFolder);

    if (d->oldItem)
    {
        int index = d->folder->indexOf(d->oldItem);

        if (index != -1) itemUpdated(index);
    }

    if (index != -1) itemUpdated(index);

    d->oldItem = d->folder->currentItemPointer();
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WModelLibraryFolder::folderDestroyed()
{
    setFolder(NULL);
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

WLibraryFolder * WModelLibraryFolder::folder() const
{
    Q_D(const WModelLibraryFolder); return d->folder;
}

void WModelLibraryFolder::setFolder(WLibraryFolder * folder)
{
    Q_D(WModelLibraryFolder);

    if (d->folder == folder) return;

    if (d->folder) d->folder->unregisterWatcher(this);

    beginResetModel();

    d->folder = folder;

    endResetModel();

    if (d->folder)
    {
        d->folder->registerWatcher(this);

        d->oldItem = d->folder->currentItemPointer();
    }
    else d->oldItem = NULL;

    emit folderChanged();
}

//=================================================================================================
// WModelLibraryFolderFiltered
//=================================================================================================

class SK_GUI_EXPORT WModelLibraryFolderFilteredPrivate : public WPrivate
{
public:
    WModelLibraryFolderFilteredPrivate(WModelLibraryFolderFiltered * p);

    void init();

public: // Variables
    WModelLibraryFolder * model;

    WLibraryItem::Types filter;

    bool local;

protected:
    W_DECLARE_PUBLIC(WModelLibraryFolderFiltered)
};

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

WModelLibraryFolderFilteredPrivate::
WModelLibraryFolderFilteredPrivate(WModelLibraryFolderFiltered * p) : WPrivate(p) {}

void WModelLibraryFolderFilteredPrivate::init()
{
    Q_Q(WModelLibraryFolderFiltered);

    model = NULL;

    filter = 0;

    local = false;

    q->setDynamicSortFilter(true);
}

//=================================================================================================
// WModelLibraryFolderFiltered
//=================================================================================================

/* explicit */ WModelLibraryFolderFiltered::WModelLibraryFolderFiltered(QObject * parent)
    : QSortFilterProxyModel(parent), WPrivatable(new WModelLibraryFolderFilteredPrivate(this))
{
    Q_D(WModelLibraryFolderFiltered); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ int WModelLibraryFolderFiltered::idAt(int index)
{
    QModelIndex modelIndex = this->index(index, 0);

    if (modelIndex.isValid())
    {
         return data(modelIndex, WModelLibraryFolder::RoleId).toInt();
    }
    else return -1;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ int WModelLibraryFolderFiltered::indexFromId(int id)
{
    QModelIndex index = this->index(0, 0);

    QModelIndexList indexes = match(index, WModelLibraryFolder::RoleId, id, 1, Qt::MatchExactly);

    if (indexes.isEmpty())
    {
         return -1;
    }
    else return indexes.first().row();
}

//-------------------------------------------------------------------------------------------------
// Protected QSortFilterProxyModel reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ bool WModelLibraryFolderFiltered::filterAcceptsRow(int source_row,
                                                                 const QModelIndex &) const
{
    Q_D(const WModelLibraryFolderFiltered);

    if (d->filter)
    {
        WLibraryFolder * folder = d->model->folder();

        const WLibraryFolderItem * item = folder->itemAt(source_row);

        if (d->local && item->isLocal() == false)
        {
             return false;
        }
        else return d->filter.testFlag(item->type);
    }
    else if (d->local)
    {
        WLibraryFolder * folder = d->model->folder();

        const WLibraryFolderItem * item = folder->itemAt(source_row);

        return item->isLocal();
    }
    else return true;
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

WModelLibraryFolder * WModelLibraryFolderFiltered::model() const
{
    Q_D(const WModelLibraryFolderFiltered); return d->model;
}

void WModelLibraryFolderFiltered::setModel(WModelLibraryFolder * model)
{
    Q_D(WModelLibraryFolderFiltered);

    if (d->model == model) return;

    if (d->model) disconnect(d->model, 0, this, 0);

    d->model = model;

    if (model)
    {
        setSourceModel(model);

        connect(model, SIGNAL(folderChanged()), this, SIGNAL(folderChanged()));
    }

    emit modelChanged();
}

//-------------------------------------------------------------------------------------------------

WLibraryFolder * WModelLibraryFolderFiltered::folder() const
{
    Q_D(const WModelLibraryFolderFiltered);

    if (d->model) return d->model->folder();
    else          return NULL;
}

void WModelLibraryFolderFiltered::setFolder(WLibraryFolder * folder)
{
    Q_D(WModelLibraryFolderFiltered);

    if (d->model) d->model->setFolder(folder);
}

//-------------------------------------------------------------------------------------------------

WLibraryItem::Types WModelLibraryFolderFiltered::filter() const
{
    Q_D(const WModelLibraryFolderFiltered); return d->filter;
}

void WModelLibraryFolderFiltered::setFilter(WLibraryItem::Types types)
{
    Q_D(WModelLibraryFolderFiltered);

    if (d->filter == types) return;

    beginResetModel();

    d->filter = types;

    endResetModel();

    emit filterChanged();
}

void WModelLibraryFolderFiltered::resetFilter()
{
    Q_D(WModelLibraryFolderFiltered);

    if (d->filter == 0) return;

    beginResetModel();

    d->filter = 0;

    endResetModel();

    emit filterChanged();
}

//-------------------------------------------------------------------------------------------------

int WModelLibraryFolderFiltered::qmlFilter() const
{
    Q_D(const WModelLibraryFolderFiltered); return d->filter;
}

void WModelLibraryFolderFiltered::qmlSetFilter(int types)
{
    setFilter(static_cast<WLibraryItem::Types> (types));
}

//-------------------------------------------------------------------------------------------------

bool WModelLibraryFolderFiltered::local() const
{
    Q_D(const WModelLibraryFolderFiltered); return d->local;
}

void WModelLibraryFolderFiltered::setLocal(bool local)
{
    Q_D(WModelLibraryFolderFiltered);

    if (d->local == local) return;

    beginResetModel();

    d->local = local;

    endResetModel();

    emit localChanged();
}

#endif // SK_NO_MODELLIBRARYFOLDER
