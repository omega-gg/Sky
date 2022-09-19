//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkCore.

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

#include "WBroadcastServer.h"

#ifndef SK_NO_BROADCASTSERVER

// Qt includes
#include <QCoreApplication>
#include <QThread>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostInfo>
#include <QNetworkInterface>

// Sk includes
#include <WControllerFile>

// Private includes
#include "WBroadcastServer_p.h"

//=================================================================================================
// WBroadcastServerThread
//=================================================================================================

class WBroadcastServerThread : public QThread
{
    Q_OBJECT

public:
    WBroadcastServerThread(WBroadcastServer * parent, int port);

protected: // QThread reimplementation
    /* virtual */ void run();

private: // Functions
    void clearSocket();

    void setConnected(bool connected);

private slots:
    void onConnection  ();
    void onDisconnected();

    void onBytesWritten(qint64 bytes);

private:
    WBroadcastServer * parent;

    QTcpServer * server;
    QTcpSocket * socket;

    int port;

    bool connected;
};

//-------------------------------------------------------------------------------------------------

WBroadcastServerThread::WBroadcastServerThread(WBroadcastServer * parent, int port)
{
    this->parent = parent;

    socket = NULL;

    this->port = port;

    connected = false;

    moveToThread(this);

    start(QThread::IdlePriority);
}

