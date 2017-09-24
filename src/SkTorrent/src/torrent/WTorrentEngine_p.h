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
#include <QMutex>
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

// Typedefs
typedef boost::shared_ptr<const torrent_info> TorrentInfo;
typedef boost::shared_ptr<torrent_info>       TorrentInfoPointer;

// Forward declarations
struct WTorrentData;

//-------------------------------------------------------------------------------------------------
// WTorrentItem
//-------------------------------------------------------------------------------------------------

struct WTorrentItem
{
    WTorrentData * data;

    WTorrent * torrent;

    int index;

    WTorrent::Mode mode;

    QStringList paths;

    qint64 size;

    int begin;
    int end;

    int current;

    bool finished;
};

//-------------------------------------------------------------------------------------------------
// WTorrentStream
//-------------------------------------------------------------------------------------------------

struct WTorrentStream : public WTorrentItem
{
    QString fileName;

    int sizePiece;

    int piece;
    int count;

    int block;

    qint64 start;

    qint64 bufferPieces;
    qint64 bufferBlocks;

    qint64 position;
};

//-------------------------------------------------------------------------------------------------
// WTorrentSource
//-------------------------------------------------------------------------------------------------

struct WTorrentSource
{
    int id;

    sha1_hash hash;

    qint64 size;

    QList<QUrl> urls;
};

//-------------------------------------------------------------------------------------------------
// WTorrentData
//-------------------------------------------------------------------------------------------------

struct WTorrentData
{
    WTorrentSource * source;

    QString path;

    int fileCount;

    torrent_handle handle;
    unsigned int   hash;

    int blockCount;

    QBitArray pieces;
    QBitArray blocks;

    QList<WTorrentItem *> items;

    std::vector<int> files;
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
        EventAddMagnet,
        EventAdded,
        EventMetaData,
        EventSeek,
        EventRemove,
        EventRemoveMagnet,
        EventSaved,
        EventProgress,
        EventPiece,
        EventBlock,
        EventFinished,
        EventError,
        EventOptions,
        EventProxy,
        EventSizeMax,
        EventClearSource,
        EventClearCache,
        EventClear
    };

public:
    WTorrentEnginePrivate(WTorrentEngine * p);

    void init(const QString & path, qint64 sizeMax, QThread * thread);

public: // Functions
    void load();
    void save();

    void loadResume(WTorrentData * data, const QString & fileName);

    WTorrentData * createData(TorrentInfoPointer info, const sha1_hash & hash, const QUrl & url);

    void updateData(WTorrentData * data);
    void removeData(WTorrentData * data);

    WTorrentItem * createItem(TorrentInfo info, WTorrentData * data,
                                                WTorrent     * torrent,
                                                int            index,
                                                WTorrent::Mode mode);

    WTorrentStream * createStream(TorrentInfo info, WTorrentData * data,
                                                    WTorrent     * torrent,
                                                    int            index,
                                                    WTorrent::Mode mode);

    void addItem  (const torrent_handle & handle, WTorrentItem   * item);
    void addStream(const torrent_handle & handle, WTorrentStream * stream);

    void selectFile  (WTorrentItem * item);
    void unselectFile(WTorrentItem * item);

    void updateFiles(WTorrentData * data);

    void updateCache(WTorrentData * data);

    void cleanCache();

    bool removeSource(WTorrentSource * source);

    void prioritize(const torrent_handle & handle, WTorrentStream * stream, qint64 position);

    void applyBlock(WTorrentStream * stream, int block);

    void applyPiece(const torrent_handle & handle, WTorrentStream * stream, int current);

    void applyBuffer(WTorrentStream * item);
    void applyFinish(WTorrentItem   * item);

    void applyMagnet(const torrent_handle & handle, WMagnet * magnet);

    WTorrentData * getData(const QUrl      & url)  const;
    WTorrentData * getData(const sha1_hash & hash) const;

    WTorrentSource * getSource(const sha1_hash & hash);

    WTorrentItem   * getItem  (WTorrent * torrent) const;
    WTorrentStream * getStream(WTorrent * torrent) const;

public: // Events
    void events();

