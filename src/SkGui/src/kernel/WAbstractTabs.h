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

#ifndef WABSTRACTTABS_H
#define WABSTRACTTABS_H

// Sk includes
#include <WLocalObject>

#ifndef SK_NO_ABSTRACTTABS

// Forward declarations
class WAbstractTabsPrivate;
class WAbstractTab;
class WTabTrack;

//-------------------------------------------------------------------------------------------------
// WAbstractTabsWatcher
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WAbstractTabsWatcher
{
protected:
    virtual void beginTabsInsert(int first, int last);
    virtual void endTabsInsert  ();

    virtual void beginTabsMove(int first, int last, int to);
    virtual void endTabsMove  ();

    virtual void beginTabsRemove(int first, int last);
    virtual void endTabsRemove  ();

    virtual void beginTabsClear();
    virtual void endTabsClear  ();

    virtual void tabsDestroyed();

private:
    friend class WAbstractTabs;
    friend class WAbstractTabsPrivate;
};

//-------------------------------------------------------------------------------------------------
// WAbstractTabs
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WAbstractTabs : public WLocalObject
{
    Q_OBJECT

    Q_PROPERTY(WAbstractTab * currentTab READ currentTab WRITE setCurrentTab
               NOTIFY currentTabChanged)

    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)

    Q_PROPERTY(int currentId READ currentId WRITE setCurrentId NOTIFY currentTabChanged)

    Q_PROPERTY(int count READ count NOTIFY countChanged)

    Q_PROPERTY(int maxCount READ maxCount WRITE setMaxCount NOTIFY maxCountChanged)

    Q_PROPERTY(bool isEmpty READ isEmpty NOTIFY countChanged)
    Q_PROPERTY(bool isFull  READ isFull  NOTIFY countChanged)

public:
    explicit WAbstractTabs(QObject * parent = NULL);
protected:
    WAbstractTabs(WAbstractTabsPrivate * p, QObject * parent = NULL);

public: // Interface
    Q_INVOKABLE WAbstractTab * addTab();
    Q_INVOKABLE WAbstractTab * insertTab(int index);

    Q_INVOKABLE void moveTab(int from, int to);

    Q_INVOKABLE void deleteTab(WAbstractTab * tab);

    Q_INVOKABLE void clearTabs();

    Q_INVOKABLE void selectPrevious();
    Q_INVOKABLE void selectNext    ();

    Q_INVOKABLE WAbstractTab * tabAt    (int index) const;
    Q_INVOKABLE WAbstractTab * tabFromId(int id)    const;

    Q_INVOKABLE int indexOf(WAbstractTab * tab) const;

    Q_INVOKABLE bool contains(WAbstractTab * tab) const;

    //---------------------------------------------------------------------------------------------
    // For convenience

    Q_INVOKABLE void deleteAt(int index);

    //---------------------------------------------------------------------------------------------
    // Watchers

    Q_INVOKABLE void registerWatcher  (WAbstractTabsWatcher * watcher);
    Q_INVOKABLE void unregisterWatcher(WAbstractTabsWatcher * watcher);

protected: // Functions
    void addTab   (WAbstractTab * tab);
    void insertTab(int index, WAbstractTab * tab);

protected: // Abstract functions
    virtual WAbstractTab * createTab(WAbstractTabs * parent = NULL) const = 0;

protected: // Virtual functions
    virtual void updateIndex();

protected: // WLocalObject reimplementation
    /* virtual */ void onSaveEnabledChanged(bool enabled);

signals:
    void currentTabChanged  ();
    void currentIndexChanged();

    void tabsMoved();

    void countChanged();

    void maxCountChanged();

    void saveEnabledChanged();

public: // Properties
    WAbstractTab * currentTab() const;
    void           setCurrentTab(WAbstractTab * tab);

    int  currentIndex() const;
    void setCurrentIndex(int index);

    int  currentId() const;
    void setCurrentId(int id);

    int count() const;

    int  maxCount() const;
    void setMaxCount(int max);

    bool isEmpty() const;
    bool isFull () const;

private:
    W_DECLARE_PRIVATE(WAbstractTabs)
};

#endif // SK_NO_ABSTRACTTABS
#endif // WABSTRACTTABS_H
