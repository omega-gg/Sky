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
#include <boost/function.hpp>

// Sk includes
#include <WControllerFile>
#include <WControllerNetwork>

//-------------------------------------------------------------------------------------------------
// Static variables

static const QString TORRENTENGINE_NAME = "1";

static const int TORRENTENGINE_BLOCK = 16000;

static const int TORRENTENGINE_PRIORITY_COUNT = 10;

static const int TORRENTENGINE_INTERVAL = 1000;

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

WTorrentEnginePrivate::WTorrentEnginePrivate(WTorrentEngine * p) : WPrivate(p) {}

//-------------------------------------------------------------------------------------------------

void WTorrentEnginePrivate::init(const QString & path, qint64 sizeMax, QThread * thread)
{
    Q_Q(WTorrentEngine);

    session = NULL;

    this->path = path + '/';

    this->sizeMax = sizeMax;

    maximum = sizeMax;

    if (thread) q->moveToThread(thread);

    QCoreApplication::postEvent(q, new QEvent(static_cast<QEvent::Type>
                                              (WTorrentEnginePrivate::EventCreate)),
                                Qt::HighEventPriority * 100);
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WTorrentEnginePrivate::load()
{
    QFile file(pathIndex);

    if (file.exists() == false) return;

    if (file.open(QIODevice::ReadOnly) == false)
    {
        qWarning("WTorrentEnginePrivate::load: Failed to open index %s.", pathIndex.C_STR);

        return;
    }

    QDataStream stream(&file);

    int count;

    stream >> count;

    while (count)
    {
        int id;

        QUrl url;

        qint64 size;

        stream >> id >> url >> size;

        ids.insertId(id);

        WTorrentSource * source = new WTorrentSource;

        source->id   = id;
        source->url  = url;
        source->size = size;

        sources.append(source);

        this->size += size;

        count--;
    }

    cleanCache();
}

void WTorrentEnginePrivate::save()
{
    if (timerSave->isActive()) return;

    timerSave->start();
}

//-------------------------------------------------------------------------------------------------

int WTorrentEnginePrivate::generateId(const QUrl & url)
{
    foreach (WTorrentSource * source, sources)
    {
        if (source->url == url)
        {
            qDebug("TORRENT CACHED");

            sources.removeOne(source);
            sources.   append(source);

            return source->id;
        }
    }

    return ids.generateId();
}

//-------------------------------------------------------------------------------------------------

bool WTorrentEnginePrivate::addToCache(WTorrentData * data)
{
    QString fileName = data->fileName;

    qint64 size = QFileInfo(fileName).size();

    if (size == 0) return false;

    int id = data->id;

    foreach (WTorrentSource * source, sources)
    {
        if (source->id == id)
        {
            qint64 sourceSize = source->size;

            if (sourceSize == size) return true;

            source->size = size;

            sources.removeOne(source);
            sources.   append(source);

            this->size -= sourceSize;
            this->size += size;

            cleanCache();

            return true;
        }
    }

    if (size < maximum)
    {
        qDebug("TORRENT CACHE");

        this->size += size;

        cleanCache();

        WTorrentSource * source = new WTorrentSource;

        source->id   = id;
        source->url  = data->url;
        source->size = size;

        sources.append(source);

        save();

        return true;
    }
    else
    {
        qWarning("WTorrentEnginePrivate::addToCache: File is too large for cache %s.",
                 fileName.C_STR);

        return false;
    }
}

bool WTorrentEnginePrivate::cleanCache()
{
    bool changed = false;

    while (sources.count() && size >= maximum)
    {
        WTorrentSource * source = sources.takeFirst();

        size -= source->size;

        QHashIterator<unsigned int, WTorrentData *> i(torrents);

        int id = source->id;

        bool toDelete = true;

        while (i.hasNext())
        {
            i.next();

            WTorrentData * data = i.value();

            if (data->id == id)
            {
                toDelete = false;

                break;
            }
        }

        if (toDelete)
        {
            Q_Q(WTorrentEngine);

            deletePaths.append(path + QString::number(id));
            deleteIds  .append(id);

            // FIXME libtorrent: Waiting before removing the torrent folder.
            QTimer::singleShot(1000, q, SLOT(onDeleteFolder()));
        }

        delete source;

        changed = true;
    }

    return changed;
}

//-------------------------------------------------------------------------------------------------

void WTorrentEnginePrivate::prioritize(WTorrentData * data, qint64 position)
{
    int piece = position / data->sizePiece;

    if (piece < 0) return;

    QBitArray * pieces = &(data->pieces);

    int count = pieces->count();

    if (piece >= count) return;

    int index = data->index;

    if (index != piece)
    {
        while (piece < count && pieces->at(piece))
        {
            piece++;
        }

        data->index   = piece;
        data->current = data->begin + piece;
    }

    qint64 sizePiece = (qint64) (piece * data->sizePiece);

    int block;

    if (position > sizePiece)
    {
         block = (position - sizePiece) / TORRENTENGINE_BLOCK;
    }
    else block = 0;

    QBitArray * blocks = &(data->blocks);

    int current = piece * data->blockCount + block;

    while (block < data->blockCount && blocks->at(current))
    {
        block++;

        current++;
    }

    data->block = block;

    qint64 buffer = (qint64) (sizePiece + block * TORRENTENGINE_BLOCK);

    qint64 size = data->size;

    if (buffer < size)
    {
         data->buffer = buffer;
    }
    else data->buffer = size;

    QCoreApplication::postEvent(data->torrent,
                                new WTorrentEventValue(WTorrent::EventBuffer, data->buffer));

    const torrent_handle & handle = data->handle;

    handle.clear_piece_deadlines();

    int priority = TORRENTENGINE_PRIORITY_COUNT;

    int deadline = 1;

    current = data->current;

    while (priority && piece < count)
    {
        if (pieces->at(piece) == false)
        {
            handle.set_piece_deadline(current, deadline++);

            priority--;
        }

        piece++;

        current++;
    }
}

//-------------------------------------------------------------------------------------------------

void WTorrentEnginePrivate::applyBlock(WTorrentData * data, int piece, int block)
{
    QBitArray * blocks = &(data->blocks);

    int blockCount = data->blockCount;

    if (block >= blockCount)
    {
        qDebug("BLOCK IS TOO HIGH");
    }

    int current = piece * blockCount + block;

    blocks->setBit(current);

    if (data->index != piece || data->block != block) return;

    block++;

    current++;

    while (block < blockCount && blocks->at(current))
    {
        block++;

        current++;
    }

    qDebug("BLOCK COMPLETE %d", block);

    if (block < blockCount)
    {
        data->block = block;

        qint64 buffer = (qint64) (piece * data->sizePiece + block * TORRENTENGINE_BLOCK);

        applyBuffer(data, buffer);
    }
    else applyPiece(data, piece);
}

void WTorrentEnginePrivate::applyPiece(WTorrentData * data, int piece)
{
    qDebug("APPLY PIECE %d", piece);

    QBitArray * pieces = &(data->pieces);

    pieces->setBit(piece);

    piece++;

    int count = pieces->count();

    while (piece < count && pieces->at(piece))
    {
        piece++;
    }

    if (piece == count)
    {
        qDebug("FILE COMPLETE");

        data->buffer = data->size;

        QCoreApplication::postEvent(data->torrent,
                                    new WTorrentEventValue(WTorrent::EventBuffer, data->buffer));

        return;
    }

    QBitArray * blocks = &(data->blocks);

    int block      = 0;
    int blockCount = data->blockCount;

    int current = piece * blockCount;

    while (block < blockCount && blocks->at(current))
    {
        block++;

        current++;
    }

    if (block == blockCount)
    {
        applyPiece(data, piece);

        return;
    }

    data->index   = piece;
    data->current = data->begin + piece;

    data->block = block;

    qint64 buffer = (qint64) (piece * data->sizePiece + block * TORRENTENGINE_BLOCK);

    applyBuffer(data, buffer);

    const torrent_handle & handle = data->handle;

    int priority = TORRENTENGINE_PRIORITY_COUNT;

    int deadline = 1;

    current = data->current;

    while (priority && piece < count)
    {
        if (pieces->at(piece) == false)
        {
            handle.set_piece_deadline(current, deadline++);

            priority--;
        }

        piece++;

        current++;
    }
}

//-------------------------------------------------------------------------------------------------

void WTorrentEnginePrivate::applyBuffer(WTorrentData * data, qint64 buffer)
{
    qint64 size = data->size;

    if (buffer > size) buffer = size;

    if (data->buffer >= buffer) return;

    data->buffer = buffer;

    QCoreApplication::postEvent(data->torrent,
                                new WTorrentEventValue(WTorrent::EventBuffer, buffer));
}

//-------------------------------------------------------------------------------------------------

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
        else if (type == block_finished_alert::alert_type)
        {
            Q_Q(WTorrentEngine);

            block_finished_alert * event = alert_cast<block_finished_alert>(alert);

            QCoreApplication::postEvent(q, new WTorrentEngineBlock(hash_value(event->handle),
                                                                   event->piece_index,
                                                                   event->block_index));

        }
        else if (type == piece_finished_alert::alert_type)
        {
            Q_Q(WTorrentEngine);

            piece_finished_alert * event = alert_cast<piece_finished_alert>(alert);

            QCoreApplication::postEvent(q, new WTorrentEngineValue(EventPiece,
                                                                   hash_value(event->handle),
                                                                   // Clang can't figure out how to convert a libtorrent piece_index_t into a QVariant, so we help it out, with an explicit cast.
                                                                   (std::int32_t)event->piece_index));
        }
        else if (type == save_resume_data_alert::alert_type)
        {
            Q_Q(WTorrentEngine);

            save_resume_data_alert * event = alert_cast<save_resume_data_alert>(alert);

            QCoreApplication::postEvent(q, new WTorrentEngineHandle(EventSaved,
                                                                    hash_value(event->handle)));
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

//-------------------------------------------------------------------------------------------------

void WTorrentEnginePrivate::onSave()
{
    QFile file(pathIndex);

    if (file.open(QIODevice::WriteOnly) == false)
    {
        qWarning("WTorrentEnginePrivate::onSave: Failed to open index %s.", pathIndex.C_STR);

        return;
    }

    QDataStream stream(&file);

    stream << sources.count();

    foreach (WTorrentSource * source, sources)
    {
        stream << source->id << source->url << (qint64) source->size;
    }
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

WTorrentEngine::WTorrentEngine(const QString & path, qint64 sizeMax, QThread * thread,
                                                                     QObject * parent)
    : QObject(parent), WPrivatable(new WTorrentEnginePrivate(this))
{
    Q_D(WTorrentEngine); d->init(path, sizeMax, thread);
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WTorrentEngine::load(WTorrent * torrent, const QByteArray & data)
{
    QVariantList variants;

    variants.append(data);

    variants.append(torrent->url  ());
    variants.append(torrent->index());

    variants.append(torrent->mode());

    QCoreApplication::postEvent(this, new WTorrentEngineAction(WTorrentEnginePrivate::EventAdd,
                                                               torrent, variants));
}

/* Q_INVOKABLE */ void WTorrentEngine::seek(WTorrent * torrent, qint64 position)
{
    QCoreApplication::postEvent(this, new WTorrentEngineAction(WTorrentEnginePrivate::EventSeek,
                                                               torrent, position));
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

        pack.set_int(settings_pack::connection_speed, 400);

        //pack.set_int(settings_pack::max_failcount,      1);
        pack.set_int(settings_pack::min_reconnect_time, 1);

        //pack.set_int(settings_pack::peer_timeout,         1);
        pack.set_int(settings_pack::peer_connect_timeout, 1);

        pack.set_bool(settings_pack::announce_to_all_tiers,    true);
        pack.set_bool(settings_pack::announce_to_all_trackers, true);

        //pack.set_bool(settings_pack::prioritize_partial_pieces, true);

        pack.set_int(settings_pack::stop_tracker_timeout, 1);

        /*pack.set_str(settings_pack::dht_bootstrap_nodes, "dht.libtorrent.org:25401,"
                                                         "router.bittorrent.com:6881,"
                                                         "router.utorrent.com:6881,"
                                                         "dht.transmissionbt.com:6881,"
                                                         "dht.aelitis.com:6881");*/

        d->session = new session(pack);

        dht_settings dht;

        dht.search_branching = 1;

        dht.max_fail_count = 3;

        dht.max_dht_items =  1000;
        dht.max_peers     = 10000;

        d->session->set_dht_settings(dht);

        d->session->add_dht_router(std::make_pair(std::string("dht.libtorrent.org"),     25401));
        d->session->add_dht_router(std::make_pair(std::string("router.bittorrent.com"),   6881));
        d->session->add_dht_router(std::make_pair(std::string("router.utorrent.com"),     6881));
        d->session->add_dht_router(std::make_pair(std::string("dht.transmissionbt.com"),  6881));
        d->session->add_dht_router(std::make_pair(std::string("dht.aelitis.com"),         6881));

        boost::function<void()> alert(boost::bind(&WTorrentEnginePrivate::events, d));

        d->session->set_alert_notify(alert);

        d->session->add_extension(&create_ut_pex_plugin);

        d->pathIndex = d->path + "index";

        d->size = 0;

        d->timerUpdate = new QTimer(this);

        d->timerUpdate->setInterval(TORRENTENGINE_INTERVAL);

        d->timerSave = new QTimer(this);

        d->timerSave->setInterval(TORRENTENGINE_INTERVAL);

        d->timerSave->setSingleShot(true);

        connect(d->timerUpdate, SIGNAL(timeout()), this, SLOT(onUpdate()));
        connect(d->timerSave,   SIGNAL(timeout()), this, SLOT(onSave  ()));

        d->load();

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

        std::shared_ptr<torrent_info> info(new torrent_info(array, array.size()));

        add_torrent_params params;

        params.ti = info;

        //-----------------------------------------------------------------------------------------
        // Torrent mode

        WTorrentData * data = new WTorrentData;

        torrent_handle handle;

        int id;

        QUrl url = variants.at(1).toUrl();

        int index = variants.at(2).toInt();

        QString     path;
        QStringList paths;

        WTorrent::Mode mode = static_cast<WTorrent::Mode> (variants.at(3).toInt());

        qint64 size;
        int    sizePiece;

        int begin;
        int end;

        int blockCount;

        if (mode == WTorrent::Stream)
        {
            id = d->generateId(url);

            path = d->path + QString::number(id);

            params.save_path = path.toStdString();

            handle = d->session->add_torrent(params);

            if (index == -1)
            {
                size      = 0;
                sizePiece = 0;

                begin = 0;
                end   = 0;

                blockCount = 0;
            }
            else
            {
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

                    data->fileName = path + '/' + fileName;

                    paths.append(data->fileName);

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
            id = d->ids.generateId();

            path = d->path + QString::number(id);

            params.save_path = path.toStdString();

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

        data->url = url;

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

        data->buffer = 0;

        d->torrents.insert(hash_value(handle), data);

        d->timerUpdate->start();

        QCoreApplication::postEvent(torrent, new WTorrentEventAdd(paths, size, count));

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WTorrentEnginePrivate::EventSeek))
    {
        WTorrentEngineAction * eventTorrent = static_cast<WTorrentEngineAction *> (event);

        WTorrentData * data = d->getTorrentData(eventTorrent->torrent);

        if (data == NULL) return true;

        qreal position = eventTorrent->value.toLongLong();

        d->prioritize(data, position);

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WTorrentEnginePrivate::EventRemove))
    {
        WTorrentEngineAction * eventTorrent = static_cast<WTorrentEngineAction *> (event);

        WTorrentData * data = d->getTorrentData(eventTorrent->torrent);

        if (data == NULL) return true;

        data->torrent = NULL;

        const torrent_handle & handle = data->handle;

        if (data->mode == WTorrent::Stream)
        {
            handle.save_resume_data();
        }
        else d->session->remove_torrent(handle);

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WTorrentEnginePrivate::EventSaved))
    {
        WTorrentEngineHandle * eventTorrent = static_cast<WTorrentEngineHandle *> (event);

        WTorrentData * data = d->torrents.value(eventTorrent->hash);

        if (data == NULL)
        {
            qDebug("EventSaved: DATA SHOULD NOT BE NULL");
        }

        d->session->remove_torrent(data->handle);

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
            d->timerUpdate->stop();
        }

        if (data->mode != WTorrent::Stream || d->addToCache(data) == false)
        {
            d->deletePaths.append(data->path);
            d->deleteIds  .append(data->id);

            // FIXME libtorrent: Waiting before removing the torrent folder.
            QTimer::singleShot(1000, this, SLOT(onDeleteFolder()));
        }

        delete data;

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
    else if (type == static_cast<QEvent::Type> (WTorrentEnginePrivate::EventBlock))
    {
        WTorrentEngineBlock * eventTorrent = static_cast<WTorrentEngineBlock *> (event);

        WTorrentData * data = d->torrents.value(eventTorrent->hash);

        if (data == NULL || data->mode != WTorrent::Stream) return true;

        int piece = eventTorrent->piece - data->begin;
        int block = eventTorrent->block;

        d->applyBlock(data, piece, block);

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WTorrentEnginePrivate::EventPiece))
    {
        WTorrentEngineValue * eventTorrent = static_cast<WTorrentEngineValue *> (event);

        WTorrentData * data = d->torrents.value(eventTorrent->hash);

        if (data == NULL || data->mode != WTorrent::Stream) return true;

        int piece = eventTorrent->value.toInt() - data->begin;

        if (data->index == piece)
        {
            d->applyPiece(data, piece);
        }
        else data->pieces.setBit(piece);

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
    else if (type == static_cast<QEvent::Type> (WTorrentEnginePrivate::EventSizeMax))
    {
        WTorrentEngineEvent * eventTorrent = static_cast<WTorrentEngineEvent *> (event);

        d->maximum = eventTorrent->value.toLongLong();

        if (d->cleanCache()) d->save();

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WTorrentEnginePrivate::EventClear))
    {
        delete d->session;

        d->session = NULL;

        QHashIterator<unsigned int, WTorrentData *> i(d->torrents);

        while (i.hasNext())
        {
            i.next();

            delete i.value();
        }

        delete d->timerUpdate;
        delete d->timerSave;

        return true;
    }
    else return QObject::event(event);
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

qint64 WTorrentEngine::sizeMax() const
{
    Q_D(const WTorrentEngine); return d->sizeMax;
}

void WTorrentEngine::setSizeMax(qint64 max)
{
    Q_D(WTorrentEngine);

    if (d->sizeMax == max) return;

    d->sizeMax = max;

    QCoreApplication::postEvent(this, new WTorrentEngineEvent(WTorrentEnginePrivate::EventSizeMax,
                                                              max));
}

#endif // SK_NO_TORRENTENGINE