public: // Slots
    void onUpdate();

    void onRemove      ();
    void onRemoveSource();

    void onFolderDelete();
    void onFolderClear ();

    void onSave();

public: // Variables
    QMutex mutexA;
    QMutex mutexB;

    libtorrent::session * session;

    QString path;
    QString pathIndex;

    qint64 size;

    qint64  sizeMax;
    qint64 _sizeMax;

    int connections;

    int upload;
    int download;

    QString proxyHost;
    int     proxyPort;

    QString proxyUser;
    QString proxyPassword;

    QList<WTorrentData *> datas;
    QList<WMagnet      *> datasMagnets;

    QHash<unsigned int, WTorrentData *> torrents;
    QHash<unsigned int, WMagnet      *> magnets;

    WListId ids;

    QList<WTorrentSource *> sources;

    QHash<unsigned int, QString> fileNames;

    QHash<QTimer *, WTorrentData *> deleteTorrents;

    QList<WTorrentSource *> deleteSources;

    QList<int>     deleteIds;
    QList<QString> deletePaths;

    QTimer * timerUpdate;
    QTimer * timerSave;

protected:
    W_DECLARE_PUBLIC(WTorrentEngine)
};

//-------------------------------------------------------------------------------------------------
// WTorrentEngineEvent
//-------------------------------------------------------------------------------------------------

class WTorrentEngineEvent : public QEvent
{
public:
    WTorrentEngineEvent(WTorrentEnginePrivate::EventType type, const QVariant & value)
        : QEvent(static_cast<QEvent::Type> (type))
    {
        this->value = value;
    }

public: // Variables
    QVariant value;
};

//-------------------------------------------------------------------------------------------------
// WTorrentEngineItem
//-------------------------------------------------------------------------------------------------

class WTorrentEngineItem : public QEvent
{
public:
    WTorrentEngineItem(WTorrentEnginePrivate::EventType type, WTorrent * torrent)
        : QEvent(static_cast<QEvent::Type> (type))
    {
        this->torrent = torrent;
    }

public: // Variables
    WTorrent * torrent;
};

//-------------------------------------------------------------------------------------------------
// WTorrentEngineAction
//-------------------------------------------------------------------------------------------------

class WTorrentEngineAction : public WTorrentEngineItem
{
public:
    WTorrentEngineAction(WTorrentEnginePrivate::EventType type, WTorrent       * torrent,
                                                                const QVariant & value)
        : WTorrentEngineItem(type, torrent)
    {
        this->value = value;
    }

public: // Variables
    QVariant value;
};

//-------------------------------------------------------------------------------------------------
// WTorrentEngineAdd
//-------------------------------------------------------------------------------------------------

class WTorrentEngineAdd : public WTorrentEngineItem
{
public:
    WTorrentEngineAdd(WTorrent  * torrent,
                      QIODevice * device, const QUrl & url, int index, WTorrent::Mode mode)
        : WTorrentEngineItem(WTorrentEnginePrivate::EventAdd, torrent)
    {
        this->device = device;

        this->url   = url;
        this->index = index;

        this->mode = mode;
    }

public: // Variables
    QIODevice * device;

    QUrl url;
    int  index;

    WTorrent::Mode mode;
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
// WTorrentEngineBlock
//-------------------------------------------------------------------------------------------------

class WTorrentEngineBlock : public WTorrentEngineHandle
{
public:
    WTorrentEngineBlock(unsigned int hash, int piece, int block)
        : WTorrentEngineHandle(WTorrentEnginePrivate::EventBlock, hash)
    {
        this->piece = piece;
        this->block = block;
    }

public: // Variables
    int piece;
    int block;
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

//-------------------------------------------------------------------------------------------------
// WTorrentEngineMagnet
//-------------------------------------------------------------------------------------------------

class WTorrentEngineMagnet : public QEvent
{
public:
    WTorrentEngineMagnet(WTorrentEnginePrivate::EventType type, WMagnet * magnet)
        : QEvent(static_cast<QEvent::Type> (type))
    {
        this->magnet = magnet;
    }

public: // Variables
    WMagnet * magnet;
};

#endif // SK_NO_TORRENTENGINE
#endif // WTORRENTENGINE_P_H
