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

#include "WModelTabs.h"

#ifndef SK_NO_MODELTABS

// Sk includes
#include <WAbstractTab>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include <private/Sk_p>

class SK_GUI_EXPORT WModelTabsPrivate : public WPrivate
{
public:
    WModelTabsPrivate(WModelTabs * p);

    /* virtual */ ~WModelTabsPrivate();

    void init();

public: // Functions
    void updateTab(int index);

public: // Variables
    WAbstractTabs * tabs;

    WAbstractTab * oldTab;

protected:
    W_DECLARE_PUBLIC(WModelTabs)
};

//-------------------------------------------------------------------------------------------------

WModelTabsPrivate::WModelTabsPrivate(WModelTabs * p) : WPrivate(p) {}

/* virtual */ WModelTabsPrivate::~WModelTabsPrivate()
{
    Q_Q(WModelTabs);

    if (tabs) tabs->unregisterWatcher(q);
}

//-------------------------------------------------------------------------------------------------

void WModelTabsPrivate::init()
{
    tabs = NULL;

    oldTab = NULL;

#ifdef QT_4
    Q_Q(WModelTabs);

    q->setRoleNames(q->roleNames());
#endif
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WModelTabsPrivate::updateTab(int index)
{
    Q_Q(WModelTabs);

    QModelIndex modelIndex = q->index(index);

    if (modelIndex.isValid()) emit q->dataChanged(modelIndex, modelIndex);
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

/* explicit */ WModelTabs::WModelTabs(QObject * parent)
    : QAbstractListModel(parent), WPrivatable(new WModelTabsPrivate(this))
{
    Q_D(WModelTabs); d->init();
}

//-------------------------------------------------------------------------------------------------
// QAbstractItemModel reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ QHash<int, QByteArray> WModelTabs::roleNames() const
{
    QHash<int, QByteArray> roles;

    roles.insert(WModelTabs::RoleItem, "item");

    return roles;
}

/* virtual */ int WModelTabs::rowCount(const QModelIndex &) const
{
    Q_D(const WModelTabs);

    if (d->tabs)
    {
         return d->tabs->count();
    }
    else return 0;
}

/* virtual */ QVariant WModelTabs::data(const QModelIndex & index, int role) const
{
    Q_D(const WModelTabs);

    if (d->tabs == NULL) return QVariant();

    if (index.row() < 0 || index.row() >= d->tabs->count()) return QVariant();

    if (role == RoleItem)
    {
        WAbstractTab * tab = d->tabs->tabAt(index.row());

        if (tab)
        {
             return QVariant::fromValue(qobject_cast<QObject *> (tab));
        }
        else return QVariant();
    }
    else return QVariant();
}

//-------------------------------------------------------------------------------------------------
// Protected WAbstractTabsWatcher reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WModelTabs::beginTabsInsert(int first, int last)
{
    beginInsertRows(QModelIndex(), first, last);
}

/* virtual */ void WModelTabs::endTabsInsert()
{
    endInsertRows();
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WModelTabs::beginTabsMove(int first, int last, int to)
{
    beginMoveRows(QModelIndex(), first, last, QModelIndex(), to);
}

/* virtual */ void WModelTabs::endTabsMove()
{
    endMoveRows();
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WModelTabs::beginTabsRemove(int first, int last)
{
    beginRemoveRows(QModelIndex(), first, last);
}

/* virtual */ void WModelTabs::endTabsRemove()
{
    endRemoveRows();
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WModelTabs::beginTabsClear()
{
    beginResetModel();
}

/* virtual */ void WModelTabs::endTabsClear()
{
    endResetModel();
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WModelTabs::tabsDestroyed()
{
    setTabs(NULL);
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

WAbstractTabs * WModelTabs::tabs() const
{
    Q_D(const WModelTabs); return d->tabs;
}

void WModelTabs::setTabs(WAbstractTabs * tabs)
{
    Q_D(WModelTabs);

    if (d->tabs == tabs) return;

    if (d->tabs)
    {
        d->tabs->unregisterWatcher(this);

        disconnect(d->tabs, 0, this, 0);
    }

    beginResetModel();

    d->tabs = tabs;

    endResetModel();

    if (d->tabs)
    {
        d->tabs->registerWatcher(this);

        connect(d->tabs, SIGNAL(countChanged()), this, SIGNAL(countChanged()));
    }

    emit tabsChanged();

    emit countChanged();
}

//-------------------------------------------------------------------------------------------------

int WModelTabs::count() const
{
    Q_D(const WModelTabs);

    if (d->tabs)
    {
         return d->tabs->count();
    }
    else return 0;
}

#endif // SK_NO_MODELTABS
