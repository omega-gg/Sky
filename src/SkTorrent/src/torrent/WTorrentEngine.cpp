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
#include <libtorrent/bencode.hpp>
#include <libtorrent/extensions/ut_pex.hpp>

// Boost includes
#include <boost/bind.hpp>

// C++ includes
#include <fstream>

// Sk includes
#include <WControllerApplication>
#include <WControllerFile>
#include <WControllerNetwork>

// Metatypes
Q_DECLARE_METATYPE(torrent_handle)

//-------------------------------------------------------------------------------------------------
// Static variables

static const int TORRENTENGINE_BLOCK = 16 * 1024; // 16 bytes

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

void WTorrentEnginePrivate::loadResume(WTorrentData * data, const QString & fileName)
{
    QFile file(fileName);

    if (file.open(QIODevice::ReadOnly) == false)
    {
        qWarning("WTorrentEnginePrivate::loadResume: Failed to open file %s.", fileName.C_STR);

        return;
    }

    QString content = Sk::readAscii(file.readAll());

    int index = WControllerTorrent::indexAfter(content, "pieces");

    QString pieces = WControllerTorrent::extractString(content, index);

    QString unfinished = WControllerTorrent::listAfter(content, "unfinished", index);

    qDebug("BLOCK COUNT %d %d [%s] [%s]",
           pieces.length(), unfinished.length(), pieces.C_STR, unfinished.C_STR);
}

//-------------------------------------------------------------------------------------------------

WTorrentData * WTorrentEnginePrivate::createData(TorrentInfoPointer info, const QUrl & url)
{
    WTorrentData * data = new WTorrentData;

    int count = info->num_pieces();

    int blockCount = info->piece_length() / TORRENTENGINE_BLOCK;

    data->blockCount = blockCount;

    data->pieces = QBitArray(count);
    data->blocks = QBitArray(count * blockCount);

    WTorrentSource * source = getSource(url);

    QString path;

    add_torrent_params params;

    if (source)
    {
        QString number = QString::number(source->id);

        path = this->path + number;

        QString fileName = path + "/." + number;

        loadResume(data, fileName);

        std::ifstream stream(fileName.C_STR, std::ios_base::binary);

        stream.unsetf(std::ios_base::skipws);

        params.resume_data.assign(std::istream_iterator<char>(stream),
                                  std::istream_iterator<char>());
    }
    else
    {
        source = new WTorrentSource;

        int id = ids.generateId();

        source->id   = id;
        source->url  = url;
        source->size = 0;

        sources.append(source);

        path = this->path + QString::number(id);

        save();
    }

    int fileCount = info->num_files();

    data->source = source;

    data->path = path;

    data->fileCount = fileCount;

    data->hash = 0;

    data->files = std::vector<int>(fileCount, 0);

    datas.append(data);

    //---------------------------------------------------------------------------------------------

    params.ti        = info;
    params.save_path = path.toStdString();

    session->async_add_torrent(params);

    return data;
}

//-------------------------------------------------------------------------------------------------

