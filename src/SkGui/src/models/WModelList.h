//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkGui module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#ifndef WMODELLIST_H
#define WMODELLIST_H

// Qt includes
#include <QAbstractListModel>
#ifdef QT_4
#include <QStringList>
#endif


// Sk includes
#include <Sk>

#ifndef SK_NO_MODELLIST

// Forward declarations
class WModelListPrivate;

class SK_GUI_EXPORT WModelList : public QAbstractListModel, public WPrivatable
{
    Q_OBJECT

    Q_ENUMS(ListRoles)

    Q_PROPERTY(QStringList titles READ titles WRITE setTitles NOTIFY titlesChanged)

public: // Enums
    enum ListRoles
    {
        RoleTitle = Qt::UserRole + 1
    };

public:
    explicit WModelList(QObject * parent = NULL);

public: // Interface
    Q_INVOKABLE QString titleAt(int index) const;

public: // QAbstractItemModel reimplementation
    /* virtual */ QHash<int, QByteArray> roleNames() const;

    /* virtual */ int rowCount(const QModelIndex & parent = QModelIndex()) const;

    /* virtual */ QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

signals:
    void titlesChanged();

public: // Properties
    QStringList titles() const;
    void        setTitles(const QStringList & titles);

private:
    W_DECLARE_PRIVATE(WModelList)
};

#endif // SK_NO_MODELLIST
#endif // WMODELLIST_H
