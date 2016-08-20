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
#include <QDir>

// Sk includes
#include <WControllerFile>

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

        if (fileCount)
        {
             return true;
        }
        else return false;
    }

    _size = info.size();

    _lastModified = info.lastModified();

    if (oldModified < _lastModified || oldSize != _size)
    {
        _modified = true;
    }

    if (_type == File) _checked = true;

    fileCount--;

    if (fileCount)
    {
         return true;
    }
    else return false;
}

/* virtual */ void WFileWatch::resetCheck()
{
    _checked  = false;
    _modified = false;
}

//-------------------------------------------------------------------------------------------------
// Protected functions
//-------------------------------------------------------------------------------------------------

void WFileWatch::sendFilesModified(const QString & path, const QStringList & fileNames)
{
    emit _watcher->filesModified(path, fileNames);
}

void WFileWatch::sendFilesCreated(const QString & path, const QStringList & fileNames)
{
    emit _watcher->filesCreated(path, fileNames);
}

void WFileWatch::sendFilesDeleted(const QString & path, const QStringList & fileNames)
{
    emit _watcher->filesDeleted(path, fileNames);
}

//-------------------------------------------------------------------------------------------------

void WFileWatch::sendFoldersModified(const QString & path, const QStringList & fileNames)
{
    emit _watcher->foldersModified(path, fileNames);
}

void WFileWatch::sendFoldersCreated(const QString & path, const QStringList & fileNames)
{
    emit _watcher->foldersCreated(path, fileNames);
}

void WFileWatch::sendFoldersDeleted(const QString & path, const QStringList & fileNames)
{
    emit _watcher->foldersDeleted(path, fileNames);
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
    if (_type == Invalid)
    {
         return false;
    }
    else return true;
}

bool WFileWatch::isFile() const
{
    if (_type == File)
    {
         return true;
    }
    else return false;
}

bool WFileWatch::isDirectory() const
{
    if (_type == Folder)
    {
         return true;
    }
    else return false;
}

//-------------------------------------------------------------------------------------------------