WTorrentItem * WTorrentEnginePrivate::createItem(TorrentInfo info, WTorrentData * data,
                                                                   WTorrent     * torrent,
                                                                   int            index,
                                                                   WTorrent::Mode mode)
{
    QStringList paths;

    qint64 size;

    int begin;
    int end;

    if (index == -1)
    {
        const file_storage & storage = info->files();

        QString filePath = data->path + '/';

        for (int i = 0; i < data->fileCount; i++)
        {
            QString fileName = QString::fromStdString(storage.file_path(i));

            paths.push_back(filePath + QDir::fromNativeSeparators(fileName));
        }

        size = info->total_size();

        begin = 0;
        end   = info->num_pieces();
    }
    else if (index < data->fileCount)
    {
        const file_storage & storage = info->files();

        QString fileName = QString::fromStdString(storage.file_path(index));

        paths.push_back(data->path + '/' + QDir::fromNativeSeparators(fileName));

        size = storage.file_size(index);

        peer_request request = info->map_file(index, 0, 0);

        begin = request.piece;

        int length = (request.start + size) / info->piece_length();

        end = begin + qMax(0, length) + 1;
    }
    else
    {
        size = 0;

        begin = 0;
        end   = 0;
    }

    WTorrentItem * item = new WTorrentItem;

    item->data = data;

    item->torrent = torrent;

    item->index = index;

    item->mode = mode;

    item->paths = paths;

    item->size = size;

    item->begin = begin;
    item->end   = end;

    data->items.append(item);

    //---------------------------------------------------------------------------------------------

    QBitArray * pieces = &(data->pieces);

    int current = begin;

    while (current < end && pieces->at(current))
    {
        current++;
    }

    item->current = current;

    if (current == end)
    {
        qDebug("TORRENT ALREADY FINISHED");

        item->finished = true;

        QCoreApplication::postEvent(torrent, new WTorrentEventAdd(paths, size));

        QCoreApplication::postEvent(torrent, new WTorrentEvent(WTorrent::EventFinished));
    }
    else
    {
        qDebug("TORRENT START AT %d", current);

        item->finished = false;

        QCoreApplication::postEvent(torrent, new WTorrentEventAdd(paths, size));
    }

    return item;
}

WTorrentStream * WTorrentEnginePrivate::createStream(TorrentInfo info, WTorrentData * data,
                                                                       WTorrent     * torrent,
                                                                       int            index,
                                                                       WTorrent::Mode mode)
{
    QStringList paths;

    qint64 size;

    int begin;
    int end;

    QString fileName;

    int sizePiece;

    if (index == -1) index = 0;

    if (index < data->fileCount)
    {
        const file_storage & storage = info->files();

        fileName = QString::fromStdString(storage.file_path(index));

        QString extension = WControllerNetwork::extractUrlExtension(fileName);

        fileName = data->path + '/' + QString::number(index + 1);

        if (extension.isEmpty() == false)
        {
            fileName.append("." + extension);
        }

        paths.append(fileName);

        //-----------------------------------------------------------------------------------------

        size = storage.file_size(index);

        peer_request request = info->map_file(index, 0, 0);

        begin = request.piece;

        sizePiece = info->piece_length();

        int length = (request.start + size) / sizePiece;

        end = begin + qMax(0, length) + 1;
    }
    else
    {
        size = 0;

        begin = 0;
        end   = 0;

        sizePiece = 0;
    }

    int count = end - begin;

    WTorrentStream * stream = new WTorrentStream;

    stream->data = data;

    stream->torrent = torrent;

    stream->index = index;

    stream->mode = mode;

    stream->paths = paths;

    stream->size = size;

    stream->begin = begin;
    stream->end   = end;

    //---------------------------------------------------------------------------------------------

    stream->fileName = fileName;

    stream->sizePiece = sizePiece;

    stream->count = count;

    stream->position = 0;

    data->items.append(stream);

    //---------------------------------------------------------------------------------------------

    QBitArray * pieces = &(data->pieces);

    int piece = 0;

    int current = begin;

    while (piece < count && pieces->at(current))
    {
        piece  ++;
        current++;
    }

    stream->piece   = piece;
    stream->current = current;

    if (piece == count)
    {
        qDebug("TORRENT ALREADY FINISHED");

        stream->block = 0;

        stream->buffer = size;

        stream->finished = true;

        QCoreApplication::postEvent(torrent, new WTorrentEventAdd(paths, size));

        QCoreApplication::postEvent(torrent, new WTorrentEventValue(WTorrent::EventBuffer,
                                                                    size));

        QCoreApplication::postEvent(torrent, new WTorrentEvent(WTorrent::EventFinished));
    }
    else
    {
        QBitArray * blocks = &(data->blocks);

        int block      = 0;
        int blockCount = data->blockCount;

        int blockCurrent = current * blockCount;

        while (block < blockCount && blocks->at(blockCurrent))
        {
            block       ++;
            blockCurrent++;
        }

        stream->block = block;

        qint64 buffer = (qint64) (piece * stream->sizePiece + block * TORRENTENGINE_BLOCK);

        stream->buffer = buffer;

        stream->finished = false;

        qDebug("TORRENT START AT %d %d", piece, block);

        QCoreApplication::postEvent(torrent, new WTorrentEventAdd(paths, size));

        if (buffer)
        {
            QCoreApplication::postEvent(torrent, new WTorrentEventValue(WTorrent::EventBuffer,
                                                                        buffer));
        }
    }

    return stream;
}

