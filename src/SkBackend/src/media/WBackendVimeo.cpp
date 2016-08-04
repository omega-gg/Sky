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

#include "WBackendVimeo.h"

#ifndef SK_NO_BACKENDVIMEO

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

class SK_BACKEND_EXPORT WBackendVimeoPrivate : public WBackendNetPrivate
{
public:
    WBackendVimeoPrivate(WBackendVimeo * p);

    void init();

public: // Functions
    QString extractId(const QString & source) const;

    WAbstractBackend::Quality extractQuality(const QString & data) const;

    QString extractCover(const QString & cover) const;

    QString getNextUrl(const WBackendNetQuery & query, const QString & data, int id) const;

protected:
    W_DECLARE_PUBLIC(WBackendVimeo)
};

//-------------------------------------------------------------------------------------------------

WBackendVimeoPrivate::WBackendVimeoPrivate(WBackendVimeo * p) : WBackendNetPrivate(p) {}

void WBackendVimeoPrivate::init() {}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

QString WBackendVimeoPrivate::extractId(const QString & source) const
{
    int index = source.lastIndexOf('/');

    if (index == -1) return QString();

    QString id = WControllerNetwork::extractUrlElement(source, index + 1);

    if (id.toInt() == 0 || source.contains("/groups/" + id) || source.contains("/channels/" + id))
    {
         return QString();
    }
    else return id;
}

//-------------------------------------------------------------------------------------------------

WAbstractBackend::Quality WBackendVimeoPrivate::extractQuality(const QString & data) const
{
    QString quality = WControllerNetwork::extractJson(data, "quality");

    if (quality == "270p" || quality == "360p")
    {
        return WAbstractBackend::QualityLow;
    }
    else if (quality == "480p")
    {
        return WAbstractBackend::QualityMedium;
    }
    else if (quality == "720p")
    {
        return WAbstractBackend::QualityHigh;
    }
    else if (quality == "1080p")
    {
        return WAbstractBackend::QualityUltra;
    }
    else return WAbstractBackend::QualityInvalid;
}

//-------------------------------------------------------------------------------------------------

QString WBackendVimeoPrivate::extractCover(const QString & cover) const
{
    if (cover.isEmpty())
    {
         return QString();
    }
    else if (cover.startsWith("https://vimeo.com"))
    {
        int index = cover.lastIndexOf('/') + 1;

        QString id = cover.mid(index);

        return "https://i.vimeocdn.com/video/" + id +  + "_1280.jpg";
    }
    else return Sk::slice(cover, "", "_") + "1280.jpg";
}

//-------------------------------------------------------------------------------------------------

