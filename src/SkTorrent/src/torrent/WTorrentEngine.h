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

class WTorrentEnginePrivate;

class SK_TORRENT_EXPORT WTorrentEngine : public QObject, public WPrivatable
{
    Q_OBJECT

public:
    WTorrentEngine(QThread * thread = NULL, QObject * parent = NULL);

public: // Interface
    Q_INVOKABLE void deleteInstance();

protected: // Events
    /* virtual */ bool event(QEvent * event);

private:
    W_DECLARE_PRIVATE(WTorrentEngine)
};

#endif // SK_NO_TORRENTENGINE
#endif // WTORRENTENGINE_H
