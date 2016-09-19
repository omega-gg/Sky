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

#include "WDeclarativeContextualPage.h"

#ifndef SK_NO_DECLARATIVECONTEXTUALPAGE

//=================================================================================================
// WContextualPageWatcher
//=================================================================================================

/* virtual */ void WContextualPageWatcher::beginItemsInsert(int, int) {}
/* virtual */ void WContextualPageWatcher::endItemsInsert  ()         {}

/* virtual */ void WContextualPageWatcher::itemUpdated(int) {}

/* virtual */ void WContextualPageWatcher::beginItemsClear() {}
/* virtual */ void WContextualPageWatcher::endItemsClear  () {}

/* virtual */ void WContextualPageWatcher::currentIndexChanged(int) {}

/* virtual */ void WContextualPageWatcher::contextualPageDestroyed() {}

//=================================================================================================
// WDeclarativeContextualPagePrivate
//=================================================================================================

#include <private/Sk_p>

class SK_GUI_EXPORT WDeclarativeContextualPagePrivate : public WPrivate
{
public:
    WDeclarativeContextualPagePrivate(WDeclarativeContextualPage * p);

    /* virtual */ ~WDeclarativeContextualPagePrivate();

    void init();

public: // Functions
    bool containsId(int id) const;

    void beginItemsInsert(int first, int last) const;
    void endItemsInsert  ()                    const;

    void itemUpdated(int index) const;

    void beginItemsClear() const;
    void endItemsClear  () const;

    void currentIndexChanged(int index) const;

public: // Variables
    QList<WContextualPageWatcher *> watchers;

    QVariantList values;

    QList<WDeclarativeContextualItem> items;

    QList<WDeclarativeContextualPage *> pages;

    int currentId;

protected:
    W_DECLARE_PUBLIC(WDeclarativeContextualPage)
};

//-------------------------------------------------------------------------------------------------

WDeclarativeContextualPagePrivate::
WDeclarativeContextualPagePrivate(WDeclarativeContextualPage * p) : WPrivate(p) {}

/* virtual */ WDeclarativeContextualPagePrivate::~WDeclarativeContextualPagePrivate()
{
    foreach (WContextualPageWatcher * watcher, watchers)
    {
        watcher->contextualPageDestroyed();
    }
}

//-------------------------------------------------------------------------------------------------

