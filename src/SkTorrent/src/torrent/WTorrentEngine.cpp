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

//-------------------------------------------------------------------------------------------------
// Static variables

static const int TORRENTENGINE_BLOCK = 16000;

static const int TORRENTENGINE_PRIORITY_COUNT = 10;

static const int TORRENTENGINE_TIMEOUT = 1000;

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

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
                                                                   event->piece_index));
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
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

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

        pack.set_bool(settings_pack::smooth_connects, false);

        pack.set_int(settings_pack::connection_speed, 400);

        //pack.set_int(settings_pack::max_failcount,      1);
        pack.set_int(settings_pack::min_reconnect_time, 1);

        pack.set_int(settings_pack::peer_timeout,         1);
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

        torrent_handle handle = d->session->add_torrent(params);

        //-----------------------------------------------------------------------------------------
        // Torrent paths

        QStringList paths;

        int index = variants.at(2).toInt();

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
            else index = -2;

            handle.prioritize_files(files);
        }

        //-----------------------------------------------------------------------------------------
        // Torrent mode

        WTorrent::Mode mode = static_cast<WTorrent::Mode> (variants.at(3).toInt());

        qint64 size;
        qint64 sizePiece;

        int begin;
        int end;

        if (index == -2)
        {
            size      = 0;
            sizePiece = 0;

            begin = 0;
            end   = 0;
        }
        else if (mode == WTorrent::Stream)
        {
            if (index == -1)
            {
                size      = 0;
                sizePiece = 0;

                begin = 0;
                end   = 0;
            }
            else
            {
                size = info->files().file_size(index);

                sizePiece = info->piece_length();

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

                //---------------------------------------------------------------------------------

                int deadline = 1;

                handle.set_piece_deadline(begin, deadline++);

                int last = end - 1;

                if (begin != last)
                {
                    handle.set_piece_deadline(last, deadline++);

                    int count = TORRENTENGINE_PRIORITY_COUNT;

                    int current = begin + 1;

                    last--;

                    while (count && current < last)
                    {
                        handle.set_piece_deadline(current, deadline++);

                        current++;

                        count--;
                    }
                }
            }
        }
        else
        {
            size      = info->total_size();
            sizePiece = 0;

            begin = 0;
            end   = info->num_pieces();

            if (mode == WTorrent::Sequential)
            {
                handle.set_sequential_download(true);
            }
        }

        //-----------------------------------------------------------------------------------------
        // Torrent add

        WTorrentData * data = new WTorrentData;

        int count = end - begin;

        data->id = id;

        data->mode = mode;

        data->path = path;

        data->torrent = torrent;

        data->handle = handle;

        data->size      = size;
        data->sizePiece = sizePiece;

        data->pieces = QBitArray(count);

        data->begin = begin;
        data->end   = end;

        data->index = 0;
        data->block = 0;

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

        const torrent_handle & handle = data->handle;

        d->session->remove_torrent(handle);

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WTorrentEnginePrivate::EventRemoved))
    {
        WTorrentEngineHandle * eventTorrent = static_cast<WTorrentEngineHandle *> (event);

        WTorrentData * data = d->torrents.value(eventTorrent->hash);

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

            if (data->mode == WTorrent::Stream)
            {
                 QCoreApplication::postEvent(data->torrent,
                                             new WTorrentEventProgress(data->progress,
                                                                       item.download, item.upload,
                                                                       item.seeds, item.peers));
            }
            else QCoreApplication::postEvent(data->torrent,
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

        if (data && data->mode == WTorrent::Stream
            &&
            data->index == eventTorrent->piece)
        {
            int block = eventTorrent->block;

            if (data->block < block)
            {
                data->block = block;

                data->progress = data->index * data->sizePiece
                                 +
                                 data->block * TORRENTENGINE_BLOCK;
            }
        }

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WTorrentEnginePrivate::EventPiece))
    {
        WTorrentEngineValue * eventTorrent = static_cast<WTorrentEngineValue *> (event);

        WTorrentData * data = d->torrents.value(eventTorrent->hash);

        int piece = eventTorrent->value.toInt() - data->begin;

        QBitArray * pieces = &(data->pieces);

        pieces->setBit(piece);

        if (data->mode == WTorrent::Stream)
        {
            int begin = data->begin;
            int last  = data->end - 1;

            int index   = data->index;
            int current = begin + index;

            if (current != last)
            {
                if (index == piece)
                {
                    index++;

                    while (index < pieces->count() && pieces->at(index))
                    {
                        index++;
                    }

                    data->index = index;
                    data->block = 0;

                    data->progress = data->index * data->sizePiece
                                     +
                                     data->block * TORRENTENGINE_BLOCK;
                }

                const torrent_handle & handle = data->handle;

                int deadline = 1;

                if (pieces->at(begin) == 0)
                {
                    handle.set_piece_deadline(begin, deadline++);
                }

                if (pieces->at(last) == 0)
                {
                    handle.set_piece_deadline(last, deadline++);
                }

                int count = TORRENTENGINE_PRIORITY_COUNT;

                last--;

                while (count && current < last)
                {
                    if (pieces->at(index) == 0)
                    {
                        handle.set_piece_deadline(current, deadline++);

                        count--;
                    }

                    index++;

                    current++;
                }
            }
        }

        QCoreApplication::postEvent(data->torrent,
                                    new WTorrentEventValue(WTorrent::EventPiece, piece));

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WTorrentEnginePrivate::EventError))
    {
        WTorrentEngineValue * eventTorrent = static_cast<WTorrentEngineValue *> (event);

        WTorrentData * data = d->torrents.value(eventTorrent->hash);

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
