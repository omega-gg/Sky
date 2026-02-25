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

#ifndef WCONTROLLERFILE_H
#define WCONTROLLERFILE_H

// Qt includes
#include <QFileInfo>
#include <QUrl>
#ifdef QT_4
#include <QStringList>
#endif

// Sk includes
#include <WController>
#include <WAbstractThreadAction>
#include <WAbstractThreadReply>

#ifndef SK_NO_CONTROLLERFILE

// Forward declarations
namespace QtLP_Private
{
    class QtLockedFile;
};
class WControllerFilePrivate;
class WControllerFileReply;
class WCache;
class WCacheFile;

#ifdef QT_6
Q_MOC_INCLUDE("WCache")
#endif

// Defines
#define wControllerFile WControllerFile::instance()

//=================================================================================================
// WControllerFileAction
//=================================================================================================

class SK_CORE_EXPORT WControllerFileAction : public WAbstractThreadAction
{
    Q_OBJECT

protected: // WAbstractThreadAction reimplementation
    /* virtual */ WAbstractThreadReply * createReply() const;

public: // Properties
    WControllerFileReply * controllerReply();
};

//=================================================================================================
// WControllerFileReply
//=================================================================================================

class SK_CORE_EXPORT WControllerFileReply : public WAbstractThreadReply
{
    Q_OBJECT

public:
    WControllerFileReply() {}

protected: // WAbstractThreadReply reimplementation
    /* virtual */ void onCompleted(bool ok);

signals:
    void complete(bool ok);

private:
    Q_DISABLE_COPY(WControllerFileReply)

    friend class WControllerFileAction;
};

//=================================================================================================
// WControllerFile
//=================================================================================================

class SK_CORE_EXPORT WControllerFile : public WController
{
    Q_OBJECT

    Q_ENUMS(Permission)

    Q_PROPERTY(QString log READ log WRITE setLog NOTIFY logChanged)

    Q_PROPERTY(QtMsgType verbosity READ verbosity WRITE setVerbosity NOTIFY verbosityChanged)

    Q_PROPERTY(QString pathApplication READ pathApplication CONSTANT)

    Q_PROPERTY(QString pathStorage READ pathStorage WRITE setPathStorage NOTIFY pathStorageChanged)

    Q_PROPERTY(QString pathLog READ pathLog NOTIFY pathStorageChanged)

    Q_PROPERTY(WCache * cache READ cache WRITE setCache NOTIFY cacheChanged)

    Q_PROPERTY(int watcherInterval READ watcherInterval WRITE setWatcherInterval
               NOTIFY watcherIntervalChanged)

    Q_PROPERTY(int maxLog READ maxLog WRITE setMaxLog NOTIFY maxLogChanged)

public: // Enums
    enum Permission
    {
        Default   = 0,
        ReadOwner = QFile::ReadOwner, WriteOwner = QFile::WriteOwner, ExeOwner = QFile::ExeOwner,
        ReadUser  = QFile::ReadUser,  WriteUser  = QFile::WriteUser,  ExeUser  = QFile::ExeUser,
        ReadGroup = QFile::ReadGroup, WriteGroup = QFile::WriteGroup, ExeGroup = QFile::ExeGroup,
        ReadOther = QFile::ReadOther, WriteOther = QFile::WriteOther, ExeOther = QFile::ExeOther
    };
    Q_DECLARE_FLAGS(Permissions, Permission)

private:
    WControllerFile();

protected: // Initialize
    /* virtual */ void init();

public: // Interface
    Q_INVOKABLE void initMessageHandler();

    // NOTE: initMessageHandler must be called prior to this function.
    Q_INVOKABLE void writeLog(const QString & text);

    Q_INVOKABLE WCacheFile * getFile(const QString & url,
                                     QObject       * parent = NULL, int maxHost = -1);

    Q_INVOKABLE void reloadFile(const QString & url, int delay   = 60 * 5, // 5 minutes
                                                     int maxHost = -1);

    Q_INVOKABLE WCacheFile * getHttp(const QString & url,
                                     QObject       * parent = NULL, int maxHost = -1);

    Q_INVOKABLE QString getFileUrl(const QString & url);

