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

#ifndef WLIBRARYFOLDER_H
#define WLIBRARYFOLDER_H

// Sk includes
#include <WLibraryItem>

#ifndef SK_NO_LIBRARYFOLDER

class WLibraryFolderPrivate;
class WBackendNetQuery;

//-------------------------------------------------------------------------------------------------
// WLibraryFolderItem
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WLibraryFolderItem
{
public:
    WLibraryFolderItem(WLibraryItem::Type  type       = WLibraryItem::Item,
                       WLocalObject::State stateQuery = WLocalObject::Loaded);

public: // Interface
    bool isFolder  () const;
    bool isPlaylist() const;

    bool isLocal () const;
    bool isOnline() const;

public: // Variables
    int id;

    WLibraryItem::Type type;

    WLocalObject::State state;
    WLocalObject::State stateQuery;

    QString source;

    QString title;
    QString cover;

    QString label;
};

//-------------------------------------------------------------------------------------------------
// WLibraryFolderWatcher
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WLibraryFolderWatcher
{
protected:
    virtual void beginItemsInsert(int first, int last); // {}
    virtual void endItemsInsert  (); // {}

    virtual void beginItemsMove(int first, int last, int to); // {}
    virtual void endItemsMove  ();                            // {}

    virtual void beginItemsRemove(int first, int last); // {}
    virtual void endItemsRemove  (); // {}

    virtual void itemUpdated(int index); // {}

    virtual void beginItemsClear(); // {}
    virtual void endItemsClear  (); // {}

    virtual void currentIndexChanged(int index); // {}

    virtual void folderDestroyed(); // {}

private:
    friend class WLibraryFolder;
    friend class WLibraryFolderPrivate;
};

