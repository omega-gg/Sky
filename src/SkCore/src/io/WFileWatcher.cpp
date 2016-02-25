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

#include "WFileWatcher.h"

// Qt includes
#include <QTimer>
#include <QDir>

// Sk includes
#include <WControllerFile>

// Private includes
#include <private/WControllerFile_p>

//=================================================================================================
// WFileWatch
//=================================================================================================

#include "WFileWatcher_p.h"

WFileWatch::WFileWatch(FileType type, const QString & path, WFileWatcher * watcher)
{
    QFileInfo info(path);

    _watcher = watcher;

    _path         = path;
    _absolutePath = info.absolutePath();

    _name = info.fileName();

    _type = type;

    _size = info.size();

    _lastModified = info.lastModified();

    _checked  = false;
    _modified = false;
}

WFileWatch::WFileWatch(const QString & path, WFileWatcher * watcher)
{
    QFileInfo info(path);

    _watcher = watcher;

    _path         = path;
    _absolutePath = info.absolutePath();

    _name = info.fileName();

    _type = File;

    _size = info.size();

    _lastModified = info.lastModified();

    _checked  = false;
    _modified = false;
}

WFileWatch::WFileWatch(WFileWatcher * watcher)
{
    _watcher = watcher;

    _type = Invalid;

    _size = -1;

    _checked  = false;
    _modified = false;
}

//-------------------------------------------------------------------------------------------------

/* virtual */ WFileWatch::~WFileWatch() {}

//-------------------------------------------------------------------------------------------------
// Virtual interface
//-------------------------------------------------------------------------------------------------

/* virtual */ bool WFileWatch::checkChange(int & fileCount)
{
    QDateTime oldModified = _lastModified;

    qint64 oldSize = _size;

    QFileInfo info(_path);

    if (info.exists() == false)
    {
        _size = -1;

        _lastModified = QDateTime();

        _checked = true;

        fileCount--;

        if (fileCount) return true;
        else           return false;
    }

    _size = info.size();

    _lastModified = info.lastModified();

    // We check both modified date and size for greater precision
    if (oldModified < _lastModified || oldSize != _size)
    {
        _modified = true;
//        if (mType == WFileWatch::Dir)
//        {
//            request_directoryModified(mAbsolutePath, mName);
//        }
//        else if (mType == WFileWatch::File)
//        {
//            request_fileModified(mAbsolutePath, mName);
//        }
    }

    if (_type == File) _checked = true;

    fileCount--;

    if (fileCount) return true;
    else           return false;
}

/* virtual */ void WFileWatch::resetCheck()
{
    _checked  = false;
    _modified = false;
}

//-------------------------------------------------------------------------------------------------
// Protected functions
//-------------------------------------------------------------------------------------------------

void WFileWatch::request_filesModified(const QString & parentPath, const QStringList & fileNames)
{
    emit _watcher->filesModified(parentPath, fileNames);
}

void WFileWatch::request_filesCreated(const QString & parentPath, const QStringList & fileNames)
{
    emit _watcher->filesCreated(parentPath, fileNames);
}

void WFileWatch::request_filesDeleted(const QString & parentPath, const QStringList & fileNames)
{
    emit _watcher->filesDeleted(parentPath, fileNames);
}

//-------------------------------------------------------------------------------------------------

void WFileWatch::request_directoriesModified(const QString & parentPath, const QStringList & dirNames)
{
    emit _watcher->directoriesModified(parentPath, dirNames);
}

void WFileWatch::request_directoriesCreated(const QString & parentPath, const QStringList & dirNames)
{
    emit _watcher->directoriesCreated(parentPath, dirNames);
}

