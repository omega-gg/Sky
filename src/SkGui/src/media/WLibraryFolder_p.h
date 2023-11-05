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
#ifdef QT_NEW
#include <WList>
#endif

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

    void vbmlItem(QString & vbml, const WLibraryFolderItem & item, const QString & tab) const;

    //---------------------------------------------------------------------------------------------
    // WLibraryItem interface

    void updateItemType      (int id, WLibraryItem::Type    type);
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
#ifdef QT_OLD
    QList<WLibraryFolderItem> items;
#else
    // NOTE: We want to ensure that data pointers are updated on a move operation.
    WList<WLibraryFolderItem> items;
#endif

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
