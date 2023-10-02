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

#include "WControllerFile.h"

#ifndef SK_NO_CONTROLLERFILE

// C++ includes
#include <iostream>

// Qt includes
#include <QCoreApplication>
#include <QDir>
#include <qtlockedfile>
#ifdef QT_4
#include <QDesktopServices>
#else
#include <QStandardPaths>
#endif
#if defined(SK_CONSOLE) == false && defined(QT_6)
#include <QQmlEngine>
#include <QQmlContext>
#endif


#ifdef Q_OS_ANDROID
// Android includes
#include <android/log.h>
#endif

// Sk includes
#include <WControllerApplication>
#include <WControllerNetwork>
#include <WLocalObject>
#include <WFileWatcher>
#include <WCache>
#include <WThreadActions>
#include <WRegExp>

// Private includes
#include <private/WFileWatcher_p>

// Namespaces
using namespace QtLP_Private;

W_INIT_CONTROLLER(WControllerFile)

//-------------------------------------------------------------------------------------------------
// Static variables

static const int CONTROLLERFILE_LOG_INTERVAL =  1000; // 1 seconds
static const int CONTROLLERFILE_LOG_MAX      = 10000;

static const QString CONTROLLERFILE_IMAGE = "^(bmp|png|jpg|jpeg|svg|tga)$";

static const QString CONTROLLERFILE_FILTER = "Images (*.bmp *.png *.jpg *.jpeg *.svg *.tga);;"
                                             "All files (*)";

//=================================================================================================
// WControllerFileAction
//=================================================================================================

/* virtual */ WAbstractThreadReply * WControllerFileAction::createReply() const
{
    return new WControllerFileReply;
}

//-------------------------------------------------------------------------------------------------

WControllerFileReply * WControllerFileAction::controllerReply()
{
    return qobject_cast<WControllerFileReply *> (reply());
}

//=================================================================================================
// WControllerFileWrite
//=================================================================================================

class WControllerFileWrite : public WControllerFileAction
{
    Q_OBJECT

protected: // WAbstractThreadAction implementation
    /* virtual */ bool run();

public: // Variables
    QStringList fileNames;

    QList<QByteArray> datas;
};

/* virtual */ bool WControllerFileWrite::run()
{
    for (int i = 0; i < fileNames.count(); i++)
    {
        WControllerFile::writeFile(fileNames.at(i), datas.at(i));
    }

    return true;
}

//=================================================================================================
// WControllerFileAppend
//=================================================================================================

class WControllerFileAppend : public WControllerFileAction
{
    Q_OBJECT

protected: // WAbstractThreadAction implementation
    /* virtual */ bool run();

public: // Variables
    QStringList fileNames;

    QList<QByteArray> datas;
};

/* virtual */ bool WControllerFileAppend::run()
{
    for (int i = 0; i < fileNames.count(); i++)
    {
        WControllerFile::appendFile(fileNames.at(i), datas.at(i));
    }

    return true;
}

//=================================================================================================
// WControllerFileRename
//=================================================================================================

class WControllerFileRename : public WControllerFileAction
{
    Q_OBJECT

protected: // WAbstractThreadAction implementation
    /* virtual */ bool run();

public: // Variables
    QStringList oldPaths;
    QStringList newPaths;
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
// WControllerFileCopy
//=================================================================================================

class WControllerFileCopy : public WControllerFileAction
{
    Q_OBJECT

protected: // WAbstractThreadAction implementation
    /* virtual */ bool run();

public: // Variables
    QStringList fileNames;
    QStringList newNames;

    WControllerFile::Permissions permissions;
};

/* virtual */ bool WControllerFileCopy::run()
{
    for (int i = 0; i < fileNames.count(); i++)
    {
        WControllerFile::copyFile(fileNames.at(i), newNames.at(i), permissions);
    }

    return true;
}

//=================================================================================================
// WControllerFileDelete
//=================================================================================================

class WControllerFileDelete : public WControllerFileAction
{
    Q_OBJECT

protected: // WAbstractThreadAction implementation
    /* virtual */ bool run();

public: // Variables
    QStringList paths;
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

class WControllerFileCreateFolders : public WControllerFileAction
{
    Q_OBJECT

protected: // WAbstractThreadAction implementation
    /* virtual */ bool run();

public: // Variables
    QStringList paths;
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

class WControllerFileDeleteFolders : public WControllerFileAction
{
    Q_OBJECT

protected: // WAbstractThreadAction implementation
    /* virtual */ bool run();

public: // Variables
    QStringList paths;

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
// WControllerFileDeleteFoldersContent
//=================================================================================================

class WControllerFileDeleteFoldersContent : public WControllerFileAction
{
    Q_OBJECT

protected: // WAbstractThreadAction implementation
    /* virtual */ bool run();

public: // Variables
    QStringList paths;

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

class WControllerFileCreatePaths : public WControllerFileAction
{
    Q_OBJECT

protected: // WAbstractThreadAction implementation
    /* virtual */ bool run();

public: // Variables
    QStringList paths;
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
    emit complete(ok);
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

