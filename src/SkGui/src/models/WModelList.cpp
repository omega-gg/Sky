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

#include "WModelList.h"

#ifndef SK_NO_MODELLIST

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include <private/Sk_p>

class SK_GUI_EXPORT WModelListPrivate : public WPrivate
{
public:
    WModelListPrivate(WModelList * p);

    void init();

public: // Variables
    QStringList titles;

protected:
    W_DECLARE_PUBLIC(WModelList)
};

//-------------------------------------------------------------------------------------------------

WModelListPrivate::WModelListPrivate(WModelList * p) : WPrivate(p) {}

//-------------------------------------------------------------------------------------------------

void WModelListPrivate::init()
{
#ifdef QT_4
    Q_Q(WModelList);

    q->setRoleNames(q->roleNames());
#endif
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

/* explicit */ WModelList::WModelList(QObject * parent)
    : QAbstractListModel(parent), WPrivatable(new WModelListPrivate(this))
{
    Q_D(WModelList); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ QString WModelList::titleAt(int index) const
{
    Q_D(const WModelList);

    if (index < 0 || index >= d->titles.count())
    {
         return QString();
    }
    else return d->titles.at(index);
}

//-------------------------------------------------------------------------------------------------
// QAbstractItemModel implementation
//-------------------------------------------------------------------------------------------------

/* virtual */ int WModelList::rowCount(const QModelIndex &) const
{
    Q_D(const WModelList);

    return d->titles.count();
}

/* virtual */ QVariant WModelList::data(const QModelIndex & index, int role) const
{
    Q_D(const WModelList);

    if (index.row() < 0 || index.row() >= d->titles.count())
    {
        return QVariant();
    }

    if (role == RoleTitle)
    {
        return d->titles.at(index.row());
    }
    else return QVariant();
}

//-------------------------------------------------------------------------------------------------
// QAbstractItemModel reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ QHash<int, QByteArray> WModelList::roleNames() const
{
    QHash<int, QByteArray> roles;

    roles.insert(WModelList::RoleTitle, "title");

    return roles;
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

QStringList WModelList::titles() const
{
    Q_D(const WModelList); return d->titles;
}

void WModelList::setTitles(const QStringList & titles)
{
    Q_D(WModelList);

    if (d->titles == titles) return;

    beginResetModel();

    d->titles = titles;

    endResetModel();

    emit titlesChanged();
}

#endif // SK_NO_MODELLIST
