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

#include "WBackendTorrent.h"

#ifndef SK_NO_BACKENDTORRENT

// Qt includes
#ifdef QT_LATEST
#include <QUrlQuery>
#endif

// Sk includes
#include <WControllerApplication>
#include <WControllerNetwork>
#include <WControllerPlaylist>
#include <WControllerTorrent>

// Private include
#include <private/WBackendNet_p>

//-------------------------------------------------------------------------------------------------
// Static variables

static const int BACKENDTORRENT_MAX_URLS    = 20;
static const int BACKENDTORRENT_MAX_SOURCES =  5;

static const int BACKENDTORRENT_TIMEOUT_A =  3000;
static const int BACKENDTORRENT_TIMEOUT_B = 10000;

//=================================================================================================
// WBackendTorrentItem
//=================================================================================================

struct WBackendTorrentItem
{
    int id;

    QString path;
    QString name;

    int index;
};

//-------------------------------------------------------------------------------------------------

inline bool sortA(const WBackendTorrentItem & itemA, const WBackendTorrentItem & itemB)
{
    return (itemA.name.toLower() < itemB.name.toLower());
}

inline bool sortB(const WBackendTorrentItem & itemA, const WBackendTorrentItem & itemB)
{
    return (itemA.index < itemB.index);
}

//=================================================================================================
// WBackendTorrentPrivate
//=================================================================================================

class SK_TORRENT_EXPORT WBackendTorrentPrivate : public WBackendNetPrivate
{
public:
    WBackendTorrentPrivate(WBackendTorrent * p);

    void init();

public: // Functions
    QList<WBackendTorrentItem> extractItems(const QString & data) const;

    int extractItem(WBackendTorrentItem * item, const QString & data, int at) const;

    int extractString(QString * string, const QString & data, int at) const;

    bool applyTorrent(WBackendNetFolder * reply, const QUrl & url, const QString & string) const;
    bool applyMagnet (WBackendNetFolder * reply, const QUrl & url, const QString & string) const;

    void applyQuery(WBackendNetItem * reply, QStringList * urls, int id) const;

    void applyQueryTorrent(WBackendNetItem * reply, QStringList * urls,
                                                    QStringList * sources) const;

    int getIndex(const QString & name) const;

    QUrl getUrl(const QString & q) const;

    QList<WBackendTorrentItem> getFolder(QList<WBackendTorrentItem> * items) const;

protected:
    W_DECLARE_PUBLIC(WBackendTorrent)
};

//-------------------------------------------------------------------------------------------------

WBackendTorrentPrivate::WBackendTorrentPrivate(WBackendTorrent * p) : WBackendNetPrivate(p) {}

void WBackendTorrentPrivate::init() {}

//-------------------------------------------------------------------------------------------------
// Private functions

QList<WBackendTorrentItem> WBackendTorrentPrivate::extractItems(const QString & data) const
{
    QList<WBackendTorrentItem> items;

    QString list = WControllerTorrent::listAfter(data, "files");

    int index = WControllerTorrent::indexAfter(list, "path");

    int id = 1;

    while (index != -1)
    {
        WBackendTorrentItem item;

        item.id = id;

        QChar character = list.at(index);

        if (character == 'l')
        {
            index++;

            index = extractItem(&item, list, index);

            if (index == -1) return items;

            if (item.name.isEmpty() == false)
            {
                items.append(item);
            }
        }
        else
        {
            QString name;

            index = extractString(&name, list, index);

            if (index == -1) return items;

            if (name.isEmpty() == false)
            {
                item.name  = name;
                item.index = getIndex(name);

                items.append(item);
            }
        }

        index = WControllerTorrent::indexAfter(list, "path", index);

        id++;
    }

    return items;
}

//-------------------------------------------------------------------------------------------------

int WBackendTorrentPrivate::extractItem(WBackendTorrentItem * item, const QString & data,
                                                                    int             at) const
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

int WBackendTorrentPrivate::extractString(QString * string, const QString & data, int at) const
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

