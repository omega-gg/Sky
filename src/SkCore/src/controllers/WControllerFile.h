//=================================================================================================
/*
    Copyright (C) 2015-2016 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkCore module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#ifndef WCONTROLLERFILE_H
#define WCONTROLLERFILE_H

// Qt includes
#include <QFileInfo>
#include <QEvent>
#include <QUrl>

// Sk includes
#include <WController>
#include <WAbstractThreadReply>

#ifndef SK_NO_CONTROLLERFILE

// Forward declarations
class WControllerFilePrivate;
class WAbstractThreadAction;
class WAbstractThreadReply;
class WCache;
class WCacheFile;

// Defines
#define wControllerFile WControllerFile::instance()

//=================================================================================================
// WControllerFileReply
//=================================================================================================

class WControllerFileReply : public WAbstractThreadReply
{
    Q_OBJECT

protected:
    WControllerFileReply() {}

signals:
    void actionComplete(bool ok);

protected: // WAbstractThreadReply reimplementation
    /* virtual */ void onCompleted(bool ok);

private:
    Q_DISABLE_COPY(WControllerFileReply)

    friend class WControllerFileThreadAction;
};

//=================================================================================================
// WControllerFile
//=================================================================================================

class SK_CORE_EXPORT WControllerFile : public WController
{
    Q_OBJECT

    Q_PROPERTY(QString pathStorage READ pathStorage WRITE setPathStorage NOTIFY pathStorageChanged)

    Q_PROPERTY(WCache * cache READ cache WRITE setCache NOTIFY cacheChanged)

private:
    WControllerFile();

protected: // Initialize
    /* virtual */ void init();

public: // Interface
    Q_INVOKABLE WCacheFile * getFile(const QUrl & url, QObject * parent = NULL, int maxHost = -1);

    Q_INVOKABLE void waitActions();

    //---------------------------------------------------------------------------------------------
    // Thread actions

    WAbstractThreadReply * startWriteAction(WAbstractThreadAction * action);
    WAbstractThreadReply * startReadAction (WAbstractThreadAction * action);

    WControllerFileReply * startRenameFiles(const QList<QString> & oldPaths,
                                            const QList<QString> & newPaths);

    WControllerFileReply * startDeleteFiles(const QList<QString> & paths);

    WControllerFileReply * startCreateFolders(const QList<QString> & paths);

    WControllerFileReply * startDeleteFolders(const QList<QString> & paths,
                                              bool                   recursive = true);

    WControllerFileReply * startDeleteFoldersContent(const QList<QString> & paths,
                                                     bool                   recursive = true);

    WControllerFileReply * startCreatePaths(const QList<QString> & paths);

    //---------------------------------------------------------------------------------------------

    WControllerFileReply * startRenameFile(const QString & oldPath, const QString & newPath);
    WControllerFileReply * startDeleteFile(const QString & path);

    WControllerFileReply * startCreateFolder       (const QString & path);
    WControllerFileReply * startDeleteFolder       (const QString & path, bool recursive = true);
    WControllerFileReply * startDeleteFolderContent(const QString & path, bool recursive = true);

    WControllerFileReply * startCreatePath(const QString & path);

public: // Static functions
    Q_INVOKABLE static QString fileUrl(const QUrl    & url);
    Q_INVOKABLE static QString fileUrl(const QString & string);

    Q_INVOKABLE static QString filePath(const QUrl    & url);
    Q_INVOKABLE static QString filePath(const QString & string);

    Q_INVOKABLE static QString applicationPath   (const QString & fileName);
    Q_INVOKABLE static QString applicationFileUrl(const QString & fileName);

    Q_INVOKABLE static QString folderPath(const QString & fileName);

    Q_INVOKABLE static QString toLocalFile(const QUrl & url);
    Q_INVOKABLE static QString toString   (const QUrl & url);

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE static QByteArray readAll(const QString & fileName);

    Q_INVOKABLE static bool tryAppend(const QString & fileName);

    //---------------------------------------------------------------------------------------------
    // Files

    static bool renameFile(const QString & oldPath, const QString & newPath);

    static bool deleteFile(const QString & fileName);

    static bool copy     (const QString     & fileName,  const QString & newName);
    static bool copyFiles(const QStringList & fileNames, const QString & destination);

    //---------------------------------------------------------------------------------------------
    // Directories

    static bool createFolder(const QString & path);

    static bool moveFolder(const QString & oldPath, const QString & newPath);

    static bool deleteFolder       (const QString & path, bool recursive = true);
    static bool deleteFolderContent(const QString & path, bool recursive = true);

    static QFileInfoList recursiveEntryInfoList(const QString & path);

signals:
    void pathStorageChanged();

    void cacheChanged();

private slots:
    void onCheckWatchers();

public: // Properties
    QString pathStorage() const;
    void    setPathStorage(const QString & path);

    WCache * cache() const;
    void     setCache(WCache * cache);

private:
    W_DECLARE_PRIVATE   (WControllerFile)
    W_DECLARE_CONTROLLER(WControllerFile)

    Q_PRIVATE_SLOT(d_func(), void onCheckWatchers())

    friend class WFileWatcher;
    friend class WFileWatcherPrivate;
    friend class WLocalObject;
    friend class WLocalObjectPrivate;
};

#include <private/WControllerFile_p>

#endif // SK_NO_CONTROLLERFILE
#endif // WCONTROLLERFILE_H
