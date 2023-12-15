//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkGui.

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

#include "WControllerPlaylist.h"

#ifndef SK_NO_CONTROLLERPLAYLIST

// Qt includes
#include <QThread>
#include <QDir>
#ifdef QT_NEW
#include <QUrlQuery>
#endif
#ifdef SK_DESKTOP
#ifdef Q_OS_WIN
    #include <QCoreApplication>
    #include <QSettings>
#endif
#endif // SK_DESKTOP

// Sk includes
#include <WControllerApplication>
#include <WControllerFile>
#include <WControllerNetwork>
#include <WControllerDownload>
#include <WRegExp>
#include <WLoaderVbml>
#include <WPixmapCache>
#include <WPlaylist>
#include <WTabTrack>
#include <WBackendLoader>
#include <WYamlReader>

// macOS includes
#ifdef Q_OS_MACX
#include <CoreServices/CoreServices.h>
#endif

// Private includes
#include <private/WPlaylist_p>
#include <private/WBackendLoader_p>

W_INIT_CONTROLLER(WControllerPlaylist)

//-------------------------------------------------------------------------------------------------
// Functions declarations

void WControllerPlaylist_patch(QString & data, const QString & api);

//-------------------------------------------------------------------------------------------------
// Static variables

static const QString CONTROLLERPLAYLIST_VIDEO = "^(mp4|webm|ogv|mkv|avi|wmv|mov|flv|3gp|m3u8)$";
static const QString CONTROLLERPLAYLIST_AUDIO = "^(mp3|ogg|mka|wav|wma|flac)$";

static const QString CONTROLLERPLAYLIST_MARKUP = "^(vbml|html|xml|json)$";

static const QString CONTROLLERPLAYLIST_TEXT = "^(txt|md)$";

static const QString CONTROLLERPLAYLIST_SUBTITLE = "^(srt)$";

static const QString CONTROLLERPLAYLIST_M3U = "^(m3u|m3u8)$";

#ifndef SK_NO_TORRENT
static const QString CONTROLLERPLAYLIST_TORRENT = "^(torrent)$";
#endif

static const QString CONTROLLERPLAYLIST_VBML = "^(vbml)$";

static const QString CONTROLLERPLAYLIST_FILTER_FILE
    =
    "Medias (*.mp4 *.webm *.ogv *.mkv *.avi *.wmv *.mov *.flv *.3gp *.m3u8 "
            "*.mp3 *.ogg *.mka *.wav *.wma *.flac "
#ifdef SK_NO_TORRENT
            "*.vbml *.html *.xml *.json "
#else
            "*.vbml *.html *.xml *.json *.torrent "
#endif
            "*.txt *.md);;"
    "All files (*)";

static const QString CONTROLLERPLAYLIST_FILTER_SUBTITLE = "Subtitles (*.srt)";

//-------------------------------------------------------------------------------------------------

static const QString CONTROLLERPLAYLIST_PATH_PLAYLISTS = "/playlists";
static const QString CONTROLLERPLAYLIST_PATH_TABS      = "/tabs";

//-------------------------------------------------------------------------------------------------

static const int CONTROLLERPLAYLIST_MAX_SIZE = 1048576 * 10; // 10 megabytes

static const int CONTROLLERPLAYLIST_MAX_TRACKS = 500;
static const int CONTROLLERPLAYLIST_MAX_ITEMS  = 500;

static const int CONTROLLERPLAYLIST_MAX_QUERY  = 100;
static const int CONTROLLERPLAYLIST_MAX_RELOAD =  10;

static const int CONTROLLERPLAYLIST_CHANNEL_DURATION = 604800000; // 7 days in milliseconds

//=================================================================================================
// WControllerPlaylistLoader
//=================================================================================================

class SK_GUI_EXPORT WControllerPlaylistLoader : public QObject
{
    Q_OBJECT

public:
    WControllerPlaylistLoader(WLibraryFolder   * folder,
                              const QList<int> & idFull, QObject * parent = NULL);

public: // Interface
    Q_INVOKABLE void load();

private: // Functions
    void create();

private slots:
    void onLoaded   ();
    void onDestroyed();

private: // Variables
    WLibraryFolder * folder;

    QList<int> idFull;
};

//-------------------------------------------------------------------------------------------------

