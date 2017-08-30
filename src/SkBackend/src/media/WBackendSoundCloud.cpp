//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkBackend module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#include "WBackendSoundCloud.h"

#ifndef SK_NO_BACKENDSOUNDCLOUD

// Qt includes
#ifdef QT_LATEST
#include <QUrlQuery>
#endif

// Sk includes
#include <WControllerApplication>
#include <WControllerNetwork>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include <private/WBackendNet_p>

class SK_BACKEND_EXPORT WBackendSoundCloudPrivate : public WBackendNetPrivate
{
public:
    WBackendSoundCloudPrivate(WBackendSoundCloud * p);

    void init();

public: // Functions
    void loadTrack(WTrackNet * track, const QString & json) const;

    bool extractId(const QString & data, const WBackendNetQuery & query,
                                         WBackendNetQuery       * nextQuery) const;

    QString extractJson(const QString & data, const QString & id) const;

    void applyQuery(WBackendNetQuery * backendQuery, const QUrl & url, int queryId) const;

    void applySearch (WBackendNetQuery * backendQuery, const QString & source, int queryId) const;
    void applyRelated(WBackendNetQuery * backendQuery, const QString & source) const;

    void applyCover(QString * cover) const;

    void setId(const WBackendNetQuery & query, const WBackendNetQuery & queryNext);

private:
    QString id;

protected:
    W_DECLARE_PUBLIC(WBackendSoundCloud)
};

//-------------------------------------------------------------------------------------------------

WBackendSoundCloudPrivate::WBackendSoundCloudPrivate(WBackendSoundCloud * p)
    : WBackendNetPrivate(p) {}

void WBackendSoundCloudPrivate::init() {}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WBackendSoundCloudPrivate::loadTrack(WTrackNet * track, const QString & json) const
{
    QString title = WControllerNetwork::extractJsonUtf8(json, "title");
    QString cover = WControllerNetwork::extractJson    (json, "artwork_url");

    if (cover == "null")
    {
        cover = WControllerNetwork::extractJson(json, "avatar_url");
    }

    applyCover(&cover);

    QString user = WControllerNetwork::extractJson(json, "user");

    QString author = WControllerNetwork::extractJsonUtf8(user, "username");
    QString feed   = WControllerNetwork::extractJson    (user, "permalink");

    QString duration = WControllerNetwork::extractJson(json, "duration");

    QString date = WControllerNetwork::extractJson(json, "created_at");

    date = date.mid(0, 19);

    track->setTitle(title);
    track->setCover(cover);

    track->setAuthor(author);
    track->setFeed  (feed);

    track->setDuration(duration.toInt());

    if (date.contains(' '))
    {
         track->setDate(QDateTime::fromString(date, "yyyy/MM/dd hh:mm:ss"));
    }
    else track->setDate(QDateTime::fromString(date, "yyyy-MM-ddThh:mm:ss"));
}

//-------------------------------------------------------------------------------------------------

bool WBackendSoundCloudPrivate::extractId(const QString          & data,
                                          const WBackendNetQuery & query,
                                          WBackendNetQuery       * nextQuery) const
{
    if (query.id == -1)
    {
        int index = data.lastIndexOf("<script crossorigin src=");

        QString source = WControllerNetwork::extractAttributeAt(data, index + 25);

        nextQuery->url  = source;
        nextQuery->id   = -2;
        nextQuery->data = query.data;

        return true;
    }
    else if (query.id == -2)
    {
        int index = data.indexOf("client_id:");

        QString id = WControllerNetwork::extractAttributeAt(data, index + 11);

        QVariantList variants = query.data.toList();

        QUrl url = variants.at(0).toUrl();

        if (url.toString().startsWith("https://api"))
        {
#ifdef QT_4
            url.addQueryItem("client_id", id);
#else
            QUrlQuery query(url);

            query.addQueryItem("client_id", id);

            url.setQuery(query);
#endif
        }

        nextQuery->url  = url;
        nextQuery->id   = variants.at(1).toInt();
        nextQuery->data = id;

        return true;
    }
    else return false;
}

//-------------------------------------------------------------------------------------------------

QString WBackendSoundCloudPrivate::extractJson(const QString & data, const QString & id) const
{
    int index = data.indexOf("<script>webpackJsonp");

    index = data.indexOf("\"id\":" + id, index + 20);

    return WControllerNetwork::extractJson(data, "data", index + 7);
}

//-------------------------------------------------------------------------------------------------

void WBackendSoundCloudPrivate::applyQuery(WBackendNetQuery * backendQuery,
                                           const QUrl       & url, int queryId) const
{
    if (id.isEmpty())
    {
        QVariantList variants;

        variants.append(url);
        variants.append(queryId);

        backendQuery->url  = "https://soundcloud.com";
        backendQuery->id   = -1;
        backendQuery->data = variants;
    }
    else
    {
        backendQuery->url  = url;
        backendQuery->id   = queryId;
        backendQuery->data = id;
    }
}

