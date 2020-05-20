//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of Sky kit.

    - GNU Lesser General Public License Usage:
    This file may be used under the terms of the GNU Lesser General Public License version 3 as
    published by the Free Software Foundation and appearing in the LICENSE.md file included in the
    packaging of this file. Please review the following information to ensure the GNU Lesser
    General Public License requirements will be met: https://www.gnu.org/licenses/lgpl.html.
*/
//=================================================================================================

#ifndef WFILEWATCHER_H
#define WFILEWATCHER_H

// Qt includes
#include <QObject>
#include <QDateTime>

// Sk includes
#include <Sk>

#ifndef SK_NO_FILEWATCHER

// Forward declarations
class QStringList;
class WFileWatcherPrivate;

class SK_CORE_EXPORT WFileWatcher : public QObject, public WPrivatable
{
    Q_OBJECT

public:
    explicit WFileWatcher(QObject * parent = NULL);

public: // Interface
    Q_INVOKABLE void addFile  (const QString & path);
    Q_INVOKABLE void addFolder(const QString & path, bool recursive = false);

    Q_INVOKABLE void removePath(const QString & path);

    Q_INVOKABLE void clearPaths();

    Q_INVOKABLE bool contains(const QString & path) const;

signals:
    void filesModified(const QString & path, const QStringList & fileNames);
    void filesCreated (const QString & path, const QStringList & fileNames);
    void filesDeleted (const QString & path, const QStringList & fileNames);

    void foldersModified(const QString & path, const QStringList & fileNames);
    void foldersCreated (const QString & path, const QStringList & fileNames);
    void foldersDeleted (const QString & path, const QStringList & fileNames);

private:
    W_DECLARE_PRIVATE(WFileWatcher)

    friend class WControllerFile;
    friend class WControllerFilePrivate;
    friend class WFileWatch;
};

#endif // SK_NO_FILEWATCHER
#endif // WFILEWATCHER_H
