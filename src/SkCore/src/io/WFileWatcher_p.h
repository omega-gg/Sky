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

#ifndef WFILEWATCHER_P_H
#define WFILEWATCHER_P_H

// Qt includes
#include <QList>

// Sk includes
#include <private/Sk_p>

// Forward declarations
class QStringList;

class SK_CORE_EXPORT WFileWatch
{
public: // Enums
    enum FileType { Invalid = 0, File, Dir };

public:
    WFileWatch(WFileWatcher * watcher);

    WFileWatch(const QString & path, WFileWatcher * watcher);

    WFileWatch(FileType type, const QString & path, WFileWatcher * watcher);

    virtual ~WFileWatch();

public: // Virtual interface
    virtual bool checkChange(int & fileCount);
    virtual void resetCheck();

protected: // Functions
    void request_filesModified(const QString & parentPath, const QStringList & dirNames);
    void request_filesCreated (const QString & parentPath, const QStringList & fileNames);
    void request_filesDeleted (const QString & parentPath, const QStringList & fileNames);

    void request_directoriesModified(const QString & parentPath, const QStringList & dirNames);
    void request_directoriesCreated (const QString & parentPath, const QStringList & dirNames);
    void request_directoriesDeleted (const QString & parentPath, const QStringList & dirNames);

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
    QString _path;
    QString _absolutePath;

    QString _name;

    FileType _type;

    QDateTime _lastModified;

    qint64 _size;

    WFileWatcher * _watcher;

    bool _checked;
    bool _modified;
};

class SK_CORE_EXPORT WDirWatch : public WFileWatch
{
public:
    WDirWatch(const QString & path, WFileWatcher * watcher, bool recursive);

public: // Interface
    bool contains(const QString & path);

public: // WFileWatch reimplementation
    bool checkChange(int & fileCount);
    void resetCheck();

private: // Functions
    void recurseDirectories();
    void checkDeleted();

    int getDirIndex_from_path (const QString & path);
    int getFileIndex_from_path(const QString & path);

private: // Variables
    QList<WFileWatch> _fileWatchs;
    QList<WDirWatch>  _dirWatchs;

    bool _recursive;
};

class SK_CORE_EXPORT WFileWatcherPrivate : public WPrivate
{
public:
    WFileWatcherPrivate(WFileWatcher * p);

    /* virtual */ ~WFileWatcherPrivate();

    void init();

public: // Functions
    void addFilePath(const QString & path);
    void addDirPath (const QString & path, bool recursive);

    void removePath(const QString & path);

    int getDirIndex_from_path (const QString & path);
    int getFileIndex_from_path(const QString & path);

    QString getAbsoluteFilePath(const QString & path) const;

public: // WControllerFile interface
    bool checkChange(int & fileCount);

    void resetCheck();

public: // Variables
    QList<WFileWatch> fileWatchs;
    QList<WDirWatch>  dirWatchs;

    bool checked;

protected:
    W_DECLARE_PUBLIC(WFileWatcher)
};

#endif // WFILEWATCHER_P_H
