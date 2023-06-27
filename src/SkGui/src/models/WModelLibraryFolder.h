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

#ifndef WMODELLIBRARYFOLDER_H
#define WMODELLIBRARYFOLDER_H

// Sk includes
#include <WLibraryFolder>

// Qt includes
#include <QSortFilterProxyModel>

#ifndef SK_NO_MODELLIBRARYFOLDER

// Forward declarations
class WModelLibraryFolderPrivate;
class WModelLibraryFolderFilteredPrivate;
class WLibraryFolder;
class WLibraryItem;

//-------------------------------------------------------------------------------------------------
// WModelLibraryFolder
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WModelLibraryFolder : public QAbstractListModel, public WLibraryFolderWatcher,
                                          public WPrivatable
{
    Q_OBJECT

    Q_ENUMS(ItemRoles)

    Q_PROPERTY(WLibraryFolder * folder READ folder WRITE setFolder NOTIFY folderChanged)

public: // Enums
    enum ItemRoles
    {
        RoleId = Qt::UserRole + 1,
        RoleType,
        RoleState,
        RoleStateQuery,
        RoleSource,
        RoleTitle,
        RoleCover,
        RoleLabel,
        RoleCurrent
    };

public:
    explicit WModelLibraryFolder(QObject * parent = NULL);

public: // QAbstractItemModel implementation
    /* virtual */ int rowCount(const QModelIndex & parent = QModelIndex()) const;

    /* virtual */ QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

public: // QAbstractItemModel reimplementation
    /* virtual */ QHash<int, QByteArray> roleNames() const;

protected: // WLibraryFolderWatcher implementation
    /* virtual */ void beginItemsInsert(int first, int last);
    /* virtual */ void endItemsInsert  ();

    /* virtual */ void beginItemsMove(int first, int last, int to);
    /* virtual */ void endItemsMove  ();

    /* virtual */ void beginItemsRemove(int first, int last);
    /* virtual */ void endItemsRemove  ();

    /* virtual */ void itemUpdated(int index);

    /* virtual */ void beginItemsClear();
    /* virtual */ void endItemsClear  ();

    /* virtual */ void currentIndexChanged(int index);

    /* virtual */ void folderDestroyed();

signals:
    void folderChanged();

public: // Properties
    WLibraryFolder * folder() const;
    void             setFolder(WLibraryFolder * folder);

private:
    W_DECLARE_PRIVATE(WModelLibraryFolder)
};

//-------------------------------------------------------------------------------------------------
// WModelLibraryFolderFiltered
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WModelLibraryFolderFiltered : public QSortFilterProxyModel, public WPrivatable
{
    Q_OBJECT

    Q_PROPERTY(WModelLibraryFolder * model READ model WRITE setModel NOTIFY modelChanged)

    Q_PROPERTY(WLibraryFolder * folder READ folder WRITE setFolder NOTIFY folderChanged)

    Q_PROPERTY(int filter READ qmlFilter WRITE qmlSetFilter RESET resetFilter NOTIFY filterChanged)

    Q_PROPERTY(bool local READ local WRITE setLocal NOTIFY localChanged)

public:
    explicit WModelLibraryFolderFiltered(QObject * parent = NULL);

public: // Interface
    Q_INVOKABLE int idAt(int index) const;

    Q_INVOKABLE int indexFromId(int id) const;

    Q_INVOKABLE int indexFromRole(int role, const QVariant & value) const;

signals:
    void modelChanged();

    void folderChanged();

    void filterChanged();

    void localChanged();

public: // Properties
    WModelLibraryFolder * model() const;
    void                  setModel(WModelLibraryFolder * model);

    WLibraryFolder * folder() const;
    void             setFolder(WLibraryFolder * folder);

    WLibraryItem::Types filter() const;
    void                setFilter(WLibraryItem::Types types);
    void                resetFilter();

    int  qmlFilter() const;
    void qmlSetFilter(int types);

    bool local() const;
    void setLocal(bool local);

protected: // QSortFilterProxyModel reimplementation
    /* virtual */ bool filterAcceptsRow(int source_row, const QModelIndex & source_parent) const;

private:
    W_DECLARE_PRIVATE(WModelLibraryFolderFiltered)
};

#include <private/WModelLibraryFolder_p>

#endif // SK_NO_MODELLIBRARYFOLDER
#endif // WMODELLIBRARYFOLDER_H