//-------------------------------------------------------------------------------------------------

void WTorrentEnginePrivate::addItem(const torrent_handle & handle, WTorrentItem * item)
{
    if (item->finished) return;

    if (item->mode == WTorrent::Sequential)
    {
        handle.set_sequential_download(true);
    }
}

void WTorrentEnginePrivate::addStream(const torrent_handle & handle, WTorrentStream * stream)
{
    if (stream->finished) return;

    handle.rename_file(stream->index, stream->fileName.toStdString());

    qint64 position = stream->position;

    if (position)
    {
        prioritize(handle, stream, position);
    }
}

//-------------------------------------------------------------------------------------------------

void WTorrentEnginePrivate::selectFile(WTorrentItem * item)
{
    WTorrentData * data = item->data;

    std::vector<int> & files = data->files;

    int index = item->index;

    if (index != -1)
    {
        if (data->items.count() == 1)
        {
            std::fill(files.begin(), files.end(), 0);
        }

        files[index] = 1;
    }
    else std::fill(files.begin(), files.end(), 1);

    data->handle.prioritize_files(files);
}

void WTorrentEnginePrivate::unselectFile(WTorrentItem * item)
{
    int index = item->index;

    if (index != -1)
    {
        WTorrentData * data = item->data;

        foreach (WTorrentItem * item, data->items)
        {
            if (item->index == index) return;
        }

        std::vector<int> & files = data->files;

        files[index] = 0;

        data->handle.prioritize_files(files);
    }
    else updateFiles(item->data);
}

//-------------------------------------------------------------------------------------------------

void WTorrentEnginePrivate::updateFiles(WTorrentData * data)
{
    qDebug("TORRENT UPDATE FILES");

    std::vector<int> & files = data->files;

    foreach (WTorrentItem * item, data->items)
    {
        int index = item->index;

        if (index == -1)
        {
            std::fill(files.begin(), files.end(), 1);

            data->handle.prioritize_files(files);

            return;
        }
        else files[index] = 1;
    }

    data->handle.prioritize_files(files);
}

//-------------------------------------------------------------------------------------------------

bool WTorrentEnginePrivate::updateCache(WTorrentData * data)
{
    const torrent_handle & handle = data->handle;

    torrent_status status = handle.status(torrent_handle::query_accurate_download_counters);

    qint64 size = status.total_done;

    foreach (WTorrentSource * source, sources)
    {
        if (source->id == data->source->id)
        {
            if (size == 0) return true;

            qint64 sourceSize = source->size;

            if (sourceSize == size) return true;

            this->size -= sourceSize;

            sources.removeOne(source);

            if (size < maximum)
            {
                qDebug("TORRENT RECACHING SOURCE");

                sources.append(source);

                source->size = size;

                this->size += size;

                cleanCache();

                save();

                return true;
            }
            else
            {
                qWarning("WTorrentEnginePrivate::updateCache: File is too large for cache %s.",
                         data->source->url.C_URL);

                save();

                return false;
            }
        }
    }

    qDebug("CACHE THIS SHOULD NOT HAPPEN");

    return false;
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

            if (data->source->id == id)
            {
                toDelete = false;

                break;
            }
        }

        if (toDelete)
        {
            Q_Q(WTorrentEngine);

            qDebug("REMOVING TORRENT %d", id);

            deleteIds  .append(id);
            deletePaths.append(path + QString::number(id));

            // FIXME libtorrent: Waiting before removing the torrent folder.
            QTimer::singleShot(1000, q, SLOT(onFolderDelete()));
        }

        delete source;

        changed = true;
    }

    return changed;
}

//-------------------------------------------------------------------------------------------------

