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

#include "WDeclarativeListView.h"

#ifndef SK_NO_DECLARATIVELISTVIEW

// Qt includes
#include <qmath.h>
#include <QAbstractProxyModel>
#include <QDeclarativeContext>
#include <QDeclarativeItem>

//-------------------------------------------------------------------------------------------------
// Static variables

static const qreal DECLARATIVELISTVIEW_ITEM_SIZE = 33;

//-------------------------------------------------------------------------------------------------
// Defines

#define W_INSERT_VERTICAL                     \
{                                             \
    items.insert(index, item);                \
                                              \
    QDeclarativeItem * object = item->object; \
                                              \
    object->setPos(QPointF(0, pos));          \
                                              \
    qreal height = object->height();          \
                                              \
    extra += itemSize - height;               \
                                              \
    pos += height;                            \
}                                             \

#define W_INSERT_HORIZONTAL                   \
{                                             \
    items.insert(index, item);                \
                                              \
    QDeclarativeItem * object = item->object; \
                                              \
    object->setPos(QPointF(pos, 0));          \
                                              \
    qreal width = object->width();            \
                                              \
    extra += itemSize - width;                \
                                              \
    pos += width;                             \
}                                             \

//-------------------------------------------------------------------------------------------------

#define W_RESET_VERTICAL                             \
                                                     \
    WDeclarativeListItem * item = createItem(index); \
                                                     \
    if (item) W_INSERT_VERTICAL;                     \
                                                     \
    index++;                                         \

#define W_RESET_HORIZONTAL                           \
                                                     \
    WDeclarativeListItem * item = createItem(index); \
                                                     \
    if (item) W_INSERT_HORIZONTAL;                   \
                                                     \
    index++;                                         \

//-------------------------------------------------------------------------------------------------

#define W_UPDATE_VERTICAL                                  \
                                                           \
    WDeclarativeListItem * item = this->items.take(index); \
                                                           \
    if (item == NULL)                                      \
    {                                                      \
        item = createItem(index);                          \
                                                           \
        if (item) W_INSERT_VERTICAL;                       \
    }                                                      \
    else W_INSERT_VERTICAL;                                \
                                                           \
    index++;                                               \

#define W_UPDATE_HORIZONTAL                                \
                                                           \
    WDeclarativeListItem * item = this->items.take(index); \
                                                           \
    if (item == NULL)                                      \
    {                                                      \
        item = createItem(index);                          \
                                                           \
        if (item) W_INSERT_HORIZONTAL;                     \
    }                                                      \
    else W_INSERT_HORIZONTAL;                              \
                                                           \
    index++;                                               \

//=================================================================================================
// WDeclarativeListItem
//=================================================================================================

WDeclarativeListItem::WDeclarativeListItem()
{
    object  = NULL;
    context = NULL;
}

/* virtual */ WDeclarativeListItem::~WDeclarativeListItem()
{
    object->deleteLater();
}

//=================================================================================================
// WDeclarativeListViewPrivate
//=================================================================================================

WDeclarativeListViewPrivate::WDeclarativeListViewPrivate(WDeclarativeListView * p)
    : WDeclarativeItemPrivate(p) {}

/* virtual */ WDeclarativeListViewPrivate::~WDeclarativeListViewPrivate()
{
    clearItems();
}

//-------------------------------------------------------------------------------------------------

