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

static const int HOOKTORRENT_MINIMUM_SIZE = 1048576; // 1 megabyte

static const int HOOKTORRENT_START_BEGIN =  100; // 0.1 percent
static const int HOOKTORRENT_START_END   = 1000; // 1.0 percent

static const int HOOKTORRENT_BUFFER = 1000; // 1 second

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

    state = StateDefault;

    byteRate = 0;
    buffer   = 0;

    bufferStart = 0;
    bufferCheck = 0;
}

//-------------------------------------------------------------------------------------------------
// Private function
//-------------------------------------------------------------------------------------------------

void WHookTorrentPrivate::load()
{
    Q_Q(WHookTorrent);

    reply = wControllerTorrent->getTorrent(source, q, WTorrent::Stream);

    state = StateLoading;

    bufferStart = HOOKTORRENT_START_BEGIN;

    q->setProgress(-1.0);

    QObject::connect(reply, SIGNAL(added (WTorrentReply *)), q, SLOT(onAdded ()));
    QObject::connect(reply, SIGNAL(loaded(WTorrentReply *)), q, SLOT(onLoaded()));

    QObject::connect(reply, SIGNAL(progress(qint64, qint64)), q, SLOT(onProgress(qint64)));
}

void WHookTorrentPrivate::start()
{
    Q_Q(WHookTorrent);

    state = StateStarting;

    backend->loadSource(fileName);

    q->backendPlay();

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
    Q_Q(WHookTorrent);

    q->setFilterActive(false);

    q->setProgress(0.0);

    backend->stop ();
    backend->clear();
}

//-------------------------------------------------------------------------------------------------

void WHookTorrentPrivate::checkBuffer(int currentTime)
{
    int msec = buffer - currentTime;

    if (msec < bufferCheck)
    {
        Q_Q(WHookTorrent);

        state = WHookTorrentPrivate::StateBuffering;

        q->backendPause();

        q->setStateLoad(WAbstractBackend::StateLoadBuffering);
    }
}

void WHookTorrentPrivate::checkResume(int currentTime)
{
    int msec = buffer - currentTime;

    if (msec >= bufferCheck)
    {
        Q_Q(WHookTorrent);

        state = WHookTorrentPrivate::StatePlaying;

        q->setState(WAbstractBackend::StatePaused);

        q->backendPlay();

        q->setStateLoad(WAbstractBackend::StateLoadDefault);
        q->setState    (WAbstractBackend::StatePlaying);
    }
}

//-------------------------------------------------------------------------------------------------

void WHookTorrentPrivate::clearReply()
{
    Q_Q(WHookTorrent);

    delete reply;

    torrent = NULL;
    reply   = NULL;

    state = StateDefault;

    byteRate = 0;
    buffer   = 0;

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

        state = StateDefault;

        byteRate = 0;
        buffer   = 0;

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
    else if (state == StateBuffering)
    {
        Q_Q(WHookTorrent);

        state = WHookTorrentPrivate::StatePlaying;

        q->backendPlay();

        q->setStateLoad(WAbstractBackend::StateLoadDefault);
    }
}

//-------------------------------------------------------------------------------------------------

