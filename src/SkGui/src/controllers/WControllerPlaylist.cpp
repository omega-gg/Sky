//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkGui module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#include "WControllerPlaylist.h"

#ifndef SK_NO_CONTROLLERPLAYLIST

// Qt includes
#include <QThread>
#include <QDir>
#ifdef QT_LATEST
#include <QUrlQuery>
#endif

// Sk includes
#include <WControllerApplication>
#include <WControllerFile>
#include <WControllerNetwork>
#include <WControllerDownload>
#include <WPlaylist>
#include <WTabTrack>
#include <WCache>

// Private includes
#include <private/WPlaylist_p>

W_INIT_CONTROLLER(WControllerPlaylist)

//-------------------------------------------------------------------------------------------------
// Static variables

static const QString CONTROLLERPLAYLIST_VIDEO = "^(mp4|webm|ogv|mkv|avi|wmv|mov|flv|3gp)$";
static const QString CONTROLLERPLAYLIST_AUDIO = "^(mp3|ogg|mka|wav|wma|flac)$";

static const QString CONTROLLERPLAYLIST_MARKUP = "^(html|xml|json)$";

static const QString CONTROLLERPLAYLIST_TEXT = "^(txt|md)$";

static const QString CONTROLLERPLAYLIST_SUBTITLE = "^(srt)$";

static const QString CONTROLLERPLAYLIST_FILTER
    =
    "Media files (*.mp4 *.webm *.ogv *.mkv *.avi *.wmv *.mov *.flv *.3gp "
                  "*.mp3 *.ogg *.mka *.wav *.wma *.flac "
                  "*.html *.xml *.json *.torrent "
                  "*.txt *.md);;"
    "All files (*)";

static const QString CONTROLLERPLAYLIST_FILTER_SUBTITLE = "Subtitle files (*.srt)";

//-------------------------------------------------------------------------------------------------

static const QString CONTROLLERPLAYLIST_PATH_PLAYLISTS = "/playlists";
static const QString CONTROLLERPLAYLIST_PATH_TABS      = "/tabs";

//-------------------------------------------------------------------------------------------------

static const int CONTROLLERPLAYLIST_MAX_SIZE = 1048576 * 10; // 10 megabytes

static const int CONTROLLERPLAYLIST_MAX_TRACKS = 500;
static const int CONTROLLERPLAYLIST_MAX_ITEMS  = 500;

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

