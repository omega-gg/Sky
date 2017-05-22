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

#include "WControllerTorrent.h"

#ifndef SK_NO_CONTROLLERTORRENT

// Qt includes
#include <QThread>
#include <QDeclarativeComponent>

// Sk includes
#include <WControllerApplication>
#include <WControllerFile>
#include <WControllerNetwork>
#include <WControllerDownload>
#include <WTorrentEngine>

W_INIT_CONTROLLER(WControllerTorrent)

//-------------------------------------------------------------------------------------------------
// Static variables

static const QString CONTROLLERTORRENT_PATH_TORRENTS = "/torrents";

//=================================================================================================
// WTorrent
//=================================================================================================
// Private

WTorrent::WTorrent(const QUrl & url, Mode mode, QObject * parent) : QObject(parent)
{
    _url = WControllerNetwork::removeUrlFragment(url);

    QString fragment = url.fragment();

    if (fragment.isEmpty() == false)
    {
        int index = fragment.toInt();

        if (index < 1)
        {
             _index = -1;
        }
        else _index = index - 1;
    }
    else _index = -1;

    _mode = mode;

    _loaded = false;

    _size     = 0;
    _progress = 0;

    _download = 0;
    _upload   = 0;

    _seeds = 0;
    _peers = 0;
}

//-------------------------------------------------------------------------------------------------
// Protected events
//-------------------------------------------------------------------------------------------------