    pathLog = "log.txt";

    cache = NULL;
}

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

        QObject::connect(&timerWatcher, SIGNAL(timeout()), q, SLOT(onCheckWatchers()));

        timerWatcher.start(1000);
    }
}

void WControllerFilePrivate::unregisterFileWatcher(WFileWatcher * watcher)
{
    watchers.removeOne(watcher);

    if (watchers.isEmpty())
    {
        Q_Q(WControllerFile);

        QObject::disconnect(&timerWatcher, 0, q, 0);

        timerWatcher.stop();
    }
}

//-------------------------------------------------------------------------------------------------

void WControllerFilePrivate::clearMessageHandler()
{
    // NOTE: We rely on 'isSingleShot' to detect if the handler is initialized.
    if (timerLog.isSingleShot())
    {
#ifdef QT_4
        qInstallMsgHandler(NULL);
#else
        qInstallMessageHandler(NULL);
#endif
    }
}

//-------------------------------------------------------------------------------------------------

bool WControllerFilePrivate::isLoading() const
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

#ifdef QT_4
/* static */ void WControllerFilePrivate::messageHandler(QtMsgType, const char *) {}
#else
/* static */ void WControllerFilePrivate::messageHandler(QtMsgType,
                                                         const QMessageLogContext &,
                                                         const QString            & message)
{
    wControllerFile->d_func()->method.invoke(wControllerFile, Q_ARG(const QString &, message));

#ifdef Q_OS_ANDROID
#ifdef QT_4
    __android_log_write(ANDROID_LOG_DEBUG, "Sky", message);
#else
    __android_log_write(ANDROID_LOG_DEBUG, "Sky", message.C_STR);
#endif
#else
#ifdef QT_4
    std::cout << message << std::endl;
#else
    std::cout << message.C_STR << std::endl;
#endif
#endif
}
#endif

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

void WControllerFilePrivate::onLog(const QString & message)
{
    Q_Q(WControllerFile);

    QString string = message;

    if (log.isEmpty() == false)
    {
        string.prepend('\n');
    }

    log.append(string);

    int length = log.length();

    if (length > CONTROLLERFILE_LOG_MAX)
    {
        log.remove(0, length - CONTROLLERFILE_LOG_MAX);
    }

    emit q->logChanged(string);

    logBuffer.append(string);

    if (timerLog.isActive() == false)
    {
        timerLog.start();
    }
}

void WControllerFilePrivate::onWriteLog()
{
    Q_Q(WControllerFile);

    q->startAppendFile(pathLog, logBuffer.toUtf8());

    logBuffer.clear();
}

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

/* Q_INVOKABLE */ void WControllerFile::initMessageHandler()
{
    Q_D(WControllerFile);

    if (d->timerLog.isSingleShot())
    {
        qWarning("WControllerFile::initMessageHandler: Handler is already initialized.");

        return;
    }

    // NOTE: We clear the log file.
    startWriteFile(d->pathLog, QByteArray());

    const QMetaObject * meta = metaObject();

    d->method = meta->method(meta->indexOfMethod("onLog(QString)"));

    d->timerLog.setInterval(CONTROLLERFILE_LOG_INTERVAL);

    d->timerLog.setSingleShot(true);

    connect(&d->timerLog, SIGNAL(timeout()), this, SLOT(onWriteLog()));

#ifdef QT_4
    // FIXME Qt4.8.7: qInstallMsgHandler breaks QML 'Keys' events.
    qWarning("WControllerFile::initMessageHandler: qInstallMsgHandler breaks QML 'Keys' events on "
             "Qt 4.");

    qInstallMsgHandler(WControllerFilePrivate::messageHandler);
#else
    qInstallMessageHandler(WControllerFilePrivate::messageHandler);
#endif
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ WCacheFile * WControllerFile::getFile(const QString & url, QObject * parent,
                                                                             int       maxHost)
{
    Q_D(WControllerFile);

    if (d->cache)
    {
         return d->cache->getFile(url, parent, maxHost);
    }
    else return NULL;
}

/* Q_INVOKABLE */ void WControllerFile::reloadFile(const QString & url, int delay, int maxHost)
{
    Q_D(WControllerFile);

    if (d->cache) d->cache->reloadFile(url, delay, maxHost);
}

/* Q_INVOKABLE */ WCacheFile * WControllerFile::getHttp(const QString & url, QObject * parent,
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

/* Q_INVOKABLE */ QString WControllerFile::getFileUrl(const QString & url)
{
    Q_D(WControllerFile);

    if (d->cache)
    {
         return d->cache->getFileUrl(url);
    }
    else return QString();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ WCacheFile * WControllerFile::writeCache(const QString    & url,
                                                           const QByteArray & array,
                                                           const QString    & extension,
                                                           QObject          * parent)
{
    Q_D(WControllerFile);

    if (d->cache)
    {
         return d->cache->writeFile(url, array, extension, parent);
    }
    else return NULL;
}

/* Q_INVOKABLE */ void WControllerFile::addCache(const QString & url, const QByteArray & array,
                                                                      const QString    & extension)
{
    Q_D(WControllerFile);

    if (d->cache)
    {
        d->cache->addFile(url, array, extension);
    }
    else qWarning("WControllerFile::addFile: Cannot add file %s. No cache.", url.C_STR);
}

/* Q_INVOKABLE */ void WControllerFile::removeCaches(const QStringList & urls)
{
    Q_D(WControllerFile);

    if (d->cache)
    {
        d->cache->removeFiles(urls);
    }
    else qWarning("WControllerFile::removeCaches: Cannot remove files. No cache.");
}

/* Q_INVOKABLE */ void WControllerFile::removeCache(const QString & url)
{
    Q_D(WControllerFile);

    if (d->cache)
    {
        d->cache->removeFile(url);
    }
    else qWarning("WControllerFile::removeCache: Cannot remove file %s. No cache.", url.C_STR);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WControllerFile::waitActions()
{
    Q_D(WControllerFile);

    QTimer timer;

    timer.start(10000); // 10 seconds

    while (d->isLoading() && timer.isActive())
    {
        qApp->processEvents();
    }
}

//-------------------------------------------------------------------------------------------------
// Thread actions

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
// File actions

WControllerFileReply * WControllerFile::startWriteFile(const QString    & fileName,
                                                       const QByteArray & data)
{
    return startWriteFiles(QStringList() << fileName, QList<QByteArray>() << data);
}

WControllerFileReply * WControllerFile::startAppendFile(const QString    & fileName,
                                                        const QByteArray & data)
{
    return startAppendFiles(QStringList() << fileName, QList<QByteArray>() << data);
}

WControllerFileReply * WControllerFile::startRenameFile(const QString & oldPath,
                                                        const QString & newPath)
{
    return startRenameFiles(QStringList() << oldPath, QStringList() << newPath);
}

WControllerFileReply * WControllerFile::startCopyFile(const QString & fileName,
                                                      const QString & newName,
                                                      Permissions     permissions)
{
    return startCopyFiles(QStringList() << fileName, QStringList() << newName, permissions);
}

WControllerFileReply * WControllerFile::startDeleteFile(const QString & path)
{
    return startDeleteFiles(QStringList() << path);
}

//-------------------------------------------------------------------------------------------------

WControllerFileReply * WControllerFile::startWriteFiles(const QStringList       & fileNames,
                                                        const QList<QByteArray> & datas)
{
    if (fileNames.isEmpty() || fileNames.count() != datas.count()) return NULL;

    WControllerFileWrite * action = new WControllerFileWrite;

    action->fileNames = fileNames;
    action->datas     = datas;

    startWriteAction(action);

    return action->controllerReply();
}

WControllerFileReply * WControllerFile::startAppendFiles(const QStringList       & fileNames,
                                                         const QList<QByteArray> & datas)
{
    if (fileNames.isEmpty() || fileNames.count() != datas.count()) return NULL;

    WControllerFileAppend * action = new WControllerFileAppend;

    action->fileNames = fileNames;
    action->datas     = datas;

    startWriteAction(action);

    return action->controllerReply();
}

WControllerFileReply * WControllerFile::startRenameFiles(const QStringList & oldPaths,
                                                         const QStringList & newPaths)
{
    if (oldPaths.isEmpty() || oldPaths.count() != newPaths.count()) return NULL;

    WControllerFileRename * action = new WControllerFileRename;

    action->oldPaths = oldPaths;
    action->newPaths = newPaths;

    startWriteAction(action);

    return action->controllerReply();
}

#ifdef QT_4
WControllerFileReply * WControllerFile::startCopyFiles(const QStringList & fileNames,
                                                       const QStringList & newNames,
                                                       Permissions         permissions)
#else
WControllerFileReply * WControllerFile::startCopyFiles(const QStringList & fileNames,
                                                       const QStringList & newNames,
                                                       Permissions         permissions)
#endif
{
    if (fileNames.isEmpty() || fileNames.count() != newNames.count()) return NULL;

    WControllerFileCopy * action = new WControllerFileCopy;

    action->fileNames = fileNames;
    action->newNames  = newNames;

    action->permissions = permissions;

    startWriteAction(action);

    return action->controllerReply();
}

WControllerFileReply * WControllerFile::startDeleteFiles(const QStringList & paths)
{
    if (paths.isEmpty()) return NULL;

    WControllerFileDelete * action = new WControllerFileDelete;

    action->paths = paths;

    startWriteAction(action);

    return action->controllerReply();
}

//-------------------------------------------------------------------------------------------------
// Folder actions

WControllerFileReply * WControllerFile::startCreateFolder(const QString & path)
{
    return startCreateFolders(QStringList() << path);
}

WControllerFileReply * WControllerFile::startDeleteFolder(const QString & path,
                                                          bool            recursive)
{
    return startDeleteFolders(QStringList() << path, recursive);
}

WControllerFileReply * WControllerFile::startDeleteFolderContent(const QString & path,
                                                                 bool            recursive)
{
    return startDeleteFoldersContent(QStringList() << path, recursive);
}

WControllerFileReply * WControllerFile::startCreatePath(const QString & path)
{
    return startCreatePaths(QStringList() << path);
}

//-------------------------------------------------------------------------------------------------

WControllerFileReply * WControllerFile::startCreateFolders(const QStringList & paths)
{
    if (paths.isEmpty()) return NULL;

    WControllerFileCreateFolders * action = new WControllerFileCreateFolders;

    action->paths = paths;

    startWriteAction(action);

    return action->controllerReply();
}

WControllerFileReply * WControllerFile::startDeleteFolders(const QStringList & paths,
                                                           bool                recursive)
{
    if (paths.isEmpty()) return NULL;

    WControllerFileDeleteFolders * action = new WControllerFileDeleteFolders;

    action->paths     = paths;
    action->recursive = recursive;

    startWriteAction(action);

    return action->controllerReply();
}

WControllerFileReply * WControllerFile::startDeleteFoldersContent(const QStringList & paths,
                                                                  bool                recursive)
{
    if (paths.isEmpty()) return NULL;

    WControllerFileDeleteFoldersContent * action = new WControllerFileDeleteFoldersContent;

    action->paths     = paths;
    action->recursive = recursive;

    startWriteAction(action);

    return action->controllerReply();
}

WControllerFileReply * WControllerFile::startCreatePaths(const QStringList & paths)
{
    if (paths.isEmpty()) return NULL;

    WControllerFileCreatePaths * action = new WControllerFileCreatePaths;

    action->paths = paths;

    startWriteAction(action);

    return action->controllerReply();
}

//-------------------------------------------------------------------------------------------------
// Static functions
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerFile::absolute(const QUrl & url)
{
    return absolute(url.toString());
}

/* Q_INVOKABLE static */ QString WControllerFile::absolute(const QString & string)
{
    QFileInfo info(string);

    return info.absoluteFilePath();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerFile::fileUrl(const QUrl & url)
{
    return QUrl::fromLocalFile(url.toString()).toString();
}

/* Q_INVOKABLE static */ QString WControllerFile::fileUrl(const QString & string)
{
    return QUrl::fromLocalFile(string).toString();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerFile::filePath(const QUrl & url)
{
    QString string = url.toString();

    if (string.startsWith("file://"))
    {
         return url.toLocalFile();
    }
    else return string;
}

/* Q_INVOKABLE static */ QString WControllerFile::filePath(const QString & string)
{
    if (string.startsWith("file://"))
    {
         return QUrl(string).toLocalFile();
    }
    else return string;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerFile::fileAbsolute(const QUrl & url)
{
    return fileAbsolute(url.toString());
}

/* Q_INVOKABLE static */ QString WControllerFile::fileAbsolute(const QString & string)
{
    return fileUrl(absolute(string));
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerFile::fileBaseName(const QString & fileName)
{
    int index = fileName.lastIndexOf('/');

    if (index == -1)
    {
        return fileName.left(fileName.indexOf('.'));
    }

    QString name = fileName.mid(index + 1);

    return name.left(name.indexOf('.'));
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerFile::currentPath(const QString & fileName)
{
    return QDir::currentPath() + '/' + fileName;
}

/* Q_INVOKABLE static */ QString WControllerFile::currentFileUrl(const QString & fileName)
{
    return fileUrl(currentPath(fileName));
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerFile::applicationName()
{
    return fileBaseName(QCoreApplication::applicationFilePath());
}

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

/* Q_INVOKABLE static */ QString WControllerFile::folderUrl(const QString & fileName)
{
    return fileUrl(folderPath(fileName));
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerFile::folderName(const QString & fileName)
{
    int index = fileName.lastIndexOf('/');

    if (index == -1)
    {
         return fileName;
    }
    else return fileName.mid(index + 1);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerFile::toLocalFile(const QString & url)
{
    if (url.toLower().startsWith("qrc"))
    {
        return url.mid(3);
    }

    QString string = QUrl(url).toLocalFile();

    if (string.isEmpty())
    {
         return url;
    }
    else return string;
}

/* Q_INVOKABLE static */ QString WControllerFile::toString(const QString & url)
{
    if (url.toLower().startsWith("qrc"))
    {
         return url.mid(3);
    }
    else return url;
}

//-------------------------------------------------------------------------------------------------

#ifndef SK_CONSOLE

/* Q_INVOKABLE static */ QString WControllerFile::resolvedUrl(const QObject * object,
                                                              const QString & url)
{
#ifdef QT_6
    QQmlContext * context = QQmlEngine::contextForObject(object);

    if (context)
    {
         return context->resolvedUrl(url).toString();
    }
    else return url;
#else
    Q_UNUSED(object);

    return url;
#endif
}

#endif

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerFile::pathWritable()
{
#ifdef Q_OS_IOS
    // NOTE iOS: We need to write in the documents location.
    return QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
#elif defined(QT_4)
    return QDesktopServices::storageLocation(QDesktopServices::DataLocation);
#elif defined(QT_5)
    return QStandardPaths::writableLocation(QStandardPaths::DataLocation);
#else
    return QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
#endif
}

/* Q_INVOKABLE static */ QString WControllerFile::pathDocuments()
{
#ifdef QT_4
    return QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);
#else
    return QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
#endif
}

/* Q_INVOKABLE static */ QString WControllerFile::pathPictures()
{
#ifdef QT_4
    return QDesktopServices::storageLocation(QDesktopServices::PicturesLocation);
#else
    return QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
#endif
}

/* Q_INVOKABLE static */ QString WControllerFile::pathTemp()
{
#ifdef QT_4
    return QDesktopServices::storageLocation(QDesktopServices::TempLocation);
#else
    return QStandardPaths::writableLocation(QStandardPaths::TempLocation);
#endif
}
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QByteArray WControllerFile::readAll(QIODevice * device)
{
    if (device->isOpen())
    {
        return device->readAll();
    }
    else return QByteArray();
}

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
// Filters

/* Q_INVOKABLE static */ QString WControllerFile::getFilterImage()
{
    return CONTROLLERFILE_FILTER;
}

//-------------------------------------------------------------------------------------------------
// Urls

/* Q_INVOKABLE static */ bool WControllerFile::urlIsImage(const QString & url)
{
    if (url.startsWith("image:", Qt::CaseInsensitive)) return true;

    QString extension = WControllerNetwork::extractUrlExtension(url);

    return extensionIsImage(extension);
}

/* Q_INVOKABLE static */ bool WControllerFile::extensionIsImage(const QString & extension)
{
    return (extension.indexOf(WRegExp(CONTROLLERFILE_IMAGE)) != -1);
}

//-------------------------------------------------------------------------------------------------
// Files

/* static */ bool WControllerFile::tryUnlock(const QtLockedFile & file, int timeout)
{
    QTimer timer;

    timer.start(timeout);

    while (file.isLocked() && timer.isActive());

    if (file.isLocked())
    {
         return false;
    }
    else return true;
}

/* static */ QByteArray WControllerFile::readFile(const QString & fileName)
{
    QtLockedFile file(fileName);

    if (WControllerFile::tryUnlock(file) == false)
    {
        qWarning("WControllerFile::readFile: File is locked %s.", fileName.C_STR);

        return QByteArray();
    }

    if (file.open(QIODevice::ReadOnly) == false)
    {
        qWarning("WControllerFile::readFile: Cannot open file %s.", fileName.C_STR);

        return QByteArray();
    }

    return file.readAll();
}

/* static */ bool WControllerFile::writeFile(const QString & fileName, const QByteArray & data)
{
    QtLockedFile file(fileName);

    if (WControllerFile::tryUnlock(file) == false)
    {
        qWarning("WControllerFile::writeFile: File is locked %s.", fileName.C_STR);

        return false;
    }

    if (file.open(QIODevice::WriteOnly) == false)
    {
        qWarning("WControllerFile::writeFile: Cannot open file %s.", fileName.C_STR);

        return false;
    }

    file.write(data);

    return true;
}

/* static */ bool WControllerFile::appendFile(const QString & fileName, const QByteArray & data)
{
    QtLockedFile file(fileName);

    if (WControllerFile::tryUnlock(file) == false)
    {
        qWarning("WControllerFile::appendFile: File is locked %s.", fileName.C_STR);

        return false;
    }

    if (file.open(QIODevice::Append) == false)
    {
        qWarning("WControllerFile::appendFile: Cannot open file %s.", fileName.C_STR);

        return false;
    }

    file.write(data);

    return true;
}

/* static */ bool WControllerFile::renameFile(const QString & oldPath, const QString & newPath)
{
    QtLockedFile file(oldPath);

    if (WControllerFile::tryUnlock(file) == false)
    {
        qWarning("WControllerFile::renameFile: File is locked %s.", oldPath.C_STR);

        return false;
    }

    return file.rename(newPath);
}

/* static */ bool WControllerFile::copyFile(const QString & fileName, const QString & newName,
                                            Permissions permissions)
{
    QtLockedFile file(newName);

    if (file.exists())
    {
        if (WControllerFile::tryUnlock(file) == false)
        {
            qWarning("WControllerFile::copyFile: File is locked %s.", newName.C_STR);

            return false;
        }
    }

    bool result = QFile::copy(fileName, newName);

    if (permissions != Default)
    {
        if (result)
        {
#ifdef QT_4
            return QFile::setPermissions(newName, QFile::Permissions((int) permissions));
#else
            return QFile::setPermissions(newName, QFileDevice::Permissions((int) permissions));
#endif
        }
        else return false;
    }
    else return result;
}

/* static */ bool WControllerFile::deleteFile(const QString & fileName)
{
    QtLockedFile file(fileName);

    if (WControllerFile::tryUnlock(file) == false)
    {
        qWarning("WControllerFile::deleteFile: File is locked %s.", fileName.C_STR);

        return false;
    }

    return file.remove();
}

//-------------------------------------------------------------------------------------------------
// Folders

/* static */ bool WControllerFile::createFolder(const QString & path)
{
    return QDir().mkdir(path);
}

/* static */ bool WControllerFile::moveFolder(const QString & oldPath, const QString & newPath)
{
    return QDir().rename(oldPath, newPath);
}

/* static */ bool WControllerFile::deleteFolder(const QString & path, bool recursive)
{
     QDir dir(path);

     if (dir.exists() == false) return false;

     WControllerFilePrivate::deleteDir(dir, recursive);

     return dir.rmdir(dir.absolutePath());
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

     QFileInfoList list = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

     foreach (QFileInfo info, list)
     {
        list.append(recursiveEntryInfoList(info.filePath()));
     }

     return list;
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

QString WControllerFile::log() const
{
    Q_D(const WControllerFile); return d->log;
}

//-------------------------------------------------------------------------------------------------

QString WControllerFile::pathStorage() const
{
    Q_D(const WControllerFile); return d->pathStorage;
}

QString WControllerFile::pathLog() const
{
    Q_D(const WControllerFile); return d->pathLog;
}

void WControllerFile::setPathStorage(const QString & path)
{
    Q_D(WControllerFile);

    if (d->pathStorage == path) return;

    d->pathStorage = path;

    d->pathLog = path + "/log.txt";

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
