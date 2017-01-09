//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkCore module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#include "WControllerFile.h"

#ifndef SK_NO_CONTROLLERFILE

// Qt includes
#include <QImageReader>
#include <QDir>

// Sk includes
#include <WControllerApplication>
#include <WControllerNetwork>
#include <WControllerScript>
#include <WLocalObject>
#include <WFileWatcher>
#include <WCache>
#include <WAbstractThreadAction>
#include <WThreadActions>

// 3rd party includes
#include <qtlockedfile>

// Private includes
#include <private/WFileWatcher_p>

// Namespaces
using namespace QtLP_Private;

W_INIT_CONTROLLER(WControllerFile)

//=================================================================================================
// WControllerFileThreadAction
//=================================================================================================

class WControllerFileThreadAction : public WAbstractThreadAction
{
    Q_OBJECT

protected: // WAbstractThreadAction reimplementation
    /* virtual */ WAbstractThreadReply * createReply() const;

public: // Properties
    WControllerFileReply * controllerReply();
};

/* virtual */ WAbstractThreadReply * WControllerFileThreadAction::createReply() const
{
    return new WControllerFileReply;
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

WControllerFileReply * WControllerFileThreadAction::controllerReply()
{
    return qobject_cast<WControllerFileReply *> (reply());
}

//=================================================================================================
// WControllerFileRename
//=================================================================================================

class WControllerFileRename : public WControllerFileThreadAction
{
    Q_OBJECT

protected: // WAbstractThreadAction implementation
    /* virtual */ bool run();

public: // Variables
    QList<QString> oldPaths;
    QList<QString> newPaths;
};

/* virtual */ bool WControllerFileRename::run()
{
    for (int i = 0; i < oldPaths.count(); i++)
    {
        WControllerFile::renameFile(oldPaths.at(i), newPaths.at(i));
    }

    return true;
}

//=================================================================================================
// WControllerFileRename
//=================================================================================================

class WControllerFileDelete : public WControllerFileThreadAction
{
    Q_OBJECT

protected: // WAbstractThreadAction implementation
    /* virtual */ bool run();

public: // Variables
    QList<QString> paths;
};

/* virtual */ bool WControllerFileDelete::run()
{
    foreach (const QString & path, paths)
    {
        WControllerFile::deleteFile(path);
    }

    return true;
}

//=================================================================================================
// WControllerFileCreateFolders
//=================================================================================================

class WControllerFileCreateFolders : public WControllerFileThreadAction
{
    Q_OBJECT

protected: // WAbstractThreadAction implementation
    /* virtual */ bool run();

public: // Variables
    QList<QString> paths;
};

/* virtual */ bool WControllerFileCreateFolders::run()
{
    foreach (const QString & path, paths)
    {
        if (WControllerFile::createFolder(path) == false)
        {
            qWarning("WControllerFileCreateFolders::run: Failed to create folder %s.", path.C_STR);
        }
    }

    return true;
}

//=================================================================================================
// WControllerFileDeleteFolders
//=================================================================================================

class WControllerFileDeleteFolders : public WControllerFileThreadAction
{
    Q_OBJECT

protected: // WAbstractThreadAction implementation
    /* virtual */ bool run();

public: // Variables
    QList<QString> paths;

    bool recursive;
};

/* virtual */ bool WControllerFileDeleteFolders::run()
{
    foreach (const QString & path, paths)
    {
        if (WControllerFile::deleteFolder(path, recursive) == false)
        {
            qWarning("WControllerFileDeleteFolders::run: Failed to delete folder %s.", path.C_STR);
        }
    }

    return true;
}

//=================================================================================================
// WControllerFileDeleteFolders
//=================================================================================================

class WControllerFileDeleteFoldersContent : public WControllerFileThreadAction
{
    Q_OBJECT

protected: // WAbstractThreadAction implementation
    /* virtual */ bool run();

public: // Variables
    QList<QString> paths;

    bool recursive;
};

/* virtual */ bool WControllerFileDeleteFoldersContent::run()
{
    foreach (const QString & path, paths)
    {
        if (WControllerFile::deleteFolderContent(path, recursive) == false)
        {
            qWarning("WControllerFileDeleteFoldersContent::run: Failed to delete folder content %s"
                     ".", path.C_STR);
        }
    }

    return true;
}

//=================================================================================================
// WControllerFileCreatePaths
//=================================================================================================

class WControllerFileCreatePaths : public WControllerFileThreadAction
{
    Q_OBJECT

protected: // WAbstractThreadAction implementation
    /* virtual */ bool run();

public: // Variables
    QList<QString> paths;
};

/* virtual */ bool WControllerFileCreatePaths::run()
{
    QDir dir;

    foreach (const QString & path, paths)
    {
        if (dir.mkpath(path) == false)
        {
            qWarning("WControllerFileCreatePaths::run: Failed to create path %s.", path.C_STR);
        }
    }

    return true;
}

//=================================================================================================
// WControllerFileReply
//=================================================================================================

/* virtual */ void WControllerFileReply::onCompleted(bool ok)
{
    emit actionComplete(ok);
}

//=================================================================================================
// WControllerFilePrivate
//=================================================================================================

WControllerFilePrivate::WControllerFilePrivate(WControllerFile * p) : WControllerPrivate(p) {}

/* virtual */ WControllerFilePrivate::~WControllerFilePrivate()
{
    W_CLEAR_CONTROLLER(WControllerFile);
}

//-------------------------------------------------------------------------------------------------

void WControllerFilePrivate::init()
{
    threadWrite = NULL;
    threadRead  = NULL;

    cache = NULL;
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WControllerFilePrivate::createThreadWrite()
{
    if (threadWrite) return;

    Q_Q(WControllerFile);

    threadWrite = new WThreadActions(q);
}

void WControllerFilePrivate::createThreadRead()
{
    if (threadRead) return;

    Q_Q(WControllerFile);

    threadRead = new WThreadActions(q);
}

//-------------------------------------------------------------------------------------------------

void WControllerFilePrivate::registerLocalObject(WLocalObject * object)
{
    objects.append(object);
}

void WControllerFilePrivate::unregisterLocalObject(WLocalObject * object)
{
    objects.removeOne(object);
}

//-------------------------------------------------------------------------------------------------

void WControllerFilePrivate::registerFileWatcher(WFileWatcher * watcher)
{
    watchers.append(watcher);

    if (watchers.count() == 1)
    {
        Q_Q(WControllerFile);

        QObject::connect(&timer, SIGNAL(timeout()), q, SLOT(onCheckWatchers()));

        timer.start(1000);
    }
}

void WControllerFilePrivate::unregisterFileWatcher(WFileWatcher * watcher)
{
    watchers.removeOne(watcher);

    if (watchers.isEmpty())
    {
        Q_Q(WControllerFile);

        QObject::disconnect(&timer, 0, q, 0);

        timer.stop();
    }
}

//-------------------------------------------------------------------------------------------------

bool WControllerFilePrivate::objectsAreLoading()
{
    foreach (WLocalObject * object, objects)
    {
        if (object->isProcessing()) return true;
    }

    return false;
}

//-------------------------------------------------------------------------------------------------
// Private static functions
//-------------------------------------------------------------------------------------------------

/* static */ void WControllerFilePrivate::deleteDir(QDir & dir, bool recursive)
{
    QFileInfoList list;

    if (recursive)
    {
        list = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files  | QDir::AllDirs
                                                      | QDir::System | QDir::Hidden);

        foreach (QFileInfo info, list)
        {
            if (info.isDir())
            {
                qDebug("Deleting folder %s", info.fileName().C_STR);

                WControllerFile::deleteFolder(info.absoluteFilePath(), true);
            }
            else
            {
                qDebug("Deleting file %s", info.fileName().C_STR);

                dir.remove(info.absoluteFilePath());
            }
        }
    }
    else
    {
        list = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files
                                                      | QDir::System | QDir::Hidden);

        foreach (QFileInfo info, list)
        {
            qDebug("Deleting file %s", info.fileName().C_STR);

            dir.remove(info.absoluteFilePath());
        }
    }
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WControllerFilePrivate::onCheckWatchers()
{
    int fileCount = 20;

    foreach (WFileWatcher * watcher, watchers)
    {
        if (watcher->d_func()->checked) continue;

        if (watcher->d_func()->checkChange(fileCount) == false) return;
    }

    foreach (WFileWatcher * watcher, watchers)
    {
        watcher->d_func()->resetCheck();
    }
}

//=================================================================================================
// WControllerFile
//=================================================================================================
// Private

WControllerFile::WControllerFile() : WController(new WControllerFilePrivate(this)) {}

//-------------------------------------------------------------------------------------------------
// Initialize
//-------------------------------------------------------------------------------------------------

/* virtual */ void WControllerFile::init()
{
    Q_D(WControllerFile); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ WCacheFile * WControllerFile::getFile(const QUrl & url, QObject * parent,
                                                                          int       maxHost)
{
    Q_D(WControllerFile);

    if (d->cache)
    {
         return d->cache->getFile(url, parent, maxHost);
    }
    else return NULL;
}

/* Q_INVOKABLE */ WCacheFile * WControllerFile::getHttp(const QUrl & url, QObject * parent,
                                                                          int       maxHost)
{
    Q_D(WControllerFile);

    if (d->cache && WControllerNetwork::urlIsHttp(url))
    {
         return d->cache->getFile(url, parent, maxHost);
    }
    else return NULL;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ WCacheFile * WControllerFile::writeFile(const QUrl       & url,
                                                          const QByteArray & array,
                                                          QObject          * parent)
{
    Q_D(WControllerFile);

    if (d->cache)
    {
         return d->cache->writeFile(url, array, parent);
    }
    else return NULL;
}

/* Q_INVOKABLE */ void WControllerFile::addFile(const QUrl & url, const QByteArray & array)
{
    Q_D(WControllerFile);

    if (d->cache)
    {
        d->cache->addFile(url, array);
    }
    else qWarning("WControllerFile::addFile: Cannot add file %s. No cache.", url.C_URL);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WControllerFile::waitActions()
{
    Q_D(WControllerFile);

    QTimer timer;

    timer.start(10000); // 10 seconds

    while (d->objectsAreLoading() && timer.isActive())
    {
        qApp->processEvents();
    }
}

//-------------------------------------------------------------------------------------------------
// Threaded write and read

WAbstractThreadReply * WControllerFile::startWriteAction(WAbstractThreadAction * action)
{
    Q_D(WControllerFile);

    d->createThreadWrite();

    return d->threadWrite->pushAction(action);
}

WAbstractThreadReply * WControllerFile::startReadAction(WAbstractThreadAction * action)
{
    Q_D(WControllerFile);

    d->createThreadRead();

    return d->threadRead->pushAction(action);
}

//-------------------------------------------------------------------------------------------------

WControllerFileReply * WControllerFile::startRenameFiles(const QList<QString> & oldPaths,
                                                         const QList<QString> & newPaths)
{
    if (oldPaths.isEmpty()
        ||
        oldPaths.count() != newPaths.count()) return NULL;

    WControllerFileRename * action = new WControllerFileRename;

    action->oldPaths = oldPaths;
    action->newPaths = newPaths;

    startWriteAction(action);

    return action->controllerReply();
}

WControllerFileReply * WControllerFile::startDeleteFiles(const QList<QString> & paths)
{
    if (paths.isEmpty()) return NULL;

    WControllerFileDelete * action = new WControllerFileDelete;

    action->paths = paths;

    startWriteAction(action);

    return action->controllerReply();
}

//-------------------------------------------------------------------------------------------------

WControllerFileReply * WControllerFile::startCreateFolders(const QList<QString> & paths)
{
    if (paths.isEmpty()) return NULL;

    WControllerFileCreateFolders * action = new WControllerFileCreateFolders;

    action->paths = paths;

    startWriteAction(action);

    return action->controllerReply();
}

WControllerFileReply * WControllerFile::startDeleteFolders(const QList<QString> & paths,
                                                           bool                   recursive)
{
    if (paths.isEmpty()) return NULL;

    WControllerFileDeleteFolders * action = new WControllerFileDeleteFolders;

    action->paths     = paths;
    action->recursive = recursive;

    startWriteAction(action);

    return action->controllerReply();
}

WControllerFileReply * WControllerFile::startDeleteFoldersContent(const QList<QString> & paths,
                                                                  bool                   recursive)
{
    if (paths.isEmpty()) return NULL;

    WControllerFileDeleteFoldersContent * action = new WControllerFileDeleteFoldersContent;

    action->paths     = paths;
    action->recursive = recursive;

    startWriteAction(action);

    return action->controllerReply();
}

//-------------------------------------------------------------------------------------------------

WControllerFileReply * WControllerFile::startCreatePaths(const QList<QString> & paths)
{
    if (paths.isEmpty()) return NULL;

    WControllerFileCreatePaths * action = new WControllerFileCreatePaths;

    action->paths = paths;

    startWriteAction(action);

    return action->controllerReply();
}

//-------------------------------------------------------------------------------------------------

WControllerFileReply * WControllerFile::startRenameFile(const QString & oldPath,
                                                        const QString & newPath)
{
    return startRenameFiles(QList<QString>() << oldPath, QList<QString>() << newPath);
}

WControllerFileReply * WControllerFile::startDeleteFile(const QString & path)
{
    return startDeleteFiles(QList<QString>() << path);
}

//-------------------------------------------------------------------------------------------------

WControllerFileReply * WControllerFile::startCreateFolder(const QString & path)
{
    return startCreateFolders(QList<QString>() << path);
}

WControllerFileReply * WControllerFile::startDeleteFolder(const QString & path,
                                                          bool            recursive)
{
    return startDeleteFolders(QList<QString>() << path, recursive);
}

WControllerFileReply * WControllerFile::startDeleteFolderContent(const QString & path,
                                                                 bool            recursive)
{
    return startDeleteFoldersContent(QList<QString>() << path, recursive);
}

//-------------------------------------------------------------------------------------------------

WControllerFileReply * WControllerFile::startCreatePath(const QString & path)
{
    return startCreatePaths(QList<QString>() << path);
}

//-------------------------------------------------------------------------------------------------
// Static functions
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerFile::fileUrl(const QUrl & url)
{
    return fileUrl(url.toString());
}

/* Q_INVOKABLE static */ QString WControllerFile::fileUrl(const QString & string)
{
    return "file:///" + string;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerFile::filePath(const QUrl & url)
{
    QString string = url.toString();

    if (string.startsWith("file:///"))
    {
         return url.toLocalFile();
    }
    else return string;
}

/* Q_INVOKABLE static */ QString WControllerFile::filePath(const QString & string)
{
    if (string.startsWith("file:///"))
    {
         return QUrl(string).toLocalFile();
    }
    else return string;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerFile::applicationPath(const QString & fileName)
{
    return QCoreApplication::applicationDirPath() + '/' + fileName;
}

/* Q_INVOKABLE static */ QString WControllerFile::applicationFileUrl(const QString & fileName)
{
    return fileUrl(applicationPath(fileName));
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerFile::folderPath(const QString & fileName)
{
    QFileInfo info(filePath(fileName));

    if (info.isDir())
    {
         return info.absoluteFilePath();
    }
    else return info.absolutePath();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerFile::toLocalFile(const QUrl & url)
{
    if (url.scheme().toLower() == "qrc")
    {
         return url.toString().mid(3);
    }
    else return url.toLocalFile();
}

/* Q_INVOKABLE static */ QString WControllerFile::toString(const QUrl & url)
{
    if (url.scheme().toLower() == "qrc")
    {
         return url.toString().mid(3);
    }
    else return url.toString();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QByteArray WControllerFile::readAll(const QString & fileName)
{
    QFile file(toLocalFile(fileName));

    if (file.open(QIODevice::ReadOnly) == false)
    {
        qWarning("WControllerFile::readAll: Failed to open file %s.", fileName.C_STR);

        return QByteArray();
    }

    return file.readAll();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ bool WControllerFile::exists(const QString & fileName)
{
    QFile file(toLocalFile(fileName));

    return file.exists();
}

/* Q_INVOKABLE static */ bool WControllerFile::tryAppend(const QString & fileName)
{
    QFile file(toLocalFile(fileName));

    if (file.exists())
    {
         return file.open(QIODevice::Append);
    }
    else return false;
}

//-------------------------------------------------------------------------------------------------
// Files

/* static */ bool WControllerFile::renameFile(const QString & oldPath, const QString & newPath)
{
    QtLockedFile file(oldPath);

    QTimer timer;

    timer.start(60000); // 1 minute timeout

    while (file.isLocked() && timer.isActive()) ;

    if (file.isLocked())
    {
        qWarning("WControllerFile::renameFile: File is locked %s.", oldPath.C_STR);

        return false;
    }

    return file.rename(newPath);
}

//-------------------------------------------------------------------------------------------------

/* static */ bool WControllerFile::deleteFile(const QString & fileName)
{
    QtLockedFile file(fileName);

    QTimer timer;

    timer.start(60000); // 1 minute timeout

    while (file.isLocked() && timer.isActive()) ;

    if (file.isLocked())
    {
        qWarning("WControllerFile::deleteFile: File is locked %s.", fileName.C_STR);

        return false;
    }

    return file.remove();
}

//-------------------------------------------------------------------------------------------------

/* static */ bool WControllerFile::copy(const QString & fileName, const QString & newName)
{
    return QFile::copy(fileName, newName);
}

/* static */ bool WControllerFile::copyFiles(const QStringList & fileNames,
                                             const QString     & destination)
{
    if (QFile::exists(destination) == false) return false;

    foreach (const QString & file, fileNames)
    {
        QFileInfo info(file);

        if (info.exists() == false) continue;

        QString path = destination + "/" + info.fileName();

        QFile::copy(file, path);
    }

    return true;
}

//-------------------------------------------------------------------------------------------------
// Directories

/* static */ bool WControllerFile::createFolder(const QString & path)
{
    return QDir().mkdir(path);
}

//-------------------------------------------------------------------------------------------------

/* static */ bool WControllerFile::moveFolder(const QString & oldPath, const QString & newPath)
{
    return QDir().rename(oldPath, newPath);
}

//-------------------------------------------------------------------------------------------------

/* static */ bool WControllerFile::deleteFolder(const QString & path, bool recursive)
{
     QDir dir(path);

     if (dir.exists() == false) return false;

     WControllerFilePrivate::deleteDir(dir, recursive);

     dir.rmdir(dir.absolutePath());

     return true;
}

/* static */ bool WControllerFile::deleteFolderContent(const QString & path, bool recursive)
{
    QDir dir(path);

    if (dir.exists() == false) return false;

    WControllerFilePrivate::deleteDir(dir, recursive);

    return true;
}

//-------------------------------------------------------------------------------------------------

/* static */ QFileInfoList WControllerFile::recursiveEntryInfoList(const QString & path)
{
     QDir dir(path);

     if (dir.exists() == false) return QFileInfoList();

     QFileInfoList list = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

     foreach (QFileInfo info, list)
     {
        list.append(recursiveEntryInfoList(info.filePath()));
     }

     return list;
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WControllerFile::onCheckWatchers()
{
    Q_D(WControllerFile);

    int fileCount = 20;

    foreach (WFileWatcher * watcher, d->watchers)
    {
        if (watcher->d_func()->checked) continue;

        if (watcher->d_func()->checkChange(fileCount) == false) return;
    }

    foreach (WFileWatcher * watcher, d->watchers)
    {
        watcher->d_func()->resetCheck();
    }
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

QString WControllerFile::pathStorage() const
{
    Q_D(const WControllerFile); return d->pathStorage;
}

void WControllerFile::setPathStorage(const QString & path)
{
    Q_D(WControllerFile);

    if (d->pathStorage == path) return;

    d->pathStorage = path;

    emit pathStorageChanged();
}

//-------------------------------------------------------------------------------------------------

WCache * WControllerFile::cache() const
{
    Q_D(const WControllerFile); return d->cache;
}

void WControllerFile::setCache(WCache * cache)
{
    Q_D(WControllerFile);

    if (d->cache == cache) return;

    if (d->cache) delete d->cache;

    d->cache = cache;

    if (cache) cache->setParent(this);

    emit cacheChanged();
}

#endif // SK_NO_CONTROLLERFILE

#include "WControllerFile.moc"