WControllerPlaylistLoader::WControllerPlaylistLoader(WLibraryFolder   * folder,
                                                     const QList<int> & idFull, QObject * parent)
    : QObject(parent)
{
    this->folder = folder;

    this->idFull = idFull;
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WControllerPlaylistLoader::load()
{
    if (folder->isLoading())
    {
        connect(folder, SIGNAL(loaded   ()), this, SLOT(onLoaded   ()));
        connect(folder, SIGNAL(destroyed()), this, SLOT(onDestroyed()));
    }
    else create();
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WControllerPlaylistLoader::create()
{
    int id = idFull.takeFirst();

    WLibraryItem * item = folder->createLibraryItemFromId(id);

    if (item == NULL)
    {
        delete this;

        return;
    }

    if (idFull.isEmpty())
    {
        item->tryDelete();

        delete this;

        return;
    }

    folder = item->toFolder();

    if (folder == NULL)
    {
        item->tryDelete();

        delete this;

        return;
    }

    load();

    item->tryDelete();
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WControllerPlaylistLoader::onLoaded()
{
    disconnect(folder, 0, this, 0);

    create();
}

void WControllerPlaylistLoader::onDestroyed()
{
    delete this;
}

//=================================================================================================
// WControllerPlaylistQuery
//=================================================================================================

WControllerPlaylistQuery::WControllerPlaylistQuery(const WBackendNetQuery & backendQuery,
                                                   Type                     type)
{
    this->backendQuery = backendQuery;
    this->type         = type;

    backend = NULL;

    data  = NULL;
    reply = NULL;

    item = NULL;

    track = NULL;
}

//=================================================================================================
// WControllerPlaylistData
//=================================================================================================
// Interface

void WControllerPlaylistData::applyVbml(const QByteArray & array, const QString & url,
                                                                  const QString & urlBase)
{
    QString content = Sk::readBml(array);

    //---------------------------------------------------------------------------------------------
    // Api

    QString api = WControllerPlaylist::vbmlVersion(content);

    if (api.isEmpty())
    {
        if (WControllerPlaylist::textIsRedirect(content, url))
        {
            type = WControllerPlaylist::Redirect;

            origin = content;
            source = content;

            return;
        }

        // NOTE: If it's HTML we try to extract a VBML link.
        if (array.contains("<html>"))
        {
            origin = WControllerPlaylistData::extractHtmlLink(array, url);

            if (origin.isEmpty() == false)
            {
                type = WControllerPlaylist::Redirect;

                return;
            }
        }
    }

    if (Sk::versionIsHigher(WControllerPlaylist::versionApi(), api))
    {
        WControllerPlaylist::vbmlPatch(content, api);

        applyVbml(content.toUtf8(), url, urlBase);

        return;
    }

    if (Sk::versionIsLower(WControllerPlaylist::versionApi(), api))
    {
        qWarning("WControllerPlaylistData::applyVbml: The required API is too high.");
    }

    WYamlReader reader(content.toUtf8());

    //---------------------------------------------------------------------------------------------
    // Settings

    QString string = reader.extractString("origin");

    if (string.isEmpty() == false
        &&
        // NOTE: The origin has to be different than the current URL.
        WControllerNetwork::removeUrlPrefix(url) != WControllerNetwork::removeUrlPrefix(string))
    {
        origin = string;
    }

    string = reader.extractString("type");

    type = WControllerPlaylist::vbmlTypeFromString(string);

    if (WControllerPlaylist::vbmlTypeTrack(type))
    {
        // NOTE: We keep a base url for our track source and make it compliant.
        parseTrack(reader, string, WControllerPlaylist::generateSource(urlBase));
    }
    else if (WControllerPlaylist::vbmlTypePlaylist(type))
    {
        parsePlaylist(reader);
    }
    else // NOTE: We default to the playlist type.
    {
        parsePlaylist(reader);

        // NOTE: When the playlist is invalid we add the url itself given it could be a media.
        if (title.isEmpty() && tracks.isEmpty()
            &&
            // NOTE: The url should not be a vbml run uri.
            WControllerPlaylist::urlIsVbmlRun(url) == false)
        {
            addMedia(url, WControllerNetwork::extractUrlFileName(url));
        }
    }
}

void WControllerPlaylistData::applyRelated(const QByteArray & array, const QString & url,
                                                                     const QString & urlBase)
{
    QString content = Sk::readBml(array);

    //---------------------------------------------------------------------------------------------
    // Api

    QString api = WControllerPlaylist::vbmlVersion(content);

    if (api.isEmpty())
    {
        if (WControllerPlaylist::textIsRedirect(content, url))
        {
            type = WControllerPlaylist::Redirect;

            origin = WControllerPlaylist::createSource("vbml", "related", "tracks", content);

            return;
        }

        // NOTE: If it's HTML we try to extract a VBML link.
        if (array.contains("<html>"))
        {
            origin = WControllerPlaylistData::extractHtmlLink(array, url);

            if (origin.isEmpty() == false)
            {
                type = WControllerPlaylist::Redirect;

                origin = WControllerPlaylist::createSource("vbml", "related", "tracks", origin);
            }

            return;
        }
    }

    if (Sk::versionIsHigher(WControllerPlaylist::versionApi(), api))
    {
        WControllerPlaylist::vbmlPatch(content, api);

        applyRelated(content.toUtf8(), url, urlBase);

        return;
    }

    if (Sk::versionIsLower(WControllerPlaylist::versionApi(), api))
    {
        qWarning("WControllerPlaylistData::applyRelated: The required API is too high.");
    }

    WYamlReader reader(content.toUtf8());

    //---------------------------------------------------------------------------------------------
    // Settings

    QString string = reader.extractString("origin");

    if (string.isEmpty() == false
        &&
        // NOTE: The origin has to be different than the current URL.
        WControllerNetwork::removeUrlPrefix(url) != WControllerNetwork::removeUrlPrefix(string))
    {
        type = WControllerPlaylist::Redirect;

        origin = WControllerPlaylist::createSource("vbml", "related", "tracks", string);

        return;
    }

    if (currentTime == -1)
    {
        origin = extractRelated(reader.node());

        if (origin.isEmpty() == false)
        {
            type = WControllerPlaylist::Related;

            return;
        }

        // NOTE: When there's no related specified we try with a default timestamp.
        currentTime = 0;
    }

    const WYamlNode * node = reader.at("source");

    if (node)
    {
        const QList<WYamlNode> & children = node->children;

        WTrack::Type type = WTrack::typeFromString(reader.extractString("type"));

        if (type == WTrack::Channel)
        {
            QString timeZone = reader.extractString("timezone");

            QDateTime date = WControllerApplication::currentDateUtc(timeZone);

            currentTime = WControllerApplication::getMsecsWeek(date);

            if (children.isEmpty())
            {
                applySource(reader.node(), node->value, CONTROLLERPLAYLIST_CHANNEL_DURATION);
            }
            else extractSource(node->children);
        }
        else if (children.isEmpty())
        {
            const WYamlNodeBase & nodeBase = reader.node();

            int duration = WControllerPlaylist::vbmlDuration(nodeBase,
                                                             nodeBase.extractMsecs("at"), -1);

            applySource(nodeBase, node->value, duration);
        }
        else
        {
            const WYamlNodeBase & nodeBase = reader.node();

            int start = nodeBase.extractMsecs("at");

            int duration = WControllerPlaylist::vbmlDuration(nodeBase, start, -1);

            // NOTE: If the duration is invalid we assume that the track is long enough.
            if (duration != -1 && currentTime >= duration)
            {
                currentTime -= duration;

                return;
            }

            currentTime += start;

            extractSource(node->children);
        }
    }
    else
    {
        origin = extractRelated(reader.node());

        if (origin.isEmpty() == false)
        {
            type = WControllerPlaylist::Related;
        }
    }
}

void WControllerPlaylistData::applyHtml(const QByteArray & array, const QString & url)
{
    // NOTE: When we get redirected to a VBML url we load it as VBML content.
    if (WControllerPlaylist::urlIsVbml(url))
    {
        type = WControllerPlaylist::Redirect;

        origin = url;

        return;
    }
    // NOTE: If we find a VMBL header we prioritize it over HTML.
    else if (WControllerPlaylist::vbmlCheck(array))
    {
        applyVbml(array, url, url);

        return;
    }

    QString content = extractHtml(array);

    QString head = WControllerNetwork::extractHead(content);

    head.replace("'", "\"");

    QString baseUrl = WControllerNetwork::extractBaseUrl(url);

    QString urlName = WControllerNetwork::urlName(baseUrl);

    QString host;
    QString title;

    if (WControllerNetwork::urlIsFile(baseUrl))
    {
        host = baseUrl;

        title = WControllerNetwork::extractUrlFileName(url);
    }
    else
    {
        host = WControllerNetwork::extractUrlHost(baseUrl);

        title = WControllerNetwork::extractTitle(head);

        if (title.isEmpty())
        {
            title = WControllerNetwork::removeUrlPrefix(baseUrl);
        }
    }

    QString cover = WControllerNetwork::extractImage(head);

    if (cover.isEmpty() == false)
    {
        cover = generateUrl(cover, host);
    }

    this->title = title;
    this->cover = cover;

    //---------------------------------------------------------------------------------------------
    // NOTE: If the head contains a VBML link we prioritize it over HTML.

    QStringList list = Sk::slices(head, "<link", "/>");

    foreach (const QString & string, list)
    {
        QString rel = WControllerNetwork::extractAttribute(string, "rel");

        if (rel.toLower() != "vbml") continue;

        QString url = WControllerNetwork::extractAttributeUtf8(string, "href");

        if (url.isEmpty()) continue;

        type = WControllerPlaylist::Redirect;

        origin = generateUrl(url, host);

        return;
    }

    //---------------------------------------------------------------------------------------------

    QStringList urls;

    list = Sk::slices(content, "<a", "</a");

    foreach (const QString & string, list)
    {
        QString tag = Sk::sliceIn(string, "<", ">");

        WControllerNetwork::fixAttributes(tag);

        QString url = WControllerNetwork::extractAttributeUtf8(tag, "href");

        if (url.isEmpty()) continue;

        url = generateUrl(url, host);

        if (addUrl(&urls, url))
        {
            addSource(url, generateTitle(url, urlName));
        }
    }

    list = Sk::slicesIn(content, "<", ">");

    foreach (QString string, list)
    {
        WControllerNetwork::fixAttributes(string);

        string.replace("href=\"", "src=\"");

        QStringList sources = Sk::slicesIn(string, "src=\"", "\"");

        foreach (const QString & source, sources)
        {
            QString url = generateUrl(source, host);

            if (addUrl(&urls, url))
            {
                addSource(url, generateTitle(url, urlName));
            }
        }
    }

    content.replace("'",      "\"");
    content.replace("&quot;", "\"");

    content.replace(">",  ">\"");
    content.replace("</", "\"</");

    foreach (const WControllerPlaylistSlice & slice, slices)
    {
        QString start = slice.start;
        QString end   = slice.end;

        list = Sk::slicesIn(content, '"' + start, end + '"');

        foreach (const QString & string, list)
        {
            QString url = generateUrl(start + string + end, host);

            if (url.contains(' ')) continue;

            if (addUrl(&urls, url))
            {
                addSource(url, generateTitle(url, urlName));
            }
        }
    }

    // NOTE: When sources are empty we treat the content as a plain file and extract urls.
    if (sources.isEmpty() == false) return;

    list = Sk::slices(array,
                      WRegExp("file://|http://|https://|vbml:", Qt::CaseInsensitive),
                      WRegExp("\\s"));

    WRegExp regExp("[\\s\\.:,;'\"\\)}\\]]");

    foreach (QString url, list)
    {
        for (int i = url.length() - 1; i; i--)
        {
            QChar character = url.at(i);

            if (regExp.indexIn(character) == -1) break;

            url.chop(1);
        }

        if (addUrl(&urls, url))
        {
            addSource(url, generateTitle(url, urlName));
        }
    }

    // NOTE: When the sources are empty we add the url itself given it could be a media.
    if (sources.isEmpty()) addMedia(url, title);
}

void WControllerPlaylistData::applyM3u(const QByteArray & array, const QString & url)
{
    QString content;

    QString baseUrl = WControllerNetwork::extractBaseUrl(url);

    QString urlName = WControllerNetwork::urlName(baseUrl);

    QString extension = WControllerNetwork::extractUrlExtension(url);

    if (extension == "m3u8")
    {
        content = Sk::readUtf8(array);
    }
    else content = array;

    // NOTE: If it's a stream we return the base url itself.
    if (content.contains("#EXT-X-STREAM-INF"))
    {
        addSource(url, generateTitle(url, urlName));

        return;
    }

    QStringList list = Sk::slicesIn(content, WRegExp("EXTINF"), WRegExp("[#$]"));

    foreach (const QString & media, list)
    {
        int index = media.lastIndexOf("http");

        // NOTE: We only support http(s) urls.
        if (index == -1) continue;

        QString source = media.mid(index);

        if (source.endsWith('\n')) source.chop(1);

        if (source.isEmpty()) continue;

        WTrack track(source);

        track.setTitle(Sk::sliceIn(media, "tvg-name=\"", "\""));
        track.setCover(Sk::sliceIn(media, "tvg-logo=\"", "\""));

        tracks.append(track);
    }

    // NOTE: We couldn't parse anything so we return the base url itself.
    if (tracks.isEmpty())
    {
        addSource(url, generateTitle(url, urlName));
    }
}

void WControllerPlaylistData::applyFolder(const QString & url)
{
    QDir dir(WControllerFile::filePath(url));

    QFileInfoList list = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files  | QDir::AllDirs
                                                                | QDir::System | QDir::Hidden,
                                           QDir::Name | QDir::Type);

    foreach (QFileInfo info, list)
    {
        addFile(info.absoluteFilePath());
    }

    title = dir.dirName();
}

void WControllerPlaylistData::applyFile(const QByteArray & array, const QString & url)
{
    QString baseUrl = WControllerNetwork::extractBaseUrl(url);

    QString urlName = WControllerNetwork::urlName(baseUrl);

    QStringList urls;

    QStringList list = Sk::slices(array,
                                  WRegExp("file://|http://|https://|vbml:", Qt::CaseInsensitive),
                                  WRegExp("\\s"));

    WRegExp regExp("[\\s\\.:,;'\"\\)}\\]]");

    foreach (QString url, list)
    {
        for (int i = url.length() - 1; i; i--)
        {
            QChar character = url.at(i);

            if (regExp.indexIn(character) == -1) break;

            url.chop(1);
        }

        if (addUrl(&urls, url))
        {
            addSource(url, generateTitle(url, urlName));
        }
    }

    title = WControllerNetwork::extractUrlFileName(url);
}

//-------------------------------------------------------------------------------------------------

void WControllerPlaylistData::addSlice(const QString & start, const QString & end)
{
    slices.append(WControllerPlaylistSlice(start, end));
}

//-------------------------------------------------------------------------------------------------
// Static functions
//-------------------------------------------------------------------------------------------------

/* static */ QString WControllerPlaylistData::extractHtml(const QByteArray & array)
{
    QString charset = WControllerNetwork::extractCharset(array);

    if (charset.isEmpty())
    {
         return Sk::readUtf8(array);
    }
    else return Sk::readCodec(array, charset);
}

/* static */ QString WControllerPlaylistData::extractHtmlLink(const QByteArray & array,
                                                              const QString    & url)
{
    QString content = extractHtml(array);

    QString head = WControllerNetwork::extractHead(content);

    head.replace("'", "\"");

    QString baseUrl = WControllerNetwork::extractBaseUrl(url);

    QString host;

    if (WControllerNetwork::urlIsFile(baseUrl))
    {
         host = baseUrl;
    }
    else host = WControllerNetwork::extractUrlHost(baseUrl);

    QStringList list = Sk::slices(head, "<link", "/>");

    foreach (const QString & string, list)
    {
        QString rel = WControllerNetwork::extractAttribute(string, "rel");

        if (rel.toLower() != "vbml") continue;

        QString url = WControllerNetwork::extractAttributeUtf8(string, "href");

        if (url.isEmpty()) continue;

        return generateUrl(url, host);
    }

    return QString();
}

/* static */ QString WControllerPlaylistData::generateUrl(const QString & url,
                                                          const QString & baseUrl)
{
    QString result = url.simplified().remove(' ');

    result = WControllerNetwork::removeUrlFragment(result);

    result = WControllerNetwork::decodeUrl(result);

    result = WControllerNetwork::htmlToUtf8(result);

    result.remove("\\");

    return WControllerNetwork::generateUrl(result, baseUrl);
}

/* static */ QString WControllerPlaylistData::generateTitle(const QString & url,
                                                            const QString & urlName)
{
    if (WControllerNetwork::urlName(url) == urlName)
    {
         return WControllerNetwork::extractUrlPath(url);
    }
    else return WControllerNetwork::removeUrlPrefix(url);
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WControllerPlaylistData::parseTrack(WYamlReader & reader, const QString & type,
                                                               const QString & url)
{
    title = reader.extractString("title");
    cover = reader.extractString("cover");

    WTrack::Type typeTrack = WTrack::typeFromString(type);

    int at;
    int duration;

    if (typeTrack == WTrack::Channel)
    {
        at = 0;

        duration = CONTROLLERPLAYLIST_CHANNEL_DURATION;
    }
    else
    {
        at = reader.extractMsecs("at");

        duration = WControllerPlaylist::vbmlDuration(reader.node(), at, -1);
    }

    WTrack track;

    track.setType(typeTrack);

    track.setState(WTrack::Default);

    // NOTE: The origin is prioritized over the source.
    if (origin.isEmpty())
    {
        const WYamlNode * node = reader.at("source");

        if (node)
        {
            if (node->children.isEmpty())
            {
                source = node->value;

                // NOTE: When the source is empty we set the vbml uri.
                if (source.isEmpty()) track.setSource(url);
                else                  track.setSource(source);
            }
            else
            {
                // NOTE: When it's a multi-track we set the vbml uri.
                track.setSource(url);

                if (duration == -1)
                {
                    duration = WControllerPlaylist::vbmlDurationSource(*node, at, -1);
                }
            }
        }
    }
    else track.setSource(origin);

    track.setTitle(title);
    track.setCover(cover);

    track.setAuthor(reader.extractString("author"));
    track.setFeed  (reader.extractString("feed"));

    track.setDuration(duration);

    track.setDate(reader.extractDate("date"));

    tracks.append(track);
}

void WControllerPlaylistData::parsePlaylist(WYamlReader & reader)
{
    title = reader.extractString("title");
    cover = reader.extractString("cover");

    const WYamlNode * node = reader.at("tracks");

    if (node == NULL) return;

    const QList<WYamlNode> & children = node->children;

    if (children.isEmpty())
    {
        QStringList list = node->value.split('\n');

        foreach (const QString & string, list)
        {
            WTrack track(string.trimmed(), WTrack::Default);

            tracks.append(track);
        }
    }
    else
    {
        foreach (const WYamlNode & child, children)
        {
            QString key = child.key;

            if      (key == "track")   parsePlaylistTrack(child, WTrack::Track);
            else if (key == "live")    parsePlaylistTrack(child, WTrack::Live);
            else if (key == "hub")     parsePlaylistTrack(child, WTrack::Hub);
            else if (key == "channel") parsePlaylistTrack(child, WTrack::Channel);
        }
    }
}

void WControllerPlaylistData::parsePlaylistTrack(const WYamlNode & node, WTrack::Type type)
{
    WTrack track;

    track.setType(type);

    QString source = node.extractString("source");

    if (source.isEmpty() == false)
    {
        track.setState(WTrack::Default);

        track.setSource(source);
    }

    track.setTitle(node.extractString("title"));
    track.setCover(node.extractString("cover"));

    track.setAuthor(node.extractString("author"));
    track.setFeed  (node.extractString("feed"));

    track.setDuration(node.extractInt("duration", -1));

    track.setDate(node.extractDate("date"));

    tracks.append(track);
}

//-------------------------------------------------------------------------------------------------

void WControllerPlaylistData::addSource(const QString & url, const QString & title)
{
    if (WControllerPlaylist::urlIsMedia(url))
    {
        WControllerPlaylistMedia media;

        media.url   = url;
        media.title = WControllerNetwork::extractUrlFileName(url);

        medias.append(media);
    }

    WControllerPlaylistSource source;

    source.url   = url;
    source.title = title;

    sources.append(source);
}

void WControllerPlaylistData::addMedia(const QString & url, const QString & title)
{
    WControllerPlaylistMedia media;

    media.url   = url;
    media.title = title;

    if (WControllerFile::urlIsImage(url))
    {
        media.cover = url;
    }

    medias.append(media);
}

void WControllerPlaylistData::addFile(const QString & path)
{
    QString extension = WControllerNetwork::extractUrlExtension(path);

    if (WControllerPlaylist::extensionIsMedia(extension))
    {
        WControllerPlaylistMedia media;

        media.url   = WControllerFile::fileUrl(path);
        media.title = WControllerNetwork::extractUrlFileName(path);

        medias.append(media);
    }
    else
    {
        WControllerPlaylistSource source;

        source.url   = WControllerFile::fileUrl(path);
        source.title = WControllerNetwork::extractUrlFileName(path);

        if (WControllerPlaylist::extensionIsAscii(extension))
        {
            files.append(source);
        }
        else sources.append(source);
    }
}

bool WControllerPlaylistData::addUrl(QStringList * urls, const QString & url) const
{
    if (WControllerNetwork::urlIsHttp(url))
    {
        QString source = WControllerNetwork::removeUrlPrefix(url);

        if (urls->contains(source) == false)
        {
            urls->append(source);

            return true;
        }
        else return false;
    }
    else if (urls->contains(url) == false)
    {
        urls->append(url);

        return true;
    }
    else return false;
}

//-------------------------------------------------------------------------------------------------

void WControllerPlaylistData::extractSource(const QList<WYamlNode> & children)
{
    foreach (const WYamlNode & child, children)
    {
        int durationSource = WControllerPlaylist::vbmlDuration(child, child.extractMsecs("at"));

        if (currentTime >= durationSource)
        {
            currentTime -= durationSource;

            continue;
        }

        const WYamlNode * node = child.at("source");

        if (node == NULL)
        {
            QString related = extractRelated(child);

            if (related.isEmpty() == false)
            {
                type = WControllerPlaylist::Related;

                origin = related;
            }

            return;
        }

        const QList<WYamlNode> & nodes = node->children;

        if (nodes.isEmpty())
        {
            // NOTE: The related is prioritized over the source.
            QString related = extractRelated(child);

            if (related.isEmpty() == false)
            {
                type = WControllerPlaylist::Related;

                origin = related;

                return;
            }

            type = WControllerPlaylist::Source;

            origin = node->value;
        }
        else extractSource(nodes);

        return;
    }
}

void WControllerPlaylistData::applySource(const WYamlNodeBase & node,
                                          const QString       & url, int duration)
{
    // NOTE: If the duration is invalid we assume that the track is long enough.
    if (duration != -1 && currentTime >= duration)
    {
        currentTime -= duration;

        return;
    }

    // NOTE: The related is prioritized over the source.
    QString related = node.extractString("related");

    if (related.isEmpty() == false)
    {
        type = WControllerPlaylist::Redirect;

        origin = related;

        return;
    }

    if (url.isEmpty()) return;

    type = WControllerPlaylist::Source;

    origin = url;
}

QString WControllerPlaylistData::extractRelated(const WYamlNodeBase & node)
{
    QString related = node.extractString("related");

    if (related.isEmpty())
    {
        return node.extractString("feed");
    }
    else return related;
}

//=================================================================================================
// WControllerPlaylistReply
//=================================================================================================

class WControllerPlaylistReply : public QObject
{
    Q_OBJECT

public: // Interface
    Q_INVOKABLE void extractVbml(QIODevice * device, const QString & url, const QString & urlBase);

    Q_INVOKABLE void extractRelated(QIODevice * device, const QString & url,
                                                        const QString & urlBase, int currentTime);

    Q_INVOKABLE void extractHtml  (QIODevice * device, const QString & url);
    Q_INVOKABLE void extractM3u   (QIODevice * device, const QString & url);
    Q_INVOKABLE void extractFolder(QIODevice * device, const QString & url);
    Q_INVOKABLE void extractFile  (QIODevice * device, const QString & url);
    Q_INVOKABLE void extractItem  (QIODevice * device, const QString & url);

signals:
    void loaded    (QIODevice * device, const WControllerPlaylistData & data);
    void loadedItem(QIODevice * device, const WControllerPlaylistItem & item);
};

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WControllerPlaylistReply::extractVbml(QIODevice     * device,
                                                             const QString & url,
                                                             const QString & urlBase)
{
    WControllerPlaylistData data;

    data.applyVbml(WControllerFile::readAll(device), url, urlBase);

    emit loaded(device, data);

    deleteLater();
}

/* Q_INVOKABLE */ void WControllerPlaylistReply::extractRelated(QIODevice     * device,
                                                                const QString & url,
                                                                const QString & urlBase,
                                                                int             currentTime)
{
    WControllerPlaylistData data;

    data.currentTime = currentTime;

    data.applyRelated(WControllerFile::readAll(device), url, urlBase);

    emit loaded(device, data);

    deleteLater();
}

/* Q_INVOKABLE */ void WControllerPlaylistReply::extractHtml(QIODevice     * device,
                                                             const QString & url)
{
    WControllerPlaylistData data;

    data.addSlice("http");

    data.applyHtml(WControllerFile::readAll(device), url);

    emit loaded(device, data);

    deleteLater();
}

/* Q_INVOKABLE */ void WControllerPlaylistReply::extractM3u(QIODevice     * device,
                                                            const QString & url)
{
    WControllerPlaylistData data;

    data.applyM3u(WControllerFile::readAll(device), url);

    emit loaded(device, data);

    deleteLater();
}

/* Q_INVOKABLE */ void WControllerPlaylistReply::extractFolder(QIODevice     * device,
                                                               const QString & url)
{
    WControllerPlaylistData data;

    data.applyFolder(url);

    emit loaded(device, data);

    deleteLater();
}

/* Q_INVOKABLE */ void WControllerPlaylistReply::extractFile(QIODevice     * device,
                                                             const QString & url)
{
    WControllerPlaylistData data;

    data.applyFile(WControllerFile::readAll(device), url);

    emit loaded(device, data);

    deleteLater();
}

/* Q_INVOKABLE */ void WControllerPlaylistReply::extractItem(QIODevice     * device,
                                                             const QString & url)
{
    WControllerPlaylistItem item;

    item.data = WControllerFile::readAll(device);

    item.extension = WControllerNetwork::extractUrlExtension(url);

    emit loadedItem(device, item);

    deleteLater();
}

//=================================================================================================
// WControllerPlaylistPrivate
//=================================================================================================

WControllerPlaylistPrivate::WControllerPlaylistPrivate(WControllerPlaylist * p)
    : WControllerPrivate(p) {}

/* virtual */ WControllerPlaylistPrivate::~WControllerPlaylistPrivate()
{
    foreach (WBackendLoader * loader, backendLoaders)
    {
        loader->deleteLater();
    }

    thread->quit();
    thread->wait();

    W_CLEAR_CONTROLLER(WControllerPlaylist);
}

//-------------------------------------------------------------------------------------------------

void WControllerPlaylistPrivate::init()
{
    Q_Q(WControllerPlaylist);

    qRegisterMetaType<WNetReplySource   *>("WNetReplySource *");
    qRegisterMetaType<WNetReplyTrack    *>("WNetReplyTrack *");
    qRegisterMetaType<WNetReplyPlaylist *>("WNetReplyPlaylist *");
    qRegisterMetaType<WNetReplyFolder   *>("WNetReplyFolder *");
    qRegisterMetaType<WNetReplyItem     *>("WNetReplyItem *");

    qRegisterMetaType<WBackendNetSource  >("WBackendNetSource");
    qRegisterMetaType<WBackendNetTrack   >("WBackendNetTrack");
    qRegisterMetaType<WBackendNetPlaylist>("WBackendNetPlaylist");
    qRegisterMetaType<WBackendNetFolder  >("WBackendNetFolder");
    qRegisterMetaType<WBackendNetItem    >("WBackendNetItem");

    qRegisterMetaType<WControllerPlaylistData>("WControllerPlaylistData");
    qRegisterMetaType<WControllerPlaylistItem>("WControllerPlaylistItem");

    const QMetaObject * meta = WControllerPlaylistReply().metaObject();

    methodVbml = meta->method(meta->indexOfMethod("extractVbml(QIODevice*,QString,QString)"));

    methodRelated
        = meta->method(meta->indexOfMethod("extractRelated(QIODevice*,QString,QString,int)"));

    methodHtml   = meta->method(meta->indexOfMethod("extractHtml(QIODevice*,QString)"));
    methodM3u    = meta->method(meta->indexOfMethod("extractM3u(QIODevice*,QString)"));
    methodFolder = meta->method(meta->indexOfMethod("extractFolder(QIODevice*,QString)"));
    methodFile   = meta->method(meta->indexOfMethod("extractFile(QIODevice*,QString)"));
    methodItem   = meta->method(meta->indexOfMethod("extractItem(QIODevice*,QString)"));

    thread = new QThread(q);

    thread->start();

    QObject::connect(wControllerFile, SIGNAL(pathStorageChanged()),
                     q,               SIGNAL(pathStorageChanged()));
}

//-------------------------------------------------------------------------------------------------
// Private interface
//-------------------------------------------------------------------------------------------------

bool WControllerPlaylistPrivate::applyQueryTrack(const QString & backendId,
                                                 WPlaylist * playlist, WTrack * track,
                                                 WBackendNetQuery * query)
{
    abortQueryTrack(track);

    if (resolveTrack(backendId, *query))
    {
        getDataTrack(playlist, track, *query);

        return true;
    }
    else return false;
}

bool WControllerPlaylistPrivate::applyQueryPlaylist(WPlaylist              * playlist,
                                                    const WBackendNetQuery & query)
{
    playlist->abortQuery();

    if (query.isValid())
    {
        getDataPlaylist(playlist, query);

        return true;
    }
    else return false;
}

bool WControllerPlaylistPrivate::applyQueryFolder(WLibraryFolder         * folder,
                                                  const WBackendNetQuery & query)
{
    folder->abortQuery();

    if (query.isValid())
    {
        getDataFolder(folder, query);

        return true;
    }
    else return false;
}

bool WControllerPlaylistPrivate::applyQueryItem(WLibraryItem           * item,
                                                const WBackendNetQuery & query)
{
    item->abortQuery();

    if (query.isValid())
    {
        getDataItem(item, query);

        return true;
    }
    else return false;
}

//-------------------------------------------------------------------------------------------------

bool WControllerPlaylistPrivate::applySourceTrack(WPlaylist     * playlist,
                                                  WTrack        * track,
                                                  const QString & url, int index)
{
    if (url.isEmpty()) return true;

    Q_Q(WControllerPlaylist);

    QString source = WControllerPlaylist::generateSource(url);

    // NOTE: The VBML uri is prioritized because it might contain an http url.
    if (WControllerPlaylist::urlIsVbmlUri(source))
    {
        if (source.startsWith("vbml://"))
        {
            source.replace("vbml://", "https://");

            track->setSource(source);

            playlist->updateTrack(track);

            return applySourceTrack(playlist, track, source, index);
        }

        WBackendNetQuery query(source, index);

        query.type   = WBackendNetQuery::TypeVbml;
        query.target = WBackendNetQuery::TargetVbml;

        getDataTrack(playlist, track, query);

        return true;
    }

    WBackendNet * backend = q->backendFromUrl(source);

    if (backend)
    {
        QString backendId = backend->id();

        if (WBackendNet::checkQuery(source))
        {
            WBackendNetQuery query = extractQuery(backend, source, backendId);

            backend->tryDelete();

            if (resolveTrack(backendId, query))
            {
                query.indexNext = index;

                // NOTE: The custom query priority should be high because it's often tied to the
                //       current action.
                query.priority = QNetworkRequest::HighPriority;

                getDataTrack(playlist, track, query);

                return true;
            }
            else return false;
        }

        WBackendNetQuery query = backend->getQueryTrack(source);

        backend->tryDelete();

        if (resolveTrack(backendId, query))
        {
            query.indexNext = index;

            getDataTrack(playlist, track, query);

            return true;
        }
    }

    if (WControllerPlaylist::urlIsVbmlFile(source))
    {
        WBackendNetQuery query(source, index);

        query.target = WBackendNetQuery::TargetVbml;

        getDataTrack(playlist, track, query);

        return true;
    }
    else if (WControllerFile::urlIsImage(source))
    {
        WBackendNetQuery query(source, index);

        query.type   = WBackendNetQuery::TypeImage;
        query.target = WBackendNetQuery::TargetVbml;

        getDataTrack(playlist, track, query);

        return true;
    }
    else if (q->urlIsMedia(source))
    {
        track->setState(WTrack::Loaded);

        track->setSource(source);

        // NOTE: We are only applying the fileName when the title is empty.
        if (track->title().isEmpty())
        {
            QString title = WControllerNetwork::extractUrlFileName(source);

            track->setTitle(title);
        }

        playlist->updateTrack(track);

        return true;
    }

    WBackendNetQuery query(source, index);

    query.target = WBackendNetQuery::TargetHtml;
    query.scope  = WAbstractLoader::ScopeText;

    getDataTrack(playlist, track, query);

    return true;
}

bool WControllerPlaylistPrivate::applySourcePlaylist(WPlaylist     * playlist,
                                                     const QString & url,
                                                     const QString & urlBase, int index)
{
    if (url.isEmpty()) return true;

    Q_Q(WControllerPlaylist);

    QString source = WControllerPlaylist::generateSource(url);

    if (WBackendNet::checkQuery(source))
    {
        WBackendNet * backend = q->backendFromUrl(source);

        if (backend)
        {
            QString backendId = backend->id();

            WBackendNetQuery query = extractQuery(backend, source, backendId);

            backend->tryDelete();

            if (resolvePlaylist(backendId, query) == false) return false;

            query.urlBase   = urlBase;
            query.indexNext = index;

            // NOTE: The custom query priority should be high because it's often tied to the
            //       current action.
            query.priority = QNetworkRequest::HighPriority;

            getDataPlaylist(playlist, query);

            return true;
        }

        WBackendNetQuery query = extractRelated(source);

        if (query.isValid() == false) return false;

        query.urlBase   = urlBase;
        query.indexNext = index;

        getDataPlaylist(playlist, query);

        return true;
    }
    // NOTE: The VBML uri is prioritized because it might contain an http url.
    else if (WControllerPlaylist::urlIsVbmlUri(source))
    {
        if (source.startsWith("vbml://"))
        {
            source.replace("vbml://", "https://");

            playlist->applySource(source);

            return applySourcePlaylist(playlist, source, urlBase, index);
        }

        WBackendNetQuery query(source, urlBase, index);

        query.type   = WBackendNetQuery::TypeVbml;
        query.target = WBackendNetQuery::TargetVbml;

        getDataPlaylist(playlist, query);

        return true;
    }

    WBackendNet * backend = q->backendFromUrl(source);

    if (backend)
    {
        QString backendId = backend->id();

        WBackendNetQuery query = backend->getQueryPlaylist(source);

        if (resolvePlaylist(backendId, query))
        {
            backend->tryDelete();

            query.urlBase   = urlBase;
            query.indexNext = index;

            getDataPlaylist(playlist, query);

            return true;
        }

        QString id = backend->getTrackId(source);

        if (id.isEmpty() == false)
        {
            playlist->addSource(backend->getUrlTrack(id), true);

            if (getDataRelated(backend, playlist, id) == false)
            {
                backend->tryDelete();

                WBackendNetQuery query(source, urlBase, index);

                query.target = WBackendNetQuery::TargetHtml;

                getDataPlaylist(playlist, query);
            }
            else backend->tryDelete();

            return true;
        }

        backend->tryDelete();
    }

    if (WControllerPlaylist::urlIsVbmlFile(source))
    {
        WBackendNetQuery query(source, urlBase, index);

        query.target = WBackendNetQuery::TargetVbml;

        getDataPlaylist(playlist, query);

        return true;
    }
    else if (WControllerFile::urlIsImage(source))
    {
        WBackendNetQuery query(source, urlBase, index);

        query.type   = WBackendNetQuery::TypeImage;
        query.target = WBackendNetQuery::TargetVbml;

        getDataPlaylist(playlist, query);

        return true;
    }
    else if (WControllerPlaylist::urlIsM3u(source))
    {
        WBackendNetQuery query(source, urlBase, index);

        query.target = WBackendNetQuery::TargetM3u;

        getDataPlaylist(playlist, query);

        return true;
    }
    else if (WControllerNetwork::urlIsFile(source))
    {
        QFileInfo info(WControllerFile::filePath(source));

        if (info.isSymLink())
        {
            source = WControllerFile::fileUrl(info.symLinkTarget());
        }

        if (info.isDir())
        {
            WBackendNetQuery query(source, urlBase, index);

            query.target = WBackendNetQuery::TargetFolder;

            getDataPlaylist(playlist, query);

            return true;
        }
        else if (info.isFile())
        {
            QString extension = info.suffix().toLower();

            if (q->extensionIsMarkup(extension) == false)
            {
                if (q->extensionIsMedia(extension))
                {
                    source = WControllerNetwork::extractBaseUrl(source);

                    WBackendNetQuery query(source, urlBase, index);

                    query.target = WBackendNetQuery::TargetFolder;

                    getDataPlaylist(playlist, query);

                    return true;
                }
                else if (info.size() < CONTROLLERPLAYLIST_MAX_SIZE)
                {
                    WBackendNetQuery query(source, urlBase, index);

                    query.target = WBackendNetQuery::TargetFile;

                    getDataPlaylist(playlist, query);

                    return true;
                }
                else return false;
            }
        }
        else return false;
    }
    else if (q->urlIsMedia(source))
    {
        QString title = WControllerNetwork::extractBaseUrl(source);

        title = WControllerNetwork::removeUrlPrefix(title);

        WTrack track(source, WTrack::Default);

        track.setFeed(source);

        playlist->setTitle(title);

        playlist->addTrack(track);

        playlist->d_func()->setQueryFinished();

        return true;
    }

    WBackendNetQuery query(source, urlBase, index);

    query.target = WBackendNetQuery::TargetHtml;
    query.scope  = WAbstractLoader::ScopeText;

    getDataPlaylist(playlist, query);

    return true;
}

bool WControllerPlaylistPrivate::applySourceFolder(WLibraryFolder * folder,
                                                   const QString  & url,
                                                   const QString  & urlBase, int index)
{
    if (url.isEmpty()) return true;

    Q_Q(WControllerPlaylist);

    QString source = WControllerPlaylist::generateSource(url);

    if (WBackendNet::checkQuery(source))
    {
#ifdef QT_4
        QString method = QUrl(source).queryItemValue("method");
#else
        QUrlQuery urlQuery(source);

        QString method = urlQuery.queryItemValue("method");
#endif

        // NOTE: The view and related methods only works for playlists.
        if (method != "view" && method != "related")
        {
            WBackendNet * backend = q->backendFromUrl(source);

            if (backend)
            {
                QString backendId = backend->id();

                if (method.isEmpty())
                {
                    folder->addItems(backend->getLibraryItems());

                    backend->tryDelete();

                    folder->setCurrentIndex(0);

                    folder->d_func()->setQueryFinished();

                    return true;
                }

                WBackendNetQuery query = extractQuery(backend, source, backendId);

                backend->tryDelete();

                if (resolveFolder(backendId, query) == false) return false;

                query.urlBase   = urlBase;
                query.indexNext = index;

                // NOTE: The custom query priority should be high because it's often tied to the
                //       current action.
                query.priority = QNetworkRequest::HighPriority;

                getDataFolder(folder, query);

                return true;
            }
        }

        WLibraryFolderItem item(WLibraryItem::Playlist, WLocalObject::Default);

        item.source = source;

        folder->addItem(item);

        folder->setCurrentIndex(0);

        folder->d_func()->setQueryFinished();

        return true;
    }
    // NOTE: The VBML uri is prioritized because it might contain an http url.
    else if (WControllerPlaylist::urlIsVbmlUri(source))
    {
        if (source.startsWith("vbml://"))
        {
            source.replace("vbml://", "https://");

            folder->applySource(source);

            return applySourceFolder(folder, source, urlBase, index);
        }

        addFolderSearch(folder, source, WControllerNetwork::urlName(source));

        WBackendNetQuery query(source, urlBase, index);

        query.type   = WBackendNetQuery::TypeVbml;
        query.target = WBackendNetQuery::TargetVbml;

        getDataFolder(folder, query);

        return true;
    }

    WBackendNet * backend = q->backendFromUrl(source);

    if (backend)
    {
        QString backendId = backend->id();

        WBackendNetQuery query = backend->getQueryFolder(source);

        if (resolveFolder(backendId, query))
        {
            backend->tryDelete();

            query.urlBase   = urlBase;
            query.indexNext = index;

            getDataFolder(folder, query);

            return true;
        }

        if (applyUrl(folder, backend, source))
        {
            backend->tryDelete();

            folder->d_func()->setQueryFinished();

            if (WControllerNetwork::urlIsHttp(source))
            {
                WBackendNetQuery query(source, urlBase, index);

                query.target = WBackendNetQuery::TargetHtml;
                query.id     = 1;

                getDataFolder(folder, query);
            }

            return true;
        }

        backend->tryDelete();

        addFolderSearch(folder, source, WControllerNetwork::urlName(source));

        if (WControllerNetwork::urlIsHttp(source) == false)
        {
            return true;
        }
    }

    if (WControllerPlaylist::urlIsVbmlFile(source))
    {
        addFolderSearch(folder, source, WControllerNetwork::urlName(source));

        WBackendNetQuery query(source, urlBase, index);

        query.target = WBackendNetQuery::TargetVbml;

        getDataFolder(folder, query);

        return true;
    }
    else if (WControllerFile::urlIsImage(source))
    {
        addFolderSearch(folder, source, WControllerNetwork::urlName(source));

        WBackendNetQuery query(source, urlBase, index);

        query.type   = WBackendNetQuery::TypeImage;
        query.target = WBackendNetQuery::TargetVbml;

        getDataFolder(folder, query);

        return true;
    }
    else if (WControllerPlaylist::urlIsM3u(source))
    {
        addFolderSearch(folder, source, WControllerNetwork::urlName(source));

        WBackendNetQuery query(source, urlBase, index);

        query.target = WBackendNetQuery::TargetM3u;

        getDataFolder(folder, query);

        return true;
    }
    else if (WControllerNetwork::urlIsFile(source))
    {
        QFileInfo info(WControllerFile::filePath(source));

        if (info.isSymLink())
        {
            source = WControllerFile::fileUrl(info.symLinkTarget());
        }

        if (info.isFile())
        {
            QString baseUrl = info.absolutePath();

            addFolderSearch(folder, source, baseUrl);

            QString extension = info.suffix().toLower();

            if (q->extensionIsMarkup(extension) == false)
            {
                if (q->extensionIsMedia(extension))
                {
                    source = WControllerNetwork::extractBaseUrl(source);

                    WBackendNetQuery query(source, urlBase, index);

                    query.target = WBackendNetQuery::TargetFolder;

                    getDataFolder(folder, query);

                    return true;
                }
                else if (info.size() < CONTROLLERPLAYLIST_MAX_SIZE)
                {
                    WBackendNetQuery query(source, urlBase, index);

                    query.target = WBackendNetQuery::TargetFile;

                    getDataFolder(folder, query);

                    return true;
                }
                else return false;
            }

            if (q->extensionIsMarkup(extension) == false
                &&
                info.size() < CONTROLLERPLAYLIST_MAX_SIZE)
            {
                WBackendNetQuery query(source, urlBase, index);

                query.target = WBackendNetQuery::TargetFile;

                getDataFolder(folder, query);

                return true;
            }
        }
        else
        {
            addFolderSearch(folder, source, info.absoluteFilePath());

            WBackendNetQuery query(source, urlBase, index);

            query.target = WBackendNetQuery::TargetFolder;

            getDataFolder(folder, query);

            return true;
        }
    }
    else if (q->urlIsMedia(source))
    {
        WLibraryFolderItem item(WLibraryItem::Playlist, WLocalObject::Default);

        item.source = source;

        folder->addItem(item);

        folder->d_func()->setQueryFinished();

        return true;
    }
    else addFolderSearch(folder, source, WControllerNetwork::urlName(source));

    WBackendNetQuery query(source, urlBase, index);

    query.target = WBackendNetQuery::TargetHtml;
    query.scope  = WAbstractLoader::ScopeText;

    getDataFolder(folder, query);

    return true;
}

bool WControllerPlaylistPrivate::applySourceItem(WLibraryItem  * item,
                                                 const QString & url, int index)
{
    abortQueriesItem(item);

    if (url.isEmpty()) return true;

    Q_Q(WControllerPlaylist);

    QString source = WControllerPlaylist::generateSource(url);

    WBackendNet * backend = q->backendFromUrl(source);

    if (backend)
    {
        QString backendId = backend->id();

        if (WBackendNet::checkQuery(source))
        {
            WBackendNetQuery query = extractQuery(backend, source, backendId);

            backend->tryDelete();

            if (resolveItem(backendId, query))
            {
                query.indexNext = index;

                // NOTE: The custom query priority should be high because it's often tied to the
                //       current action.
                query.priority = QNetworkRequest::HighPriority;

                getDataItem(item, query);

                return true;
            }
            else return false;
        }

        WBackendNetQuery query = backend->getQueryItem(source);

        backend->tryDelete();

        if (resolveItem(backendId, query))
        {
            query.indexNext = index;

            getDataItem(item, query);

            return true;
        }
    }

    if (WControllerNetwork::urlIsFile(source))
    {
        QFileInfo info(WControllerFile::filePath(source));

        if (info.isFile() == false || info.size() >= CONTROLLERPLAYLIST_MAX_SIZE)
        {
            return false;
        }

        if (info.isSymLink())
        {
            source = WControllerFile::fileUrl(info.symLinkTarget());
        }
    }

    WBackendNetQuery query(source, index);

    query.target = WBackendNetQuery::TargetItem;

    getDataItem(item, query);

    return true;
}

bool WControllerPlaylistPrivate::applyNextTrack(WPlaylist     * playlist,
                                                WTrack        * track,
                                                const QString & url, int index)
{
    if (index >= CONTROLLERPLAYLIST_MAX_QUERY)
    {
        qWarning("WControllerPlaylistPrivate::applyNextTrack: Maximum queries reached.");

        return false;
    }
    else return applySourceTrack(playlist, track, url, index + 1);
}

bool WControllerPlaylistPrivate::applyNextPlaylist(WPlaylist     * playlist,
                                                   const QString & url,
                                                   const QString & urlBase, int index)
{
    if (index >= CONTROLLERPLAYLIST_MAX_QUERY)
    {
        qWarning("WControllerPlaylistPrivate::applyNextPlaylist: Maximum queries reached.");

        return false;
    }
    else return applySourcePlaylist(playlist, url, urlBase, index + 1);
}

bool WControllerPlaylistPrivate::applyNextFolder(WLibraryFolder * folder,
                                                 const QString  & url,
                                                 const QString  & urlBase, int index)
{
    if (index >= CONTROLLERPLAYLIST_MAX_QUERY)
    {
        qWarning("WControllerPlaylistPrivate::applyNextFolder: Maximum queries reached.");

        return false;
    }
    else return applySourceFolder(folder, url, urlBase, index + 1);
}

bool WControllerPlaylistPrivate::applyNextItem(WLibraryItem   * item,
                                               const QString  & url, int index)
{
    if (index >= CONTROLLERPLAYLIST_MAX_QUERY)
    {
        qWarning("WControllerPlaylistPrivate::applyNextItem: Maximum queries reached.");

        return false;
    }
    else return applySourceItem(item, url, index + 1);
}

//-------------------------------------------------------------------------------------------------

bool WControllerPlaylistPrivate::abortQueryTrack(WTrack * track)
{
    int count = queries.count();

    abortTrack(track);

    WPlaylist * playlist = track->playlist();

    if (playlist)
    {
        int index = playlist->indexOf(track);

        playlist->setTrackState(index, WTrack::Default);
    }

    return (count != queries.count());
}

bool WControllerPlaylistPrivate::abortQueriesTracks(WPlaylist * playlist)
{
    int count = queries.count();

    foreach (WControllerPlaylistQuery * query, queries)
    {
        if (query->type == WControllerPlaylistQuery::TypeTrack && query->item == playlist)
        {
            int index = playlist->indexOf(query->track);

            removeQuery(query);

            playlist->d_func()->applyTrackAbort(index);
        }
    }

    return (count != queries.count());
}

//-------------------------------------------------------------------------------------------------

bool WControllerPlaylistPrivate::abortQueriesPlaylist(WPlaylist * playlist)
{
    int count = queries.count();

    abortPlaylist(playlist);

    playlist->d_func()->setQueryLoading(false);

    return (count != queries.count());
}

bool WControllerPlaylistPrivate::abortQueriesFolder(WLibraryFolder * folder)
{
    int count = queries.count();

    abortFolder(folder);

    folder->d_func()->setQueryLoading(false);

    return (count != queries.count());
}

bool WControllerPlaylistPrivate::abortQueriesItem(WLibraryItem * item)
{
    int count = queries.count();

    abortItem(item);

    item->d_func()->setQueryLoading(false);

    return (count != queries.count());
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WControllerPlaylistPrivate::registerLoader(WBackendLoader * loader)
{
    backendLoaders.append(loader);
}

void WControllerPlaylistPrivate::unregisterLoader(WBackendLoader * loader)
{
    backendLoaders.removeOne(loader);
}

//-------------------------------------------------------------------------------------------------

void WControllerPlaylistPrivate::registerFolder(WLibraryFolder * folder)
{
    folders.append(folder);
}

void WControllerPlaylistPrivate::unregisterFolder(WLibraryFolder * folder)
{
    folders.removeOne(folder);
}

//-------------------------------------------------------------------------------------------------

void WControllerPlaylistPrivate::registerTab(WTabTrack * tab)
{
    tabs.append(tab);
}

void WControllerPlaylistPrivate::unregisterTab(WTabTrack * tab)
{
    tabs.removeOne(tab);
}

//-------------------------------------------------------------------------------------------------

void WControllerPlaylistPrivate::registerItemId(WLibraryItem * item)
{
    QList<int> idFull = item->idFull();

    foreach (WTabTrack * tab, tabs)
    {
        tab->d_func()->onRegisterItemId(item, idFull);
    }
}

//-------------------------------------------------------------------------------------------------

#if defined(SK_CONSOLE) == false && defined(Q_OS_MACX)

bool WControllerPlaylistPrivate::compareBundle(const CFStringRef bundle,
                                               const CFStringRef handler) const
{
    if (handler == NULL) return false;

    bool result = (CFStringCompare(bundle, handler, 0) == kCFCompareEqualTo);

    CFRelease(handler);

    return result;
}

#endif

//-------------------------------------------------------------------------------------------------

WLibraryFolder * WControllerPlaylistPrivate::getFolderRoot(int id) const
{
    foreach (WLibraryFolder * folder, folders)
    {
        if (folder->idFull().first() == id)
        {
            return folder;
        }
    }

    return NULL;
}

WLibraryFolder * WControllerPlaylistPrivate::getFolder(WLibraryFolder * folder,
                                                       QList<int>     * idFull) const
{
    idFull->removeFirst();

    if (idFull->isEmpty()) return NULL;

    int id = idFull->first();

    WLibraryItem * item = folder->getLibraryItemFromId(id);

    if (item == NULL)
    {
        return folder;
    }

    folder = item->toFolder();

    if (folder)
    {
         return getFolder(folder, idFull);
    }
    else return NULL;
}

//-------------------------------------------------------------------------------------------------

void WControllerPlaylistPrivate::removeQuery(WControllerPlaylistQuery * query)
{
    QIODevice * reply = query->reply;

    // NOTE: This means the query is already in progress.
    if (reply)
    {
        WControllerPlaylistQuery * query = replies.value(reply);

        // NOTE: We clear the item to notify we should not do anything after loading the query.
        //       This is done like this because we need to keep the WBackendNetQuery alive.
        query->item = NULL;

        return;
    }

    WRemoteData * data = query->data;

    // NOTE: We must check if the job still exists because we could be in a backendFrom*
    //       processEvents from the onLoaded() function.
    if (jobs.remove(data) == 0)
    {
        query->item = NULL;

        return;
    }

    delete data;

    deleteQuery(query);
}

void WControllerPlaylistPrivate::deleteQuery(WControllerPlaylistQuery * query)
{
    queries.removeOne(query);

    WBackendNet * backend = query->backend;

    if (backend) backend->tryDelete();

    delete query;
}

//-------------------------------------------------------------------------------------------------

void WControllerPlaylistPrivate::addFolderSearch(WLibraryFolder * folder,
                                                 const QString  & source,
                                                 const QString  & title) const
{
    WLibraryFolderItem item(WLibraryItem::Playlist);

    item.source = source;
    item.title  = title;

    folder->addItem(item);

    WLibraryItem * libraryItem = folder->createLibraryItemAt(0, true);

    libraryItem->d_func()->setQueryLoading(true);

    libraryItem->tryDelete();
}

//-------------------------------------------------------------------------------------------------

bool WControllerPlaylistPrivate::applyUrl(WLibraryFolder * folder,
                                          WBackendNet    * backend, const QString & url) const
{
    bool result = false;

    QString id = backend->getTrackId(url);

    if (id.isEmpty() == false)
    {
        WLibraryFolderItem item(WLibraryItem::Playlist, WLocalObject::Default);

        item.source = backend->getUrlTrack(id);

        folder->addItem(item);

        result = true;
    }

    WBackendNetPlaylistInfo info = backend->getPlaylistInfo(url);

    if (info.isValid())
    {
        WLibraryFolderItem item(info.type, WLocalObject::Default);

        item.source = backend->getUrlPlaylist(info);

        folder->addItem(item);

        result = true;
    }

    return result;
}

//-------------------------------------------------------------------------------------------------

void WControllerPlaylistPrivate::applyTrack(WPlaylist   * playlist,
                                            WTrack      * track,
                                            WTrack::State state, int index) const
{
    if (state > WTrack::Loaded)
    {
         track->setState(state);
    }
    else track->setState(WTrack::Loaded);

    playlist->updateTrack(index);

    WPlaylistPrivate * p = playlist->d_func();

    QString source = p->source;

    if (WControllerNetwork::removeUrlPrefix(source)
        ==
        WControllerNetwork::removeUrlPrefix(track->source()))
    {
        Q_Q(const WControllerPlaylist);

        QString title = track->title();
        QString cover = track->cover();

        // NOTE: If the source is both a track and a playlist we only update its values when they
        //       are empty.
        if (q->urlIsPlaylist(source))
        {
            if (p->title.isEmpty() && title.isEmpty() == false)
            {
                playlist->setTitle(title);
            }

            if (p->cover.isEmpty() && cover.isEmpty() == false)
            {
                playlist->setCover(cover);
            }
        }
        else
        {
            if (title.isEmpty() == false) playlist->setTitle(title);
            if (cover.isEmpty() == false) playlist->setCover(cover);
        }
    }
    else playlist->updateCover();
}

void WControllerPlaylistPrivate::applyPlaylist(WLibraryFolder * folder, WBackendNet   * backend,
                                                                        const QString & url,
                                                                        QStringList   * urls) const
{
    WBackendNetPlaylistInfo info = backend->getPlaylistInfo(url);

    if (info.isValid() == false) return;

    QString source = backend->getUrlPlaylist(info);

    if (urls->contains(source)) return;

    urls->append(source);

    WLibraryFolderItem item(info.type, WLocalObject::Default);

    item.source = url;
    item.title  = backend->title() + " - " + info.id.simplified();

    folder->addItem(item);
}

void WControllerPlaylistPrivate::applyPlaylistVbml(WLibraryFolder * folder,
                                                   const QString  & url, QStringList * urls) const
{
    if (urls->contains(url)) return;

    urls->append(url);

    WLibraryFolderItem item(WLibraryItem::Playlist, WLocalObject::Default);

    item.source = url;

    folder->addItem(item);
}

void WControllerPlaylistPrivate::applySources(WLibraryFolder                         * folder,
                                              const QList<WControllerPlaylistSource> & sources,
                                              QStringList                            * urls) const
{
    foreach (const WControllerPlaylistSource & source, sources)
    {
        if (urls->count() == CONTROLLERPLAYLIST_MAX_ITEMS) return;

        const QString & url = source.url;

        if (urls->contains(url)) continue;

        urls->append(url);

        QString extension = WControllerNetwork::extractUrlExtension(url);

        WLibraryFolderItem item(WLibraryItem::Playlist, WLocalObject::Default);

        item.source = url;
        item.title  = source.title.simplified();

        folder->addItem(item);
    }
}

//-------------------------------------------------------------------------------------------------

void WControllerPlaylistPrivate::applyCurrentIndex(WPlaylist * playlist) const
{
    QString source = playlist->source();

    for (int i = 0; i < playlist->count(); i++)
    {
        if (playlist->trackSource(i) == source)
        {
            playlist->setCurrentIndex(i);

            return;
        }
    }
}

//-------------------------------------------------------------------------------------------------

void WControllerPlaylistPrivate::loadUrls(QIODevice * device, const WBackendNetQuery & query,
                                                              const QString          & url,
                                                              const char             * signal,
                                                              const char             * slot) const
{
    Q_Q(const WControllerPlaylist);

    WControllerPlaylistReply * reply = new WControllerPlaylistReply;

    QObject::connect(reply, signal, q, slot);

    reply->moveToThread(thread);

    // NOTE: We get crashes in QNetworkRequest when doing this on Android. But maybe we don't need
    //       it since readAll() seems to be thread safe.
    //device->moveToThread(thread);

    if (query.type == WBackendNetQuery::TypeImage)
    {
        // NOTE: We set the VBML uri instead of the image url.
        QString currentUrl = query.urlRedirect;

        if (WControllerPlaylist::urlIsVbmlUri(currentUrl) == false)
        {
            currentUrl = query.url;
        }

        methodVbml.invoke(reply, Q_ARG(QIODevice *, device), Q_ARG(const QString &, currentUrl),
                                                             Q_ARG(const QString &, url));

        return;
    }

    WBackendNetQuery::Target target = query.target;

    if (target == WBackendNetQuery::TargetVbml)
    {
        methodVbml.invoke(reply, Q_ARG(QIODevice     *, device),
                                 Q_ARG(const QString &, query.urlRedirect),
                                 Q_ARG(const QString &, url));

        return;
    }
    else if (target == WBackendNetQuery::TargetRelated)
    {
        methodRelated.invoke(reply, Q_ARG(QIODevice     *, device),
                                    Q_ARG(const QString &, query.urlRedirect),
                                    Q_ARG(const QString &, url),
                                    Q_ARG(int,             query.currentTime));

        return;
    }
    else if (target == WBackendNetQuery::TargetHtml)
    {
        methodHtml.invoke(reply,
                          Q_ARG(QIODevice *, device), Q_ARG(const QString &, query.urlRedirect));

        return;
    }

    QMetaMethod method;

    if (target == WBackendNetQuery::TargetM3u)
    {
        method = methodM3u;
    }
    else if (target == WBackendNetQuery::TargetFolder)
    {
        method = methodFolder;
    }
    else if (target == WBackendNetQuery::TargetFile)
    {
        method = methodFile;
    }
    else method = methodItem;

    method.invoke(reply, Q_ARG(QIODevice *, device), Q_ARG(const QString &, query.url));
}

//-------------------------------------------------------------------------------------------------

void WControllerPlaylistPrivate::scanItems(QList<WLibraryFolderItem> * items) const
{
    Q_Q(const WControllerPlaylist);

    for (int i = 0; i < items->count(); i++)
    {
        WLibraryFolderItem * item = const_cast<WLibraryFolderItem *> (&(items->at(i)));

        if (item->isFolder())
        {
            QString cover = q->backendCoverFromUrl(item->source);

            if (cover.isEmpty() == false)
            {
                item->cover = cover;
            }
        }
        else
        {
            QString url = item->source;

            WBackendNet * backend = q->backendFromUrl(url);

            if (backend)
            {
                WBackendNetPlaylistInfo info = backend->getPlaylistInfo(url);

                backend->tryDelete();

                if (info.isValid())
                {
                    item->type = info.type;
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------

void WControllerPlaylistPrivate::addToCache(const QString    & url,
                                            const QByteArray & array,
                                            const QString    & extension) const
{
    if (array.isEmpty()) return;

    wControllerFile->addCache(url, array, extension);
}

//-------------------------------------------------------------------------------------------------

WBackendNet * WControllerPlaylistPrivate::backendTrack(const QString & source,
                                                       WPlaylist * playlist, int index) const
{
    Q_Q(const WControllerPlaylist);

    WBackendNet * backend = q->backendFromUrl(source);

    if (backend)
    {
        return backend;
    }
    else return q->backendFromUrl(playlist->trackSource(index));
}

//-------------------------------------------------------------------------------------------------

WBackendNetQuery WControllerPlaylistPrivate::extractQuery(WBackendNet * backend,
                                                          const QUrl & url, QString & id) const
{
#ifdef QT_4
    QString method = url.queryItemValue("method");
    QString label  = url.queryItemValue("label");
    QString q      = url.queryItemValue("q");
#else
    QUrlQuery urlQuery(url);

    QString method = urlQuery.queryItemValue("method");
    QString label  = urlQuery.queryItemValue("label");
    QString q      = urlQuery.queryItemValue("q");
#endif

    q = WControllerNetwork::decodeUrl(q);

    WBackendNetQuery query = backend->createQuery(method, label, q);

    if (query.isValid()) return query;

    //---------------------------------------------------------------------------------------------
    // NOTE: When we fail to create a valid query we try to create one with the backendSearch.

    QString host = backend->getHost();

    backend = q_func()->backendSearch();

    if (backend == NULL) return query;

    if (host.isEmpty())
    {
         query = backend->createQuery(method, label, id + " " + q);
    }
    else query = backend->createQuery(method, label, "site:" + host + " " + q);

    id = backend->id();

    backend->tryDelete();

    return query;
}

WBackendNetQuery WControllerPlaylistPrivate::extractRelated(const QUrl & url) const
{
#ifdef QT_4
    QString method = url.queryItemValue("method");
#else
    QUrlQuery urlQuery(url);

    QString method = urlQuery.queryItemValue("method");
#endif

    if (method != "related") return WBackendNetQuery();

#ifdef QT_4
    QString q = WControllerNetwork::decodeUrl(url.queryItemValue("q"));
#else
    QString q = WControllerNetwork::decodeUrl(urlQuery.queryItemValue("q"));
#endif

    if (WControllerPlaylist::urlIsVbml(q))
    {
        WBackendNetQuery query(q);

        query.target = WBackendNetQuery::TargetRelated;

        query.currentTime = WControllerPlaylist::extractTime(q, -1);

        return query;
    }
    else if (WControllerNetwork::urlIsFile(q))
    {
        QFileInfo info(WControllerFile::filePath(q));

        WBackendNetQuery query("file:///" + info.absolutePath());

        query.target = WBackendNetQuery::TargetFolder;

        return query;
    }
    else if (WControllerNetwork::urlIsHttp(q))
    {
        WBackendNetQuery query(q);

        query.target = WBackendNetQuery::TargetRelated;

        // NOTE: The url might be a large media file so we scope it to text.
        query.scope = WAbstractLoader::ScopeText;

        query.currentTime = WControllerPlaylist::extractTime(q, -1);

        return query;
    }
    else return WBackendNetQuery();
}

//-------------------------------------------------------------------------------------------------

bool WControllerPlaylistPrivate::resolveTrack(const QString    & backendId,
                                              WBackendNetQuery & query) const
{
    if (resolveQuery(query)) return query.isValid();

    QString id = query.backend;

    if (id.isEmpty() || id == backendId) return query.isValid();

    Q_Q(const WControllerPlaylist);

    WBackendNet * backend = q->backendFromId(id);

    if (backend == NULL) return query.isValid();

    query = backend->getQueryTrack(query.url);

    backend->tryDelete();

    return query.isValid();
}

bool WControllerPlaylistPrivate::resolvePlaylist(const QString    & backendId,
                                                 WBackendNetQuery & query) const
{
    if (resolveQuery(query)) return query.isValid();

    QString id = query.backend;

    if (id.isEmpty() || id == backendId) return query.isValid();

    Q_Q(const WControllerPlaylist);

    WBackendNet * backend = q->backendFromId(id);

    if (backend == NULL) return query.isValid();

    query = backend->getQueryPlaylist(query.url);

    backend->tryDelete();

    return query.isValid();
}

bool WControllerPlaylistPrivate::resolveFolder(const QString    & backendId,
                                               WBackendNetQuery & query) const
{
    if (resolveQuery(query)) return query.isValid();

    QString id = query.backend;

    if (id.isEmpty() || id == backendId) return query.isValid();

    Q_Q(const WControllerPlaylist);

    WBackendNet * backend = q->backendFromId(id);

    if (backend == NULL) return query.isValid();

    query = backend->getQueryFolder(query.url);

    backend->tryDelete();

    return query.isValid();
}

bool WControllerPlaylistPrivate::resolveItem(const QString    & backendId,
                                             WBackendNetQuery & query) const
{
    if (resolveQuery(query)) return query.isValid();

    QString id = query.backend;

    if (id.isEmpty() || id == backendId) return query.isValid();

    Q_Q(const WControllerPlaylist);

    WBackendNet * backend = q->backendFromId(id);

    if (backend == NULL) return query.isValid();

    query = backend->getQueryItem(query.url);

    backend->tryDelete();

    return query.isValid();
}

bool WControllerPlaylistPrivate::resolveQuery(WBackendNetQuery & query) const
{
    QString url = query.url;

    if (WBackendNet::checkQuery(url) == false) return false;

    Q_Q(const WControllerPlaylist);

    WBackendNet * backend = q->backendFromUrl(url);

    if (backend == NULL) return true;

    query = backend->extractQuery(url);

    backend->tryDelete();

    return true;
}

//-------------------------------------------------------------------------------------------------

void WControllerPlaylistPrivate::getDataTrack(WPlaylist * playlist,
                                              WTrack    * track, const WBackendNetQuery & query)
{
    Q_Q(WControllerPlaylist);

    WAbstractLoader * loader = loaders.value(query.type);

    WRemoteData * data = WControllerPlaylist::getDataQuery(loader, query, q);

    WControllerPlaylistQuery * queryTrack
        = new WControllerPlaylistQuery(query, WControllerPlaylistQuery::TypeTrack);

    queryTrack->data  = data;
    queryTrack->item  = playlist;
    queryTrack->track = track;

    queries.append(queryTrack);

    jobs.insert(data, queryTrack);

    QObject::connect(data, SIGNAL(loaded(WRemoteData *)), q, SLOT(onLoaded(WRemoteData *)));

    int index = playlist->indexOf(track);

    playlist->setTrackState(index, WTrack::Loading);
}

void WControllerPlaylistPrivate::getDataPlaylist(WPlaylist              * playlist,
                                                 const WBackendNetQuery & query)
{
    if (query.clearItems)
    {
        playlist->clearTracks();
    }

    getDataLibraryItem(playlist, query, WControllerPlaylistQuery::TypePlaylist);
}

void WControllerPlaylistPrivate::getDataFolder(WLibraryFolder         * folder,
                                               const WBackendNetQuery & query)
{
    if (query.clearItems)
    {
        folder->clearItems();
    }

    getDataLibraryItem(folder, query, WControllerPlaylistQuery::TypeFolder);
}

void WControllerPlaylistPrivate::getDataItem(WLibraryItem * item, const WBackendNetQuery & query)
{
    getDataLibraryItem(item, query, WControllerPlaylistQuery::TypeItem);
}

bool WControllerPlaylistPrivate::getDataRelated(WBackendNet * backend,
                                                WPlaylist   * playlist, const QString & id)
{
    WBackendNetQuery query = backend->createQuery("related", "tracks", id);

    if (resolvePlaylist(backend->id(), query))
    {
        getDataPlaylist(playlist, query);

        return true;
    }
    else return false;
}

//-------------------------------------------------------------------------------------------------

void WControllerPlaylistPrivate::getDataLibraryItem(WLibraryItem                   * item,
                                                    const WBackendNetQuery         & query,
                                                    WControllerPlaylistQuery::Type   type)
{
    Q_Q(WControllerPlaylist);

    WAbstractLoader * loader = loaders.value(query.type);

    WRemoteData * data = WControllerPlaylist::getDataQuery(loader, query, q);

    WControllerPlaylistQuery * queryItem = new WControllerPlaylistQuery(query, type);

    queryItem->data = data;
    queryItem->item = item;

    queries.append(queryItem);

    jobs.insert(data, queryItem);

    QObject::connect(data, SIGNAL(loaded(WRemoteData *)), q, SLOT(onLoaded(WRemoteData *)));

    item->d_func()->setQueryLoading(true);

    emit item->queryStarted();
}

//-------------------------------------------------------------------------------------------------

bool WControllerPlaylistPrivate::getNextTracks(const QString                 & backendId,
                                               WPlaylist                     * playlist,
                                               WTrack                        * track,
                                               const QList<WBackendNetQuery> & queries, int index)
{
    bool result = false;

    foreach (WBackendNetQuery query, queries)
    {
        if (index >= CONTROLLERPLAYLIST_MAX_QUERY)
        {
            qWarning("WControllerPlaylistPrivate::getNextTracks: Maximum queries reached.");

            return false;
        }

        if (resolveTrack(backendId, query) == false) continue;

        index++;

        query.indexNext = index;

        getDataTrack(playlist, track, query);

        result = true;
    }

    return result;
}

bool WControllerPlaylistPrivate::getNextPlaylists(const QString                 & backendId,
                                                  WPlaylist                     * playlist,
                                                  const QList<WBackendNetQuery> & queries,
                                                  int                             index)
{
    bool result = false;

    foreach (WBackendNetQuery query, queries)
    {
        if (index >= CONTROLLERPLAYLIST_MAX_QUERY)
        {
            qWarning("WControllerPlaylistPrivate::getNextPlaylists: Maximum queries reached.");

            return false;
        }

        if (resolvePlaylist(backendId, query) == false) continue;

        index++;

        query.indexNext = index;

        getDataPlaylist(playlist, query);

        result = true;
    }

    return result;
}

bool WControllerPlaylistPrivate::getNextFolders(const QString                 & backendId,
                                                WLibraryFolder                * folder,
                                                const QList<WBackendNetQuery> & queries, int index)
{
    bool result = false;

    foreach (WBackendNetQuery query, queries)
    {
        if (index >= CONTROLLERPLAYLIST_MAX_QUERY)
        {
            qWarning("WControllerPlaylistPrivate::getNextFolders: Maximum queries reached.");

            return false;
        }

        if (resolveFolder(backendId, query) == false) continue;

        index++;

        query.indexNext = index;

        getDataFolder(folder, query);

        result = true;
    }

    return result;
}

bool WControllerPlaylistPrivate::getNextItems(const QString                 & backendId,
                                              WLibraryItem                  * item,
                                              const QList<WBackendNetQuery> & queries, int index)
{
    bool result = false;

    foreach (WBackendNetQuery query, queries)
    {
        if (index >= CONTROLLERPLAYLIST_MAX_QUERY)
        {
            qWarning("WControllerPlaylistPrivate::getNextItems: Maximum queries reached.");

            return false;
        }

        if (resolveItem(backendId, query) == false) continue;

        index++;

        query.indexNext = index;

        getDataItem(item, query);

        result = true;
    }

    return result;
}

bool WControllerPlaylistPrivate::getNextTrack(const QString          & backendId,
                                              WPlaylist              * playlist,
                                              WTrack                 * track,
                                              const WBackendNetQuery & query, int index)
{
    QList<WBackendNetQuery> queries;

    queries.append(query);

    return getNextTracks(backendId, playlist, track, queries, index);
}

bool WControllerPlaylistPrivate::getNextPlaylist(const QString          & backendId,
                                                 WPlaylist              * playlist,
                                                 const WBackendNetQuery & query, int index)
{
    QList<WBackendNetQuery> queries;

    queries.append(query);

    return getNextPlaylists(backendId, playlist, queries, index);
}

bool WControllerPlaylistPrivate::getNextFolder(const QString          & backendId,
                                               WLibraryFolder         * folder,
                                               const WBackendNetQuery & query, int index)
{
    QList<WBackendNetQuery> queries;

    queries.append(query);

    return getNextFolders(backendId, folder, queries, index);
}

bool WControllerPlaylistPrivate::getNextItem(const QString          & backendId,
                                             WLibraryItem           * item,
                                             const WBackendNetQuery & query, int index)
{
    QList<WBackendNetQuery> queries;

    queries.append(query);

    return getNextItems(backendId, item, queries, index);
}

//-------------------------------------------------------------------------------------------------

bool WControllerPlaylistPrivate::checkTrack(WTrack * track) const
{
    foreach (WControllerPlaylistQuery * query, queries)
    {
        if (query->track == track)
        {
            return true;
        }
    }

    return false;
}

bool WControllerPlaylistPrivate::checkPlaylist(WPlaylist * playlist) const
{
    foreach (WControllerPlaylistQuery * query, queries)
    {
        if (query->type == WControllerPlaylistQuery::TypePlaylist && query->item == playlist)
        {
            return true;
        }
    }

    return false;
}

bool WControllerPlaylistPrivate::checkFolder(WLibraryFolder * folder) const
{
    foreach (WControllerPlaylistQuery * query, queries)
    {
        if (query->type == WControllerPlaylistQuery::TypeFolder && query->item == folder)
        {
            return true;
        }
    }

    return false;
}

bool WControllerPlaylistPrivate::checkItem(WLibraryItem * item) const
{
    foreach (WControllerPlaylistQuery * query, queries)
    {
        if (query->type == WControllerPlaylistQuery::TypeItem && query->item == item)
        {
            return true;
        }
    }

    return false;
}

//-------------------------------------------------------------------------------------------------

void WControllerPlaylistPrivate::abortTrack(WTrack * track)
{
    foreach (WControllerPlaylistQuery * query, queries)
    {
        if (query->track == track)
        {
            removeQuery(query);
        }
    }
}

void WControllerPlaylistPrivate::abortPlaylist(WPlaylist * playlist)
{
    foreach (WControllerPlaylistQuery * query, queries)
    {
        if (query->type == WControllerPlaylistQuery::TypePlaylist && query->item == playlist)
        {
            removeQuery(query);
        }
    }
}

void WControllerPlaylistPrivate::abortFolder(WLibraryFolder * folder)
{
    foreach (WControllerPlaylistQuery * query, queries)
    {
        if (query->type == WControllerPlaylistQuery::TypeFolder && query->item == folder)
        {
            removeQuery(query);
        }
    }
}

void WControllerPlaylistPrivate::abortItem(WLibraryItem * item)
{
    foreach (WControllerPlaylistQuery * query, queries)
    {
        // NOTE: This is supposed to remove all queries for a given item, no matter the type.
        if (query->item == item)
        {
            removeQuery(query);
        }
    }
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WControllerPlaylistPrivate::onLoaded(WRemoteData * data)
{
    Q_Q(WControllerPlaylist);

    WControllerPlaylistQuery * query = jobs.take(data);

    if (query == NULL) qDebug("TEST: QUERY SHOULD NOT BE NULL");

    WBackendNetQuery * backendQuery = &(query->backendQuery);

    WBackendNet * backend;

    QString id = backendQuery->backend;

    if (id.isEmpty())
    {
         backend = q->backendFromUrl(backendQuery->url);
    }
    else if (id == "vbml")
    {
        backend = NULL;

        backendQuery->target = WBackendNetQuery::TargetVbml;
    }
    else if (id == "related")
    {
        backend = NULL;

        backendQuery->target = WBackendNetQuery::TargetRelated;
    }
    else backend = q->backendFromId(id);

    // NOTE: We apply the backend to the query so we can delete it later.
    query->backend = backend;

    WLibraryItem * item = query->item;

    // NOTE: Maybe the item was destroyed while we were loading the backend.
    if (item == NULL)
    {
        deleteQuery(query);

        delete data;

        return;
    }

    backendQuery->urlRedirect = data->url();

    if (data->hasError() && backendQuery->skipError == false)
    {
        if (query->type == WControllerPlaylistQuery::TypeTrack)
        {
            WPlaylist * playlist = item->toPlaylist();

            if (backend) backend->queryFailed(*backendQuery);

            int index = playlist->indexOf(query->track);

            if (index != -1)
            {
                WTrack * track = query->track;

                deleteQuery(query);

                track->setState(WTrack::Default);

                playlist->updateTrack(index);
            }
            else deleteQuery(query);

            playlist->d_func()->applyTrackDefault();

            delete data;

            return;
        }
        else if (query->type == WControllerPlaylistQuery::TypeFolder && backendQuery->id == 0)
        {
            WLibraryItem * libraryItem = item->toFolder()->createLibraryItemAt(0, true);

            if (libraryItem)
            {
                libraryItem->d_func()->setQueryDefault();

                if (backend) backend->queryFailed(*backendQuery);

                deleteQuery(query);

                item->d_func()->setQueryDefault();

                libraryItem->tryDelete();

                delete data;

                return;
            }
        }

        if (backend) backend->queryFailed(*backendQuery);

        deleteQuery(query);

        item->d_func()->setQueryDefault();

        delete data;

        return;
    }

    if (backend == NULL && backendQuery->target == WBackendNetQuery::TargetDefault)
    {
        deleteQuery(query);

        item->d_func()->setQueryDefault();

        delete data;

        return;
    }

    QIODevice * networkReply = data->takeReply(NULL);

    //query->data  = data;
    query->reply = networkReply;

    replies.insert(networkReply, query);

    if (query->type == WControllerPlaylistQuery::TypeTrack)
    {
        if (backendQuery->target == WBackendNetQuery::TargetDefault)
        {
            backend->loadTrack(networkReply, *backendQuery,
                               q, SLOT(onTrackLoaded(QIODevice *, WBackendNetTrack)));
        }
        else loadUrls(networkReply, *backendQuery, item->source(),
                      SIGNAL(loaded(QIODevice *, const WControllerPlaylistData &)),
                      SLOT(onUrlTrack(QIODevice *, const WControllerPlaylistData &)));
    }
    else if (query->type == WControllerPlaylistQuery::TypePlaylist)
    {
        if (backendQuery->target == WBackendNetQuery::TargetDefault)
        {
            backend->loadPlaylist(networkReply, *backendQuery,
                                  q, SLOT(onPlaylistLoaded(QIODevice *, WBackendNetPlaylist)));
        }
        else loadUrls(networkReply, *backendQuery, item->source(),
                      SIGNAL(loaded(QIODevice *, const WControllerPlaylistData &)),
                      SLOT(onUrlPlaylist(QIODevice *, const WControllerPlaylistData &)));
    }
    else if (query->type == WControllerPlaylistQuery::TypeFolder)
    {
        if (backendQuery->target == WBackendNetQuery::TargetDefault)
        {
            backend->loadFolder(networkReply, *backendQuery,
                                q, SLOT(onFolderLoaded(QIODevice *, WBackendNetFolder)));
        }
        else loadUrls(networkReply, *backendQuery, item->source(),
                      SIGNAL(loaded(QIODevice *, const WControllerPlaylistData &)),
                      SLOT(onUrlFolder(QIODevice *, const WControllerPlaylistData &)));
    }
    else // if (query->type == WControllerPlaylistQuery::TypeItem)
    {
        if (backendQuery->target == WBackendNetQuery::TargetDefault)
        {
            backend->loadItem(networkReply, *backendQuery,
                              q, SLOT(onItemLoaded(QIODevice *, WBackendNetItem)));
        }
        else loadUrls(networkReply, *backendQuery, item->source(),
                      SIGNAL(loadedItem(QIODevice *, const WControllerPlaylistItem &)),
                      SLOT(onUrlItem(QIODevice *, const WControllerPlaylistItem &)));
    }

    delete data;
}

//-------------------------------------------------------------------------------------------------

void WControllerPlaylistPrivate::onTrackLoaded(QIODevice * device, const WBackendNetTrack & reply)
{
    WControllerPlaylistQuery * query = replies.take(device);

    device->deleteLater();

    WLibraryItem * item = query->item;

    // NOTE: The item was cleared so the query is deleted right away.
    if (item == NULL)
    {
        deleteQuery(query);

        return;
    }

    const WBackendNetQuery & backendQuery = query->backendQuery;

    WPlaylist * playlist = item->toPlaylist();
    WTrack    * track    = query->track;

    if (reply.reload)
    {
        int indexReload = backendQuery.indexReload;

        if (indexReload < CONTROLLERPLAYLIST_MAX_RELOAD)
        {
            WBackendNetQuery nextQuery = backendQuery;

            deleteQuery(query);

            nextQuery.indexReload = indexReload + 1;

            getDataTrack(playlist, track, nextQuery);

            return;
        }
    }

    int indexNext = backendQuery.indexNext;

    WBackendNet * backend = query->backend;

    backend->applyTrack(backendQuery, reply);

    QString backendId = backend->id();

    deleteQuery(query);

    int index = playlist->indexOf(track);

    if (index == -1)
    {
        playlist->d_func()->applyTrackDefault();

        return;
    }

    if (reply.valid)
    {
        const WTrack & trackReply = reply.track;

        trackReply.applyDataTo(track);

        emit playlist->trackQueryEnded();

        addToCache(track->source(), reply.cache);

        if (getNextTracks(backendId, playlist, track, reply.nextQueries, indexNext))
        {
            playlist->updateTrack(index);

            return;
        }

        // NOTE: Maybe other queries are still loading.
        if (checkTrack(track)) return;

        applyTrack(playlist, track, trackReply.state(), index);
    }
    else
    {
        emit playlist->trackQueryEnded();

        // NOTE: Maybe other queries are still loading.
        if (checkTrack(track)) return;

        track->setState(WTrack::Default);

        playlist->updateTrack(index);
    }

    // NOTE: We reload the cover in case it changed since the last time.
    WControllerPlaylist::reloadCover(track->cover());

    playlist->d_func()->applyTrackLoaded(index);
}

void WControllerPlaylistPrivate::onPlaylistLoaded(QIODevice                 * device,
                                                  const WBackendNetPlaylist & reply)
{
    WControllerPlaylistQuery * query = replies.take(device);

    device->deleteLater();

    WLibraryItem * item = query->item;

    // NOTE: The item was cleared so the query is deleted right away.
    if (item == NULL)
    {
        deleteQuery(query);

        return;
    }

    const WBackendNetQuery & backendQuery = query->backendQuery;

    WPlaylist * playlist = item->toPlaylist();

    if (reply.reload)
    {
        int indexReload = backendQuery.indexReload;

        if (indexReload < CONTROLLERPLAYLIST_MAX_RELOAD)
        {
            WBackendNetQuery nextQuery = backendQuery;

            deleteQuery(query);

            nextQuery.indexReload = indexReload + 1;

            getDataPlaylist(playlist, nextQuery);

            return;
        }
    }

    int indexNext = backendQuery.indexNext;

    WBackendNet * backend = query->backend;

    backend->applyPlaylist(backendQuery, reply);

    QString backendId = backend->id();

    deleteQuery(query);

    if (reply.valid)
    {
        QString title = reply.title;
        QString cover = reply.cover;

        if (playlist->isPlaylistSearch() == false)
        {
            if (title.isEmpty() == false)
            {
                playlist->setTitle(title);
            }

            if (cover.isEmpty() == false)
            {
                playlist->setCover(cover);
            }
        }

        if (reply.clearDuplicate)
        {
            QList<WTrack> * tracks = const_cast<QList<WTrack> *> (&(reply.tracks));

            for (int i = 0; i < tracks->count();)
            {
                const WTrack & track = tracks->at(i);

                if (playlist->containsSource(track.source()))
                {
                    tracks->removeAt(i);
                }
                else i++;
            }
        }

        playlist->addTracks(reply.tracks);

        int index = reply.currentIndex;

        // NOTE: Sometimes the current index has already been set prior to this.
        if (index != -1 && playlist->currentIndex() == -1)
        {
            playlist->setCurrentIndex(index);
        }

        emit playlist->queryEnded();

        addToCache(playlist->source(), reply.cache);

        if (getNextPlaylists(backendId, playlist, reply.nextQueries, indexNext)) return;
    }
    else emit playlist->queryEnded();

    // NOTE: Maybe other queries are still loading.
    if (checkPlaylist(playlist)) return;

    // NOTE: We reload the cover in case it changed since the last time.
    WControllerPlaylist::reloadCover(playlist->cover());

    playlist->d_func()->setQueryLoaded();
}

void WControllerPlaylistPrivate::onFolderLoaded(QIODevice               * device,
                                                const WBackendNetFolder & reply)
{
    WControllerPlaylistQuery * query = replies.take(device);

    device->deleteLater();

    WLibraryItem * item = query->item;

    // NOTE: The item was cleared so the query is deleted right away.
    if (item == NULL)
    {
        deleteQuery(query);

        return;
    }

    const WBackendNetQuery & backendQuery = query->backendQuery;

    WLibraryFolder * folder = item->toFolder();

    if (reply.reload)
    {
        int indexReload = backendQuery.indexReload;

        if (indexReload < CONTROLLERPLAYLIST_MAX_RELOAD)
        {
            WBackendNetQuery nextQuery = backendQuery;

            deleteQuery(query);

            nextQuery.indexReload = indexReload + 1;

            getDataFolder(folder, nextQuery);

            return;
        }
    }

    int indexNext = backendQuery.indexNext;

    WBackendNet * backend = query->backend;

    backend->applyFolder(backendQuery, reply);

    QString backendId = backend->id();

    deleteQuery(query);

    if (reply.valid)
    {
        QString title = reply.title;
        QString cover = reply.cover;

        if (folder->isFolderSearch() == false)
        {
            if (title.isEmpty() == false)
            {
                folder->setTitle(title);
            }

            if (cover.isEmpty() == false)
            {
                folder->setCover(cover);
            }
        }

        if (reply.clearDuplicate)
        {
            QList<WLibraryFolderItem> * items
                                      = const_cast<QList<WLibraryFolderItem> *> (&(reply.items));

            for (int i = 0; i < items->count();)
            {
                const WLibraryFolderItem & item = items->at(i);

                if (folder->containsSource(item.source))
                {
                    items->removeAt(i);
                }
                else i++;
            }

            if (reply.scanItems)
            {
                scanItems(items);
            }
        }
        else if (reply.scanItems)
        {
            QList<WLibraryFolderItem> * items
                                      = const_cast<QList<WLibraryFolderItem> *> (&(reply.items));

            scanItems(items);
        }

        folder->addItems(reply.items);

        int index = reply.currentIndex;

        // NOTE: Sometimes the current index has already been set prior to this.
        if (index != -1 && folder->currentIndex() == -1)
        {
            folder->setCurrentIndex(index);
        }

        emit folder->queryEnded();

        addToCache(folder->source(), reply.cache);

        if (getNextFolders(backendId, folder, reply.nextQueries, indexNext)) return;
    }
    else emit folder->queryEnded();

    // NOTE: Maybe other queries are still loading.
    if (checkFolder(folder)) return;

    // NOTE: We reload the cover in case it changed since the last time.
    WControllerPlaylist::reloadCover(folder->cover());

    folder->d_func()->setQueryLoaded();
}

void WControllerPlaylistPrivate::onItemLoaded(QIODevice * device, const WBackendNetItem & reply)
{
    WControllerPlaylistQuery * query = replies.take(device);

    device->deleteLater();

    WLibraryItem * item = query->item;

    // NOTE: The item was cleared so the query is deleted right away.
    if (item == NULL)
    {
        deleteQuery(query);

        return;
    }

    const WBackendNetQuery & backendQuery = query->backendQuery;

    int indexNext = backendQuery.indexNext;

    WBackendNet * backend = query->backend;

    backend->applyItem(backendQuery, reply);

    QString backendId = backend->id();

    deleteQuery(query);

    if (reply.valid)
    {
        QString extension = reply.extension;

        emit item->queryData(reply.data, extension);

        emit item->queryEnded();

        addToCache(item->source(), reply.cache, extension);

        if (getNextItems(backendId, item, reply.nextQueries, indexNext)) return;
    }
    else emit item->queryEnded();

    // NOTE: Maybe other queries are still loading.
    if (checkItem(item)) return;

    item->d_func()->setQueryLoaded();
}

//-------------------------------------------------------------------------------------------------

void WControllerPlaylistPrivate::onUrlTrack(QIODevice                     * device,
                                            const WControllerPlaylistData & data)
{
    WControllerPlaylistQuery * query = replies.take(device);

    device->deleteLater();

    WLibraryItem * item = query->item;

    // NOTE: The item was cleared so the query is deleted right away.
    if (item == NULL)
    {
        deleteQuery(query);

        return;
    }

    WPlaylist * playlist = item->toPlaylist();
    WTrack    * track    = query->track;

    int indexNext = query->backendQuery.indexNext;

    deleteQuery(query);

    int index = playlist->indexOf(track);

    if (index == -1)
    {
        playlist->d_func()->applyTrackDefault();

        return;
    }

    QString origin = data.origin;

    if (data.type == WControllerPlaylist::Redirect)
    {
        QString source = data.source;

        if (source.isEmpty() == false)
        {
            track->setSource(source);

            playlist->updateTrack(index);
        }

        applyNextTrack(playlist, track, origin, indexNext);

        return;
    }

    const QList<WTrack> & tracks = data.tracks;

    if (tracks.isEmpty())
    {
        emit playlist->trackQueryEnded();

        track->setState(WTrack::Default);

        playlist->updateTrack(index);
    }
    else
    {
        const WTrack & trackReply = tracks.first();

        trackReply.applyDataTo(track);

        emit playlist->trackQueryEnded();

        if (origin.isEmpty() == false)
        {
            Q_Q(WControllerPlaylist);

            WBackendNet * backend = q->backendFromUrl(origin);

            if (backend)
            {
                QString backendId = backend->id();

                WBackendNetQuery query = backend->getQueryTrack(origin);

                backend->tryDelete();

                if (getNextTrack(backendId, playlist, track, query, indexNext))
                {
                    playlist->updateTrack(index);

                    return;
                }
            }

            WBackendNetQuery query(origin);

            query.target = WBackendNetQuery::TargetVbml;

            if (getNextTrack("", playlist, track, query, indexNext))
            {
                playlist->updateTrack(index);

                return;
            }
        }

        applyTrack(playlist, track, trackReply.state(), index);
    }

    // NOTE: We reload the cover in case it changed since the last time.
    WControllerPlaylist::reloadCover(track->cover());

    playlist->d_func()->applyTrackLoaded(index);
}

void WControllerPlaylistPrivate::onUrlPlaylist(QIODevice                     * device,
                                               const WControllerPlaylistData & data)
{
    WControllerPlaylistQuery * query = replies.take(device);

    device->deleteLater();

    WLibraryItem * item = query->item;

    // NOTE: The item was cleared so the query is deleted right away.
    if (item == NULL)
    {
        deleteQuery(query);

        return;
    }

    const WBackendNetQuery & backendQuery = query->backendQuery;

    QString urlQuery = backendQuery.urlBase;

    if (urlQuery.isEmpty()) urlQuery = backendQuery.url;

    int indexNext = backendQuery.indexNext;

    deleteQuery(query);

    //---------------------------------------------------------------------------------------------
    // VBML

    WPlaylist * playlist = item->toPlaylist();

    WControllerPlaylist::Type type = data.type;

    QString origin = data.origin;
    QString source = data.source;

    if (type == WControllerPlaylist::Redirect)
    {
        if (source.isEmpty() == false)
        {
            playlist->applySource(source);
        }

        applyNextPlaylist(playlist, origin, urlQuery, indexNext);

        return;
    }
    else if (type == WControllerPlaylist::Source)
    {
        Q_Q(WControllerPlaylist);

        WBackendNet * backend = q->backendFromTrack(origin);

        if (backend)
        {
            playlist->addSource(origin, true);

            playlist->d_func()->setQueryLoaded();

            QString backendId = backend->id();

            WBackendNetQuery query = backend->createQuery("related", "tracks",
                                                          backend->getTrackId(origin));

            backend->tryDelete();

            getNextPlaylist(backendId, playlist, query, indexNext);
        }
        else
        {
            // NOTE: We want the currentTime in seconds.
            QString time = QString::number(data.currentTime / 1000);

            origin = WControllerNetwork::applyFragmentValue(origin, "t", time);

            origin = WControllerPlaylist::createSource("vbml", "related", "tracks", origin);

            applyNextPlaylist(playlist, origin, QString(), indexNext);
        }

        return;
    }
    else if (type == WControllerPlaylist::Related)
    {
        // NOTE: By checking the indexNext we can deduce it's a multi-track so we append the track
        //       source.
        if (indexNext)
        {
            // NOTE: We want a clean fragment without the timestamp.
            playlist->addSource(WControllerPlaylist::cleanSource(urlQuery), true);

            emit playlist->queryEnded();
        }

        applyNextPlaylist(playlist, origin, QString(), indexNext);

        return;
    }

    if (type == WControllerPlaylist::Feed)
    {
         playlist->setType(WLibraryItem::PlaylistFeed);
    }
    else playlist->setType(WLibraryItem::Playlist);

    playlist->setTitle(data.title);
    playlist->setCover(data.cover);

    if (origin.isEmpty() == false)
    {
        playlist->applySource(origin);

        applyNextPlaylist(playlist, origin, QString(), indexNext);

        return;
    }

    // NOTE: We are adding tracks when origin is not specified.
    playlist->addTracks(data.tracks);

    if (WControllerPlaylist::vbmlTypeTrack(type))
    {
        // NOTE: We select the first track right away.
        playlist->setCurrentIndex(0);
    }

    Q_Q(WControllerPlaylist);

    //---------------------------------------------------------------------------------------------
    // Media sources

    QStringList urlTracks;

    if (type == WControllerPlaylist::Unknown)
    {
        for (int i = 0; i < playlist->count(); i++)
        {
            const QString & url = playlist->d_func()->itemAt(i)->source();

            WBackendNet * backend = q->backendFromUrl(url);

            if (backend)
            {
                QString id = backend->getTrackId(url);

                if (id.isEmpty())
                {
                     urlTracks.append(url);
                }
                else urlTracks.append(backend->getUrlTrack(id));

                backend->tryDelete();
            }
            else urlTracks.append(url);
        }

        foreach (const WControllerPlaylistSource & source, data.sources)
        {
            const QString & url = source.url;

            WBackendNet * backend = q->backendFromUrl(url);

            if (backend == NULL)
            {
                if (WControllerPlaylist::urlIsVbml(url)
                    &&
                    urlTracks.count() != CONTROLLERPLAYLIST_MAX_TRACKS
                    &&
                    urlTracks.contains(url) == false)
                {
                    urlTracks.append(url);

                    WTrack track(url, WTrack::Default);

                    playlist->addTrack(track);
                }

                continue;
            }

            QString id = backend->getTrackId(url);

            if (id.isEmpty() == false)
            {
                if (urlTracks.count() == CONTROLLERPLAYLIST_MAX_TRACKS)
                {
                    backend->tryDelete();

                    break;
                }

                QString source = backend->getUrlTrack(id);

                backend->tryDelete();

                if (urlTracks.contains(source)) continue;

                urlTracks.append(source);

                WTrack track(url, WTrack::Default);

                playlist->addTrack(track);
            }
            else backend->tryDelete();
        }

        foreach (const WControllerPlaylistMedia & media, data.medias)
        {
            if (urlTracks.count() == CONTROLLERPLAYLIST_MAX_TRACKS) break;

            const QString & url = media.url;

            if (urlTracks.contains(url)) continue;

            urlTracks.append(url);

            WTrack track(url, WTrack::Default);

            track.setTitle(media.title);

            QString cover = media.cover;

            if (cover.isEmpty())
            {
                 track.setCover(data.cover);
            }
            else track.setCover(cover);

            track.setFeed(urlQuery);

            playlist->addTrack(track);
        }

        if (WControllerNetwork::urlIsFile(playlist->source()))
        {
            applyCurrentIndex(playlist);
        }
    }
    // NOTE: When having a single track we try to load related tracks.
    else if (playlist->count() == 1)
    {
        const QString & url = playlist->d_func()->itemAt(0)->source();

        urlTracks.append(url);
    }

    //---------------------------------------------------------------------------------------------
    // Next query

    emit playlist->queryEnded();

    if (source.isEmpty() == false)
    {
        WBackendNet * backend = q->backendFromUrl(source);

        if (backend)
        {
            QString backendId = backend->id();

            WBackendNetQuery query = backend->getQueryPlaylist(source);

            backend->tryDelete();

            if (getNextPlaylist(backendId, playlist, query, indexNext)) return;
        }
    }

    if (urlTracks.count() == 1)
    {
        int index = playlist->count() - 1;

        WBackendNet * backend = backendTrack(source, playlist, index);

        if (backend)
        {
            QString id = backend->getTrackId(source);

            if (id.isEmpty() == false)
            {
                playlist->loadTrack(index);

                playlist->d_func()->setQueryLoaded();

                getDataRelated(backend, playlist, id);

                backend->tryDelete();

                return;
            }
        }
    }

    // NOTE: We reload the cover in case it changed since the last time.
    WControllerPlaylist::reloadCover(playlist->cover());

    playlist->d_func()->setQueryLoaded();
}

void WControllerPlaylistPrivate::onUrlFolder(QIODevice                     * device,
                                             const WControllerPlaylistData & data)
{
    WControllerPlaylistQuery * query = replies.take(device);

    device->deleteLater();

    WLibraryItem * item = query->item;

    // NOTE: The item was cleared so the query is deleted right away.
    if (item == NULL)
    {
        deleteQuery(query);

        return;
    }

    //---------------------------------------------------------------------------------------------
    // Backend html query

    WLibraryFolder * folder = item->toFolder();

    QStringList urls;

    if (query->backendQuery.id == 1)
    {
        Q_Q(WControllerPlaylist);

        deleteQuery(query);

        for (int i = 0; i < folder->count(); i++)
        {
            const QString & url = folder->itemAt(i)->source;

            WBackendNet * backend = q->backendFromUrl(url);

            if (backend)
            {
                WBackendNetPlaylistInfo info = backend->getPlaylistInfo(url);

                if (info.isValid())
                {
                     urls.append(backend->getUrlPlaylist(info));

                     backend->tryDelete();
                }
                else
                {
                    backend->tryDelete();

                    urls.append(url);
                }
            }
            else urls.append(url);
        }

        foreach (const WControllerPlaylistSource & source, data.sources)
        {
            const QString & url = source.url;

            WBackendNet * backend = q->backendFromUrl(url);

            if (backend)
            {
                applyPlaylist(folder, backend, url, &urls);

                backend->tryDelete();

                if (urls.count() == CONTROLLERPLAYLIST_MAX_ITEMS) break;
            }
        }

        applySources(folder, data.files, &urls);

        folder->d_func()->setQueryFinished();

        return;
    }


    const WBackendNetQuery & backendQuery = query->backendQuery;

    QString urlQuery = backendQuery.urlBase;

    if (urlQuery.isEmpty()) urlQuery = backendQuery.url;

    int indexNext = backendQuery.indexNext;

    deleteQuery(query);

    //---------------------------------------------------------------------------------------------
    // VBML

    WPlaylist * playlist = folder->createLibraryItemAt(0, true)->toPlaylist();

    WControllerPlaylist::Type type = data.type;

    QString origin = data.origin;
    QString source = data.source;

    if (data.type == WControllerPlaylist::Redirect)
    {
        playlist->tryDelete();

        folder->removeAt(0);

        if (source.isEmpty() == false)
        {
            folder->applySource(source);
        }

        applyNextFolder(folder, origin, urlQuery, indexNext);

        return;
    }

    if (type == WControllerPlaylist::Feed)
    {
         playlist->setType(WLibraryItem::PlaylistFeed);
    }
    else playlist->setType(WLibraryItem::Playlist);

    playlist->setTitle(data.title);
    playlist->setCover(data.cover);

    if (origin.isEmpty() == false)
    {
        playlist->tryDelete();

        folder->removeAt(0);

        folder->applySource(origin);

        applyNextFolder(folder, origin, QString(), indexNext);

        return;
    }

    // NOTE: We are adding tracks when origin is not specified.
    playlist->addTracks(data.tracks);

    if (WControllerPlaylist::vbmlTypeTrack(type))
    {
        // NOTE: We select the first track right away.
        playlist->setCurrentIndex(0);
    }

    Q_Q(WControllerPlaylist);

    //---------------------------------------------------------------------------------------------
    // Media sources

    QStringList urlTracks;

    if (type == WControllerPlaylist::Unknown)
    {
        foreach (const WControllerPlaylistSource & source, data.sources)
        {
            const QString & url = source.url;

            WBackendNet * backend = q->backendFromUrl(url);

            if (backend == NULL)
            {
                if (WControllerPlaylist::urlIsVbml(url))
                {
                    if (urlTracks.count() != CONTROLLERPLAYLIST_MAX_TRACKS
                        &&
                        urlTracks.contains(url) == false)
                    {
                        urlTracks.append(url);

                        WTrack track(url, WTrack::Default);

                        playlist->addTrack(track);
                    }

                    if (urls.count() != CONTROLLERPLAYLIST_MAX_ITEMS)
                    {
                        applyPlaylistVbml(folder, url, &urls);
                    }
                }

                continue;
            }

            QString id = backend->getTrackId(url);

            if (id.isEmpty() == false && urlTracks.count() != CONTROLLERPLAYLIST_MAX_TRACKS)
            {
                QString source = backend->getUrlTrack(id);

                backend->tryDelete();

                if (urlTracks.contains(source)) continue;

                urlTracks.append(source);

                WTrack track(url, WTrack::Default);

                playlist->addTrack(track);
            }
            else if (urls.count() != CONTROLLERPLAYLIST_MAX_ITEMS)
            {
                applyPlaylist(folder, backend, url, &urls);

                backend->tryDelete();
            }
            else backend->tryDelete();
        }

        applySources(folder, data.files, &urls);

        foreach (const WControllerPlaylistMedia & media, data.medias)
        {
            if (urlTracks.count() == CONTROLLERPLAYLIST_MAX_TRACKS) break;

            const QString & url = media.url;

            if (urlTracks.contains(url)) continue;

            urlTracks.append(url);

            WTrack track(url, WTrack::Default);

            track.setTitle(media.title);

            QString cover = media.cover;

            if (cover.isEmpty())
            {
                 track.setCover(data.cover);
            }
            else track.setCover(cover);

            track.setFeed(urlQuery);

            playlist->addTrack(track);
        }

        if (WControllerNetwork::urlIsFile(folder->source()))
        {
            applyCurrentIndex(playlist);
        }
    }
    // NOTE: When having a single track we try to load related tracks.
    else if (playlist->count() == 1)
    {
        const QString & url = playlist->d_func()->itemAt(0)->source();

        urlTracks.append(url);
    }

    //---------------------------------------------------------------------------------------------
    // Next query

    emit playlist->queryEnded();

    if (source.isEmpty() == false)
    {
        WBackendNet * backend = q->backendFromUrl(source);

        if (backend)
        {
            QString backendId = backend->id();

            WBackendNetQuery query = backend->getQueryPlaylist(source);

            backend->tryDelete();

            if (resolvePlaylist(backendId, query))
            {
                getDataPlaylist(playlist, query);

                playlist->tryDelete();

                folder->d_func()->setQueryFinished();

                return;
            }
        }
    }

    if (urlTracks.count() == 1)
    {
        int index = playlist->count() - 1;

        WBackendNet * backend = backendTrack(source, playlist, index);

        if (backend)
        {
            QString id = backend->getTrackId(source);

            if (id.isEmpty() == false)
            {
                playlist->loadTrack(index);

                playlist->d_func()->setQueryLoaded();

                getDataRelated(backend, playlist, id);

                backend->tryDelete();

                playlist->tryDelete();

                folder->d_func()->setQueryFinished();

                return;
            }
        }
    }
    // NOTE: Clearing the default playlist when it's empty and we have other playlist(s).
    else if (folder->count() > 1 && playlist->isEmpty())
    {
        folder->loadCurrentIndex(1, true);

        folder->removeItem(playlist->id());
    }

    // NOTE: We reload the cover in case it changed since the last time.
    WControllerPlaylist::reloadCover(folder->cover());

    playlist->d_func()->setQueryLoaded();

    playlist->tryDelete();

    folder->d_func()->setQueryFinished();
}

void WControllerPlaylistPrivate::onUrlItem(QIODevice                     * device,
                                           const WControllerPlaylistItem & data)
{
    WControllerPlaylistQuery * query = replies.take(device);

    device->deleteLater();

    WLibraryItem * item = query->item;

    deleteQuery(query);

    // NOTE: The item was cleared so we return right away.
    if (item == NULL) return;

    emit item->queryData(data.data, data.extension);

    emit item->queryEnded();

    // NOTE: Maybe other queries are still loading.
    if (checkItem(item)) return;

    item->d_func()->setQueryLoaded();
}

//=================================================================================================
// WControllerPlaylist
//=================================================================================================
// Private

WControllerPlaylist::WControllerPlaylist() : WController(new WControllerPlaylistPrivate(this)) {}

//-------------------------------------------------------------------------------------------------
// Initialize
//-------------------------------------------------------------------------------------------------

/* virtual */ void WControllerPlaylist::init()
{
    Q_D(WControllerPlaylist); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ WRemoteData * WControllerPlaylist::getData(WAbstractLoader        * loader,
                                                             const WBackendNetQuery & query,
                                                             QObject                * parent) const
{
    Q_D(const WControllerPlaylist);

    if (loader == NULL)
    {
        loader = d->loaders.value(query.type);
    }

    return WControllerPlaylist::getDataQuery(loader, query, parent);
}

/* Q_INVOKABLE */ WRemoteData * WControllerPlaylist::getData(const WBackendNetQuery & query,
                                                             QObject                * parent) const
{
    Q_D(const WControllerPlaylist);

    WAbstractLoader * loader = d->loaders.value(query.type);

    return WControllerPlaylist::getDataQuery(loader, query, parent);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ WLibraryItem * WControllerPlaylist::getLibraryItem(const QList<int> & idFull)
{
    if (idFull.isEmpty()) return NULL;

    Q_D(WControllerPlaylist);

    QList<int> ids = idFull;

    int id = ids.takeFirst();

    WLibraryFolder * folder = d->getFolderRoot(id);

    if (ids.isEmpty() || folder == NULL)
    {
        return folder;
    }

    while (folder)
    {
        id = ids.takeFirst();

        WLibraryItem * item = folder->getLibraryItemFromId(id);

        if (ids.isEmpty() || item == NULL)
        {
            return item;
        }

        folder = item->toFolder();
    }

    return NULL;
}

/* Q_INVOKABLE */ void WControllerPlaylist::loadLibraryItem(const QList<int> & idFull)
{
    if (idFull.isEmpty()) return;

    Q_D(WControllerPlaylist);

    WLibraryFolder * folder = d->getFolderRoot(idFull.first());

    if (folder == NULL) return;

    QList<int> id = idFull;

    folder = d->getFolder(folder, &id);

    if (folder == NULL) return;

    WControllerPlaylistLoader * loader = new WControllerPlaylistLoader(folder, id, this);

    loader->load();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ WBackendNet * WControllerPlaylist::backendFromId(const QString & id) const
{
    Q_D(const WControllerPlaylist);

    WBackendNet * backend = WBackendLoader::getBackend(id);

    if (backend) return backend;

    foreach (WBackendLoader * loader, d->backendLoaders)
    {
        backend = loader->createNow(id);

        if (backend) return backend;
    }

    return NULL;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ WBackendNet * WControllerPlaylist::backendFromUrl(const QString & url) const
{
    if (WBackendNet::checkQuery(url))
    {
        QUrl source(url);

#ifdef QT_4
        QString backend = source.queryItemValue("backend");
#else
        QString backend = QUrlQuery(source).queryItemValue("backend");
#endif

        return backendFromId(backend);
    }
    else return backendFromSource(url);
}

/* Q_INVOKABLE */ WBackendNet * WControllerPlaylist::backendFromSource(const QString & url) const
{
    Q_D(const WControllerPlaylist);

    QString source = WControllerPlaylist::simpleSource(url).toLower();

    foreach (WBackendLoader * loader, d->backendLoaders)
    {
        QString id = loader->getId(source);

        if (id.isEmpty() == false)
        {
            return loader->create(id);
        }
    }

    return NULL;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ WBackendNet * WControllerPlaylist::backendFromTrack(const QString & url) const
{
    WBackendNet * backend = backendFromSource(url);

    if (backend == NULL) return NULL;

    if (backend->getTrackId(url).isEmpty())
    {
        backend->tryDelete();

        return NULL;
    }
    else return backend;
}

/* Q_INVOKABLE */ WBackendNet * WControllerPlaylist::backendFromPlaylist(const QString & url) const
{
    WBackendNet * backend = backendFromSource(url);

    if (backend == NULL) return NULL;

    if (backend->getPlaylistInfo(url).isValid() == false)
    {
        backend->tryDelete();

        return NULL;
    }
    else return backend;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ WBackendNet * WControllerPlaylist::backendSearch() const
{
    Q_D(const WControllerPlaylist);

    foreach (WBackendLoader * loader, d->backendLoaders)
    {
        WBackendNet * backend = loader->backendSearch();

        if (backend) return backend;
    }

    return NULL;
}

/* Q_INVOKABLE */ WBackendNet * WControllerPlaylist::backendForCover(const QString & label,
                                                                     const QString & q) const
{
    Q_D(const WControllerPlaylist);

    foreach (WBackendLoader * loader, d->backendLoaders)
    {
        WBackendNet * backend = loader->matchCover(label, q);

        if (backend) return backend;
    }

    return NULL;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ QString WControllerPlaylist::backendIdFromSource(const QString & url) const
{
    WBackendNet * backend = backendFromSource(url);

    if (backend)
    {
        QString id = backend->id();

        backend->tryDelete();

        return id;
    }
    else return QString();
}

/* Q_INVOKABLE */ QString WControllerPlaylist::backendCoverFromId(const QString & id) const
{
    Q_D(const WControllerPlaylist);

    foreach (WBackendLoader * loader, d->backendLoaders)
    {
        QString cover = loader->coverFromId(id);

        if (cover.isEmpty() == false)
        {
            return cover;
        }
    }

    return QString();
}

/* Q_INVOKABLE */ QString WControllerPlaylist::backendCoverFromUrl(const QString & url) const
{
    Q_D(const WControllerPlaylist);

    foreach (WBackendLoader * loader, d->backendLoaders)
    {
        QString cover = loader->coverFromUrl(url);

        if (cover.isEmpty() == false)
        {
            return cover;
        }
    }

    return QString();
}

/* Q_INVOKABLE */ QString WControllerPlaylist::backendHubFromId(const QString & id) const
{
    Q_D(const WControllerPlaylist);

    foreach (WBackendLoader * loader, d->backendLoaders)
    {
        QString hub = loader->hubFromId(id);

        if (hub.isEmpty() == false)
        {
            return hub;
        }
    }

    return QString();
}

/* Q_INVOKABLE */ QString WControllerPlaylist::backendHubFromUrl(const QString & url) const
{
    Q_D(const WControllerPlaylist);

    foreach (WBackendLoader * loader, d->backendLoaders)
    {
        QString hub = loader->hubFromUrl(url);

        if (hub.isEmpty() == false)
        {
            return hub;
        }
    }

    return QString();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ WLibraryItem::Type WControllerPlaylist::urlType(const QString & url) const
{
    WBackendNet * backend = backendFromUrl(url);

    if (backend == NULL) return WLibraryItem::Item;

    WBackendNetPlaylistInfo info = backend->getPlaylistInfo(url);

    backend->tryDelete();

    if (info.isValid())
    {
         return info.type;
    }
    else return WLibraryItem::Item;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ bool WControllerPlaylist::urlIsTrack(const QString & url) const
{
    WBackendNet * backend = backendFromTrack(url);

    if (backend)
    {
        backend->tryDelete();

        return true;
    }
    else return urlIsMedia(url);
}

/* Q_INVOKABLE */ bool WControllerPlaylist::urlIsTrackOnly(const QString & url) const
{
    WBackendNet * backend = backendFromSource(url);

    if (backend == NULL) return false;

    if (backend->getPlaylistInfo(url).isValid())
    {
        backend->tryDelete();

        return false;
    }
    else if (backend->getTrackId(url).isEmpty())
    {
        backend->tryDelete();

        return false;
    }
    else
    {
        backend->tryDelete();

        return true;
    }
}

/* Q_INVOKABLE */ bool WControllerPlaylist::urlIsPlaylist(const QString & url) const
{
    WBackendNet * backend = backendFromPlaylist(url);

    if (backend)
    {
        backend->tryDelete();

        return true;
    }
    else return false;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ bool WControllerPlaylist::sourceIsVideo(const QString & url) const
{
    if (sourceIsAudio(url))
    {
         return false;
    }
    else return true;
}

/* Q_INVOKABLE */ bool WControllerPlaylist::sourceIsAudio(const QString & url) const
{
    if (urlIsAudio(url) == false)
    {
        WBackendNet * backend = backendFromUrl(url);

        if (backend)
        {
            WAbstractBackend::Output output = backend->getTrackOutput(url);

            backend->tryDelete();

            if (output == WAbstractBackend::OutputAudio)
            {
                 return true;
            }
            else return false;
        }
        else return false;
    }
    else return true;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ QString WControllerPlaylist::sourceRelatedTracks(const QString & url,
                                                                   const QString & title) const
{
    WBackendNet * backend = backendFromUrl(url);

    if (backend)
    {
        QString id = backend->id();

        QString trackId = backend->getTrackId(url);

        if (trackId.isEmpty()) trackId = url;

        WBackendNetQuery query = backend->createQuery("related", "tracks", trackId);

        backend->tryDelete();

        if      (query.isValid()) return createSource(id, "related", "tracks", trackId);
        else if (title.isEmpty()) return createSource(id, "related", "tracks", url);
        else                      return createSource(id, "related", "tracks", title);
    }

    if (url.isEmpty()) return QString();

    // NOTE: When we can't find a backend we load the url as a VBML resource and try to extract
    //       the 'related' property.
    return createSource("vbml", "related", "tracks", url);
}

/* Q_INVOKABLE */ WBackendNetQuery WControllerPlaylist::queryPlaylist(const QString & url) const
{
    WBackendNet * backend = backendFromUrl(url);

    if (backend)
    {
        WBackendNetQuery query = backend->getQueryPlaylist(url);

        backend->tryDelete();

        return query;
    }

    if (url.isEmpty()) return WBackendNetQuery();

    WBackendNetQuery query(url);

    query.target = WBackendNetQuery::TargetHtml;
    query.scope  = WAbstractLoader::ScopeText;

    return query;
}

/* Q_INVOKABLE */
WBackendNetQuery WControllerPlaylist::queryRelatedTracks(const QString & url,
                                                         const QString & title,
                                                         const QString & prefix) const
{
    WBackendNet * backend = backendFromUrl(url);

    if (backend)
    {
        QString id = backend->getTrackId(url);

        WBackendNetQuery query = backend->createQuery("related", "tracks", id);

        if (query.isValid())
        {
            backend->tryDelete();

            return query;
        }

        //-----------------------------------------------------------------------------------------
        // NOTE: When we fail to create a valid query we try to create one with the backendSearch.

        id = backend->getId();

        QString host = backend->getHost();

        backend->tryDelete();

        backend = backendSearch();

        if (backend == NULL || title.isEmpty()) return query;

        if (host.isEmpty())
        {
             query = backend->createQuery("related", "tracks", id + " " + title);
        }
        else query = backend->createQuery("related", "tracks", prefix + host + " " + title);

        backend->tryDelete();

        return query;
    }

    if (url.isEmpty()) return WBackendNetQuery();

    Q_D(const WControllerPlaylist);

    // NOTE: When we can't find a backend we load the url as a VBML resource and try to extract
    //       the 'related' property.
    QString source = createSource("vbml", "related", "tracks", url);

    return d->extractRelated(source);

}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WControllerPlaylist::abortFolderItems()
{
    Q_D(WControllerPlaylist);

    foreach (WLibraryFolder * folder, d->folders)
    {
        folder->abortItems();
    }
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WControllerPlaylist::abortQueries()
{
    Q_D(WControllerPlaylist);

    foreach (WControllerPlaylistQuery * query, d->queries)
    {
        WLibraryItem * item = query->item;

        d->removeQuery(query);

        if (item) item->d_func()->setQueryLoading(false);
    }
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WControllerPlaylist::registerLoader(WBackendNetQuery::Type type,
                                                           WAbstractLoader      * loader)
{
    Q_D(WControllerPlaylist);

    d->loaders.insert(type, loader);
}

/* Q_INVOKABLE */ void WControllerPlaylist::unregisterLoader(WBackendNetQuery::Type type)
{
    Q_D(WControllerPlaylist);

    d->loaders.remove(type);
}

/* Q_INVOKABLE */ void WControllerPlaylist::unregisterLoaders()
{
    Q_D(WControllerPlaylist);

    d->loaders.clear();
}

//-------------------------------------------------------------------------------------------------
// Static functions
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ void WControllerPlaylist::reloadCover(const QString & url, int delay,
                                                                                    int maxHost)
{
#ifndef SK_CONSOLE
    WPixmapCache::removePath(url);
#endif

    wControllerFile->reloadFile(url, delay, maxHost);
}

/* Q_INVOKABLE static */ QString WControllerPlaylist::generateSource(const QString & url)
{
    QString source = WControllerNetwork::decodeUrl(url);

    if (WControllerNetwork::urlIsFile(source))
    {
        return url;
    }
    else if (WControllerNetwork::urlIsHttp(source))
    {
        source = WControllerNetwork::removeUrlPrefix(source);

        if (source.startsWith("vbml.", Qt::CaseInsensitive))
        {
            return WControllerPlaylist::vbmlUriFromUrl(source);
        }
        else return url;
    }
#ifdef Q_OS_WIN
    else if (source.startsWith('/') || source.startsWith('\\')
             ||
             (source.length() > 1 && source.at(1) == ':'))
#else
    else if (source.startsWith('/') || (source.length() > 1 && source.at(1) == ':'))
#endif
    {
        source = QDir::fromNativeSeparators(source);

        if (source.endsWith(':'))
        {
             return WControllerFile::fileUrl(source + '/');
        }
        else return WControllerFile::fileUrl(source);
    }
    else if (QUrl(url).scheme().isEmpty())
    {
        return "https://" + url;
    }
    else return url;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerPlaylist::simpleSource(const QString & url)
{
    int indexA = url.indexOf(":");

    if (indexA == -1) return url;

    int indexB = url.indexOf("//", indexA);

    if (indexA != (indexB - 1)) return url;

    indexA = indexB + 2;

    while (indexA < url.length() && url.at(indexA) == '/')
    {
        indexA++;
    }

    indexB = url.indexOf("www.", indexA);

    if (indexB != -1 && indexB == indexA)
    {
        indexA += 4;
    }
    else if (indexA == -1)
    {
        indexB = url.indexOf(WRegExp("[\\?#]"));

        if (indexB == -1)
        {
             return url;
        }
        else return url.mid(0, indexB);
    }

    indexB = url.indexOf(WRegExp("[\\?#]"), indexA);

    if (indexB == -1)
    {
         return url.mid(indexA);
    }
    else return url.mid(indexA, indexB - indexA);
}

/* Q_INVOKABLE static */ QString WControllerPlaylist::simpleQuery(const QString & text)
{
    QString result = text;

    result.replace(WRegExp("[.:]"), " ");

    return result.simplified();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerPlaylist::createSource(const QString & backend,
                                                                   const QString & method,
                                                                   const QString & label,
                                                                   const QString & q)
{
    QUrl source("vbml:run");

#ifdef QT_4
    source.addQueryItem("backend", backend);
    source.addQueryItem("method",  method);
    source.addQueryItem("label",   label);

    if (q.isEmpty() == false)
    {
        source.addQueryItem("q", QUrl::toPercentEncoding(q, QByteArray(), "?&"));
    }
#else
    QUrlQuery query(source);

    query.addQueryItem("backend", backend);
    query.addQueryItem("method",  method);
    query.addQueryItem("label",   label);

    if (q.isEmpty() == false)
    {
        query.addQueryItem("q", QUrl::toPercentEncoding(q, QByteArray(), "?&"));
    }

    source.setQuery(query);
#endif

    return source.toString();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerPlaylist::cleanSource(const QString & url)
{
    int indexA = url.indexOf('#');

    if (indexA == -1) return url;

    int indexB = indexA + 1;

    // NOTE: If we have a digit it's probably a file index that we want to keep.
    if (indexB < url.length() && url.at(indexB).isDigit())
    {
        indexB = url.indexOf('&', indexB);

        if (indexB == -1)
        {
             return url;
        }
        else return url.mid(0, indexB);
    }
    else return url.mid(0, indexA);
}

/* Q_INVOKABLE static */ bool WControllerPlaylist::cleanMatch(const QString & urlA,
                                                              const QString & urlB)
{
    return (cleanSource(urlA) == cleanSource(urlB));
}

/* Q_INVOKABLE static */ int WControllerPlaylist::extractTime(const QString & string,
                                                              int             defaultValue)
{
    QString time = WControllerNetwork::extractFragmentValue(string, "t");

    if (time.isEmpty()) return defaultValue;

    // NOTE: We want the time in milliseconds.
    return time.toInt() * 1000;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */
WControllerFileReply * WControllerPlaylist::copyBackends(const QString & path,
                                                         const QString & newPath)
{
    QStringList fileNames;
    QStringList newNames;

    QFileInfoList list = QDir(path).entryInfoList(QDir::Files);

    foreach (QFileInfo info, list)
    {
        if (info.suffix().toLower() != "vbml") continue;

        fileNames.append(info.filePath());

        newNames.append(newPath + info.fileName());
    }

    if (QFile::exists(newPath))
    {
         wControllerFile->startDeleteFolderContent(newPath);
    }
    else wControllerFile->startCreateFolder(newPath);

#ifdef Q_OS_UNIX
    // NOTE Unix: We need to make sure we can write on these files.
    return wControllerFile->startCopyFiles(fileNames, newNames, WControllerFile::ReadOwner |
                                                                WControllerFile::WriteOwner);
#else
    return wControllerFile->startCopyFiles(fileNames, newNames);
#endif
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */
WRemoteData * WControllerPlaylist::getDataQuery(WAbstractLoader        * loader,
                                                const WBackendNetQuery & query, QObject * parent)
{
    WRemoteParameters parameters;

    parameters.scope = query.scope;

    parameters.header = query.header;
    parameters.body   = query.body;

    parameters.cookies = query.cookies;

    parameters.maxHost = query.maxHost;
    parameters.delay   = query.delay;
    parameters.timeout = query.timeout;

    return wControllerDownload->getData(loader, query.url, parent, query.priority, parameters);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerPlaylist::getFeed(const QString & feed,
                                                              const QString & url)
{
    if (feed.isEmpty()) return QString();

    if (WControllerNetwork::urlScheme(feed) == "")
    {
        const QString & host = WControllerNetwork::extractUrlHost(url);

        return WControllerNetwork::resolveUrl(feed, host);
    }
    else return feed;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerPlaylist::getPlayerTime(int msec, int max)
{
    if (msec < 1) return "0:00";

#ifdef QT_4
    QTime time;

    time = time.addMSecs(msec);
#else
    QTime time = QTime::fromMSecsSinceStartOfDay(msec);
#endif

    QString string;

    if (time.hour())
    {
         string = time.toString("h:mm:ss");
    }
    else string = time.toString("m:ss");

    if (max != -1) string.truncate(max);

    return string;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QStringList WControllerPlaylist::getLanguages()
{
    QStringList languages;

    languages.append(tr("Afrikaans"));
    languages.append(tr("Albanian"));
    languages.append(tr("Arabic"));
    languages.append(tr("Aragonese"));
    languages.append(tr("Asturian"));
    languages.append(tr("Azerbaijani"));
    languages.append(tr("Basque"));
    languages.append(tr("Belarusian"));
    languages.append(tr("Bosnian"));
    languages.append(tr("Breton"));
    languages.append(tr("Bulgarian"));
    languages.append(tr("Burmese"));
    languages.append(tr("Catalan"));
    languages.append(tr("Chinese"));
    languages.append(tr("Chinese (TR)"));
    languages.append(tr("Chinese (BI)"));
    languages.append(tr("Croatian"));
    languages.append(tr("Czech"));
    languages.append(tr("Danish"));
    languages.append(tr("Dutch"));
    languages.append(tr("English"));
    languages.append(tr("Esperanto"));
    languages.append(tr("Estonian"));
    languages.append(tr("Finnish"));
    languages.append(tr("French"));
    languages.append(tr("Gaelic"));
    languages.append(tr("Galician"));
    languages.append(tr("Georgian"));
    languages.append(tr("German"));
    languages.append(tr("Greek"));
    languages.append(tr("Hebrew"));
    languages.append(tr("Hindi"));
    languages.append(tr("Hungarian"));
    languages.append(tr("Icelandic"));
    languages.append(tr("Indonesian"));
    languages.append(tr("Irish"));
    languages.append(tr("Italian"));
    languages.append(tr("Japanese"));
    languages.append(tr("Kannada"));
    languages.append(tr("Kazakh"));
    languages.append(tr("Khmer"));
    languages.append(tr("Korean"));
    languages.append(tr("Kurdish"));
    languages.append(tr("Latvian"));
    languages.append(tr("Lithuanian"));
    languages.append(tr("Macedonian"));
    languages.append(tr("Malay"));
    languages.append(tr("Malayalam"));
    languages.append(tr("Norwegian"));
    languages.append(tr("Occitan"));
    languages.append(tr("Persian"));
    languages.append(tr("Polish"));
    languages.append(tr("Portuguese"));
    languages.append(tr("Portuguese (BR)"));
    languages.append(tr("Portuguese (MZ)"));
    languages.append(tr("Romanian"));
    languages.append(tr("Russian"));
    languages.append(tr("Serbian"));
    languages.append(tr("Sinhala"));
    languages.append(tr("Slovak"));
    languages.append(tr("Slovenian"));
    languages.append(tr("Spanish"));
    languages.append(tr("Spanish (EU)"));
    languages.append(tr("Spanish (LA)"));
    languages.append(tr("Swedish"));
    languages.append(tr("Tamil"));
    languages.append(tr("Telugu"));
    languages.append(tr("Thai"));
    languages.append(tr("Turkish"));
    languages.append(tr("Ukranian"));
    languages.append(tr("Vietnamese"));

    return languages;
}

//-------------------------------------------------------------------------------------------------
// Filters

/* Q_INVOKABLE static */ QString WControllerPlaylist::getFilterFile()
{
    return CONTROLLERPLAYLIST_FILTER_FILE;
}

/* Q_INVOKABLE static */ QString WControllerPlaylist::getFilterSubtitle()
{
    return CONTROLLERPLAYLIST_FILTER_SUBTITLE;
}

//-------------------------------------------------------------------------------------------------
// Texts

/* Q_INVOKABLE static */ bool WControllerPlaylist::textIsRedirect(const QString & text,
                                                                  const QString & baseUrl)
{
    // NOTE: We redirect when the text is a vbml uri or a url that's different from the baseUrl.
    return ((urlIsVbmlUri(text) && text != baseUrl)
            ||
            (WControllerNetwork::textIsUrl(text) && WControllerNetwork::removeUrlPrefix(text)
                                                    !=
                                                    WControllerNetwork::removeUrlPrefix(baseUrl)));
}

//-------------------------------------------------------------------------------------------------
// Urls

/* Q_INVOKABLE static */ bool WControllerPlaylist::urlIsMedia(const QString & url)
{
    QString extension = WControllerNetwork::extractUrlExtension(url);

    return extensionIsMedia(extension);
}

/* Q_INVOKABLE static */ bool WControllerPlaylist::urlIsVideo(const QString & url)
{
    QString extension = WControllerNetwork::extractUrlExtension(url);

    return extensionIsVideo(extension);
}

/* Q_INVOKABLE static */ bool WControllerPlaylist::urlIsAudio(const QString & url)
{
    QString extension = WControllerNetwork::extractUrlExtension(url);

    return extensionIsAudio(extension);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ bool WControllerPlaylist::urlIsAscii(const QString & url)
{
    QString extension = WControllerNetwork::extractUrlExtension(url);

    return extensionIsAscii(extension);
}

/* Q_INVOKABLE static */ bool WControllerPlaylist::urlIsMarkup(const QString & url)
{
    QString extension = WControllerNetwork::extractUrlExtension(url);

    return extensionIsMarkup(extension);
}

/* Q_INVOKABLE static */ bool WControllerPlaylist::urlIsText(const QString & url)
{
    QString extension = WControllerNetwork::extractUrlExtension(url);

    return extensionIsText(extension);
}

/* Q_INVOKABLE static */ bool WControllerPlaylist::urlIsSubtitle(const QString & url)
{
    QString extension = WControllerNetwork::extractUrlExtension(url);

    return extensionIsSubtitle(extension);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ bool WControllerPlaylist::urlIsM3u(const QString & url)
{
    QString extension = WControllerNetwork::extractUrlExtension(url);

    return extensionIsM3u(extension);
}

#ifndef SK_NO_TORRENT

/* Q_INVOKABLE static */ bool WControllerPlaylist::urlIsTorrent(const QString & url)
{
    if (url.startsWith("magnet:?", Qt::CaseInsensitive)) return true;

    QString extension = WControllerNetwork::extractUrlExtension(url);

    return extensionIsTorrent(extension);
}

#endif

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ bool WControllerPlaylist::urlIsVbml(const QString & url)
{
    if (urlIsVbmlFile(url) || urlIsVbmlUri(url)) return true;

    QString source = WControllerNetwork::removeUrlPrefix(url);

    return source.startsWith("vbml.", Qt::CaseInsensitive);
}

/* Q_INVOKABLE static */ bool WControllerPlaylist::urlIsVbmlFile(const QString & url)
{
    QString extension = WControllerNetwork::extractUrlExtension(url);

    return extensionIsVbml(extension);
}

/* Q_INVOKABLE static */ bool WControllerPlaylist::urlIsVbmlUri(const QString & url)
{
    return url.startsWith("vbml:", Qt::CaseInsensitive);
}

/* Q_INVOKABLE static */ bool WControllerPlaylist::urlIsVbmlRun(const QString & url)
{
    return url.startsWith("vbml:run", Qt::CaseInsensitive);
}

/* Q_INVOKABLE static */ bool WControllerPlaylist::urlIsVbmlConnect(const QString & url)
{
    QString source = WControllerNetwork::removeUrlPrefix(url);

    if (source.startsWith("vbml:", Qt::CaseInsensitive))
    {
        return (source.indexOf("connect/", 5) == 5);
    }
    else if (source.startsWith("vbml.", Qt::CaseInsensitive))
    {
        int index = source.indexOf('/', 5);

        if (index == -1) return false;

        index++;

        source = WControllerNetwork::extractUrlElement(source, index);

        return (source.toLower() == "connect");
    }
    else return false;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ bool WControllerPlaylist::extensionIsMedia(const QString & extension)
{
    return (extensionIsVideo(extension) || extensionIsAudio(extension));
}

/* Q_INVOKABLE static */ bool WControllerPlaylist::extensionIsVideo(const QString & extension)
{
    return (extension.indexOf(WRegExp(CONTROLLERPLAYLIST_VIDEO)) != -1);
}

/* Q_INVOKABLE static */ bool WControllerPlaylist::extensionIsAudio(const QString & extension)
{
    return (extension.indexOf(WRegExp(CONTROLLERPLAYLIST_AUDIO)) != -1);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ bool WControllerPlaylist::extensionIsAscii(const QString & extension)
{
    return (extensionIsMarkup(extension) || extensionIsText(extension));
}

/* Q_INVOKABLE static */ bool WControllerPlaylist::extensionIsMarkup(const QString & extension)
{
    return (extension.indexOf(WRegExp(CONTROLLERPLAYLIST_MARKUP)) != -1);
}

/* Q_INVOKABLE static */ bool WControllerPlaylist::extensionIsText(const QString & extension)
{
    return (extension.indexOf(WRegExp(CONTROLLERPLAYLIST_TEXT)) != -1);
}

/* Q_INVOKABLE static */ bool WControllerPlaylist::extensionIsSubtitle(const QString & extension)
{
    return (extension.indexOf(WRegExp(CONTROLLERPLAYLIST_SUBTITLE)) != -1);
}

//---------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ bool WControllerPlaylist::extensionIsM3u(const QString & extension)
{
    return (extension.indexOf(WRegExp(CONTROLLERPLAYLIST_M3U)) != -1);
}

#ifndef SK_NO_TORRENT

/* Q_INVOKABLE static */ bool WControllerPlaylist::extensionIsTorrent(const QString & extension)
{
    return (extension.indexOf(WRegExp(CONTROLLERPLAYLIST_TORRENT)) != -1);
}

#endif

/* Q_INVOKABLE static */ bool WControllerPlaylist::extensionIsVbml(const QString & extension)
{
    return (extension.indexOf(WRegExp(CONTROLLERPLAYLIST_VBML)) != -1);
}

//---------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */
QStringList WControllerPlaylist::extractTracks(const WControllerPlaylistData & data)
{
    QStringList list;

    foreach (const WControllerPlaylistSource & source, data.sources)
    {
        const QString & url = source.url;

        WBackendNet * backend = wControllerPlaylist->backendFromUrl(url);

        if (backend == NULL) continue;

        QString id = backend->getTrackId(url);

        if (id.isEmpty())
        {
            backend->tryDelete();

            continue;
        }

        QString urlTrack = backend->getUrlTrack(id);

        backend->tryDelete();

        if (list.contains(urlTrack)) continue;

        list.append(urlTrack);

        if (list.count() == CONTROLLERPLAYLIST_MAX_TRACKS)
        {
            return list;
        }
    }

    foreach (const WControllerPlaylistMedia & media, data.medias)
    {
        const QString & url = media.url;

        if (list.contains(url)) continue;

        list.append(url);

        if (list.count() == CONTROLLERPLAYLIST_MAX_TRACKS)
        {
            return list;
        }
    }

    return list;
}

/* Q_INVOKABLE static */ QList<WControllerPlaylistUrl>
WControllerPlaylist::extractPlaylists(const WControllerPlaylistData & data)
{
    QStringList list;

    QList<WControllerPlaylistUrl> items;

    foreach (const WControllerPlaylistSource & source, data.sources)
    {
        const QString & url = source.url;

        WBackendNet * backend = wControllerPlaylist->backendFromUrl(url);

        if (backend == NULL) continue;

        WBackendNetPlaylistInfo info = backend->getPlaylistInfo(url);

        if (info.isValid() == false)
        {
            backend->tryDelete();

            continue;
        }

        QString urlPlaylist = backend->getUrlPlaylist(info);

        backend->tryDelete();

        if (list.contains(urlPlaylist)) continue;

        list.append(urlPlaylist);

        WControllerPlaylistUrl item;

        item.type = info.type;

        item.url = urlPlaylist;

        items.append(item);

        if (items.count() == CONTROLLERPLAYLIST_MAX_ITEMS)
        {
            return items;
        }
    }

    return items;
}

//---------------------------------------------------------------------------------------------
// QML

/* Q_INVOKABLE static */ WLibraryFolder * WControllerPlaylist::createFolder(int type)
{
    return WLibraryFolder::create(static_cast<WLibraryItem::Type> (type));
}

/* Q_INVOKABLE static */ WPlaylist * WControllerPlaylist::createPlaylist(int type)
{
    return WPlaylist::create(static_cast<WLibraryItem::Type> (type));
}

//---------------------------------------------------------------------------------------------
// VBML

/* Q_INVOKABLE static */ QString WControllerPlaylist::vbml(const QString & append)
{
    QString vbml;

    Sk::bmlVersion(vbml, "vbml", versionApi(), append);

    return vbml;
}

/* Q_INVOKABLE static */ bool WControllerPlaylist::vbmlCheck(const QString & data)
{
    return (vbmlHeader(data) != -1);
}

/* Q_INVOKABLE static */ int WControllerPlaylist::vbmlHeader(const QString & vbml)
{
    int index = vbml.indexOf('#');

    if (index == -1) return -1;

    QString line = Sk::getLine(vbml, index).simplified();

    int at = 1;

    while (at < line.length() && line.at(at) == ' ') at++;

    if (line.mid(at, 4).toLower() == "vbml")
    {
        return index;
    }
    else return -1;
}

/* Q_INVOKABLE static */ QString WControllerPlaylist::vbmlVersion(const QString & vbml)
{
    int index = vbml.indexOf('#');

    if (index == -1) return QString();

    QString line = Sk::getLine(vbml, index).simplified();

    index = 1;

    while (index < line.length() && line.at(index) == ' ') index++;

    if (line.mid(index, 4).toLower() != "vbml") return QString();

    index += 5;

    QString version;

    while (index < line.length())
    {
        QChar character = line.at(index);

        if (character.isDigit() == false && character != '.')
        {
            return version;
        }

        version.append(character);

        index++;
    }

    return version;
}

/* Q_INVOKABLE static */
WControllerPlaylist::Type WControllerPlaylist::vbmlType(const QString & vbml)
{
    int index = vbml.indexOf("type", Qt::CaseInsensitive);

    if (index == -1) return Unknown;

    QString line = Sk::getLine(vbml, index);

    index = line.indexOf(':', 4);

    if (index == -1) return Unknown;

    index++;

    line = line.mid(index, line.length() - index).simplified();

    return vbmlTypeFromString(line.toLower());
}

/* Q_INVOKABLE static */ int WControllerPlaylist::vbmlDuration(const WYamlNodeBase & node,
                                                               int                   at,
                                                               int                   defaultValue)
{
    int duration = node.extractMsecs("duration", -1);

    if (duration != -1) return duration;

    duration = node.extractMsecs("end", -1);

    if (duration == -1)
    {
        return defaultValue;
    }
    else return duration - at;
}

/* Q_INVOKABLE static */ int WControllerPlaylist::vbmlDurationSource(const WYamlNode & node,
                                                                     int               at,
                                                                     int               defaultValue)
{
    const QList<WYamlNode> & children = node.children;

    if (children.isEmpty()) return defaultValue;

    int duration = 0;

    foreach (const WYamlNode & child, children)
    {
        int durationSource = vbmlDuration(child, child.extractMsecs("at"));

        duration += durationSource;
    }

    if (duration == 0)
    {
        return defaultValue;
    }
    else return duration - at;
}

/* Q_INVOKABLE static */
WControllerPlaylist::Type WControllerPlaylist::vbmlTypeFromString(const QString & string)
{
    if      (string == "track")    return Track;
    else if (string == "live")     return Live;
    else if (string == "hub")      return Hub;
    else if (string == "channel")  return Channel;
    else if (string == "playlist") return Playlist;
    else if (string == "feed")     return Feed;
    else if (string == "index")    return Index;
    else if (string == "backend")  return Backend;
    else                           return Unknown;
}

/* Q_INVOKABLE static */ bool WControllerPlaylist::vbmlTypeTrack(Type type)
{
    return (type == Track || type == Live || type == Hub || type == Channel);
}

/* Q_INVOKABLE static */ bool WControllerPlaylist::vbmlTypePlaylist(Type type)
{
    return (type == Playlist || type == Feed);
}

/* Q_INVOKABLE static */ void WControllerPlaylist::vbmlPatch(QString & data, const QString & api)
{
    return WControllerPlaylist_patch(data, api);
}

/* Q_INVOKABLE static */ void WControllerPlaylist::vbmlApplyTrack(WTrack * track, const QString & uri)
{
    QByteArray data = WLoaderVbml::decode(uri);

    WYamlReader reader(data);

    QString string = reader.extractString("type");

    track->setType(WTrack::typeFromString(string));

    track->setState(WTrack::Default);

    track->setSource(reader.extractString("source"));

    track->setTitle(reader.extractString("title"));
    track->setCover(reader.extractString("cover"));

    track->setAuthor(reader.extractString("author"));
    track->setFeed  (reader.extractString("feed"));

    track->setDuration(reader.extractInt("duration", -1));

    track->setDate(reader.extractDate("date"));
}

/* Q_INVOKABLE static */ QString WControllerPlaylist::vbmlUriFromUrl(const QString & url)
{
    QString result = url;

    int index = result.indexOf("/view/");

    if (index == -1)
    {
        index = result.indexOf('/');

        if (index != -1)
        {
            result.remove(0, index + 1);
        }
    }
    else result.remove(0, index + 6);

    if (result.contains('.'))
    {
        return "vbml://" + result;
    }
    else return "vbml:" + result;
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

QThread * WControllerPlaylist::thread() const
{
    Q_D(const WControllerPlaylist); return d->thread;
}

//-------------------------------------------------------------------------------------------------

/* static */ QString WControllerPlaylist::versionApi()
{
    return "1.0.6";
}

//-------------------------------------------------------------------------------------------------

QString WControllerPlaylist::pathStorage() const
{
    return wControllerFile->pathStorage() + CONTROLLERPLAYLIST_PATH_PLAYLISTS;
}

QString WControllerPlaylist::pathStorageTabs() const
{
    return wControllerFile->pathStorage() + CONTROLLERPLAYLIST_PATH_TABS;
}

//-------------------------------------------------------------------------------------------------

#if defined(SK_DESKTOP) && defined(SK_CONSOLE) == false

bool WControllerPlaylist::associateVbml() const
{
#ifdef Q_OS_WIN
    QSettings settings("HKEY_CURRENT_USER\\Software\\Classes", QSettings::NativeFormat);

    QString fileName = QCoreApplication::applicationFilePath();

    QString value = WControllerFile::fileBaseName(fileName);

    if (settings.value(".vbml/Default") != value) return false;

    value = Sk::quote(QDir::toNativeSeparators(fileName)) + " \"%1\"";

    if (settings.value("vbml/shell/open/command/Default") != value) return false;

    return true;
#elif defined(Q_OS_MACX)
    const CFStringRef bundle = CFBundleGetIdentifier(CFBundleGetMainBundle());

    if (bundle == NULL) return false;

    Q_D(const WControllerPlaylist);

    const CFStringRef scheme = CFSTR("vbml");

    if (d->compareBundle(bundle, LSCopyDefaultHandlerForURLScheme(scheme)) == false) return false;

    bool result = false;

    const CFStringRef id = UTTypeCreatePreferredIdentifierForTag(kUTTagClassFilenameExtension,
                                                                 scheme, NULL);

    if (id)
    {
        result = d->compareBundle(bundle,
                                  LSCopyDefaultRoleHandlerForContentType(id, kLSRolesViewer));

        CFRelease(id);
    }

    return result;
#else
    return false;
#endif
}

void WControllerPlaylist::setAssociateVbml(bool associate)
{
    if (associateVbml() == associate) return;

#ifdef Q_OS_WIN
    QSettings settings("HKEY_CURRENT_USER\\Software\\Classes", QSettings::NativeFormat);

    QString fileName = QCoreApplication::applicationFilePath();

    QString name = WControllerFile::fileBaseName(fileName);

    QString path = Sk::quote(QDir::toNativeSeparators(fileName));

    QString value = path + " \"%1\"";

    if (associate)
    {
        settings.setValue(".vbml/Default",         name);
        settings.setValue(".vbml/OpenWithProgids", name);

        settings.setValue("vbml/Default", "URL:VBML link");

        settings.setValue("vbml/URL Protocol", QString());

        settings.setValue("vbml/DefaultIcon/Default", path);

        settings.setValue("vbml/shell/Default", "open");

        settings.setValue("vbml/shell/open/command/Default", value);

        emit associateVbmlChanged();
    }
    else
    {
        settings.setValue(".vbml/Default", QString());

        settings.remove("vbml");

        emit associateVbmlChanged();
    }
#elif defined(Q_OS_MACX)
    const CFStringRef bundle = CFBundleGetIdentifier(CFBundleGetMainBundle());

    if (bundle == NULL) return;

    const CFStringRef scheme = CFSTR("vbml");

    const CFStringRef id = UTTypeCreatePreferredIdentifierForTag(kUTTagClassFilenameExtension,
                                                                 scheme, NULL);

    if (associate)
    {
        LSSetDefaultHandlerForURLScheme(scheme, bundle);

        if (id)
        {
            LSSetDefaultRoleHandlerForContentType(id, kLSRolesViewer, bundle);

            CFRelease(id);
        }
    }
    else
    {
        const CFStringRef empty = CFSTR("");

        LSSetDefaultHandlerForURLScheme(scheme, empty);

        if (id)
        {
            LSSetDefaultRoleHandlerForContentType(id, kLSRolesViewer, empty);

            CFRelease(id);
        }
    }
#else
    Q_UNUSED(associate);
#endif
}

#endif

#endif // SK_NO_CONTROLLERPLAYLIST

#include "WControllerPlaylist.moc"