/* virtual */ bool WTorrent::event(QEvent * event)
{
    QEvent::Type type = event->type();

    if (type == static_cast<QEvent::Type> (EventAdd))
    {
        WTorrentEventAdd * eventTorrent = static_cast<WTorrentEventAdd *> (event);

        _paths = eventTorrent->paths;
        _size  = eventTorrent->size;

        _pieces = QBitArray(eventTorrent->pieces);

        _pieces.fill(false);

        foreach (WTorrentReply * reply, _replies)
        {
            emit reply->added(reply);
        }

        return true;
    }
    else if (type == static_cast<QEvent::Type> (EventProgress))
    {
        WTorrentEventProgress * eventTorrent = static_cast<WTorrentEventProgress *> (event);

        if (_paths.isEmpty()) return true;

        _progress = eventTorrent->progress;

        if (_download != -1)
        {
            _download = eventTorrent->download;
            _upload   = eventTorrent->upload;

            _seeds = eventTorrent->seeds;
            _peers = eventTorrent->peers;
        }

        foreach (WTorrentReply * reply, _replies)
        {
            emit reply->progress(_progress, _size);
        }

        return true;
    }
    else if (type == static_cast<QEvent::Type> (EventPiece))
    {
        WTorrentEventValue * eventTorrent = static_cast<WTorrentEventValue *> (event);

        int index = eventTorrent->value.toInt();

        _pieces.setBit(index);

        foreach (WTorrentReply * reply, _replies)
        {
            emit reply->pieceReady(index);
        }

        return true;
    }
    else if (type == static_cast<QEvent::Type> (EventFinished))
    {
        _loaded = true;

        foreach (WTorrentReply * reply, _replies)
        {
            emit reply->loaded(reply);
        }

        return true;
    }
    else if (type == static_cast<QEvent::Type> (EventError))
    {
        WTorrentEventValue * eventTorrent = static_cast<WTorrentEventValue *> (event);

        _error = eventTorrent->value.toString();

        qWarning("WTorrent::event: Torrent error: %s", _error.C_STR);

        foreach (WTorrentReply * reply, _replies)
        {
            emit reply->loaded(reply);
        }

        return true;
    }
    else return QObject::event(event);
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

QUrl WTorrent::url() const
{
    return _url;
}

int WTorrent::index() const
{
    return _index;
}

//-------------------------------------------------------------------------------------------------

WTorrent::Mode WTorrent::mode() const
{
    return _mode;
}

//-------------------------------------------------------------------------------------------------

bool WTorrent::isDefault() const
{
    return (_mode == Default);
}

bool WTorrent::isSequential() const
{
    return (_mode == Sequential);
}

bool WTorrent::isStream() const
{
    return (_mode == Stream);
}

//-------------------------------------------------------------------------------------------------

bool WTorrent::isLoaded() const
{
    return _loaded;
}

//-------------------------------------------------------------------------------------------------

QStringList WTorrent::paths() const
{
    return _paths;
}

//-------------------------------------------------------------------------------------------------

qint64 WTorrent::size() const
{
    return _size;
}

qint64 WTorrent::progress() const
{
    return _progress;
}

//-------------------------------------------------------------------------------------------------

QBitArray WTorrent::pieces() const
{
    return _pieces;
}

//-------------------------------------------------------------------------------------------------

int WTorrent::download() const
{
    return _download;
}

int WTorrent::upload() const
{
    return _upload;
}

//-------------------------------------------------------------------------------------------------

int WTorrent::seeds() const
{
    return _seeds;
}

int WTorrent::peers() const
{
    return _peers;
}

//-------------------------------------------------------------------------------------------------

bool WTorrent::hasError() const
{
    return (_error.isEmpty() == false);
}

QString WTorrent::error() const
{
    return _error;
}

//=================================================================================================
// WTorrentReply
//=================================================================================================
// Private

WTorrentReply::WTorrentReply(QObject * parent) : QObject(parent) {}

//-------------------------------------------------------------------------------------------------
// Public

/* virtual */ WTorrentReply::~WTorrentReply()
{
    W_GET_CONTROLLER(WControllerTorrent, controller);

    if (controller)
    {
        controller->d_func()->removeTorrent(_torrent, this);
    }
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

WTorrent * WTorrentReply::torrent() const
{
    return _torrent;
}

//=================================================================================================
// WControllerTorrentPrivate
//=================================================================================================

#include "WControllerTorrent_p.h"

WControllerTorrentPrivate::WControllerTorrentPrivate(WControllerTorrent * p)
    : WControllerPrivate(p) {}

/* virtual */ WControllerTorrentPrivate::~WControllerTorrentPrivate()
{
    engine->deleteInstance();

    sk->processEvents();

    thread->quit();
    thread->wait();

    delete engine;

    W_CLEAR_CONTROLLER(WControllerTorrent);
}

void WControllerTorrentPrivate::init()
{
    Q_Q(WControllerTorrent);

    thread = new QThread(q);

    thread->start();

    engine = new WTorrentEngine(thread);

    QObject::connect(wControllerFile, SIGNAL(pathStorageChanged()),
                     q,               SIGNAL(pathStorageChanged()));
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WControllerTorrentPrivate::loadTorrent(WTorrentReply * reply, const QUrl     & url,
                                                                   WTorrent::Mode   mode)
{
    Q_Q(WControllerTorrent);

    if (mode == WTorrent::Default)
    {
        QHashIterator<WRemoteData *, WTorrent *> i(jobs);

        while (i.hasNext())
        {
            i.next();

            WTorrent * torrent = i.value();

            if (torrent->_url == url && torrent->_mode == WTorrent::Default)
            {
                reply->_torrent = torrent;

                torrent->_replies.append(reply);

                return;
            }
        }

        foreach (WTorrent * torrent, downloads)
        {
            if (torrent->_mode == WTorrent::Default)
            {
                reply->_torrent = torrent;

                torrent->_replies.append(reply);

                return;
            }
        }
    }

    QUrl fileUrl = wControllerFile->getFileUrl(url);

    WRemoteData * data;

    if (fileUrl.isValid())
    {
         data = wControllerDownload->getData(WControllerFile::fileUrl(fileUrl), q,
                                             QNetworkRequest::HighPriority);
    }
    else data = wControllerDownload->getData(url, q, QNetworkRequest::HighPriority);

    if (data == NULL)
    {
        qWarning("WControllerTorrentPrivate::loadTorrent: Failed to load torrent %s.", url.C_URL);

        return;
    }

    QObject::connect(data, SIGNAL(loaded(WRemoteData *)), q, SLOT(onLoaded(WRemoteData *)));

    WTorrent * torrent = new WTorrent(url, mode, q);

    reply->_torrent = torrent;

    torrent->_replies.append(reply);

    jobs.insert(data, torrent);
}

void WControllerTorrentPrivate::removeTorrent(WTorrent * torrent, WTorrentReply * reply)
{
    QList<WTorrentReply *> & replies = torrent->_replies;

    replies.removeOne(reply);

    if (replies.isEmpty() == false) return;

    WRemoteData * data = jobs.key(torrent);

    if (data)
    {
        jobs.remove(data);

        delete data;
    }

    downloads.removeOne(torrent);

    engine->remove(torrent, true);

    delete torrent;
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WControllerTorrentPrivate::onLoaded(WRemoteData * data)
{
    WTorrent * torrent = jobs.take(data);

    if (data->hasError())
    {
        qWarning("WControllerTorrentPrivate::onLoaded: Failed to load torrent %s.",
                 data->url().C_URL);

        QString error = data->error();

        torrent->_error = error;

        foreach (WTorrentReply * reply, torrent->_replies)
        {
            emit reply->loaded(reply);
        }

        delete torrent;
    }
    else
    {
        downloads.push_back(torrent);

        engine->load(torrent, data->readAll(),
                     wControllerFile->pathStorage() + CONTROLLERTORRENT_PATH_TORRENTS);
    }

    delete data;
}

//=================================================================================================
// WControllerTorrent
//=================================================================================================

WControllerTorrent::WControllerTorrent() : WController(new WControllerTorrentPrivate(this)) {}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ WTorrentReply * WControllerTorrent::getTorrent(const QUrl     & url,
                                                                 QObject        * parent,
                                                                 WTorrent::Mode   mode)
{
    if (url.isValid() == false) return NULL;

    Q_D(WControllerTorrent);

    WTorrentReply * reply;

    if (parent) reply = new WTorrentReply(parent);
    else        reply = new WTorrentReply(this);

    d->loadTorrent(reply, url, mode);

    return reply;
}

/* Q_INVOKABLE */ void WControllerTorrent::clearTorrents()
{
    Q_D(WControllerTorrent);

    QList<WTorrentReply *> replies;

    QHashIterator<WRemoteData *, WTorrent *> i(d->jobs);

    while (i.hasNext())
    {
        i.next();

        foreach (WTorrentReply * reply, i.value()->_replies)
        {
            replies.append(reply);
        }
    }

    foreach (WTorrent * torrent, d->downloads)
    {
        replies.append(torrent->_replies);
    }

    foreach (WTorrentReply * reply, replies)
    {
        delete reply;
    }
}

//-------------------------------------------------------------------------------------------------
// Initialize
//-------------------------------------------------------------------------------------------------

/* virtual */ void WControllerTorrent::init()
{
    Q_D(WControllerTorrent); d->init();
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

WTorrentEngine * WControllerTorrent::engine() const
{
    Q_D(const WControllerTorrent); return d->engine;
}

//-------------------------------------------------------------------------------------------------

QString WControllerTorrent::pathStorage() const
{
    return wControllerFile->pathStorage() + CONTROLLERTORRENT_PATH_TORRENTS;
}

#endif // SK_NO_CONTROLLERTORRENT
