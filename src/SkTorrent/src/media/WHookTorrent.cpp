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

// Qt includes
#include <QThread>
#include <QTcpServer>

// Sk includes
#include <WControllerFile>
#include <WControllerNetwork>
#include <WControllerTorrent>
#include <WTorrentEngine>

// Forward declarations
class WTorrentSocket;

//-------------------------------------------------------------------------------------------------
// Static variables

static const int HOOKTORRENT_MINIMUM_SIZE = 1048576; // 1 megabyte

static const int HOOKTORRENT_START =  100; // 0.1 percent

static const int HOOKTORRENT_BUFFER = 1000; // 1 second

static const int HOOKTORRENT_DEFAULT_RATE = 3600000; // 1 hour

//-------------------------------------------------------------------------------------------------

static const int HOOKTORRENT_SOCKET_SIZE = 1048576; // 1 megabyte

static const int HOOKTORRENT_SOCKET_BUFFER = HOOKTORRENT_SOCKET_SIZE / 4;

static const int HOOKTORRENT_SOCKET_INTERVAL = 20;

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

private slots:
    void onFile(WTorrent * torrent, const QString & fileName, qint64 size);

    void onBuffer(qint64 progress);
    void onStart ();

    void onClear();

    void onConnection  ();
    void onDisconnected();

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
    void applyPosition();

    void writeBuffer(int length);

private slots:
    void onRead ();
    void onWrite();

private:
    WTorrentThread * thread;
    QTcpSocket     * socket;

    QString buffer;

    qint64 position;

    QTimer timer;

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

    position = 0;

    connect(socket, SIGNAL(readyRead()), this, SLOT(onRead()));

    connect(&timer, SIGNAL(timeout()), this, SLOT(onWrite()));
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WTorrentSocket::applyPosition()
{
    int indexA = buffer.indexOf("Range: bytes=");

    if (indexA != -1)
    {
        indexA += 13;

        int indexB = buffer.indexOf('-', indexA);

        if (indexB == -1)
        {
             position = 0;
        }
        else position = buffer.mid(indexA, indexB - indexA).toLongLong();
    }
    else position = 0;
}

//-------------------------------------------------------------------------------------------------

