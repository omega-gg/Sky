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
// WBackendTorrentPrivate
//=================================================================================================

class SK_TORRENT_EXPORT WBackendTorrentPrivate : public WBackendNetPrivate
{
public:
    WBackendTorrentPrivate(WBackendTorrent * p);

    void init();

public: // Functions
    QStringList extractUrls(const QByteArray & data) const;

    int extractString(QString * string, const QString & data, int at) const;

    void applyHtml(WControllerPlaylistData * playlistData, const QByteArray & data,
                                                           const QString    & url) const;

    bool applyTorrent(WBackendNetFolder * reply, const QString & url) const;
    bool applyMagnet (WBackendNetFolder * reply, const QString & url) const;

    void applyQuerySearch(const QByteArray       & data,
                          const WBackendNetQuery & query,
                          WBackendNetBase        * reply, int id) const;

    void applyQueryUrl(WBackendNetBase * reply, QStringList * urls, int id) const;

    void applyQueryTorrent(WBackendNetBase * reply, QStringList * urls,
                                                    QStringList * sources) const;

    int getIndex(const QString & name) const;

    QString getUrl(const QString & q) const;

protected:
    W_DECLARE_PUBLIC(WBackendTorrent)
};

//-------------------------------------------------------------------------------------------------

WBackendTorrentPrivate::WBackendTorrentPrivate(WBackendTorrent * p) : WBackendNetPrivate(p) {}

void WBackendTorrentPrivate::init() {}

//-------------------------------------------------------------------------------------------------
// Private functions

QStringList WBackendTorrentPrivate::extractUrls(const QByteArray & data) const
{
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

    return urls;
}

//-------------------------------------------------------------------------------------------------

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

void WBackendTorrentPrivate::applyHtml(WControllerPlaylistData * playlistData,
                                       const QByteArray        & data,
                                       const QString           & url) const
{
    playlistData->addSlice("http");
    playlistData->addSlice("", ".torrent");
    playlistData->addSlice("magnet:?");

    playlistData->applyHtml(data, url);
}

//-------------------------------------------------------------------------------------------------

bool WBackendTorrentPrivate::applyTorrent(WBackendNetFolder * reply, const QString & url) const
{
    if (WControllerNetwork::extractUrlExtension(url) == "torrent")
    {
        WLibraryFolderItem playlist(WLibraryItem::Playlist, WLocalObject::Default);

        playlist.source = url;

        playlist.title = QObject::tr("Torrent") + " - " + url.simplified();

        reply->items.append(playlist);

        return true;
    }
    else return false;
}

bool WBackendTorrentPrivate::applyMagnet(WBackendNetFolder * reply, const QString & url) const
{
    if (url.startsWith("magnet:?"))
    {
        WLibraryFolderItem playlist(WLibraryItem::Playlist, WLocalObject::Default);

        playlist.source = url;

        playlist.title = QObject::tr("Magnet") + " - " + url.simplified();

        reply->items.append(playlist);

        return true;
    }
    else return false;
}

//-------------------------------------------------------------------------------------------------

void WBackendTorrentPrivate::applyQuerySearch(const QByteArray       & data,
                                              const WBackendNetQuery & query,
                                              WBackendNetBase        * reply, int id) const
{
    Q_Q(const WBackendTorrent);

    QString content = Sk::readUtf8(data);

    QString source = Sk::sliceIn(content, ";nrj('", "'");

    WBackendNetQuery * nextQuery = &(reply->nextQuery);

    nextQuery->backend = q->getId();

    nextQuery->url = "https://duckduckgo.com" + source;

    nextQuery->id = id;

    nextQuery->data = query.data;
}