QString WBackendVimeoPrivate::getNextUrl(const WBackendNetQuery & query,
                                         const QString          & data, int id) const
{
    QString paging = WControllerNetwork::extractJson(data, "paging");

    if (paging.isEmpty())
    {
        if (data.indexOf("<li class=\"pagination_next") == -1)
        {
            return QString();
        }
    }
    else if (WControllerNetwork::extractJson(paging, "next") == "null")
    {
        return QString();
    }

    QString url = query.url.toString();

    url.replace("/page:" + QString::number(id),
                "/page:" + QString::number(id + 1));

    return url;
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

WBackendVimeo::WBackendVimeo() : WBackendNet(new WBackendVimeoPrivate(this))
{
    Q_D(WBackendVimeo); d->init();
}

//-------------------------------------------------------------------------------------------------
// WBackendNet implementation
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QList<WLibraryFolderItem> WBackendVimeo::getLibraryItems() const
{
    WLibraryFolderItem tracks(WLibraryItem::PlaylistSearch);

    tracks.title = tr("Tracks");
    tracks.label = "tracks";

    WLibraryFolderItem people(WLibraryItem::FolderSearch);

    people.title = tr("People");
    people.label = "people";

    WLibraryFolderItem channels(WLibraryItem::FolderSearch);

    channels.title = tr("Channels");
    channels.label = "channels";

    WLibraryFolderItem groups(WLibraryItem::FolderSearch);

    groups.title = tr("Groups");
    groups.label = "groups";

    QList<WLibraryFolderItem> items;

    items.append(tracks);
    items.append(people);
    items.append(channels);
    items.append(groups);

    return items;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QString WBackendVimeo::getId() const
{
    return "vimeo";
}

/* Q_INVOKABLE virtual */ QString WBackendVimeo::getTitle() const
{
    return "Vimeo";
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ bool WBackendVimeo::checkValidUrl(const QUrl & url) const
{
    QString source = WControllerNetwork::removeUrlPrefix(url);

    if (source.startsWith("vimeo.com") || source.startsWith("player.vimeo.com"))
    {
         return true;
    }
    else return false;
}

//-------------------------------------------------------------------------------------------------
// WBackendNet reimplementation
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QString WBackendVimeo::getHost() const
{
    return "vimeo.com";
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QString WBackendVimeo::getTrackId(const QUrl & url) const
{
    Q_D(const WBackendVimeo);

    return d->extractId(url.toString());
}

/* Q_INVOKABLE virtual */
WBackendNetPlaylistInfo WBackendVimeo::getPlaylistInfo(const QUrl & url) const
{
    QString source = WControllerNetwork::removeUrlPrefix(url);

    if (source.startsWith("vimeo.com") == false)
    {
        return WBackendNetPlaylistInfo();
    }

    if (source.startsWith("vimeo.com/channels/") || source.startsWith("vimeo.com/groups/"))
    {
        return WBackendNetPlaylistInfo(WLibraryItem::PlaylistFeed,
                                       WControllerNetwork::extractUrlElements(source, 2, 10));
    }
    else if (source.startsWith("vimeo.com/"))
    {
        QString id = WControllerNetwork::extractUrlElement(source, 10);

        if (id.toInt())
        {
             return WBackendNetPlaylistInfo();
        }
        else return WBackendNetPlaylistInfo(WLibraryItem::PlaylistFeed, id);
    }
    else return WBackendNetPlaylistInfo();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
QUrl WBackendVimeo::getUrlTrack(const QString & id) const
{
    return "https://vimeo.com/" + id;
}

/* Q_INVOKABLE virtual */
QUrl WBackendVimeo::getUrlPlaylist(const WBackendNetPlaylistInfo & info) const
{
    return "https://vimeo.com/" + info.id;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
WBackendNetQuery WBackendVimeo::getQuerySource(const QUrl & url) const
{
    WBackendNetQuery backendQuery;

    QString id = getTrackId(url);

    if (id.isEmpty()) return backendQuery;

    backendQuery.url = "https://player.vimeo.com/video/" + id;

    return backendQuery;
}

/* Q_INVOKABLE virtual */
WBackendNetQuery WBackendVimeo::getQueryTrack(const QUrl & url) const
{
    WBackendNetQuery backendQuery;

    QString id = getTrackId(url);

    if (id.isEmpty()) return backendQuery;

    backendQuery.url = "https://vimeo.com/" + id;

    return backendQuery;
}

/* Q_INVOKABLE virtual */
WBackendNetQuery WBackendVimeo::getQueryPlaylist(const QUrl & url) const
{
    WBackendNetQuery backendQuery;

    QString id = getPlaylistInfo(url).id;

    if (id.isEmpty()) return backendQuery;

    if (id.startsWith("tag:"))
    {
         backendQuery.url = "https://vimeo.com/" + id + "/page:1/sort:date/format:thumbnail";
    }
    else backendQuery.url = "https://vimeo.com/" + id
                            +
                            "/videos/page:1/sort:date/format:thumbnail";

    return backendQuery;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
WBackendNetQuery WBackendVimeo::createQuery(const QString & method,
                                            const QString & label, const QString & q) const
{
    WBackendNetQuery backendQuery;

    if (method == "search")
    {
        if (label == "tracks")
        {
            QUrl url("https://vimeo.com/search/page:1/sort:relevant/format:thumbnail");

#ifdef QT_LATEST
            QUrlQuery query(url);

            query.addQueryItem("type", "video");

            query.addQueryItem("q", q);

            url.setQuery(query);
#else
            url.addQueryItem("type", "video");

            url.addQueryItem("q", q);
#endif

            backendQuery.type = WBackendNetQuery::TypeWeb;
            backendQuery.url  = url;
            backendQuery.id   = 1;
        }
        else if (label == "people")
        {
            QUrl url("https://vimeo.com/search/people/page:1/sort:relevant/format:thumbnail");

#ifdef QT_LATEST
            QUrlQuery query(url);

            query.addQueryItem("q", q);

            url.setQuery(query);
#else
            url.addQueryItem("q", q);
#endif

            backendQuery.type = WBackendNetQuery::TypeWeb;
            backendQuery.url  = url;
            backendQuery.id   = 1;
        }
        else if (label == "channels")
        {
            QUrl url("https://vimeo.com/search/channels/page:1/sort:relevant/format:thumbnail");

#ifdef QT_LATEST
            QUrlQuery query(url);

            query.addQueryItem("q", q);

            url.setQuery(query);
#else
            url.addQueryItem("q", q);
#endif

            backendQuery.type = WBackendNetQuery::TypeWeb;
            backendQuery.url  = url;
        }
        else if (label == "groups")
        {
            QUrl url("https://vimeo.com/search/groups/page:1/sort:relevant/format:thumbnail");

#ifdef QT_LATEST
            QUrlQuery query(url);

            query.addQueryItem("q", q);

            url.setQuery(query);
#else
            url.addQueryItem("q", q);
#endif

            backendQuery.type = WBackendNetQuery::TypeWeb;
            backendQuery.url  = url;
        }
    }
    else if (method == "related" && label == "tracks")
    {
        backendQuery.url = "https://vimeo.com/" + q
                           +
                           "/collections/channels/sort:relevant/format:thumbnail";

        backendQuery.id = 2;
    }

    return backendQuery;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
WBackendNetSource WBackendVimeo::extractSource(const QByteArray       & data,
                                               const WBackendNetQuery &) const
{
    Q_D(const WBackendVimeo);

    WBackendNetSource source;

    QString content = Sk::readUtf8(data);

    QHash<WAbstractBackend::Quality, QUrl> * medias = &(source.medias);

    QString json = WControllerNetwork::extractJsonHtml(content, "files");

    json = WControllerNetwork::extractJsonHtml(content, "progressive");

    QStringList list = WControllerNetwork::splitJson(json);

    foreach (const QString & string, list)
    {
        WAbstractBackend::Quality quality = d->extractQuality(string);

        if (quality != WAbstractBackend::QualityInvalid)
        {
            QUrl url = WControllerNetwork::extractJson(string, "url");

            medias->insert(quality, url);
        }
    }

    return source;
}

/* Q_INVOKABLE virtual */
WBackendNetTrack WBackendVimeo::extractTrack(const QByteArray       & data,
                                             const WBackendNetQuery &) const
{
    Q_D(const WBackendVimeo);

    WBackendNetTrack reply;

    QString content = Sk::readUtf8(data);

    content = Sk::slice(content, "<script type=\"application", "</script>");

    QString title = WControllerNetwork::extractJsonUtf8(content, "name");
    QString cover = WControllerNetwork::extractJson    (content, "thumbnailUrl");

    cover = d->extractCover(cover);

    QString author = WControllerNetwork::extractJsonUtf8(content, "author");

    QString feed = WControllerNetwork::extractJson(author, "url");

    feed = WControllerNetwork::extractUrlPath(feed);

    author = WControllerNetwork::extractJson(author, "name");

    QString duration = WControllerNetwork::extractJson(content, "duration");

    duration = Sk::sliceIn(duration, "T", "S");

    QString date = WControllerNetwork::extractJson(content, "uploadDate");

    if (date.isEmpty())
    {
        date = WControllerNetwork::extractJson(content, "datePublished");
    }

    date = date.mid(0, date.lastIndexOf('-'));

    QString quality = WControllerNetwork::extractJson(content, "videoQuality");

    WTrackNet * track = &(reply.track);

    track->setTitle(title);
    track->setCover(cover);

    track->setAuthor(author);
    track->setFeed  (feed);

    QTime time = QTime::fromString(duration, "hh'H'mm'M'ss");

    track->setDuration(Sk::getMsecs(time));

    track->setDate(QDateTime::fromString(date, "yyyy-MM-ddThh:mm:ss"));

    if (quality == "HD")
    {
         track->setQuality(WAbstractBackend::QualityHigh);
    }
    else track->setQuality(WAbstractBackend::QualityMedium);

    return reply;
}

/* Q_INVOKABLE virtual */
WBackendNetPlaylist WBackendVimeo::extractPlaylist(const QByteArray       & data,
                                                   const WBackendNetQuery & query) const
{
    Q_D(const WBackendVimeo);

    WBackendNetPlaylist reply;

    QString content = Sk::readUtf8(data);

    if (query.id == 2)
    {
        QStringList urls = query.data.toStringList();

        int index = content.indexOf("<ol class=\"js-browse_list");

        QString string;

        if (index == -1)
        {
             string = Sk::slice(content, "<ul class=\"small-block-grid", "</ul>");
        }
        else string = Sk::slice(content, "<ol class=\"js-browse_list", "</ol>");

        QStringList list = Sk::slices(string, "<li ", "</li>");

        for (int i = 0; i < list.count() && i < 3; i++)
        {
            const QString & string = list.at(i);

            QString id = WControllerNetwork::extractAttribute(string, "href");

            urls.append("https://vimeo.com" + id + "/videos/page:1/sort:relevant/format:thumbnail");
        }

        if (urls.isEmpty() == false)
        {
            WBackendNetQuery * nextQuery = &(reply.nextQuery);

            nextQuery->type = WBackendNetQuery::TypeWeb;
            nextQuery->url  = urls.takeFirst();

            if (urls.isEmpty() == false)
            {
                nextQuery->id   = 3;
                nextQuery->data = urls;
            }
            else nextQuery->id = 4;
        }

        return reply;
    }
    else if (query.id == 3)
    {
        QStringList urls = query.data.toStringList();

        WBackendNetQuery * nextQuery = &(reply.nextQuery);

        nextQuery->type = WBackendNetQuery::TypeWeb;
        nextQuery->url  = urls.takeFirst();

        if (urls.isEmpty() == false)
        {
            nextQuery->id   = 3;
            nextQuery->data = urls;
        }
        else nextQuery->id = 4;

        reply.clearDuplicate = true;
    }
    else if (query.id == 4)
    {
        reply.clearDuplicate = true;
    }
    else
    {
        int id = query.data.toInt() + 1;

        if (id < 3)
        {
            if (query.id == 0 && id == 1)
            {
                QString title = WControllerNetwork::extractValue(content, "title");

                if (title.contains("&quot;"))
                {
                     title = Sk::sliceIn(title, "&quot;", "&quot;");
                }
                else title = Sk::sliceIn(title, "", "&rsquo;");

                reply.title = WControllerNetwork::htmlToUtf8(title);
            }

            QString url = d->getNextUrl(query, content, id);

            if (url.isEmpty() == false)
            {
                WBackendNetQuery * nextQuery = &(reply.nextQuery);

                nextQuery->type = WBackendNetQuery::TypeWeb;
                nextQuery->url  = url;
                nextQuery->id   = query.id;
                nextQuery->data = id;
            }
        }

        if (query.id == 1)
        {
            QString json = WControllerNetwork::extractJsonHtml(content, "data");

            QStringList list = WControllerNetwork::splitJson(json);

            foreach (const QString & string, list)
            {
                QString privacy = WControllerNetwork::extractJson(string, "privacy");

                if (privacy == "\"view\":\"ptv\"") continue;

                QString id = WControllerNetwork::extractJson(string, "uri");

                id = id.mid(8);

                QString title = WControllerNetwork::extractJsonUtf8(string, "name");

                QString cover = WControllerNetwork::extractJson(string, "pictures");

                cover = WControllerNetwork::extractJson(cover, "link");

                cover = d->extractCover(cover);

                WTrackNet track("https://vimeo.com/" + id, WAbstractTrack::Default);

                track.setTitle(title);
                track.setCover(cover);

                reply.tracks.append(track);
            }

            return reply;
        }
    }

    QString string = Sk::slice(content, "<ol class=\"js-browse_list", "</ol>");

    QStringList list = Sk::slices(string, "<li ", "</li>");

    foreach (const QString & string, list)
    {
        QString id = WControllerNetwork::extractAttribute(string, "href");

        id = d->extractId(id);

        QString title = WControllerNetwork::extractAttributeUtf8(string, "title");
        QString cover = WControllerNetwork::extractAttribute    (string, "src");

        cover = d->extractCover(cover);

        WTrackNet track("https://vimeo.com/" + id, WAbstractTrack::Default);

        track.setTitle(title);
        track.setCover(cover);

        reply.tracks.append(track);
    }

    return reply;
}

/* Q_INVOKABLE virtual */
WBackendNetFolder WBackendVimeo::extractFolder(const QByteArray       & data,
                                               const WBackendNetQuery & query) const
{
    Q_D(const WBackendVimeo);

    WBackendNetFolder reply;

    QString content = Sk::readUtf8(data);

    QString json = WControllerNetwork::extractJsonHtml(content, "data");

    QStringList list = WControllerNetwork::splitJson(json);

    foreach (const QString & string, list)
    {
        QString source = WControllerNetwork::extractJson(string, "link");

        QString title = WControllerNetwork::extractJsonUtf8(string, "name");

        QString cover = WControllerNetwork::extractJson(string, "pictures");

        cover = WControllerNetwork::extractJson(cover, "link");

        cover = d->extractCover(cover);

        WLibraryFolderItem playlist(WLibraryItem::PlaylistFeed, WLibraryItem::Default);

        playlist.source = source;

        playlist.title = title;
        playlist.cover = cover;

        reply.items.append(playlist);
    }

    int id = query.data.toInt() + 1;

    if (id < 3)
    {
        QString url = d->getNextUrl(query, content, id);

        if (url.isEmpty() == false)
        {
            WBackendNetQuery * nextQuery = &(reply.nextQuery);

            nextQuery->type = WBackendNetQuery::TypeWeb;
            nextQuery->url  = url;
            nextQuery->id   = query.id;
            nextQuery->data = id;
        }
    }

    return reply;
}

#endif // SK_NO_BACKENDVIMEO