void WTorrentEnginePrivate::prioritize(const torrent_handle & handle,
                                       WTorrentStream       * stream, qint64 position)
{
    int piece = position / stream->sizePiece;

    int count = stream->count;

    if (piece >= count) return;

    //---------------------------------------------------------------------------------------------
    // Pieces

    WTorrentData * data = stream->data;

    QBitArray * pieces = &(data->pieces);

    int current;

    if (stream->piece != piece)
    {
        current = stream->begin + piece;

        while (piece < count && pieces->at(current))
        {
            piece  ++;
            current++;
        }

        stream->piece   = piece;
        stream->current = current;
    }
    else current = stream->current;

    //---------------------------------------------------------------------------------------------
    // Blocks

    if (piece == count)
    {
        stream->block = 0;

        stream->buffer = stream->size;
    }
    else
    {
        int block;

        qint64 sizePiece = (qint64) (piece * stream->sizePiece);

        if (position > sizePiece)
        {
             block = (position - sizePiece) / TORRENTENGINE_BLOCK;
        }
        else block = 0;

        QBitArray * blocks = &(data->blocks);

        int blockCount = data->blockCount;

        int blockCurrent = current * blockCount + block;

        while (block < blockCount && blocks->at(blockCurrent))
        {
            block       ++;
            blockCurrent++;
        }

        stream->block = block;

        qint64 buffer = (qint64) (sizePiece + block * TORRENTENGINE_BLOCK);

        qint64 size = stream->size;

        if (buffer < size)
        {
             stream->buffer = buffer;
        }
        else stream->buffer = size;
    }

    //---------------------------------------------------------------------------------------------
    // Buffer

    QCoreApplication::postEvent(stream->torrent,
                                new WTorrentEventValue(WTorrent::EventBuffer, stream->buffer));

    //---------------------------------------------------------------------------------------------
    // Deadline

    // FIXME
    //handle.clear_piece_deadlines();

    int priority = TORRENTENGINE_PRIORITY_COUNT;

    int deadline = 1;

    while (priority && piece < count)
    {
        if (pieces->at(current) == false)
        {
            handle.set_piece_deadline(current, deadline);

            deadline++;

            priority--;
        }

        piece++;

        current++;
    }
}

//-------------------------------------------------------------------------------------------------

void WTorrentEnginePrivate::applyBlock(const torrent_handle & handle,
                                       WTorrentStream       * stream, int piece, int block)
{
    WTorrentData * data = stream->data;

    QBitArray * blocks = &(data->blocks);

    int blockCount = data->blockCount;

    int blockCurrent = stream->current * blockCount + block;

    block       ++;
    blockCurrent++;

    while (block < blockCount && blocks->at(blockCurrent))
    {
        block       ++;
        blockCurrent++;
    }

    qDebug("BLOCK COMPLETE %d", block);

    if (block < blockCount)
    {
        stream->block = block;

        qint64 buffer = (qint64) (piece * stream->sizePiece + block * TORRENTENGINE_BLOCK);

        applyBuffer(stream, buffer);
    }
    else applyPiece(handle, stream, piece);
}

