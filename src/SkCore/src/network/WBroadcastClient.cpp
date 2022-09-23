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
#include <QHostInfo>
#include <QtEndian>

// Sk includes
#include <WControllerApplication>
#include <WControllerNetwork>

// Private includes
#include "WBroadcastClient_p.h"

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
        EventDisconnect,
        EventMessage
    };

public:
    WBroadcastClientThread(WBroadcastClient * parent);

protected: // QThread reimplementation
    /* virtual */ void run();

protected: // Events
    /* virtual */ bool event(QEvent * event);

private: // Functions
    void clearSocket();

    void setConnected(bool connected);

private slots:
    void onConnected   ();
    void onDisconnected();

    void onRead();

private:
    WBroadcastClient * parent;

    QTcpSocket * socket;

    bool connected;
};

//=================================================================================================
// WBroadcastClientThread events
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

class WBroadcastClientMessage : public QEvent
{
public:
    WBroadcastClientMessage(const QList<WBroadcastMessage> & messages)
        : QEvent(static_cast<QEvent::Type> (WBroadcastClientThread::EventMessage))
    {
        this->messages = messages;
    }

public: // Variables
    QList<WBroadcastMessage> messages;
};

//=================================================================================================
// WBroadcastClientThread
//=================================================================================================

WBroadcastClientThread::WBroadcastClientThread(WBroadcastClient * parent)
{
    this->parent = parent;

    socket = NULL;

    connected = false;

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

        if (socket) clearSocket();

        setConnected(false);

        return true;
    }
    else if (type == static_cast<QEvent::Type> (EventMessage))
    {
        if (socket == NULL)
        {
            qWarning("WBroadcastClientThread::EventMessage: Not connected.");

            return true;
        }

        WBroadcastClientMessage * eventMessage = static_cast<WBroadcastClientMessage *> (event);

        QByteArray data;

        foreach (const WBroadcastMessage & message, eventMessage->messages)
        {
            data.append(message.generateData());
        }

        int length = data.length();

        if (length == 0)
        {
            qWarning("WBroadcastClientThread::EventMessage: Data is empty.");
        }
        else socket->write(data.constData(), data.length());

        return true;
    }
    else return QThread::event(event);
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WBroadcastClientThread::onConnected()
{
    setConnected(true);
}

