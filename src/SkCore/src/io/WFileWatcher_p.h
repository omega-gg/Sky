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

#ifndef WFILEWATCHER_P_H
#define WFILEWATCHER_P_H

// Qt includes
#include <QList>

// Private includes
#include <private/Sk_p>

//-------------------------------------------------------------------------------------------------
// WFileWatch
//-------------------------------------------------------------------------------------------------

class SK_CORE_EXPORT WFileWatch
{
public: // Enums
    enum FileType { Invalid = 0, File, Folder };

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

#endif // WFILEWATCHER_P_H
