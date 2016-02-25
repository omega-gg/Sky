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

#ifndef WMODELTABS_H
#define WMODELTABS_H

// Qt includes
#include <QAbstractListModel>

// Sk includes
#include <WAbstractTabs>

#ifndef SK_NO_MODELTABS

// Forward declarations
class WModelTabsPrivate;

class SK_GUI_EXPORT WModelTabs : public QAbstractListModel, public WAbstractTabsWatcher,
                                 public WPrivatable
{
    Q_OBJECT

    Q_ENUMS(TabRoles)

    Q_PROPERTY(WAbstractTabs * tabs READ tabs WRITE setTabs NOTIFY tabsChanged)

    Q_PROPERTY(int count READ count NOTIFY countChanged)

public: // Enums
    enum TabRoles
    {
        RoleItem = Qt::UserRole + 1
    };

public:
    explicit WModelTabs(QObject * parent = NULL);

public: // QAbstractItemModel reimplementation
    /* virtual */ QHash<int, QByteArray> roleNames() const;

    /* virtual */ int rowCount(const QModelIndex & parent = QModelIndex()) const;

    /* virtual */ QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

signals:
    void tabsChanged();

    void countChanged();

protected: // WAbstractTabsWatcher reimplementation
    /* virtual */ void beginTabsInsert(int first, int last);
    /* virtual */ void endTabsInsert  ();

    /* virtual */ void beginTabsMove(int first, int last, int to);
    /* virtual */ void endTabsMove  ();

    /* virtual */ void beginTabsRemove(int first, int last);
    /* virtual */ void endTabsRemove  ();

    /* virtual */ void beginTabsClear();
    /* virtual */ void endTabsClear  ();

    /* virtual */ void tabsDestroyed();

public: // Properties
    WAbstractTabs * tabs() const;
    void            setTabs(WAbstractTabs * tabs);

    int count() const;

private:
    W_DECLARE_PRIVATE(WModelTabs)
};

#endif // SK_NO_MODELTABS
#endif // WMODELTABS_H
