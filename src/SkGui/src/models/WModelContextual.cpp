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

#include "WModelContextual.h"

#ifndef SK_NO_MODELCONTEXTUAL

// Private include
#include "WModelContextual_p.h"

//=================================================================================================
// WModelContextualBase
//=================================================================================================

class WModelContextualBase : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit WModelContextualBase(WModelContextual * model = NULL);

public: // QAbstractItemModel reimplementation
    /* virtual */ int rowCount(const QModelIndex & parent = QModelIndex()) const;

    /* virtual */ QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

private: // Variables
    WModelContextual * _model;

private:
    Q_DISABLE_COPY(WModelContextualBase)

    friend class WModelContextual;
    friend class WModelContextualPrivate;
};

//-------------------------------------------------------------------------------------------------

/* explicit */ WModelContextualBase::WModelContextualBase(WModelContextual * model)
    : QAbstractListModel(model)
{
    _model = model;
}

//-------------------------------------------------------------------------------------------------
// QAbstractItemModel reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ int WModelContextualBase::rowCount(const QModelIndex &) const
{
    WDeclarativeContextualPage * page = _model->d_func()->currentPage;

    if (page)
    {
         return page->count();
    }
    else return 0;
}

/* virtual */ QVariant WModelContextualBase::data(const QModelIndex & index, int role) const
{
    WDeclarativeContextualPage * page = _model->d_func()->currentPage;

    if (page == NULL) return QVariant();

    int row = index.row();

    if (row < 0 || row >= page->count())
    {
        return QVariant();
    }

    const WDeclarativeContextualItem * item = page->itemAt(row);

    if      (role == WModelContextual::RoleType)     return item->type;
    else if (role == WModelContextual::RoleId)       return item->id;
    else if (role == WModelContextual::RoleTitle)    return item->title;
    else if (role == WModelContextual::RoleIcon)     return item->icon;
    else if (role == WModelContextual::RoleIconSize) return item->iconSize;
    else if (role == WModelContextual::RoleEnabled)  return item->enabled;
    else if (role == WModelContextual::RoleSelected) return (page->selectedIndex() == row);
    else if (role == WModelContextual::RoleCurrent)  return (page->currentIndex() == row);
    else                                             return QVariant();
}

//=================================================================================================
// WModelContextualPrivate
//=================================================================================================

WModelContextualPrivate::WModelContextualPrivate(WModelContextual * p) : WPrivate(p) {}

/* virtual */ WModelContextualPrivate::~WModelContextualPrivate()
{
    Q_Q(WModelContextual);

    if (currentPage) currentPage->unregisterWatcher(q);
}

//-------------------------------------------------------------------------------------------------

void WModelContextualPrivate::init()
{
    Q_Q(WModelContextual);

    model = new WModelContextualBase(q);

    currentPage = NULL;

    oldSelected = NULL;
    oldCurrent  = NULL;

    q->setSourceModel(model);

    q->setDynamicSortFilter(true);

#ifdef QT_4
    q->setRoleNames(q->roleNames());
#endif
}

//=================================================================================================
// WModelContextual
//=================================================================================================

/* explicit */ WModelContextual::WModelContextual(QObject * parent)
    : QSortFilterProxyModel(parent), WPrivatable(new WModelContextualPrivate(this))
{
    Q_D(WModelContextual); d->init();
}

//-------------------------------------------------------------------------------------------------
// QAbstractItemModel reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ QHash<int, QByteArray> WModelContextual::roleNames() const
{
    QHash<int, QByteArray> roles;

    roles.insert(WModelContextual::RoleType,     "type");
    roles.insert(WModelContextual::RoleId,       "id");
    roles.insert(WModelContextual::RoleTitle,    "title");
    roles.insert(WModelContextual::RoleIcon,     "icon");
    roles.insert(WModelContextual::RoleIconSize, "iconSize");
    roles.insert(WModelContextual::RoleEnabled,  "isEnabled");
    roles.insert(WModelContextual::RoleSelected, "isSelected");
    roles.insert(WModelContextual::RoleCurrent,  "isCurrent");

    return roles;
}

//-------------------------------------------------------------------------------------------------
// Protected QSortFilterProxyModel reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ bool WModelContextual::filterAcceptsRow(int source_row, const QModelIndex &) const
{
    Q_D(const WModelContextual);

    if (d->currentPage == NULL) return true;

    const WDeclarativeContextualItem * item = d->currentPage->itemAt(source_row);

    return item->visible;
}

//-------------------------------------------------------------------------------------------------
// Protected WContextualPageWatcher implementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WModelContextual::beginItemsInsert(int first, int last)
{
    Q_D(WModelContextual);

    d->model->beginInsertRows(QModelIndex(), first, last);
}

/* virtual */ void WModelContextual::endItemsInsert()
{
    Q_D(WModelContextual);

    d->model->endInsertRows();
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WModelContextual::itemUpdated(int index)
{
    Q_D(WModelContextual);

    QModelIndex modelIndex = d->model->index(index);

    if (modelIndex.isValid())
    {
        emit d->model->dataChanged(modelIndex, modelIndex);
    }
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WModelContextual::beginItemsClear()
{
    Q_D(WModelContextual);

    d->model->beginResetModel();
}

/* virtual */ void WModelContextual::endItemsClear()
{
    Q_D(WModelContextual);

    d->model->endResetModel();
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WModelContextual::selectedIndexChanged(int index)
{
    Q_D(WModelContextual);

    if (d->oldSelected)
    {
        int index = d->currentPage->indexOf(d->oldSelected);

        if (index != -1) itemUpdated(index);
    }

    if (index != -1) itemUpdated(index);

    d->oldSelected = d->currentPage->itemSelected();
}

/* virtual */ void WModelContextual::currentIndexChanged(int index)
{
    Q_D(WModelContextual);

    if (d->oldCurrent)
    {
        int index = d->currentPage->indexOf(d->oldCurrent);

        if (index != -1) itemUpdated(index);
    }

    if (index != -1) itemUpdated(index);

    d->oldCurrent = d->currentPage->itemCurrent();
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WModelContextual::contextualPageDestroyed()
{
    setCurrentPage(NULL);
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

WDeclarativeContextualPage * WModelContextual::currentPage() const
{
    Q_D(const WModelContextual); return d->currentPage;
}

void WModelContextual::setCurrentPage(WDeclarativeContextualPage * page)
{
    Q_D(WModelContextual);

    if (d->currentPage == page) return;

    if (d->currentPage) d->currentPage->unregisterWatcher(this);

    beginResetModel();

    d->currentPage = page;

    endResetModel();

    if (d->currentPage)
    {
        d->currentPage->registerWatcher(this);

        d->oldSelected = d->currentPage->itemSelected();
        d->oldCurrent  = d->currentPage->itemCurrent ();
    }
    else
    {
        d->oldSelected = NULL;
        d->oldCurrent  = NULL;
    }

    emit currentPageChanged();
}

#endif // SK_NO_MODELCONTEXTUAL

#include "WModelContextual.moc"
