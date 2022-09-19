//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkTorrent.

    - GNU Lesser General Public License Usage:
    This file may be used under the terms of the GNU Lesser General Public License version 3 as
    published by the Free Software Foundation and appearing in the LICENSE.md file included in the
    packaging of this file. Please review the following information to ensure the GNU Lesser
    General Public License requirements will be met: https://www.gnu.org/licenses/lgpl.html.

    - Private License Usage:
    Sky kit licensees holding valid private licenses may use this file in accordance with the
    private license agreement provided with the Software or, alternatively, in accordance with the
    terms contained in written agreement between you and Sky kit authors. For further information
    contact us at contact@omega.gg.
*/
//=================================================================================================

#include "WHookTorrent.h"

#ifndef SK_NO_HOOKTORRENT

// Qt includes
#include <QThread>
#include <QTcpServer>

// Sk includes
#include <WControllerFile>
#include <WControllerNetwork>
#include <WControllerPlaylist>
#include <WTorrentEngine>

// Forward declarations
class WTorrentSocket;

//-------------------------------------------------------------------------------------------------
// Static variables

static const int HOOKTORRENT_SIZE = 524288; // 512 kilobytes

static const int HOOKTORRENT_PROGRESS = 100; // 0.1 percent

static const int HOOKTORRENT_BUFFER = HOOKTORRENT_SIZE / 8;

static const int HOOKTORRENT_METADATA = 1048576 * 10; // 10 megabytes

static const int HOOKTORRENT_SKIP      =  100;
static const int HOOKTORRENT_SKIP_SIZE = 1024; // 1 kilobyte

//static const int HOOKTORRENT_INTERVAL = 16;

//=================================================================================================
// WTorrentThread
//=================================================================================================

class WTorrentThread : public QThread
{
    Q_OBJECT

public:
    WTorrentThread(WTorrentEngine * engine, int port);

protected: // QThread reimplementation
    /* virtual */ void run();

private: // Functions
    void clearData();

private slots:
    void onFile(WTorrent * torrent, const QString & fileName, qint64 size);

    void onBuffer(qint64 progress);
    void onSeek  (qint64 progress);

    void onStart();
    //void onSkip ();

    void onClear();

    void onConnection  ();
    void onDisconnected();

    void onBytesWritten(qint64 bytes);

    void onError(QAbstractSocket::SocketError error);

private:
    WTorrentEngine * engine;
    WTorrent       * torrent;

    QTcpServer * server;

    WTorrentSocket * data;

    QFile * file;

    qint64 size;
    qint64 progress;

    qint64 position;

    bool started;
    bool seeking;

    int skip;

    int port;

private:
    friend class WTorrentSocket;
};

//=================================================================================================
// WTorrentSocket
//=================================================================================================

class WTorrentSocket : public QObject
{
    Q_OBJECT

public:
    WTorrentSocket(WTorrentThread * thread, QTcpSocket * socket);

private: // Functions
    void writeBuffer(int length);

private slots:
    void onRead ();
    void onWrite();

private:
    WTorrentThread * thread;
    QTcpSocket     * socket;

    QString buffer;

    bool ready;

    int skip;

    //QTimer timer;

private:
    friend class WTorrentThread;
};

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