//-------------------------------------------------------------------------------------------------
// Protected QThread reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WBroadcastServerThread::run()
{
    server = new QTcpServer(this);

    connect(server, SIGNAL(newConnection()), this, SLOT(onConnection()));

    qDebug("WBroadcastServerThread: Listening on port %d...", port);

    server->listen(QHostAddress::Any, port);

    exec();
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WBroadcastServerThread::clearSocket()
{
    // NOTE: We need to disconnect to avoid receiving a disconnect signal upon deletion.
    disconnect(socket, 0, this, 0);

    delete socket;

    socket = NULL;
}

void WBroadcastServerThread::setConnected(bool connected)
{
    if (this->connected == connected) return;

    this->connected = connected;

    if (connected)
    {
         QCoreApplication::postEvent(parent,
                                     new QEvent(static_cast<QEvent::Type>
                                                (WBroadcastServerPrivate::EventConnected)));
    }
    else QCoreApplication::postEvent(parent,
                                     new QEvent(static_cast<QEvent::Type>
                                                (WBroadcastServerPrivate::EventDisconnected)));
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WBroadcastServerThread::onConnection()
{
    QTcpSocket * socket = server->nextPendingConnection();

    if (socket == NULL) return;

    qDebug("WBroadcastServerThread: New connection from %s.",
           socket->peerAddress().toString().C_STR);

    if (this->socket) clearSocket();

    this->socket = socket;

    connect(socket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));

    connect(socket, SIGNAL(bytesWritten(qint64)), this, SLOT(onBytesWritten(qint64)));

    setConnected(true);
}

void WBroadcastServerThread::onDisconnected()
{
    if (socket == NULL) return;

    disconnect(socket, 0, this, 0);

    socket->deleteLater();

    socket = NULL;

    setConnected(false);
}

//-------------------------------------------------------------------------------------------------

void WBroadcastServerThread::onBytesWritten(qint64)
{

}

//=================================================================================================
// WBroadcastServerSource and WBroadcastServerReply
//=================================================================================================

class WBroadcastServerSource : public WAbstractThreadAction
{
    Q_OBJECT

public:
    WBroadcastServerSource(int port, const QString & prefix)
    {
        this->port = port;

        this->prefix = prefix;
    }

protected: // WAbstractThreadAction reimplementation
    /* virtual */ WAbstractThreadReply * createReply() const;

protected: // WAbstractThreadAction implementation
    /* virtual */ bool run();

public: // Variables
    int port;

    QString prefix;
};

class WBroadcastServerReply : public WAbstractThreadReply
{
    Q_OBJECT

protected: // WAbstractThreadReply reimplementation
    /* virtual */ void onCompleted(bool ok);

signals:
    void complete(const QString & text);

public: // Variables
    QString source;
};

//-------------------------------------------------------------------------------------------------

/* virtual */ WAbstractThreadReply * WBroadcastServerSource::createReply() const
{
    return new WBroadcastServerReply;
}

/* virtual */ bool WBroadcastServerSource::run()
{
    WBroadcastServerReply * reply = qobject_cast<WBroadcastServerReply *> (this->reply());

    reply->source = WBroadcastServer::source(port, prefix);

    return true;
}

/* virtual */ void WBroadcastServerReply::onCompleted(bool)
{
    emit complete(source);
}

//=================================================================================================
// WBroadcastServerPrivate
//=================================================================================================

WBroadcastServerPrivate::WBroadcastServerPrivate(WBroadcastServer * p) : WPrivate(p) {}

/* virtual */ WBroadcastServerPrivate::~WBroadcastServerPrivate()
{
    thread->quit();
    thread->wait();

    delete thread;
}

//-------------------------------------------------------------------------------------------------

void WBroadcastServerPrivate::init(int port)
{
    Q_Q(WBroadcastServer);

    thread = new WBroadcastServerThread(q, port);

    connected = false;
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WBroadcastServerPrivate::setConnected(bool connected)
{
    if (this->connected == connected) return;

    Q_Q(WBroadcastServer);

    this->connected = connected;

    emit q->connectedChanged();
}

//=================================================================================================
// WBroadcastServer
//=================================================================================================

/* explicit */ WBroadcastServer::WBroadcastServer(int port, QObject * parent)
    : QObject(parent), WPrivatable(new WBroadcastServerPrivate(this))
{
    Q_D(WBroadcastServer); d->init(port);
}

//-------------------------------------------------------------------------------------------------
// Static functions
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WBroadcastServer::source(int port, const QString & prefix)
{
    const QHostAddress & local = QHostAddress(QHostAddress::LocalHost);

    QString host;

    foreach (const QHostAddress & address, QNetworkInterface::allAddresses())
    {
#ifdef QT_4
        if (local == address || address.protocol() != QAbstractSocket::IPv4Protocol) continue;
#else
        if (local == address
            ||
            address.protocol() != QAbstractSocket::IPv4Protocol || address.isLoopback()) continue;
#endif

        host = address.toString();

        break;
    }

    if (host.isEmpty()) return QString();

    host += ':' + QString::number(port);

    QString name = QHostInfo::localHostName();

    if (prefix.isEmpty())
    {
        return host + '/' + name;
    }
    else return prefix + '/' + host + '/' + name;
}

/* Q_INVOKABLE static */
WAbstractThreadAction * WBroadcastServer::startSource(int port, const QString & prefix,
                                                                QObject       * receiver,
                                                                const char    * method)
{
    WBroadcastServerSource * action = new WBroadcastServerSource(port, prefix);

    WBroadcastServerReply * reply = qobject_cast<WBroadcastServerReply *>
                                    (wControllerFile->startWriteAction(action));

    if (receiver) connect(reply, SIGNAL(complete(const QString &)), receiver, method);

    return action;
}

//-------------------------------------------------------------------------------------------------
// Events
//-------------------------------------------------------------------------------------------------

/* virtual */ bool WBroadcastServer::event(QEvent * event)
{
    QEvent::Type type = event->type();

    if (type == static_cast<QEvent::Type> (WBroadcastServerPrivate::EventConnected))
    {
        Q_D(WBroadcastServer);

        qDebug("WBroadcastClient: Connected.");

        d->setConnected(true);

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WBroadcastServerPrivate::EventDisconnected))
    {
        Q_D(WBroadcastServer);

        qDebug("WBroadcastClient: Disconnected.");

        d->setConnected(false);

        return true;
    }
    else return QObject::event(event);
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

bool WBroadcastServer::isConnected() const
{
    Q_D(const WBroadcastServer); return d->connected;
}

#endif // SK_NO_BROADCASTSERVER

#include "WBroadcastServer.moc"
