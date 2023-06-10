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

    Q_INVOKABLE void clear() const;

public: // QAbstractItemModel implementation
    /* virtual */ int rowCount(const QModelIndex & parent = QModelIndex()) const;

    /* virtual */ QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

public: // QAbstractItemModel reimplementation
    /* virtual */ QHash<int, QByteArray> roleNames() const;

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