void WTorrentEnginePrivate::applyPiece(const torrent_handle & handle,
                                       WTorrentStream       * stream, int piece)
{
    qDebug("APPLY PIECE %d", piece);

    WTorrentData * data = stream->data;

    //---------------------------------------------------------------------------------------------
    // Pieces

    QBitArray * pieces = &(data->pieces);

    int count = stream->count;

    int current = stream->current;

    piece  ++;
    current++;

    while (piece < count && pieces->at(current))
    {
        piece  ++;
        current++;
    }

    if (piece == count)
    {
        qDebug("FILE AT END");

        stream->buffer = stream->size;

        WTorrent * torrent = stream->torrent;

        QCoreApplication::postEvent(torrent,
                                    new WTorrentEventValue(WTorrent::EventBuffer, stream->buffer));

        int end = stream->end - 1;

        for (int i = stream->begin; i < end; i++)
        {
            if (pieces->at(i) == false) return;
        }

        qDebug("TORRENT STREAM FINISHED");

        stream->finished = true;

        QCoreApplication::postEvent(torrent, new WTorrentEvent(WTorrent::EventFinished));

        return;
    }

    stream->piece   = piece;
    stream->current = current;

    //---------------------------------------------------------------------------------------------
    // Blocks

    QBitArray * blocks = &(data->blocks);

    int block      = 0;
    int blockCount = data->blockCount;

    int blockCurrent = current * blockCount;

    while (block < blockCount && blocks->at(blockCurrent))
    {
        block       ++;
        blockCurrent++;
    }

    if (block == blockCount)
    {
        qDebug("THIS SHOULD NOT BE CALLED");

        applyPiece(handle, stream, piece);

        return;
    }

    stream->block = block;

    //---------------------------------------------------------------------------------------------
    // Buffer

    qint64 buffer = (qint64) (piece * stream->sizePiece + block * TORRENTENGINE_BLOCK);

    applyBuffer(stream, buffer);

    //---------------------------------------------------------------------------------------------
    // Deadline

    int priority = TORRENTENGINE_PRIORITY_COUNT;

    int deadline = 1;

    while (priority && piece < count)
    {
        if (pieces->at(current) == false)
        {
            handle.set_piece_deadline(current, deadline);

            deadline++;

            priority--;
        }

        piece++;

        current++;
    }
}

//-------------------------------------------------------------------------------------------------

void WTorrentEnginePrivate::applyBuffer(WTorrentStream * stream, qint64 buffer)
{
    qint64 size = stream->size;

    if (buffer > size) buffer = size;

    if (stream->buffer >= buffer) return;

    stream->buffer = buffer;

    QCoreApplication::postEvent(stream->torrent,
                                new WTorrentEventValue(WTorrent::EventBuffer, buffer));
}

//-------------------------------------------------------------------------------------------------

WTorrentData * WTorrentEnginePrivate::getData(const QUrl & url) const
{
    foreach (WTorrentData * data, datas)
    {
        if (data->source->url == url)
        {
            return data;
        }
    }

    QHashIterator<unsigned int, WTorrentData *> i(torrents);

    while (i.hasNext())
    {
        i.next();

        WTorrentData * data = i.value();

        if (data->source->url == url)
        {
            return data;
        }
    }

    return NULL;
}

//-------------------------------------------------------------------------------------------------

WTorrentSource * WTorrentEnginePrivate::getSource(const QUrl & url)
{
    foreach (WTorrentSource * source, sources)
    {
        if (source->url == url)
        {
            qDebug("TORRENT CACHED");

            sources.removeOne(source);
            sources.   append(source);

            return source;
        }
    }

    return NULL;
}

//-------------------------------------------------------------------------------------------------

WTorrentItem * WTorrentEnginePrivate::getItem(WTorrent * torrent) const
{
    foreach (WTorrentData * data, datas)
    {
        foreach (WTorrentItem * item, data->items)
        {
            if (item->torrent == torrent)
            {
                return item;
            }
        }
    }

    QHashIterator<unsigned int, WTorrentData *> i(torrents);

    while (i.hasNext())
    {
        i.next();

        foreach (WTorrentItem * item, i.value()->items)
        {
            if (item->torrent == torrent)
            {
                return item;
            }
        }
    }

    return NULL;
}