void WBackendSoundCloudPrivate::applySearch(WBackendNetQuery * backendQuery,
                                            const QString    & source,  int queryId) const
{
    if (id.isEmpty())
    {
        QVariantList variants;

        variants.append(source);
        variants.append(queryId);

        backendQuery->url  = "https://soundcloud.com";
        backendQuery->id   = -1;
        backendQuery->data = variants;
    }
    else
    {
        QUrl url = source;

#ifdef QT_4
        url.addQueryItem("client_id", id);
#else
        QUrlQuery query(url);

        query.addQueryItem("client_id", id);

        url.setQuery(query);
#endif

        backendQuery->url  = url;
        backendQuery->id   = queryId;
        backendQuery->data = id;
    }
}

void WBackendSoundCloudPrivate::applyRelated(WBackendNetQuery * backendQuery,
                                             const QString    & source) const
{
    if (id.isEmpty())
    {
        QVariantList variants;

        variants.append(source);
        variants.append(3);

        backendQuery->url  = "https://soundcloud.com";
        backendQuery->id   = -1;
        backendQuery->data = variants;
    }
    else
    {
        backendQuery->url  = source;
        backendQuery->id   = 3;
        backendQuery->data = id;
    }
}

//-------------------------------------------------------------------------------------------------

void WBackendSoundCloudPrivate::applyCover(QString * cover) const
{
    cover->replace("-large.", "-t500x500.");
}

//-------------------------------------------------------------------------------------------------