bool WBackendTorrentPrivate::applyTorrent(WBackendNetFolder * reply, const QUrl    & url,
                                                                     const QString & string) const
{
    if (WControllerNetwork::extractUrlExtension(string) == "torrent")
    {
        WLibraryFolderItem playlist(WLibraryItem::Playlist, WLocalObject::Default);

        playlist.source = url;

        playlist.title = QObject::tr("Torrent") + " - " + string.simplified();

        reply->items.append(playlist);

        return true;
    }
    else return false;
}

bool WBackendTorrentPrivate::applyMagnet(WBackendNetFolder * reply, const QUrl    & url,
                                                                    const QString & string) const
{
    if (string.startsWith("magnet:?"))
    {
        WLibraryFolderItem playlist(WLibraryItem::Playlist, WLocalObject::Default);

        playlist.source = url;

        playlist.title = QObject::tr("Magnet") + " - " + string.simplified();

        reply->items.append(playlist);

        return true;
    }
    else return false;
}

//-------------------------------------------------------------------------------------------------

void WBackendTorrentPrivate::applyQuery(WBackendNetItem * reply,
                                        QStringList     * urls, int id) const
{
    if (urls->isEmpty()) return;

    Q_Q(const WBackendTorrent);

    WBackendNetQuery * nextQuery = &(reply->nextQuery);

    nextQuery->backend = q->getId();

    nextQuery->url = urls->takeFirst();

    nextQuery->id = id;

    nextQuery->data = *urls;

    nextQuery->skipError = true;

    nextQuery->timeout = BACKENDTORRENT_TIMEOUT_A;
}

void WBackendTorrentPrivate::applyQueryTorrent(WBackendNetItem * reply,
                                               QStringList     * urls, QStringList * sources) const
{
    WBackendNetQuery * nextQuery = &(reply->nextQuery);

    QString source = sources->takeFirst();

    if (source.startsWith("magnet:?"))
    {
        nextQuery->type = WBackendNetQuery::TypeTorrent;

        nextQuery->timeout = BACKENDTORRENT_TIMEOUT_B;
    }
    else nextQuery->timeout = BACKENDTORRENT_TIMEOUT_A;

    nextQuery->url = source;

    nextQuery->id = 5;

    QVariantList list;

    list.append(*sources);
    list.append(*urls);

    nextQuery->data = list;

    nextQuery->skipError = true;
}

//-------------------------------------------------------------------------------------------------

int WBackendTorrentPrivate::getIndex(const QString & name) const
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

QUrl WBackendTorrentPrivate::getUrl(const QString & q) const
{
    QUrl url("https://duckduckgo.com/");

    QString search = q.simplified();

    if (search.startsWith("torrent ", Qt::CaseInsensitive) == false)
    {
        search.prepend("torrent ");
    }

#ifdef QT_4
    url.addQueryItem("q", search);

    url.addQueryItem("kl", "us-en");

    url.addQueryItem("kp", "-2");
#else
    QUrlQuery query(url);

    query.addQueryItem("q", search);

    query.addQueryItem("kl", "us-en");

    query.addQueryItem("kp", "-2");

    url.setQuery(query);
#endif

    return url;
}

//-------------------------------------------------------------------------------------------------

QList<WBackendTorrentItem>
WBackendTorrentPrivate::getFolder(QList<WBackendTorrentItem> * items) const
{
    QList<WBackendTorrentItem> list;

    WBackendTorrentItem item = items->takeAt(0);

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

    return list;
}

//=================================================================================================
// WBackendTorrent
//=================================================================================================

WBackendTorrent::WBackendTorrent() : WBackendNet(new WBackendTorrentPrivate(this))
{
    Q_D(WBackendTorrent); d->init();
}

//-------------------------------------------------------------------------------------------------
// WBackendNet implementation
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QString WBackendTorrent::getId() const
{
    return "bittorrent";
}

/* Q_INVOKABLE virtual */ QString WBackendTorrent::getTitle() const
{
    return "BitTorrent";
}

