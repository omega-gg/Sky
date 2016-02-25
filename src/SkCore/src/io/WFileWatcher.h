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

#ifndef WFILEWATCHER_H
#define WFILEWATCHER_H

// Qt includes
#include <QObject>
#include <QDateTime>

// Sk includes
#include <Sk>

class WFileWatcherPrivate;
class QStringList;

class SK_CORE_EXPORT WFileWatcher : public QObject, public WPrivatable
{
    Q_OBJECT

public:
    explicit WFileWatcher(QObject * parent = NULL);

public: // Interface
    void addFilePath(const QString & path);
    void addDirPath (const QString & path, bool recursive = false);

    void removePath(const QString & path);

    bool contains(const QString & path);

signals:
    // Dir
    void directoriesModified (const QString & parentPath, const QStringList & dirNames);
    void directoriesCreated  (const QString & parentPath, const QStringList & dirNames);
    void directoriesDeleted  (const QString & parentPath, const QStringList & dirNames);

    // File
    void filesModified(const QString & parentPath, const QStringList & fileNames);
    void filesCreated (const QString & parentPath, const QStringList & fileNames);
    void filesDeleted (const QString & parentPath, const QStringList & fileNames);

private:
    W_DECLARE_PRIVATE(WFileWatcher)

    friend class WFileWatch;
    friend class WDirWatch;
    friend class WControllerFile;
    friend class WControllerFilePrivate;
};

#endif // WFILEWATCHER_H
