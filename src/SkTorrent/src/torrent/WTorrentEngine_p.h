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
#include <QEvent>
#include <QVariant>
#include <QTimer>

// libtorrent includes
#include <libtorrent/session.hpp>

// Private includes
#include <private/Sk_p>

#ifndef SK_NO_TORRENTENGINE

// Namespaces
using namespace libtorrent;

//-------------------------------------------------------------------------------------------------
// WTorrentData
//-------------------------------------------------------------------------------------------------

struct WTorrentData
{
    WTorrent * torrent;

    torrent_handle handle;

    qint64 size;

    int first;
    int last;
};

//-------------------------------------------------------------------------------------------------
// WTorrentEnginePrivate
//-------------------------------------------------------------------------------------------------

class SK_TORRENT_EXPORT WTorrentEnginePrivate : public WPrivate
{
public: // Enums
    enum EventType
    {
        EventCreate = QEvent::User,
        EventAdd,
        EventRemove,
        EventState,
        EventProgress,
        EventPiece,
        EventFinished,
        EventError,
        EventClear
    };

public:
    WTorrentEnginePrivate(WTorrentEngine * p);

    void init(QThread * thread);

public: // Functions
    WTorrentData * getTorrentData(WTorrent * torrent) const;

public: // Slots
    void onAlert ();
    void onUpdate();

public: // Variables
    session * session;

    QHash<unsigned int, WTorrentData *> torrents;

    QTimer timer;

protected:
    W_DECLARE_PUBLIC(WTorrentEngine)
};

//-------------------------------------------------------------------------------------------------
// WTorrentEngineValue
//-------------------------------------------------------------------------------------------------

class WTorrentEngineValue : public QEvent
{
public:
    WTorrentEngineValue(WTorrentEnginePrivate::EventType type, WTorrent       * torrent,
                                                               const QVariant & value)
        : QEvent(static_cast<QEvent::Type> (type))
    {
        this->torrent = torrent;
        this->value   = value;
    }

public: // Variables
    WTorrent * torrent;

    QVariant value;
};

//-------------------------------------------------------------------------------------------------
// WTorrentEngineHandle
//-------------------------------------------------------------------------------------------------

class WTorrentEngineHandle : public QEvent
{
public:
    WTorrentEngineHandle(WTorrentEnginePrivate::EventType type, unsigned int     hash,
                                                                const QVariant & value)
        : QEvent(static_cast<QEvent::Type> (type))
    {
        this->hash  = hash;
        this->value = value;
    }

public: // Variables
    unsigned int hash;

    QVariant value;
};

//-------------------------------------------------------------------------------------------------
// WTorrentProgress
//-------------------------------------------------------------------------------------------------

struct WTorrentProgress
{
    unsigned int hash;

    qint64 progress;

    int download;
    int upload;

    int seeds;
    int peers;
};

//-------------------------------------------------------------------------------------------------
// WTorrentEngineProgress
//-------------------------------------------------------------------------------------------------

class WTorrentEngineProgress : public QEvent
{
public:
    WTorrentEngineProgress(const QList<WTorrentProgress> & list)
        : QEvent(static_cast<QEvent::Type> (WTorrentEnginePrivate::EventProgress))
    {
        this->list = list;
    }

public: // Variables
    QList<WTorrentProgress> list;
};

#endif // SK_NO_TORRENTENGINE
#endif // WTORRENTENGINE_P_H
