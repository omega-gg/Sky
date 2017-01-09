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
#include <WControllerFile>
#include <WControllerNetwork>
#include <WControllerDownload>
#include <WTorrentEngine>

W_INIT_CONTROLLER(WControllerTorrent)

//-------------------------------------------------------------------------------------------------
// Static variables

static const QString CONTROLLERTORRENT_PATH_TORRENTS = "/torrents";

//=================================================================================================
// WTorrentReply
//=================================================================================================
// Private

WTorrentReply::WTorrentReply(const QUrl & url, QObject * parent) : QObject(parent)
{
    _url = WControllerNetwork::removeUrlFragment(url);

    _index = extractIndex(url);
}

//-------------------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------------------

int WTorrentReply::extractIndex(const QUrl & url)
{
    QString fragment = url.fragment();

    if (fragment.isEmpty())
    {
         return -1;
    }
    else return fragment.toInt();
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

QUrl WTorrentReply::url() const
{
    return _url;
}

int WTorrentReply::index() const
{
    return _index;
}

//-------------------------------------------------------------------------------------------------

bool WTorrentReply::hasError() const
{
    return (_error.isEmpty() == false);
}

QString WTorrentReply::error() const
{
    return _error;
}

//=================================================================================================
// WControllerTorrentPrivate
//=================================================================================================

#include <private/WControllerTorrent_p>

WControllerTorrentPrivate::WControllerTorrentPrivate(WControllerTorrent * p)
    : WControllerPrivate(p) {}

/* virtual */ WControllerTorrentPrivate::~WControllerTorrentPrivate()
{
    delete engine;

    thread->quit();
    thread->wait();

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

void WControllerTorrentPrivate::loadTorrent(WTorrentReply * reply)
{
    Q_Q(WControllerTorrent);

    const QUrl & url = reply->_url;

    QHashIterator<WRemoteData *, WPrivateTorrentData *> i(jobs);

    WRemoteData         * data    = NULL;
    WPrivateTorrentData * torrent = NULL;

    while (i.hasNext())
    {
         i.next();

         if (i.value()->url == url)
         {
             data    = i.key  ();
             torrent = i.value();

             break;
         }
    }

    if (data == NULL)
    {
        data = wControllerDownload->getData(url, q, QNetworkRequest::HighPriority);

        if (data == NULL)
        {
            qWarning("WControllerTorrentPrivate::loadTorrent: Failed to load torrent %s.",
                     url.C_URL);

            return;
        }

        QObject::connect(data, SIGNAL(loaded(WRemoteData *)), q, SLOT(onLoaded(WRemoteData *)));

        torrent = new WPrivateTorrentData;

        torrent->url = url;

        jobs.insert(data, torrent);
    }

    torrent->replies.append(reply);
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WControllerTorrentPrivate::onLoaded(WRemoteData * data)
{
    WPrivateTorrentData * torrent = jobs.take(data);

    if (data->hasError())
    {
        qWarning("WControllerTorrentPrivate::onLoaded: Failed to load torrent %s.",
                 data->url().C_URL);

        QString error = data->error();

        foreach (WTorrentReply * reply, torrent->replies)
        {
            reply->_error = error;

            emit reply->loaded(reply);
        }

        delete torrent;
    }
    else
    {
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

/* Q_INVOKABLE */ WTorrentReply * WControllerTorrent::getTorrent(const QUrl & url,
                                                                 QObject    * parent)
{
    if (url.isValid() == false) return NULL;

    Q_D(WControllerTorrent);

    WTorrentReply * reply;

    if (parent) reply = new WTorrentReply(url, parent);
    else        reply = new WTorrentReply(url, this);

    d->loadTorrent(reply);

    return reply;
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
    return wControllerFile->pathStorage() + "/torrents";
}

#endif // SK_NO_CONTROLLERTORRENT
