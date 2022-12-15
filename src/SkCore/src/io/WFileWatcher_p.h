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

#ifndef WFILEWATCHER_P_H
#define WFILEWATCHER_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

// Qt includes
#include <QList>

// Private includes
#include <private/Sk_p>

#ifndef SK_NO_FILEWATCHER

//-------------------------------------------------------------------------------------------------
// WFileWatch
//-------------------------------------------------------------------------------------------------

class SK_CORE_EXPORT WFileWatch
{
public: // Enums
    enum FileType { Invalid, File, Folder };

public:
    WFileWatch(WFileWatcher * watcher);
    WFileWatch(WFileWatcher * watcher, const QString & path);
    WFileWatch(WFileWatcher * watcher, const QString & path, FileType type);

    virtual ~WFileWatch();

public: // Virtual interface
    virtual bool checkChange(int & fileCount);
    virtual void resetCheck ();

protected: // Functions
    void sendFilesModified(const QString & path, const QStringList & fileNames);
    void sendFilesCreated (const QString & path, const QStringList & fileNames);
    void sendFilesDeleted (const QString & path, const QStringList & fileNames);

    void sendFoldersModified(const QString & path, const QStringList & fileNames);
    void sendFoldersCreated (const QString & path, const QStringList & fileNames);
    void sendFoldersDeleted (const QString & path, const QStringList & fileNames);

public: // Properties
    QString path()         const;
    QString absolutePath() const;

    QString name() const;

    FileType type() const;

    bool isValid    () const;
    bool isFile     () const;
    bool isDirectory() const;

    bool exists() const;

    bool isChecked () const;
    bool isModified() const;

protected: // Variables
    WFileWatcher * _watcher;

    QString _path;
    QString _absolutePath;

    QString _name;

    FileType _type;

    QDateTime _lastModified;

    qint64 _size;

    bool _checked;
    bool _modified;
};

//-------------------------------------------------------------------------------------------------
// WFolderWatch
//-------------------------------------------------------------------------------------------------

class SK_CORE_EXPORT WFolderWatch : public WFileWatch
{
public:
    WFolderWatch(WFileWatcher * watcher, const QString & path, bool recursive);

public: // Interface
    bool contains(const QString & path) const;

public: // WFileWatch reimplementation
    bool checkChange(int & fileCount);
    void resetCheck ();

private: // Functions
    void scanFolders();

    void checkFolder();

    int getFileIndex  (const QString & path) const;
    int getFolderIndex(const QString & path) const;

private: // Variables
    QList<WFileWatch>   _fileWatchs;
    QList<WFolderWatch> _folderWatchs;

    bool _recursive;
};

//-------------------------------------------------------------------------------------------------
// WFileWatcherPrivate
//-------------------------------------------------------------------------------------------------

class SK_CORE_EXPORT WFileWatcherPrivate : public WPrivate
{
public:
    WFileWatcherPrivate(WFileWatcher * p);

    /* virtual */ ~WFileWatcherPrivate();

    void init();

public: // Functions
    void addFile  (const QString & path);
    void addFolder(const QString & path, bool recursive);

    void removePath(const QString & path);

    int getFileIndex  (const QString & path) const;
    int getFolderIndex(const QString & path) const;

    QString getAbsoluteFilePath(const QString & path) const;

public: // WControllerFile interface
    bool checkChange(int & fileCount);

    void resetCheck();

public: // Variables
    QList<WFileWatch>   fileWatchs;
    QList<WFolderWatch> folderWatchs;

    bool checked;

protected:
    W_DECLARE_PUBLIC(WFileWatcher)
};

#endif // SK_NO_FILEWATCHER
#endif // WFILEWATCHER_P_H
