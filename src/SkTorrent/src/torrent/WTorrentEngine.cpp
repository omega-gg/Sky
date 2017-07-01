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

#include "WTorrentEngine.h"

#ifndef SK_NO_TORRENTENGINE

// Qt includes
#include <QCoreApplication>
#include <QDir>

// libtorrent includes
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/extensions/ut_pex.hpp>

// Boost includes
#include <boost/bind.hpp>

// Sk includes
#include <WControllerFile>
#include <WControllerNetwork>

// Forward declarations
class WTorrentStorage;

//-------------------------------------------------------------------------------------------------
// Static variables

static const QString TORRENTENGINE_NAME = "1";

static const int TORRENTENGINE_BLOCK = 16000;

static const int TORRENTENGINE_PRIORITY_COUNT = 10;

static const int TORRENTENGINE_TIMEOUT = 100;

//=================================================================================================
// WTorrentStore
//=================================================================================================

class WTorrentStore : public QObject
{
    Q_OBJECT

public:
    WTorrentStore();

public: // Variables
    QList<WTorrentStorage *> items;
};

Q_GLOBAL_STATIC(WTorrentStore, torrentStore);

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

WTorrentStore::WTorrentStore() : QObject() {}

//=================================================================================================
// WTorrentStorage
//=================================================================================================

class WTorrentStorage : public default_storage
{
public:
    WTorrentStorage(storage_params const & params);

public: // Static functions
    static storage_interface * create(storage_params const & params);

public: // default_storage reimplementation
    /* virtual */ int writev(file::iovec_t const * bufs, int num_bufs,
                                                         int piece,
                                                         int offset,
                                                         int flags, storage_error & ec);

public: // Variables
    WTorrentEngine * engine;

    unsigned int hash;
};

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

WTorrentStorage::WTorrentStorage(storage_params const & params) : default_storage(params) {}

//-------------------------------------------------------------------------------------------------
// Static functions
//-------------------------------------------------------------------------------------------------

/* static */ storage_interface * WTorrentStorage::create(storage_params const & params)
{
    WTorrentStorage * storage = new WTorrentStorage(params);

    torrentStore()->items.append(storage);

    return storage;
}

//-------------------------------------------------------------------------------------------------
// default_storage reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ int WTorrentStorage::writev(file::iovec_t const * bufs, int num_bufs,
                                                                      int piece,
                                                                      int offset,
                                                                      int flags,
                                                                      storage_error & ec)
{
    int length = default_storage::writev(bufs, num_bufs, piece, offset, flags, ec);

    QCoreApplication::postEvent(engine, new WTorrentEngineBuffer(hash, piece, offset, length));

    return length;
}

//=================================================================================================
// WTorrentEnginePrivate
//=================================================================================================

WTorrentEnginePrivate::WTorrentEnginePrivate(WTorrentEngine * p) : WPrivate(p) {}

//-------------------------------------------------------------------------------------------------

