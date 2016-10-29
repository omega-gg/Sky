//=================================================================================================
/*
    Copyright (C) 2015-2016 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkBackend module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#include "WBackendDailymotion.h"

#ifndef SK_NO_BACKENDDAILYMOTION

#ifdef QT_LATEST
#include <QUrlQuery>
#endif

// Sk includes
#include <WControllerApplication>
#include <WControllerNetwork>

//-------------------------------------------------------------------------------------------------
// Static variables

static const QString BACKENDDAILYMOTION_FIELDS = "url,title,thumbnail_url,duration,owner.username,"
                                                 "owner.screenname,created_time,available_formats";

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include <private/WBackendNet_p>

class SK_BACKEND_EXPORT WBackendDailymotionPrivate : public WBackendNetPrivate
{
public:
    WBackendDailymotionPrivate(WBackendDailymotion * p);

    void init();

public: // Functions
    void loadTrack(WTrackNet * track, const QString & json) const;

    QString extractSource(const QString & json, const QString & quality) const;

    QString getSource(const QUrl & url) const;

    QUrl getUrlVideos(const QString & id) const;

protected:
    W_DECLARE_PUBLIC(WBackendDailymotion)
};

//-------------------------------------------------------------------------------------------------

WBackendDailymotionPrivate::WBackendDailymotionPrivate(WBackendDailymotion * p)
    : WBackendNetPrivate(p) {}

void WBackendDailymotionPrivate::init() {}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WBackendDailymotionPrivate::loadTrack(WTrackNet * track, const QString & json) const
{
    QString title = WControllerNetwork::extractJsonUtf8(json, "title");
    QString cover = WControllerNetwork::extractJson    (json, "thumbnail_url");

    QString author = WControllerNetwork::extractJsonUtf8(json, "owner.screenname");
    QString feed   = WControllerNetwork::extractJson    (json, "owner.username");

    QString duration = WControllerNetwork::extractJson(json, "duration");

    QString date = WControllerNetwork::extractJson(json, "created_time");

    QString quality = WControllerNetwork::extractJson(json, "available_formats");

    track->setTitle(title);
    track->setCover(cover);

    track->setAuthor(author);
    track->setFeed  (feed);

    track->setDuration(duration.toInt() * 1000);

    track->setDate(QDateTime::fromTime_t(date.toUInt()));

    if (quality.contains("hd720"))
    {
         track->setQuality(WAbstractBackend::QualityHigh);
    }
    else track->setQuality(WAbstractBackend::QualityMedium);
}

//-------------------------------------------------------------------------------------------------

QString WBackendDailymotionPrivate::extractSource(const QString & json,
                                                  const QString & quality) const
{
    QString source = WControllerNetwork::extractJson(json, quality);

    if (source.isEmpty() == false)
    {
        int index = source.indexOf("video/mp4");

        return WControllerNetwork::extractJson(source, "url", index);
    }
    else return QString();
}

//-------------------------------------------------------------------------------------------------

QString WBackendDailymotionPrivate::getSource(const QUrl & url) const
{
    QString source = WControllerNetwork::removeUrlPrefix(url);

    if (source.startsWith("games."))
    {
        source.remove(0, 6);
    }

    if (source.startsWith("dailymotion.com"))
    {
         return source;
    }
    else return QString();
}

//-------------------------------------------------------------------------------------------------

QUrl WBackendDailymotionPrivate::getUrlVideos(const QString & id) const
{
    QUrl url("https://api.dailymotion.com/" + id + "/videos");

#ifdef QT_LATEST
    QUrlQuery query(url);

    query.addQueryItem("fields", BACKENDDAILYMOTION_FIELDS);

    query.addQueryItem("limit", "50");

    url.setQuery(query);
#else
    url.addQueryItem("fields", BACKENDDAILYMOTION_FIELDS);

    url.addQueryItem("limit", "50");
#endif

    return url;
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

WBackendDailymotion::WBackendDailymotion() : WBackendNet(new WBackendDailymotionPrivate(this))
{
    Q_D(WBackendDailymotion); d->init();
}

//-------------------------------------------------------------------------------------------------
// WBackendNet implementation
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QList<WLibraryFolderItem> WBackendDailymotion::getLibraryItems() const
{
    WLibraryFolderItem tracks(WLibraryItem::PlaylistSearch);

    tracks.title = tr("Tracks");
    tracks.label = "tracks";

    WLibraryFolderItem channels(WLibraryItem::FolderSearch);

    channels.title = tr("Channels");
    channels.label = "channels";

    WLibraryFolderItem playlists(WLibraryItem::FolderSearch);

    playlists.title = tr("Playlists");
    playlists.label = "playlists";

    QList<WLibraryFolderItem> items;

    items.append(tracks);
    items.append(channels);
    items.append(playlists);

    return items;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QString WBackendDailymotion::getId() const
{
    return "dailymotion";
}

/* Q_INVOKABLE virtual */ QString WBackendDailymotion::getTitle() const
{
    return "Dailymotion";
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ bool WBackendDailymotion::checkValidUrl(const QUrl & url) const
{
    QString source = WControllerNetwork::removeUrlPrefix(url);

    if (source.startsWith("dailymotion.com") || source.startsWith("api.dailymotion.com")
        ||
        source.startsWith("games.dailymotion.com"))
    {
         return true;
    }
    else return false;
}

//-------------------------------------------------------------------------------------------------
// WBackendNet reimplementation
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QString WBackendDailymotion::getHost() const
{
    return "dailymotion.com";
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QString WBackendDailymotion::getTrackId(const QUrl & url) const
{
    Q_D(const WBackendDailymotion);

    QString source = d->getSource(url);

    if (source.isEmpty()) return QString();

    QRegExp regExp("/video/|#videoId=|#video=");

    int index = source.indexOf(regExp);

    if (index != -1)
    {
        index += regExp.matchedLength();

        return WControllerNetwork::extractUrlElement(source, index);
    }
    else return QString();
}

/* Q_INVOKABLE virtual */
WBackendNetPlaylistInfo WBackendDailymotion::getPlaylistInfo(const QUrl & url) const
{
    Q_D(const WBackendDailymotion);

    QString source = d->getSource(url);

    if (source.isEmpty()) return WBackendNetPlaylistInfo();

    if (source.startsWith("dailymotion.com/playlist/"))
    {
        return WBackendNetPlaylistInfo(WLibraryItem::PlaylistNet,
                                       WControllerNetwork::extractUrlElement(source, 25));
    }
    else
    {
        if (source.startsWith("dailymotion.com/group/"))
        {
            return WBackendNetPlaylistInfo(WLibraryItem::PlaylistFeed,
                                           WControllerNetwork::extractUrlElement(source, 22));
        }
        else if (source.startsWith("dailymotion.com/user/"))
        {
            return WBackendNetPlaylistInfo(WLibraryItem::PlaylistFeed,
                                           WControllerNetwork::extractUrlElement(source, 21));
        }
        else
        {
            QString id = WControllerNetwork::extractUrlElement(source, 16);

            if (id.isEmpty() || id.length() < 3
                ||
                source.indexOf(QRegExp("[/\\?#.]"), 16) != -1)
            {
                 return WBackendNetPlaylistInfo();
            }
            else return WBackendNetPlaylistInfo(WLibraryItem::PlaylistFeed, id);
        }
    }
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
QUrl WBackendDailymotion::getUrlTrack(const QString & id) const
{
    return "http://www.dailymotion.com/video/" + id;
}

/* Q_INVOKABLE virtual */
QUrl WBackendDailymotion::getUrlPlaylist(const WBackendNetPlaylistInfo & info) const
{
    if (info.isFeed())
    {
         return "http://www.dailymotion.com/user/" + info.id;
    }
    else return "http://www.dailymotion.com/playlist/" + info.id;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
WBackendNetQuery WBackendDailymotion::getQuerySource(const QUrl & url) const
{
    WBackendNetQuery backendQuery;

    QString id = getTrackId(url);

    if (id.isEmpty()) return backendQuery;

    backendQuery.url = "http://www.dailymotion.com/embed/video/" + id;

    return backendQuery;
}

/* Q_INVOKABLE virtual */
WBackendNetQuery WBackendDailymotion::getQueryTrack(const QUrl & url) const
{
    WBackendNetQuery backendQuery;

    QString id = getTrackId(url);

    if (id.isEmpty()) return backendQuery;

    QUrl source("https://api.dailymotion.com/video/" + id);

#ifdef QT_LATEST
    QUrlQuery query(source);

    query.addQueryItem("fields", BACKENDDAILYMOTION_FIELDS);

    source.setQuery(query);
#else
    source.addQueryItem("fields", BACKENDDAILYMOTION_FIELDS);
#endif

    backendQuery.url = source;

    return backendQuery;
}

/* Q_INVOKABLE virtual */
WBackendNetQuery WBackendDailymotion::getQueryPlaylist(const QUrl & url) const
{
    WBackendNetQuery backendQuery;

    QString id = getPlaylistInfo(url).id;

    if (id.isEmpty()) return backendQuery;

    Q_D(const WBackendDailymotion);

    QString source = d->getSource(url);

    if (source.startsWith("dailymotion.com/playlist/"))
    {
        QUrl url("https://api.dailymotion.com/playlist/" + id);

#ifdef QT_LATEST
        QUrlQuery query(url);

        query.addQueryItem("fields", "name,thumbnail_url");

        url.setQuery(query);
#else
        url.addQueryItem("fields", "name,thumbnail_url");
#endif

        backendQuery.url  = url;
        backendQuery.id   = 1;
        backendQuery.data = "playlist/" + id;
    }
    else if (source.startsWith("dailymotion.com/group/"))
    {
        QUrl url("https://api.dailymotion.com/group/" + id);

#ifdef QT_LATEST
        QUrlQuery query(url);

        query.addQueryItem("fields", "name,avatar_160_url");

        url.setQuery(query);
#else
        url.addQueryItem("fields", "name,avatar_160_url");
#endif

        backendQuery.url  = url;
        backendQuery.id   = 3;
        backendQuery.data = "group/" + id;
    }
    else
    {
        QUrl url("https://api.dailymotion.com/user/" + id);

#ifdef QT_LATEST
        QUrlQuery query(url);

        query.addQueryItem("fields", "screenname,avatar_720_url");

        url.setQuery(query);
#else
        url.addQueryItem("fields", "screenname,avatar_720_url");
#endif

        backendQuery.url  = url;
        backendQuery.id   = 2;
        backendQuery.data = "user/" + id;
    }

    return backendQuery;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
WBackendNetQuery WBackendDailymotion::createQuery(const QString & method,
                                                  const QString & label, const QString & q) const
{
    WBackendNetQuery backendQuery;

    if (method == "search")
    {
        if (label == "tracks")
        {
            QUrl url("https://api.dailymotion.com/videos");

#ifdef QT_LATEST
            QUrlQuery query(url);

            query.addQueryItem("search", q);

            query.addQueryItem("fields", BACKENDDAILYMOTION_FIELDS);
            query.addQueryItem("sort",   "relevance");

            query.addQueryItem("limit", "50");

            url.setQuery(query);
#else
            url.addQueryItem("search", q);

            url.addQueryItem("fields", BACKENDDAILYMOTION_FIELDS);
            url.addQueryItem("sort",   "relevance");

            url.addQueryItem("limit", "50");
#endif

            backendQuery.url = url;
        }
        else if (label == "playlists")
        {
            QUrl url("https://api.dailymotion.com/playlists");

#ifdef QT_LATEST
            QUrlQuery query(url);

            query.addQueryItem("search", q);

            query.addQueryItem("fields", "id,name,thumbnail_url");
            query.addQueryItem("sort",   "relevance");

            query.addQueryItem("limit", "20");

            url.setQuery(query);
#else
            url.addQueryItem("search", q);

            url.addQueryItem("fields", "id,name,thumbnail_url");
            url.addQueryItem("sort",   "relevance");

            url.addQueryItem("limit", "20");
#endif

            backendQuery.url = url;
        }
        else if (label == "channels")
        {
            QUrl url("https://api.dailymotion.com/users");

#ifdef QT_LATEST
            QUrlQuery query(url);

            query.addQueryItem("search", q);

            query.addQueryItem("fields", "id,screenname,avatar_720_url");
            query.addQueryItem("sort",   "relevance");

            query.addQueryItem("limit", "20");

            url.setQuery(query);
#else
            url.addQueryItem("search", q);

            url.addQueryItem("fields", "id,screenname,avatar_720_url");
            url.addQueryItem("sort",   "relevance");

            url.addQueryItem("limit", "20");
#endif

            backendQuery.url = url;
            backendQuery.id  = 1;
        }
        else if (label == "groups")
        {
            QUrl url("https://api.dailymotion.com/groups");

#ifdef QT_LATEST
            QUrlQuery query(url);

            query.addQueryItem("search", q);

            query.addQueryItem("fields", "url_name,name,avatar_160_url");
            query.addQueryItem("sort",   "relevance");

            query.addQueryItem("limit", "20");

            url.setQuery(query);
#else
            url.addQueryItem("search", q);

            url.addQueryItem("fields", "url_name,name,avatar_160_url");
            url.addQueryItem("sort",   "relevance");

            url.addQueryItem("limit", "20");
#endif

            backendQuery.url = url;
            backendQuery.id  = 2;
        }
    }
    else if (method == "related" && label == "tracks")
    {
        QUrl url("https://api.dailymotion.com/video/" + q + "/related");

#ifdef QT_LATEST
        QUrlQuery query(url);

        query.addQueryItem("fields", BACKENDDAILYMOTION_FIELDS);

        query.addQueryItem("limit", "50");

        url.setQuery(query);
#else
        url.addQueryItem("fields", BACKENDDAILYMOTION_FIELDS);

        url.addQueryItem("limit", "50");
#endif

        backendQuery.url = url;
    }

    return backendQuery;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
WBackendNetSource WBackendDailymotion::extractSource(const QByteArray       & data,
                                                     const WBackendNetQuery &) const
{
    Q_D(const WBackendDailymotion);

    WBackendNetSource source;

    QString content = Sk::readUtf8(data);

    QString json = WControllerNetwork::extractJsonHtml(content, "qualities");

    QStringList urls;

    urls.append(d->extractSource(json,  "240"));
    urls.append(d->extractSource(json,  "380"));
    urls.append(d->extractSource(json,  "480"));
    urls.append(d->extractSource(json,  "720"));
    urls.append(d->extractSource(json, "1080"));

    QHash<WAbstractBackend::Quality, QUrl> * medias = &(source.medias);

    for (int i = WAbstractBackend::QualityMinimum; i < WAbstractBackend::QualityMaximum; i++)
    {
        const QString & url = urls.at(i - 1);

        if (url.isEmpty() == false)
        {
            medias->insert(static_cast<WAbstractBackend::Quality> (i), url);
        }
    }

    return source;
}

/* Q_INVOKABLE virtual */
WBackendNetTrack WBackendDailymotion::extractTrack(const QByteArray       & data,
                                                   const WBackendNetQuery &) const
{
    Q_D(const WBackendDailymotion);

    WBackendNetTrack reply;

    QString content = Sk::readUtf8(data);

    QString json = WControllerNetwork::extractJsonHtml(content);

    d->loadTrack(&(reply.track), json);

    return reply;
}

/* Q_INVOKABLE virtual */
WBackendNetPlaylist WBackendDailymotion::extractPlaylist(const QByteArray       & data,
                                                         const WBackendNetQuery & query) const
{
    Q_D(const WBackendDailymotion);

    WBackendNetPlaylist reply;

    QString content = Sk::readUtf8(data);

    if (query.id == 1) // playlist
    {
        QString json = WControllerNetwork::extractJsonHtml(content);

        reply.title = WControllerNetwork::extractJsonUtf8(json, "name");
        reply.cover = WControllerNetwork::extractJson    (json, "thumbnail_url");

        reply.nextQuery.url = d->getUrlVideos(query.data.toString());
    }
    else if (query.id == 2) // channel
    {
        QString json = WControllerNetwork::extractJsonHtml(content);

        reply.title = WControllerNetwork::extractJsonUtf8(json, "screenname");
        reply.cover = WControllerNetwork::extractJson    (json, "avatar_720_url");

        reply.nextQuery.url = d->getUrlVideos(query.data.toString());
    }
    else if (query.id == 3) // group
    {
        QString json = WControllerNetwork::extractJsonHtml(content);

        reply.title = WControllerNetwork::extractJsonUtf8(json, "name");
        reply.cover = WControllerNetwork::extractJson    (json, "avatar_160_url");

        reply.nextQuery.url = d->getUrlVideos(query.data.toString());
    }
    else // search tracks
    {
        QString json = WControllerNetwork::extractJsonHtml(content, "list");

        QStringList list = WControllerNetwork::splitJson(json);

        foreach (const QString & string, list)
        {
            QString source = WControllerNetwork::extractJson(string, "url");

            WTrackNet track(source);

            d->loadTrack(&track, string);

            reply.tracks.append(track);
        }
    }

    return reply;
}

/* Q_INVOKABLE virtual */
WBackendNetFolder WBackendDailymotion::extractFolder(const QByteArray       & data,
                                                     const WBackendNetQuery & query) const
{
    WBackendNetFolder reply;

    QString content = Sk::readUtf8(data);

    QString json = WControllerNetwork::extractJsonHtml(content, "list");

    QStringList list = WControllerNetwork::splitJson(json);

    if (query.id == 1) // channel
    {
        foreach (const QString & string, list)
        {
            QString id = WControllerNetwork::extractJson(string, "id");

            QString title = WControllerNetwork::extractJsonUtf8(string, "screenname");
            QString cover = WControllerNetwork::extractJson    (string, "avatar_720_url");

            WLibraryFolderItem playlist(WLibraryItem::PlaylistFeed, WLibraryItem::Default);

            playlist.source = "http://www.dailymotion.com/user/" + id;

            playlist.title = title;
            playlist.cover = cover;

            reply.items.append(playlist);
        }
    }
    else if (query.id == 2) // groups
    {
        foreach (const QString & string, list)
        {
            QString id = WControllerNetwork::extractJson(string, "url_name");

            QString title = WControllerNetwork::extractJsonUtf8(string, "name");
            QString cover = WControllerNetwork::extractJson    (string, "avatar_160_url");

            WLibraryFolderItem playlist(WLibraryItem::PlaylistFeed, WLibraryItem::Default);

            playlist.source = "http://www.dailymotion.com/group/" + id;

            playlist.title = title;
            playlist.cover = cover;

            reply.items.append(playlist);
        }
    }
    else // playlists
    {
        foreach (const QString & string, list)
        {
            QString id = WControllerNetwork::extractJson(string, "id");

            QString title = WControllerNetwork::extractJsonUtf8(string, "name");
            QString cover = WControllerNetwork::extractJson    (string, "thumbnail_url");

            WLibraryFolderItem playlist(WLibraryItem::PlaylistNet, WLibraryItem::Default);

            playlist.source = "http://www.dailymotion.com/playlist/" + id;

            playlist.title = title;
            playlist.cover = cover;

            reply.items.append(playlist);
        }
    }

    return reply;
}

#endif // SK_NO_BACKENDDAILYMOTION