WTorrentStream * WTorrentEnginePrivate::getStream(WTorrent * torrent) const
{
    WTorrentItem * item = getItem(torrent);

    if (item && item->mode == WTorrent::Stream)
    {
         return static_cast<WTorrentStream *> (item);
    }
    else return NULL;
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

        if (type == state_update_alert::alert_type)
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
                                                                   event->piece_index));
        }
        else if (type == save_resume_data_alert::alert_type)
        {
            Q_Q(WTorrentEngine);

            save_resume_data_alert * event = alert_cast<save_resume_data_alert>(alert);

            unsigned int hash = hash_value(event->handle);

            mutex.lock();

            QString fileName = fileNames.take(hash);

            mutex.unlock();

            std::ofstream stream(fileName.C_STR, std::ios_base::binary);

            stream.unsetf(std::ios_base::skipws);

            bencode(std::ostream_iterator<char>(stream), *(event->resume_data));

            QCoreApplication::postEvent(q, new WTorrentEngineHandle(EventSaved, hash));
        }
        else if (type == save_resume_data_failed_alert::alert_type)
        {
            Q_Q(WTorrentEngine);

            save_resume_data_failed_alert * event
                = alert_cast<save_resume_data_failed_alert>(alert);

            QCoreApplication::postEvent(q, new WTorrentEngineHandle(EventSaved,
                                                                    hash_value(event->handle)));
        }
        else if (type == torrent_added_alert::alert_type)
        {
            Q_Q(WTorrentEngine);

            torrent_added_alert * event = alert_cast<torrent_added_alert>(alert);

            QVariant variant;

            variant.setValue(event->handle);

            QCoreApplication::postEvent(q, new WTorrentEngineEvent(EventAdded, variant));
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

void WTorrentEnginePrivate::onFolderDelete()
{
    if (deleteIds.isEmpty()) return;

    int id = deleteIds.takeFirst();

    ids.removeOne(id);

    QString path = deletePaths.takeFirst();

    WControllerFile::deleteFolder(path);
}

void WTorrentEnginePrivate::onFolderClear()
{
    qDebug("TORRENT CACHE CLEARED");

    timerSave->stop();

    WControllerFile::deleteFolder(path);
}

//-------------------------------------------------------------------------------------------------

void WTorrentEnginePrivate::onSave()
{
    qDebug("TORRENT INDEX SAVED");

    if (QFile::exists(path) == false)
    {
        WControllerFile::createFolder(path);
    }

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

/* Q_INVOKABLE */ void WTorrentEngine::load(WTorrent * torrent, QIODevice * device)
{
    device->moveToThread(thread());

    QCoreApplication::postEvent(this, new WTorrentEngineAdd(torrent, device, torrent->url  (),
                                                                             torrent->index(),
                                                                             torrent->mode ()));
}

/* Q_INVOKABLE */ void WTorrentEngine::seek(WTorrent * torrent, qint64 position)
{
    if (position < 0) return;

    QCoreApplication::postEvent(this, new WTorrentEngineAction(WTorrentEnginePrivate::EventSeek,
                                                               torrent, position));
}

/* Q_INVOKABLE */ void WTorrentEngine::remove(WTorrent * torrent, bool deleteFiles)
{
    QCoreApplication::postEvent(this, new WTorrentEngineAction(WTorrentEnginePrivate::EventRemove,
                                                               torrent, deleteFiles));
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WTorrentEngine::clearCache()
{
    QCoreApplication::postEvent(this, new QEvent(static_cast<QEvent::Type>
                                                 (WTorrentEnginePrivate::EventClearCache)));
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

        pack.set_bool(settings_pack::smooth_connects, false);

        pack.set_int(settings_pack::connection_speed, 500);

        //pack.set_int(settings_pack::max_failcount,      1);
        pack.set_int(settings_pack::min_reconnect_time, 1);

        //pack.set_int(settings_pack::peer_timeout,         1);
        pack.set_int(settings_pack::peer_connect_timeout, 1);

        // FIXME: Workaround to disable write cache.
        pack.set_int(settings_pack::cache_size, 0);

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

        //dht.search_branching = 1;

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
        WTorrentEngineAdd * eventTorrent = static_cast<WTorrentEngineAdd *> (event);

        WTorrent * torrent = eventTorrent->torrent;

        QIODevice * device = eventTorrent->device;

        QUrl url   = eventTorrent->url;
        int  index = eventTorrent->index;

        WTorrent::Mode mode = static_cast<WTorrent::Mode> (eventTorrent->mode);

        WTorrentData * data = d->getData(url);

        if (mode == WTorrent::Stream)
        {
            if (data)
            {
                WTorrentStream * stream = d->createStream(data->handle.torrent_file(),
                                                          data, torrent, index, mode);

                if (data->hash)
                {
                    d->addStream(data->handle, stream);

                    d->selectFile(stream);
                }
            }
            else
            {
                QByteArray array = device->readAll();

                boost::shared_ptr<torrent_info> info
                    = boost::shared_ptr<torrent_info> (new torrent_info(array, array.size()));

                WTorrentData * data = d->createData(info, url);

                d->createStream(info, data, torrent, index, mode);
            }
        }
        else if (data)
        {
            WTorrentItem * item = d->createItem(data->handle.torrent_file(), data, torrent, index,
                                                mode);

            if (data->hash)
            {
                d->addItem(data->handle, item);

                d->selectFile(item);
            }
        }
        else
        {
            QByteArray array = device->readAll();

            boost::shared_ptr<torrent_info> info
                = boost::shared_ptr<torrent_info> (new torrent_info(array, array.size()));

            WTorrentData * data = d->createData(info, url);

            d->createItem(info, data, torrent, index, mode);
        }

        delete device;

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WTorrentEnginePrivate::EventAdded))
    {
        WTorrentEngineEvent * eventTorrent = static_cast<WTorrentEngineEvent *> (event);

        WTorrentData * data = d->datas.takeFirst();

        if (data == NULL) return true;

        torrent_handle handle = eventTorrent->value.value<torrent_handle>();

        unsigned int hash = hash_value(handle);

        data->handle = handle;
        data->hash   = hash;

        d->torrents.insert(hash, data);

        foreach (WTorrentItem * item, data->items)
        {
            if (item->mode == WTorrent::Stream)
            {
                WTorrentStream * stream = static_cast<WTorrentStream *> (item);

                d->addStream(handle, stream);
            }
            else d->addItem(handle, item);
        }

        d->updateFiles(data);

        d->timerUpdate->start();

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WTorrentEnginePrivate::EventSeek))
    {
        WTorrentEngineAction * eventTorrent = static_cast<WTorrentEngineAction *> (event);

        WTorrentStream * stream = d->getStream(eventTorrent->torrent);

        if (stream->finished == false)
        {
            WTorrentData * data = stream->data;

            if (data->hash == 0)
            {
                stream->position = eventTorrent->value.toLongLong();
            }
            else d->prioritize(data->handle, stream, eventTorrent->value.toLongLong());
        }
        else QCoreApplication::postEvent(stream->torrent,
                                         new WTorrentEventValue(WTorrent::EventBuffer,
                                                                stream->size));

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WTorrentEnginePrivate::EventRemove))
    {
        WTorrentEngineAction * eventTorrent = static_cast<WTorrentEngineAction *> (event);

        WTorrentItem * item = d->getItem(eventTorrent->torrent);

        if (item == NULL) return true;

        WTorrentData * data = item->data;

        QList<WTorrentItem *> * items = &(data->items);

        items->removeOne(item);

        if (items->isEmpty() == false)
        {
            d->unselectFile(item);

            delete item;

            return true;
        }

        delete item;

        const torrent_handle & handle = data->handle;

        if (data->hash == 0)
        {
            qDebug("REMOVE TORRENT ADD");

            d->datas.removeOne(data);

            d->session->remove_torrent(handle);

            delete data;
        }
        else
        {
            qDebug("REMOVE TORRENT");

            int id = data->source->id;

            QString fileName = data->path + "/." + QString::number(id);

            d->mutex.lock();

            d->fileNames.insert(data->hash, fileName);

            d->mutex.unlock();

            handle.save_resume_data();

            if (d->updateCache(data) == false)
            {
                d->deleteIds  .append(id);
                d->deletePaths.append(data->path);

                // FIXME libtorrent: Waiting before removing the torrent folder.
                QTimer::singleShot(1000, this, SLOT(onFolderDelete()));
            }
        }

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

        if (data->items.isEmpty() == false) return true;

        unsigned int hash = data->hash;

        d->torrents.remove(hash);

        if (d->torrents.isEmpty())
        {
            d->timerUpdate->stop();
        }

        d->session->remove_torrent(data->handle);

        delete data;

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WTorrentEnginePrivate::EventProgress))
    {
        WTorrentEngineProgress * eventProgress = static_cast<WTorrentEngineProgress *> (event);

        const QList<WTorrentProgress> & list = eventProgress->list;

        foreach (const WTorrentProgress & value, list)
        {
            WTorrentData * data = d->torrents.value(value.hash);

            if (data == NULL) continue;

            foreach (WTorrentItem * item, data->items)
            {
                QCoreApplication::postEvent(item->torrent,
                                            new WTorrentEventProgress(value.progress,
                                                                      value.download, value.upload,
                                                                      value.seeds, value.peers));
            }
        }

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WTorrentEnginePrivate::EventBlock))
    {
        WTorrentEngineBlock * eventTorrent = static_cast<WTorrentEngineBlock *> (event);

        WTorrentData * data = d->torrents.value(eventTorrent->hash);

        if (data == NULL) return true;

        int piece = eventTorrent->piece;
        int block = eventTorrent->block;

        data->blocks.setBit(piece * data->blockCount + block);

        foreach (WTorrentItem * item, data->items)
        {
            if (item->mode == WTorrent::Stream)
            {
                WTorrentStream * stream = static_cast<WTorrentStream *> (item);

                if (stream->finished == false && stream->current == piece && stream->block == block)
                {
                    d->applyBlock(data->handle, stream, piece - stream->begin, block);
                }
            }
        }

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WTorrentEnginePrivate::EventPiece))
    {
        WTorrentEngineValue * eventTorrent = static_cast<WTorrentEngineValue *> (event);

        WTorrentData * data = d->torrents.value(eventTorrent->hash);

        if (data == NULL) return true;

        int piece = eventTorrent->value.toInt();

        data->pieces.setBit(piece);

        int block      = 0;
        int blockCount = data->blockCount;

        int blockCurrent = piece * blockCount;

        while (block < blockCount)
        {
            data->blocks.setBit(blockCurrent);

            block       ++;
            blockCurrent++;
        }

        foreach (WTorrentItem * item, data->items)
        {
            if (item->mode == WTorrent::Stream)
            {
                WTorrentStream * stream = static_cast<WTorrentStream *> (item);

                if (stream->finished == false && stream->current == piece)
                {
                    d->applyPiece(data->handle, stream, piece - stream->begin);
                }
            }
            else if (item->finished == false)
            {
                int current = item->current;

                if (current == piece)
                {
                    current++;

                    if (current == item->end)
                    {
                        item->finished = true;

                        QCoreApplication::postEvent(item->torrent,
                                                    new WTorrentEvent(WTorrent::EventFinished));

                        d->save();
                    }
                    else item->current = current;
                }
            }
        }

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

        qDebug("TORRENT ERROR");

        foreach (WTorrentItem * item, data->items)
        {
            QCoreApplication::postEvent(item->torrent,
                                        new WTorrentEventValue(WTorrent::EventError,
                                                               eventTorrent->value));
        }

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WTorrentEnginePrivate::EventSizeMax))
    {
        WTorrentEngineEvent * eventTorrent = static_cast<WTorrentEngineEvent *> (event);

        d->maximum = eventTorrent->value.toLongLong();

        if (d->cleanCache()) d->save();

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WTorrentEnginePrivate::EventClearCache))
    {
        qDebug("TORRENT CLEAR CACHE");

        if (d->sources.isEmpty() == false)
        {
            d->size = 0;

            foreach (WTorrentSource * source, d->sources)
            {
                delete source;
            }

            d->sources.clear();

            foreach (int id, d->deleteIds)
            {
                d->ids.removeOne(id);
            }

            d->deleteIds  .clear();
            d->deletePaths.clear();
        }

        // FIXME libtorrent: Waiting before clearing the torrent folder.
        QTimer::singleShot(1000, this, SLOT(onFolderClear()));

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WTorrentEnginePrivate::EventClear))
    {
        d->timerUpdate->stop();

        delete d->session;

        d->session = NULL;

        foreach (WTorrentData * data, d->datas)
        {
            delete data;
        }

        QHashIterator<unsigned int, WTorrentData *> i(d->torrents);

        while (i.hasNext())
        {
            i.next();

            delete i.value();
        }

        if (d->timerSave->isActive())
        {
            d->onSave();
        }

        foreach (WTorrentSource * source, d->sources)
        {
            delete source;
        }

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