void WBackendSoundCloudPrivate::setId(const WBackendNetQuery & query,
                                      const WBackendNetQuery & nextQuery)
{
    if (query.id != -2) return;

    id = nextQuery.data.toString();
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

WBackendSoundCloud::WBackendSoundCloud() : WBackendNet(new WBackendSoundCloudPrivate(this))
{
    Q_D(WBackendSoundCloud); d->init();
}

//-------------------------------------------------------------------------------------------------
// WBackendNet implementation
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QString WBackendSoundCloud::getId() const
{
    return "soundcloud";
}

/* Q_INVOKABLE virtual */ QString WBackendSoundCloud::getTitle() const
{
    return "SoundCloud";
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ bool WBackendSoundCloud::checkValidUrl(const QUrl & url) const
{
    QString source = WControllerNetwork::removeUrlPrefix(url);

    if (source.startsWith("soundcloud.com") || source.startsWith("api.soundcloud.com")
        ||
        source.startsWith("a-v2.sndcdn.com"))
    {
         return true;
    }
    else return false;
}

//-------------------------------------------------------------------------------------------------
// WBackendNet reimplementation
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QString WBackendSoundCloud::getHost() const
{
    return "soundcloud.com";
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QList<WLibraryFolderItem> WBackendSoundCloud::getLibraryItems() const
{
    WLibraryFolderItem tracks(WLibraryItem::PlaylistSearch);

    tracks.title = tr("Tracks");
    tracks.label = "tracks";

    WLibraryFolderItem people(WLibraryItem::FolderSearch);

    people.title = tr("People");
    people.label = "people";

    WLibraryFolderItem playlists(WLibraryItem::FolderSearch);

    playlists.title = tr("Playlists");
    playlists.label = "playlists";

    QList<WLibraryFolderItem> items;

    items.append(tracks);
    items.append(people);
    items.append(playlists);

    return items;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
QString WBackendSoundCloud::getTrackId(const QUrl & url) const
{
    QString source = WControllerNetwork::removeUrlPrefix(url);

    if (source.startsWith("soundcloud.com/") == false
        ||
        source.contains(QRegExp("/tracks/|/sets/|/albums/|/reposts/|/tags/|/popular/")))
    {
        return QString();
    }

    source = source.mid(15);

    if (source.count('/') == 1)
    {
         return source;
    }
    else return QString();
}

/* Q_INVOKABLE virtual */
WAbstractBackend::Output WBackendSoundCloud::getTrackOutput(const QUrl &) const
{
    return WAbstractBackend::OutputAudio;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
WBackendNetPlaylistInfo WBackendSoundCloud::getPlaylistInfo(const QUrl & url) const
{
    QString source = WControllerNetwork::removeUrlPrefix(url);

    if (source.startsWith("soundcloud.com/") == false)
    {
        return WBackendNetPlaylistInfo();
    }

    source = source.mid(15);

    if (source.contains(QRegExp("[.\\?#]")))
    {
        return WBackendNetPlaylistInfo();
    }
    else if (source.count('/'))
    {
        if (source.contains("/sets/"))
        {
             return WBackendNetPlaylistInfo(WLibraryItem::PlaylistNet, source);
        }
        else return WBackendNetPlaylistInfo();
    }
    else return WBackendNetPlaylistInfo(WLibraryItem::PlaylistFeed, source);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
QUrl WBackendSoundCloud::getUrlTrack(const QString & id) const
{
    return "https://soundcloud.com/" + id;
}

/* Q_INVOKABLE virtual */
QUrl WBackendSoundCloud::getUrlPlaylist(const WBackendNetPlaylistInfo & info) const
{
    return "https://soundcloud.com/" + info.id;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
WBackendNetQuery WBackendSoundCloud::getQuerySource(const QUrl & url) const
{
    Q_D(const WBackendSoundCloud);

    WBackendNetQuery backendQuery;

    d->applyQuery(&backendQuery, url, 0);

    return backendQuery;
}

/* Q_INVOKABLE virtual */
WBackendNetQuery WBackendSoundCloud::getQueryTrack(const QUrl & url) const
{
    WBackendNetQuery backendQuery;

    backendQuery.url = url;

    return backendQuery;
}

/* Q_INVOKABLE virtual */
WBackendNetQuery WBackendSoundCloud::getQueryPlaylist(const QUrl & url) const
{
    Q_D(const WBackendSoundCloud);

    WBackendNetQuery backendQuery;

    WBackendNetPlaylistInfo info = getPlaylistInfo(url);

    if (info.isValid() == false) return backendQuery;

    if (info.isFeed())
    {
         d->applyQuery(&backendQuery, url, 2);
    }
    else d->applyQuery(&backendQuery, url, 1);

    return backendQuery;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
WBackendNetQuery WBackendSoundCloud::createQuery(const QString & method,
                                                 const QString & label, const QString & q) const
{
    WBackendNetQuery backendQuery;

    if (method == "search")
    {
        if (label == "tracks")
        {
            Q_D(const WBackendSoundCloud);

            d->applySearch(&backendQuery,
                           "https://api.soundcloud.com/tracks?q=" + q + "&limit=50", 0);
        }
        else if (label == "people")
        {
            Q_D(const WBackendSoundCloud);

            d->applySearch(&backendQuery,
                           "https://api.soundcloud.com/users?q=" + q + "&limit=50", 0);
        }
        else if (label == "playlists")
        {
            Q_D(const WBackendSoundCloud);

            d->applySearch(&backendQuery,
                           "https://api.soundcloud.com/playlists?q=" + q + "&limit=50", 1);
        }
    }
    else if (method == "related" && label == "tracks")
    {
        Q_D(const WBackendSoundCloud);

        d->applyRelated(&backendQuery, "https://soundcloud.com/" + q);
    }

    return backendQuery;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
WBackendNetSource WBackendSoundCloud::extractSource(const QByteArray       & data,
                                                    const WBackendNetQuery & query) const
{
    Q_D(const WBackendSoundCloud);

    WBackendNetSource reply;

    QString content = Sk::readUtf8(data);

    if (d->extractId(content, query, &reply.nextQuery)) return reply;

    if (query.id == 0)
    {
        QString json = d->extractJson(content, "67");

        QString idTrack = WControllerNetwork::extractJsonUtf8(json, "id");

        WBackendNetQuery * nextQuery = &(reply.nextQuery);

        nextQuery->url = "https://api.soundcloud.com/tracks/" + idTrack
                         +
                         "/streams?client_id=" + query.data.toString();

        nextQuery->id = 1;
    }
    else
    {
        QString source = WControllerNetwork::extractJsonUtf8(content, "http_mp3_128_url");

        reply.medias.insert(WAbstractBackend::QualityMedium, source);
    }

    return reply;
}

/* Q_INVOKABLE virtual */
WBackendNetTrack WBackendSoundCloud::extractTrack(const QByteArray       & data,
                                                  const WBackendNetQuery &) const
{
    Q_D(const WBackendSoundCloud);

    WBackendNetTrack reply;

    QString content = Sk::readUtf8(data);

    QString json = d->extractJson(content, "67");

    d->loadTrack(&(reply.track), json);

    return reply;
}

/* Q_INVOKABLE virtual */
WBackendNetPlaylist WBackendSoundCloud::extractPlaylist(const QByteArray       & data,
                                                        const WBackendNetQuery & query) const
{
    Q_D(const WBackendSoundCloud);

    WBackendNetPlaylist reply;

    QString content = Sk::readUtf8(data);

    if (d->extractId(content, query, &reply.nextQuery)) return reply;

    if (query.id == 0)
    {
        if (content.startsWith('[') == false)
        {
            content = WControllerNetwork::extractJson(content, "tracks");
        }

        QStringList list = WControllerNetwork::splitJson(content);

        foreach (const QString & string, list)
        {
            QString source = WControllerNetwork::extractJson(string, "permalink");

            QString feed = WControllerNetwork::extractJson(string, "user");

            feed = WControllerNetwork::extractJson(feed, "permalink");

            WTrackNet track("https://soundcloud.com/" + feed + '/' + source);

            d->loadTrack(&track, string);

            reply.tracks.append(track);
        }
    }
    else if (query.id == 1) // playlist
    {
        QString json = d->extractJson(content, "84");

        QString idPlaylist = WControllerNetwork::extractJsonUtf8(json, "id");

        QString title = WControllerNetwork::extractJsonUtf8(json, "title");
        QString cover = WControllerNetwork::extractJson    (json, "artwork_url");

        if (cover == "null")
        {
            cover = WControllerNetwork::extractJson(json, "avatar_url");
        }

        d->applyCover(&cover);

        reply.title = title;
        reply.cover = cover;

        WBackendNetQuery * nextQuery = &(reply.nextQuery);

        nextQuery->url = "https://api.soundcloud.com/playlists/" + idPlaylist
                         +
                         "?client_id=" + query.data.toString();
    }
    else if (query.id == 2) // feed
    {
        QString json = d->extractJson(content, "65");

        QString source = WControllerNetwork::extractJsonUtf8(json, "uri");

        QString title = WControllerNetwork::extractJsonUtf8(json, "username");
        QString cover = WControllerNetwork::extractJson    (json, "avatar_url");

        d->applyCover(&cover);

        reply.title = title;
        reply.cover = cover;

        WBackendNetQuery * nextQuery = &(reply.nextQuery);

        nextQuery->url = source + "/tracks?client_id=" + query.data.toString();
    }
    else if (query.id == 3) // related
    {
        QString json = d->extractJson(content, "67");

        QString idTrack = WControllerNetwork::extractJsonUtf8(json, "id");

        WBackendNetQuery * nextQuery = &(reply.nextQuery);

        nextQuery->url = "https://api.soundcloud.com/tracks/" + idTrack
                         +
                         "/related?limit=50&client_id=" + query.data.toString();
    }

    return reply;
}

/* Q_INVOKABLE virtual */
WBackendNetFolder WBackendSoundCloud::extractFolder(const QByteArray       & data,
                                                    const WBackendNetQuery & query) const
{
    Q_D(const WBackendSoundCloud);

    WBackendNetFolder reply;

    QString content = Sk::readUtf8(data);

    if (d->extractId(content, query, &reply.nextQuery)) return reply;

    QStringList list = WControllerNetwork::splitJson(content);

    if (query.id == 0) // people
    {
        foreach (const QString & json, list)
        {
            QString id = WControllerNetwork::extractJson(json, "permalink");

            QString title = WControllerNetwork::extractJsonUtf8(json, "username");
            QString cover = WControllerNetwork::extractJson    (json, "avatar_url");

            d->applyCover(&cover);

            WLibraryFolderItem playlist(WLibraryItem::PlaylistFeed, WLibraryItem::Default);

            playlist.source = "https://soundcloud.com/" + id;

            playlist.title = title;
            playlist.cover = cover;

            reply.items.append(playlist);
        }
    }
    else // playlists
    {
        foreach (const QString & json, list)
        {
            QString source = WControllerNetwork::extractJson(json, "permalink_url");

            QString title = WControllerNetwork::extractJsonUtf8(json, "title");
            QString cover = WControllerNetwork::extractJson    (json, "artwork_url");

            if (cover == "null")
            {
                cover = WControllerNetwork::extractJson(json, "avatar_url");
            }

            d->applyCover(&cover);

            WLibraryFolderItem playlist(WLibraryItem::PlaylistNet, WLibraryItem::Default);

            playlist.source = source;

            playlist.title = title;
            playlist.cover = cover;

            reply.items.append(playlist);
        }
    }

    return reply;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ void WBackendSoundCloud::queryFailed(const WBackendNetQuery &)
{
    Q_D(WBackendSoundCloud); d->id.clear();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
void WBackendSoundCloud::applySource(const WBackendNetQuery  & query,
                                     const WBackendNetSource & source)
{
    Q_D(WBackendSoundCloud); d->setId(query, source.nextQuery);
}

/* Q_INVOKABLE virtual */
void WBackendSoundCloud::applyPlaylist(const WBackendNetQuery    & query,
                                       const WBackendNetPlaylist & playlist)
{
    Q_D(WBackendSoundCloud); d->setId(query, playlist.nextQuery);
}

/* Q_INVOKABLE virtual */
void WBackendSoundCloud::applyFolder(const WBackendNetQuery  & query,
                                     const WBackendNetFolder & folder)
{
    Q_D(WBackendSoundCloud); d->setId(query, folder.nextQuery);
}

#endif // SK_NO_BACKENDSOUNDCLOUD