WTorrentSocket::WTorrentSocket(WTorrentThread * thread, QTcpSocket * socket) : QObject(thread)
{
    this->thread = thread;
    this->socket = socket;

    ready = false;

    skip = 0;

    //timer.setInterval(HOOKTORRENT_INTERVAL);

    //timer.setSingleShot(true);

    connect(socket, SIGNAL(readyRead()), this, SLOT(onRead()));

    //connect(&timer, SIGNAL(timeout()), this, SLOT(onWrite()));
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WTorrentSocket::writeBuffer(int length)
{
    QByteArray buffer(length, '0');

    socket->write(buffer);
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WTorrentSocket::onRead()
{
    buffer.append(socket->readAll());

    if (buffer.contains("\r\n\r\n") == false) return;

    qDebug("MESSAGE %s", buffer.C_STR);

    disconnect(socket, SIGNAL(readyRead()), this, SLOT(onRead()));

    qint64 position;

    int indexA = buffer.indexOf("Range: bytes=");

    if (indexA != -1)
    {
        indexA += 13;

        int indexB = buffer.indexOf('-', indexA);

        if (indexB != -1)
        {
             position = buffer.mid(indexA, indexB - indexA).toLongLong();
        }
        else position = 0;
    }
    else position = 0;

    qint64 size = thread->size;

    qint64 length = size - position;

    qint64 end;

    // FIXME VLC: Sometimes the position is equal to the size.
    if (position == size)
    {
         end = size;
    }
    else end = size - 1;

    QString header = "HTTP/1.1 206 Partial Content\r\n"
                     "Accept-Ranges: bytes\r\n"
                     "Content-Length: "      + QString::number(length)   + "\r\n"
                     "Content-Range: bytes " + QString::number(position) + '-'
                                             + QString::number(end)      + '/'
                                             + QString::number(size)     + "\r\n"
                     "Connection: close\r\n\r\n";

    qDebug("REPLY %s", header.C_STR);

    ready = true;

    if (socket->write(header.toLatin1()) != header.length())
    {
        qDebug("HEADER FAILED");

        socket->disconnect();

        return;
    }

    socket->waitForBytesWritten();

    qint64 progress = thread->progress;

    if (thread->started == false
        &&
        length > HOOKTORRENT_METADATA && (progress + HOOKTORRENT_SIZE) < position)
    {
        if (thread->skip != -1)
        {
            if (thread->skip == 1)
            {
                qDebug("SKIP DATA");

                skip = HOOKTORRENT_SKIP;

                ready = false;

                writeBuffer(HOOKTORRENT_SKIP_SIZE);

                return;
            }
            else thread->skip += 1;
        }
    }

    ready = true;

    skip = 0;

    if (position < thread->position || progress < position)
    {
        qDebug("SEEKING");

        thread->progress = -1;
        thread->position = position;

        thread->seeking = true;

        thread->engine->seek(thread->torrent, position);

        return;
    }

    if (thread->position != position)
    {
        qDebug("SEEKING LIGHT");

        thread->position = position;

        thread->seeking = true;
    }

    onWrite();
}

void WTorrentSocket::onWrite()
{
    if (skip)
    {
        qDebug("SKIP SEEK");

        skip--;

        if (skip) ready = false;

        writeBuffer(HOOKTORRENT_SIZE);

        return;
    }

    qint64 progress = thread->progress;
    qint64 position = thread->position;

    qint64 buffer = progress - position;

    if (buffer > HOOKTORRENT_BUFFER)
    {
        /*if (timer.isActive() == false)
        {
            qDebug("WRITE");
        }*/

        qDebug("WRITE");

        if (thread->seeking)
        {
            thread->seeking = false;

            if (thread->file->seek(position) == false)
            {
                qDebug("SEEK FAILED");
            }
        }

        buffer -= HOOKTORRENT_BUFFER;

        if (buffer > HOOKTORRENT_SIZE)
        {
            buffer = HOOKTORRENT_SIZE;
        }

        QByteArray bytes = thread->file->read(buffer);

        if (bytes.count() == buffer)
        {
            ready = false;

            qint64 result = socket->write(bytes);

            if (result != -1)
            {
                thread->position += result;

                if (result != buffer)
                {
                    qDebug("WRITE INCOMPLETE");

                    thread->seeking = true;
                }
            }
            else
            {
                qDebug("WRITE FAILED");

                socket->disconnect();
            }
        }
        else
        {
            qDebug("READ INCOMPLETE");

            thread->seeking = true;
        }
    }
    else
    {
        /*if (timer.isActive())
        {
            qDebug("WAIT");
        }*/

        qDebug("WAIT");

        qint64 size = thread->size;

        if (progress != size) return;

        qDebug("END");

        if (thread->seeking)
        {
            thread->seeking = false;

            if (thread->file->seek(position) == false)
            {
                qDebug("SEEK FAILED");
            }
        }

        buffer = size - position;

        QByteArray bytes = thread->file->read(buffer);

        if (bytes.count() == buffer)
        {
            ready = false;

            qint64 result = socket->write(bytes);

            if (result != -1)
            {
                thread->position += result;

                if (result != buffer)
                {
                    qDebug("END WRITE INCOMPLETE");

                    thread->seeking = true;
                }
            }
            else
            {
                qDebug("END WRITE FAILED");

                socket->disconnect();
            }

            qDebug("END ! %d", thread->file->atEnd());
        }
        else
        {
            qDebug("END INCOMPLETE %d", thread->file->atEnd());

            thread->seeking = true;

            //timer.start();
        }
    }
}

//=================================================================================================
// WTorrentThread
//=================================================================================================

WTorrentThread::WTorrentThread(WTorrentEngine * engine, int port)
{
    this->engine = engine;

    torrent = NULL;

    this->port = port;

    moveToThread(this);

    start(QThread::IdlePriority);
}

//-------------------------------------------------------------------------------------------------
// Protected QThread reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WTorrentThread::run()
{
    server = new QTcpServer(this);

    data = NULL;

    file = NULL;

    connect(server, SIGNAL(newConnection()), this, SLOT(onConnection()));

    server->listen(QHostAddress("127.0.0.1"), port);

    exec();
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WTorrentThread::clearData()
{
    QTcpSocket * socket = data->socket;

    // NOTE: We need to disconnect to avoid receiving a disconnect signal upon deletion.
    disconnect(socket, 0, this, 0);

    delete socket;
    delete data;
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WTorrentThread::onFile(WTorrent * torrent, const QString & fileName, qint64 size)
{
    qDebug("TORRENT FILE");

    if (data)
    {
        clearData();

        data = NULL;
    }

    if (file) delete file;

    this->torrent = torrent;

    file = new QFile(fileName);

    this->size = size;

    progress = 0;

    position = 0;

    started = false;
    seeking = false;

    // NOTE VLC: We get the wrong duration when skipping data on avi files.
    if (WControllerNetwork::extractUrlExtension(fileName).toLower() == "avi")
    {
         skip = -1;
    }
    else skip = 0;
}

//-------------------------------------------------------------------------------------------------

void WTorrentThread::onBuffer(qint64 progress)
{
    if (this->progress == -1) return;

    this->progress = progress;

    if (data && data->ready)
    {
        //data->timer.stop();

        data->onWrite();
    }
}

void WTorrentThread::onSeek(qint64 progress)
{
    this->progress = progress;

    if (data && data->ready)
    {
        //data->timer.stop();

        data->onWrite();
    }
}

//-------------------------------------------------------------------------------------------------

void WTorrentThread::onStart()
{
    started = true;
}

/*void WTorrentThread::onSkip()
{
    qDebug("SKIP SEEK");

    seeking = false;

    if (data && data->ready)
    {
        //data->timer.stop();

        data->skip = HOOKTORRENT_SKIP;

        data->onWrite();
    }
}*/

//-------------------------------------------------------------------------------------------------

void WTorrentThread::onClear()
{
    qDebug("CLEAR CONNECTION");

    if (data)
    {
        clearData();

        data = NULL;
    }

    if (file)
    {
        delete file;

        file = NULL;
    }
}

//-------------------------------------------------------------------------------------------------

void WTorrentThread::onConnection()
{
    qDebug("NEW CONNECTION");

    QTcpSocket * socket = server->nextPendingConnection();

    if (socket == NULL) return;

    if (file == NULL)
    {
        qDebug("SKIP CONNECTION");

        delete socket;

        return;
    }

    if (data) clearData();

    if (file->isOpen() || file->open(QIODevice::ReadOnly))
    {
        data = new WTorrentSocket(this, socket);

        connect(socket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));

        connect(socket, SIGNAL(bytesWritten(qint64)), this, SLOT(onBytesWritten(qint64)));

#ifdef QT_OLD
        connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
                this,   SLOT(onError(QAbstractSocket::SocketError)));
#else
        connect(socket, SIGNAL(errorOccurred(QAbstractSocket::SocketError)),
                this,   SLOT(onError(QAbstractSocket::SocketError)));
#endif
    }
    else
    {
        qDebug("FAILED TO OPEN FILE");

        data = NULL;

        delete socket;
    }
}

void WTorrentThread::onDisconnected()
{
    qDebug("SOCKET DISCONNECTED");

    QTcpSocket * socket = data->socket;

    disconnect(socket, 0, this, 0);

    socket->deleteLater();

    delete data;

    data = NULL;
}

//-------------------------------------------------------------------------------------------------

void WTorrentThread::onBytesWritten(qint64)
{
    if (data == NULL || data->ready) return;

    //data->timer.stop();

    data->ready = true;

    data->onWrite();
}

//-------------------------------------------------------------------------------------------------

void WTorrentThread::onError(QAbstractSocket::SocketError error)
{
    qDebug("SOCKET ERROR %d", error);

    data->socket->disconnect();
}

//=================================================================================================
// WHookTorrentPrivate
//=================================================================================================

WHookTorrentPrivate::WHookTorrentPrivate(WHookTorrent * p) : WAbstractHookPrivate(p) {}

/* virtual */ WHookTorrentPrivate::~WHookTorrentPrivate()
{
    thread->quit();
    thread->wait();

    delete thread;

    W_GET_CONTROLLER(WControllerTorrent, controller);

    if (controller) controller->unregisterPort(port);
}

//-------------------------------------------------------------------------------------------------

void WHookTorrentPrivate::init()
{
    torrent = NULL;
    reply   = NULL;

    state = StateDefault;

    port = wControllerTorrent->registerPort();

    url = "http://127.0.0.1:" + QString::number(port);

    currentTime = -1;

    thread = new WTorrentThread(wControllerTorrent->engine(), port);

    const QMetaObject * meta = thread->metaObject();

    methodFile   = meta->method(meta->indexOfMethod("onFile(WTorrent*,QString,qint64)"));
    methodBuffer = meta->method(meta->indexOfMethod("onBuffer(qint64)"));
    methodSeek   = meta->method(meta->indexOfMethod("onSeek(qint64)"));
    methodStart  = meta->method(meta->indexOfMethod("onStart()"));
    //methodSkip   = meta->method(meta->indexOfMethod("onSkip()"));
    methodClear  = meta->method(meta->indexOfMethod("onClear()"));
}

//-------------------------------------------------------------------------------------------------
// Private function
//-------------------------------------------------------------------------------------------------

void WHookTorrentPrivate::load()
{
    Q_Q(WHookTorrent);

    reply = wControllerTorrent->getTorrent(source, q, WTorrent::Stream);

    state = StateLoading;

    q->setProgress(-1.0);

    QObject::connect(reply, SIGNAL(added (WTorrentReply *)), q, SLOT(onAdded ()));
    QObject::connect(reply, SIGNAL(loaded(WTorrentReply *)), q, SLOT(onLoaded()));

    QObject::connect(reply, SIGNAL(buffer(qint64, qint64)), q, SLOT(onBuffer(qint64, qint64)));
    QObject::connect(reply, SIGNAL(seek  (qint64, qint64)), q, SLOT(onSeek  (qint64, qint64)));

    QObject::connect(reply, SIGNAL(destroyed()), q, SLOT(onDestroyed()));
}

void WHookTorrentPrivate::start()
{
    Q_Q(WHookTorrent);

    if (torrent->bufferPieces() == torrent->size())
    {
        qDebug("START QUICK");

        state = StatePlaying;

        q->applySource(url);

        q->backendSetSource(url);

        q->setFilterActive(true);

        q->backendPlay();

        q->setProgress(1.0);
    }
    else
    {
        state = StateStarting;

        q->applySource(url);

        q->applyState(WAbstractBackend::StateStopped);

        q->backendSetSource(url);

        q->setFilterActive(true);

        currentTime = backend->currentTime();

        if (currentTime != -1)
        {
            q->backendSetVolume(0.0);

            q->applyCurrentTime(-1);

            q->backendPlay();

            q->applyCurrentTime(currentTime);
        }
        else q->backendPlay();

        q->applyState(WAbstractBackend::StatePlaying);
    }

    if (WControllerPlaylist::urlIsAudio(fileName))
    {
        q->setOutputActive(WAbstractBackend::OutputAudio);
    }
}

//-------------------------------------------------------------------------------------------------

void WHookTorrentPrivate::play()
{
    Q_Q(WHookTorrent);

    load();

    q->setState(WAbstractBackend::StatePlaying);

    if (backend->currentTime() == -1)
    {
         q->setStateLoad(WAbstractBackend::StateLoadStarting);
    }
    else q->setStateLoad(WAbstractBackend::StateLoadResuming);
}

void WHookTorrentPrivate::stop()
{
    Q_Q(WHookTorrent);

    methodClear.invoke(thread);

    q->setProgress(0.0);

    q->setFilterActive(false);

    q->backendSetSource(QString());
}

//-------------------------------------------------------------------------------------------------

void WHookTorrentPrivate::applyBuffer(qint64 bufferBlocks)
{
    if (state == StateLoading)
    {
        if (WControllerFile::exists(fileName))
        {
            start();
        }
    }
    else if (state != StateStarting) return;

    Q_Q(WHookTorrent);

    int buffer = (bufferBlocks * 100000) / torrent->size();

    qreal progress = (qreal) buffer / HOOKTORRENT_PROGRESS;

    if (progress < 0.01)
    {
        progress = 0.01;
    }
    else if (progress > 0.9)
    {
        progress = 0.9;
    }

    if (backend->progress() < progress)
    {
        q->setProgress(progress);
    }
}

//-------------------------------------------------------------------------------------------------

void WHookTorrentPrivate::clearReply()
{
    Q_Q(WHookTorrent);

    QObject::disconnect(reply, 0, q, 0);

    delete reply;

    clearData();

    q->setState    (WAbstractBackend::StateStopped);
    q->setStateLoad(WAbstractBackend::StateLoadDefault);
}

void WHookTorrentPrivate::clearData()
{
    torrent = NULL;
    reply   = NULL;

    state = StateDefault;

    if (currentTime != -1)
    {
        Q_Q(WHookTorrent);

        currentTime = -1;

        q->backendSetVolume(backend->volume());
    }
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WHookTorrentPrivate::onAdded()
{
    torrent = reply->torrent();

    QStringList paths = torrent->paths();

    if (paths.isEmpty() == false)
    {
        QString path = paths.first();

        fileName = WControllerFile::fileUrl(path);

        methodFile.invoke(thread, Q_ARG(WTorrent      *, torrent),
                                  Q_ARG(const QString &, path), Q_ARG(qint64, torrent->size()));
    }
    else clearReply();
}

void WHookTorrentPrivate::onLoaded()
{
    if (torrent == NULL)
    {
        Q_Q(WHookTorrent);

        q->setProgress(0.0);

        QObject::disconnect(reply, 0, q, 0);

        reply->deleteLater();

        reply = NULL;

        state = StateDefault;

        q->setState    (WAbstractBackend::StateStopped);
        q->setStateLoad(WAbstractBackend::StateLoadDefault);
    }
    else if (torrent->hasError())
    {
        Q_Q(WHookTorrent);

        wControllerTorrent->clearSource(torrent->url());

        if (state == StateLoading)
        {
            q->setProgress(0.0);
        }
        else stop();

        QObject::disconnect(reply, 0, q, 0);

        reply->deleteLater();

        clearData();

        q->setState    (WAbstractBackend::StateStopped);
        q->setStateLoad(WAbstractBackend::StateLoadDefault);
    }
    else if (state == StateLoading)
    {
        start();
    }
}

//-------------------------------------------------------------------------------------------------

void WHookTorrentPrivate::onBuffer(qint64, qint64 bufferBlocks)
{
    methodBuffer.invoke(thread, Q_ARG(qint64, bufferBlocks));

    applyBuffer(bufferBlocks);
}

void WHookTorrentPrivate::onSeek(qint64, qint64 bufferBlocks)
{
    methodSeek.invoke(thread, Q_ARG(qint64, bufferBlocks));

    applyBuffer(bufferBlocks);
}

//-------------------------------------------------------------------------------------------------

void WHookTorrentPrivate::onUpdateState()
{
    if (currentTime != -2) return;

    Q_Q(WHookTorrent);

    currentTime = -1;

    q->backendSetVolume(backend->volume());

    state = WHookTorrentPrivate::StatePlaying;

    q->setState(WAbstractBackend::StatePlaying);
}

//-------------------------------------------------------------------------------------------------

void WHookTorrentPrivate::onDestroyed()
{
    Q_Q(WHookTorrent);

    qDebug("DESTROYED!");

    if (state == StateLoading)
    {
        q->setProgress(0.0);
    }
    else stop();

    clearData();

    q->setState    (WAbstractBackend::StateStopped);
    q->setStateLoad(WAbstractBackend::StateLoadDefault);
}

//=================================================================================================
// WHookTorrent
//=================================================================================================

WHookTorrent::WHookTorrent(WAbstractBackend * backend)
    : WAbstractHook(new WHookTorrentPrivate(this), backend)
{
    Q_D(WHookTorrent); d->init();
}

//-------------------------------------------------------------------------------------------------
// WAbstractHook reimplementation
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ void WHookTorrent::loadSource(const QString & url, int duration,
                                                                             int currentTime)
{
    Q_D(WHookTorrent);

    if (d->source != url)
    {
        if (d->state == WHookTorrentPrivate::StateDefault)
        {
            d->source = url;

            setDuration   (duration);
            setCurrentTime(currentTime);

            updateSource();
        }
        else if (d->state == WHookTorrentPrivate::StatePaused)
        {
            d->stop();

            d->clearReply();

            d->source = url;

            setDuration   (duration);
            setCurrentTime(currentTime);

            updateSource();
        }
        else
        {
            d->stop();

            disconnect(d->reply, 0, this, 0);

            delete d->reply;

            d->clearData();

            d->source = url;

            setDuration   (duration);
            setCurrentTime(currentTime);

            updateSource();

            d->play();
        }
    }
    else if (d->state < WHookTorrentPrivate::StateStarting)
    {
        setDuration   (duration);
        setCurrentTime(currentTime);
    }
    else if (d->state == WHookTorrentPrivate::StateStarting)
    {
        setDuration   (duration);
        setCurrentTime(currentTime);

        d->currentTime = currentTime;

        if (currentTime == -1)
        {
            backendSetVolume(d->backend->volume());

            setStateLoad(WAbstractBackend::StateLoadStarting);
        }
        else
        {
            backendSetVolume(0.0);

            setStateLoad(WAbstractBackend::StateLoadResuming);
        }
    }
    else if (d->state == WHookTorrentPrivate::StatePaused)
    {
        d->stop();

        d->clearReply();

        setDuration   (duration);
        setCurrentTime(currentTime);
    }
    else
    {
        setDuration(duration);

        //d->methodSkip.invoke(d->thread);

        d->backend->seek(currentTime);
    }
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ void WHookTorrent::play()
{
    Q_D(WHookTorrent);

    if (d->state == WHookTorrentPrivate::StateDefault)
    {
        if (d->reply) return;

        d->play();
    }
    else if (d->state == WHookTorrentPrivate::StatePaused)
    {
        d->state = WHookTorrentPrivate::StatePlaying;

        backendPlay();

        setState(WAbstractBackend::StatePlaying);
    }
}

/* Q_INVOKABLE virtual */ void WHookTorrent::replay()
{
    Q_D(WHookTorrent);

    if (d->reply) return;

    d->stop();
    d->load();

    setState    (WAbstractBackend::StatePlaying);
    setStateLoad(WAbstractBackend::StateLoadStarting);
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
    else
    {
        d->stop();

        d->clearReply();
    }
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

    if (d->state == WHookTorrentPrivate::StateDefault) return;

    d->stop();

    d->clearReply();

    d->source = QString();

    setDuration   (-1);
    setCurrentTime(-1);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ void WHookTorrent::seek(int msec)
{
    Q_D(WHookTorrent);

    if (d->state < WHookTorrentPrivate::StatePlaying)
    {
        setCurrentTime(msec);

        // NOTE: We set a temporary variable and we'll apply once we start streaming.
        d->currentTime = msec;
    }
    else
    {
        //d->methodSkip.invoke(d->thread);

        d->backend->seek(msec);
    }
}

//-------------------------------------------------------------------------------------------------
// WBackendFilter reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WHookTorrent::filterState(WAbstractBackend::State * state)
{
    if (*state != WAbstractBackend::StateStopped) return;

    Q_D(WHookTorrent);

    wControllerTorrent->clearSource(d->torrent->url());

    stop();
}

/* virtual */ void WHookTorrent::filterStateLoad(WAbstractBackend::StateLoad * stateLoad)
{
    Q_D(WHookTorrent);

    if (d->state != WHookTorrentPrivate::StateStarting) return;

    if (*stateLoad != WAbstractBackend::StateLoadDefault)
    {
        *stateLoad = d->backend->stateLoad();

        return;
    }

    qDebug("START!!! %d", d->currentTime);

    if (d->currentTime == -1)
    {
        d->methodStart.invoke(d->thread);

        d->state = WHookTorrentPrivate::StatePlaying;

        setState(WAbstractBackend::StatePlaying);

        setProgress(1.0);
    }
    else if (d->backend->progress() == 1.0)
    {
        *stateLoad = d->backend->stateLoad();

        if (d->currentTime == -2) return;

        d->currentTime = -2;

        // FIXME VLC: Waiting for the first frame.
        QTimer::singleShot(64, this, SLOT(onUpdateState()));
    }
    else
    {
        *stateLoad = d->backend->stateLoad();

        d->methodStart.invoke(d->thread);
        //d->methodSkip .invoke(d->thread);

        backendSeek(d->currentTime);

        setProgress(1.0);
    }
}

//-------------------------------------------------------------------------------------------------
// Protected WAbstractHook reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ bool WHookTorrent::hookCheck(const QString & url)
{
    if (WControllerNetwork::extractUrlExtension(url) == "torrent"
        ||
        url.startsWith("magnet:?", Qt::CaseInsensitive))
    {
         return true;
    }
    else return false;
}

#endif // SK_NO_HOOKTORRENT

#include "WHookTorrent.moc"
