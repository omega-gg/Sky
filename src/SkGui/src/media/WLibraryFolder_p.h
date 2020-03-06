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

#ifndef WLIBRARYFOLDER_P_H
#define WLIBRARYFOLDER_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

// Qt includes
#include <QHash>

// Sk includes
#include <WListId>

// Private includes
#include <private/WLibraryItem_p>

#ifndef SK_NO_LIBRARYFOLDER

// Forward declarations
class WThreadActionWriteFolder;
class WThreadActionReadFolder;

//-------------------------------------------------------------------------------------------------
// WLibraryFolderPrivate
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WLibraryFolderPrivate : public WLibraryItemPrivate
{
public:
    WLibraryFolderPrivate(WLibraryFolder * p);

    /* virtual */ ~WLibraryFolderPrivate();

    void init();

public: // Functions
    void registerItemId  (WLibraryItem * item);
    void unregisterItemId(WLibraryItem * item);

    void setCurrentItem(WLibraryItem * item);

    void clearItemNext();

    void deleteFile(int id);

    WLibraryFolderItem * itemFromId(int id);
    WLibraryFolderItem * itemAt    (int index);

    void updateLock(WLibraryItem * item, bool locked);

    void loadItems(const QList<WLibraryFolderItem> & items);

    void deleteItems();

    //---------------------------------------------------------------------------------------------
    // WLibraryItem interface

    void updateItemState     (int id, WLocalObject::State   state);
    void updateItemStateQuery(int id, WLocalObject::State   stateQuery);
    void updateItemSource    (int id, const QString       & source);
    void updateItemTitle     (int id, const QString       & title);
    void updateItemCover     (int id, const QString       & cover);
    void updateItemLabel     (int id, const QString       & label);

    //---------------------------------------------------------------------------------------------
    // WLibraryFolderWatcher

    void beginItemsInsert(int first, int last) const;
    void endItemsInsert  ()                    const;

    void beginItemsMove(int first, int last, int to) const;
    void endItemsMove  ()                            const;

    void beginItemsRemove(int first, int last) const;
    void endItemsRemove  ()                    const;

    void itemUpdated(int index) const;

    void beginItemsClear() const;
    void endItemsClear  () const;

    void currentIndexChanged();

public: // Slots
    void onFolderLoaded  ();
    void onItemNextLoaded();
    void onItemLoaded    ();

public: // Variables
    QList<WLibraryFolderItem> items;

    WListId                    ids;
    QHash<int, WLibraryItem *> idHash;

    QMultiHash<WLibraryItem *, QList<int> > itemIds;

    int defaultId;
    int currentId;
    int activeId;

    WLibraryItem * currentItem;

    int currentIndex;
    int activeIndex;

    WLibraryItem * itemNext;
    WLibraryItem * itemLock;

    qreal scrollValue;

    QList<WLibraryFolderWatcher *> watchers;

    bool isLoadingItem;

    int maxCount;

protected:
    W_DECLARE_PUBLIC(WLibraryFolder)

    friend class WLibraryFolderReadReply;
};

#endif // SK_NO_LIBRARYFOLDER
#endif // WLIBRARYFOLDER_P_H
