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

#ifndef WTORRENTENGINE_P_H
#define WTORRENTENGINE_P_H

// Qt includes
#include <QObject>

// libtorrent includes
#include <libtorrent/session.hpp>

// Private includes
#include <private/Sk_p>

#ifndef SK_NO_TORRENTENGINE

// Namespaces
using namespace libtorrent;

class SK_TORRENT_EXPORT WTorrentEnginePrivate : public WPrivate
{
public: // Enums
    enum EventType
    {
        EventCreate = QEvent::User,
        EventClear
    };

public:
    WTorrentEnginePrivate(WTorrentEngine * p);

    void init(QThread * thread);

public: // Functions
    boost::function<void()> const processAlert();

public: // Variables
    libtorrent::session * session;

protected:
    W_DECLARE_PUBLIC(WTorrentEngine)
};

#endif // SK_NO_TORRENTENGINE
#endif // WTORRENTENGINE_P_H
