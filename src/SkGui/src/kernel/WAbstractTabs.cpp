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

#include "WAbstractTabs.h"

#ifndef SK_NO_ABSTRACTTABS

// Sk includes
#include <WControllerFile>
#include <WAbstractTab>
#include <WTabTrack>

// Private includes
#include <private/WAbstractTab_p>

//-------------------------------------------------------------------------------------------------
// Static variables

static const int ABSTRACTTABS_MAX = 16;

//=================================================================================================
// WAbstractTabsWatcher
//=================================================================================================

/* virtual */ void WAbstractTabsWatcher::beginTabsInsert(int, int) {}
/* virtual */ void WAbstractTabsWatcher::endTabsInsert  ()         {}

/* virtual */ void WAbstractTabsWatcher::beginTabsMove(int, int, int) {}
/* virtual */ void WAbstractTabsWatcher::endTabsMove  ()              {}

/* virtual */ void WAbstractTabsWatcher::beginTabsRemove(int, int) {}
/* virtual */ void WAbstractTabsWatcher::endTabsRemove  ()         {}

/* virtual */ void WAbstractTabsWatcher::beginTabsClear() {}
/* virtual */ void WAbstractTabsWatcher::endTabsClear  () {}

/* virtual */ void WAbstractTabsWatcher::tabsDestroyed() {}

//=================================================================================================
// WAbstractTabsPrivate
//=================================================================================================

#include "WAbstractTabs_p.h"

WAbstractTabsPrivate::WAbstractTabsPrivate(WAbstractTabs * p) : WLocalObjectPrivate(p) {}

/* virtual */ WAbstractTabsPrivate::~WAbstractTabsPrivate()
{
    foreach (WAbstractTabsWatcher * watcher, watchers)
    {
        watcher->tabsDestroyed();
    }
}

//-------------------------------------------------------------------------------------------------

void WAbstractTabsPrivate::init()
{
    currentTab   = NULL;
    currentIndex = -1;

    maxCount = ABSTRACTTABS_MAX;
}

//-------------------------------------------------------------------------------------------------
// Private WTabsTrackReadReply interface
//-------------------------------------------------------------------------------------------------

WAbstractTab * WAbstractTabsPrivate::createTab(WAbstractTabs * parent) const
{
    Q_Q(const WAbstractTabs); return q->createTab(parent);
}

void WAbstractTabsPrivate::loadTabs(const QList<WAbstractTab *> & tabs)
{
    Q_Q(WAbstractTabs);

    int count = this->tabs.count();

    if (count)
    {
        ids   .clear();
        idHash.clear();

        foreach (WAbstractTab * tab, this->tabs)
        {
            tab->abortAndDelete();
        }

        beginTabsClear();

        this->tabs.clear();

        endTabsClear();
    }

    if (tabs.isEmpty())
    {
        if (count) emit q->countChanged();

        return;
    }

    beginTabsInsert(0, tabs.count() - 1);

    foreach (WAbstractTab * tab, tabs)
    {
        tab->setParentTabs(q);

        int id = tab->id();

        ids.insertId(id);

        idHash.insert(id, tab);

        this->tabs.append(tab);

        tab->load();
    }

    endTabsInsert();

    emit q->countChanged();
}

//-------------------------------------------------------------------------------------------------
// Private watcher functions
//-------------------------------------------------------------------------------------------------

void WAbstractTabsPrivate::beginTabsInsert(int first, int last) const
{
    foreach (WAbstractTabsWatcher * watcher, watchers)
    {
        watcher->beginTabsInsert(first, last);
    }
}

void WAbstractTabsPrivate::endTabsInsert() const
{
    foreach (WAbstractTabsWatcher * watcher, watchers)
    {
        watcher->endTabsInsert();
    }
}

//-------------------------------------------------------------------------------------------------

void WAbstractTabsPrivate::beginTabsMove(int first, int last, int to) const
{
    foreach (WAbstractTabsWatcher * watcher, watchers)
    {
        watcher->beginTabsMove(first, last, to);
    }
}

void WAbstractTabsPrivate::endTabsMove() const
{
    foreach (WAbstractTabsWatcher * watcher, watchers)
    {
        watcher->endTabsMove();
    }
}

//-------------------------------------------------------------------------------------------------

void WAbstractTabsPrivate::beginTabsRemove(int first, int last) const
{
    foreach (WAbstractTabsWatcher * watcher, watchers)
    {
        watcher->beginTabsRemove(first, last);
    }
}


void WAbstractTabsPrivate::endTabsRemove() const
{
    foreach (WAbstractTabsWatcher * watcher, watchers)
    {
        watcher->endTabsRemove();
    }
}

//-------------------------------------------------------------------------------------------------