//-------------------------------------------------------------------------------------------------
// WBackendNet reimplementation
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ bool WBackendTorrent::checkValidUrl(const QUrl & url) const
{
    QString source = url.toString();

    if (WControllerNetwork::extractUrlExtension(source) == "torrent"
        ||
        source.startsWith("magnet:?"))
    {
         return true;
    }
    else return false;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QList<WLibraryFolderItem> WBackendTorrent::getLibraryItems() const
{
    WLibraryFolderItem tracks(WLibraryItem::PlaylistSearch);

    tracks.title = tr("Tracks");
    tracks.label = "tracks";

    WLibraryFolderItem all(WLibraryItem::FolderSearch);

    all.title = tr("Torrents & Magnets");
    all.label = "all";

    WLibraryFolderItem torrents(WLibraryItem::FolderSearch);

    torrents.title = tr("Torrents");
    torrents.label = "torrents";

    WLibraryFolderItem magnets(WLibraryItem::FolderSearch);

    magnets.title = tr("Magnets");
    magnets.label = "magnets";

    QList<WLibraryFolderItem> items;

    items.append(tracks);
    items.append(all);
    items.append(torrents);
    items.append(magnets);

    return items;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
WAbstractBackend::Output WBackendTorrent::getTrackOutput(const QUrl & url) const
{
    QString fragment = url.fragment();

    int index = fragment.indexOf('.');

    if (index != -1)
    {
        fragment = fragment.mid(index + 1);

        if (WControllerPlaylist::extensionIsAudio(fragment))
        {
            return WAbstractBackend::OutputAudio;
        }
    }

    return WAbstractBackend::OutputMedia;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
WBackendNetPlaylistInfo WBackendTorrent::getPlaylistInfo(const QUrl & url) const
{
    if (checkValidUrl(url))
    {
         return WBackendNetPlaylistInfo(WLibraryItem::Playlist, url.toString());
    }
    else return WBackendNetPlaylistInfo();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
QUrl WBackendTorrent::getUrlPlaylist(const WBackendNetPlaylistInfo & info) const
{
    return WControllerNetwork::encodedUrl(info.id);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
WBackendNetQuery WBackendTorrent::getQueryPlaylist(const QUrl & url) const
{
    WBackendNetQuery query;

    QString source = url.toString();

    if (source.startsWith("magnet:?"))
    {
        query.type = WBackendNetQuery::TypeTorrent;
    }
    else if (WControllerNetwork::extractUrlExtension(source) != "torrent")
    {
        return query;
    }

    int index = source.indexOf('#');

    if (index == -1)
    {
        query.url = url;

        query.data = -1;
    }
    else
    {
        QString number;

        for (int i = index + 1; i < source.length(); i++)
        {
            QChar character = source.at(i);

            if (character.isDigit() == false) break;

            number.append(character);
        }

        query.url = WControllerNetwork::encodedUrl(source.mid(0, index));

        query.data = number.toInt();
    }

    return query;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
WBackendNetQuery WBackendTorrent::createQuery(const QString & method,
                                              const QString & label, const QString & q) const
{
    WBackendNetQuery query;

    if (method == "search")
    {
        if (label == "tracks")
        {
            Q_D(const WBackendTorrent);

            query.backend = getId();

            query.url = d->getUrl(q);
            query.id  = 2;
        }
        else if (label == "all")
        {
            Q_D(const WBackendTorrent);

            query.backend = getId();

            query.url = d->getUrl(q);
        }
        else if (label == "torrents")
        {
            Q_D(const WBackendTorrent);

            query.backend = getId();

            query.url = d->getUrl(q);

            query.data = 1;
        }
        else if (label == "magnets")
        {
            Q_D(const WBackendTorrent);

            query.backend = getId();

            query.url = d->getUrl(q);

            query.data = 2;
        }
    }
    else if (method == "related" && label == "tracks")
    {
        if (q.startsWith("magnet:?"))
        {
            query.type = WBackendNetQuery::TypeTorrent;
        }

        int index = q.indexOf('#');

        query.url = WControllerNetwork::encodedUrl(q.mid(0, index));
        query.id  = 1;
    }

    return query;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
WBackendNetPlaylist WBackendTorrent::extractPlaylist(const QByteArray       & data,
                                                     const WBackendNetQuery & query) const
{
    WBackendNetPlaylist reply;

    int id = query.id;

    if (id == 2) // Search urls
    {
        QString content = Sk::readUtf8(data);

        QString source = Sk::sliceIn(content, ";nrj('", "'");

        WBackendNetQuery * nextQuery = &(reply.nextQuery);

        nextQuery->backend = getId();

        nextQuery->url = WControllerNetwork::encodedUrl("https://duckduckgo.com" + source);

        nextQuery->id = 3;

        nextQuery->data = query.data;
    }
    else if (id == 3) // Extract urls
    {
        Q_D(const WBackendTorrent);

        QStringList urls;

        QString string = Sk::sliceIn(data, "('d',[", "]);");

        QStringList list = WControllerNetwork::splitJson(string);

        foreach (const QString & string, list)
        {
            QString source = WControllerNetwork::extractJsonUtf8(string, "c");

            if (source.isEmpty() || urls.contains(source)) continue;

            urls.append(source);

            if (urls.count() == BACKENDTORRENT_MAX_URLS) break;
        }

        d->applyQuery(&reply, &urls, 4);
    }
    else if (id == 4) // Extract sources
    {
        Q_D(const WBackendTorrent);

        WControllerPlaylistData playlistData;

        playlistData.applyHtml(data, query.url.toString());

        QStringList sources;

        int index = 0;

        foreach (const WControllerPlaylistSource & source, playlistData.sources)
        {
            QString string = source.url.toString();

            if (WControllerNetwork::extractUrlExtension(string) == "torrent")
            {
                sources.append(string);

                index++;

                if (index == BACKENDTORRENT_MAX_SOURCES) break;
            }
        }

        if (index != BACKENDTORRENT_MAX_SOURCES)
        {
            foreach (const WControllerPlaylistSource & source, playlistData.sources)
            {
                QString string = source.url.toString();

                if (string.startsWith("magnet:?"))
                {
                    sources.append(string);

                    index++;

                    if (index == BACKENDTORRENT_MAX_SOURCES) break;
                }
            }
        }

        QStringList urls = query.data.toStringList();

        if (sources.isEmpty())
        {
             d->applyQuery(&reply, &urls, 4);
        }
        else d->applyQueryTorrent(&reply, &urls, &sources);
    }
    else if (id == 5) // Extract torrent
    {
        Q_D(const WBackendTorrent);

        if (data.isEmpty() == false)
        {
            QString content = Sk::readAscii(data);

            content = WControllerTorrent::listAfter(content, "info");

            QString name = WControllerTorrent::stringAfter(content, "name");

            name = Sk::latinToUtf8(name);

            QList<WBackendTorrentItem> items = d->extractItems(content);

            QList<WTrack> * tracks = &(reply.tracks);

            QString url = query.url.toString();

            while (items.isEmpty() == false)
            {
                QList<WBackendTorrentItem> list = d->getFolder(&items);

                if (list.first().index == -1)
                {
                     qSort(list.begin(), list.end(), sortA);
                }
                else qSort(list.begin(), list.end(), sortB);

                foreach (const WBackendTorrentItem & item, list)
                {
                    QString title = Sk::latinToUtf8(item.name);

                    QString extension = WControllerNetwork::extractUrlExtension(title);

                    if (WControllerPlaylist::extensionIsMedia(extension))
                    {
                        QString source = url + '#' + QString::number(item.id) + '.' + extension;

                        WTrack track(WControllerNetwork::encodedUrl(source), WTrack::Default);

                        track.setTitle(title);

                        track.setAuthor(name);
                        track.setFeed  (url);

                        tracks->append(track);
                    }
                }
            }

            reply.cache = data;

            reply.clearDuplicate = true;
        }

        QVariantList list = query.data.toList();

        QStringList sources = list.first().toStringList();
        QStringList urls    = list.last ().toStringList();

        if (sources.isEmpty())
        {
             d->applyQuery(&reply, &urls, 4);
        }
        else d->applyQueryTorrent(&reply, &urls, &sources);
    }
    else
    {
        Q_D(const WBackendTorrent);

        QString content = Sk::readAscii(data);

        content = WControllerTorrent::listAfter(content, "info");

        QString name = WControllerTorrent::stringAfter(content, "name");

        name = Sk::latinToUtf8(name);

        QList<WBackendTorrentItem> items = d->extractItems(content);

        if (items.isEmpty())
        {
            WBackendTorrentItem item;

            item.id    = 1;
            item.name  = name;
            item.index = -1;

            items.append(item);
        }

        QList<WTrack> * tracks = &(reply.tracks);

        QList<int> ids;

        QString url = query.url.toString();

        while (items.isEmpty() == false)
        {
            QList<WBackendTorrentItem> list = d->getFolder(&items);

            if (list.first().index == -1)
            {
                 qSort(list.begin(), list.end(), sortA);
            }
            else qSort(list.begin(), list.end(), sortB);

            foreach (const WBackendTorrentItem & item, list)
            {
                QString title = Sk::latinToUtf8(item.name);

                QString extension = WControllerNetwork::extractUrlExtension(title);

                if (WControllerPlaylist::extensionIsMedia(extension))
                {
                    int id = item.id;

                    QString source = url + '#' + QString::number(id) + '.' + extension;

                    WTrack track(WControllerNetwork::encodedUrl(source), WTrack::Default);

                    track.setTitle(title);

                    track.setAuthor(name);
                    track.setFeed  (url);

                    tracks->append(track);

                    ids.append(id);
                }
            }
        }

        reply.title = name;

        reply.cache = data;

        if (id == 0)
        {
            int index = query.data.toInt();

            if (index == -1) return reply;

            for (int i = 0; i < ids.count(); i++)
            {
                if (ids.at(i) == index)
                {
                    reply.currentIndex = i;

                    break;
                }
            }
        }
        else reply.clearDuplicate = true;
    }

    return reply;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
WBackendNetFolder WBackendTorrent::extractFolder(const QByteArray       & data,
                                                 const WBackendNetQuery & query) const
{
    WBackendNetFolder reply;

    int id = query.id;

    if (id == 0) // Search urls
    {
        QString content = Sk::readUtf8(data);

        QString source = Sk::sliceIn(content, ";nrj('", "'");

        WBackendNetQuery * nextQuery = &(reply.nextQuery);

        nextQuery->backend = getId();

        nextQuery->url = WControllerNetwork::encodedUrl("https://duckduckgo.com" + source);

        nextQuery->id = 1;

        nextQuery->data = query.data;
    }
    else if (id == 1) // Extract urls
    {
        Q_D(const WBackendTorrent);

        QStringList urls;

        QString string = Sk::sliceIn(data, "('d',[", "]);");

        QStringList list = WControllerNetwork::splitJson(string);

        foreach (const QString & string, list)
        {
            QString source = WControllerNetwork::extractJsonUtf8(string, "c");

            if (source.isEmpty() || urls.contains(source)) continue;

            urls.append(source);

            if (urls.count() == BACKENDTORRENT_MAX_URLS) break;
        }

        d->applyQuery(&reply, &urls, query.data.toInt() + 2);
    }
    else // Extract sources
    {
        Q_D(const WBackendTorrent);

        QStringList urls = query.data.toStringList();

        WControllerPlaylistData playlistData;

        playlistData.applyHtml(data, query.url.toString());

        int index = 0;

        if (id == 2)
        {
            foreach (const WControllerPlaylistSource & source, playlistData.sources)
            {
                QUrl url = source.url;

                QString string = url.toString();

                if (d->applyTorrent(&reply, url, string) || d->applyMagnet(&reply, url, string))
                {
                    index++;

                    if (index == BACKENDTORRENT_MAX_SOURCES) break;
                }
            }

            d->applyQuery(&reply, &urls, 2);
        }
        else if (id == 3)
        {
            foreach (const WControllerPlaylistSource & source, playlistData.sources)
            {
                QUrl url = source.url;

                QString string = url.toString();

                if (d->applyTorrent(&reply, url, string))
                {
                    index++;

                    if (index == BACKENDTORRENT_MAX_SOURCES) break;
                }
            }

            d->applyQuery(&reply, &urls, 3);
        }
        else // if (id == 4)
        {
            foreach (const WControllerPlaylistSource & source, playlistData.sources)
            {
                QUrl url = source.url;

                QString string = url.toString();

                if (d->applyMagnet(&reply, url, string))
                {
                    index++;

                    if (index == BACKENDTORRENT_MAX_SOURCES) break;
                }
            }

            d->applyQuery(&reply, &urls, 4);
        }

        reply.clearDuplicate = true;
    }

    return reply;
}

#endif // SK_NO_BACKENDTORRENT