void WTorrentSocket::writeBuffer(int length)
{
    timer.stop();

    QString buffer;

    for (int i = 0; i < length; i++)
    {
        buffer.append('0');
    }

    socket->write(buffer.toLatin1());
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WTorrentSocket::onRead()
{
    buffer.append(socket->readAll());

    if (buffer.contains("\r\n\r\n") == false) return;

    qDebug("MESSAGE %s", buffer.C_STR);

    QString header;

    header.append("HTTP/1.1 206 Partial Content\r\n"
                  "Accept-Ranges: bytes\r\n");

    applyPosition();

    qint64 size = thread->size;

    qint64 length = size - position;

    header.append("Content-Length: " + QString::number(length) + "\r\n");

    qint64 end = size - 1;

    header.append("Content-Range: bytes " + QString::number(position) + '-' + QString::number(end)
                  + '/' + QString::number(size) + "\r\nConnection: close\r\n\r\n");

    qDebug("REPLY %s", header.C_STR);

    socket->write(header.toLatin1());

    qint64 progress = thread->progress;

    if (thread->started == false)
    {
        if (length < 64)
        {
            qDebug("SKIP METADATA");

            writeBuffer(length);

            return;
        }
        else if (progress < position)
        {
            qDebug("SKIP DATA");

            writeBuffer(1024);

            return;
        }
    }

    if (thread->file->seek(position) == false)
    {
        qDebug("SEEK FAILED");
    }

    if (position < thread->position || progress < position)
    {
        qDebug("SEEKING");

        timer.stop();

        thread->position = position;

        thread->engine->seek(thread->torrent, position);
    }
    else onWrite();
}

void WTorrentSocket::onWrite()
{
    qint64 progress = thread->progress;

    qint64 buffer = progress - position;

    if (buffer > HOOKTORRENT_SOCKET_BUFFER)
    {
        if (timer.isActive() == false)
        {
            qDebug("WRITE");
        }

        buffer -= HOOKTORRENT_SOCKET_BUFFER;

        QByteArray bytes;

        if (buffer < HOOKTORRENT_SOCKET_SIZE)
        {
             bytes = thread->file->read(buffer);
        }
        else bytes = thread->file->read(HOOKTORRENT_SOCKET_SIZE);

        int count = bytes.count();

        if (count)
        {
            socket->write(bytes);

            position += count;
        }

        timer.start(HOOKTORRENT_SOCKET_INTERVAL);
    }
    else
    {
        if (timer.isActive())
        {
            qDebug("WAIT");
        }

        qint64 size = thread->size;

        if (progress == size)
        {
            qDebug("END");

            qint64 length = size - position;

            QByteArray bytes = thread->file->read(length);

            int count = bytes.count();

            if (count == length)
            {
                timer.stop();

                socket->write(bytes);

                position = size;

                qDebug("END ! %d", thread->file->atEnd());

                return;
            }

            if (count)
            {
                qDebug("END INCOMPLETE");

                socket->write(bytes);

                position += count;
            }

            timer.start(HOOKTORRENT_SOCKET_INTERVAL);
        }
        else timer.stop();
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

    server->listen(QHostAddress::LocalHost, port);

    exec();
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WTorrentThread::onFile(WTorrent * torrent, const QString & fileName, qint64 size)
{
    if (data)
    {
        delete data->socket;
        delete data;
    }

    if (file) delete file;

    this->torrent = torrent;

    file = new QFile(fileName);

    this->size = size;

    progress = 0;

    position = 0;

    started = false;
}

//-------------------------------------------------------------------------------------------------

void WTorrentThread::onBuffer(qint64 progress)
{
    this->progress = progress;

    if (data) data->onWrite();
}

void WTorrentThread::onStart()
{
    started = true;
}

//-------------------------------------------------------------------------------------------------

void WTorrentThread::onClear()
{
    if (data)
    {
        delete data->socket;
        delete data;

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

    if (file)
    {
        if (data)
        {
            delete data->socket;
            delete data;
        }

        data = new WTorrentSocket(this, socket);

        if (file->isOpen() == false)
        {
            if (file->open(QIODevice::ReadOnly) == false)
            {
                qDebug("FAILED TO OPEN FILE");
            }
        }

        connect(socket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    }
    else delete socket;
}

void WTorrentThread::onDisconnected()
{
    qDebug("SOCKET DISCONNECTED");

    data->socket->deleteLater();

    delete data;

    data = NULL;
}

//=================================================================================================
// WHookTorrentPrivate
//=================================================================================================

#include "WHookTorrent_p.h"

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
    methodStart  = meta->method(meta->indexOfMethod("onStart()"));
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

    QObject::connect(reply, SIGNAL(buffer(qint64)), q, SLOT(onBuffer(qint64)));
}

void WHookTorrentPrivate::start()
{
    Q_Q(WHookTorrent);

    state = StateStarting;

    q->applySource(url);

    q->backendSetSource(url);

    currentTime = backend->currentTime();

    q->applyCurrentTime(-1);

    q->setFilterActive(true);

    q->backendPlay();

    q->applyCurrentTime(currentTime);
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

    methodClear.invoke(thread);

    q->setFilterActive(false);

    q->setProgress(0.0);

    backend->stop();

    q->backendSetSource(QUrl());
}

//-------------------------------------------------------------------------------------------------

void WHookTorrentPrivate::clearReply()
{
    Q_Q(WHookTorrent);

    delete reply;

    torrent = NULL;
    reply   = NULL;

    state = StateDefault;

    currentTime = -1;

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
    if (torrent->hasError())
    {
        Q_Q(WHookTorrent);

        stop();

        reply->deleteLater();

        torrent = NULL;
        reply   = NULL;

        state = StateDefault;

        currentTime = -1;

        q->setStateLoad(WAbstractBackend::StateLoadDefault);
        q->setState    (WAbstractBackend::StateStopped);
    }
    else if (state == StateLoading)
    {
        start();
    }
}

//-------------------------------------------------------------------------------------------------

void WHookTorrentPrivate::onBuffer(qint64 bytesReceived)
{
    methodBuffer.invoke(thread, Q_ARG(qint64, bytesReceived));

    if (state != StateLoading) return;

    qint64 size = torrent->size();

    int buffer = (bytesReceived * 100000) / size;

    if (buffer >= HOOKTORRENT_START)
    {
        Q_Q(WHookTorrent);

        if (q->backend()->progress() < 0.9)
        {
            q->setProgress(0.9);
        }

        if (bytesReceived >= HOOKTORRENT_MINIMUM_SIZE && WControllerFile::exists(fileName))
        {
            start();
        }
    }
    else if (buffer)
    {
        Q_Q(WHookTorrent);

        q->setProgress((qreal) buffer / HOOKTORRENT_START);
    }
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
    else seek(currentTime);
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

    if (d->state == WHookTorrentPrivate::StateDefault) return;

    d->stop();

    d->clearReply();

    setDuration   (-1);
    setCurrentTime(-1);

    d->source = QUrl();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ void WHookTorrent::seek(int msec)
{
    Q_D(WHookTorrent);

    if (d->state < WHookTorrentPrivate::StatePlaying)
    {
        setCurrentTime(msec);
    }
    else d->backend->seek(msec);
}

//-------------------------------------------------------------------------------------------------
// WBackendFilter reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WHookTorrent::filterState(WAbstractBackend::State * state)
{
    if (*state == WAbstractBackend::StateStopped)
    {
        stop();
    }
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
    else
    {
        if (d->backend->progress() == 1.0)
        {
            if (d->backend->currentTime() > d->currentTime)
            {
                d->currentTime = -1;

                d->state = WHookTorrentPrivate::StatePlaying;

                setState(WAbstractBackend::StatePlaying);
            }
            else *stateLoad = d->backend->stateLoad();
        }
        else
        {
            *stateLoad = d->backend->stateLoad();

            d->methodStart.invoke(d->thread);

            backendSeek(d->currentTime);

            setProgress(1.0);
        }
    }
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

#include "WHookTorrent.moc"