//-------------------------------------------------------------------------------------------------
// WLibraryFolder
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WLibraryFolder : public WLibraryItem
{
    Q_OBJECT

    Q_PROPERTY(int currentId READ currentId WRITE setCurrentId NOTIFY currentIdChanged)
    Q_PROPERTY(int activeId  READ activeId  WRITE setActiveId  NOTIFY activeIdChanged)

    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(int activeIndex  READ activeIndex  WRITE setActiveIndex  NOTIFY activeIndexChanged)

    Q_PROPERTY(WLibraryItem * currentItem READ currentItem NOTIFY currentIdChanged)

    Q_PROPERTY(qreal scrollValue READ scrollValue WRITE setScrollValue NOTIFY scrollValueChanged)

    Q_PROPERTY(int count READ count NOTIFY countChanged)

    Q_PROPERTY(int maxCount READ maxCount WRITE setMaxCount NOTIFY maxCountChanged)

    Q_PROPERTY(bool isEmpty READ isEmpty NOTIFY countChanged)
    Q_PROPERTY(bool isFull  READ isFull  NOTIFY countChanged)

public:
    explicit WLibraryFolder(WLibraryFolder * parent = NULL);
protected:
    WLibraryFolder(WLibraryFolderPrivate * p, Type type, WLibraryFolder * parent = NULL);

public: // Interface
    Q_INVOKABLE WLibraryFolderItem createFolderItem(WLibraryItem * item);

    Q_INVOKABLE void addLibraryItem (WLibraryItem                * item);
    Q_INVOKABLE void addLibraryItems(const QList<WLibraryItem *> & items);

    Q_INVOKABLE void insertLibraryItem (int index, WLibraryItem                * item);
    Q_INVOKABLE void insertLibraryItems(int index, const QList<WLibraryItem *> & items);

    Q_INVOKABLE void addItem (const WLibraryFolderItem        & item);
    Q_INVOKABLE void addItems(const QList<WLibraryFolderItem> & items);

    Q_INVOKABLE void insertItem (int index, const WLibraryFolderItem        & item);
    Q_INVOKABLE void insertItems(int index, const QList<WLibraryFolderItem> & items);

    Q_INVOKABLE void addNewItem(Type type, const QString & source = QString(),
                                           const QString & title  = QString(),
                                           const QString & cover  = QString());

    Q_INVOKABLE void insertNewItem(int index, Type type, const QString & source = QString(),
                                                         const QString & title  = QString(),
                                                         const QString & cover  = QString());

    Q_INVOKABLE void moveItem(int id,   int to);
    Q_INVOKABLE void moveAt  (int from, int to);

    Q_INVOKABLE void moveItemFrom(WLibraryFolder * source, int from, int to);

    Q_INVOKABLE void moveItemTo(int from, WLibraryFolder * destination, int to);

    Q_INVOKABLE void removeItem(int id);
    Q_INVOKABLE void removeAt  (int index);

    Q_INVOKABLE void clearItems();

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE void loadItem (int at);
    Q_INVOKABLE void loadItems(int at, int count);

    Q_INVOKABLE void abortItems();

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE bool checkFull(int count = 1) const;

    Q_INVOKABLE bool contains(int id) const;

    Q_INVOKABLE bool containsSource(const QString & source) const;

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE int idAt(int index) const;

    Q_INVOKABLE int indexOf(const WLibraryFolderItem * item) const;

    Q_INVOKABLE int indexFromId(int id) const;

    Q_INVOKABLE int indexFromSource(const QString & source) const;

    Q_INVOKABLE int indexFromLabel(const QString & label) const;

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE WLibraryFolderItem getItemAt    (int index) const;
    Q_INVOKABLE WLibraryFolderItem getItemFromId(int id)    const;

    Q_INVOKABLE const WLibraryFolderItem * itemAt    (int index) const;
    Q_INVOKABLE const WLibraryFolderItem * itemFromId(int id)    const;

    Q_INVOKABLE const WLibraryFolderItem * currentItemPointer() const;

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE void loadCurrentId(int id, bool instantSet  = false,
                                           bool instantLoad = false);

    Q_INVOKABLE void loadCurrentIndex(int index, bool instantSet  = false,
                                                 bool instantLoad = false);

    Q_INVOKABLE void loadLibraryItems(const QList<int> & ids);

    Q_INVOKABLE void loadTabItems(WTabTrack * tab);

    //---------------------------------------------------------------------------------------------
    // Item interface

    Q_INVOKABLE Type itemType(int index) const;

    Q_INVOKABLE bool itemIsPlaylist(int index) const;

    Q_INVOKABLE WLocalObject::State itemState     (int index) const;
    Q_INVOKABLE WLocalObject::State itemStateQuery(int index) const;

    Q_INVOKABLE QString itemSource   (int index) const;
    Q_INVOKABLE void    setItemSource(int index, const QString & source);

    Q_INVOKABLE bool itemIsLocal(int index) const;

    Q_INVOKABLE QString itemTitle   (int index) const;
    Q_INVOKABLE void    setItemTitle(int index, const QString & title);

    Q_INVOKABLE QString itemCover   (int index) const;
    Q_INVOKABLE void    setItemCover(int index, const QString & cover);

    Q_INVOKABLE QString itemLabel   (int index) const;
    Q_INVOKABLE void    setItemLabel(int index, const QString & label);

    //---------------------------------------------------------------------------------------------
    // WLibraryItem

    Q_INVOKABLE WLibraryItem * createLibraryItem(const WLibraryFolderItem & item,
                                                 bool                       instant = false);

    Q_INVOKABLE WLibraryItem * createLibraryItemFromId(int id,    bool instant = false);
    Q_INVOKABLE WLibraryItem * createLibraryItemAt    (int index, bool instant = false);

    Q_INVOKABLE WLibraryItem * getLibraryItemFromId(int id)    const;
    Q_INVOKABLE WLibraryItem * getLibraryItemAt    (int index) const;

    //---------------------------------------------------------------------------------------------
    // Watchers

    Q_INVOKABLE void registerWatcher  (WLibraryFolderWatcher * watcher);
    Q_INVOKABLE void unregisterWatcher(WLibraryFolderWatcher * watcher);

public: // Static functions
    Q_INVOKABLE static WLibraryFolder * create(Type type = Folder);

    Q_INVOKABLE static bool move(WLibraryFolder * folderA, int from,
                                 WLibraryFolder * folderB, int to, bool remove = true);

protected: // Functions
    void updateIndex();

protected: // Virtual functions
    virtual WLibraryItem * createItem(Type type, WLibraryFolder * parent = NULL); // {}

protected: // WLocalObject reimplementation
    /* virtual */ void applyId(int id);

    /* virtual */ WAbstractThreadAction * onSave(const QString & path);
    /* virtual */ WAbstractThreadAction * onLoad(const QString & path);

    /* virtual */ void onSaveEnabledChanged(bool enabled);

    /* virtual */ void onFolderDeleted();

    /* virtual */ bool hasFolder() const;

protected: // WLibraryItem reimplementation
    /* virtual */ bool applySource(const QString          & source);
    /* virtual */ bool applyQuery (const WBackendNetQuery & query);

    /* virtual */ bool stopQuery();

    /* virtual */ void onApplyCurrentIds(const QList<int> & ids);

signals:
    void itemsInserted(int index, int count);

    void itemMoved(int from, int to);

    void itemRemoved(int index);

    void itemsCleared();

    void currentIdChanged();
    void currentIdUpdated();

    void currentIndexChanged();
    void activeIndexChanged ();

    void activeIdChanged();

    void scrollValueChanged();

    void countChanged();

    void maxCountChanged();

public: // Properties
    int  currentId() const;
    void setCurrentId(int id);

    int  activeId() const;
    void setActiveId(int id);

    int  currentIndex() const;
    void setCurrentIndex(int index);

    int  activeIndex() const;
    void setActiveIndex(int index);

    WLibraryItem * currentItem() const;

    qreal scrollValue() const;
    void  setScrollValue(qreal value);

    int count() const;

    int  maxCount() const;
    void setMaxCount(int max);

    bool isEmpty() const;
    bool isFull () const;

private:
    W_DECLARE_PRIVATE(WLibraryFolder)

    Q_PRIVATE_SLOT(d_func(), void onFolderLoaded  ())
    Q_PRIVATE_SLOT(d_func(), void onItemNextLoaded())
    Q_PRIVATE_SLOT(d_func(), void onItemLoaded    ())

    friend class WLibraryItem;
    friend class WLibraryItemPrivate;
    friend class WControllerPlaylist;
    friend class WControllerPlaylistPrivate;
};

//-------------------------------------------------------------------------------------------------
// WLibraryFolderSearch
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WLibraryFolderSearch : public WLibraryFolder
{
    Q_OBJECT

public:
    explicit WLibraryFolderSearch(WLibraryFolder * parent = NULL);

private:
    Q_DISABLE_COPY(WLibraryFolderSearch)
};

//-------------------------------------------------------------------------------------------------
// WLibraryFolderSearchable
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WLibraryFolderSearchable : public WLibraryFolder
{
    Q_OBJECT

public:
    explicit WLibraryFolderSearchable(WLibraryFolder * parent = NULL);

private:
    Q_DISABLE_COPY(WLibraryFolderSearchable)
};

#include <private/WLibraryFolder_p>

#endif // SK_NO_LIBRARYFOLDER
#endif // WLIBRARYFOLDER_H
