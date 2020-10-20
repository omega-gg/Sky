//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkCore.

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

#include "WLocalObject.h"

#ifndef SK_NO_LOCALOBJECT

// Qt includes
#include <QDir>

// Sk includes
#include <WControllerFile>
#include <WAbstractThreadAction>

//=================================================================================================
// WLocalObjectPrivate
//=================================================================================================

WLocalObjectPrivate::WLocalObjectPrivate(WLocalObject * p) : WPrivate(p) {}

/* virtual */ WLocalObjectPrivate::~WLocalObjectPrivate()
{
    if (actionSave)
    {
        qWarning("WLocalObjectPrivate::~WLocalObjectPrivate: Object is still saving.");
    }

    if (actionLoad)
    {
        qWarning("WLocalObjectPrivate::~WLocalObjectPrivate: Object is still loading.");
    }

    W_GET_CONTROLLER(WControllerFile, controller);

    if (controller)
    {
        Q_Q(WLocalObject);

        controller->d_func()->unregisterLocalObject(q);
    }
}

//-------------------------------------------------------------------------------------------------

void WLocalObjectPrivate::init()
{
    Q_Q(WLocalObject);

    id = -1;

    pathCreated   = false;
    folderCreated = false;

    processing = false;

    actionSave = NULL;
    actionLoad = NULL;

    saveEnabled = false;
    saving      = false;

    state = WLocalObject::Default;

    blocked   = false;
    lockCount = 0;

    cacheLoad = false;
    instant   = false;

    toDelete = false;

    locked = true;

    QObject::connect(&timer, SIGNAL(timeout()), q, SLOT(onSaveTimeout()));

    wControllerFile->d_func()->registerLocalObject(q);
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WLocalObjectPrivate::updateProcessing()
{
    bool processing;

    if (saving || (state == WLocalObject::Loading))
    {
         processing = true;
    }
    else processing = false;

    if (this->processing == processing) return;

    Q_Q(WLocalObject);

    this->processing = processing;

    emit q->processingChanged();

    updateLock();

    if (toDelete && locked == false)
    {
        q->deleteLater();
    }
}

//-------------------------------------------------------------------------------------------------

void WLocalObjectPrivate::setSaving(bool saving)
{
    Q_Q(WLocalObject);

    this->saving = saving;

    emit q->savingChanged();

    updateProcessing();
}

//-------------------------------------------------------------------------------------------------

void WLocalObjectPrivate::setState(WLocalObject::State state)
{
    Q_Q(WLocalObject);

    this->state = state;

    q->onStateChanged(state);

    emit q->stateChanged();

    updateProcessing();
}

//-------------------------------------------------------------------------------------------------

void WLocalObjectPrivate::setSaved(bool ok)
{
    Q_Q(WLocalObject); q->setSaved(ok);
}

void WLocalObjectPrivate::setLoaded(bool ok)
{
    Q_Q(WLocalObject); q->setLoaded(ok);
}

//-------------------------------------------------------------------------------------------------

bool WLocalObjectPrivate::startSave(bool instant)
{
    Q_Q(WLocalObject);

    actionSave = q->onSave(q->getFilePath());

    if (actionSave == NULL)
    {
        qWarning("WLocalObjectPrivate::startSave: Failed to start save.");

        return false;
    }

    q->createPath();

    createFolder();

    if (instant)
    {
        actionSave->start();
    }
    else wControllerFile->startWriteAction(actionSave);

    /*if (instant == false)
    {
        if (wControllerFile->startWriteAction(actionSave) == NULL)
        {
            qWarning("WLocalObjectPrivate::startSave: Failed to start save.");

            actionSave->abortAndDelete();

            actionSave = NULL;

            return false;
        }
    }
    else actionSave->start();*/

    return true;
}

//-------------------------------------------------------------------------------------------------

void WLocalObjectPrivate::createFolder()
{
    Q_Q(WLocalObject);

    if (q->hasFolder() == false || folderCreated) return;

    QString path = q->getFolderPath();

    if (path.endsWith("/-1"))
    {
        qWarning("WLocalObjectPrivate::createFolder: Failed to create folder. Invalid path %s.",
                 path.C_STR);

        return;
    }

    if (QFile::exists(path))
    {
        folderCreated = true;

        return;
    }

    if (QDir().mkpath(path) == false)
    {
         qWarning("WLocalObjectPrivate::createFolder: Failed to create folder %s.", path.C_STR);
    }
    else folderCreated = true;
}

//-------------------------------------------------------------------------------------------------

void WLocalObjectPrivate::setLockCount(int count)
{
    if (lockCount == count) return;

    Q_Q(WLocalObject);

    lockCount = count;

    if (count) toDelete = false;

    emit q->lockCountChanged();

    updateLock();
}

//-------------------------------------------------------------------------------------------------

void WLocalObjectPrivate::updateLock()
{
    bool lock;

    if (processing || lockCount || blocked)
    {
         lock = true;
    }
    else lock = false;

    if (locked == lock) return;

    Q_Q(WLocalObject);

    locked = lock;

    q->onLockChanged(lock);

    emit q->lockedChanged();
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WLocalObjectPrivate::onSaveTimeout()
{
    if (state == WLocalObject::Loading || actionSave) return;

    timer.stop();

    if (startSave(false) == false)
    {
        setSaving(false);
    }
}

//=================================================================================================
// WLocalObject
//=================================================================================================

/* explicit */ WLocalObject::WLocalObject(QObject * parent)
    : QObject(parent), WPrivatable(new WLocalObjectPrivate(this))
{
    Q_D(WLocalObject); d->init();
}

//-------------------------------------------------------------------------------------------------
// Protected

WLocalObject::WLocalObject(WLocalObjectPrivate * p, QObject * parent)
    : QObject(parent), WPrivatable(p)
{
    Q_D(WLocalObject); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WLocalObject::createPath()
{
    Q_D(WLocalObject);

    if (d->pathCreated) return;

    QString path = getParentPath();

    if (QFile::exists(path))
    {
        d->pathCreated = true;

        return;
    }

    if (QDir().mkpath(path) == false)
    {
        qWarning("WLocalObject::createPath: Failed to create path %s.", path.C_STR);
    }
    else d->pathCreated = true;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ bool WLocalObject::saveNow()
{
    return save(false, 0);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WLocalObject::abortSave()
{
    Q_D(WLocalObject);

    if (d->saving == false) return;

    d->timer.stop();

    if (d->actionSave)
    {
        d->actionSave->abortAndDelete();

        d->actionSave = NULL;
    }

    d->setSaving(false);
}

/* Q_INVOKABLE */ void WLocalObject::abortLoad()
{
    Q_D(WLocalObject);

    if (d->state != Loading) return;

    if (d->actionLoad)
    {
        d->actionLoad->abortAndDelete();

        d->actionLoad = NULL;
    }

    d->cacheLoad = false;

    d->setState(Default);

    emit loaded();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WLocalObject::abortAll()
{
    abortSave();
    abortLoad();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WLocalObject::deleteFile(bool instant)
{
    Q_D(WLocalObject);

    QString path = getFilePath();

    if (QFile::exists(path) == false) return;

    if (instant)
    {
         WControllerFile::deleteFile(path);
    }
    else wControllerFile->startDeleteFile(path);

    d->pathCreated = false;

    onFileDeleted();
}

/* Q_INVOKABLE */ void WLocalObject::deleteFolder(bool instant)
{
    Q_D(WLocalObject);

    QString path = getFolderPath();

    if (QFile::exists(path) == false) return;

    if (instant)
    {
         WControllerFile::deleteFolder(path);
    }
    else wControllerFile->startDeleteFolder(path);

    d->folderCreated = false;

    onFolderDeleted();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WLocalObject::deleteFileAndFolder(bool instant)
{
    deleteFile  (instant);
    deleteFolder(instant);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WLocalObject::addDeleteLock()
{
    Q_D(WLocalObject); d->setLockCount(d->lockCount + 1);
}

/* Q_INVOKABLE */ void WLocalObject::clearDeleteLock()
{
    Q_D(WLocalObject); d->setLockCount(0);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ QString WLocalObject::pathFileFromId(int id) const
{
    return getParentPath() + "/" + QString::number(id) + ".xml";
}

/* Q_INVOKABLE */ QString WLocalObject::pathFolderFromId(int id) const
{
    return getParentPath() + "/" + QString::number(id);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ QString WLocalObject::pathChildFile(int id) const
{
    return getFolderPath() + "/" + QString::number(id) + ".xml";
}

/* Q_INVOKABLE */ QString WLocalObject::pathChildFolder(int id) const
{
    return getFolderPath() + "/" + QString::number(id);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WLocalObject::abortAndDelete()
{
    abortAll();

    deleteLater();
}

//-------------------------------------------------------------------------------------------------
// Virtual Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ bool WLocalObject::save(bool instant, int duration)
{
    Q_D(WLocalObject);

    if (d->saveEnabled == false) return false;

    QString path = getFilePath();

    if (path.endsWith("/-1.xml"))
    {
        qWarning("WLocalObject::save: Failed to save. Invalid path %s.", path.C_STR);

        return false;
    }

    if (instant)
    {
        if (d->actionSave) return false;

        if (d->saving == false)
        {
            d->setSaving(true);
        }

        d->timer.stop();

        d->instant = true;

        if (d->startSave(true) == false)
        {
            d->instant = false;

            d->setSaving(false);

            return false;
        }

        d->instant = false;
    }
    else
    {
        if (d->saving == false)
        {
            d->setSaving(true);
        }

        d->timer.start(duration);
    }

    return true;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ bool WLocalObject::load(bool instant)
{
    Q_D(WLocalObject);

    if (d->state != Default || d->saving) return false;

    QString path = getFilePath();

    if (path.endsWith("/-1.xml"))
    {
        qWarning("WLocalObject::load: Failed to load. Invalid path %s.", path.C_STR);

        return false;
    }

    if (QFile::exists(path) == false) return false;

    d->actionLoad = onLoad(path);

    if (d->actionLoad == NULL)
    {
        qWarning("WLocalObject::load: Failed to start load.");

        return false;
    }

    d->setState(Loading);

    if (instant)
    {
        d->instant = true;

        d->actionLoad->start();

        d->instant = false;
    }
    else wControllerFile->startReadAction(d->actionLoad);

    /*else if (wControllerFile->startReadAction(d->actionLoad) == NULL)
    {
        qWarning("WLocalObject::load: Failed to start load.");

        d->actionLoad->abortAndDelete();

        d->actionLoad = NULL;

        d->setState(Default);

        return false;
    }*/

    return true;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ bool WLocalObject::tryDelete()
{
    Q_D(WLocalObject);

    if (d->lockCount > 0)
    {
        d->setLockCount(d->lockCount - 1);

        return false;
    }

    if (d->locked)
    {
        d->toDelete = true;

        return true;
    }

    deleteLater();

    return true;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QString WLocalObject::getParentPath() const
{
    return wControllerFile->pathStorage();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QString WLocalObject::getFilePath() const
{
    Q_D(const WLocalObject); return pathFileFromId(d->id);
}

/* Q_INVOKABLE virtual */ QString WLocalObject::getFolderPath() const
{
    Q_D(const WLocalObject); return pathFolderFromId(d->id);
}

//-------------------------------------------------------------------------------------------------
// Protected functions
//-------------------------------------------------------------------------------------------------

void WLocalObject::setBlocked(bool block)
{
    Q_D(WLocalObject);

    if (d->blocked == block) return;

    d->blocked = block;

    d->updateLock();

    if (d->toDelete && d->locked == false)
    {
        deleteLater();
    }
}

//-------------------------------------------------------------------------------------------------

bool WLocalObject::isCacheLoad() const
{
    Q_D(const WLocalObject); return d->cacheLoad;
}

//-------------------------------------------------------------------------------------------------
// Protected virtual functions
//-------------------------------------------------------------------------------------------------

/* virtual */ void WLocalObject::applyId(int id)
{
    Q_D(WLocalObject);

    d->id = id;

    emit idChanged();
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WLocalObject::setSaved(bool ok)
{
    Q_D(WLocalObject);

    if (d->saving == false)
    {
        qWarning("WLocalObject::setSaved: Object is not saving.");

        return;
    }

    d->actionSave = NULL;

    if (d->state == Default)
    {
        d->state = Loaded;

        onStateChanged(Loaded);

        emit stateChanged();
    }

    if (d->timer.isActive() == false)
    {
        d->saving = false;

        emit savingChanged();

        if (ok) emit saved();

        d->updateProcessing();
    }
    else if (ok) emit saved();
}

/* virtual */ void WLocalObject::setLoaded(bool ok)
{
    Q_D(WLocalObject);

    if (d->state != Loading)
    {
        qWarning("WLocalObject::setLoaded: Object is not loading.");

        return;
    }

    d->actionLoad = NULL;

    d->cacheLoad = false;

    if (hasFolder()) d->folderCreated = true;

    if (ok)
    {
        d->pathCreated = true;

        d->setState(Loaded);
    }
    else d->setState(Default);

    emit loaded();
}

//-------------------------------------------------------------------------------------------------

/* virtual */ WAbstractThreadAction * WLocalObject::onSave(const QString &)
{
    qWarning("WLocalObject::onSave: Not supported.");

    return NULL;
}

/* virtual */ WAbstractThreadAction * WLocalObject::onLoad(const QString &)
{
    qWarning("WLocalObject::onLoad: Not supported.");

    return NULL;
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WLocalObject::onStateChanged      (State) {}
/* virtual */ void WLocalObject::onLockChanged       (bool)  {}
/* virtual */ void WLocalObject::onSaveEnabledChanged(bool)  {}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WLocalObject::onFileDeleted  () {}
/* virtual */ void WLocalObject::onFolderDeleted() {}

//-------------------------------------------------------------------------------------------------

/* virtual */ bool WLocalObject::hasFolder() const
{
    return false;
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

int WLocalObject::id() const
{
    Q_D(const WLocalObject); return d->id;
}

void WLocalObject::setId(int id)
{
    Q_D(WLocalObject);

    if (d->id == id) return;

    if (d->id != -1)
    {
        qWarning("WLocalObject::setId: Id already set '%d'.", d->id);

        return;
    }

    applyId(id);
}

//-------------------------------------------------------------------------------------------------

bool WLocalObject::isProcessing() const
{
    Q_D(const WLocalObject); return d->processing;
}

//-------------------------------------------------------------------------------------------------

bool WLocalObject::isSaving() const
{
    Q_D(const WLocalObject); return d->saving;
}

//-------------------------------------------------------------------------------------------------

WLocalObject::State WLocalObject::state() const
{
    Q_D(const WLocalObject); return d->state;
}

//-------------------------------------------------------------------------------------------------

bool WLocalObject::isDefault() const
{
    Q_D(const WLocalObject); return (d->state == Default);
}

bool WLocalObject::isLoading() const
{
    Q_D(const WLocalObject); return (d->state == Loading);
}

bool WLocalObject::isLoaded() const
{
    Q_D(const WLocalObject); return (d->state == Loaded);
}

//-------------------------------------------------------------------------------------------------

bool WLocalObject::isLocked() const
{
    Q_D(const WLocalObject); return d->locked;
}

//-------------------------------------------------------------------------------------------------

bool WLocalObject::saveEnabled() const
{
    Q_D(const WLocalObject); return d->saveEnabled;
}

void WLocalObject::setSaveEnabled(bool enabled)
{
    Q_D(WLocalObject);

    if (d->saveEnabled == enabled) return;

    d->saveEnabled = enabled;

    onSaveEnabledChanged(enabled);

    emit saveEnabledChanged();
}

//-------------------------------------------------------------------------------------------------

int WLocalObject::lockCount() const
{
    Q_D(const WLocalObject); return d->lockCount;
}

//=================================================================================================
// WLocalObjectReplySave
//=================================================================================================

WLocalObjectReplySave::WLocalObjectReplySave(WLocalObject * object)
{
    this->object = object;
}

//-------------------------------------------------------------------------------------------------
// WAbstractThreadReply reimplementation
//-------------------------------------------------------------------------------------------------

void WLocalObjectReplySave::onCompleted(bool ok)
{
    object->setSaved(ok);
}

//=================================================================================================
// WLocalObjectReplyLoad
//=================================================================================================

WLocalObjectReplyLoad::WLocalObjectReplyLoad(WLocalObject * object)
{
    this->object = object;
}

//-------------------------------------------------------------------------------------------------
// WAbstractThreadReply reimplementation
//-------------------------------------------------------------------------------------------------

void WLocalObjectReplyLoad::onCompleted(bool ok)
{
    object->setLoaded(ok);
}

#endif // SK_NO_LOCALOBJECT
