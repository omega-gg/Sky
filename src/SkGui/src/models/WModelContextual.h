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
        RoleCurrent
    };

public:
    explicit WModelContextual(QObject * parent = NULL);

protected: // QSortFilterProxyModel reimplementation
    /* virtual */ QHash<int, QByteArray> roleNames() const;

    /* virtual */ bool filterAcceptsRow(int source_row, const QModelIndex & source_parent) const;

protected: // WContextualPageWatcher implementation
    /* virtual */ void beginItemsInsert(int first, int last);
    /* virtual */ void endItemsInsert();

    /* virtual */ void itemUpdated(int index);

    /* virtual */ void beginItemsClear();
    /* virtual */ void endItemsClear  ();

    /* virtual */ void currentIndexChanged(int index);

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