void WControllerPlaylistData::applyHtml(const QByteArray & array, const QString & url)
{
    QString charset = WControllerNetwork::extractCharset(array);

    QString content;

    if (charset.isEmpty())
    {
         content = Sk::readUtf8(array);
    }
    else content = Sk::readCodec(array, charset);

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

    QStringList urls;

    QStringList list = Sk::slices(content, "<a", "</a");

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

    this->title = title;
    this->cover = cover;
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

    QStringList list = Sk::slices(array, QRegExp("file://|http://|https://"), QRegExp("\\s"));

    QRegExp regExp("[\\s\\.:,;'\"\\)}\\]]");

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
// Private functions
//-------------------------------------------------------------------------------------------------

void WControllerPlaylistData::addSource(const QString & url, const QString & title)
{
    if (WControllerPlaylist::urlIsMedia(url))
    {
        WControllerPlaylistMedia media;

        media.url   = url;
        media.title = WControllerNetwork::extractUrlFileName(url);

        media.local = false;

        medias.append(media);
    }

    WControllerPlaylistSource source;

    source.url   = url;
    source.title = title;

    sources.append(source);
}

void WControllerPlaylistData::addFolder(const QString & url, const QString & title)
{
    WControllerPlaylistSource source;

    source.url   = url;
    source.title = title;

    folders.append(source);
}

//-------------------------------------------------------------------------------------------------

void WControllerPlaylistData::addFile(const QString & path)
{
    if (QFileInfo(path).isDir())
    {
        WControllerPlaylistSource source;

        source.url   = WControllerFile::fileUrl(path);
        source.title = WControllerNetwork::extractUrlFileName(path);

        folders.append(source);

        return;
    }

    QString extension = WControllerNetwork::extractUrlExtension(path);

    if (WControllerPlaylist::extensionIsMedia(extension))
    {
        WControllerPlaylistMedia media;

        media.url   = WControllerFile::fileUrl(path);
        media.title = WControllerNetwork::extractUrlFileName(path);

        media.local = true;

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

//-------------------------------------------------------------------------------------------------

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

QString WControllerPlaylistData::generateUrl(const QString & url, const QString & baseUrl) const
{
    QString result = url.simplified().remove(' ');

    result = WControllerNetwork::removeUrlFragment(url);

    result = WControllerNetwork::decodeUrl(url);

    result = WControllerNetwork::htmlToUtf8(result);

    result.remove("\\");

    return WControllerNetwork::generateUrl(result, baseUrl);
}

QString WControllerPlaylistData::generateTitle(const QString & url, const QString & urlName) const
{
    if (WControllerNetwork::urlName(url) == urlName)
    {
         return WControllerNetwork::extractUrlPath(url);
    }
    else return WControllerNetwork::removeUrlPrefix(url);
}

//=================================================================================================
// WControllerPlaylistReply
//=================================================================================================

class WControllerPlaylistReply : public QObject
{
    Q_OBJECT

public: // Interface
    Q_INVOKABLE void extractHtml  (QIODevice * device, const QString & url);
    Q_INVOKABLE void extractFolder(QIODevice * device, const QString & url);
    Q_INVOKABLE void extractFile  (QIODevice * device, const QString & url);

signals:
    void loaded(QIODevice * device, const WControllerPlaylistData & data);
};

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WControllerPlaylistReply::extractHtml(QIODevice     * device,
                                                             const QString & url)
{
    QByteArray array = device->readAll();

    WControllerPlaylistData data;

    data.addSlice("http");

    data.applyHtml(array, url);

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
    QByteArray array = device->readAll();

    WControllerPlaylistData data;

    data.applyFile(array, url);

    emit loaded(device, data);

    deleteLater();
}

//=================================================================================================
// WControllerPlaylistPrivate
//=================================================================================================

WControllerPlaylistPrivate::WControllerPlaylistPrivate(WControllerPlaylist * p)
    : WControllerPrivate(p) {}

/* virtual */ WControllerPlaylistPrivate::~WControllerPlaylistPrivate()
{
    foreach (WBackendNet * backend, backends)
    {
        backend->deleteLater();
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

    qRegisterMetaType<WBackendNetSource  >("WBackendNetSource");
    qRegisterMetaType<WBackendNetTrack   >("WBackendNetTrack");
    qRegisterMetaType<WBackendNetPlaylist>("WBackendNetPlaylist");
    qRegisterMetaType<WBackendNetFolder  >("WBackendNetFolder");

    qRegisterMetaType<WControllerPlaylistData>("WControllerPlaylistData");

    const QMetaObject * meta = WControllerPlaylistReply().metaObject();

    methodHtml   = meta->method(meta->indexOfMethod("extractHtml(QIODevice*,QString)"));
    methodFolder = meta->method(meta->indexOfMethod("extractFolder(QIODevice*,QString)"));
    methodFile   = meta->method(meta->indexOfMethod("extractFile(QIODevice*,QString)"));

    thread = new QThread(q);

    thread->start();

    QObject::connect(wControllerFile, SIGNAL(pathStorageChanged()),
                     q,               SIGNAL(pathStorageChanged()));
}

//-------------------------------------------------------------------------------------------------
// Private interface
//-------------------------------------------------------------------------------------------------

bool WControllerPlaylistPrivate::applyQueryTrack(WPlaylist * playlist,
                                                 WTrack    * track, const WBackendNetQuery & query)
{
    abortQueryTrack(track);

    if (query.isValid())
    {
        getDataTrack(playlist, track, query);

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

//-------------------------------------------------------------------------------------------------

bool WControllerPlaylistPrivate::applySourceTrack(WPlaylist * playlist,
                                                  WTrack    * track, const QString & url)
{
    Q_Q(WControllerPlaylist);

    abortQueryTrack(track);

    QString source = WControllerPlaylist::generateSource(url);

    WBackendNet * backend = q->backendFromUrl(source);

    if (backend)
    {
        if (backend->checkQuery(source))
        {
            WBackendNetQuery query = backend->extractQuery(source);

            if (query.isValid())
            {
                getDataTrack(playlist, track, query);

                return true;
            }
            else return false;
        }

        WBackendNetQuery query = backend->getQueryTrack(source);

        if (query.isValid())
        {
            getDataTrack(playlist, track, query);

            return true;
        }
    }

    if (q->urlIsMedia(source))
    {
        QString title = WControllerNetwork::extractUrlFileName(source);

        track->setState(WTrack::Loaded);

        track->setSource(source);
        track->setTitle (title);

        playlist->updateTrack(track);

        return true;
    }
    else return false;
}

bool WControllerPlaylistPrivate::applySourcePlaylist(WPlaylist * playlist, const QString & url)
{
    Q_Q(WControllerPlaylist);

    playlist->clearTracks();

    QString source = WControllerPlaylist::generateSource(url);

    WBackendNet * backend = q->backendFromUrl(source);

    if (backend)
    {        
        if (backend->checkQuery(source))
        {
            WBackendNetQuery query = backend->extractQuery(source);

            if (query.isValid())
            {
                getDataPlaylist(playlist, query);

                return true;
            }
            else return false;
        }

        WBackendNetQuery query = backend->getQueryPlaylist(source);

        if (query.isValid())
        {
            getDataPlaylist(playlist, query);

            return true;
        }

        QString id = backend->getTrackId(source);

        if (id.isEmpty() == false)
        {
            WTrack track(backend->getUrlTrack(id), WTrack::Default);

            playlist->addTrack(track);

            playlist->loadTrack(0);

            if (getDataRelated(backend, playlist, id) == false)
            {
                WBackendNetQuery query(source);

                query.target     = WBackendNetQuery::TargetHtml;
                query.clearItems = false;

                getDataPlaylist(playlist, query);
            }
            else playlist->d_func()->setQueryEnded();

            return true;
        }
    }

    if (WControllerNetwork::urlIsFile(source))
    {
        QFileInfo info(WControllerFile::filePath(source));

        if (info.isSymLink())
        {
            source = WControllerFile::fileUrl(info.symLinkTarget());
        }

        if (info.isDir())
        {
            WBackendNetQuery query(source);

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

                    WBackendNetQuery query(source);

                    query.target = WBackendNetQuery::TargetFolder;

                    getDataPlaylist(playlist, query);

                    return true;
                }
                else if (info.size() < CONTROLLERPLAYLIST_MAX_SIZE)
                {
                    WBackendNetQuery query(source);

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

        playlist->d_func()->setQueryEnded();

        return true;
    }

    WBackendNetQuery query(source);

    query.target = WBackendNetQuery::TargetHtml;

    getDataPlaylist(playlist, query);

    return true;
}

bool WControllerPlaylistPrivate::applySourceFolder(WLibraryFolder * folder, const QString & url)
{
    Q_Q(WControllerPlaylist);

    folder->clearItems();

    QString source = WControllerPlaylist::generateSource(url);

    WBackendNet * backend = q->backendFromUrl(source);

    if (backend)
    {
        if (backend->checkQuery(source))
        {
            WBackendNetQuery query = backend->extractQuery(source);

            if (query.isValid())
            {
                getDataFolder(folder, query);

                return true;
            }
            else return false;
        }

        WBackendNetQuery query = backend->getQueryFolder(source);

        if (query.isValid())
        {
            getDataFolder(folder, query);

            return true;
        }

        if (applyUrl(folder, backend, source))
        {
            folder->d_func()->setQueryEnded();

            if (WControllerNetwork::urlIsHttp(source))
            {
                WBackendNetQuery query(source);

                query.target = WBackendNetQuery::TargetHtml;
                query.id     = 1;

                query.clearItems = false;

                getDataFolder(folder, query);
            }

            return true;
        }
        else addFolderSearch(folder, source, WControllerNetwork::urlName(source));

        if (WControllerNetwork::urlIsHttp(source) == false)
        {
            return true;
        }
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

                    WBackendNetQuery query(source);

                    query.target     = WBackendNetQuery::TargetFolder;
                    query.clearItems = false;

                    getDataFolder(folder, query);

                    return true;
                }
                else if (info.size() < CONTROLLERPLAYLIST_MAX_SIZE)
                {
                    WBackendNetQuery query(source);

                    query.target     = WBackendNetQuery::TargetFile;
                    query.clearItems = false;

                    getDataFolder(folder, query);

                    return true;
                }
                else return false;
            }

            if (q->extensionIsMarkup(extension) == false
                &&
                info.size() < CONTROLLERPLAYLIST_MAX_SIZE)
            {
                WBackendNetQuery query(source);

                query.target     = WBackendNetQuery::TargetFile;
                query.clearItems = false;

                getDataFolder(folder, query);

                return true;
            }
        }
        else
        {
            addFolderSearch(folder, source, info.absoluteFilePath());

            WBackendNetQuery query(source);

            query.target     = WBackendNetQuery::TargetFolder;
            query.clearItems = false;

            getDataFolder(folder, query);

            return true;
        }
    }
    else if (q->urlIsMedia(source))
    {
        WLibraryFolderItem item(WLibraryItem::Playlist, WLocalObject::Default);

        item.source = source;

        folder->addItem(item);

        folder->d_func()->setQueryEnded();

        return true;
    }
    else addFolderSearch(folder, source, WControllerNetwork::urlName(source));

    WBackendNetQuery query(source);

    query.target     = WBackendNetQuery::TargetHtml;
    query.clearItems = false;

    getDataFolder(folder, query);

    return true;
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

            playlist->setTrackState(index, WTrack::Default);
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

//-------------------------------------------------------------------------------------------------

bool WControllerPlaylistPrivate::abortQueriesItem(WLibraryItem * item)
{
    int count = queries.count();

    foreach (WControllerPlaylistQuery * query, queries)
    {
        if (query->item == item)
        {
            removeQuery(query);
        }
    }

    item->d_func()->setQueryLoading(false);

    return (count != queries.count());
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WControllerPlaylistPrivate::registerBackend(WBackendNet * backend)
{
    backends.append(backend);
}

void WControllerPlaylistPrivate::unregisterBackend(WBackendNet * backend)
{
    backends.removeOne(backend);
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

    if (reply == NULL)
    {
        WRemoteData * data = query->data;

        jobs.remove(data);

        delete data;
    }
    else replies.remove(reply);

    deleteQuery(query);
}

void WControllerPlaylistPrivate::deleteQuery(WControllerPlaylistQuery * query)
{
    queries.removeOne(query);

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

        WLibraryItem::Type type;

        QString extension = WControllerNetwork::extractUrlExtension(url);

        if (WControllerPlaylist::extensionIsAscii(extension))
        {
             type = WLibraryItem::FolderSearch;
        }
        else type = WLibraryItem::Playlist;

        WLibraryFolderItem item(type, WLocalObject::Default);

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
                                                              const char             * slot) const
{
    Q_Q(const WControllerPlaylist);

    QMetaMethod method;

    WBackendNetQuery::Target target = query.target;

    if (target == WBackendNetQuery::TargetHtml)
    {
        method = methodHtml;
    }
    else if (target == WBackendNetQuery::TargetFolder)
    {
        method = methodFolder;
    }
    else method = methodFile;

    WControllerPlaylistReply * reply = new WControllerPlaylistReply;

    QObject::connect(reply, SIGNAL(loaded(QIODevice *, const WControllerPlaylistData &)), q, slot);

    reply ->moveToThread(thread);
    device->moveToThread(thread);

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
            WBackendNet * backend = q->backendFromUrl(item->title);

            if (backend)
            {
                item->cover = q->backendCover(backend);
            }
        }
        else
        {
            QString url = item->source;

            WBackendNet * backend = q->backendFromUrl(url);

            if (backend)
            {
                WBackendNetPlaylistInfo info = backend->getPlaylistInfo(url);

                if (info.isValid())
                {
                    item->type = info.type;
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------

void WControllerPlaylistPrivate::addToCache(const QString & url, const QByteArray & array) const
{
    if (array.isEmpty()) return;

    wControllerFile->addFile(url, array);
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
    Q_Q(WControllerPlaylist);

    if (query.clearItems)
    {
        playlist->clearTracks();
    }

    WAbstractLoader * loader = loaders.value(query.type);

    WRemoteData * data = WControllerPlaylist::getDataQuery(loader, query, q);

    WControllerPlaylistQuery * queryPlaylist
        = new WControllerPlaylistQuery(query, WControllerPlaylistQuery::TypePlaylist);

    queryPlaylist->data = data;
    queryPlaylist->item = playlist;

    queries.append(queryPlaylist);

    jobs.insert(data, queryPlaylist);

    QObject::connect(data, SIGNAL(loaded(WRemoteData *)), q, SLOT(onLoaded(WRemoteData *)));

    playlist->d_func()->setQueryLoading(true);

    emit playlist->queryStarted();
}

void WControllerPlaylistPrivate::getDataFolder(WLibraryFolder         * folder,
                                               const WBackendNetQuery & query)
{
    Q_Q(WControllerPlaylist);

    if (query.clearItems)
    {
        folder->clearItems();
    }

    WAbstractLoader * loader = loaders.value(query.type);

    WRemoteData * data = WControllerPlaylist::getDataQuery(loader, query, q);

    WControllerPlaylistQuery * queryFolder
        = new WControllerPlaylistQuery(query, WControllerPlaylistQuery::TypeFolder);

    queryFolder->data = data;
    queryFolder->item = folder;

    queries.append(queryFolder);

    jobs.insert(data, queryFolder);

    QObject::connect(data, SIGNAL(loaded(WRemoteData *)), q, SLOT(onLoaded(WRemoteData *)));

    folder->d_func()->setQueryLoading(true);

    emit folder->queryStarted();
}

bool WControllerPlaylistPrivate::getDataRelated(WBackendNet * backend,
                                                WPlaylist   * playlist, const QString & id)
{
    WBackendNetQuery query = backend->createQuery("related", "tracks", id);

    if (query.isValid())
    {
        query.clearItems = false;

        getDataPlaylist(playlist, query);

        return true;
    }
    else return false;
}

//-------------------------------------------------------------------------------------------------

void WControllerPlaylistPrivate::abortTrack(WTrack * track)
{
    foreach (WControllerPlaylistQuery * query, queries)
    {
        if (query->track == track)
        {
            removeQuery(query);

            return;
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

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WControllerPlaylistPrivate::onLoaded(WRemoteData * data)
{
    WControllerPlaylistQuery * query = jobs.take(data);

    if (query == NULL) qDebug("QUERY SHOULD NOT BE NULL");

    WLibraryItem * item = query->item;

    WBackendNetQuery * backendQuery = &(query->backendQuery);

    backendQuery->urlRedirect = data->url();

    WBackendNet * backend;

    QString id = backendQuery->backend;

    if (id.isEmpty())
    {
         backend = wControllerPlaylist->backendFromUrl(backendQuery->url);
    }
    else backend = wControllerPlaylist->backendFromId(id);

    if (data->hasError() && backendQuery->skipError == false)
    {
        if (query->type == WControllerPlaylistQuery::TypeTrack)
        {
            if (backend) backend->queryFailed(*backendQuery);

            WPlaylist * playlist = item->toPlaylist();

            int index = playlist->indexOf(query->track);

            if (index != -1)
            {
                WTrack * track = query->track;

                deleteQuery(query);

                track->setState(WTrack::Default);

                playlist->updateTrack(index);
            }
            else deleteQuery(query);

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

    Q_Q(WControllerPlaylist);

    QIODevice * networkReply = data->takeReply(NULL);

    //query->data  = data;
    query->reply = networkReply;

    replies.insert(networkReply, query);

    if (query->type == WControllerPlaylistQuery::TypeTrack)
    {
        if (backend == NULL) qDebug("BACKEND SHOULD NOT BE NULL");

        query->backend = backend;

        backend->loadTrack(networkReply, *backendQuery,
                           q, SLOT(onTrackLoaded(QIODevice *, WBackendNetTrack)));
    }
    else if (query->type == WControllerPlaylistQuery::TypePlaylist)
    {
        if (backendQuery->target == WBackendNetQuery::TargetDefault)
        {
            query->backend = backend;

            backend->loadPlaylist(networkReply, *backendQuery,
                                  q, SLOT(onPlaylistLoaded(QIODevice *, WBackendNetPlaylist)));
        }
        else loadUrls(networkReply, *backendQuery,
                      SLOT(onUrlPlaylist(QIODevice *, const WControllerPlaylistData &)));
    }
    else // if (query->type == WControllerPlaylistQuery::TypeFolder)
    {
        if (backendQuery->target == WBackendNetQuery::TargetDefault)
        {
            query->backend = backend;

            backend->loadFolder(networkReply, *backendQuery,
                                q, SLOT(onFolderLoaded(QIODevice *, WBackendNetFolder)));
        }
        else loadUrls(networkReply, *backendQuery,
                      SLOT(onUrlFolder(QIODevice *, const WControllerPlaylistData &)));
    }

    delete data;
}

//-------------------------------------------------------------------------------------------------

void WControllerPlaylistPrivate::onTrackLoaded(QIODevice              * device,
                                               const WBackendNetTrack & reply)
{
    WControllerPlaylistQuery * query = replies.take(device);

    device->deleteLater();

    if (query == NULL) return;

    query->backend->applyTrack(query->backendQuery, reply);

    WPlaylist * playlist = query->item->toPlaylist();
    WTrack    * track    = query->track;

    deleteQuery(query);

    int index = playlist->indexOf(track);

    if (index == -1) return;

    if (reply.valid)
    {
        reply.track.applyDataTo(track);

        addToCache(track->source(), reply.cache);

        WBackendNetQuery nextQuery = reply.nextQuery;

        if (nextQuery.isValid())
        {
            playlist->updateTrack(index);

            nextQuery.priority
                = static_cast<QNetworkRequest::Priority> (QNetworkRequest::NormalPriority - 1);

            getDataTrack(playlist, track, nextQuery);

            return;
        }
        else
        {
            track->setState(WTrack::Loaded);

            playlist->updateTrack(index);

            if (WControllerNetwork::removeUrlPrefix(playlist->source())
                ==
                WControllerNetwork::removeUrlPrefix(track->source()))
            {
                QString title = track->title();
                QString cover = track->cover();

                if (title.isEmpty() == false)
                {
                    playlist->setTitle(title);
                }

                if (cover.isEmpty() == false)
                {
                    playlist->setCover(cover);
                }
            }
            else playlist->updateCover();
        }
    }
    else
    {
        track->setState(WTrack::Default);

        playlist->updateTrack(index);
    }

    emit playlist->trackQueryEnded();
}

void WControllerPlaylistPrivate::onPlaylistLoaded(QIODevice                 * device,
                                                  const WBackendNetPlaylist & reply)
{
    WControllerPlaylistQuery * query = replies.take(device);

    device->deleteLater();

    if (query == NULL) return;

    query->backend->applyPlaylist(query->backendQuery, reply);

    WPlaylist * playlist = query->item->toPlaylist();

    deleteQuery(query);

    if (reply.valid)
    {
        const QString & title = reply.title;
        const QString & cover = reply.cover;

        if (title.isEmpty() == false)
        {
            playlist->setTitle(title);
        }

        if (cover.isEmpty() == false)
        {
            playlist->setCover(cover);
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

        if (index != -1)
        {
            playlist->setCurrentIndex(index);
        }

        emit playlist->queryEnded();

        addToCache(playlist->source(), reply.cache);

        WBackendNetQuery nextQuery = reply.nextQuery;

        if (nextQuery.isValid())
        {
            nextQuery.priority
                = static_cast<QNetworkRequest::Priority> (QNetworkRequest::NormalPriority - 1);

            nextQuery.clearItems = false;

            getDataPlaylist(playlist, nextQuery);

            return;
        }
    }
    else emit playlist->queryEnded();

    playlist->d_func()->setQueryLoaded();
}

void WControllerPlaylistPrivate::onFolderLoaded(QIODevice               * device,
                                                const WBackendNetFolder & reply)
{
    WControllerPlaylistQuery * query = replies.take(device);

    device->deleteLater();

    if (query == NULL) return;

    query->backend->applyFolder(query->backendQuery, reply);

    WLibraryFolder * folder = query->item->toFolder();

    deleteQuery(query);

    if (reply.valid)
    {
        const QString & title = reply.title;
        const QString & cover = reply.cover;

        if (title.isEmpty() == false)
        {
            folder->setTitle(title);
        }

        if (cover.isEmpty() == false)
        {
            folder->setCover(cover);
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

        if (index != -1)
        {
            folder->setCurrentIndex(index);
        }

        emit folder->queryEnded();

        addToCache(folder->source(), reply.cache);

        WBackendNetQuery nextQuery = reply.nextQuery;

        if (nextQuery.isValid())
        {
            nextQuery.priority
                = static_cast<QNetworkRequest::Priority> (QNetworkRequest::NormalPriority - 1);

            nextQuery.clearItems = false;

            getDataFolder(folder, nextQuery);

            return;
        }
    }
    else emit folder->queryEnded();

    folder->d_func()->setQueryLoaded();
}

//-------------------------------------------------------------------------------------------------

void WControllerPlaylistPrivate::onUrlPlaylist(QIODevice                     * device,
                                               const WControllerPlaylistData & data)
{
    WControllerPlaylistQuery * query = replies.take(device);

    device->deleteLater();

    if (query == NULL) return;

    WPlaylist * playlist = query->item->toPlaylist();

    QString feed = query->backendQuery.url;

    QStringList urlTracks;

    for (int i = 0; i < playlist->count(); i++)
    {
        const QString & url = playlist->d_func()->itemAt(i)->source();

        WBackendNet * backend = wControllerPlaylist->backendFromUrl(url);

        if (backend)
        {
            QString id = backend->getTrackId(url);

            if (id.isEmpty())
            {
                 urlTracks.append(url);
            }
            else urlTracks.append(backend->getUrlTrack(id));
        }
        else urlTracks.append(url);
    }

    deleteQuery(query);

    playlist->setTitle(data.title);
    playlist->setCover(data.cover);

    foreach (const WControllerPlaylistSource & source, data.sources)
    {
        const QString & url = source.url;

        WBackendNet * backend = wControllerPlaylist->backendFromUrl(url);

        if (backend == NULL) continue;

        QString id = backend->getTrackId(url);

        if (id.isEmpty() == false)
        {
            if (urlTracks.count() == CONTROLLERPLAYLIST_MAX_TRACKS) break;

            QString source = backend->getUrlTrack(id);

            if (urlTracks.contains(source)) continue;

            urlTracks.append(source);

            WTrack track(url, WTrack::Default);

            playlist->addTrack(track);
        }
    }

    int indexTrack;

    if (urlTracks.count() == 1)
    {
         indexTrack = playlist->count() - 1;
    }
    else indexTrack = -1;

    foreach (const WControllerPlaylistMedia & media, data.medias)
    {
        if (urlTracks.count() == CONTROLLERPLAYLIST_MAX_TRACKS) break;

        const QString & url = media.url;

        if (urlTracks.contains(url)) continue;

        urlTracks.append(url);

        WTrack track(url, WTrack::Default);

        track.setTitle(media.title);

        track.setCover(data.cover);

        track.setFeed(feed);

        playlist->addTrack(track);
    }

    if (WControllerNetwork::urlIsFile(playlist->source()))
    {
        applyCurrentIndex(playlist);
    }

    if (indexTrack != -1)
    {
        QString source = playlist->trackSource(indexTrack);

        WBackendNet * backend = wControllerPlaylist->backendFromUrl(source);

        if (backend)
        {
            emit playlist->queryEnded();

            playlist->loadTrack(indexTrack);

            if (getDataRelated(backend, playlist, backend->getTrackId(source))) return;
        }
    }
    else emit playlist->queryEnded();

    playlist->d_func()->setQueryLoaded();
}

void WControllerPlaylistPrivate::onUrlFolder(QIODevice                     * device,
                                             const WControllerPlaylistData & data)
{
    WControllerPlaylistQuery * query = replies.take(device);

    device->deleteLater();

    if (query == NULL) return;

    WLibraryFolder * folder = query->item->toFolder();

    QStringList urls;

    if (query->backendQuery.id == 1)
    {
        deleteQuery(query);

        for (int i = 0; i < folder->count(); i++)
        {
            const QString & url = folder->itemAt(i)->source;

            WBackendNet * backend = wControllerPlaylist->backendFromUrl(url);

            if (backend)
            {
                WBackendNetPlaylistInfo info = backend->getPlaylistInfo(url);

                if (info.isValid())
                {
                     urls.append(backend->getUrlPlaylist(info));
                }
                else urls.append(url);
            }
            else urls.append(url);
        }

        foreach (const WControllerPlaylistSource & source, data.sources)
        {
            const QString & url = source.url;

            WBackendNet * backend = wControllerPlaylist->backendFromUrl(url);

            if (backend)
            {
                applyPlaylist(folder, backend, url, &urls);

                if (urls.count() == CONTROLLERPLAYLIST_MAX_ITEMS) break;
            }
        }

        applySources(folder, data.folders, &urls);
        applySources(folder, data.files,   &urls);

        folder->d_func()->setQueryEnded();

        return;
    }

    QString source = folder->source();

    QString feed = query->backendQuery.url;

    deleteQuery(query);

    WPlaylist * playlist = folder->createLibraryItemAt(0, true)->toPlaylist();

    playlist->setTitle(data.title);
    playlist->setCover(data.cover);

    QStringList urlTracks;

    foreach (const WControllerPlaylistSource & source, data.sources)
    {
        const QString & url = source.url;

        WBackendNet * backend = wControllerPlaylist->backendFromUrl(url);

        if (backend == NULL) continue;

        QString id = backend->getTrackId(url);

        if (id.isEmpty() == false && urlTracks.count() != CONTROLLERPLAYLIST_MAX_TRACKS)
        {
            QString source = backend->getUrlTrack(id);

            if (urlTracks.contains(source)) continue;

            urlTracks.append(source);

            WTrack track(url, WTrack::Default);

            playlist->addTrack(track);
        }
        else if (urls.count() != CONTROLLERPLAYLIST_MAX_ITEMS)
        {
            applyPlaylist(folder, backend, url, &urls);
        }
    }

    applySources(folder, data.folders, &urls);
    applySources(folder, data.files,   &urls);

    bool singleTrack;

    if (urlTracks.count() == 1)
    {
         singleTrack = true;
    }
    else singleTrack = false;

    foreach (const WControllerPlaylistMedia & media, data.medias)
    {
        if (urlTracks.count() == CONTROLLERPLAYLIST_MAX_TRACKS) break;

        const QString & url = media.url;

        if (urlTracks.contains(url)) continue;

        urlTracks.append(url);

        WTrack track(url, WTrack::Default);

        track.setTitle(media.title);

        track.setCover(data.cover);

        track.setFeed(feed);

        playlist->addTrack(track);
    }

    if (WControllerNetwork::urlIsFile(source))
    {
        applyCurrentIndex(playlist);
    }

    if (singleTrack)
    {
        QString source = playlist->trackSource(0);

        WBackendNet * backend = wControllerPlaylist->backendFromUrl(source);

        if (backend)
        {
            playlist->loadTrack(0);

            getDataRelated(backend, playlist, backend->getTrackId(source));
        }

        playlist->d_func()->setQueryEnded();
    }
    else if (folder->count() > 1 && playlist->isEmpty())
    {
        folder->loadCurrentIndex(1, true);

        folder->removeItem(playlist->id());
    }
    else playlist->d_func()->setQueryEnded();

    folder->d_func()->setQueryEnded();

    playlist->tryDelete();
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

    return wControllerDownload->getData(loader, query.url, parent, query.priority, true,
                                        query.cookies, query.header, query.maxHost, query.delay,
                                        query.timeout);
}

/* Q_INVOKABLE */ WRemoteData * WControllerPlaylist::getData(const WBackendNetQuery & query,
                                                             QObject                * parent) const
{
    Q_D(const WControllerPlaylist);

    WAbstractLoader * loader = d->loaders.value(query.type);

    return wControllerDownload->getData(loader, query.url, parent, query.priority, true,
                                        query.cookies, query.header, query.maxHost, query.delay,
                                        query.timeout);
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

    foreach (WBackendNet * backend, d->backends)
    {
        if (backend->id() == id)
        {
            return backend;
        }
    }

    return NULL;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ WBackendNet * WControllerPlaylist::backendFromUrl(const QString & url) const
{
    QUrl source(url);

    if (source.host() == sk->applicationHost())
    {
#ifdef QT_4
        QString backend = source.queryItemValue("backend");
#else
        QString backend = QUrlQuery(source).queryItemValue("backend");
#endif

        return backendFromId(backend);
    }
    else
    {
        Q_D(const WControllerPlaylist);

        foreach (WBackendNet * backend, d->backends)
        {
            if (backend->checkValidUrl(url))
            {
                return backend;
            }
        }
    }

    return NULL;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ WBackendNet * WControllerPlaylist::backendFromTrack(const QString & url) const
{
    Q_D(const WControllerPlaylist);

    foreach (WBackendNet * backend, d->backends)
    {
        if (backend->getTrackId(url).isEmpty() == false)
        {
            return backend;
        }
    }

    return NULL;
}

/* Q_INVOKABLE */ WBackendNet * WControllerPlaylist::backendFromPlaylist(const QString & url) const
{
    Q_D(const WControllerPlaylist);

    foreach (WBackendNet * backend, d->backends)
    {
        if (backend->getPlaylistInfo(url).isValid())
        {
            return backend;
        }
    }

    return NULL;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ WBackendNet * WControllerPlaylist::backendForCover(const QString & label,
                                                                     const QString & q) const
{
    Q_D(const WControllerPlaylist);

    foreach (WBackendNet * backend, d->backends)
    {
        if (backend->checkCover(label, q))
        {
            return backend;
        }
    }

    return NULL;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ QString WControllerPlaylist::backendCover(WBackendNet * backend) const
{
    Q_ASSERT(backend);

    Q_D(const WControllerPlaylist);

    return d->pathCover + '/' + backend->id() + ".png";
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ QString WControllerPlaylist::backendCoverFromId(const QString & id) const
{
    WBackendNet * backend = backendFromId(id);

    if (backend)
    {
         return backendCover(backend);
    }
    else return QString();
}

/* Q_INVOKABLE */ QString WControllerPlaylist::backendCoverFromUrl(const QString & url) const
{
    WBackendNet * backend = backendFromUrl(url);

    if (backend)
    {
         return backendCover(backend);
    }
    else return QString();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ WLibraryItem::Type WControllerPlaylist::urlType(const QString & url) const
{
    Q_D(const WControllerPlaylist);

    foreach (WBackendNet * backend, d->backends)
    {
        WBackendNetPlaylistInfo info = backend->getPlaylistInfo(url);

        if (info.isValid())
        {
            return info.type;
        }
    }

    return WLibraryItem::Item;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ bool WControllerPlaylist::urlIsTrack(const QString & url) const
{
    if (backendFromTrack(url))
    {
         return true;
    }
    else return urlIsMedia(url);
}

/* Q_INVOKABLE */ bool WControllerPlaylist::urlIsPlaylist(const QString & url) const
{
    if (backendFromPlaylist(url))
    {
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

/* Q_INVOKABLE */ void WControllerPlaylist::createBackendItems(WLibraryFolder * folder) const
{
    Q_ASSERT(folder);

    Q_D(const WControllerPlaylist);

    QString path = d->pathCover + '/';

    foreach (WBackendNet * backend, d->backends)
    {
        QList<WLibraryFolderItem> items = backend->getLibraryItems();

        if (items.isEmpty()) continue;

        WLibraryFolderSearchable * folderBackend = new WLibraryFolderSearchable;

        folderBackend->setTitle(backend->title());

        folderBackend->setCover(path + backend->id() + ".png");

        folderBackend->setLabel(backend->id());

        folderBackend->addItems(items);

        folder->addLibraryItem(folderBackend);

        folderBackend->setCurrentIndex(0);

        folderBackend->tryDelete();
    }
}

/* Q_INVOKABLE */ void WControllerPlaylist::restoreBackendItems(WLibraryFolder * folder) const
{
    Q_ASSERT(folder);

    for (int i = 0; i < folder->count(); i++)
    {
        WLibraryFolder * folderBackend = folder->createLibraryItemAt(i)->toFolder();

        WBackendNet * backend = backendFromId(folderBackend->label());

        if (backend)
        {
            folderBackend->clearItems();

            folderBackend->addItems(backend->getLibraryItems());

            folderBackend->setCurrentIndex(0);
        }

        folderBackend->tryDelete();
    }
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

        if (item)
        {
            item->d_func()->setQueryLoading(false);
        }
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

/* Q_INVOKABLE static */ QString WControllerPlaylist::generateSource(const QString & url)
{
    QString source = WControllerNetwork::decodeUrl(url);

    if (WControllerNetwork::urlIsFile(source) || WControllerNetwork::urlIsHttp(source))
    {
        return url;
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
         return "http://" + source;
    }
    else return source;
}

/* Q_INVOKABLE static */ QString WControllerPlaylist::createSource(const QString & backend,
                                                                   const QString & method,
                                                                   const QString & label,
                                                                   const QString & q)
{
    QUrl source(sk->applicationUrl());

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

/* Q_INVOKABLE static */
WRemoteData * WControllerPlaylist::getDataQuery(WAbstractLoader        * loader,
                                                const WBackendNetQuery & query, QObject * parent)
{
    return wControllerDownload->getData(loader, query.url, parent, query.priority, true,
                                        query.cookies, query.header, query.maxHost, query.delay,
                                        query.timeout);
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
    languages.append(tr("Chinese #1"));
    languages.append(tr("Chinese #2"));
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
    languages.append(tr("Romanian"));
    languages.append(tr("Russian"));
    languages.append(tr("Serbian"));
    languages.append(tr("Sinhala"));
    languages.append(tr("Slovak"));
    languages.append(tr("Slovenian"));
    languages.append(tr("Spanish"));
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

/* Q_INVOKABLE static */ QString WControllerPlaylist::getFilterFile()
{
    return CONTROLLERPLAYLIST_FILTER;
}

/* Q_INVOKABLE static */ QString WControllerPlaylist::getFilterSubtitle()
{
    return CONTROLLERPLAYLIST_FILTER_SUBTITLE;
}

//-------------------------------------------------------------------------------------------------

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

/* Q_INVOKABLE static */ bool WControllerPlaylist::extensionIsMedia(const QString & extension)
{
    return (extensionIsVideo(extension) || extensionIsAudio(extension));
}

/* Q_INVOKABLE static */ bool WControllerPlaylist::extensionIsVideo(const QString & extension)
{
    if (extension.indexOf(QRegExp(CONTROLLERPLAYLIST_VIDEO)) == -1)
    {
         return false;
    }
    else return true;
}

/* Q_INVOKABLE static */ bool WControllerPlaylist::extensionIsAudio(const QString & extension)
{
    if (extension.indexOf(QRegExp(CONTROLLERPLAYLIST_AUDIO)) == -1)
    {
         return false;
    }
    else return true;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ bool WControllerPlaylist::extensionIsAscii(const QString & extension)
{
    return (extensionIsMarkup(extension) || extensionIsText(extension));
}

/* Q_INVOKABLE static */ bool WControllerPlaylist::extensionIsMarkup(const QString & extension)
{
    if (extension.indexOf(QRegExp(CONTROLLERPLAYLIST_MARKUP)) == -1)
    {
         return false;
    }
    else return true;
}

/* Q_INVOKABLE static */ bool WControllerPlaylist::extensionIsText(const QString & extension)
{
    if (extension.indexOf(QRegExp(CONTROLLERPLAYLIST_TEXT)) == -1)
    {
         return false;
    }
    else return true;
}

/* Q_INVOKABLE static */ bool WControllerPlaylist::extensionIsSubtitle(const QString & extension)
{
    if (extension.indexOf(QRegExp(CONTROLLERPLAYLIST_SUBTITLE)) == -1)
    {
         return false;
    }
    else return true;
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

QThread * WControllerPlaylist::thread() const
{
    Q_D(const WControllerPlaylist); return d->thread;
}

//-------------------------------------------------------------------------------------------------

QList<WBackendNet *> WControllerPlaylist::backends() const
{
    Q_D(const WControllerPlaylist); return d->backends;
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

QString WControllerPlaylist::pathCover() const
{
    Q_D(const WControllerPlaylist); return d->pathCover;
}

void WControllerPlaylist::setPathCover(const QString & path)
{
    Q_D(WControllerPlaylist);

    if (d->pathCover == path) return;

    d->pathCover = path;

    emit pathCoverChanged();
}

#endif // SK_NO_CONTROLLERPLAYLIST

#include "WControllerPlaylist.moc"
