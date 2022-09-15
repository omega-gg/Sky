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

#include "WBroadcastClient.h"

#ifndef SK_NO_BROADCASTCLIENT

// Qt includes
#include <QCoreApplication>
#include <QThread>
#include <QEvent>
#include <QTcpSocket>
#include <QHostInfo>

// Sk includes
#include <WControllerNetwork>

//=================================================================================================
// WBroadcastClientThread
//=================================================================================================

class WBroadcastClientThread : public QThread
{
    Q_OBJECT

public: // Enums
    enum EventType
    {
        EventConnect = QEvent::User,
        EventDisconnect
    };

public:
    WBroadcastClientThread();

protected: // QThread reimplementation
    /* virtual */ void run();

protected: // Events
    /* virtual */ bool event(QEvent * event);

private: // Functions
    void clearSocket();

private slots:
    void onConnected   ();
    void onDisconnected();

    void onRead();

private:
    QTcpSocket * socket;
};

//=================================================================================================
// WBroadcastClientConnect
//=================================================================================================

class WBroadcastClientConnect : public QEvent
{
public:
    WBroadcastClientConnect(const QString & address, int port)
        : QEvent(static_cast<QEvent::Type> (WBroadcastClientThread::EventConnect))
    {
        this->address = address;
        this->port    = port;
    }

public: // Variables
    QString address;
    int     port;
};

//=================================================================================================
// WBroadcastClientThread
//=================================================================================================

WBroadcastClientThread::WBroadcastClientThread()
{
    socket = NULL;

    moveToThread(this);

    start(QThread::IdlePriority);
}

//-------------------------------------------------------------------------------------------------
// Protected QThread reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WBroadcastClientThread::run()
{
    exec();
}

//-------------------------------------------------------------------------------------------------
// Events
//-------------------------------------------------------------------------------------------------

/* virtual */ bool WBroadcastClientThread::event(QEvent * event)
{
    QEvent::Type type = event->type();

    if (type == static_cast<QEvent::Type> (EventConnect))
    {
        WBroadcastClientConnect * eventConnect = static_cast<WBroadcastClientConnect *> (event);

        QString address = eventConnect->address;
        int     port    = eventConnect->port;

        qDebug("WBroadcastClientThread: Connecting [%s] on port %d...",
               address.toLatin1().constData(), port);

        if (socket) clearSocket();

        socket = new QTcpSocket(this);

        socket->connectToHost(QHostAddress(address), port);

        connect(socket, SIGNAL(connected   ()), this, SLOT(onConnected   ()));
        connect(socket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));

        connect(socket, SIGNAL(readyRead()), this, SLOT(onRead()));

        return true;
    }
    else if (type == static_cast<QEvent::Type> (EventDisconnect))
    {
        qDebug("WBroadcastClientThread: Disconnecting...");

        return true;
    }
    else return QThread::event(event);
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WBroadcastClientThread::onConnected()
{

}

void WBroadcastClientThread::onDisconnected()
{
    qDebug("WBroadcastClientThread: Disconnected.");

    if (socket == NULL) return;

    disconnect(socket, 0, this, 0);

    socket->deleteLater();

    socket = NULL;
}

void WBroadcastClientThread::onRead()
{

}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WBroadcastClientThread::clearSocket()
{
    // NOTE: We need to disconnect to avoid receiving a disconnect signal upon deletion.
    disconnect(socket, 0, this, 0);

    delete socket;

    socket = NULL;
}

//=================================================================================================
// WBroadcastClientPrivate
//=================================================================================================

#include "WBroadcastClient_p.h"

WBroadcastClientPrivate::WBroadcastClientPrivate(WBroadcastClient * p) : WPrivate(p) {}

/* virtual */ WBroadcastClientPrivate::~WBroadcastClientPrivate()
{
    thread->quit();
    thread->wait();

    delete thread;
}

//-------------------------------------------------------------------------------------------------

void WBroadcastClientPrivate::init()
{
    thread = new WBroadcastClientThread();

    connected = false;
}

//=================================================================================================
// WBroadcastClient
//=================================================================================================

/* explicit */ WBroadcastClient::WBroadcastClient(QObject * parent)
    : QObject(parent), WPrivatable(new WBroadcastClientPrivate(this))
{
    Q_D(WBroadcastClient); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WBroadcastClient::connectHost(const QString & address, int port)
{
    Q_D(WBroadcastClient);

    QCoreApplication::postEvent(d->thread, new WBroadcastClientConnect(address, port));
}

/* Q_INVOKABLE */ void WBroadcastClient::disconnectHost()
{
    Q_D(WBroadcastClient);

    QCoreApplication::postEvent(d->thread, new QEvent(static_cast<QEvent::Type>
                                                      (WBroadcastClientThread::EventDisconnect)));
}

//-------------------------------------------------------------------------------------------------
// Static functions
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ WBroadcastSource WBroadcastClient::extractSource(const QString & url)
{
    WBroadcastSource source;

    source.port = 0;

    QStringList list = WControllerNetwork::removeUrlPrefix(url).split('/');

    int count = list.count();

    if (count < 3 || list.at(1).toLower() != "connect") return source;

    QStringList host = list.at(2).split(':');

    if (host.count() < 2) return source;

    source.ip   = host.at(0);
    source.port = host.at(1).toInt();

    if (count > 3) source.name = list.at(3);

    return source;
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

bool WBroadcastClient::isConnected() const
{
    Q_D(const WBroadcastClient); return d->connected;
}

#endif // SK_NO_BROADCASTCLIENT

#include "WBroadcastClient.moc"