void WDeclarativeListViewPrivate::init(Qt::Orientation orientation)
{
    Q_Q(WDeclarativeListView);

    this->orientation = orientation;

    model    = NULL;
    delegate = NULL;

    count = 0;

    itemSize = DECLARATIVELISTVIEW_ITEM_SIZE;

    size = 0;

    start =  0;
    end   = -1;

    clearDelayed = true;
    clearDelay   = 1000;

    timer.setInterval(clearDelay);

    timer.setSingleShot(true);

    QObject::connect(&timer, SIGNAL(timeout()), q, SLOT(onClearItemsOld()));
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WDeclarativeListViewPrivate::reset()
{
    int count = model->rowCount();

    int index = start / itemSize;

    qreal pos   = index * itemSize;
    qreal extra = 0;

    if (orientation == Qt::Vertical)
    {
        if (end == -1)
        {
            while (index < count)
            {
                W_RESET_VERTICAL;
            }
        }
        else
        {
            while (index < count && pos < end)
            {
                W_RESET_VERTICAL;
            }
        }
    }
    else
    {
        if (end == -1)
        {
            while (index < count)
            {
                W_RESET_HORIZONTAL;
            }
        }
        else
        {
            while (index < count && pos < end)
            {
                W_RESET_HORIZONTAL;
            }
        }
    }

    if (this->count != count)
    {
        Q_Q(WDeclarativeListView);

        this->count = count;

        size = count * itemSize;

        setSize(size - extra);

        emit q->countChanged();
    }
    else setSize(size - extra);
}

//-------------------------------------------------------------------------------------------------

qreal WDeclarativeListViewPrivate::updatePosition()
{
    if (model == NULL || delegate == NULL)
    {
        return 0;
    }

    QHash<int, WDeclarativeListItem *> items;

    int index = start / itemSize;

    qreal pos   = index * itemSize;
    qreal extra = 0;

    if (orientation == Qt::Vertical)
    {
        if (end == -1)
        {
            while (index < count)
            {
                W_UPDATE_VERTICAL;
            }
        }
        else
        {
            while (index < count && pos < end)
            {
                W_UPDATE_VERTICAL;
            }
        }
    }
    else if (end == -1)
    {
        while (index < count)
        {
            W_UPDATE_HORIZONTAL;
        }
    }
    else
    {
        while (index < count && pos < end)
        {
            W_UPDATE_HORIZONTAL;
        }
    }

    QHashIterator<int, WDeclarativeListItem *> i(this->items);

    while (i.hasNext())
    {
        i.next();

        WDeclarativeListItem * item = i.value();

        item->object->setVisible(false);

        itemsOld.append(item);
    }

    this->items = items;

    if (clearDelayed)
    {
        timer.start();
    }
    else onClearItemsOld();

    return extra;
}

void WDeclarativeListViewPrivate::applyPosition()
{
    size = count * itemSize;

    qreal extra = updatePosition();

    setSize(size - extra);
}

//-------------------------------------------------------------------------------------------------

WDeclarativeListItem * WDeclarativeListViewPrivate::createItem(int index)
{
    Q_Q(WDeclarativeListView);

    if (itemsOld.isEmpty())
    {
        QDeclarativeContext * context = new QDeclarativeContext(qmlContext(q));

        QDeclarativeItem * object
                         = static_cast<QDeclarativeItem *> (delegate->beginCreate(context));

        context->setContextObject(q);

        if (object == NULL)
        {
            delegate->completeCreate();

            delete context;

            return NULL;
        }

        object->setParent    (q);
        object->setParentItem(q);

        object->setZValue(-1);

        context->setParent(object);

        context->setContextProperty("list",  q);
        context->setContextProperty("model", model);
        context->setContextProperty("index", index);

        QModelIndex	modelIndex = model->index(index, 0);

        const QHash<int, QByteArray> & names = model->roleNames();

        QHashIterator<int, QByteArray> i(names);

        while (i.hasNext())
        {
            i.next();

            QVariant data = model->data(modelIndex, i.key());

            context->setContextProperty(i.value(), data);
        }

        delegate->completeCreate();

        WDeclarativeListItem * item = new WDeclarativeListItem;

        item->object  = object;
        item->context = context;

        if (orientation == Qt::Vertical)
        {
             QObject::connect(object, SIGNAL(heightChanged()), q, SLOT(onSizeChanged()));
        }
        else QObject::connect(object, SIGNAL(widthChanged()), q, SLOT(onSizeChanged()));

        return item;
    }
    else
    {
        WDeclarativeListItem * item = itemsOld.takeFirst();

        QDeclarativeContext * context = item->context;

        context->setContextProperty("index", index);

        QModelIndex modelIndex = model->index(index, 0);

        const QHash<int, QByteArray> & names = model->roleNames();

        QHashIterator<int, QByteArray> j(names);

        while (j.hasNext())
        {
            j.next();

            QVariant data = model->data(modelIndex, j.key());

            context->setContextProperty(j.value(), data);
        }

        item->object->setVisible(true);

        return item;
    }
}

void WDeclarativeListViewPrivate::updateItem(int index, WDeclarativeListItem * item)
{
    QDeclarativeContext * context = item->context;

    QModelIndex modelIndex = model->index(index, 0);

    const QHash<int, QByteArray> & names = model->roleNames();

    QHashIterator<int, QByteArray> i(names);

    while (i.hasNext())
    {
        i.next();

        QVariant data = model->data(modelIndex, i.key());

        context->setContextProperty(i.value(), data);
    }
}

//-------------------------------------------------------------------------------------------------

void WDeclarativeListViewPrivate::clearItems()
{
    QHashIterator<int, WDeclarativeListItem *> i(items);

    while (i.hasNext())
    {
        i.next();

        WDeclarativeListItem * item = i.value();

        item->object->setVisible(false);

        delete item;
    }

    items.clear();

    if (timer.isActive())
    {
        timer.stop();

        onClearItemsOld();
    }
}

//-------------------------------------------------------------------------------------------------

void WDeclarativeListViewPrivate::setSize(qreal size)
{
    Q_Q(WDeclarativeListView);

    if (orientation == Qt::Vertical)
    {
         q->setHeight(size);
    }
    else q->setWidth(size);
}

//-------------------------------------------------------------------------------------------------

int WDeclarativeListViewPrivate::getLast(int first, int count)
{
    if (end != -1)
    {
        first += (end - start) / itemSize + 2;

        if (first > count)
        {
            first = count;
        }

        return first;
    }
    else return count;
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WDeclarativeListViewPrivate::onReset()
{
    clearItems();

    if (delegate) reset();
}

//-------------------------------------------------------------------------------------------------

void WDeclarativeListViewPrivate::onInsert(const QModelIndex &, int start, int end)
{
    Q_Q(WDeclarativeListView);

    QHash<int, WDeclarativeListItem *> items;

    int size = end - start + 1;

    QMutableHashIterator<int, WDeclarativeListItem *> i(this->items);

    while (i.hasNext())
    {
        i.next();

        int index = i.key();

        if (index >= start)
        {
            items.insert(index + size, i.value());

            i.remove();
        }
    }

    QHashIterator<int, WDeclarativeListItem *> j(items);

    while (j.hasNext())
    {
        j.next();

        int index = j.key();

        WDeclarativeListItem * item = j.value();

        this->items.insert(index, item);

        item->context->setContextProperty("index", index);
    }

    count = model->rowCount();

    applyPosition();

    emit q->countChanged();
}

void WDeclarativeListViewPrivate::onRemove(const QModelIndex &, int start, int end)
{
    Q_Q(WDeclarativeListView);

    QHash<int, WDeclarativeListItem *> items;

    int size = end - start + 1;

    QMutableHashIterator<int, WDeclarativeListItem *> i(this->items);

    while (i.hasNext())
    {
        i.next();

        int index = i.key();

        WDeclarativeListItem * item = i.value();

        if (index >= start && index <= end)
        {
            delete item;

            i.remove();
        }
        else if (index > end)
        {
            items.insert(index - size, item);

            i.remove();
        }
    }

    QHashIterator<int, WDeclarativeListItem *> j(items);

    while (j.hasNext())
    {
        j.next();

        int index = j.key();

        WDeclarativeListItem * item = j.value();

        this->items.insert(index, item);

        item->context->setContextProperty("index", index);
    }

    count = model->rowCount();

    applyPosition();

    emit q->countChanged();
}

//-------------------------------------------------------------------------------------------------

void WDeclarativeListViewPrivate::onMove(const QModelIndex &,
                                         int                 sourceStart,
                                         int                 sourceEnd,
                                         const QModelIndex &,
                                         int                 destinationRow)
{
    QHash<int, WDeclarativeListItem *> items;

    int size = sourceEnd - sourceStart + 1;

    QMutableHashIterator<int, WDeclarativeListItem *> i(this->items);

    while (i.hasNext())
    {
        i.next();

        int index = i.key();

        if (index >= sourceStart && index <= sourceEnd)
        {
            int to = destinationRow + (index - sourceStart);

            if (index < to)
            {
                 items.insert(to - 1, i.value());
            }
            else items.insert(to, i.value());

            i.remove();
        }
        else if (index >= destinationRow && index < sourceStart)
        {
            items.insert(index + size, i.value());

            i.remove();
        }
        else if (index < destinationRow && index > sourceEnd)
        {
            items.insert(index - size, i.value());

            i.remove();
        }
    }

    QHashIterator<int, WDeclarativeListItem *> j(items);

    while (j.hasNext())
    {
        j.next();

        int index = j.key();

        WDeclarativeListItem * item = j.value();

        this->items.insert(index, item);

        item->context->setContextProperty("index", index);
    }

    updatePosition();
}

//-------------------------------------------------------------------------------------------------

void WDeclarativeListViewPrivate::onDataChanged(const QModelIndex & topLeft,
                                                const QModelIndex & bottomRight)
{
    int begin = topLeft    .row();
    int end   = bottomRight.row();

    for (int i = begin; i <= end; i++)
    {
        WDeclarativeListItem * item = items.value(i);

        if (item) updateItem(i, item);
    }
}

//-------------------------------------------------------------------------------------------------

void WDeclarativeListViewPrivate::onLayoutChanged()
{
    QHashIterator<int, WDeclarativeListItem *> i(items);

    while (i.hasNext())
    {
        i.next();

        updateItem(i.key(), i.value());
    }
}

//-------------------------------------------------------------------------------------------------

void WDeclarativeListViewPrivate::onSizeChanged()
{
    qreal extra = updatePosition();

    setSize(size - extra);
}

//-------------------------------------------------------------------------------------------------

void WDeclarativeListViewPrivate::onClearItemsOld()
{
    foreach (WDeclarativeListItem * item, itemsOld)
    {
        delete item;
    }

    itemsOld.clear();
}

//=================================================================================================
// WDeclarativeListView
//=================================================================================================
// Protected

WDeclarativeListView::WDeclarativeListView(Qt::Orientation orientation, QDeclarativeItem * parent)
    : WDeclarativeItem(new WDeclarativeListViewPrivate(this), parent)
{
    Q_D(WDeclarativeListView); d->init(orientation);
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WDeclarativeListView::setRange(qreal start, qreal end)
{
    Q_D(WDeclarativeListView);

    if ((d->start == start && d->end == end)
        ||
        (start < 0 || start > d->size || end < -1 || end > d->size)) return;

    qreal oldStart = d->start;
    qreal oldEnd   = d->end;

    d->start = start;
    d->end   = end;

    d->updatePosition();

    if (oldStart != start)
    {
        emit startChanged();
    }

    if (oldEnd != end)
    {
        emit endChanged();
    }
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ QDeclarativeItem * WDeclarativeListView::itemAt(int index) const
{
    Q_D(const WDeclarativeListView);

    WDeclarativeListItem * item = d->items.value(index);

    if (item) return item->object;
    else      return NULL;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ qreal WDeclarativeListView::itemY(int index) const
{
    Q_D(const WDeclarativeListView);

    if (index < 0 || index >= d->count) return -1;

    return index * d->itemSize;
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

QAbstractItemModel * WDeclarativeListView::model() const
{
    Q_D(const WDeclarativeListView); return d->model;
}

void WDeclarativeListView::setModel(QAbstractItemModel * model)
{
    Q_D(WDeclarativeListView);

    if (d->model == model) return;

    if (d->model)
    {
        d->clearItems();

        QObject::disconnect(d->model, 0, this, 0);
    }

    d->model = model;

    if (model)
    {
        QObject::connect(model, SIGNAL(modelReset()), this, SLOT(onReset()));

        QObject::connect(model, SIGNAL(rowsInserted(const QModelIndex &, int, int)),
                         this,  SLOT(onInsert      (const QModelIndex &, int, int)));

        QObject::connect(model, SIGNAL(rowsRemoved(const QModelIndex &, int, int)),
                         this,  SLOT(onRemove     (const QModelIndex &, int, int)));

        QObject::connect(model, SIGNAL(rowsMoved(const QModelIndex &, int, int,
                                                    const QModelIndex &, int)),
                         this, SLOT(onMove(const QModelIndex &, int, int,
                                           const QModelIndex &, int)));

        QObject::connect(model, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
                         this,  SLOT(onDataChanged(const QModelIndex &, const QModelIndex &)));

        if (qobject_cast<QAbstractProxyModel *> (model))
        {
            QObject::connect(model, SIGNAL(layoutChanged()), this, SLOT(onLayoutChanged()));
        }

        d->reset();
    }

    emit modelChanged();
}

//-------------------------------------------------------------------------------------------------

QDeclarativeComponent * WDeclarativeListView::delegate() const
{
    Q_D(const WDeclarativeListView); return d->delegate;
}

void WDeclarativeListView::setDelegate(QDeclarativeComponent * delegate)
{
    Q_D(WDeclarativeListView);

    if (d->delegate == delegate) return;

    if (d->model)
    {
        d->clearItems();

        d->delegate = delegate;

        if (delegate) d->reset();
    }
    else d->delegate = delegate;

    emit delegateChanged();
}

//-------------------------------------------------------------------------------------------------

int WDeclarativeListView::count() const
{
    Q_D(const WDeclarativeListView); return d->count;
}

//-------------------------------------------------------------------------------------------------

qreal WDeclarativeListView::itemSize() const
{
    Q_D(const WDeclarativeListView); return d->itemSize;
}

void WDeclarativeListView::setItemSize(qreal size)
{
    Q_D(WDeclarativeListView);

    if (d->itemSize == size) return;

    d->itemSize = size;

    d->applyPosition();

    emit itemSizeChanged();
}

//-------------------------------------------------------------------------------------------------

qreal WDeclarativeListView::start() const
{
    Q_D(const WDeclarativeListView); return d->start;
}

void WDeclarativeListView::setStart(qreal start)
{
    Q_D(WDeclarativeListView);

    if (d->start == start || start < 0 || start > d->size) return;

    d->start = start;

    d->updatePosition();

    emit startChanged();
}

void WDeclarativeListView::resetStart()
{
    Q_D(WDeclarativeListView);

    if (d->start == 0) return;

    d->start = 0;

    d->updatePosition();

    emit startChanged();
}

//-------------------------------------------------------------------------------------------------

qreal WDeclarativeListView::end() const
{
    Q_D(const WDeclarativeListView); return d->end;
}

void WDeclarativeListView::setEnd(qreal end)
{
    Q_D(WDeclarativeListView);

    if (d->end == end || end < -1 || end > d->size) return;

    d->end = end;

    d->updatePosition();

    emit endChanged();
}

void WDeclarativeListView::resetEnd()
{
    Q_D(WDeclarativeListView);

    if (d->end == -1) return;

    d->end = -1;

    d->updatePosition();

    emit endChanged();
}

//-------------------------------------------------------------------------------------------------

bool WDeclarativeListView::clearDelayed() const
{
    Q_D(const WDeclarativeListView); return d->clearDelayed;
}

void WDeclarativeListView::setClearDelayed(bool delayed)
{
    Q_D(WDeclarativeListView);

    if (d->clearDelayed == delayed) return;

    if (d->timer.isActive())
    {
        d->timer.stop();

        d->onClearItemsOld();
    }

    d->clearDelayed = delayed;

    emit clearDelayedChanged();
}

//-------------------------------------------------------------------------------------------------

int WDeclarativeListView::clearDelay() const
{
    Q_D(const WDeclarativeListView); return d->clearDelay;
}

void WDeclarativeListView::setClearDelay(int delay)
{
    Q_D(WDeclarativeListView);

    if (d->clearDelay == delay) return;

    d->clearDelay = delay;

    d->timer.setInterval(delay);

    emit clearDelayChanged();
}

//=================================================================================================
// WDeclarativeListHorizontal
//=================================================================================================

/* explicit */ WDeclarativeListHorizontal::WDeclarativeListHorizontal(QDeclarativeItem * parent)
    : WDeclarativeListView(Qt::Horizontal, parent) {}

//=================================================================================================
// WDeclarativeListVertical
//=================================================================================================

/* explicit */ WDeclarativeListVertical::WDeclarativeListVertical(QDeclarativeItem * parent)
    : WDeclarativeListView(Qt::Vertical, parent) {}

#endif // SK_NO_DECLARATIVELISTVIEW
