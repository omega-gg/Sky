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

#include "WLibraryItem.h"

#ifndef SK_NO_LIBRARYITEM

// Sk includes
#include <WControllerPlaylist>
#include <WTabTrack>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include "WLibraryItem_p.h"

WLibraryItemPrivate::WLibraryItemPrivate(WLibraryItem * p) : WLocalObjectPrivate(p) {}

/* virtual */ WLibraryItemPrivate::~WLibraryItemPrivate()
{
    Q_Q(WLibraryItem);

    if (parentFolder)
    {
        parentFolder->d_func()->unregisterItemId(q);
    }

    W_GET_CONTROLLER(WControllerPlaylist, controller);

    if (controller) controller->d_func()->abortQueriesItem(q);
}

//-------------------------------------------------------------------------------------------------

void WLibraryItemPrivate::init(WLibraryItem::Type type)
{
    this->type = type;

    parentFolder = NULL;

    stateQuery = WLocalObject::Loaded;
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WLibraryItemPrivate::applyIds(const QList<int> & ids)
{
    Q_Q(WLibraryItem);

    if (q->isLoading())
    {
        futureIds = ids;
    }
    else q->onApplyCurrentIds(ids);
}

//-------------------------------------------------------------------------------------------------

void WLibraryItemPrivate::setStateQuery(WLocalObject::State state)
{
    if (stateQuery == state) return;

    Q_Q(WLibraryItem);

    stateQuery = state;

    if (parentFolder)
    {
        parentFolder->d_func()->updateItemStateQuery(id, state);
    }

    emit q->stateQueryChanged();
}

//-------------------------------------------------------------------------------------------------

void WLibraryItemPrivate::setQueryLoading(bool loading)
{
    if (loading)
    {
        setStateQuery(WLocalObject::Loading);
        setState     (WLocalObject::Loading);
    }
    else if (stateQuery == WLocalObject::Loading)
    {
        setQueryDefault();
    }
}

void WLibraryItemPrivate::setQueryDefault()
{
    Q_Q(WLibraryItem);

    emit q->queryEnded();

    setStateQuery(WLocalObject::Default);
    setState     (WLocalObject::Loaded);

    emit q->queryCompleted();
}

void WLibraryItemPrivate::setQueryLoaded()
{
    Q_Q(WLibraryItem);

    setStateQuery(WLocalObject::Loaded);
    setState     (WLocalObject::Loaded);

    emit q->queryCompleted();
}

void WLibraryItemPrivate::setQueryEnded()
{
    Q_Q(WLibraryItem);

    emit q->queryEnded();

    setQueryLoaded();
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

/* explicit */ WLibraryItem::WLibraryItem(WLibraryFolder * parent)
    : WLocalObject(new WLibraryItemPrivate(this))
{
    Q_D(WLibraryItem);

    d->init(Item);

    setParentFolder(parent);
}

//-------------------------------------------------------------------------------------------------
// Protected

WLibraryItem::WLibraryItem(WLibraryItemPrivate * p, Type type, WLibraryFolder * parent)
    : WLocalObject(p)
{
    Q_D(WLibraryItem);

    d->init(type);

    setParentFolder(parent);
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ WLibraryFolder * WLibraryItem::toFolder()
{
    return qobject_cast<WLibraryFolder *> (this);
}

/* Q_INVOKABLE */ WPlaylist * WLibraryItem::toPlaylist()
{
    return qobject_cast<WPlaylist *> (this);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ bool WLibraryItem::loadSource(const QString & source, bool load)
{
    Q_D(WLibraryItem);

    if (d->source == source) return false;

    if (load && applySource(source) == false)
    {
        qWarning("WLibraryItem::loadSource: Failed to apply source %s.", source.C_STR);

        return false;
    }

    d->source = source;

    if (d->parentFolder)
    {
        d->parentFolder->d_func()->updateItemSource(d->id, source);
    }

    emit sourceChanged();

    save();

    return true;
}

/* Q_INVOKABLE */ bool WLibraryItem::reloadSource(const QString & source)
{
    if (applySource(source) == false)
    {
        qWarning("WLibraryItem::reloadSource: Failed to apply source %s.", source.C_STR);

        return false;
    }

    Q_D(WLibraryItem);

    if (d->source == source) return true;

    d->source = source;

    if (d->parentFolder)
    {
        d->parentFolder->d_func()->updateItemSource(d->id, source);
    }

    emit sourceChanged();

    save();

    return true;
}

/* Q_INVOKABLE */ void WLibraryItem::clearSource()
{
    loadSource(QString(), false);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ bool WLibraryItem::loadQuery()
{
    Q_D(WLibraryItem);

    if (d->stateQuery == Default && d->source.isEmpty() == false)
    {
        if (applySource(d->source) == false)
        {
            qWarning("WLibraryItem::loadQuery: Failed to load source %s.", d->source.C_STR);

            d->setStateQuery(WLocalObject::Loaded);

            return false;
        }
        else return true;
    }
    else return false;
}

/* Q_INVOKABLE */ bool WLibraryItem::reloadQuery()
{
    Q_D(WLibraryItem);

    if (d->source.isEmpty()) return false;

    if (applySource(d->source) == false)
    {
        qWarning("WLibraryItem::reloadQuery: Failed to reload source %s.", d->source.C_STR);

        d->setStateQuery(WLocalObject::Loaded);

        return false;
    }
    else return true;
}

/* Q_INVOKABLE */ bool WLibraryItem::abortQuery()
{
    Q_D(WLibraryItem);

    if (d->stateQuery == Loading)
    {
         return stopQuery();
    }
    else return false;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WLibraryItem::setFutureId(int id)
{
    Q_D(WLibraryItem);

    d->applyIds(QList<int>() << id);
}

/* Q_INVOKABLE */ void WLibraryItem::setCurrentIds(const QList<int> & ids)
{
    if (ids.isEmpty()) return;

    Q_D(WLibraryItem);

    d->applyIds(ids);
}

/* Q_INVOKABLE */ void WLibraryItem::setCurrentTabIds(WTabTrack * tab)
{
    Q_ASSERT(tab);

    setCurrentIds(tab->folderIds());
}

//-------------------------------------------------------------------------------------------------
// Static functions
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ bool WLibraryItem::typeIsFolder(Type type)
{
    return (type >= Folder && type <= FolderRelated);
}

/* Q_INVOKABLE static */ bool WLibraryItem::typeIsPlaylist(Type type)
{
    return (type >= Playlist && type <= PlaylistSearch);
}

/* Q_INVOKABLE static */ QString WLibraryItem::typeToString(Type type)
{
    if (typeIsFolder(type))   return "folder";
    if (typeIsPlaylist(type)) return "playlist";
    else                      return "item";
}

//-------------------------------------------------------------------------------------------------
// Virtual interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QString WLibraryItem::toVbml(bool) const
{
    qWarning("WLibraryItem::toVbml: Not supported.");

    return QString();
}

//-------------------------------------------------------------------------------------------------
// WLocalObject reimplementation
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QString WLibraryItem::getParentPath() const
{
    Q_D(const WLibraryItem);

    QString path;

    WLibraryFolder * folder = d->parentFolder;

    while (folder)
    {
        path.prepend("/" + QString::number(folder->id()));

        folder = folder->d_func()->parentFolder;
    }

    return wControllerPlaylist->pathStorage() + path;
}

//-------------------------------------------------------------------------------------------------
// Protected virtual functions
//-------------------------------------------------------------------------------------------------

/* virtual */ bool WLibraryItem::applySource(const QString & source)
{
    return wControllerPlaylist->d_func()->applySourceItem(this, source);
}

/* virtual */ bool WLibraryItem::applyQuery(const WBackendNetQuery & query)
{
    return wControllerPlaylist->d_func()->applyQueryItem(this, query);
}

/* virtual */ bool WLibraryItem::stopQuery()
{
    return wControllerPlaylist->d_func()->abortQueriesItem(this);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WLibraryItem::onApplyCurrentIds(const QList<int> &)
{
    qWarning("WLibraryItem::onApplyCurrentIds: Not supported.");
}

//-------------------------------------------------------------------------------------------------
// Protected WLocalObject reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WLibraryItem::applyId(int id)
{
    Q_D(WLibraryItem);

    d->id = id;

    d->idFull.clear();

    if (d->parentFolder)
    {
        WLibraryFolder * parent = d->parentFolder;

        while (parent)
        {
            d->idFull.prepend(parent->d_func()->id);

            parent = parent->d_func()->parentFolder;
        }

        d->idFull.append(id);

        d->parentFolder->d_func()->registerItemId(this);

        wControllerPlaylist->d_func()->registerItemId(this);
    }
    else d->idFull.append(id);

    emit idChanged();

    emit idFullChanged();
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WLibraryItem::setLoaded(bool ok)
{
    if (ok)
    {
        Q_D(WLibraryItem);

        if (d->futureIds.count())
        {
            onApplyCurrentIds(d->futureIds);

            d->futureIds.clear();
        }
    }

    WLocalObject::setLoaded(ok);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WLibraryItem::onStateChanged(WLocalObject::State state)
{
    Q_D(WLibraryItem);

    if (d->parentFolder && d->actionLoad == NULL)
    {
        d->parentFolder->d_func()->updateItemState(d->id, state);
    }
}

/* virtual */ void WLibraryItem::onLockChanged(bool locked)
{
    Q_D(WLibraryItem);

    if (d->parentFolder)
    {
        d->parentFolder->d_func()->updateLock(this, locked);
    }
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WLibraryItem::onFileDeleted()
{
    Q_D(WLibraryItem);

    emit wControllerPlaylist->filesDeleted(d->idFull);
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

QList<int> WLibraryItem::idFull() const
{
    Q_D(const WLibraryItem); return d->idFull;
}

//-------------------------------------------------------------------------------------------------

WLibraryFolder * WLibraryItem::parentFolder() const
{
    Q_D(const WLibraryItem); return d->parentFolder;
}

void WLibraryItem::setParentFolder(WLibraryFolder * folder)
{
    Q_D(WLibraryItem);

    if (d->parentFolder != NULL)
    {
        qWarning("WLibraryItem::setParentFolder: Item already has a parent folder.");

        return;
    }

    d->parentFolder = folder;

    if (d->parentFolder)
    {
        setSaveEnabled(d->parentFolder->saveEnabled());
    }

    emit parentFolderChanged();
}

//-------------------------------------------------------------------------------------------------

WLibraryItem::Type WLibraryItem::type() const
{
    Q_D(const WLibraryItem); return d->type;
}

void WLibraryItem::setType(Type type)
{
    Q_D(WLibraryItem);

    if (d->type == type) return;

    d->type = type;

    if (d->parentFolder)
    {
        d->parentFolder->d_func()->updateItemType(d->id, d->type);
    }

    emit typeChanged();

    save();
}

//-------------------------------------------------------------------------------------------------

bool WLibraryItem::isFolder() const
{
    Q_D(const WLibraryItem); return (d->type <= FolderRelated);
}

bool WLibraryItem::isPlaylist() const
{
    Q_D(const WLibraryItem);

    return (d->type >= Playlist && d->type <= PlaylistSearch);
}

//-------------------------------------------------------------------------------------------------

bool WLibraryItem::isFolderBase() const
{
    Q_D(const WLibraryItem); return (d->type == Folder);
}

bool WLibraryItem::isFolderSearch() const
{
    Q_D(const WLibraryItem); return (d->type == FolderSearch);
}

bool WLibraryItem::isFolderSearchable() const
{
    Q_D(const WLibraryItem); return (d->type == FolderSearchable);
}

bool WLibraryItem::isFolderRelated() const
{
    Q_D(const WLibraryItem); return (d->type == FolderRelated);
}

//-------------------------------------------------------------------------------------------------

bool WLibraryItem::isPlaylistBase() const
{
    Q_D(const WLibraryItem); return (d->type == Playlist);
}

bool WLibraryItem::isPlaylistFeed() const
{
    Q_D(const WLibraryItem); return (d->type == PlaylistFeed);
}

bool WLibraryItem::isPlaylistSearch() const
{
    Q_D(const WLibraryItem); return (d->type == PlaylistSearch);
}

//-------------------------------------------------------------------------------------------------

WLocalObject::State WLibraryItem::stateQuery() const
{
    Q_D(const WLibraryItem); return d->stateQuery;
}

//-------------------------------------------------------------------------------------------------

bool WLibraryItem::queryIsDefault() const
{
    Q_D(const WLibraryItem); return (d->stateQuery == Default);
}

bool WLibraryItem::queryIsLoading() const
{
    Q_D(const WLibraryItem); return (d->stateQuery == Loading);
}

bool WLibraryItem::queryIsLoaded() const
{
    Q_D(const WLibraryItem); return (d->stateQuery == Loaded);
}

//-------------------------------------------------------------------------------------------------

QString WLibraryItem::source() const
{
    Q_D(const WLibraryItem); return d->source;
}

void WLibraryItem::setSource(const QString & source)
{
    loadSource(source);
}

//-------------------------------------------------------------------------------------------------

bool WLibraryItem::isLocal() const
{
    Q_D(const WLibraryItem); return d->source.isEmpty();
}

bool WLibraryItem::isOnline() const
{
    Q_D(const WLibraryItem); return (d->source.isEmpty() == false);
}

//-------------------------------------------------------------------------------------------------

QString WLibraryItem::title() const
{
    Q_D(const WLibraryItem); return d->title;
}

void WLibraryItem::setTitle(const QString & title)
{
    Q_D(WLibraryItem);

    if (d->title == title) return;

    d->title = title.simplified();

    if (d->parentFolder)
    {
        d->parentFolder->d_func()->updateItemTitle(d->id, d->title);
    }

    emit titleChanged();

    save();
}

//-------------------------------------------------------------------------------------------------

QString WLibraryItem::cover() const
{
    Q_D(const WLibraryItem); return d->cover;
}

void WLibraryItem::setCover(const QString & cover)
{
    Q_D(WLibraryItem);

    if (d->cover == cover) return;

    d->cover = cover;

    if (d->parentFolder)
    {
        d->parentFolder->d_func()->updateItemCover(d->id, d->cover);
    }

    emit coverChanged();

    save();
}

//-------------------------------------------------------------------------------------------------

QString WLibraryItem::label() const
{
    Q_D(const WLibraryItem); return d->label;
}

void WLibraryItem::setLabel(const QString & label)
{
    Q_D(WLibraryItem);

    if (d->label == label) return;

    d->label = label;

    if (d->parentFolder)
    {
        d->parentFolder->d_func()->updateItemLabel(d->id, d->label);
    }

    emit labelChanged();

    save();
}

#endif // SK_NO_LIBRARYITEM