void WAbstractTabsPrivate::beginTabsClear() const
{
    foreach (WAbstractTabsWatcher * watcher, watchers)
    {
        watcher->beginTabsClear();
    }
}

void WAbstractTabsPrivate::endTabsClear() const
{
    foreach (WAbstractTabsWatcher * watcher, watchers)
    {
        watcher->endTabsClear();
    }
}

//=================================================================================================
// WAbstractTabs
//=================================================================================================

/* explicit */ WAbstractTabs::WAbstractTabs(QObject * parent)
    : WLocalObject(new WAbstractTabsPrivate(this), parent)
{
    Q_D(WAbstractTabs); d->init();
}

//-------------------------------------------------------------------------------------------------
// Protected

WAbstractTabs::WAbstractTabs(WAbstractTabsPrivate * p, QObject * parent)
    : WLocalObject(p, parent)
{
    Q_D(WAbstractTabs); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ WAbstractTab * WAbstractTabs::addTab()
{
    return insertTab(count());
}

/* Q_INVOKABLE */ WAbstractTab * WAbstractTabs::insertTab(int index)
{
    Q_D(WAbstractTabs);

    if (index < 0 || index > d->tabs.count() || d->tabs.count() >= d->maxCount) return NULL;

    WAbstractTab * tab = createTab(this);

    insertTab(index, tab);

    return tab;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WAbstractTabs::moveTab(int from, int to)
{
    Q_D(WAbstractTabs);

    if (from < 0 || from > d->tabs.count()
        ||
        to < 0 || to > d->tabs.count()) return;

    if (to > from)
    {
         d->beginTabsMove(from, from, to + 1);
    }
    else d->beginTabsMove(from, from, to);

    d->tabs.move(from, to);

    d->endTabsMove();

    updateIndex();

    emit tabsMoved();

    save();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WAbstractTabs::deleteTab(WAbstractTab * tab)
{
    Q_ASSERT(tab);

    Q_D(WAbstractTabs);

    if (d->tabs.contains(tab) == false) return;

    if (d->currentTab == tab)
    {
        setCurrentTab(NULL);
    }

    int id = tab->id();

    d->ids.removeOne(id);

    d->idHash.remove(id);

    if (tab->isLoaded())
    {
        tab->deleteFileAndFolder();
    }

    int index = d->tabs.indexOf(tab);

    d->beginTabsRemove(index, index);

    d->tabs.removeOne(tab);

    d->endTabsRemove();

    tab->abortAndDelete();

    if (d->tabs.isEmpty())
    {
        deleteFolder();
    }

    updateIndex();

    emit countChanged();

    save();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WAbstractTabs::clearTabs()
{
    Q_D(WAbstractTabs);

    abortAll();

    deleteFolder(true);

    if (d->tabs.count())
    {
        setCurrentTab(NULL);

        d->ids   .clear();
        d->idHash.clear();

        foreach (WAbstractTab * tab, d->tabs)
        {
            tab->abortAndDelete();
        }

        d->beginTabsClear();

        d->tabs.clear();

        d->endTabsClear();

        emit countChanged();
    }

    save();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WAbstractTabs::selectPrevious()
{
    Q_D(WAbstractTabs);

    if (d->tabs.isEmpty()) return;

    int index = currentIndex();

    if (index > 0)
    {
        setCurrentIndex(index - 1);
    }
}

/* Q_INVOKABLE */ void WAbstractTabs::selectNext()
{
    Q_D(WAbstractTabs);

    int count = d->tabs.count() - 1;

    if (count == -1) return;

    int index = currentIndex();

    if (index < count)
    {
        setCurrentIndex(index + 1);
    }
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ WAbstractTab * WAbstractTabs::tabAt(int index) const
{
    Q_D(const WAbstractTabs);

    if (index < 0 || index >= d->tabs.count())
    {
         return NULL;
    }
    else return d->tabs.at(index);
}

/* Q_INVOKABLE */ WAbstractTab * WAbstractTabs::tabFromId(int id) const
{
    Q_D(const WAbstractTabs);

    foreach (WAbstractTab * tab, d->tabs)
    {
        if (tab->id() == id) return tab;
    }

    return NULL;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ int WAbstractTabs::indexOf(WAbstractTab * tab) const
{
    Q_D(const WAbstractTabs); return d->tabs.indexOf(tab);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ bool WAbstractTabs::contains(WAbstractTab * tab) const
{
    Q_D(const WAbstractTabs); return d->tabs.contains(tab);
}

//-------------------------------------------------------------------------------------------------
// For convenience

/* Q_INVOKABLE */ void WAbstractTabs::deleteAt(int index)
{
    WAbstractTab * tab = tabAt(index);

    if (tab) deleteTab(tab);
}

//-------------------------------------------------------------------------------------------------
// Watchers

/* Q_INVOKABLE */ void WAbstractTabs::registerWatcher(WAbstractTabsWatcher * watcher)
{
    Q_ASSERT(watcher);

    Q_D(WAbstractTabs);

    if (d->watchers.contains(watcher)) return;

    d->watchers.append(watcher);
}

/* Q_INVOKABLE */ void WAbstractTabs::unregisterWatcher(WAbstractTabsWatcher * watcher)
{
    Q_ASSERT(watcher);

    Q_D(WAbstractTabs);

    if (d->watchers.contains(watcher) == false) return;

    d->watchers.removeOne(watcher);
}

//-------------------------------------------------------------------------------------------------
// Protected functions
//-------------------------------------------------------------------------------------------------

void WAbstractTabs::addTab(WAbstractTab * tab)
{
    insertTab(count(), tab);
}

void WAbstractTabs::insertTab(int index, WAbstractTab * tab)
{
    Q_ASSERT(tab);

    Q_D(WAbstractTabs);

    int id = tab->d_func()->id;

    if (id == -1)
    {
        tab->setId(d->ids.generateId());
    }
    else if (d->ids.insertId(id) == false)
    {
        qWarning("WAbstractTabs::insertTab: Id is already taken '%d'.", id);

        tab->setId(d->ids.generateId());
    }

    tab->setParentTabs(this);

    tab->setSaveEnabled(d->saveEnabled);

    d->beginTabsInsert(index, index);

    d->tabs.insert(index, tab);

    d->endTabsInsert();

    setCurrentTab(tab);

    emit countChanged();

    tab->save();

    save();
}

//-------------------------------------------------------------------------------------------------
// Protected virtual functions
//-------------------------------------------------------------------------------------------------

/* virtual */ void WAbstractTabs::updateIndex()
{
    Q_D(WAbstractTabs);

    int index = d->tabs.indexOf(d->currentTab);

    if (d->currentIndex != index)
    {
        d->currentIndex = index;

        emit currentIndexChanged();
    }
}

//-------------------------------------------------------------------------------------------------
// Protected WLocalObject reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WAbstractTabs::onSaveEnabledChanged(bool enabled)
{
    Q_D(WAbstractTabs);

    foreach (WAbstractTab * tab, d->tabs)
    {
        tab->setSaveEnabled(enabled);
    }
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

WAbstractTab * WAbstractTabs::currentTab() const
{
    Q_D(const WAbstractTabs); return d->currentTab;
}

void WAbstractTabs::setCurrentTab(WAbstractTab * tab)
{
    Q_D(WAbstractTabs);

    if (d->currentTab == tab || (tab && d->tabs.contains(tab) == false)) return;

    if (d->currentTab)
    {
        d->currentTab->d_func()->setFocus(false);
    }

    d->currentTab   = tab;
    d->currentIndex = d->tabs.indexOf(tab);

    emit currentTabChanged  ();
    emit currentIndexChanged();

    if (tab)
    {
        tab->d_func()->setFocus(true);
    }

    save();
}

//-------------------------------------------------------------------------------------------------

int WAbstractTabs::currentIndex() const
{
    Q_D(const WAbstractTabs); return d->currentIndex;
}

void WAbstractTabs::setCurrentIndex(int index)
{
    WAbstractTab * tab = tabAt(index);

    setCurrentTab(tab);
}

//-------------------------------------------------------------------------------------------------

int WAbstractTabs::currentId() const
{
    Q_D(const WAbstractTabs);

    if (d->currentTab)
    {
         return d->currentTab->id();
    }
    else return -1;
}

void WAbstractTabs::setCurrentId(int id)
{
    WAbstractTab * tab = tabFromId(id);

    if (tab) setCurrentTab(tab);
}

//-------------------------------------------------------------------------------------------------

int WAbstractTabs::count() const
{
    Q_D(const WAbstractTabs); return d->tabs.count();
}

//-------------------------------------------------------------------------------------------------

int WAbstractTabs::maxCount() const
{
    Q_D(const WAbstractTabs); return d->maxCount;
}

void WAbstractTabs::setMaxCount(int max)
{
    Q_D(WAbstractTabs);

    if (d->maxCount == max) return;

    d->maxCount = max;

    emit maxCountChanged();
}

//-------------------------------------------------------------------------------------------------

bool WAbstractTabs::isEmpty() const
{
    Q_D(const WAbstractTabs); return d->tabs.isEmpty();
}

bool WAbstractTabs::isFull() const
{
    Q_D(const WAbstractTabs);

    return (d->tabs.count() >= d->maxCount);
}

#endif // SK_NO_ABSTRACTTABS