void WDeclarativeContextualPagePrivate::init()
{
    currentId = -1;
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

bool WDeclarativeContextualPagePrivate::containsId(int id) const
{
    foreach (const WDeclarativeContextualItem & item, items)
    {
        if (item.id == id) return true;
    }

    return false;
}

//-------------------------------------------------------------------------------------------------

void WDeclarativeContextualPagePrivate::beginItemsInsert(int first, int last) const
{
    foreach (WContextualPageWatcher * watcher, watchers)
    {
        watcher->beginItemsInsert(first, last);
    }
}

void WDeclarativeContextualPagePrivate::endItemsInsert() const
{
    foreach (WContextualPageWatcher * watcher, watchers)
    {
        watcher->endItemsInsert();
    }
}

//-------------------------------------------------------------------------------------------------

void WDeclarativeContextualPagePrivate::itemUpdated(int index) const
{
    foreach (WContextualPageWatcher * watcher, watchers)
    {
        watcher->itemUpdated(index);
    }
}

//-------------------------------------------------------------------------------------------------

void WDeclarativeContextualPagePrivate::beginItemsClear() const
{
    foreach (WContextualPageWatcher * watcher, watchers)
    {
        watcher->beginItemsClear();
    }
}

void WDeclarativeContextualPagePrivate::endItemsClear() const
{
    foreach (WContextualPageWatcher * watcher, watchers)
    {
        watcher->endItemsClear();
    }
}

//-------------------------------------------------------------------------------------------------

void WDeclarativeContextualPagePrivate::currentIndexChanged(int index) const
{
    foreach (WContextualPageWatcher * watcher, watchers)
    {
        watcher->currentIndexChanged(index);
    }
}

//=================================================================================================
// WDeclarativeContextualPage
//=================================================================================================

/* explicit */ WDeclarativeContextualPage::WDeclarativeContextualPage(QObject * parent)
    : QObject(parent), WPrivatable(new WDeclarativeContextualPagePrivate(this))
{
    Q_D(WDeclarativeContextualPage); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Items

void WDeclarativeContextualPage::registerWatcher(WContextualPageWatcher * watcher)
{
    Q_ASSERT(watcher);

    Q_D(WDeclarativeContextualPage);

    if (d->watchers.contains(watcher)) return;

    d->watchers.append(watcher);
}

void WDeclarativeContextualPage::unregisterWatcher(WContextualPageWatcher * watcher)
{
    Q_ASSERT(watcher);

    Q_D(WDeclarativeContextualPage);

    if (d->watchers.contains(watcher) == false) return;

    d->watchers.removeOne(watcher);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WDeclarativeContextualPage::addItem(const QString & title,
                                                           const QUrl    & icon,
                                                           const QSizeF  & iconSize,
                                                           ItemType        type,
                                                           int             id,
                                                           bool            visible,
                                                           bool            enabled)
{
    Q_D(WDeclarativeContextualPage);

    insertItem(d->items.count(), title, icon, iconSize, type, id, visible, enabled);
}

/* Q_INVOKABLE */ void WDeclarativeContextualPage::insertItem(int             index,
                                                              const QString & title,
                                                              const QUrl    & icon,
                                                              const QSizeF  & iconSize,
                                                              ItemType        type,
                                                              int             id,
                                                              bool            visible,
                                                              bool            enabled)
{
    Q_D(WDeclarativeContextualPage);

    WDeclarativeContextualItem item;

    item.type     = type;
    item.id       = id;
    item.title    = title;
    item.icon     = icon;
    item.iconSize = iconSize;
    item.visible  = visible;
    item.enabled  = enabled;

    d->beginItemsInsert(index, index);

    d->items.insert(index, item);

    d->endItemsInsert();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WDeclarativeContextualPage::set(int index, const QVariant & variant)
{
    Q_D(WDeclarativeContextualPage);

    if (index < 0 || index >= d->items.count()) return;

    WDeclarativeContextualItem * item = &(d->items[index]);

    QMap<QString, QVariant> map = variant.toMap();

    if (map.isEmpty()) return;

    bool update = false;

    if (map.contains("type"))
    {
        item->type = static_cast<WDeclarativeContextualPage::ItemType> (map.value("type").toInt());

        update = true;
    }

    if (map.contains("id"))
    {
        item->id = map.value("id").toInt();

        update = true;
    }

    if (map.contains("title"))
    {
        item->title = map.value("title").toString();

        update = true;
    }

    if (map.contains("icon"))
    {
        item->icon = map.value("icon").toUrl();

        update = true;
    }

    if (map.contains("iconSize"))
    {
        item->iconSize = map.value("iconSize").toSizeF();

        update = true;
    }

    if (map.contains("visible"))
    {
        item->visible = map.value("visible").toBool();

        update = true;
    }

    if (map.contains("enabled"))
    {
        item->enabled = map.value("enabled").toBool();

        update = true;
    }

    if (update) d->itemUpdated(index);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WDeclarativeContextualPage::setItemVisible(int index, bool visible)
{
    Q_D(WDeclarativeContextualPage);

    if (index < 0 || index >= d->items.count()) return;

    WDeclarativeContextualItem * item = &(d->items[index]);

    if (item->visible == visible) return;

    d->beginItemsClear();

    item->visible = visible;

    d->endItemsClear();
}

/* Q_INVOKABLE */ void WDeclarativeContextualPage::setItemEnabled(int index, bool enabled)
{
    Q_D(WDeclarativeContextualPage);

    if (index < 0 || index >= d->items.count()) return;

    WDeclarativeContextualItem * item = &(d->items[index]);

    if (item->enabled == enabled) return;

    item->enabled = enabled;

    d->itemUpdated(index);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WDeclarativeContextualPage::clearItems()
{
    Q_D(WDeclarativeContextualPage);

    d->currentId = -1;

    d->beginItemsClear();

    d->items.clear();

    d->endItemsClear();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ int WDeclarativeContextualPage::count() const
{
    Q_D(const WDeclarativeContextualPage); return d->items.count();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WDeclarativeContextualPage::selectPrevious()
{
    Q_D(WDeclarativeContextualPage);

    int index;

    if (d->currentId == -1)
    {
         index = d->items.count() - 1;
    }
    else index = currentIndex() - 1;

    while (index != -1)
    {
        const WDeclarativeContextualItem & item = d->items.at(index);

        if (item.id != -1 && item.visible && item.enabled)
        {
            setCurrentId(item.id);

            return;
        }

        index--;
    }

    if (d->currentId != -1)
    {
        d->currentId = -1;

        selectPrevious();
    }
}

/* Q_INVOKABLE */ void WDeclarativeContextualPage::selectNext()
{
    Q_D(WDeclarativeContextualPage);

    int index;

    if (d->currentId == -1) index = 0;
    else                    index = currentIndex() + 1;

    while (index != d->items.count())
    {
        const WDeclarativeContextualItem & item = d->items.at(index);

        if (item.id != -1 && item.visible && item.enabled)
        {
            setCurrentId(item.id);

            return;
        }

        index++;
    }

    if (d->currentId != -1)
    {
        d->currentId = -1;

        selectNext();
    }
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ int WDeclarativeContextualPage::idAt(int index) const
{
    Q_D(const WDeclarativeContextualPage);

    if (index < 0 || index >= d->items.count())
    {
         return -1;
    }
    else return d->items.at(index).id;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */
int WDeclarativeContextualPage::indexOf(const WDeclarativeContextualItem * item) const
{
    Q_D(const WDeclarativeContextualPage);

    for (int i = 0; i < d->items.count(); i++)
    {
        if (&(d->items.at(i)) == item)
        {
            return i;
        }
    }

    return -1;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ int WDeclarativeContextualPage::indexFromId(int id) const
{
    Q_D(const WDeclarativeContextualPage);

    for (int i = 0; i < d->items.count(); i++)
    {
        if (d->items.at(i).id == id)
        {
            return i;
        }
    }

    return -1;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */
WDeclarativeContextualItem WDeclarativeContextualPage::getItemAt(int index) const
{
    Q_D(const WDeclarativeContextualPage);

    if (index < 0 || index >= d->items.count())
    {
         return WDeclarativeContextualItem();
    }
    else return d->items.at(index);
}

/* Q_INVOKABLE */
WDeclarativeContextualItem WDeclarativeContextualPage::getItemFromId(int id) const
{
    return getItemAt(indexFromId(id));
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */
const WDeclarativeContextualItem * WDeclarativeContextualPage::itemAt(int index) const
{
    Q_D(const WDeclarativeContextualPage);

    if (index < 0 || index >= d->items.count())
    {
         return NULL;
    }
    else return &(d->items.at(index));
}

/* Q_INVOKABLE */
const WDeclarativeContextualItem * WDeclarativeContextualPage::itemFromId(int id) const
{
    return itemAt(indexFromId(id));
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */
const WDeclarativeContextualItem * WDeclarativeContextualPage::currentItemPointer() const
{
    Q_D(const WDeclarativeContextualPage);

    return itemFromId(d->currentId);
}

//-------------------------------------------------------------------------------------------------
// Pages

/* Q_INVOKABLE */ void WDeclarativeContextualPage::addPage(WDeclarativeContextualPage * page)
{
    Q_D(WDeclarativeContextualPage);

    if (d->pages.contains(page)) return;

    if (page->parent() == NULL) page->setParent(this);

    d->pages.append(page);
}

/* Q_INVOKABLE */ int WDeclarativeContextualPage::pageCount() const
{
    Q_D(const WDeclarativeContextualPage); return d->pages.count();
}

/* Q_INVOKABLE */ WDeclarativeContextualPage * WDeclarativeContextualPage::pageAt(int index)
{
    Q_D(WDeclarativeContextualPage);

    if (index < 0 || index >= d->pages.count()) return NULL;

    return d->pages.at(index);
}

/* Q_INVOKABLE */ void WDeclarativeContextualPage::clearPages()
{
    Q_D(WDeclarativeContextualPage);

    foreach (WDeclarativeContextualPage * page, d->pages) delete page;

    d->pages.clear();
}

//-------------------------------------------------------------------------------------------------
// Declarative
//-------------------------------------------------------------------------------------------------

/* static */
void WDeclarativeContextualPage::childrenAppend(QDeclarativeListProperty
                                                <WDeclarativeContextualPage> * property,
                                                WDeclarativeContextualPage * item)
{
    static_cast<WDeclarativeContextualPage *> (property->object)->addPage(item);
}

/* static */
void WDeclarativeContextualPage::childrenClear(QDeclarativeListProperty
                                               <WDeclarativeContextualPage> * property)
{
    static_cast<WDeclarativeContextualPage *> (property->object)->clearPages();
}

/* static */
int WDeclarativeContextualPage::childrenCount(QDeclarativeListProperty
                                              <WDeclarativeContextualPage> * property)
{
    return static_cast<WDeclarativeContextualPage *> (property->object)->pageCount();
}

/* static */ WDeclarativeContextualPage *
    WDeclarativeContextualPage::childrenAt(QDeclarativeListProperty
                                           <WDeclarativeContextualPage> * property,
                                           int index)
{
    return static_cast<WDeclarativeContextualPage *> (property->object)->pageAt(index);
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

QVariantList WDeclarativeContextualPage::values() const
{
    Q_D(const WDeclarativeContextualPage); return d->values;
}

void WDeclarativeContextualPage::setValues(const QVariantList & values)
{
    Q_D(WDeclarativeContextualPage);

    d->values = values;

    clearItems();

    foreach (const QVariant & variant, d->values)
    {
        QMap<QString, QVariant> map = variant.toMap();

        //-----------------------------------------------------------------------------------------

        QString title = map.value("title").toString();

        QUrl icon = map.value("icon").toUrl();

        QSizeF iconSize = map.value("iconSize").toSizeF();

        //-----------------------------------------------------------------------------------------

        WDeclarativeContextualPage::ItemType type;

        int value = map.value("type").toInt();

        if (value > 0)
        {
             type = static_cast<WDeclarativeContextualPage::ItemType> (value);
        }
        else type = WDeclarativeContextualPage::Item;

        //-----------------------------------------------------------------------------------------

        QVariant id = map.value("id");

        if (id.isValid())
        {
             addItem(title, icon, iconSize, type, id.toInt());
        }
        else addItem(title, icon, iconSize, type);
    }

    emit valuesChanged(d->values);
}

//-------------------------------------------------------------------------------------------------

QDeclarativeListProperty<WDeclarativeContextualPage> WDeclarativeContextualPage::pages()
{
    return QDeclarativeListProperty<WDeclarativeContextualPage>(this, 0,
                                                                childrenAppend, childrenCount,
                                                                childrenAt, childrenClear);
}

//-------------------------------------------------------------------------------------------------

int WDeclarativeContextualPage::currentId() const
{
    Q_D(const WDeclarativeContextualPage); return d->currentId;
}

void WDeclarativeContextualPage::setCurrentId(int id)
{
    Q_D(WDeclarativeContextualPage);

    if (d->currentId == id) return;

    if (id != -1 && d->containsId(id) == false)
    {
        return;
    }

    d->currentId = id;

    int index = indexFromId(id);

    d->currentIndexChanged(index);

    emit currentIdChanged();
}

//-------------------------------------------------------------------------------------------------

int WDeclarativeContextualPage::currentIndex() const
{
    Q_D(const WDeclarativeContextualPage); return indexFromId(d->currentId);
}

void WDeclarativeContextualPage::setCurrentIndex(int index)
{
    int id = idAt(index);

    setCurrentId(id);
}

//=================================================================================================
// WDeclarativeContextualItem
//=================================================================================================

WDeclarativeContextualItem::WDeclarativeContextualItem()
{
    type = WDeclarativeContextualPage::Item;

    id = -1;

    visible = true;
    enabled = true;
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

bool WDeclarativeContextualItem::isValid() const
{
    if (title.isNull() && icon.isEmpty())
    {
         return false;
    }
    else return true;
}

#endif // SK_NO_DECLARATIVECONTEXTUALPAGE