void WBroadcastClientThread::onDisconnected()
{
    if (socket == NULL) return;

    disconnect(socket, 0, this, 0);

    socket->deleteLater();

    socket = NULL;

    setConnected(false);
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

void WBroadcastClientThread::setConnected(bool connected)
{
    if (this->connected == connected) return;

    this->connected = connected;

    if (connected)
    {
         QCoreApplication::postEvent(parent,
                                     new QEvent(static_cast<QEvent::Type>
                                                (WBroadcastClientPrivate::EventConnected)));
    }
    else QCoreApplication::postEvent(parent,
                                     new QEvent(static_cast<QEvent::Type>
                                                (WBroadcastClientPrivate::EventDisconnected)));
}

//=================================================================================================
// WBroadcastSource
//=================================================================================================

WBroadcastSource::WBroadcastSource()
{
    port = 0;
}

//-------------------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------------------

bool WBroadcastSource::isValid() const
{
    return (address.isEmpty() == false);
}

//-------------------------------------------------------------------------------------------------
// Operators
//-------------------------------------------------------------------------------------------------

WBroadcastSource::WBroadcastSource(const WBroadcastSource & other)
{
    *this = other;
}

bool WBroadcastSource::operator==(const WBroadcastSource & other) const
{
    return (address == other.address && port == other.port && name == other.name);
}

WBroadcastSource & WBroadcastSource::operator=(const WBroadcastSource & other)
{
    address = other.address;
    port    = other.port;

    name = other.name;

    return *this;
}

//=================================================================================================
// WBroadcastMessage
//=================================================================================================

/* explicit */ WBroadcastMessage::WBroadcastMessage(Type type, const QStringList & parameters)
{
    this->type       = type;
    this->parameters = parameters;
}

/* explicit */ WBroadcastMessage::WBroadcastMessage(const QByteArray & data)
{
    QString content = QString::fromUtf8(data);

    QString name = extractName(&content);

    Type type = typeFromString(name);

    if (type == Unknown) return;

    while (content.isEmpty() == false)
    {
        QString parameter = extractParameter(&content);

        parameters.append(parameter);
    }

    //---------------------------------------------------------------------------------------------
    // NOTE: Checking parameters according to the expected API.

    if (type == SOURCE)
    {
        if (parameters.count() != 3) return;
    }
    else if (parameters.isEmpty() == false) return;

    //---------------------------------------------------------------------------------------------

    this->type = type;
}

WBroadcastMessage::WBroadcastMessage()
{
    type = Unknown;
}

//-------------------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------------------

bool WBroadcastMessage::isValid() const
{
    return (type != Unknown);
}

QByteArray WBroadcastMessage::generateData() const
{
    if (isValid() == false) return QByteArray();

    QString data;

    data.append(typeToString(type));

    foreach (QString parameter, parameters)
    {
        parameter.replace('"', "\\\"");

        data.append(" \"" + parameter + '"');
    }

    QByteArray array = data.toUtf8();

    qDebug("WBroadcastMessage::generateData: [%s]", array.constData());

    QByteArray bytes;

    WBroadcastClient::appendInt(&bytes, data.length());

    array.prepend(bytes, 4);

    return array;
}

//-------------------------------------------------------------------------------------------------
// Static functions
//-------------------------------------------------------------------------------------------------

/* static */ WBroadcastMessage::Type WBroadcastMessage::typeFromString(const QString & string)
{
    if      (string == "SOURCE") return SOURCE;
    else if (string == "PLAY")   return PLAY;
    else if (string == "REPLAY") return REPLAY;
    else if (string == "PAUSE")  return PAUSE;
    else if (string == "STOP")   return STOP;
    else                         return Unknown;
}

/* static */ QString WBroadcastMessage::typeToString(Type type)
{
    if      (type == SOURCE) return "SOURCE";
    else if (type == PLAY)   return "PLAY";
    else if (type == REPLAY) return "REPLAY";
    else if (type == PAUSE)  return "PAUSE";
    else if (type == STOP)   return "STOP";
    else                     return "";
}

//-------------------------------------------------------------------------------------------------
// Operators
//-------------------------------------------------------------------------------------------------

WBroadcastMessage::WBroadcastMessage(const WBroadcastMessage & other)
{
    *this = other;
}

bool WBroadcastMessage::operator==(const WBroadcastMessage & other) const
{
    return (type == other.type && parameters == other.parameters);
}

WBroadcastMessage & WBroadcastMessage::operator=(const WBroadcastMessage & other)
{
    type       = other.type;
    parameters = other.parameters;

    return *this;
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

QString WBroadcastMessage::extractName(QString * data) const
{
    int index = data->indexOf(' ');

    QString name;

    if (index == -1)
    {
        name = *data;

        data->clear();
    }
    else
    {
        name = data->left(index);

        // NOTE: Remove the space and the first parameter double quote.
        data->remove(0, index + 2);
    }

    return name;
}

QString WBroadcastMessage::extractParameter(QString * data) const
{
    int index = data->indexOf('"');

    if (index == -1) return QString();

    while (Sk::checkEscaped(*data, index))
    {
        int at = data->indexOf('"', index + 1);

        if (at == -1) break;

        index = at;
    }

    QString result = data->left(index);

    result.replace("\\\"", "\"");

    // NOTE: Remove the double quote, the space and the next parameter double quote.
    data->remove(0, index + 3);

    return result;
}

//=================================================================================================
// WBroadcastBuffer
//=================================================================================================

WBroadcastBuffer::WBroadcastBuffer()
{
    state = HEADER;

    size = 4;
}

int WBroadcastBuffer::append(QByteArray * array)
{
    int length = array->length();

    if (length - size < 0)
    {
        data.append(*array);

        array->clear();

        size -= length;

        return 0;
    }

    data.append(array->left(size));

    array->remove(0, size);

    if (state == HEADER)
    {
        state = DATA;

        size = WBroadcastClient::getInt(data.constData());

        // NOTE: Removing the 'size' integer at the start.
        data.remove(0, 4);

        if (size == 0)
        {
            return -1;
        }
        else return 0;
    }
    else
    {
        size = 0;

        return 1;
    }
}

void WBroadcastBuffer::clear()
{
    data.clear();

    state = HEADER;

    size = 4;
}

//-------------------------------------------------------------------------------------------------

QByteArray WBroadcastBuffer::getData() const
{
    return data;
}

//=================================================================================================
// WBroadcastClientPrivate
//=================================================================================================

WBroadcastClientPrivate::WBroadcastClientPrivate(WBroadcastClient * p) : WPrivate(p) {}

/* virtual */ WBroadcastClientPrivate::~WBroadcastClientPrivate()
{
    if (thread == NULL) return;

    thread->quit();
    thread->wait();

    delete thread;
}

//-------------------------------------------------------------------------------------------------

void WBroadcastClientPrivate::init()
{
    thread = NULL;

    connected = false;
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WBroadcastClientPrivate::createThread()
{
    if (thread) return;

    Q_Q(WBroadcastClient);

    thread = new WBroadcastClientThread(q);
}

void WBroadcastClientPrivate::postEvent(QEvent * event)
{
    createThread();

    QCoreApplication::postEvent(thread, event);
}

void WBroadcastClientPrivate::setConnected(bool connected)
{
    if (this->connected == connected) return;

    Q_Q(WBroadcastClient);

    this->connected = connected;

    emit q->connectedChanged();
}

void WBroadcastClientPrivate::setSource(const WBroadcastSource & source)
{
    if (this->source == source) return;

    Q_Q(WBroadcastClient);

    this->source = source;

    emit q->sourceChanged();
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

/* Q_INVOKABLE */ bool WBroadcastClient::connectToHost(const WBroadcastSource & source)
{
    if (source.isValid() == false) return false;

    Q_D(WBroadcastClient);

    d->setSource(source);

    d->postEvent(new WBroadcastClientConnect(source.address, source.port));

    return true;
}

/* Q_INVOKABLE */ bool WBroadcastClient::connectToHost(const QString & url)
{
    return connectToHost(extractSource(url));
}

/* Q_INVOKABLE */ void WBroadcastClient::disconnectHost()
{
    Q_D(WBroadcastClient);

    d->postEvent(new QEvent(static_cast<QEvent::Type> (WBroadcastClientThread::EventDisconnect)));
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ bool WBroadcastClient::addMessage(const WBroadcastMessage & message)
{
    Q_D(WBroadcastClient);

    if (message.isValid() == false)
    {
        qWarning("WBroadcastClient::addMessage: Invalid message.");

        return false;
    }

    d->messages.append(message);

    emit messagesChanged();

    return true;
}

/* Q_INVOKABLE */ bool WBroadcastClient::addAndSend(const WBroadcastMessage & message)
{
    if (addMessage(message) == false)
    {
        return false;
    }

    return sendMessages();
}

/* Q_INVOKABLE */ bool WBroadcastClient::addMessage(WBroadcastMessage::Type type,
                                                    const QStringList & parameters)
{
    return addMessage(WBroadcastMessage(type, parameters));
}

/* Q_INVOKABLE */ bool WBroadcastClient::addAndSend(WBroadcastMessage::Type type,
                                                    const QStringList & parameters)
{
    return addAndSend(WBroadcastMessage(type, parameters));
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ bool WBroadcastClient::sendMessages()
{
    Q_D(WBroadcastClient);

    if (d->connected == false)
    {
        qWarning("WBroadcastClient::sendMessage: Not connected.");

        return false;
    }

    if (d->messages.isEmpty())
    {
        qWarning("WBroadcastClient::sendMessage: Messages are empty.");

        return false;
    }

    d->postEvent(new WBroadcastClientMessage(d->messages));

    d->messages.clear();

    emit messagesChanged();

    return true;
}

//-------------------------------------------------------------------------------------------------
// Static functions
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ WBroadcastSource WBroadcastClient::extractSource(const QString & url)
{
    WBroadcastSource source;

    QStringList list = WControllerNetwork::removeUrlPrefix(url).split('/');

    int count = list.count();

    if (count < 3 || list.at(1).toLower() != "connect") return source;

    QStringList host = list.at(2).split(':');

    if (host.count() < 2) return source;

    source.address = host.at(0);
    source.port    = host.at(1).toInt();

    if (count > 3) source.name = list.at(3);

    return source;
}

/* Q_INVOKABLE static */ void WBroadcastClient::appendInt(QByteArray * array, qint32 value)
{
    // NOTE: Converting to big endian for the QTcpSocket.
    value = qToBigEndian(value);

    for (int i = 0; i < 4; i++)
    {
        array->append(static_cast<char> (value));

        value >>= 8;
    }
}

/* Q_INVOKABLE static */ qint32 WBroadcastClient::getInt(const char * data)
{
#ifdef QT_4
    return qFromBigEndian<qint32>((const uchar *) data);
#else
    return qFromBigEndian<qint32>(data);
#endif
}

//-------------------------------------------------------------------------------------------------
// Events
//-------------------------------------------------------------------------------------------------

/* virtual */ bool WBroadcastClient::event(QEvent * event)
{
    QEvent::Type type = event->type();

    if (type == static_cast<QEvent::Type> (WBroadcastClientPrivate::EventConnected))
    {
        Q_D(WBroadcastClient);

        qDebug("WBroadcastClient: Connected.");

        d->setConnected(true);

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WBroadcastClientPrivate::EventDisconnected))
    {
        Q_D(WBroadcastClient);

        qDebug("WBroadcastClient: Disconnected.");

        d->setConnected(false);

        return true;
    }
    else return QObject::event(event);
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

bool WBroadcastClient::isConnected() const
{
    Q_D(const WBroadcastClient); return d->connected;
}

const WBroadcastSource & WBroadcastClient::source() const
{
    Q_D(const WBroadcastClient); return d->source;
}

const QList<WBroadcastMessage> & WBroadcastClient::messages() const
{
    Q_D(const WBroadcastClient); return d->messages;
}

#endif // SK_NO_BROADCASTCLIENT

#include "WBroadcastClient.moc"
