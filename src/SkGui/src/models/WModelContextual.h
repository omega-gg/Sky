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

#ifndef WMODELCONTEXTUAL_H
#define WMODELCONTEXTUAL_H

// Qt includes
#include <QSortFilterProxyModel>

// Sk includes
#include <WDeclarativeContextualPage>

#ifndef SK_NO_MODELCONTEXTUAL

class WModelContextualPrivate;

class SK_GUI_EXPORT WModelContextual : public QSortFilterProxyModel, public WContextualPageWatcher,
                                       public WPrivatable
{
    Q_OBJECT

    Q_PROPERTY(WDeclarativeContextualPage * currentPage READ currentPage WRITE setCurrentPage
               NOTIFY currentPageChanged)

public: // Enums
    enum ItemRoles
    {
        RoleType = Qt::UserRole + 1,
        RoleId,
        RoleTitle,
        RoleIcon,
        RoleIconSize,
        RoleEnabled,
        RoleSelected,
        RoleCurrent
    };

public:
    explicit WModelContextual(QObject * parent = NULL);

public: // QAbstractItemModel reimplementation
    /* virtual */ QHash<int, QByteArray> roleNames() const;

protected: // QSortFilterProxyModel reimplementation
    /* virtual */ bool filterAcceptsRow(int source_row, const QModelIndex & source_parent) const;

protected: // WContextualPageWatcher implementation
    /* virtual */ void beginItemsInsert(int first, int last);
    /* virtual */ void endItemsInsert();

    /* virtual */ void itemUpdated(int index);

    /* virtual */ void beginItemsClear();
    /* virtual */ void endItemsClear  ();

    /* virtual */ void selectedIndexChanged(int index);
    /* virtual */ void currentIndexChanged (int index);

    /* virtual */ void contextualPageDestroyed();

signals:
    void currentPageChanged();

public: // Properties
    WDeclarativeContextualPage * currentPage() const;
    void                         setCurrentPage(WDeclarativeContextualPage * page);

private:
    W_DECLARE_PRIVATE(WModelContextual)

    friend class WModelContextualBase;
};

#endif // SK_NO_MODELCONTEXTUAL
#endif // WMODELCONTEXTUAL_H
