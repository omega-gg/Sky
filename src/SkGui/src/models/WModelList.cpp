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

#include "WModelList.h"

#ifndef SK_NO_MODELLIST

#ifdef QT_4
// Qt includes
#include <QStringList>
#endif

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

void WModelListPrivate::init() {}

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
// QAbstractItemModel reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ QHash<int, QByteArray> WModelList::roleNames() const
{
    QHash<int, QByteArray> roles;

    roles.insert(WModelList::RoleTitle, "title");

    return roles;
}

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