    Q_INVOKABLE WCacheFile * writeCache(const QString    & url,
                                        const QByteArray & array,
                                        const QString    & extension = QString(),
                                        QObject          * parent    = NULL);

    Q_INVOKABLE void addCache(const QString & url, const QByteArray & array,
                                                   const QString    & extension = QString());

    Q_INVOKABLE void removeCaches(const QStringList & urls);
    Q_INVOKABLE void removeCache (const QString     & url);

    Q_INVOKABLE void waitActions();

    //---------------------------------------------------------------------------------------------
    // Thread actions

    WAbstractThreadReply * startWriteAction(WAbstractThreadAction * action);
    WAbstractThreadReply * startReadAction (WAbstractThreadAction * action);

    //---------------------------------------------------------------------------------------------
    // File actions

    WControllerFileReply * startWriteFile(const QString & fileName, const QByteArray & data);

    WControllerFileReply * startAppendFile(const QString & fileName, const QByteArray & data);

    WControllerFileReply * startRenameFile(const QString & oldPath, const QString & newPath);

    WControllerFileReply * startCopyFile(const QString & fileName,
                                         const QString & newName,
                                         Permissions permissions = Default);

    WControllerFileReply * startDeleteFile(const QString & path);

    //---------------------------------------------------------------------------------------------

    WControllerFileReply * startWriteFiles(const QStringList       & fileNames,
                                           const QList<QByteArray> & datas);

    WControllerFileReply * startAppendFiles(const QStringList       & fileNames,
                                            const QList<QByteArray> & datas);

    WControllerFileReply * startRenameFiles(const QStringList & oldPaths,
                                            const QStringList & newPaths);

    WControllerFileReply * startCopyFiles(const QStringList & fileNames,
                                          const QStringList & newNames,
                                          Permissions permissions = Default);

    WControllerFileReply * startDeleteFiles(const QStringList & paths);

    //---------------------------------------------------------------------------------------------
    // Folder actions

    WControllerFileReply * startCreateFolder       (const QString & path);
    WControllerFileReply * startDeleteFolder       (const QString & path, bool recursive = true);
    WControllerFileReply * startDeleteFolderContent(const QString & path, bool recursive = true);

    WControllerFileReply * startCreatePath(const QString & path);

    //---------------------------------------------------------------------------------------------

    WControllerFileReply * startCreateFolders(const QStringList & paths);

    WControllerFileReply * startDeleteFolders(const QStringList & paths,
                                              bool                recursive = true);

    WControllerFileReply * startDeleteFoldersContent(const QStringList & paths,
                                                     bool                recursive = true);

    WControllerFileReply * startCreatePaths(const QStringList & paths);

public: // Static functions
    // NOTE: Copies files from path to destination with the lowercase 'extension'. We create the
    //       path if it does not exist.
    Q_INVOKABLE static WControllerFileReply * copyFiles(const QString & path,
                                                        const QString & destination,
                                                        const QString & extension,
                                                        bool            asynchronous = true);

    // NOTE: Copies folders from path to destination. We create the path if it does not exist.
    Q_INVOKABLE static WControllerFileReply * copyFolders(const QString & path,
                                                          const QString & destination,
                                                          bool            asynchronous = true);

    Q_INVOKABLE static QString absolute(const QUrl    & url);
    Q_INVOKABLE static QString absolute(const QString & string);

    Q_INVOKABLE static QString fileUrl(const QUrl    & url);
    Q_INVOKABLE static QString fileUrl(const QString & string);

    Q_INVOKABLE static QString filePath(const QUrl    & url);
    Q_INVOKABLE static QString filePath(const QString & string);

    Q_INVOKABLE static QString fileAbsolute(const QUrl    & url);
    Q_INVOKABLE static QString fileAbsolute(const QString & string);

    // NOTE: Returns the file name without the suffix.
    Q_INVOKABLE static QString fileBaseName(const QString & fileName);

    Q_INVOKABLE static QString currentPath   (const QString & fileName);
    Q_INVOKABLE static QString currentFileUrl(const QString & fileName);

    Q_INVOKABLE static QString applicationName();

    Q_INVOKABLE static QString applicationPath   (const QString & fileName);
    Q_INVOKABLE static QString applicationFileUrl(const QString & fileName);