void WBackendTorrentPrivate::applyQueryUrl(WBackendNetBase * reply,
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

void WBackendTorrentPrivate::applyQueryTorrent(WBackendNetBase * reply,
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

QString WBackendTorrentPrivate::getUrl(const QString & q) const
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

    return url.toString();
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

/* Q_INVOKABLE virtual */ QString WBackendTorrent::validate() const
{
    return ".torrent$|^magnet:?";
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
WAbstractBackend::Output WBackendTorrent::getTrackOutput(const QString & url) const
{
    QString fragment = QUrl(url).fragment();

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
WBackendNetPlaylistInfo WBackendTorrent::getPlaylistInfo(const QString & url) const
{
    if (WControllerNetwork::extractUrlExtension(url) == "torrent" || url.startsWith("magnet:?"))
    {
         return WBackendNetPlaylistInfo(WLibraryItem::Playlist, url);
    }
    else return WBackendNetPlaylistInfo();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
QString WBackendTorrent::getUrlPlaylist(const WBackendNetPlaylistInfo & info) const
{
    return info.id;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
WBackendNetQuery WBackendTorrent::getQueryTrack(const QString & url) const
{
    WBackendNetQuery query;

    if (url.startsWith("magnet:?"))
    {
        query.type = WBackendNetQuery::TypeTorrent;
    }
    else if (WControllerNetwork::extractUrlExtension(url) != "torrent")
    {
        return query;
    }

    int index = url.indexOf('#');

    if (index == -1)
    {
        query.url = url;

        query.data = -1;
    }
    else
    {
        QString number;

        for (int i = index + 1; i < url.length(); i++)
        {
            QChar character = url.at(i);

            if (character.isDigit() == false) break;

            number.append(character);
        }

        query.url = url.mid(0, index);

        query.data = number.toInt();
    }

    return query;
}

/* Q_INVOKABLE virtual */
WBackendNetQuery WBackendTorrent::getQueryPlaylist(const QString & url) const
{
    return getQueryTrack(url);
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

        query.url = q.mid(0, index);
        query.id  = 1;
    }

    return query;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
WBackendNetTrack WBackendTorrent::extractTrack(const QByteArray       & data,
                                               const WBackendNetQuery & query) const
{
    WBackendNetTrack reply;

    QString content = Sk::readAscii(data);

    content = WControllerTorrent::listAfter(content, "info");

    QString name = WControllerTorrent::stringAfter(content, "name");

    QList<WTorrentItemData> items = WControllerTorrent::torrentItems(content);

    QString title;

    if (items.isEmpty())
    {
        title = Sk::latinToUtf8(name);

        if (WControllerPlaylist::urlIsMedia(title) == false)
        {
            return reply;
        }

        name = title;
    }
    else
    {
        int index = query.data.toInt() - 1;

        if (index > 0)
        {
            int count = items.count() - 1;

            if (index > count)
            {
                index = count;
            }
        }
        else index = 0;

        const WTorrentItemData & item = items.at(index);

        title = Sk::latinToUtf8(item.name);

        if (WControllerPlaylist::urlIsMedia(title) == false)
        {
            return reply;
        }

        name = Sk::latinToUtf8(name);
    }

    WTrack * track = &(reply.track);

    track->setState(WTrack::Cover);

    track->setTitle(title);

    track->setAuthor(name);
    track->setFeed  (query.url);

    return reply;
}

/* Q_INVOKABLE virtual */
WBackendNetPlaylist WBackendTorrent::extractPlaylist(const QByteArray       & data,
                                                     const WBackendNetQuery & query) const
{
    WBackendNetPlaylist reply;

    int id = query.id;

    if (id == 2) // Search urls
    {
        Q_D(const WBackendTorrent);

        d->applyQuerySearch(data, query, &reply, 3);
    }
    else if (id == 3) // Extract urls
    {
        Q_D(const WBackendTorrent);

        QStringList urls = d->extractUrls(data);

        d->applyQueryUrl(&reply, &urls, 4);
    }
    else if (id == 4) // Extract sources
    {
        Q_D(const WBackendTorrent);

        WControllerPlaylistData playlistData;

        d->applyHtml(&playlistData, data, query.url);

        QStringList sources;

        int index = 0;

        foreach (const WControllerPlaylistSource & source, playlistData.sources)
        {
            QString string = source.url;

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
                QString string = source.url;

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
             d->applyQueryUrl(&reply, &urls, 4);
        }
        else d->applyQueryTorrent(&reply, &urls, &sources);
    }
    else if (id == 5) // Extract torrent
    {
        Q_D(const WBackendTorrent);

        if (data.isEmpty() == false)
        {
            reply.cache = data;

            QString content = Sk::readAscii(data);

            content = WControllerTorrent::listAfter(content, "info");

            QString name = WControllerTorrent::stringAfter(content, "name");

            QList<WTorrentItemData> items = WControllerTorrent::torrentItems(content);

            if (items.isEmpty())
            {
                WTorrentItemData item;

                item.id    = 1;
                item.name  = name;
                item.index = -1;

                items.append(item);
            }

            name = Sk::latinToUtf8(name);

            QList<WTrack> * tracks = &(reply.tracks);

            QString url = query.url;

            QList<WTorrentItemFolder> folders = WControllerTorrent::torrentFolders(items);

            foreach (const WTorrentItemFolder & folder, folders)
            {
                foreach (const WTorrentItemData & item, folder.items)
                {
                    QString title = Sk::latinToUtf8(item.name);

                    QString extension = WControllerNetwork::extractUrlExtension(title);

                    if (WControllerPlaylist::extensionIsMedia(extension))
                    {
                        QString source = url + '#' + QString::number(item.id) + '.' + extension;

                        WTrack track(source, WTrack::Cover);

                        track.setTitle(title);

                        track.setAuthor(name);
                        track.setFeed  (url);

                        tracks->append(track);
                    }
                }
            }

            reply.clearDuplicate = true;
        }

        QVariantList list = query.data.toList();

        QStringList sources = list.first().toStringList();
        QStringList urls    = list.last ().toStringList();

        if (sources.isEmpty())
        {
             d->applyQueryUrl(&reply, &urls, 4);
        }
        else d->applyQueryTorrent(&reply, &urls, &sources);
    }
    else
    {
        QString content = Sk::readAscii(data);

        content = WControllerTorrent::listAfter(content, "info");

        QString name = WControllerTorrent::stringAfter(content, "name");

        QList<WTorrentItemData> items = WControllerTorrent::torrentItems(content);

        if (items.isEmpty())
        {
            WTorrentItemData item;

            item.id    = 1;
            item.name  = name;
            item.index = -1;

            items.append(item);
        }

        name = Sk::latinToUtf8(name);

        QList<WTrack> * tracks = &(reply.tracks);

        QList<int> ids;

        QString url = query.url;

        QList<WTorrentItemFolder> folders = WControllerTorrent::torrentFolders(items);

        foreach (const WTorrentItemFolder & folder, folders)
        {
            foreach (const WTorrentItemData & item, folder.items)
            {
                QString title = Sk::latinToUtf8(item.name);

                QString extension = WControllerNetwork::extractUrlExtension(title);

                if (WControllerPlaylist::extensionIsMedia(extension))
                {
                    int id = item.id;

                    QString source = url + '#' + QString::number(id) + '.' + extension;

                    WTrack track(source, WTrack::Cover);

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
        Q_D(const WBackendTorrent);

        d->applyQuerySearch(data, query, &reply, 1);
    }
    else if (id == 1) // Extract urls
    {
        Q_D(const WBackendTorrent);

        QStringList urls = d->extractUrls(data);

        d->applyQueryUrl(&reply, &urls, query.data.toInt() + 2);
    }
    else // Extract sources
    {
        Q_D(const WBackendTorrent);

        QStringList urls = query.data.toStringList();

        WControllerPlaylistData playlistData;

        d->applyHtml(&playlistData, data, query.url);

        int index = 0;

        if (id == 2)
        {
            foreach (const WControllerPlaylistSource & source, playlistData.sources)
            {
                QString url = source.url;

                if (d->applyTorrent(&reply, url) || d->applyMagnet(&reply, url))
                {
                    index++;

                    if (index == BACKENDTORRENT_MAX_SOURCES) break;
                }
            }
        }
        else if (id == 3)
        {
            foreach (const WControllerPlaylistSource & source, playlistData.sources)
            {
                QString url = source.url;

                if (d->applyTorrent(&reply, url))
                {
                    index++;

                    if (index == BACKENDTORRENT_MAX_SOURCES) break;
                }
            }
        }
        else // if (id == 4)
        {
            foreach (const WControllerPlaylistSource & source, playlistData.sources)
            {
                QString url = source.url;

                if (d->applyMagnet(&reply, url))
                {
                    index++;

                    if (index == BACKENDTORRENT_MAX_SOURCES) break;
                }
            }
        }

        d->applyQueryUrl(&reply, &urls, id);

        reply.clearDuplicate = true;
    }

    return reply;
}

#endif // SK_NO_BACKENDTORRENT