void WHookTorrentPrivate::onProgress(qint64 bytesReceived)
{
    if (state == StateStarting) return;

    if (state == StatePlaying)
    {
        buffer = bytesReceived / byteRate;

        checkBuffer(backend->currentTime());
    }
    else if (state == StateBuffering)
    {
        buffer = bytesReceived / byteRate;

        checkResume(backend->currentTime());
    }
    else if (state == StateLoading)
    {
        qint64 size = torrent->size();

        int buffer = (bytesReceived * 100000) / size;

        if (bufferStart != HOOKTORRENT_START_BEGIN)
        {
            if (buffer >= bufferStart)
            {
                start();
            }
        }
        else if (buffer >= bufferStart)
        {
            if (bytesReceived < HOOKTORRENT_MINIMUM_SIZE)
            {
                Q_Q(WHookTorrent);

                q->setProgress(0.9);
            }
            else if (WControllerFile::exists(fileName))
            {
                int duration = backend->duration();

                if (duration > 0)
                {
                    byteRate = size / duration;
                }

                start();
            }
        }
        else if (buffer)
        {
            Q_Q(WHookTorrent);

            q->setProgress((qreal) buffer / bufferStart);
        }
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
        if (d->state == WHookTorrentPrivate::StateDefault)
        {
            setDuration   (duration);
            setCurrentTime(currentTime);

            d->source = url;
        }
        else if (d->state == WHookTorrentPrivate::StatePaused)
        {
            d->stop();

            d->clearReply();

            setDuration   (duration);
            setCurrentTime(currentTime);

            d->source = url;
        }
        else
        {
            d->stop();

            d->clearReply();

            setDuration   (duration);
            setCurrentTime(currentTime);

            d->source = url;

            d->play();
        }
    }
    else seekTo(currentTime);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ void WHookTorrent::play()
{
    Q_D(WHookTorrent);

    if (d->state == WHookTorrentPrivate::StateDefault)
    {
        d->play();
    }
    else if (d->state == WHookTorrentPrivate::StatePaused)
    {
        if (d->torrent->isLoaded())
        {
            d->state = WHookTorrentPrivate::StatePlaying;

            backendPlay();
        }
        else
        {
            int msec = d->buffer - d->backend->currentTime();

            if (msec < d->bufferCheck)
            {
                d->state = WHookTorrentPrivate::StateBuffering;

                setStateLoad(WAbstractBackend::StateLoadBuffering);
            }
            else
            {
                d->state = WHookTorrentPrivate::StatePlaying;

                backendPlay();
            }
        }

        setState(WAbstractBackend::StatePlaying);
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

    if (d->state == WHookTorrentPrivate::StatePaused) return;

    if (d->state == WHookTorrentPrivate::StatePlaying)
    {
        d->state = WHookTorrentPrivate::StatePaused;

        backendPause();

        setState(WAbstractBackend::StatePaused);
    }
    else if (d->state == WHookTorrentPrivate::StateBuffering)
    {
        d->state = WHookTorrentPrivate::StatePaused;

        backendPause();

        setStateLoad(WAbstractBackend::StateLoadDefault);
        setState    (WAbstractBackend::StatePaused);
    }
    else stop();
}

/* Q_INVOKABLE virtual */ void WHookTorrent::stop()
{
    Q_D(WHookTorrent);

    if (d->state == WHookTorrentPrivate::StateDefault) return;

    d->stop();

    d->clearReply();
}

/* Q_INVOKABLE virtual */ void WHookTorrent::clear()
{
    Q_D(WHookTorrent);

    if (d->state != WHookTorrentPrivate::StateDefault)
    {
        d->stop();

        d->clearReply();
    }

    setDuration   (-1);
    setCurrentTime(-1);

    d->source = QUrl();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ void WHookTorrent::seekTo(int) {}

//-------------------------------------------------------------------------------------------------
// WBackendFilter reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WHookTorrent::filterState(WAbstractBackend::State * state)
{
    Q_D(WHookTorrent);

    if (d->state == WHookTorrentPrivate::StateStarting)
    {
        if (*state == WAbstractBackend::StateStopped)
        {
            if (d->bufferStart < HOOKTORRENT_START_END)
            {
                setFilterActive(false);

                setProgress(0.9);

                d->bufferStart += 100;

                d->state = WHookTorrentPrivate::StateLoading;

                if (d->backend->currentTime() == -1)
                {
                     setStateLoad(WAbstractBackend::StateLoadStarting);
                }
                else setStateLoad(WAbstractBackend::StateLoadResuming);

                *state = WAbstractBackend::StatePlaying;

                qint64 size = d->torrent->size();

                int buffer = (d->torrent->progress() * 100000) / size;

                if (buffer >= d->bufferStart)
                {
                    d->start();
                }
            }
            else stop();
        }
    }
    else if (d->state == WHookTorrentPrivate::StateBuffering)
    {
        *state = WAbstractBackend::StatePlaying;
    }
    else if (*state == WAbstractBackend::StateStopped)
    {
        stop();
    }
}

/* virtual */ void WHookTorrent::filterStateLoad(WAbstractBackend::StateLoad * stateLoad)
{
    Q_D(WHookTorrent);

    if (d->state == WHookTorrentPrivate::StateBuffering)
    {
        *stateLoad = WAbstractBackend::StateLoadBuffering;
    }
    else if (d->state == WHookTorrentPrivate::StateStarting)
    {
        if (*stateLoad == WAbstractBackend::StateLoadDefault)
        {
            setProgress(1.0);

            if (d->backend->duration() < 1)
            {
                d->byteRate = d->torrent->size() / HOOKTORRENT_DEFAULT_RATE;
            }

            d->buffer = d->torrent->progress() / d->byteRate;

            d->bufferCheck = qMin(HOOKTORRENT_BUFFER, d->backend->duration());

            if (d->buffer < d->bufferCheck)
            {
                d->state = WHookTorrentPrivate::StateBuffering;

                backendPause();

                *stateLoad = WAbstractBackend::StateLoadBuffering;
            }
            else d->state = WHookTorrentPrivate::StatePlaying;

            setState(WAbstractBackend::StatePlaying);
        }
    }
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WHookTorrent::filterCurrentTime(int * msec)
{
    Q_D(WHookTorrent);

    if (d->state == WHookTorrentPrivate::StateStarting) return;

    if (d->state == WHookTorrentPrivate::StatePlaying)
    {
        int currentTime = *msec;

        d->bufferCheck = qMin(HOOKTORRENT_BUFFER, d->backend->duration() - currentTime);

        d->checkBuffer(*msec);
    }
    else if (d->state == WHookTorrentPrivate::StateBuffering)
    {
        int currentTime = *msec;

        d->bufferCheck = qMin(HOOKTORRENT_BUFFER, d->backend->duration() - currentTime);

        d->checkResume(*msec);
    }
}

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