void WTorrentEnginePrivate::init(QThread * thread)
{
    Q_Q(WTorrentEngine);

    session = NULL;

    if (thread)
    {
        q->moveToThread(thread);

        timer.moveToThread(thread);
    }

    QObject::connect(&timer, SIGNAL(timeout()), q, SLOT(onUpdate()));

    QCoreApplication::postEvent(q, new QEvent(static_cast<QEvent::Type>
                                              (WTorrentEnginePrivate::EventCreate)),
                                Qt::HighEventPriority * 100);
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WTorrentEnginePrivate::applyBuffer(WTorrentData * data, int piece, int block, int length)
{
    QBitArray * blocks = &(data->blocks);

    int blockCount = data->blockCount;

    int current = piece * blockCount + block;

    if (data->index == piece && data->block == block)
    {
        block   += length;
        current += length;

        while (length)
        {
            blocks->setBit(current);

            length--;
        }

        while (block < blockCount)
        {
            if (blocks->at(current) == false)
            {
                qDebug("BLOCK COMPLETE %d", block);

                data->block = block;

                qint64 buffer = piece * data->sizePiece + block * TORRENTENGINE_BLOCK;

                QCoreApplication::postEvent(data->torrent,
                                            new WTorrentEventValue(WTorrent::EventBuffer, buffer));

                return;
            }

            block++;

            current++;
        }
    }
    else
    {
        while (length)
        {
            blocks->setBit(current);

            length--;
        }

        block   = 0;
        current = piece * blockCount;

        while (block < blockCount)
        {
            if (data->blocks.at(current) == false) return;

            block++;

            current++;
        }
    }

    applyPiece(data, piece);
}

void WTorrentEnginePrivate::applyPiece(WTorrentData * data, int piece)
{
    qDebug("APPLY PIECE %d", piece);

    QBitArray * pieces = &(data->pieces);

    pieces->setBit(piece);

    int index   = data->index;
    int current = data->current;

    int count = pieces->count();

    if (index == piece)
    {
        index++;

        while (index < count && pieces->at(index))
        {
            index++;
        }

        data->index   = index;
        data->current = data->begin + index;

        if (index == count)
        {
            qDebug("FILE COMPLETE");

            QCoreApplication::postEvent(data->torrent,
                                        new WTorrentEventValue(WTorrent::EventBuffer, data->size));

            return;
        }

        QBitArray * blocks = &(data->blocks);

        int block = 0;

        int current = index * data->blockCount;

        while (current < blocks->count() && blocks->at(current))
        {
            block++;

            current++;
        }

        data->block = block;

        qint64 buffer = index * data->sizePiece + block * TORRENTENGINE_BLOCK;

        QCoreApplication::postEvent(data->torrent,
                                    new WTorrentEventValue(WTorrent::EventBuffer, buffer));
    }

    const torrent_handle & handle = data->handle;

    int priority = TORRENTENGINE_PRIORITY_COUNT;

    int deadline = 1;

    while (priority && index < count)
    {
        if (pieces->at(index) == 0)
        {
            handle.set_piece_deadline(current, deadline++);

            priority--;
        }

        index++;

        current++;
    }
}

WTorrentData * WTorrentEnginePrivate::getTorrentData(WTorrent * torrent) const
{
    QHashIterator<unsigned int, WTorrentData *> i(torrents);

    while (i.hasNext())
    {
        i.next();

        WTorrentData * data = i.value();

        if (data->torrent == torrent)
        {
            return data;
        }
    }

    return NULL;
}

//-------------------------------------------------------------------------------------------------
// Private events
//-------------------------------------------------------------------------------------------------

void WTorrentEnginePrivate::events()
{
    std::vector<alert *> alerts;

    session->pop_alerts(&alerts);

    std::vector<alert *>::iterator i = alerts.begin();

    while (i != alerts.end())
    {
        alert * alert = *i;

        int type = alert->type();

        if (type == state_changed_alert::alert_type)
        {
            Q_Q(WTorrentEngine);

            state_changed_alert * event = alert_cast<state_changed_alert>(alert);

            QCoreApplication::postEvent(q, new WTorrentEngineValue(EventState,
                                                                   hash_value(event->handle),
                                                                   event->state));
        }
        else if (type == state_update_alert::alert_type)
        {
            state_update_alert * event = alert_cast<state_update_alert>(alert);

            std::vector<torrent_status> & status = event->status;

            if (status.empty()) return;

            Q_Q(WTorrentEngine);

            QList<WTorrentProgress> list;

            std::vector<torrent_status>::iterator i = status.begin();

            while (i != status.end())
            {
                const torrent_status & status = *i;

                WTorrentProgress progress;

                progress.hash = hash_value(status.handle);

                progress.progress = status.total_done;

                progress.download = status.download_rate;
                progress.upload   = status.upload_rate;

                progress.seeds = status.num_seeds;
                progress.peers = status.num_peers;

                list.append(progress);

                i++;
            }

            QCoreApplication::postEvent(q, new WTorrentEngineProgress(list));
        }
        else if (type == torrent_removed_alert::alert_type)
        {
            Q_Q(WTorrentEngine);

            torrent_removed_alert * event = alert_cast<torrent_removed_alert>(alert);

            QCoreApplication::postEvent(q, new WTorrentEngineHandle(EventRemoved,
                                                                    hash_value(event->handle)));
        }
        else if (type == torrent_error_alert::alert_type)
        {
            Q_Q(WTorrentEngine);

            torrent_error_alert * event = alert_cast<torrent_error_alert>(alert);

            QString message = QString::fromStdString(event->message());

            QCoreApplication::postEvent(q, new WTorrentEngineValue(EventError,
                                                                   hash_value(event->handle),
                                                                   message));
        }

        i++;
    }
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WTorrentEnginePrivate::onUpdate()
{
    session->post_torrent_updates(torrent_handle::query_accurate_download_counters);
}

//-------------------------------------------------------------------------------------------------

void WTorrentEnginePrivate::onDeleteFolder()
{
    Q_Q(WTorrentEngine);

    QString path = deletePaths.takeFirst();

    WControllerFileReply * reply = wControllerFile->startDeleteFolder(path);

    QObject::connect(reply, SIGNAL(actionComplete(bool)), q, SLOT(onDeleteId()));
}

void WTorrentEnginePrivate::onDeleteId()
{
    int id = deleteIds.takeFirst();

    ids.removeOne(id);
}

//=================================================================================================
// WTorrentEngine
//=================================================================================================

WTorrentEngine::WTorrentEngine(QThread * thread, QObject * parent)
    : QObject(parent), WPrivatable(new WTorrentEnginePrivate(this))
{
    Q_D(WTorrentEngine); d->init(thread);
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WTorrentEngine::load(WTorrent * torrent, const QByteArray & data,
                                                                const QString    & path)
{
    QVariantList variants;

    variants.append(data);
    variants.append(path);

    variants.append(torrent->index());
    variants.append(torrent->mode ());

    QCoreApplication::postEvent(this, new WTorrentEngineAction(WTorrentEnginePrivate::EventAdd,
                                                               torrent, variants));
}

/* Q_INVOKABLE */ void WTorrentEngine::remove(WTorrent * torrent, bool deleteFiles)
{
    QCoreApplication::postEvent(this, new WTorrentEngineAction(WTorrentEnginePrivate::EventRemove,
                                                               torrent, deleteFiles));
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WTorrentEngine::deleteInstance()
{
    QCoreApplication::postEvent(this, new QEvent(static_cast<QEvent::Type>
                                                 (WTorrentEnginePrivate::EventClear)),
                                Qt::HighEventPriority * 100);
}

//-------------------------------------------------------------------------------------------------
// Events
//-------------------------------------------------------------------------------------------------

/* virtual */ bool WTorrentEngine::event(QEvent * event)
{
    Q_D(WTorrentEngine);

    QEvent::Type type = event->type();

    if (type == static_cast<QEvent::Type> (WTorrentEnginePrivate::EventCreate))
    {
        settings_pack pack;

        pack.set_int(settings_pack::alert_mask, alert::error_notification   |
                                                alert::storage_notification |
                                                alert::status_notification  |
                                                alert::progress_notification);

        pack.set_bool(settings_pack::enable_dht, true);
        pack.set_bool(settings_pack::enable_lsd, true);

        pack.set_bool(settings_pack::enable_upnp,   true);
        pack.set_bool(settings_pack::enable_natpmp, true);

        //pack.set_bool(settings_pack::smooth_connects, false);

        pack.set_int(settings_pack::connection_speed, 500);

        //pack.set_int(settings_pack::max_failcount,      1);
        pack.set_int(settings_pack::min_reconnect_time, 1);

        //pack.set_int(settings_pack::peer_timeout,         1);
        pack.set_int(settings_pack::peer_connect_timeout, 1);

        pack.set_bool(settings_pack::announce_to_all_tiers,    true);
        pack.set_bool(settings_pack::announce_to_all_trackers, true);

        //pack.set_bool(settings_pack::prioritize_partial_pieces, true);

        d->session = new session(pack);

        boost::function<void()> alert(boost::bind(&WTorrentEnginePrivate::events, d));

        d->session->set_alert_notify(alert);

        d->session->add_dht_router(std::make_pair(std::string("dht.libtorrent.org"),     25401));
        d->session->add_dht_router(std::make_pair(std::string("router.bittorrent.com"),   6881));
        d->session->add_dht_router(std::make_pair(std::string("router.utorrent.com"),     6881));
        d->session->add_dht_router(std::make_pair(std::string("dht.transmissionbt.com"),  6881));
        d->session->add_dht_router(std::make_pair(std::string("dht.aelitis.com"),         6881));

        d->session->add_extension(&create_ut_pex_plugin);

        return true;
    }
    else if (d->session == NULL)
    {
        return QObject::event(event);
    }
    else if (type == static_cast<QEvent::Type> (WTorrentEnginePrivate::EventAdd))
    {
        WTorrentEngineAction * eventTorrent = static_cast<WTorrentEngineAction *> (event);

        WTorrent * torrent = eventTorrent->torrent;

        QVariantList variants = eventTorrent->value.toList();

        //-----------------------------------------------------------------------------------------
        // Torrent info

        QByteArray array = variants.at(0).toByteArray();

        boost::shared_ptr<torrent_info> info(new torrent_info(array, array.size()));

        add_torrent_params params;

        int id = d->ids.generateId();

        QString path = variants.at(1).toString() + '/' + QString::number(id);

        params.ti        = info;
        params.save_path = path.toStdString();

        //-----------------------------------------------------------------------------------------
        // Torrent mode

        torrent_handle handle;

        WTorrentData * data = new WTorrentData;

        QStringList paths;

        WTorrent::Mode mode = static_cast<WTorrent::Mode> (variants.at(3).toInt());

        int index = variants.at(2).toInt();

        qint64 size;
        int    sizePiece;

        int begin;
        int end;

        int blockCount;

        if (mode == WTorrent::Stream)
        {
            if (index == -1)
            {
                handle = d->session->add_torrent(params);

                size      = 0;
                sizePiece = 0;

                begin = 0;
                end   = 0;

                blockCount = 0;
            }
            else
            {
                params.storage = WTorrentStorage::create;

                handle = d->session->add_torrent(params);

                int count = info->num_files();

                std::vector<int> files;

                for (int i = 0; i < count; i++)
                {
                    files.push_back(0);
                }

                if (index < count)
                {
                    QString fileName = QString::fromStdString(info->files().file_path(index));

                    QString extension = WControllerNetwork::extractUrlExtension(fileName);

                    fileName = TORRENTENGINE_NAME;

                    if (extension.isEmpty() == false)
                    {
                        fileName.append("." + extension);
                    }

                    paths.push_back(path + '/' + fileName);

                    files[index] = 1;

                    handle.rename_file(index, fileName.toStdString());

                    handle.prioritize_files(files);

                    //-----------------------------------------------------------------------------

                    size = info->files().file_size(index);

                    sizePiece = info->piece_length();

                    blockCount = (sizePiece / TORRENTENGINE_BLOCK);

                    //-----------------------------------------------------------------------------

                    peer_request request = info->map_file(index, 0, 0);

                    begin = request.piece;

                    int length = (request.start + size) / sizePiece;

                    end = begin + qMax(0, length) + 1;

                    /*handle.set_sequential_download(true);

                    std::vector<int> pieces;

                    for (int i = 0; i < begin; i++)
                    {
                        pieces.push_back(0);
                    }

                    for (int i = begin; i < end; i++)
                    {
                        pieces.push_back(1);
                    }

                    for (int i = end; i < info->num_pieces(); i++)
                    {
                        pieces.push_back(0);
                    }

                    handle.prioritize_pieces(pieces);*/

                    //-----------------------------------------------------------------------------

                    int count = TORRENTENGINE_PRIORITY_COUNT;

                    int deadline = 1;

                    int current = begin;
                    int last    = end - 1;

                    while (count && current < last)
                    {
                        handle.set_piece_deadline(current, deadline++);

                        current++;

                        count--;
                    }
                }
                else
                {
                    handle.prioritize_files(files);

                    size      = 0;
                    sizePiece = 0;

                    begin = 0;
                    end   = 0;

                    blockCount = 0;
                }
            }
        }
        else
        {
            handle = d->session->add_torrent(params);

            if (index == -1)
            {
                QString filePath = path + '/';

                const file_storage & storage = info->files();

                for (int i = 0; i < info->num_files(); i++)
                {
                    QString fileName = QString::fromStdString(storage.file_path(i));

                    paths.push_back(filePath + QDir::fromNativeSeparators(fileName));
                }
            }
            else
            {
                std::vector<int> files;

                int count = info->num_files();

                for (int i = 0; i < count; i++)
                {
                    files.push_back(0);
                }

                if (index < count)
                {
                    QString fileName = QString::fromStdString(info->files().file_path(index));

                    paths.push_back(path + '/' + QDir::fromNativeSeparators(fileName));

                    files[index] = 1;
                }

                handle.prioritize_files(files);
            }

            if (mode == WTorrent::Sequential)
            {
                handle.set_sequential_download(true);
            }

            size      = info->total_size();
            sizePiece = 0;

            begin = 0;
            end   = info->num_pieces();

            blockCount = 0;
        }

        //-----------------------------------------------------------------------------------------
        // Torrent add

        int count = end - begin;

        data->id = id;

        data->mode = mode;

        data->path = path;

        data->torrent = torrent;

        data->handle = handle;

        data->size      = size;
        data->sizePiece = sizePiece;

        data->pieces = QBitArray(count);
        data->blocks = QBitArray(count * blockCount);

        data->begin = begin;
        data->end   = end;

        data->index   = 0;
        data->current = begin;

        data->block      = 0;
        data->blockCount = blockCount;

        data->progress = 0;

        d->torrents.insert(hash_value(handle), data);

        d->timer.start(TORRENTENGINE_TIMEOUT);

        QCoreApplication::postEvent(torrent, new WTorrentEventAdd(paths, size, count));

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WTorrentEnginePrivate::EventRemove))
    {
        WTorrentEngineAction * eventTorrent = static_cast<WTorrentEngineAction *> (event);

        WTorrentData * data = d->getTorrentData(eventTorrent->torrent);

        if (data == NULL) return true;

        data->torrent = NULL;

        const torrent_handle & handle = data->handle;

        d->session->remove_torrent(handle);

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WTorrentEnginePrivate::EventRemoved))
    {
        WTorrentEngineHandle * eventTorrent = static_cast<WTorrentEngineHandle *> (event);

        WTorrentData * data = d->torrents.value(eventTorrent->hash);

        if (data == NULL)
        {
            qDebug("EventRemoved: DATA SHOULD NOT BE NULL");
        }

        d->torrents.remove(eventTorrent->hash);

        if (d->torrents.isEmpty())
        {
            d->timer.stop();
        }

        d->deletePaths.append(data->path);
        d->deleteIds  .append(data->id);

        delete data;

        // FIXME libtorrent: Waiting before removing the torrent folder.
        QTimer::singleShot(1000, this, SLOT(onDeleteFolder()));

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WTorrentEnginePrivate::EventState))
    {
        WTorrentEngineValue * eventTorrent = static_cast<WTorrentEngineValue *> (event);

        torrent_status::state_t state
            = static_cast<torrent_status::state_t> (eventTorrent->value.toInt());

        if (state == torrent_status::finished)
        {
            WTorrentData * data = d->torrents.value(eventTorrent->hash);

            if (data == NULL)
            {
                qDebug("EventState: DATA SHOULD NOT BE NULL");
            }

            QCoreApplication::postEvent(data->torrent, new WTorrentEvent(WTorrent::EventFinished));
        }

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WTorrentEnginePrivate::EventProgress))
    {
        WTorrentEngineProgress * eventProgress = static_cast<WTorrentEngineProgress *> (event);

        const QList<WTorrentProgress> & list = eventProgress->list;

        foreach (const WTorrentProgress & item, list)
        {
            WTorrentData * data = d->torrents.value(item.hash);

            if (data == NULL)
            {
                qDebug("EventProgress: DATA SHOULD NOT BE NULL");
            }

            QCoreApplication::postEvent(data->torrent,
                                        new WTorrentEventProgress(item.progress,
                                                                  item.download, item.upload,
                                                                  item.seeds, item.peers));
        }

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WTorrentEnginePrivate::EventBuffer))
    {
        WTorrentEngineBuffer * eventTorrent = static_cast<WTorrentEngineBuffer *> (event);

        WTorrentData * data = d->torrents.value(eventTorrent->hash);

        if (data == NULL || data->torrent == NULL) return true;

        int piece = eventTorrent->piece - data->begin;

        int offset = eventTorrent->offset;

        int block = offset / TORRENTENGINE_BLOCK;

        int length = (offset + eventTorrent->length) / TORRENTENGINE_BLOCK - block;

        d->applyBuffer(data, piece, block, length);

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WTorrentEnginePrivate::EventError))
    {
        WTorrentEngineValue * eventTorrent = static_cast<WTorrentEngineValue *> (event);

        WTorrentData * data = d->torrents.value(eventTorrent->hash);

        if (data == NULL)
        {
            qDebug("EventError: DATA SHOULD NOT BE NULL");
        }

        QCoreApplication::postEvent(data->torrent, new WTorrentEventValue(WTorrent::EventError,
                                                                          eventTorrent->value));

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WTorrentEnginePrivate::EventClear))
    {
        d->timer.stop();

        delete d->session;

        d->session = NULL;

        QHashIterator<unsigned int, WTorrentData *> i(d->torrents);

        while (i.hasNext())
        {
            i.next();

            delete i.value();
        }

        return true;
    }
    else return QObject::event(event);
}

#endif // SK_NO_TORRENTENGINE

#include "WTorrentEngine.moc"
