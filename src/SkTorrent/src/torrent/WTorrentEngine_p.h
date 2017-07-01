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
#include <QBitArray>
#include <QTimer>

// libtorrent includes
#include <libtorrent/session.hpp>

// Sk includes
#include <WControllerTorrent>
#include <WListId>

#ifndef SK_NO_TORRENTENGINE

// Namespaces
using namespace libtorrent;

//-------------------------------------------------------------------------------------------------
// WTorrentData
//-------------------------------------------------------------------------------------------------

struct WTorrentData
{
    int id;

    WTorrent::Mode mode;

    QString path;

    WTorrent * torrent;

    torrent_handle handle;

    qint64 size;
    int    sizePiece;

    QBitArray pieces;
    QBitArray blocks;

    int begin;
    int end;

    int index;
    int current;

    int block;
    int blockCount;

    qint64 progress;
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
        EventRemoved,
        EventState,
        EventProgress,
        EventBuffer,
        EventFinished,
        EventError,
        EventClear
    };

public:
    WTorrentEnginePrivate(WTorrentEngine * p);

    void init(QThread * thread);

public: // Functions
    void applyBuffer(WTorrentData * data, int piece, int block, int length);
    void applyPiece (WTorrentData * data, int piece);

    WTorrentData * getTorrentData(WTorrent * torrent) const;

public: // Events
    void events();

public: // Slots
    void onUpdate();

    void onDeleteFolder();
    void onDeleteId    ();

public: // Variables
    session * session;

    QHash<unsigned int, WTorrentData *> torrents;

    WListId ids;

    QList<QString> deletePaths;
    QList<int>     deleteIds;

    QTimer timer;

protected:
    W_DECLARE_PUBLIC(WTorrentEngine)
};

//-------------------------------------------------------------------------------------------------
// WTorrentEngineAction
//-------------------------------------------------------------------------------------------------

class WTorrentEngineAction : public QEvent
{
public:
    WTorrentEngineAction(WTorrentEnginePrivate::EventType type, WTorrent       * torrent,
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
    WTorrentEngineHandle(WTorrentEnginePrivate::EventType type, unsigned int hash)
        : QEvent(static_cast<QEvent::Type> (type))
    {
        this->hash = hash;
    }

public: // Variables
    unsigned int hash;
};

//-------------------------------------------------------------------------------------------------
// WTorrentEngineValue
//-------------------------------------------------------------------------------------------------

class WTorrentEngineValue : public WTorrentEngineHandle
{
public:
    WTorrentEngineValue(WTorrentEnginePrivate::EventType type, unsigned int     hash,
                                                               const QVariant & value)
        : WTorrentEngineHandle(type, hash)
    {
        this->value = value;
    }

public: // Variables
    QVariant value;
};

//-------------------------------------------------------------------------------------------------
// WTorrentEngineBuffer
//-------------------------------------------------------------------------------------------------

class WTorrentEngineBuffer : public WTorrentEngineHandle
{
public:
    WTorrentEngineBuffer(unsigned int hash, int piece, int offset, int length)
        : WTorrentEngineHandle(WTorrentEnginePrivate::EventBuffer, hash)
    {
        this->piece  = piece;
        this->offset = offset;
        this->length = length;
    }

public: // Variables
    int piece;
    int offset;
    int length;
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