void WFileWatch::request_directoriesDeleted(const QString & parentPath, const QStringList & dirNames)
{
    emit _watcher->directoriesDeleted(parentPath, dirNames);
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

QString WFileWatch::path() const
{
    return _path;
}

QString WFileWatch::absolutePath() const
{
    return _absolutePath;
}

//-------------------------------------------------------------------------------------------------

QString WFileWatch::name() const
{
    return _name;
}

//-------------------------------------------------------------------------------------------------

WFileWatch::FileType WFileWatch::type() const
{
    return _type;
}

//-------------------------------------------------------------------------------------------------

bool WFileWatch::isValid() const
{
    if (_type == Invalid) return false;
    else                  return true;
}

bool WFileWatch::isFile() const
{
    if (_type == File) return true;
    else               return false;
}

bool WFileWatch::isDirectory() const
{
    if (_type == Dir) return true;
    else              return false;
}

//-------------------------------------------------------------------------------------------------

bool WFileWatch::exists() const
{
    if (_size == -1) return false;
    else             return true;
}

//-------------------------------------------------------------------------------------------------

bool WFileWatch::isChecked() const
{
    return _checked;
}

bool WFileWatch::isModified() const
{
    return _modified;
}

//=================================================================================================
// WDirWatch
//=================================================================================================

WDirWatch::WDirWatch(const QString & path, WFileWatcher * watcher, bool recursive)
    : WFileWatch(Dir, path, watcher)
{
    _recursive = recursive;

    if (recursive) recurseDirectories();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

bool WDirWatch::contains(const QString & path)
{
    for (int i = 0; i < _dirWatchs.count(); i++)
    {
        if (_dirWatchs.at(i).path() == path) return true;
    }

    for (int i = 0; i < _fileWatchs.count(); i++)
    {
        if (_fileWatchs.at(i).path() == path) return true;
    }

    return false;
}

//-------------------------------------------------------------------------------------------------
// WFileWatch reimplementation
//-------------------------------------------------------------------------------------------------

bool WDirWatch::checkChange(int & fileCount)
{
    QDateTime oldModified = _lastModified;

    if (WFileWatch::checkChange(fileCount) == false)
    {
        return false;
    }

    if (_recursive)
    {
        // Checking all sub-directories
        for (int i = 0; i < _dirWatchs.count(); i++)
        {
            if (_dirWatchs.at(i).isChecked())
            {
                continue;
            }
            else if (_dirWatchs[i].checkChange(fileCount) == false)
            {
                return false;
            }
        }

        // Note: on windows we have to check files even if the directory has not changed
        for (int i = 0; i < _fileWatchs.count(); i++)
        {
            if (_fileWatchs.at(i).isChecked())
            {
                continue;
            }
            else if (_fileWatchs[i].checkChange(fileCount) == false)
            {
                return false;
            }
        }

        // Checking files only if the directory has been modified
        if (oldModified != _lastModified)
        {
            // Checking for deleted files
            checkDeleted();

            // Recurse for new files
            recurseDirectories();
        }
    }

    _checked = true;

    return true;
}

void WDirWatch::resetCheck()
{
   for (int i = 0; i < _dirWatchs.count(); i++)
   {
       _dirWatchs[i].resetCheck();
   }

   for (int i = 0; i < _fileWatchs.count(); i++)
   {
       _fileWatchs[i].resetCheck();
   }

   WFileWatch::resetCheck();
}

//-------------------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------------------

void WDirWatch::recurseDirectories()
{
    QDir dir(_path);

    QFileInfoList list = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);

    QStringList newDirs;
    QStringList newFiles;

    foreach (QFileInfo info, list)
    {
        if (contains(info.filePath()) || info.isHidden())
        {
            continue;
        }
        else if (info.isDir())
        {
            WDirWatch dir(info.filePath(), _watcher, true);

            _dirWatchs.append(dir);

            newDirs.append(info.fileName());
        }
        else if (info.isFile())
        {
            WFileWatch file(info.filePath(), _watcher);

            _fileWatchs.append(file);

            newFiles.append(info.fileName());
        }
    }

    if (newDirs.count())
    {
        request_directoriesCreated(_path, newDirs);
    }

    if (newFiles.count())
    {
        request_filesCreated(_path, newFiles);
    }
}

void WDirWatch::checkDeleted()
{
    QStringList deletedDirs;
    QStringList deletedFiles;

    QStringList modifiedDirs;
    QStringList modifiedFiles;

    for (int i = 0; i < _dirWatchs.count(); i++)
    {
        const WDirWatch & watch = _dirWatchs.at(i);

        if (watch.exists() == false)
        {
            deletedDirs.append(watch.name());

            _dirWatchs.removeAt(i);

            i--;
        }
        else if (watch.isModified())
        {
            modifiedDirs.append(watch.name());
        }
    }

    for (int i = 0; i < _fileWatchs.count(); i++)
    {
        const WDirWatch & watch = _dirWatchs.at(i);

        if (watch.exists() == false)
        {
            deletedFiles.append(watch.name());

            _fileWatchs.removeAt(i);

            i--;
        }
        else if (watch.isModified())
        {
            modifiedFiles.append(watch.name());
        }
    }

    if (deletedDirs .count()) request_directoriesDeleted(_path, deletedDirs);
    if (deletedFiles.count()) request_filesDeleted      (_path, deletedFiles);

    if (modifiedDirs .count()) request_directoriesModified(_path, modifiedDirs);
    if (modifiedFiles.count()) request_filesModified      (_path, modifiedFiles);
}

//-------------------------------------------------------------------------------------------------

int WDirWatch::getDirIndex_from_path(const QString & path)
{
    for (int i = 0; i < _dirWatchs.count(); i++)
    {
        if (_dirWatchs.at(i).path() == path) return i;
    }
    return -1;
}

int WDirWatch::getFileIndex_from_path(const QString & path)
{
    for (int i = 0; i < _fileWatchs.count(); i++)
    {
        if (_fileWatchs.at(i).path() == path) return i;
    }
    return -1;
}

//=================================================================================================
// WFileWatcherPrivate
//=================================================================================================

WFileWatcherPrivate::WFileWatcherPrivate(WFileWatcher * p) : WPrivate(p)
{
    wControllerFile->d_func()->registerFileWatcher(p);
}

WFileWatcherPrivate::~WFileWatcherPrivate()
{
    Q_Q(WFileWatcher); wControllerFile->d_func()->unregisterFileWatcher(q);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WFileWatcherPrivate::init()
{
    checked = false;
}

//-------------------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------------------

void WFileWatcherPrivate::addFilePath(const QString & path)
{
    Q_Q(WFileWatcher);

    QFileInfo info(path);

    if (info.isFile() == false) return;

    WFileWatch file(path, q);

    fileWatchs.append(file);
}

void WFileWatcherPrivate::addDirPath(const QString & path, bool recursive)
{
    Q_Q(WFileWatcher);

    QFileInfo info(path);

    if (info.isDir() == false) return;

    WDirWatch dir(path, q, recursive);

    dirWatchs.append(dir);
}

//-------------------------------------------------------------------------------------------------

void WFileWatcherPrivate::removePath(const QString & path)
{
    int index = getDirIndex_from_path(path);

    if (index != -1) dirWatchs.removeAt(index);

    index = getFileIndex_from_path(path);

    if (index != -1) fileWatchs.removeAt(index);
}

//-------------------------------------------------------------------------------------------------

int WFileWatcherPrivate::getDirIndex_from_path(const QString & path)
{
    for (int i = 0; i < dirWatchs.count(); i++)
    {
        if (dirWatchs.at(i).path() == path) return i;
    }
    return -1;
}

int WFileWatcherPrivate::getFileIndex_from_path(const QString & path)
{
    for (int i = 0; i < fileWatchs.count(); i++)
    {
        if (fileWatchs.at(i).path() == path) return i;
    }
    return -1;
}

//-------------------------------------------------------------------------------------------------

QString WFileWatcherPrivate::getAbsoluteFilePath(const QString & path) const
{
    QFileInfo info(path);

    if (info.exists() == false) return QString();

    return info.absoluteFilePath();
}

//-------------------------------------------------------------------------------------------------
// WControllerFile interface
//-------------------------------------------------------------------------------------------------

bool WFileWatcherPrivate::checkChange(int & fileCount)
{
    Q_Q(WFileWatcher);

    for (int i = 0; i < fileWatchs.count(); i++)
    {
        if (fileWatchs[i].checkChange(fileCount))
        {
            const WFileWatch & watch = fileWatchs.at(i);

            if (watch.exists() == false)
            {
                emit q->filesDeleted(watch.absolutePath(), QStringList(watch.name()));

                fileWatchs.removeAt(i);

                i--;
            }
            else if (watch.isModified())
            {
                emit q->filesModified(watch.absolutePath(), QStringList(watch.name()));
            }
        }
        else return false;
    }

    for (int i = 0; i < dirWatchs.count(); i++)
    {
        if (dirWatchs[i].checkChange(fileCount))
        {
            const WDirWatch & watch = dirWatchs.at(i);

            if (watch.exists() == false)
            {
                emit q->directoriesDeleted(watch.absolutePath(), QStringList(watch.name()));

                dirWatchs.removeAt(i);

                i--;
            }
            else if (watch.isModified())
            {
               emit q->directoriesModified(watch.absolutePath(), QStringList(watch.name()));
            }
        }
        else return false;
    }

    checked = true;

    return true;
}

void WFileWatcherPrivate::resetCheck()
{
    for (int i = 0; i < dirWatchs.count(); i++)
    {
        dirWatchs[i].resetCheck();
    }

    for (int i = 0; i < fileWatchs.count(); i++)
    {
        fileWatchs[i].resetCheck();
    }

    checked = false;
}

//=================================================================================================
// WFileWatcher
//=================================================================================================

/* explicit */ WFileWatcher::WFileWatcher(QObject * parent)
    : QObject(parent), WPrivatable(new WFileWatcherPrivate(this))
{
    Q_D(WFileWatcher); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

void WFileWatcher::addFilePath(const QString & path)
{
    Q_D(WFileWatcher);

    QString absolutePath = d->getAbsoluteFilePath(path);

    if (absolutePath.isNull() || contains(absolutePath)) return;

    d->addFilePath(absolutePath);
}

void WFileWatcher::addDirPath(const QString & path, bool recursive)
{
    Q_D(WFileWatcher);

    QString absolutePath = d->getAbsoluteFilePath(path);

    if (absolutePath.isNull() || contains(absolutePath)) return;

    d->addDirPath(absolutePath, recursive);
}

void WFileWatcher::removePath(const QString & path)
{
    Q_D(WFileWatcher);

    QString absolutePath = d->getAbsoluteFilePath(path);

    if (absolutePath.isNull() || contains(absolutePath) == false) return;

    d->removePath(absolutePath);
}

bool WFileWatcher::contains(const QString & path)
{
    Q_D(WFileWatcher);

    QString absolutePath = d->getAbsoluteFilePath(path);

    if (absolutePath.isNull()) return false;

    for (int i = 0; i < d->dirWatchs.count(); i++)
    {
        if (d->dirWatchs.at(i).path() == absolutePath) return true;
    }

    for (int i = 0; i < d->fileWatchs.count(); i++)
    {
        if (d->fileWatchs.at(i).path() == absolutePath) return true;
    }

    return false;
}
