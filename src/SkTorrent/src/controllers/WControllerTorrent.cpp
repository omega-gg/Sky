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

#include "WControllerTorrent.h"

#ifndef SK_NO_CONTROLLERTORRENT

// Qt includes
#include <QThread>

// Sk includes
#include <WControllerApplication>
#include <WControllerFile>
#include <WControllerNetwork>
#include <WControllerDownload>
#include <WTorrentEngine>

W_INIT_CONTROLLER(WControllerTorrent)

//=================================================================================================
// Static functions
//=================================================================================================

inline bool sortA(const WTorrentItemData & itemA, const WTorrentItemData & itemB)
{
    return (itemA.name.toLower() < itemB.name.toLower());
}

inline bool sortB(const WTorrentItemData & itemA, const WTorrentItemData & itemB)
{
    return (itemA.index < itemB.index);
}

//=================================================================================================
// WTorrent
//=================================================================================================
// Private

WTorrent::WTorrent(const QString & url, int index, Mode mode, QObject * parent) : QObject(parent)
{
    _url   = url;
    _index = index;

    _mode = mode;

    _loaded = false;

    _size = 0;

    _progress = 0;

    _bufferPieces = 0;
    _bufferBlocks = 0;

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

        foreach (WTorrentReply * reply, _replies)
        {
            emit reply->added(reply);
        }

        return true;
    }
    else if (type == static_cast<QEvent::Type> (EventRemove))
    {
        deleteLater();

        return true;
    }
    else if (type == static_cast<QEvent::Type> (EventProgress))
    {
        WTorrentEventProgress * eventTorrent = static_cast<WTorrentEventProgress *> (event);

        _progress = eventTorrent->progress;

        _download = eventTorrent->download;
        _upload   = eventTorrent->upload;

        _seeds = eventTorrent->seeds;
        _peers = eventTorrent->peers;

        foreach (WTorrentReply * reply, _replies)
        {
            emit reply->progress(_progress, _size);
        }

        return true;
    }
    else if (type == static_cast<QEvent::Type> (EventBuffer))
    {
        WTorrentEventBuffer * eventTorrent = static_cast<WTorrentEventBuffer *> (event);

        _bufferPieces = eventTorrent->bufferPieces;
        _bufferBlocks = eventTorrent->bufferBlocks;

        foreach (WTorrentReply * reply, _replies)
        {
            emit reply->buffer(_bufferPieces, _bufferBlocks);
        }

        return true;
    }
    else if (type == static_cast<QEvent::Type> (EventSeek))
    {
        WTorrentEventSeek * eventTorrent = static_cast<WTorrentEventSeek *> (event);

        _bufferPieces = eventTorrent->bufferPieces;
        _bufferBlocks = eventTorrent->bufferBlocks;

        foreach (WTorrentReply * reply, _replies)
        {
            emit reply->seek(_bufferPieces, _bufferBlocks);
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

QString WTorrent::url() const
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

//-------------------------------------------------------------------------------------------------

qint64 WTorrent::progress() const
{
    return _progress;
}

//-------------------------------------------------------------------------------------------------

qint64 WTorrent::bufferPieces() const
{
    return _bufferPieces;
}

qint64 WTorrent::bufferBlocks() const
{
    return _bufferBlocks;
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
// WMagnet
//=================================================================================================
// Private

WMagnet::WMagnet(const QString & url, QObject * parent) : QObject(parent)
{
    _url = url;
}

//-------------------------------------------------------------------------------------------------
// Protected events
//-------------------------------------------------------------------------------------------------

/* virtual */ bool WMagnet::event(QEvent * event)
{
    QEvent::Type type = event->type();

    if (type == static_cast<QEvent::Type> (WTorrent::EventRemove))
    {
        deleteLater();

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WTorrent::EventFinished))
    {
        WTorrentEventMagnet * eventMagnet = static_cast<WTorrentEventMagnet *> (event);

        WControllerTorrentPrivate::applyMagnet(this, eventMagnet->data);

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WTorrent::EventError))
    {
        WTorrentEventValue * eventTorrent = static_cast<WTorrentEventValue *> (event);

        _error = eventTorrent->value.toString();

        qWarning("WMagnet::event: Magnet error: %s", _error.C_STR);

        foreach (WMagnetReply * reply, _replies)
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

QString WMagnet::url() const
{
    return _url;
}

//-------------------------------------------------------------------------------------------------

QByteArray WMagnet::data() const
{
    return _data;
}

//-------------------------------------------------------------------------------------------------

bool WMagnet::hasError() const
{
    return (_error.isEmpty() == false);
}

QString WMagnet::error() const
{
    return _error;
}

//=================================================================================================
// WMagnetReply
//=================================================================================================
// Private

WMagnetReply::WMagnetReply(QObject * parent) : QObject(parent) {}

//-------------------------------------------------------------------------------------------------
// Public

/* virtual */ WMagnetReply::~WMagnetReply()
{
    W_GET_CONTROLLER(WControllerTorrent, controller);

    if (controller)
    {
        controller->d_func()->removeMagnet(_magnet, this);
    }
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

WMagnet * WMagnetReply::magnet() const
{
    return _magnet;
}

//=================================================================================================
// WControllerTorrentPrivate
//=================================================================================================

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

void WControllerTorrentPrivate::init(const QString & path, int port, qint64 sizeMax)
{
    Q_Q(WControllerTorrent);

    qRegisterMetaType<WTorrent *>("WTorrent *");

    thread = new QThread(q);

    this->port = port;

    thread->start();

    engine = new WTorrentEngine(path, sizeMax, thread);
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WControllerTorrentPrivate::loadTorrent(WTorrentReply * reply,
                                            const QString & url, WTorrent::Mode mode)
{
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

        foreach (WTorrent * torrent, torrents)
        {
            if (torrent->_url == url && torrent->_mode == WTorrent::Default)
            {
                reply->_torrent = torrent;

                torrent->_replies.append(reply);

                return;
            }
        }
    }

    Q_Q(WControllerTorrent);

    QString source = WControllerNetwork::removeUrlFragment(url);

    QString fileUrl = wControllerFile->getFileUrl(source);

    WRemoteData * data;

    if (fileUrl.isEmpty())
    {
        WAbstractLoader * loader;

        if (source.startsWith("magnet:?", Qt::CaseInsensitive))
        {
             loader = loaders.value(WBackendNetQuery::TypeTorrent);
        }
        else loader = loaders.value(WBackendNetQuery::TypeDefault);

        data = wControllerDownload->getData(loader, source, q, QNetworkRequest::HighPriority);
    }
    else data = wControllerDownload->getData(WControllerFile::fileUrl(fileUrl), q,
                                             QNetworkRequest::HighPriority);

    QObject::connect(data, SIGNAL(loaded(WRemoteData *)), q, SLOT(onLoaded(WRemoteData *)));

    WTorrent * torrent = new WTorrent(source, extractIndex(url), mode, q);

    reply->_torrent = torrent;

    torrent->_replies.append(reply);

    torrents.append(torrent);

    jobs.insert(data, torrent);
}

void WControllerTorrentPrivate::loadMagnet(WMagnetReply * reply, const QString & url)
{
    QHashIterator<WRemoteData *, WMagnet *> i(jobsMagnets);

    while (i.hasNext())
    {
        i.next();

        WMagnet * magnet = i.value();

        if (magnet->_url == url)
        {
            reply->_magnet = magnet;

            magnet->_replies.append(reply);

            return;
        }
    }

    foreach (WMagnet * magnet, magnets)
    {
        if (magnet->_url == url)
        {
            reply->_magnet = magnet;

            magnet->_replies.append(reply);

            return;
        }
    }

    Q_Q(WControllerTorrent);

    WMagnet * magnet = new WMagnet(url, q);

    reply->_magnet = magnet;

    magnet->_replies.append(reply);

    magnets.append(magnet);

    QString fileUrl = wControllerFile->getFileUrl(url);

    if (fileUrl.isEmpty())
    {
        magnet->_cache = false;

        engine->loadMagnet(magnet);
    }
    else
    {
        magnet->_cache = true;

        WRemoteData * data = wControllerDownload->getData(WControllerFile::fileUrl(fileUrl), q,
                                                          QNetworkRequest::HighPriority);

        QObject::connect(data, SIGNAL(loaded      (WRemoteData *)),
                         q,    SLOT(onMagnetLoaded(WRemoteData *)));

        jobsMagnets.insert(data, magnet);
    }
}

//-------------------------------------------------------------------------------------------------

void WControllerTorrentPrivate::removeTorrent(WTorrent * torrent, WTorrentReply * reply)
{
    QList<WTorrentReply *> & replies = torrent->_replies;

    replies.removeOne(reply);

    if (replies.isEmpty() == false) return;

    torrents.removeOne(torrent);

    WRemoteData * data = jobs.key(torrent);

    if (data)
    {
        jobs.remove(data);

        delete data;

        delete torrent;
    }
    else engine->remove(torrent);
}

void WControllerTorrentPrivate::removeMagnet(WMagnet * magnet, WMagnetReply * reply)
{
    QList<WMagnetReply *> & replies = magnet->_replies;

    replies.removeOne(reply);

    if (replies.isEmpty() == false) return;

    magnets.removeOne(magnet);

    WRemoteData * data = jobsMagnets.key(magnet);

    if (data)
    {
        jobsMagnets.remove(data);

        delete data;

        delete magnet;
    }
    else if (magnet->_cache)
    {
        delete magnet;
    }
    else engine->removeMagnet(magnet);
}

//-------------------------------------------------------------------------------------------------

int WControllerTorrentPrivate::extractIndex(const QString & url) const
{
    QString fragment = QUrl(url).fragment();

    if (fragment.isEmpty()) return -1;

    fragment = fragment.mid(0, fragment.indexOf('.'));

    int index = fragment.toInt();

    if (index > 0)
    {
         return index - 1;
    }
    else return -1;
}

//-------------------------------------------------------------------------------------------------
// Private static functions
//-------------------------------------------------------------------------------------------------

/* static */ void WControllerTorrentPrivate::applyMagnet(WMagnet * magnet, const QByteArray & data)
{
    magnet->_data = data;

    if (data.isEmpty() == false)
    {
        wControllerFile->addCache(magnet->_url, data);
    }

    foreach (WMagnetReply * reply, magnet->_replies)
    {
        emit reply->loaded(reply);
    }
}

//-------------------------------------------------------------------------------------------------

/* static */ int WControllerTorrentPrivate::listAfter(const QString & text,
                                                      const QString & string, int * at)
{
    int from = *at;

    from++;

    while (from < text.length())
    {
        QChar character = text.at(from);

        if (character == 'e')
        {
            *at = from + 1;

            return -1;
        }

        WControllerTorrent::Type type = WControllerTorrent::getType(character);

        if (type == WControllerTorrent::Null)
        {
            *at = -1;

            return -1;
        }

        if (type == WControllerTorrent::String)
        {
            int index = text.indexOf(':', from);

            if (index == -1)
            {
                *at = -1;

                return -1;
            }

            int length = text.mid(from, index - from).toInt();

            index++;

            from = index + length;

            if (string.length() == length && string == text.mid(index, length))
            {
                return from;
            }
        }
        else if (type == WControllerTorrent::Integer)
        {
            from = WControllerTorrent::skipInteger(text, from);
        }
        else // if (type == WControllerTorrent::List || type == WControllerTorrent::Dictionary)
        {
            int index = listAfter(text, string, &from);

            if (index != -1)
            {
                return index;
            }
            else if (from == -1)
            {
                *at = -1;

                return -1;
            }
        }
    }

    *at = from;

    return -1;
}

//-------------------------------------------------------------------------------------------------

/* static */ int WControllerTorrentPrivate::extractItem(WTorrentItemData * item,
                                                        const QString    & data, int at)
{
    int index = data.indexOf(':', at);

    if (index == -1) return -1;

    int length = data.mid(at, index - at).toInt();

    if (length == 0) return -1;

    index++;

    QString string = data.mid(index, length);

    at = index + length;

    if (at < data.length())
    {
        if (data.at(at) == 'e')
        {
            item->name  = string;
            item->index = getIndex(string);

            at++;
        }
        else
        {
            item->path.append(string);

            return extractItem(item, data, at);
        }
    }

    return at;
}

//-------------------------------------------------------------------------------------------------

/* static */ int WControllerTorrentPrivate::extractString(QString       * string,
                                                          const QString & data, int at)
{
    int index = data.indexOf(':', at);

    if (index == -1) return -1;

    int length = data.mid(at, index - at).toInt();

    if (length == 0) return -1;

    index++;

    *string = data.mid(index, length);

    return index + length;
}

//-------------------------------------------------------------------------------------------------

/* static */ WTorrentItemFolder
WControllerTorrentPrivate::extractFolder(QList<WTorrentItemData> * items)
{
    QList<WTorrentItemData> list;

    WTorrentItemData item = items->takeAt(0);

    QString path = item.path;

    list.append(item);

    int index = 0;

    while (index < items->length())
    {
        if (items->at(index).path == path)
        {
            item = items->takeAt(index);

            list.append(item);
        }
        else index++;
    }

    if (list.first().index == -1)
    {
         std::sort(list.begin(), list.end(), sortA);
    }
    else std::sort(list.begin(), list.end(), sortB);

    WTorrentItemFolder folder;

    folder.items = list;

    return folder;
}

//-------------------------------------------------------------------------------------------------

/* static */ int WControllerTorrentPrivate::getIndex(const QString & name)
{
    QString string;

    for (int i = 0; i < name.length(); i++)
    {
        QChar character = name.at(i);

        if (character.isDigit() == false) break;

        string.append(character);
    }

    if (string.isEmpty())
    {
         return -1;
    }
    else return string.toInt();
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WControllerTorrentPrivate::onLoaded(WRemoteData * data)
{
    WTorrent * torrent = jobs.take(data);

    if (data->hasError() || data->reply()->size() == 0)
    {
        qWarning("WControllerTorrentPrivate::onLoaded: Failed to load torrent %s.",
                 data->url().C_STR);

        torrent->_error = data->error();

        foreach (WTorrentReply * reply, torrent->_replies)
        {
            emit reply->loaded(reply);
        }
    }
    else engine->load(torrent, data->takeReply());

    delete data;
}

void WControllerTorrentPrivate::onMagnetLoaded(WRemoteData * data)
{
    WMagnet * magnet = jobsMagnets.take(data);

    if (data->hasError())
    {
        qWarning("WControllerTorrentPrivate::onMagnetLoaded: Failed to load magnet %s.",
                 data->url().C_STR);

        magnet->_error = data->error();

        foreach (WMagnetReply * reply, magnet->_replies)
        {
            emit reply->loaded(reply);
        }
    }
    else applyMagnet(magnet, data->readAll());

    delete data;
}

//=================================================================================================
// WControllerTorrent
//=================================================================================================

WControllerTorrent::WControllerTorrent() : WController(new WControllerTorrentPrivate(this)) {}

//-------------------------------------------------------------------------------------------------
// Initialize
//-------------------------------------------------------------------------------------------------

/* virtual */ void WControllerTorrent::initController(const QString & path,
                                                      int port, qint64 sizeMax)
{
    Q_D(WControllerTorrent);

    if (d->created == false)
    {
        d->created = true;

        d->init(path, port, sizeMax);
    }
    else qWarning("WControllerTorrent::initController: Controller is already initialized.");
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ WTorrentReply * WControllerTorrent::getTorrent(const QString  & url,
                                                                 QObject        * parent,
                                                                 WTorrent::Mode   mode)
{
    Q_D(WControllerTorrent);

    WTorrentReply * reply;

    if (parent) reply = new WTorrentReply(parent);
    else        reply = new WTorrentReply(this);

    d->loadTorrent(reply, url, mode);

    return reply;
}

/* Q_INVOKABLE */ WMagnetReply * WControllerTorrent::getMagnet(const QString & url,
                                                               QObject       * parent)
{
    Q_D(WControllerTorrent);

    WMagnetReply * reply;

    if (parent) reply = new WMagnetReply(parent);
    else        reply = new WMagnetReply(this);

    d->loadMagnet(reply, url);

    return reply;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WControllerTorrent::clearSource(const QString & url)
{
    Q_D(WControllerTorrent);

    d->engine->clearSource(url);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WControllerTorrent::clearTorrents()
{
    Q_D(WControllerTorrent);

    QHashIterator<WRemoteData *, WTorrent *> i(d->jobs);

    while (i.hasNext())
    {
        i.next();

        foreach (WTorrentReply * reply, i.value()->_replies)
        {
            delete reply;
        }
    }

    QHashIterator<WRemoteData *, WMagnet *> j(d->jobsMagnets);

    while (j.hasNext())
    {
        j.next();

        foreach (WMagnetReply * reply, j.value()->_replies)
        {
            delete reply;
        }
    }

    foreach (WTorrent * torrent, d->torrents)
    {
        foreach (WTorrentReply * reply, torrent->_replies)
        {
            delete reply;
        }
    }

    foreach (WMagnet * magnet, d->magnets)
    {
        foreach (WMagnetReply * reply, magnet->_replies)
        {
            delete reply;
        }
    }

    d->engine->clearCache();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WControllerTorrent::clearCache()
{
    Q_D(WControllerTorrent);

    d->engine->clearCache();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ int WControllerTorrent::registerPort()
{
    Q_D(WControllerTorrent);

    return d->ports.generateId(d->port);
}

/* Q_INVOKABLE */ void WControllerTorrent::unregisterPort(int port)
{
    Q_D(WControllerTorrent);

    d->ports.removeOne(port);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WControllerTorrent::setOptions(int connections, int upload, int download)
{
    Q_D(WControllerTorrent);

    d->engine->setOptions(connections, upload, download);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WControllerTorrent::setProxy(const QString & host,
                                                    int             port, const QString & password)
{
    Q_D(WControllerTorrent);

    d->engine->setProxy(host, port, password);
}

/* Q_INVOKABLE */ void WControllerTorrent::clearProxy()
{
    Q_D(WControllerTorrent);

    d->engine->clearProxy();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WControllerTorrent::registerLoader(WBackendNetQuery::Type type,
                                                          WAbstractLoader      * loader)
{
    Q_D(WControllerTorrent);

    d->loaders.insert(type, loader);
}

/* Q_INVOKABLE */ void WControllerTorrent::unregisterLoader(WBackendNetQuery::Type type)
{
    Q_D(WControllerTorrent);

    d->loaders.remove(type);
}

/* Q_INVOKABLE */ void WControllerTorrent::unregisterLoaders()
{
    Q_D(WControllerTorrent);

    d->loaders.clear();
}

//-------------------------------------------------------------------------------------------------
// Static functions
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */
QList<WTorrentItemData> WControllerTorrent::torrentItems(const QString & data)
{
    QList<WTorrentItemData> items;

    QString list = listAfter(data, "files");

    int index = indexAfter(list, "path");

    int id = 1;

    while (index != -1)
    {
        WTorrentItemData item;

        item.id = id;

        QChar character = list.at(index);

        if (character == 'l')
        {
            index++;

            index = WControllerTorrentPrivate::extractItem(&item, list, index);

            if (index == -1) return items;

            if (item.name.isEmpty() == false)
            {
                items.append(item);
            }
        }
        else
        {
            QString name;

            index = WControllerTorrentPrivate::extractString(&name, list, index);

            if (index == -1) return items;

            if (name.isEmpty() == false)
            {
                item.name  = name;
                item.index = WControllerTorrentPrivate::getIndex(name);

                items.append(item);
            }
        }

        index = indexAfter(list, "path", index);

        id++;
    }

    return items;
}

/* Q_INVOKABLE static */
QList<WTorrentItemFolder> WControllerTorrent::torrentFolders(const QList<WTorrentItemData> & items)
{
    QList<WTorrentItemFolder> list;

    QList<WTorrentItemData> data = items;

    while (data.isEmpty() == false)
    {
        list.append(WControllerTorrentPrivate::extractFolder(&data));
    }

    return list;
}

//-------------------------------------------------------------------------------------------------
// Bencode

/* Q_INVOKABLE static */
WControllerTorrent::Type WControllerTorrent::extractType(const QString & text, int at)
{
    if (at < 0 || at >= text.length())
    {
         return Null;
    }
    else return getType(text.at(at));
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerTorrent::extractString(const QString & text, int at)
{
    int index = text.indexOf(':', at);

    if (index == -1) return QString();

    int length = text.mid(at, index - at).toInt();

    if (length)
    {
         return text.mid(index + 1, length);
    }
    else return QString();
}

/* Q_INVOKABLE static */ int WControllerTorrent::extractInteger(const QString & text, int at)
{
    at++;

    int index = text.indexOf('e', at);

    if (index == -1)
    {
         return -1;
    }
    else return text.mid(at, index - at).toInt();
}

/* Q_INVOKABLE static */ QString WControllerTorrent::extractList(const QString & text, int at)
{
    int index = skipList(text, at);

    if (index != -1)
    {
        at++;

        return text.mid(at, index - at);
    }
    else return QString();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ int WControllerTorrent::indexAfter(const QString & text,
                                                            const QString & string, int at)
{
    while (at < text.length())
    {
        QChar character = text.at(at);

        if (character == 'e')
        {
            at++;

            continue;
        }

        Type type = getType(character);

        if (type == Null) return -1;

        if (type == String)
        {
            int index = text.indexOf(':', at);

            if (index == -1) return -1;

            int length = text.mid(at, index - at).toInt();

            index++;

            at = index + length;

            if (string.length() == length && string == text.mid(index, length))
            {
                return at;
            }
        }
        else if (type == Integer)
        {
            at = skipInteger(text, at);
        }
        else // if (type == List || type == Dictionary)
        {
            int index = WControllerTorrentPrivate::listAfter(text, string, &at);

            if (index != -1)
            {
                return index;
            }
            else if (at == -1)
            {
                return -1;
            }
        }
    }

    return -1;
}

/* Q_INVOKABLE static */ QString WControllerTorrent::stringAfter(const QString & text,
                                                                 const QString & string, int at)
{
    int index = indexAfter(text, string, at);

    if (index == -1)
    {
         return QString();
    }
    else return extractString(text, index);
}

/* Q_INVOKABLE static */ int WControllerTorrent::integerAfter(const QString & text,
                                                              const QString & string, int at)
{
    int index = indexAfter(text, string, at);

    if (index == -1)
    {
         return -1;
    }
    else return extractInteger(text, index);
}

/* Q_INVOKABLE static */ QString WControllerTorrent::listAfter(const QString & text,
                                                               const QString & string, int at)
{
    int index = indexAfter(text, string, at);

    if (index == -1)
    {
         return QString();
    }
    else return extractList(text, index);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ int WControllerTorrent::skipString(const QString & text, int at)
{
    int index = text.indexOf(':', at);

    if (index == -1) return at;

    QString length = text.mid(at, index - at);

    return index + 1 + length.toInt();
}

/* Q_INVOKABLE static */ int WControllerTorrent::skipInteger(const QString & text, int at)
{
    at++;

    int index = text.indexOf('e', at);

    if (index == -1)
    {
         return at;
    }
    else return index + 1;
}

/* Q_INVOKABLE static */ int WControllerTorrent::skipList(const QString & text, int at)
{
    at++;

    while (at < text.length())
    {
        QChar character = text.at(at);

        if (character == 'e')
        {
            return at + 1;
        }

        Type type = getType(character);

        if (type == Null) return at;

        if (type == String)
        {
            at = skipString(text, at);
        }
        else if (type == Integer)
        {
            at = skipInteger(text, at);
        }
        else // if (type == List || type == Dictionary)
        {
            at = skipList(text, at);
        }
    }

    return at;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QStringList WControllerTorrent::splitList(const QString & text)
{
    QStringList list;

    int index = 0;
    int at    = skipList(text);

    while (at < text.length())
    {
        QString string = text.mid(index, at - index);

        list.append(string);

        index = at;

        at = skipList(text, index);
    }

    return list;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */
WControllerTorrent::Type WControllerTorrent::getType(const QChar & character)
{
    if      (character == 'i')    return WControllerTorrent::Integer;
    else if (character == 'l')    return WControllerTorrent::List;
    else if (character == 'd')    return WControllerTorrent::Dictionary;
    else if (character.isDigit()) return WControllerTorrent::String;
    else                          return WControllerTorrent::Null;
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

WTorrentEngine * WControllerTorrent::engine() const
{
    Q_D(const WControllerTorrent); return d->engine;
}

//-------------------------------------------------------------------------------------------------

int WControllerTorrent::port() const
{
    Q_D(const WControllerTorrent); return d->port;
}

void WControllerTorrent::setPort(int port)
{
    Q_D(WControllerTorrent);

    if (d->port == port) return;

    d->port = port;

    emit portChanged();
}

//-------------------------------------------------------------------------------------------------

qint64 WControllerTorrent::sizeMax() const
{
    Q_D(const WControllerTorrent); return d->engine->sizeMax();
}

void WControllerTorrent::setSizeMax(qint64 max)
{
    Q_D(WControllerTorrent); d->engine->setSizeMax(max);
}

#endif // SK_NO_CONTROLLERTORRENT
