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

#include "WHookTorrent.h"

#ifndef SK_NO_HOOKTORRENT

// Sk includes
#include <WControllerFile>
#include <WControllerNetwork>
#include <WControllerTorrent>

//-------------------------------------------------------------------------------------------------
// Static variables

static const int HOOKTORRENT_MINIMUM_SIZE = 524288; // 512 kilobytes

static const int HOOKTORRENT_DEFAULT_RATE = 3600000; // 1 hour

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include "WHookTorrent_p.h"

WHookTorrentPrivate::WHookTorrentPrivate(WHookTorrent * p) : WAbstractHookPrivate(p) {}

void WHookTorrentPrivate::init()
{
    torrent = NULL;
    reply   = NULL;

    byteRate = -1;

    state = StateDefault;
}

//-------------------------------------------------------------------------------------------------
// Private function
//-------------------------------------------------------------------------------------------------

void WHookTorrentPrivate::load()
{
    Q_Q(WHookTorrent);

    reply = wControllerTorrent->getTorrent(source, q, WTorrent::Stream);

    state = StateLoading;

    QObject::connect(reply, SIGNAL(added (WTorrentReply *)), q, SLOT(onAdded ()));
    QObject::connect(reply, SIGNAL(loaded(WTorrentReply *)), q, SLOT(onLoaded()));

    QObject::connect(reply, SIGNAL(progress(qint64, qint64)), q, SLOT(onProgress(qint64)));
}

void WHookTorrentPrivate::start()
{
    Q_Q(WHookTorrent);

    state = StateStarting;

    backend->setSource(fileName);

    backend->play();

    q->setFilterActive(true);
}

//-------------------------------------------------------------------------------------------------

void WHookTorrentPrivate::play()
{
    Q_Q(WHookTorrent);

    load();

    if (backend->currentTime() == -1)
    {
         q->setStateLoad(WAbstractBackend::StateLoadStarting);
    }
    else q->setStateLoad(WAbstractBackend::StateLoadResuming);

    q->setState(WAbstractBackend::StatePlaying);
}

void WHookTorrentPrivate::stop()
{
    backend->stop ();
    backend->clear();
}

//-------------------------------------------------------------------------------------------------