    Q_INVOKABLE static QString folderPath(const QString & fileName);
    Q_INVOKABLE static QString folderUrl (const QString & fileName);

    Q_INVOKABLE static QString folderName(const QString & fileName);

    Q_INVOKABLE static QString toLocalFile(const QString & url);
    Q_INVOKABLE static QString toString   (const QString & url);

#ifndef SK_CONSOLE
    Q_INVOKABLE static QString resolvedUrl(const QObject * object, const QString & url);
#endif

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE static QString pathWritable ();
    Q_INVOKABLE static QString pathDesktop  ();
    Q_INVOKABLE static QString pathDocuments();
    Q_INVOKABLE static QString pathPictures ();
    Q_INVOKABLE static QString pathTemp     ();

    //---------------------------------------------------------------------------------------------

    // NOTE: When the device is not opened we return a default QByteArray.
    Q_INVOKABLE static QByteArray readAll(QIODevice * device);

    Q_INVOKABLE static QByteArray readAll(const QString & fileName);

    Q_INVOKABLE static bool exists   (const QString & fileName);
    Q_INVOKABLE static bool tryAppend(const QString & fileName);

    //---------------------------------------------------------------------------------------------
    // Filters

    Q_INVOKABLE static QString getFilterImage();

    //---------------------------------------------------------------------------------------------
    // Urls

    Q_INVOKABLE static bool urlIsImage(const QString & url);

    Q_INVOKABLE static bool extensionIsImage(const QString & extension);

    //---------------------------------------------------------------------------------------------
    // Files

    static bool tryUnlock(const QtLP_Private::QtLockedFile & file,
                          int                                timeout = 10000); // 10 seconds

    static QByteArray readFile(const QString & fileName);

    static bool writeFile(const QString & fileName, const QByteArray & data);

    static bool appendFile(const QString & fileName, const QByteArray & data);

    static bool renameFile(const QString & oldPath, const QString & newPath);

    static bool copyFile(const QString & fileName,
                         const QString & newName, Permissions permissions = Default);

    static bool deleteFile(const QString & fileName);

    //---------------------------------------------------------------------------------------------
    // Folders

    static bool createFolder(const QString & path);
    static bool createPath  (const QString & path);

    static bool moveFolder(const QString & oldPath, const QString & newPath);

    static bool deleteFolder       (const QString & path, bool recursive = true);
    static bool deleteFolderContent(const QString & path, bool recursive = true);

    static QFileInfoList recursiveEntryInfoList(const QString & path);

    //---------------------------------------------------------------------------------------------
    // QML

    static bool generateQml(const QString     & fileName,
                            const QString     & fileOutput,
                            const QStringList & defines = QStringList());

    static QStringList qmlDefines();

signals:
    void logChanged(const QString & message);

    void verbosityChanged();

    void pathStorageChanged();

    void cacheChanged();

    void watcherIntervalChanged();

    void maxLogChanged();

public: // Properties
    QString log() const;
    void    setLog(const QString & log);

    QtMsgType verbosity() const;
    void      setVerbosity(QtMsgType verbosity);

    QString pathApplication() const;
    QString pathStorage    () const;
    QString pathLog        () const;

    void setPathStorage(const QString & path);

    WCache * cache() const;
    void     setCache(WCache * cache);

    int  watcherInterval() const;
    void setWatcherInterval(int interval);

    int  maxLog() const;
    void setMaxLog(int max);

private:
    W_DECLARE_PRIVATE   (WControllerFile)
    W_DECLARE_CONTROLLER(WControllerFile)

    Q_PRIVATE_SLOT(d_func(), void onLog(const QString &))

    Q_PRIVATE_SLOT(d_func(), void onWriteLog())

    Q_PRIVATE_SLOT(d_func(), void onCheckWatchers())

    friend class WControllerApplicationPrivate;
    friend class WFileWatcher;
    friend class WFileWatcherPrivate;
    friend class WLocalObject;
    friend class WLocalObjectPrivate;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(WControllerFile::Permissions)

#include <private/WControllerFile_p>

#endif // SK_NO_CONTROLLERFILE
#endif // WCONTROLLERFILE_H
