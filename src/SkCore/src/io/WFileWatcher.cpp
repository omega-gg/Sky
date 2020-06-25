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

#include "WFileWatcher.h"

#ifndef SK_NO_FILEWATCHER

// Qt includes
#include <QDir>

// Sk includes
#include <WControllerFile>

//=================================================================================================
// WFileWatch
//=================================================================================================

#include "WFileWatcher_p.h"

WFileWatch::WFileWatch(WFileWatcher * watcher)
{
    _watcher = watcher;

    _type = Invalid;

    _size = -1;

    _checked  = false;
    _modified = false;
}

WFileWatch::WFileWatch(WFileWatcher * watcher, const QString & path)
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

WFileWatch::WFileWatch(WFileWatcher * watcher, const QString & path, FileType type)
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

WFolderWatch::WFolderWatch(WFileWatcher * watcher, const QString & path, bool recursive)
    : WFileWatch(watcher, path, Folder)
{
    _recursive = recursive;

    if (recursive) scanFolders();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

bool WFolderWatch::contains(const QString & path) const
{
    foreach (const WFileWatch & watch, _fileWatchs)
    {
        if (watch.path() == path)
        {
            return true;
        }
    }

    foreach (const WFolderWatch & watch, _folderWatchs)
    {
        if (watch.path() == path)
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

        if (_modified)
        {
            checkFolder();

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

    QStringList folders;
    QStringList files;

    foreach (QFileInfo info, list)
    {
        if (contains(info.filePath()) || info.isHidden()) continue;

        if (info.isFile())
        {
            WFileWatch file(_watcher, info.filePath());

            _fileWatchs.append(file);

            files.append(info.fileName());
        }
        else if (info.isDir())
        {
            WFolderWatch folder(_watcher, info.filePath(), true);

            _folderWatchs.append(folder);

            folders.append(info.fileName());
        }
    }

    if (files.count())
    {
        sendFilesCreated(_path, files);
    }

    if (folders.count())
    {
        sendFoldersCreated(_path, folders);
    }
}

//-------------------------------------------------------------------------------------------------

void WFolderWatch::checkFolder()
{
    QStringList modifiedFiles;
    QStringList modifiedFolders;

    QStringList deletedFiles;
    QStringList deletedFolders;

    for (int i = 0; i < _fileWatchs.count(); i++)
    {
        const WFileWatch & watch = _fileWatchs.at(i);

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
            deletedFolders.append(watch.name());

            _folderWatchs.removeAt(i);

            i--;
        }
        else if (watch.isModified())
        {
            modifiedFolders.append(watch.name());
        }
    }

    if (modifiedFiles.count())
    {
        sendFilesModified(_path, modifiedFiles);
    }

    if (modifiedFolders.count())
    {
        sendFoldersModified(_path, modifiedFolders);
    }

    if (deletedFiles.count())
    {
        sendFilesDeleted(_path, deletedFiles);
    }

    if (deletedFolders.count())
    {
        sendFoldersDeleted(_path, deletedFolders);
    }
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

    WFileWatch file(q, path);

    fileWatchs.append(file);
}

void WFileWatcherPrivate::addFolder(const QString & path, bool recursive)
{
    Q_Q(WFileWatcher);

    QFileInfo info(path);

    if (info.isDir() == false) return;

    WFolderWatch folder(q, path, recursive);

    folderWatchs.append(folder);
}

//-------------------------------------------------------------------------------------------------

void WFileWatcherPrivate::removePath(const QString & path)
{
    int index = getFileIndex(path);

    if (index != -1)
    {
        fileWatchs.removeAt(index);
    }

    index = getFolderIndex(path);

    if (index != -1)
    {
        folderWatchs.removeAt(index);
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

/* Q_INVOKABLE */ void WFileWatcher::addFile(const QString & path)
{
    Q_D(WFileWatcher);

    QString absolutePath = d->getAbsoluteFilePath(path);

    if (absolutePath.isNull() || contains(absolutePath)) return;

    d->addFile(absolutePath);
}

/* Q_INVOKABLE */ void WFileWatcher::addFolder(const QString & path, bool recursive)
{
    Q_D(WFileWatcher);

    QString absolutePath = d->getAbsoluteFilePath(path);

    if (absolutePath.isNull() || contains(absolutePath)) return;

    d->addFolder(absolutePath, recursive);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WFileWatcher::removePath(const QString & path)
{
    Q_D(WFileWatcher);

    QString absolutePath = d->getAbsoluteFilePath(path);

    if (absolutePath.isNull() || contains(absolutePath) == false) return;

    d->removePath(absolutePath);
}

/* Q_INVOKABLE */ void WFileWatcher::clearPaths()
{
    Q_D(WFileWatcher);

    d->fileWatchs  .clear();
    d->folderWatchs.clear();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ bool WFileWatcher::contains(const QString & path) const
{
    Q_D(const WFileWatcher);

    QString absolutePath = d->getAbsoluteFilePath(path);

    if (absolutePath.isNull())
    {
        return false;
    }

    foreach (const WFileWatch & watch, d->fileWatchs)
    {
        if (watch.path() == absolutePath)
        {
            return true;
        }
    }

    foreach (const WFolderWatch & watch, d->folderWatchs)
    {
        if (watch.path() == absolutePath)
        {
            return true;
        }
    }

    return false;
}

#endif // SK_NO_FILEWATCHER
