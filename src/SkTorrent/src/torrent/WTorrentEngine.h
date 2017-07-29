//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkTorrent module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#ifndef WTORRENTENGINE_H
#define WTORRENTENGINE_H

// Qt includes
#include <QObject>

// Sk includes
#include <Sk>

#ifndef SK_NO_TORRENTENGINE

// Forward declarations
class WTorrentEnginePrivate;
class WTorrent;

class SK_TORRENT_EXPORT WTorrentEngine : public QObject, public WPrivatable
{
    Q_OBJECT

    Q_PROPERTY(qint64 sizeMax READ sizeMax WRITE setSizeMax NOTIFY sizeMaxChanged)

public:
    WTorrentEngine(const QString & path, qint64    sizeMax = 1048576 * 1000, // 1 gigabyte
                                         QThread * thread  = NULL,
                                         QObject * parent  = NULL);

public: // Interface
    Q_INVOKABLE void load(WTorrent * torrent, QIODevice * device);

    Q_INVOKABLE void seek(WTorrent * torrent, qint64 position);

    Q_INVOKABLE void remove(WTorrent * torrent, bool deleteFiles = false);

    Q_INVOKABLE void clearCache();

    Q_INVOKABLE void deleteInstance();

protected: // Events
    /* virtual */ bool event(QEvent * event);

signals:
    void sizeMaxChanged();

public: // Properties
    qint64 sizeMax() const;
    void   setSizeMax(qint64 max);

private:
    W_DECLARE_PRIVATE(WTorrentEngine)

    Q_PRIVATE_SLOT(d_func(), void onUpdate())

    Q_PRIVATE_SLOT(d_func(), void onFolderDelete())
    Q_PRIVATE_SLOT(d_func(), void onFolderClear ())

    Q_PRIVATE_SLOT(d_func(), void onSave())
};

#include <private/WTorrentEngine_p>

#endif // SK_NO_TORRENTENGINE
#endif // WTORRENTENGINE_H