void WHookTorrentPrivate::clearReply()
{
    Q_Q(WHookTorrent);

    q->setFilterActive(false);

    delete reply;

    torrent = NULL;
    reply   = NULL;

    byteRate = -1;

    state = StateDefault;

    q->setStateLoad(WAbstractBackend::StateLoadDefault);
    q->setState    (WAbstractBackend::StateStopped);
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WHookTorrentPrivate::onAdded()
{
    torrent = reply->torrent();

    QStringList paths = torrent->paths();

    if (paths.isEmpty())
    {
        stop();

        clearReply();
    }
    else fileName = WControllerFile::fileUrl(paths.first());
}

void WHookTorrentPrivate::onLoaded()
{
    if (torrent->hasError())
    {
        Q_Q(WHookTorrent);

        q->setFilterActive(false);

        backend->stop();

        reply->deleteLater();

        torrent = NULL;
        reply   = NULL;

        byteRate = -1;

        state = StateDefault;

        q->setStateLoad(WAbstractBackend::StateLoadDefault);
        q->setState    (WAbstractBackend::StateStopped);
    }
    else if (state == StateLoading)
    {
        qint64 size = torrent->size();

        int duration = backend->duration();

        if (duration > 0)
        {
            byteRate = size / duration;
        }

        start();
    }
}

//-------------------------------------------------------------------------------------------------

void WHookTorrentPrivate::onProgress(qint64 bytesReceived)
{
    if (state != StateLoading || bytesReceived < HOOKTORRENT_MINIMUM_SIZE) return;

    qint64 size = torrent->size();

    int buffer = (bytesReceived * 10000) / size;

    if (buffer > 29 && WControllerFile::exists(fileName))
    {
        int duration = backend->duration();

        if (duration > 0)
        {
            byteRate = size / duration;
        }

        start();
    }
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

WHookTorrent::WHookTorrent(WAbstractBackend * backend)
    : WAbstractHook(new WHookTorrentPrivate(this), backend)
{
    Q_D(WHookTorrent); d->init();
}

//-------------------------------------------------------------------------------------------------
// WAbstractHook reimplementation
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ void WHookTorrent::loadSource(const QUrl & url, int duration,
                                                                          int currentTime)
{
    Q_D(WHookTorrent);

    if (d->source != url)
    {
        if (d->backend->isPlaying() || d->state != WHookTorrentPrivate::StateDefault)
        {
            d->stop();

            d->clearReply();

            setDuration   (duration);
            setCurrentTime(currentTime);

            d->source = url;

            d->play();

            return;
        }

        if (d->backend->isPaused())
        {
            d->stop();

            d->clearReply();
        }
        else if (d->reply)
        {
            d->clearReply();
        }

        setDuration   (duration);
        setCurrentTime(currentTime);

        d->source = url;
    }
    else seekTo(currentTime);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ void WHookTorrent::play()
{
    Q_D(WHookTorrent);

    WAbstractBackend::State state = d->backend->state();

    if (state == WAbstractBackend::StatePaused)
    {
        d->backend->play();
    }
    else if (state != WAbstractBackend::StatePlaying && d->reply == NULL)
    {
        d->play();
    }
}

/* Q_INVOKABLE virtual */ void WHookTorrent::replay()
{
    Q_D(WHookTorrent);

    if (d->reply) return;

    d->stop();
    d->load();

    setStateLoad(WAbstractBackend::StateLoadStarting);
    setState    (WAbstractBackend::StatePlaying);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ void WHookTorrent::pause()
{
    Q_D(WHookTorrent);

    if (d->backend->isPlaying())
    {
        d->backend->pause();
    }
    else stop();
}

/* Q_INVOKABLE virtual */ void WHookTorrent::stop()
{
    Q_D(WHookTorrent);

    d->stop();

    if (d->reply) d->clearReply();
}

/* Q_INVOKABLE virtual */ void WHookTorrent::clear()
{
    loadSource(QUrl());
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ void WHookTorrent::seekTo(int) {}

//-------------------------------------------------------------------------------------------------
// WBackendFilter reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WHookTorrent::filterStateLoad(WAbstractBackend::StateLoad * stateLoad)
{
    if (*stateLoad == WAbstractBackend::StateLoadDefault)
    {
        Q_D(WHookTorrent);

        if (d->state == WHookTorrentPrivate::StateStarting)
        {
            qDebug("STARTING");

            d->state = WHookTorrentPrivate::StateDefault;

            if (d->backend->duration() == 0)
            {
                d->byteRate = d->torrent->size() / HOOKTORRENT_DEFAULT_RATE;
            }
        }
    }
    else if (*stateLoad == WAbstractBackend::StateLoadBuffering)
    {
        qDebug("BUFFERING");
    }
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WHookTorrent::filterCurrentTime(int *) {}

/* virtual */ void WHookTorrent::filterDuration(int * msec)
{
    Q_D(WHookTorrent);

    int duration = *msec;

    if (duration > 0)
    {
         d->byteRate = d->torrent->size() / duration;
    }
    else d->byteRate = d->torrent->size() / HOOKTORRENT_DEFAULT_RATE;
}

//-------------------------------------------------------------------------------------------------
// Protected WAbstractHook implementation
//-------------------------------------------------------------------------------------------------

/* virtual */ bool WHookTorrent::hookCheckSource(const QUrl & url)
{
    QString extension = WControllerNetwork::extractUrlExtension(url);

    if (extension == "torrent")
    {
         return true;
    }
    else return false;
}

#endif // SK_NO_HOOKTORRENT