bool WFileWatch::exists() const
{
    if (_size == -1)
    {
         return false;
    }
    else return true;
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
// WFolderWatch
//=================================================================================================

WFolderWatch::WFolderWatch(const QString & path, WFileWatcher * watcher, bool recursive)
    : WFileWatch(Folder, path, watcher)
{
    _recursive = recursive;

    if (recursive) scanFolders();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

bool WFolderWatch::contains(const QString & path) const
{
    for (int i = 0; i < _fileWatchs.count(); i++)
    {
        if (_fileWatchs.at(i).path() == path)
        {
            return true;
        }
    }

    for (int i = 0; i < _folderWatchs.count(); i++)
    {
        if (_folderWatchs.at(i).path() == path)
        {
            return true;
        }
    }

    return false;
}

//-------------------------------------------------------------------------------------------------
// WFileWatch reimplementation
//-------------------------------------------------------------------------------------------------

bool WFolderWatch::checkChange(int & fileCount)
{
    QDateTime oldModified = _lastModified;

    if (WFileWatch::checkChange(fileCount) == false)
    {
        return false;
    }

    if (_recursive)
    {
        for (int i = 0; i < _fileWatchs.count(); i++)
        {
            if (_fileWatchs.at(i).isChecked()) continue;

            if (_fileWatchs[i].checkChange(fileCount) == false)
            {
                return false;
            }
        }

        for (int i = 0; i < _folderWatchs.count(); i++)
        {
            if (_folderWatchs.at(i).isChecked()) continue;

            if (_folderWatchs[i].checkChange(fileCount) == false)
            {
                return false;
            }
        }

        if (oldModified != _lastModified)
        {
            checkDeleted();

            scanFolders();
        }
    }

    _checked = true;

    return true;
}

void WFolderWatch::resetCheck()
{
    for (int i = 0; i < _fileWatchs.count(); i++)
    {
        _fileWatchs[i].resetCheck();
    }

    for (int i = 0; i < _folderWatchs.count(); i++)
    {
        _folderWatchs[i].resetCheck();
    }

   WFileWatch::resetCheck();
}

//-------------------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------------------

void WFolderWatch::scanFolders()
{
    QDir dir(_path);

    QFileInfoList list = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);

    QStringList newDirs;
    QStringList newFiles;

    foreach (QFileInfo info, list)
    {
        if (contains(info.filePath()) || info.isHidden()) continue;

        if (info.isFile())
        {
            WFileWatch file(info.filePath(), _watcher);

            _fileWatchs.append(file);

            newFiles.append(info.fileName());
        }
        else if (info.isDir())
        {
            WFolderWatch dir(info.filePath(), _watcher, true);

            _folderWatchs.append(dir);

            newDirs.append(info.fileName());
        }
    }

    if (newFiles.count())
    {
        sendFilesCreated(_path, newFiles);
    }

    if (newDirs.count())
    {
        sendFoldersCreated(_path, newDirs);
    }
}

void WFolderWatch::checkDeleted()
{
    QStringList deletedFiles;
    QStringList deletedDirs;

    QStringList modifiedFiles;
    QStringList modifiedDirs;

    for (int i = 0; i < _fileWatchs.count(); i++)
    {
        const WFolderWatch & watch = _folderWatchs.at(i);

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

    for (int i = 0; i < _folderWatchs.count(); i++)
    {
        const WFolderWatch & watch = _folderWatchs.at(i);

        if (watch.exists() == false)
        {
            deletedDirs.append(watch.name());

            _folderWatchs.removeAt(i);

            i--;
        }
        else if (watch.isModified())
        {
            modifiedDirs.append(watch.name());
        }
    }

    if (deletedFiles.count()) sendFilesDeleted  (_path, deletedFiles);
    if (deletedDirs .count()) sendFoldersDeleted(_path, deletedDirs);

    if (modifiedFiles.count()) sendFilesModified  (_path, modifiedFiles);
    if (modifiedDirs .count()) sendFoldersModified(_path, modifiedDirs);
}

//-------------------------------------------------------------------------------------------------

int WFolderWatch::getFileIndex(const QString & path) const
{
    for (int i = 0; i < _fileWatchs.count(); i++)
    {
        if (_fileWatchs.at(i).path() == path)
        {
            return i;
        }
    }

    return -1;
}

int WFolderWatch::getFolderIndex(const QString & path) const
{
    for (int i = 0; i < _folderWatchs.count(); i++)
    {
        if (_folderWatchs.at(i).path() == path)
        {
            return i;
        }
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
    Q_Q(WFileWatcher);

    wControllerFile->d_func()->unregisterFileWatcher(q);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WFileWatcherPrivate::init()
{
    checked = false;
}

//-------------------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------------------

void WFileWatcherPrivate::addFile(const QString & path)
{
    Q_Q(WFileWatcher);

    QFileInfo info(path);

    if (info.isFile() == false) return;

    WFileWatch file(path, q);

    fileWatchs.append(file);
}

void WFileWatcherPrivate::addFolder(const QString & path, bool recursive)
{
    Q_Q(WFileWatcher);

    QFileInfo info(path);

    if (info.isDir() == false) return;

    WFolderWatch dir(path, q, recursive);

    folderWatchs.append(dir);
}

//-------------------------------------------------------------------------------------------------

void WFileWatcherPrivate::removePath(const QString & path)
{
    int index = getFolderIndex(path);

    if (index != -1)
    {
        folderWatchs.removeAt(index);
    }

    index = getFileIndex(path);

    if (index != -1)
    {
        fileWatchs.removeAt(index);
    }
}

//-------------------------------------------------------------------------------------------------

int WFileWatcherPrivate::getFileIndex(const QString & path) const
{
    for (int i = 0; i < fileWatchs.count(); i++)
    {
        if (fileWatchs.at(i).path() == path)
        {
            return i;
        }
    }

    return -1;
}

int WFileWatcherPrivate::getFolderIndex(const QString & path) const
{
    for (int i = 0; i < folderWatchs.count(); i++)
    {
        if (folderWatchs.at(i).path() == path)
        {
            return i;
        }
    }

    return -1;
}

//-------------------------------------------------------------------------------------------------

QString WFileWatcherPrivate::getAbsoluteFilePath(const QString & path) const
{
    QFileInfo info(path);

    if (info.exists())
    {
         return info.absoluteFilePath();
    }
    else return QString();
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

    for (int i = 0; i < folderWatchs.count(); i++)
    {
        if (folderWatchs[i].checkChange(fileCount))
        {
            const WFolderWatch & watch = folderWatchs.at(i);

            if (watch.exists() == false)
            {
                emit q->foldersDeleted(watch.absolutePath(), QStringList(watch.name()));

                folderWatchs.removeAt(i);

                i--;
            }
            else if (watch.isModified())
            {
               emit q->foldersModified(watch.absolutePath(), QStringList(watch.name()));
            }
        }
        else return false;
    }

    checked = true;

    return true;
}

void WFileWatcherPrivate::resetCheck()
{
    for (int i = 0; i < fileWatchs.count(); i++)
    {
        fileWatchs[i].resetCheck();
    }

    for (int i = 0; i < folderWatchs.count(); i++)
    {
        folderWatchs[i].resetCheck();
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

void WFileWatcher::addFile(const QString & path)
{
    Q_D(WFileWatcher);

    QString absolutePath = d->getAbsoluteFilePath(path);

    if (absolutePath.isNull() || contains(absolutePath)) return;

    d->addFile(absolutePath);
}

void WFileWatcher::addFolder(const QString & path, bool recursive)
{
    Q_D(WFileWatcher);

    QString absolutePath = d->getAbsoluteFilePath(path);

    if (absolutePath.isNull() || contains(absolutePath)) return;

    d->addFolder(absolutePath, recursive);
}

//-------------------------------------------------------------------------------------------------

void WFileWatcher::removePath(const QString & path)
{
    Q_D(WFileWatcher);

    QString absolutePath = d->getAbsoluteFilePath(path);

    if (absolutePath.isNull() || contains(absolutePath) == false) return;

    d->removePath(absolutePath);
}

//-------------------------------------------------------------------------------------------------

bool WFileWatcher::contains(const QString & path) const
{
    Q_D(const WFileWatcher);

    QString absolutePath = d->getAbsoluteFilePath(path);

    if (absolutePath.isNull())
    {
        return false;
    }

    for (int i = 0; i < d->fileWatchs.count(); i++)
    {
        if (d->fileWatchs.at(i).path() == absolutePath)
        {
            return true;
        }
    }

    for (int i = 0; i < d->folderWatchs.count(); i++)
    {
        if (d->folderWatchs.at(i).path() == absolutePath)
        {
            return true;
        }
    }

    return false;
}
