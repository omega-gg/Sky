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

#include "WLibraryFolder.h"

#ifndef SK_NO_LIBRARYFOLDER

// Qt includes
#include <QXmlStreamWriter>
#include <qtlockedfile>

// Sk includes
#include <WControllerApplication>
#include <WControllerFile>
#include <WControllerXml>
#include <WControllerNetwork>
#include <WControllerPlaylist>
#include <WAbstractThreadAction>
#include <WLibraryFolderRelated>
#include <WPlaylist>
#include <WTabTrack>

// Private includes
#include <private/WControllerPlaylist_p>

// Namespaces
using namespace QtLP_Private;

//-------------------------------------------------------------------------------------------------
// Static variables

static const int LIBRARYFOLDER_MAX = 500;

//=================================================================================================
// WLibraryFolderItem
//=================================================================================================

WLibraryFolderItem::WLibraryFolderItem(WLibraryItem::Type type, WLocalObject::State stateQuery)
{
    id = -1;

    this->type = type;

    state = WLocalObject::Default;

    this->stateQuery = stateQuery;
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

bool WLibraryFolderItem::isFolder() const
{
    return WLibraryItem::typeIsFolder(type);
}

bool WLibraryFolderItem::isPlaylist() const
{
    return WLibraryItem::typeIsPlaylist(type);
}

//-------------------------------------------------------------------------------------------------

bool WLibraryFolderItem::isValid() const
{
    return (id != -1);
}

bool WLibraryFolderItem::isLocal() const
{
    return source.isEmpty();
}

bool WLibraryFolderItem::isOnline() const
{
    return (source.isEmpty() == false);
}

//=================================================================================================
// WLibraryFolderWatcher
//=================================================================================================

/* virtual */ void WLibraryFolderWatcher::beginItemsInsert(int, int) {}
/* virtual */ void WLibraryFolderWatcher::endItemsInsert  ()         {}

/* virtual */ void WLibraryFolderWatcher::beginItemsMove(int, int, int) {}
/* virtual */ void WLibraryFolderWatcher::endItemsMove  ()              {}

/* virtual */ void WLibraryFolderWatcher::beginItemsRemove(int, int) {}
/* virtual */ void WLibraryFolderWatcher::endItemsRemove  ()         {}

/* virtual */ void WLibraryFolderWatcher::itemUpdated(int) {}

/* virtual */ void WLibraryFolderWatcher::beginItemsClear() {}
/* virtual */ void WLibraryFolderWatcher::endItemsClear  () {}

/* virtual */ void WLibraryFolderWatcher::currentIndexChanged(int) {}

/* virtual */ void WLibraryFolderWatcher::folderDestroyed() {}

//=================================================================================================
// WLibraryFolderWrite and WLibraryFolderWriteReply
//=================================================================================================

class WLibraryFolderWrite : public WAbstractThreadAction
{
    Q_OBJECT

public:
    WLibraryFolderWrite(WLibraryFolderPrivate * data)
    {
        this->data = data;

        name    = sk->name      ();
        version = sk->versionSky();
    }

protected: // WAbstractThreadAction reimplementation
    /* virtual */ WAbstractThreadReply * createReply() const;

protected: // WAbstractThreadAction implementation
    /* virtual */ bool run();

public: // Variables
    WLibraryFolderPrivate * data;

    QString path;

    QString name;
    QString version;

    WLibraryItem::Type type;

    QString source;

    QString title;
    QString cover;

    QString label;

    int currentId;

    qreal scrollValue;

    QList<WLibraryFolderItem> dataItems;
};

//-------------------------------------------------------------------------------------------------

class WLibraryFolderWriteReply : public WAbstractThreadReply
{
    Q_OBJECT

public:
    WLibraryFolderWriteReply(WLibraryFolderPrivate * data)
    {
        this->data = data;
    }

protected: // WAbstractThreadReply reimplementation
    /* virtual */ void onCompleted(bool ok);

public: // Variables
    WLibraryFolderPrivate * data;
};

//=================================================================================================
// WLibraryFolderWrite
//=================================================================================================

/* virtual */ WAbstractThreadReply * WLibraryFolderWrite::createReply() const
{
    return new WLibraryFolderWriteReply(data);
}

/* virtual */ bool WLibraryFolderWrite::run()
{
    QByteArray data;

    QXmlStreamWriter stream(&data);

    stream.setAutoFormatting(true);

    stream.writeStartDocument();

    stream.writeStartElement(name);

    stream.writeTextElement("version", version);

    stream.writeStartElement("folder");

    stream.writeTextElement("type", QString::number(type));

    stream.writeTextElement("source", source);

    stream.writeTextElement("title", title);
    stream.writeTextElement("cover", cover);

    stream.writeTextElement("label", label);

    stream.writeTextElement("currentId", QString::number(currentId));

    stream.writeTextElement("scrollValue", QString::number(scrollValue));

    stream.writeStartElement("items");

    foreach (const WLibraryFolderItem & data, dataItems)
    {
        stream.writeStartElement("item");

        stream.writeTextElement("id", QString::number(data.id));

        stream.writeTextElement("type", QString::number(data.type));

        stream.writeTextElement("state",      QString::number(data.state));
        stream.writeTextElement("stateQuery", QString::number(data.stateQuery));

        stream.writeTextElement("source", data.source);

        stream.writeTextElement("title", data.title);
        stream.writeTextElement("cover", data.cover);

        stream.writeTextElement("label", data.label);

        stream.writeEndElement(); // item
    }

    stream.writeEndElement(); // items
    stream.writeEndElement(); // folder
    stream.writeEndElement(); // name

    stream.writeEndDocument();

    WControllerFile::writeFile(path, data);

    qDebug("FOLDER SAVED");

    return true;
}

//=================================================================================================
// WLibraryFolderWriteReply
//=================================================================================================

/* virtual */ void WLibraryFolderWriteReply::onCompleted(bool ok)
{
    data->setSaved(ok);
}

//=================================================================================================
// WLibraryFolderRead and WLibraryFolderReadReply
//=================================================================================================

class WLibraryFolderReadReply;

class WLibraryFolderRead : public WAbstractThreadAction
{
    Q_OBJECT

public:
    WLibraryFolderRead(WLibraryFolderPrivate * data)
    {
        this->data = data;
    }

protected: // WAbstractThreadAction reimplementation
    /* virtual */ WAbstractThreadReply * createReply() const;

protected: // WAbstractThreadAction implementation
    /* virtual */ bool run();

private: // Functions
    bool loadFolder(QXmlStreamReader * stream, WLibraryFolderReadReply * reply);
    bool loadItems (QXmlStreamReader * stream, WLibraryFolderReadReply * reply);

public: // Variables
    WLibraryFolderPrivate * data;

    QString path;
};

//-------------------------------------------------------------------------------------------------

class WLibraryFolderReadReply : public WAbstractThreadReply
{
    Q_OBJECT

public:
    WLibraryFolderReadReply(WLibraryFolderPrivate * data)
    {
        this->data = data;
    }

protected: // WAbstractThreadReply reimplementation
    /* virtual */ void onCompleted(bool ok);

public: // Variables
    WLibraryFolderPrivate * data;

    WLibraryItem::Type type;

    QString source;

    QString title;
    QString cover;

    QString label;

    int currentId;

    qreal scrollValue;

    QList<WLibraryFolderItem> dataItems;
};

//=================================================================================================
// WLibraryFolderRead
//=================================================================================================

/* virtual */ WAbstractThreadReply * WLibraryFolderRead::createReply() const
{
    return new WLibraryFolderReadReply(data);
}

/* virtual */ bool WLibraryFolderRead::run()
{
    WLibraryFolderReadReply * reply = qobject_cast<WLibraryFolderReadReply *> (this->reply());

    QByteArray data = WControllerFile::readFile(path);

    QXmlStreamReader stream(data);

    if (loadFolder(&stream, reply) == false || loadItems(&stream, reply) == false)
    {
        qWarning("WLibraryFolderRead::run: Invalid file %s.", path.C_STR);

        return false;
    }

    qDebug("FOLDER LOADED");

    return true;
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

bool WLibraryFolderRead::loadFolder(QXmlStreamReader * stream, WLibraryFolderReadReply * reply)
{
    //---------------------------------------------------------------------------------------------
    // type

    if (WControllerXml::readNextStartElement(stream, "type") == false) return false;

    int type = WControllerXml::readNextInt(stream);

    if (type < 0) return false;

    reply->type = static_cast<WLibraryItem::Type> (type);

    //---------------------------------------------------------------------------------------------
    // source

    if (WControllerXml::readNextStartElement(stream, "source") == false) return false;

    reply->source = WControllerXml::readNextString(stream);

    //---------------------------------------------------------------------------------------------
    // title

    if (WControllerXml::readNextStartElement(stream, "title") == false) return false;

    reply->title = WControllerXml::readNextString(stream);

    //---------------------------------------------------------------------------------------------
    // cover

    if (WControllerXml::readNextStartElement(stream, "cover") == false) return false;

    reply->cover = WControllerXml::readNextString(stream);

    //---------------------------------------------------------------------------------------------
    // label

    if (WControllerXml::readNextStartElement(stream, "label") == false) return false;

    reply->label = WControllerXml::readNextString(stream);

    //---------------------------------------------------------------------------------------------
    // currentId

    if (WControllerXml::readNextStartElement(stream, "currentId") == false) return false;

    reply->currentId = WControllerXml::readNextInt(stream);

    //---------------------------------------------------------------------------------------------
    // scrollValue

    if (WControllerXml::readNextStartElement(stream, "scrollValue") == false) return false;

    reply->scrollValue = WControllerXml::readNextFloat(stream);

    return true;
}

bool WLibraryFolderRead::loadItems(QXmlStreamReader * stream, WLibraryFolderReadReply * reply)
{
    while (WControllerXml::readNextStartElement(stream, "item"))
    {
        WLibraryFolderItem data;

        //-----------------------------------------------------------------------------------------
        // id

        if (WControllerXml::readNextStartElement(stream, "id") == false) return false;

        int id = WControllerXml::readNextInt(stream);

        if (id < 0) return false;

        data.id = id;

        //-----------------------------------------------------------------------------------------
        // type

        if (WControllerXml::readNextStartElement(stream, "type") == false) return false;

        int type = WControllerXml::readNextInt(stream);

        if (type < 0) return false;

        data.type = static_cast<WLibraryItem::Type> (type);

        //-----------------------------------------------------------------------------------------
        // state

        if (WControllerXml::readNextStartElement(stream, "state") == false) return false;

        data.state = static_cast<WLocalObject::State> (WControllerXml::readNextInt(stream));

        //-----------------------------------------------------------------------------------------
        // stateQuery

        if (WControllerXml::readNextStartElement(stream, "stateQuery") == false) return false;

        data.stateQuery = static_cast<WLocalObject::State> (WControllerXml::readNextInt(stream));

        //-----------------------------------------------------------------------------------------
        // source

        if (WControllerXml::readNextStartElement(stream, "source") == false) return false;

        data.source = WControllerXml::readNextString(stream);

        //-----------------------------------------------------------------------------------------
        // title

        if (WControllerXml::readNextStartElement(stream, "title") == false) return false;

        data.title = WControllerXml::readNextString(stream);

        //-----------------------------------------------------------------------------------------
        // cover

        if (WControllerXml::readNextStartElement(stream, "cover") == false) return false;

        data.cover = WControllerXml::readNextString(stream);

        //-----------------------------------------------------------------------------------------
        // label

        if (WControllerXml::readNextStartElement(stream, "label") == false) return false;

        data.label = WControllerXml::readNextString(stream);

        //-----------------------------------------------------------------------------------------

        reply->dataItems.append(data);
    }

    return true;
}

//=================================================================================================
// WLibraryFolderReadReply
//=================================================================================================

/* virtual */ void WLibraryFolderReadReply::onCompleted(bool ok)
{
    WLibraryFolder * q = data->q_func();

    bool saveEnabled = q->saveEnabled();

    q->setSaveEnabled(false);

    if (ok)
    {
        if (data->cacheLoad)
        {
            if (data->source.isEmpty()) q->loadSource(source, false);

            if (data->title.isEmpty()) q->setTitle(title);
            if (data->cover.isEmpty()) q->setCover(cover);

            if (data->label.isEmpty()) q->setLabel(label);

            if (data->scrollValue == 0) q->setScrollValue(scrollValue);
        }
        else
        {
            q->loadSource(source, false);

            q->setTitle(title);
            q->setCover(cover);

            q->setLabel(label);

            q->setScrollValue(scrollValue);
        }

        data->loadItems(dataItems);

        q->setSaveEnabled(saveEnabled);

        if (data->futureIds.isEmpty())
        {
            data->defaultId = currentId;

            q->loadCurrentId(currentId, data->instant, data->instant);
        }
    }
    else
    {
        q->loadSource(QString(), false);

        q->setTitle(QString());
        q->setCover(QString());

        q->setLabel(QString());

        q->setScrollValue(0);

        q->setSaveEnabled(saveEnabled);
    }

    if (data->itemNext)
    {
        data->isLoadingItem = true;

        data->actionLoad = NULL;
    }
    else data->setLoaded(ok);
}

//=================================================================================================
// WLibraryFolderPrivate
//=================================================================================================

WLibraryFolderPrivate::WLibraryFolderPrivate(WLibraryFolder * p) : WLibraryItemPrivate(p) {}

/* virtual */ WLibraryFolderPrivate::~WLibraryFolderPrivate()
{
    Q_Q(WLibraryFolder);

    foreach (WLibraryFolderWatcher * watcher, watchers)
    {
        watcher->folderDestroyed();
    }

    deleteItems();

    W_GET_CONTROLLER(WControllerPlaylist, controller);

    if (controller) controller->d_func()->unregisterFolder(q);
}

//-------------------------------------------------------------------------------------------------

void WLibraryFolderPrivate::init()
{
    Q_Q(WLibraryFolder);

    defaultId = -1;
    currentId = -1;
    activeId  = -1;

    scrollValue = 0;

    currentItem = NULL;

    currentIndex = -1;
    activeIndex  = -1;

    itemNext = NULL;
    itemLock = NULL;

    isLoadingItem = false;

    maxCount = LIBRARYFOLDER_MAX;

    wControllerPlaylist->d_func()->registerFolder(q);
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WLibraryFolderPrivate::registerItemId(WLibraryItem * item)
{
    int id = item->id();

    if (id == -1) return;

    idHash.insert(id, item);

    updateLock(item, item->isLocked());
}

void WLibraryFolderPrivate::unregisterItemId(WLibraryItem * item)
{
    int id = item->id();

    if (id == -1) return;

    if (item == itemNext)
    {
        Q_Q(WLibraryFolder);

        QObject::disconnect(itemNext, SIGNAL(loaded()), q, SLOT(onItemNextLoaded()));

        itemNext = NULL;
    }
    else if (item == currentItem)
    {
        setCurrentItem(NULL);
    }

    idHash.remove(id);

    itemIds.remove(item);

    updateLock(item, false);
}

//-------------------------------------------------------------------------------------------------

void WLibraryFolderPrivate::setCurrentItem(WLibraryItem * item)
{
    Q_Q(WLibraryFolder);

    if (itemNext && currentItem == item)
    {
        clearItemNext();

        return;
    }

    WLibraryItem * oldItem = currentItem;

    currentItem = item;

    if (item)
    {
        int id = item->id();

        currentId    = id;
        currentIndex = q->indexFromId(id);
    }
    else
    {
        currentId    = -1;
        currentIndex = -1;
    }

    currentIndexChanged();

    emit q->currentIdChanged   ();
    emit q->currentIndexChanged();

    if (currentItem)
    {
        updateLock(currentItem, currentItem->isLocked());
    }

    if (oldItem) oldItem->tryDelete();

    if (itemNext)
    {
        QObject::disconnect(itemNext, SIGNAL(loaded()), q, SLOT(onItemNextLoaded()));

        if (item == NULL)
        {
            item = itemNext;

            itemNext = NULL;

            item->tryDelete();
        }
        else itemNext = NULL;

        if (isLoadingItem)
        {
            isLoadingItem = false;

            q->setLoaded(true);
        }
    }

    if (defaultId != -1)
    {
        if (defaultId == currentId)
        {
            defaultId = -1;

            return;
        }
        else defaultId = -1;
    }

    q->save();
}

//-------------------------------------------------------------------------------------------------

void WLibraryFolderPrivate::clearItemNext()
{
    Q_Q(WLibraryFolder);

    QObject::disconnect(itemNext, SIGNAL(loaded()), q, SLOT(onItemNextLoaded()));

    WLibraryItem * item = itemNext;

    itemNext = NULL;

    item->tryDelete();

    if (isLoadingItem)
    {
        isLoadingItem = false;

        q->setLoaded(true);
    }
}

//-------------------------------------------------------------------------------------------------

void WLibraryFolderPrivate::deleteFile(int id)
{
    Q_Q(WLibraryFolder);

    const WLibraryFolderItem * item = q->itemFromId(id);

    QString path = q->pathChildFile(id);

    if (QFile::exists(path))
    {
        WControllerFile::deleteFile(path);
    }

    if (item->isFolder())
    {
        path = q->pathChildFolder(id);

        if (QFile::exists(path))
        {
            WControllerFile::deleteFolder(path);
        }
    }

    QList<int> idFull = q->idFull();

    idFull.append(id);

    emit wControllerPlaylist->filesDeleted(idFull);
}

//-------------------------------------------------------------------------------------------------

WLibraryFolderItem * WLibraryFolderPrivate::itemFromId(int id)
{
    Q_Q(WLibraryFolder);

    return itemAt(q->indexFromId(id));
}

WLibraryFolderItem * WLibraryFolderPrivate::itemAt(int index)
{
    if (index < 0 || index >= items.count())
    {
         return NULL;
    }
    else return &(items[index]);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WLibraryFolderPrivate::updateLock(WLibraryItem * item, bool locked)
{
    Q_Q(WLibraryFolder);

    if (locked)
    {
        if (itemLock != NULL) return;

        itemLock = item;

        q->setBlocked(true);
    }
    else
    {
        if (itemLock && itemLock != item) return;

        if (currentItem && currentItem->d_func()->locked)
        {
            itemLock = currentItem;

            q->setBlocked(true);
        }
        else
        {
            QHashIterator<int, WLibraryItem *> i(idHash);

            while (i.hasNext())
            {
                i.next();

                WLibraryItem * item = i.value();

                if (item != currentItem)
                {
                    itemLock = item;

                    q->setBlocked(true);

                    return;
                }
            }

            itemLock = NULL;

            q->setBlocked(false);
        }
    }
}

//-------------------------------------------------------------------------------------------------

void WLibraryFolderPrivate::loadItems(const QList<WLibraryFolderItem> & items)
{
    Q_Q(WLibraryFolder);

    if (this->items.count())
    {
        setCurrentItem(NULL);

        q->setActiveId(-1);

        beginItemsClear();

        this->items.clear();

        endItemsClear();

        deleteItems();

        ids   .clear();
        idHash.clear();

        itemIds.clear();

        emit q->itemsCleared();

        if (items.isEmpty())
        {
            emit q->countChanged();
        }
        else q->addItems(items);
    }
    else if (items.count())
    {
        q->addItems(items);
    }
}

//-------------------------------------------------------------------------------------------------

void WLibraryFolderPrivate::deleteItems()
{
    QHashIterator<int, WLibraryItem *> i(idHash);

    while (i.hasNext())
    {
        i.next();

        WLibraryItem * item = i.value();

        if (item)
        {
            item->d_func()->parentFolder = NULL;

            item->abortAndDelete();
        }
    }
}

//---------------------------------------------------------------------------------------------
// WLibraryItem interface

void WLibraryFolderPrivate::updateItemType(int id, WLibraryItem::Type type)
{
    Q_Q(WLibraryFolder);

    int index = q->indexFromId(id);

    WLibraryFolderItem * item = itemAt(index);

    if (item == NULL || item->type == type) return;

    item->type = type;

    itemUpdated(type);

    q->save();
}

void WLibraryFolderPrivate::updateItemState(int id, WLocalObject::State state)
{
    Q_Q(WLibraryFolder);

    int index = q->indexFromId(id);

    WLibraryFolderItem * item = itemAt(index);

    if (item == NULL || item->state == state) return;

    item->state = state;

    itemUpdated(index);

    q->save();
}

void WLibraryFolderPrivate::updateItemStateQuery(int id, WLocalObject::State state)
{
    Q_Q(WLibraryFolder);

    int index = q->indexFromId(id);

    WLibraryFolderItem * item = itemAt(index);

    if (item == NULL || item->stateQuery == state) return;

    item->stateQuery = state;

    itemUpdated(index);

    q->save();
}

void WLibraryFolderPrivate::updateItemSource(int id, const QString & source)
{
    Q_Q(WLibraryFolder);

    int index = q->indexFromId(id);

    WLibraryFolderItem * item = itemAt(index);

    if (item == NULL || item->source == source) return;

    item->source = source;

    itemUpdated(index);

    q->save();
}

void WLibraryFolderPrivate::updateItemTitle(int id, const QString & title)
{
    Q_Q(WLibraryFolder);

    int index = q->indexFromId(id);

    WLibraryFolderItem * item = itemAt(index);

    if (item == NULL || item->title == title) return;

    item->title = title;

    itemUpdated(index);

    q->save();
}

void WLibraryFolderPrivate::updateItemCover(int id, const QString & cover)
{
    Q_Q(WLibraryFolder);

    int index = q->indexFromId(id);

    WLibraryFolderItem * item = itemAt(index);

    if (item == NULL || item->cover == cover) return;

    item->cover = cover;

    itemUpdated(index);

    q->save();
}

void WLibraryFolderPrivate::updateItemLabel(int id, const QString & label)
{
    Q_Q(WLibraryFolder);

    int index = q->indexFromId(id);

    WLibraryFolderItem * item = itemAt(index);

    if (item == NULL || item->label == label) return;

    item->label = label;

    itemUpdated(index);

    q->save();
}

//-------------------------------------------------------------------------------------------------
// WLibraryFolderWatcher

void WLibraryFolderPrivate::beginItemsInsert(int first, int last) const
{
    foreach (WLibraryFolderWatcher * watcher, watchers)
    {
        watcher->beginItemsInsert(first, last);
    }
}

void WLibraryFolderPrivate::endItemsInsert() const
{
    foreach (WLibraryFolderWatcher * watcher, watchers)
    {
        watcher->endItemsInsert();
    }
}

//-------------------------------------------------------------------------------------------------

void WLibraryFolderPrivate::beginItemsMove(int first, int last, int to) const
{
    foreach (WLibraryFolderWatcher * watcher, watchers)
    {
        watcher->beginItemsMove(first, last, to);
    }
}

void WLibraryFolderPrivate::endItemsMove() const
{
    foreach (WLibraryFolderWatcher * watcher, watchers)
    {
        watcher->endItemsMove();
    }
}

//-------------------------------------------------------------------------------------------------

void WLibraryFolderPrivate::beginItemsRemove(int first, int last) const
{
    foreach (WLibraryFolderWatcher * watcher, watchers)
    {
        watcher->beginItemsRemove(first, last);
    }
}

void WLibraryFolderPrivate::endItemsRemove() const
{
    foreach (WLibraryFolderWatcher * watcher, watchers)
    {
        watcher->endItemsRemove();
    }
}

//-------------------------------------------------------------------------------------------------

void WLibraryFolderPrivate::itemUpdated(int index) const
{
    foreach (WLibraryFolderWatcher * watcher, watchers)
    {
        watcher->itemUpdated(index);
    }
}

//-------------------------------------------------------------------------------------------------

void WLibraryFolderPrivate::beginItemsClear() const
{
    foreach (WLibraryFolderWatcher * watcher, watchers)
    {
        watcher->beginItemsClear();
    }
}

void WLibraryFolderPrivate::endItemsClear() const
{
    foreach (WLibraryFolderWatcher * watcher, watchers)
    {
        watcher->endItemsClear();
    }
}

//-------------------------------------------------------------------------------------------------

void WLibraryFolderPrivate::currentIndexChanged()
{
    foreach (WLibraryFolderWatcher * watcher, watchers)
    {
        watcher->currentIndexChanged(currentIndex);
    }
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WLibraryFolderPrivate::onFolderLoaded()
{
    Q_Q(WLibraryFolder);

    QMultiHash<WLibraryItem *, QList<int> >::iterator i = itemIds.find(q);

    while (i != itemIds.end() && i.key() == q)
    {
        q->loadLibraryItems(i.value());

        ++i;
    }

    itemIds.remove(q);

    QObject::disconnect(q, SIGNAL(loaded()), q, SLOT(onItemLoaded()));
}

void WLibraryFolderPrivate::onItemNextLoaded()
{
    setCurrentItem(itemNext);
}

void WLibraryFolderPrivate::onItemLoaded()
{
    Q_Q(WLibraryFolder);

    WLibraryFolder * folder = static_cast<WLibraryFolder *> (q->sender());

    QMultiHash<WLibraryItem *, QList<int> >::iterator i = itemIds.find(folder);

    while (i != itemIds.end() && i.key() == q)
    {
        folder->loadLibraryItems(i.value());

        ++i;
    }

    itemIds.remove(folder);

    QObject::disconnect(folder, SIGNAL(loaded()), q, SLOT(onItemLoaded()));
}

//=================================================================================================
// WLibraryFolder
//=================================================================================================

/* explicit */ WLibraryFolder::WLibraryFolder(WLibraryFolder * parent)
    : WLibraryItem(new WLibraryFolderPrivate(this), Folder, parent)
{
    Q_D(WLibraryFolder); d->init();
}

//-------------------------------------------------------------------------------------------------
// Protected

WLibraryFolder::WLibraryFolder(WLibraryFolderPrivate * p, Type type, WLibraryFolder * parent)
    : WLibraryItem(p, type, parent)
{
    Q_D(WLibraryFolder); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ WLibraryFolderItem WLibraryFolder::createFolderItem(WLibraryItem * item)
{
    Q_ASSERT(item);

    WLibraryFolder * parentFolder = item->d_func()->parentFolder;

    if (parentFolder)
    {
        if (parentFolder != this)
        {
            qWarning("WLibraryFolder::createFolderItem: Item already has a parent folder.");

            return WLibraryFolderItem();
        }
    }
    else item->setParentFolder(this);

    Q_D(WLibraryFolder);

    int id = item->d_func()->id;

    if (id == -1)
    {
        item->setId(d->ids.generateId());
    }
    else if (d->ids.insertId(id) == false)
    {
        qWarning("WLibraryFolder::createFolderItem: Id is already taken '%d'.", id);

        item->setId(d->ids.generateId());
    }

    WLibraryItemPrivate * p = item->d_func();

    WLibraryFolderItem folderItem;

    folderItem.id = p->id;

    folderItem.type = p->type;

    folderItem.state      = p->state;
    folderItem.stateQuery = p->stateQuery;

    folderItem.source = p->source;

    folderItem.title = p->title;
    folderItem.cover = p->cover;

    folderItem.label = p->label;

    return folderItem;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WLibraryFolder::addLibraryItem(WLibraryItem * item)
{
    Q_ASSERT(item);

    insertItem(count(), createFolderItem(item));
}

/* Q_INVOKABLE */ void WLibraryFolder::addLibraryItems(const QList<WLibraryItem *> & items)
{
    insertLibraryItems(count(), items);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WLibraryFolder::insertLibraryItem(int index, WLibraryItem * item)
{
    Q_ASSERT(item);

    insertItem(index, createFolderItem(item));
}

/* Q_INVOKABLE */ void WLibraryFolder::insertLibraryItems(int index,
                                                          const QList<WLibraryItem *> & items)
{
    QList<WLibraryFolderItem> folderItems;

    foreach (WLibraryItem * item, items)
    {
        Q_ASSERT(item);

        folderItems.append(createFolderItem(item));
    }

    insertItems(index, folderItems);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WLibraryFolder::addItem(const WLibraryFolderItem & item)
{
    insertItems(count(), QList<WLibraryFolderItem>() << item);
}

/* Q_INVOKABLE */ void WLibraryFolder::addItems(const QList<WLibraryFolderItem> & items)
{
    insertItems(count(), items);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WLibraryFolder::insertItem(int index, const WLibraryFolderItem & item)
{
    insertItems(index, QList<WLibraryFolderItem>() << item);
}

/* Q_INVOKABLE */ void WLibraryFolder::insertItems(int index,
                                                   const QList<WLibraryFolderItem> & items)
{
    Q_D(WLibraryFolder);

    int countAdd = items.count();

    if (countAdd == 0 || checkFull(countAdd)) return;

    int count = d->items.count();

    if (index < 0 || index > count)
    {
        index = count;
    }

    int oldIndex = index;

    d->beginItemsInsert(index, index + countAdd - 1);

    foreach (const WLibraryFolderItem & item, items)
    {
        d->items.insert(index, item);

        int id = item.id;

        if (id != -1)
        {
            d->ids.insertId(id);

            WLibraryItem * libraryItem = d->idHash.value(id);

            if (libraryItem) libraryItem->save();
        }
        else d->items[index].id = d->ids.generateId();

        index++;
    }

    d->endItemsInsert();

    updateIndex();

    emit countChanged();

    emit itemsInserted(oldIndex, countAdd);

    save();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WLibraryFolder::addNewItem(Type type,
                                                  const QString & source,
                                                  const QString & title,
                                                  const QString & cover)
{
    insertNewItem(count(), type, source, title, cover);
}

/* Q_INVOKABLE */ void WLibraryFolder::insertNewItem(int index, Type type,
                                                     const QString & source,
                                                     const QString & title,
                                                     const QString & cover)
{
    WLibraryFolderItem item;

    item.type = type;

    item.source = source;

    item.title = title;
    item.cover = cover;

    insertItem(index, item);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WLibraryFolder::moveItem(int id, int to)
{
    int from = indexFromId(id);

    moveAt(from, to);
}

/* Q_INVOKABLE */ void WLibraryFolder::moveAt(int from, int to)
{
    Q_D(WLibraryFolder);

    int count = d->items.count();

    if (from < 0 || from > count
        ||
        to < 0 || to > count) return;

    if (from <= to && from > to - 2) return;

    d->beginItemsMove(from, from, to);

    if (from < to)
    {
         d->items.move(from, to - 1);
    }
    else d->items.move(from, to);

    d->endItemsMove();

    updateIndex();

    QList<int> indexes;

    indexes.append(from);

    emit itemsMoved(indexes, to);

    save();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WLibraryFolder::moveItemFrom(WLibraryFolder * source, int from, int to)
{
    move(source, from, this, to);
}

/* Q_INVOKABLE */ void WLibraryFolder::moveItemTo(int from, WLibraryFolder * destination, int to)
{
    move(this, from, destination, to);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WLibraryFolder::removeItem(int id)
{
    removeAt(indexFromId(id));
}

/* Q_INVOKABLE */ void WLibraryFolder::removeAt(int index)
{
    removeItems(QList<int>() << index);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WLibraryFolder::removeItems(const QList<int> & indexes)
{
    Q_D(WLibraryFolder);

    if (indexes.isEmpty()) return;

    QList<int> sortedIndexes = indexes;

    std::sort(sortedIndexes.begin(), sortedIndexes.end());

    QList<int> ids;

    foreach (int index, sortedIndexes)
    {
        if (index < 0 || index >= d->items.count()) continue;

        int id = d->items.at(index).id;

        ids.append(id);
    }

    foreach (int id, ids)
    {
        if (d->currentId == id)
        {
            d->setCurrentItem(NULL);
        }

        if (d->activeId == id)
        {
            setActiveId(-1);
        }

        d->ids.removeOne(id);

        WLibraryItem * item = d->idHash.take(id);

        d->itemIds.remove(item);

        d->deleteFile(id);

        int index = indexFromId(id);

        d->beginItemsRemove(index, index);

        d->items.removeAt(index);

        d->endItemsRemove();

        if (item)
        {
            item->d_func()->parentFolder = NULL;

            item->abortAndDelete();
        }
    }

    updateIndex();

    emit countChanged();

    emit itemsRemoved(sortedIndexes);

    save();
}

/* Q_INVOKABLE */ void WLibraryFolder::removeItems(int from, int count)
{
    QList<int> indexes;

    while (count)
    {
        indexes.append(from);

        from++;

        count--;
    }

    removeItems(indexes);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WLibraryFolder::clearItems()
{
    Q_D(WLibraryFolder);

    abortAll();

    wControllerPlaylist->d_func()->abortQueriesItem(this);

    deleteFolder(true);

    if (d->items.count())
    {
        d->setCurrentItem(NULL);

        setActiveId(-1);

        setScrollValue(0);

        d->beginItemsClear();

        d->items.clear();

        d->endItemsClear();

        d->deleteItems();

        d->ids   .clear();
        d->idHash.clear();

        d->itemIds.clear();

        emit itemsCleared();

        emit countChanged();
    }

    save();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WLibraryFolder::loadItem(int at)
{
    Q_D(WLibraryFolder);

    if (at < 0 || at >= d->items.count()) return;

    const WLibraryFolderItem & item = d->items.at(at);

    if (item.stateQuery != Loading) return;

    WLibraryItem * libraryItem = createLibraryItemAt(at);

    libraryItem->tryDelete();
}

/* Q_INVOKABLE */ void WLibraryFolder::loadItems(int at, int count)
{
    Q_D(WLibraryFolder);

    if (at < 0 || at >= d->items.count()) return;

    int index = at - count / 2;

    if (index < 0) index = 0;

    while (index < at)
    {
        const WLibraryFolderItem & item = d->items.at(index);

        if (item.stateQuery == Default) break;

        index++;
    }

    while (index < d->items.count() && count)
    {
        const WLibraryFolderItem & item = d->items.at(index);

        if (item.stateQuery == Default)
        {
            WLibraryItem * libraryItem = createLibraryItem(item);

            libraryItem->loadQuery();

            libraryItem->tryDelete();
        }

        count--;

        index++;
    }
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WLibraryFolder::abortItems()
{
    Q_D(WLibraryFolder);

    bool changed = false;

    QHashIterator<int, WLibraryItem *> i(d->idHash);

    while (i.hasNext())
    {
        i.next();

        WLibraryItem * libraryItem = i.value();

        if (libraryItem->abortQuery())
        {
            int index = indexFromId(libraryItem->d_func()->id);

            WLibraryFolderItem * item = &(d->items[index]);

            item->stateQuery = Default;

            changed = true;
        }
    }

    if (changed) this->save();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ bool WLibraryFolder::checkFull(int count) const
{
    Q_D(const WLibraryFolder);

    return (this->count() + count > d->maxCount);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ bool WLibraryFolder::contains(int id) const
{
    return (indexFromId(id) != -1);
}

/* Q_INVOKABLE */ bool WLibraryFolder::containsSource(const QString & source) const
{
    return (indexFromSource(source) != -1);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ int WLibraryFolder::idAt(int index) const
{
    Q_D(const WLibraryFolder);

    if (index < 0 || index >= d->items.count())
    {
         return -1;
    }
    else return d->items.at(index).id;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ int WLibraryFolder::indexOf(const WLibraryFolderItem * item) const
{
    Q_D(const WLibraryFolder);

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

/* Q_INVOKABLE */ int WLibraryFolder::indexFromId(int id) const
{
    Q_D(const WLibraryFolder);

    for (int i = 0; i < d->items.count(); i++)
    {
        if (d->items.at(i).id == id)
        {
            return i;
        }
    }

    return -1;
}

/* Q_INVOKABLE */ int WLibraryFolder::indexFromSource(const QString & source,
                                                      bool            noFragment) const
{
    Q_D(const WLibraryFolder);

    if (noFragment)
    {
        QString string = WControllerNetwork::removeUrlFragment(source);

        for (int i = 0; i < d->items.count(); i++)
        {
            if (WControllerNetwork::removeUrlFragment(d->items.at(i).source) == string)
            {
                return i;
            }
        }
    }
    else
    {
        for (int i = 0; i < d->items.count(); i++)
        {
            if (d->items.at(i).source == source)
            {
                return i;
            }
        }
    }

    return -1;
}

/* Q_INVOKABLE */ int WLibraryFolder::indexFromLabel(const QString & label) const
{
    Q_D(const WLibraryFolder);

    for (int i = 0; i < d->items.count(); i++)
    {
        if (d->items.at(i).label == label)
        {
            return i;
        }
    }

    return -1;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ WLibraryFolderItem WLibraryFolder::getItemAt(int index) const
{
    Q_D(const WLibraryFolder);

    if (index < 0 || index >= d->items.count())
    {
         return WLibraryFolderItem();
    }
    else return d->items.at(index);
}

/* Q_INVOKABLE */ WLibraryFolderItem WLibraryFolder::getItemFromId(int id) const
{
    return getItemAt(indexFromId(id));
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ const WLibraryFolderItem * WLibraryFolder::itemAt(int index) const
{
    Q_D(const WLibraryFolder);

    if (index < 0 || index >= d->items.count())
    {
         return NULL;
    }
    else return &(d->items.at(index));
}

/* Q_INVOKABLE */ const WLibraryFolderItem * WLibraryFolder::itemFromId(int id) const
{
    return itemAt(indexFromId(id));
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ const WLibraryFolderItem * WLibraryFolder::currentItemPointer() const
{
    Q_D(const WLibraryFolder);

    return itemFromId(d->currentId);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WLibraryFolder::loadCurrentId(int id, bool instantSet,
                                                             bool instantLoad)
{

    Q_D(WLibraryFolder);

    if (d->currentId == id)
    {
        if (d->itemNext && d->itemNext->d_func()->id != id)
        {
            d->clearItemNext();
        }

        return;
    }

    if (id == -1)
    {
        d->setCurrentItem(NULL);

        return;
    }

    if (contains(id) == false) return;

    if (d->itemNext)
    {
        if (d->itemNext->d_func()->id == id)
        {
            if (instantSet) d->setCurrentItem(d->itemNext);

            return;
        }

        d->clearItemNext();
    }

    d->itemNext = createLibraryItemFromId(id, instantLoad);

    if (instantSet == false && d->itemNext->d_func()->actionLoad)
    {
         connect(d->itemNext, SIGNAL(loaded()), this, SLOT(onItemNextLoaded()));
    }
    else d->setCurrentItem(d->itemNext);
}

/* Q_INVOKABLE */ void WLibraryFolder::loadCurrentIndex(int index, bool instantSet,
                                                                   bool instantLoad)
{
    int id = idAt(index);

    loadCurrentId(id, instantSet, instantLoad);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WLibraryFolder::loadLibraryItems(const QList<int> & ids)
{
    if (ids.isEmpty()) return;

    Q_D(WLibraryFolder);

    if (isLoading())
    {
        if (d->itemIds.contains(this) == false)
        {
            connect(this, SIGNAL(loaded()), this, SLOT(onFolderLoaded()));
        }

        d->itemIds.insert(this, ids);

        return;
    }

    QList<int> folderIds = ids;

    int id = folderIds.takeFirst();

    if (id == -1) return;

    if (folderIds.empty())
    {
        WLibraryItem * item = createLibraryItemFromId(id);

        if (item) item->tryDelete();

        return;
    }

    WLibraryItem * item = createLibraryItemFromId(id);

    if (item == NULL) return;

    if (item->isLoading())
    {
        if (d->itemIds.contains(item) == false)
        {
            connect(item, SIGNAL(loaded()), this, SLOT(onItemLoaded()));
        }

        d->itemIds.insert(item, folderIds);
    }
    else if (item->isFolder())
    {
        item->toFolder()->loadLibraryItems(folderIds);
    }

    item->tryDelete();
}

/* Q_INVOKABLE */ void WLibraryFolder::loadTabItems(WTabTrack * tab)
{
    Q_ASSERT(tab);

    loadLibraryItems(tab->folderIds());
}

//---------------------------------------------------------------------------------------------
// Item interface

/* Q_INVOKABLE */ WLibraryItem::Type WLibraryFolder::itemType(int index) const
{
    const WLibraryFolderItem * item = itemAt(index);

    if (item)
    {
         return item->type;
    }
    else return Item;
}

/* Q_INVOKABLE */ bool WLibraryFolder::itemIsPlaylist(int index) const
{
    const WLibraryFolderItem * item = itemAt(index);

    if (item)
    {
         return typeIsPlaylist(item->type);
    }
    else return false;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ WLocalObject::State WLibraryFolder::itemState(int index) const
{
    const WLibraryFolderItem * item = itemAt(index);

    if (item)
    {
         return item->state;
    }
    else return WLocalObject::Default;
}

/* Q_INVOKABLE */ WLocalObject::State WLibraryFolder::itemStateQuery(int index) const
{
    const WLibraryFolderItem * item = itemAt(index);

    if (item)
    {
         return item->stateQuery;
    }
    else return WLocalObject::Loaded;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ QString WLibraryFolder::itemSource(int index) const
{
    const WLibraryFolderItem * item = itemAt(index);

    if (item)
    {
         return item->source;
    }
    else return QString();
}

/* Q_INVOKABLE */ void WLibraryFolder::setItemSource(int index, const QString & source)
{
    WLibraryItem * item = createLibraryItemAt(index);

    if (item == NULL) return;

    item->setSource(source);

    item->tryDelete();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ bool WLibraryFolder::itemIsLocal(int index) const
{
    return itemSource(index).isEmpty();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ QString WLibraryFolder::itemTitle(int index) const
{
    const WLibraryFolderItem * item = itemAt(index);

    if (item)
    {
         return item->title;
    }
    else return QString();
}

/* Q_INVOKABLE */ void WLibraryFolder::setItemTitle(int index, const QString & title)
{
    WLibraryItem * item = createLibraryItemAt(index);

    if (item == NULL) return;

    item->setTitle(title);

    item->tryDelete();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ QString WLibraryFolder::itemCover(int index) const
{
    const WLibraryFolderItem * item = itemAt(index);

    if (item)
    {
         return item->cover;
    }
    else return QString();
}

/* Q_INVOKABLE */ void WLibraryFolder::setItemCover(int index, const QString & cover)
{
    WLibraryItem * item = createLibraryItemAt(index);

    if (item == NULL) return;

    item->setCover(cover);

    item->tryDelete();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ QString WLibraryFolder::itemLabel(int index) const
{
    const WLibraryFolderItem * item = itemAt(index);

    if (item)
    {
         return item->label;
    }
    else return QString();
}

/* Q_INVOKABLE */ void WLibraryFolder::setItemLabel(int index, const QString & label)
{
    WLibraryItem * item = createLibraryItemAt(index);

    if (item == NULL) return;

    item->setLabel(label);

    item->tryDelete();
}

//-------------------------------------------------------------------------------------------------
// WLibraryItem

/* Q_INVOKABLE */
WLibraryItem * WLibraryFolder::createLibraryItem(const WLibraryFolderItem & item, bool instant)
{
    Q_D(WLibraryFolder);

    WLibraryItem * libraryItem = d->idHash.value(item.id);

    if (libraryItem == NULL)
    {
        if (item.id == -1) return NULL;

        if (item.type == Item)
        {
            libraryItem = new WLibraryItem(this);
        }
        else if (item.type == Folder)
        {
            libraryItem = new WLibraryFolder(this);
        }
        else if (item.type == FolderSearch)
        {
            libraryItem = new WLibraryFolderSearch(this);
        }
        else if (item.type == FolderSearchable)
        {
            libraryItem = new WLibraryFolderSearchable(this);
        }
        else if (item.type == FolderRelated)
        {
            libraryItem = new WLibraryFolderRelated(this);
        }
        else if (item.type == Playlist)
        {
            libraryItem = new WPlaylist(this);
        }
        else if (item.type == PlaylistFeed)
        {
            libraryItem = new WPlaylistFeed(this);
        }
        else if (item.type == PlaylistSearch)
        {
            libraryItem = new WPlaylistSearch(this);
        }
        else libraryItem = createItem(item.type, this);

        libraryItem->setSaveEnabled(false);

        libraryItem->setId(item.id);

        if (item.stateQuery == Loading)
        {
             libraryItem->d_func()->stateQuery = Default;
        }
        else libraryItem->d_func()->stateQuery = item.stateQuery;

        libraryItem->d_func()->source = item.source;

        libraryItem->setTitle(item.title);
        libraryItem->setCover(item.cover);

        libraryItem->setLabel(item.label);

        libraryItem->d_func()->cacheLoad = true;

        libraryItem->setSaveEnabled(d->saveEnabled);

        if (libraryItem->load(instant) == false)
        {
            libraryItem->d_func()->cacheLoad = false;
        }
    }
    else libraryItem->addDeleteLock();

    return libraryItem;
}

/* Q_INVOKABLE */ WLibraryItem * WLibraryFolder::createLibraryItemFromId(int id, bool instant)
{
    const WLibraryFolderItem * item = itemFromId(id);

    if (item)
    {
         return createLibraryItem(*item, instant);
    }
    else return NULL;
}

/* Q_INVOKABLE */ WLibraryItem * WLibraryFolder::createLibraryItemAt(int index, bool instant)
{
    const WLibraryFolderItem * item = itemAt(index);

    if (item)
    {
         return createLibraryItem(*item, instant);
    }
    else return NULL;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ WLibraryItem * WLibraryFolder::getLibraryItemFromId(int id) const
{
    Q_D(const WLibraryFolder);

    return d->idHash.value(id);
}

/* Q_INVOKABLE */ WLibraryItem * WLibraryFolder::getLibraryItemAt(int index) const
{
    return getLibraryItemFromId(idAt(index));
}

//-------------------------------------------------------------------------------------------------
// Watchers

/* Q_INVOKABLE */ void WLibraryFolder::registerWatcher(WLibraryFolderWatcher * watcher)
{
    Q_ASSERT(watcher);

    Q_D(WLibraryFolder);

    if (d->watchers.contains(watcher)) return;

    d->watchers.append(watcher);
}

/* Q_INVOKABLE */ void WLibraryFolder::unregisterWatcher(WLibraryFolderWatcher * watcher)
{
    Q_ASSERT(watcher);

    Q_D(WLibraryFolder);

    if (d->watchers.contains(watcher) == false) return;

    d->watchers.removeOne(watcher);
}

//-------------------------------------------------------------------------------------------------
// Static functions
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ WLibraryFolder * WLibraryFolder::create(Type type)
{
    if (type == FolderSearch)
    {
        return new WLibraryFolderSearch;
    }
    else if (type == FolderSearchable)
    {
        return new WLibraryFolderSearchable;
    }
    else if (type == FolderRelated)
    {
        return new WLibraryFolderRelated;
    }
    else return new WLibraryFolder;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ bool WLibraryFolder::move(WLibraryFolder * folderA, int from,
                                                   WLibraryFolder * folderB, int to,
                                                   bool remove)
{
    Q_ASSERT(folderA); Q_ASSERT(folderB);

    if (from < 0 || from >= folderA->count()) return false;

    WLibraryFolderItem itemA = folderA->getItemAt(from);

    int idA = itemA.id;

    itemA.id = -1;

    int count = folderB->count();

    if (to < 0 || to > count)
    {
        to = count;
    }

    folderB->insertItem(to, itemA);

    int idB = folderB->idAt(to);

    QString pathA = folderA->pathChildFile(idA);
    QString pathB = folderB->pathChildFile(idB);

    WControllerFile::renameFile(pathA, pathB);

    if (itemA.isFolder())
    {
        pathA = folderA->pathChildFolder(idA);
        pathB = folderB->pathChildFolder(idB);

        WControllerFile::moveFolder(pathA, pathB);
    }

    if (remove) folderA->removeAt(from);

    return true;
}

//---------------------------------------------------------------------------------------------
// WLibraryItem reimplementation
//---------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QString WLibraryFolder::toVbml(bool expand) const
{
    Q_D(const WLibraryFolder);

    QString vbml = WControllerPlaylist::vbml();

    Sk::bmlPair(vbml, "type", "folder", "\n\n");

    Sk::bmlPair(vbml, "source", d->source, "\n\n");

    Sk::bmlPair(vbml, "title", d->title, "\n\n");
    Sk::bmlPair(vbml, "cover", d->cover, "\n\n");

    Sk::bmlTag(vbml, "items");

    QString tabA = Sk::tabs(1);

    if (expand)
    {
        QString tabB = Sk::tabs(2);

        foreach (const WLibraryFolderItem & item, d->items)
        {
            Sk::bmlTag(vbml, tabA + WLibraryItem::typeToString(item.type));

            Sk::bmlPair(vbml, tabB + "source", item.source);

            Sk::bmlPair(vbml, tabB + "title", item.title);
            Sk::bmlPair(vbml, tabB + "cover", item.cover);

            Sk::bmlPair(vbml, tabB + "label", item.label);

            vbml.append('\n');
        }
    }
    else
    {
        W_FOREACH (const WLibraryFolderItem & item, d->items)
        {
            Sk::bmlPair(vbml, tabA, item.source);
        }
    }

    // NOTE: We clear the last '\n'.
    if (d->items.isEmpty() == false) vbml.chop(1);

    return vbml;
}

//-------------------------------------------------------------------------------------------------
// Protected functions
//-------------------------------------------------------------------------------------------------

void WLibraryFolder::updateIndex()
{
    Q_D(WLibraryFolder);

    int index = indexFromId(d->currentId);

    if (d->currentIndex != index)
    {
        d->currentIndex = index;

        d->currentIndexChanged();

        emit currentIndexChanged();
    }

    index = indexFromId(d->activeId);

    if (d->activeIndex != index)
    {
        d->activeIndex = index;

        emit activeIndexChanged();
    }
}

//-------------------------------------------------------------------------------------------------
// Protected Virtual functions
//-------------------------------------------------------------------------------------------------

/* virtual */ WLibraryItem * WLibraryFolder::createItem(Type, WLibraryFolder *)
{
    Q_ASSERT(0); return NULL;
}

//-------------------------------------------------------------------------------------------------
// Protected WLocalObject reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WLibraryFolder::applyId(int id)
{
    Q_D(WLibraryFolder);

    WLibraryItem::applyId(id);

    QHashIterator<int, WLibraryItem *> i(d->idHash);

    while (i.hasNext())
    {
        i.next();

        WLibraryItem * item = i.value();

        item->applyId(i.key());
    }
}

//-------------------------------------------------------------------------------------------------

/* virtual */ WAbstractThreadAction * WLibraryFolder::onSave(const QString & path)
{
    Q_D(WLibraryFolder);

    WLibraryFolderWrite * action = new WLibraryFolderWrite(d);

    action->path = path;

    action->type = d->type;

    action->source = d->source;

    action->title = d->title;
    action->cover = d->cover;

    action->label = d->label;

    action->currentId = d->currentId;

    action->scrollValue = d->scrollValue;

    foreach (const WLibraryFolderItem & item, d->items)
    {
        action->dataItems.append(item);
    }

    return action;
}

/* virtual */ WAbstractThreadAction * WLibraryFolder::onLoad(const QString & path)
{
    Q_D(WLibraryFolder);

    WLibraryFolderRead * action = new WLibraryFolderRead(d);

    action->path = path;

    return action;
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WLibraryFolder::onSaveEnabledChanged(bool enabled)
{
    Q_D(WLibraryFolder);

    QHashIterator<int, WLibraryItem *> i(d->idHash);

    while (i.hasNext())
    {
        i.next();

        i.value()->setSaveEnabled(enabled);
    }
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WLibraryFolder::onFolderDeleted()
{
    Q_D(WLibraryFolder);

    emit wControllerPlaylist->filesCleared(d->idFull);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ bool WLibraryFolder::hasFolder() const
{
    return true;
}

//-------------------------------------------------------------------------------------------------
// Protected WLibraryItem reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ bool WLibraryFolder::applySource(const QString & source)
{
    return wControllerPlaylist->d_func()->applySourceFolder(this, source);
}

/* virtual */ bool WLibraryFolder::applyQuery(const WBackendNetQuery & query)
{
    return wControllerPlaylist->d_func()->applyQueryFolder(this, query);
}

/* virtual */ bool WLibraryFolder::stopQuery()
{
    return wControllerPlaylist->d_func()->abortQueriesFolder(this);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WLibraryFolder::onApplyCurrentIds(const QList<int> & ids)
{
    Q_D(WLibraryFolder);

    QList<int> folderIds = ids;

    setCurrentId(folderIds.takeFirst());

    emit currentIdUpdated();

    if (d->itemNext)
    {
        d->itemNext->setCurrentIds(folderIds);
    }
    else if (d->currentItem)
    {
        d->currentItem->setCurrentIds(folderIds);
    }
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

int WLibraryFolder::currentId() const
{
    Q_D(const WLibraryFolder); return d->currentId;
}

void WLibraryFolder::setCurrentId(int id)
{
    loadCurrentId(id);
}

//-------------------------------------------------------------------------------------------------

int WLibraryFolder::activeId() const
{
    Q_D(const WLibraryFolder); return d->activeId;
}

void WLibraryFolder::setActiveId(int id)
{
    Q_D(WLibraryFolder);

    if (d->activeId == id) return;

    if (d->parentFolder)
    {
        if (id == -1)
        {
             d->parentFolder->setActiveId(-1);
        }
        else d->parentFolder->setActiveId(d->id);
    }

    d->activeId = id;

    d->activeIndex = indexFromId(id);

    emit activeIdChanged   ();
    emit activeIndexChanged();
}

//-------------------------------------------------------------------------------------------------

int WLibraryFolder::currentIndex() const
{
    Q_D(const WLibraryFolder); return d->currentIndex;
}

void WLibraryFolder::setCurrentIndex(int index)
{
    if (index < 0 || index >= count()) return;

    int id = idAt(index);

    setCurrentId(id);
}

//-------------------------------------------------------------------------------------------------

int WLibraryFolder::activeIndex() const
{
    Q_D(const WLibraryFolder); return d->activeIndex;
}

void WLibraryFolder::setActiveIndex(int index)
{
    int id = idAt(index);

    setActiveId(id);
}

//-------------------------------------------------------------------------------------------------

WLibraryItem * WLibraryFolder::currentItem() const
{
    Q_D(const WLibraryFolder); return d->currentItem;
}

//-------------------------------------------------------------------------------------------------

qreal WLibraryFolder::scrollValue() const
{
    Q_D(const WLibraryFolder); return d->scrollValue;
}

void WLibraryFolder::setScrollValue(qreal value)
{
    Q_D(WLibraryFolder);

    if (d->scrollValue == value) return;

    d->scrollValue = value;

    emit scrollValueChanged();

    save();
}

//-------------------------------------------------------------------------------------------------

int WLibraryFolder::count() const
{
    Q_D(const WLibraryFolder); return d->items.count();
}

//-------------------------------------------------------------------------------------------------

int WLibraryFolder::maxCount() const
{
    Q_D(const WLibraryFolder); return d->maxCount;
}

void WLibraryFolder::setMaxCount(int max)
{
    Q_D(WLibraryFolder);

    if (d->maxCount == max) return;

    d->maxCount = max;

    emit maxCountChanged();
}

//-------------------------------------------------------------------------------------------------

bool WLibraryFolder::isEmpty() const
{
    Q_D(const WLibraryFolder);

    return d->items.isEmpty();
}

bool WLibraryFolder::isFull() const
{
    Q_D(const WLibraryFolder);

    return (d->items.count() >= d->maxCount);
}

//=================================================================================================
// WLibraryFolderSearch
//=================================================================================================

/* explicit */ WLibraryFolderSearch::WLibraryFolderSearch(WLibraryFolder * parent)
    : WLibraryFolder(new WLibraryFolderPrivate(this), FolderSearch, parent) {}

//=================================================================================================
// WLibraryFolderSearchable
//=================================================================================================

/* explicit */ WLibraryFolderSearchable::WLibraryFolderSearchable(WLibraryFolder * parent)
    : WLibraryFolder(new WLibraryFolderPrivate(this), FolderSearchable, parent) {}

#endif // SK_NO_LIBRARYFOLDER

#include "WLibraryFolder.moc"
