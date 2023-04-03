//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkMedia.

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

#include "WControllerMedia.h"

#ifndef SK_NO_CONTROLLERMEDIA

// Qt includes
#include <QThread>

// Sk includes
#include <WControllerApplication>
#include <WControllerFile>
#include <WControllerNetwork>
#include <WControllerDownload>
#include <WControllerPlaylist>
#include <WYamlReader>
#ifndef SK_NO_PLAYER
#include <WVlcEngine>
#include <WVlcPlayer>
#endif

W_INIT_CONTROLLER(WControllerMedia)

//-------------------------------------------------------------------------------------------------
// Static variables

static const int CONTROLLERMEDIA_CACHE_MAX = 1000;

static const int CONTROLLERMEDIA_MAX_QUERY  = 100;
static const int CONTROLLERMEDIA_MAX_RELOAD =  10;

//=================================================================================================
// WMediaReply
//=================================================================================================
// Private

WMediaReply::WMediaReply(const QString & url,
                         WAbstractBackend::SourceMode mode, QObject * parent) : QObject(parent)
{
    _url = url;

    _mode = mode;

    _type = WTrack::Track;

    _backend = NULL;

    _loaded = false;
}

//-------------------------------------------------------------------------------------------------
// Public

/* virtual */ WMediaReply::~WMediaReply()
{
    W_GET_CONTROLLER(WControllerMedia, controller);

    if (controller) controller->d_func()->clearReply(this);
}

//-------------------------------------------------------------------------------------------------
// Virtual interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QString WMediaReply::toVbml() const
{
    QString vbml = WControllerPlaylist::vbml();

    Sk::bmlPair(vbml, "type", "media", "\n\n");

    QString tab = Sk::tabs(1);

    if (_medias.isEmpty() == false)
    {
        Sk::bmlTag(vbml, "medias");

        QHashIterator<WAbstractBackend::Quality, QString> i(_medias);

        while (i.hasNext())
        {
            i.next();

            Sk::bmlPair(vbml, tab + WAbstractBackend::qualityToString(i.key()), i.value(), "\n\n");
        }
    }

    if (_audios.isEmpty() == false)
    {
        Sk::bmlTag(vbml, "audios", "\n");

        QHashIterator<WAbstractBackend::Quality, QString> i(_audios);

        while (i.hasNext())
        {
            i.next();

            Sk::bmlPair(vbml, tab + WAbstractBackend::qualityToString(i.key()), i.value(), "\n\n");
        }
    }

    // NOTE: We clear the last '\n'.
    vbml.chop(1);

    return vbml;
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

QString WMediaReply::url() const
{
    return _url;
}

//-------------------------------------------------------------------------------------------------

WTrack::Type WMediaReply::type() const
{
    return _type;
}

QHash<WAbstractBackend::Quality, QString> WMediaReply::medias() const
{
    return _medias;
}

QHash<WAbstractBackend::Quality, QString> WMediaReply::audios() const
{
    return _audios;
}

//-------------------------------------------------------------------------------------------------

bool WMediaReply::isLoaded() const
{
    return _loaded;
}

//-------------------------------------------------------------------------------------------------

bool WMediaReply::hasError() const
{
    return (_error.isEmpty() == false);
}

QString WMediaReply::error() const
{
    return _error;
}

//=================================================================================================
// WControllerMediaData
//=================================================================================================
// Interface

void WControllerMediaData::applyVbml(const QByteArray & array, const QString & url)
{
    QString content = Sk::readBml(array);

    //---------------------------------------------------------------------------------------------
    // Api

    QString api = WControllerPlaylist::vbmlVersion(content);

    if (api.isEmpty())
    {
        if (WControllerPlaylist::textIsRedirect(content, url))
        {
            origin = content;

            return;
        }

        // NOTE: If it's HTML we try to extract a VBML link.
        if (array.contains("<html>"))
        {
            origin = WControllerPlaylistData::extractHtmlLink(array, url);

            if (origin.isEmpty() == false) return;
        }
    }

    if (Sk::versionIsHigher(WControllerPlaylist::versionApi(), api))
    {
        WControllerPlaylist::vbmlPatch(content, api);

        applyVbml(content.toUtf8(), url);

        return;
    }

    if (Sk::versionIsLower(WControllerPlaylist::versionApi(), api))
    {
        qWarning("WControllerMediaReply::applyVbml: The required API is too high.");
    }

    WYamlReader reader(content.toUtf8());

    //---------------------------------------------------------------------------------------------
    // Source

    QString string = reader.extractString("origin");

    if (string.isEmpty() == false
        &&
        // NOTE: The origin has to be different than the current URL.
        WControllerNetwork::removeUrlPrefix(url) != WControllerNetwork::removeUrlPrefix(string))
    {
        origin = string;

        return;
    }

    // NOTE: The media is prioritized over the source.
    source = reader.extractString("media");

    if (source.isEmpty())
    {
        source = reader.extractString("source");
    }

    // NOTE: If parsing fails, apply the url itself given it might be readable by the player.
    if (source.isEmpty())
    {
        source = url;

        return;
    }

    type = WTrack::typeFromString(reader.extractString("type"));
}

//=================================================================================================
// WControllerMediaReply
//=================================================================================================

class WControllerMediaReply : public QObject
{
    Q_OBJECT

public: // Interface
    Q_INVOKABLE void extractVbml(QIODevice * device, const QString & url);

private: // Functions
    void applyVbml(const QByteArray & array, const QString & url);

signals:
    void loaded(QIODevice * device, const WControllerMediaData & data);
};

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WControllerMediaReply::extractVbml(QIODevice * device, const QString & url)
{
    WControllerMediaData data;

    data.applyVbml(device->readAll(), url);

    emit loaded(device, data);

    deleteLater();
}

//=================================================================================================
// WControllerMediaPrivate
//=================================================================================================

WControllerMediaPrivate::WControllerMediaPrivate(WControllerMedia * p) : WControllerPrivate(p) {}

/* virtual */ WControllerMediaPrivate::~WControllerMediaPrivate()
{
#ifndef SK_NO_PLAYER
    engine->deleteInstance();
#endif

    QHashIterator<WRemoteData *, WPrivateMediaData *> i(jobs);

    while (i.hasNext())
    {
        i.next();

        delete i.value();
    }

    QHashIterator<QIODevice *, WPrivateMediaData *> j(queries);

    while (j.hasNext())
    {
        j.next();

        delete j.value();
    }

    jobs   .clear();
    queries.clear();

    // FIXME Qt4: Not sure why we needed that before but that causes a crash.
    //sk->processEvents();

    thread->quit();
    thread->wait();

#ifndef SK_NO_PLAYER
    delete engine;
#endif

    W_CLEAR_CONTROLLER(WControllerMedia);
}

//-------------------------------------------------------------------------------------------------

#ifdef SK_NO_PLAYER
void WControllerMediaPrivate::init(const QStringList &)
#else
void WControllerMediaPrivate::init(const QStringList & options)
#endif
{
    Q_Q(WControllerMedia);

    loader = NULL;

    qRegisterMetaType<WControllerMediaData>("WControllerMediaData");

    const QMetaObject * meta = WControllerMediaReply().metaObject();

    methodVbml = meta->method(meta->indexOfMethod("extractVbml(QIODevice*,QString)"));

    thread = new QThread(q);

    thread->start();

#ifndef SK_NO_PLAYER
    engine = new WVlcEngine(options, thread);
#endif
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WControllerMediaPrivate::loadSources(WMediaReply * reply)
{
    const QString & url = reply->_url;

    QHashIterator<WRemoteData *, WPrivateMediaData *> i(jobs);

    while (i.hasNext())
    {
        i.next();

        WPrivateMediaData * media = i.value();

        if (media->url == url)
        {
            media->replies.append(reply);

            return;
        }
    }

    QHashIterator<QIODevice *, WPrivateMediaData *> j(queries);

    while (j.hasNext())
    {
        j.next();

        WPrivateMediaData * media = j.value();

        if (media->url == url)
        {
            media->replies.append(reply);

            return;
        }
    }

    QString source = WControllerMedia::generateSource(url);

    WBackendNet * backend = wControllerPlaylist->backendFromUrl(source);

    WBackendNetQuery query;

    if (backend)
    {
        QString backendId = backend->id();

        query = backend->getQuerySource(source);

        backend->tryDelete();

        if (resolve(backendId, query) == false)
        {
            reply->_medias.insert(WAbstractBackend::QualityDefault, url);

            reply->_loaded = true;

            return;
        }
    }
    else if (WControllerPlaylist::urlIsVbmlUri(source))
    {
        query.type = WBackendNetQuery::TypeVbml;

        query.url = source;
    }
    else if (WControllerPlaylist::urlIsVbmlFile(source)
             ||
             // NOTE: The source could be VBML so we try to load it anyway.
             (WControllerNetwork::extractUrlExtension(source) == QString()
              &&
              WControllerNetwork::urlIsHttp(source)
              &&
              // NOTE: The url should not be an IP because it can be a HookTorrent server.
              WControllerNetwork::urlIsIp(source) == false))
    {
        query.url = source;
    }
    else
    {
        reply->_medias.insert(WAbstractBackend::QualityDefault, url);

        reply->_loaded = true;

        return;
    }

    query.mode = reply->_mode;

    WPrivateMediaData * media = new WPrivateMediaData;

    media->type    = WTrack::Track;
    media->url     = url;
    media->backend = NULL;
    media->query   = query;
    media->reply   = NULL;

    media->replies.append(reply);

    medias.append(media);

    getData(media, &query);
}

void WControllerMediaPrivate::loadUrl(QIODevice * device, const WBackendNetQuery & query) const
{
    Q_Q(const WControllerMedia);

    WControllerMediaReply * reply = new WControllerMediaReply;

    QObject::connect(reply, SIGNAL(loaded(QIODevice *, const WControllerMediaData &)),
                     q,     SLOT  (onUrl (QIODevice *, const WControllerMediaData &)));

    reply->moveToThread(thread);

    methodVbml.invoke(reply, Q_ARG(QIODevice *, device), Q_ARG(const QString &, query.url));
}

//-------------------------------------------------------------------------------------------------

void WControllerMediaPrivate::updateSources()
{
    QDateTime date = QDateTime::currentDateTime();

    QMutableHashIterator<QString, WPrivateMediaSource> i(sources);

    while (i.hasNext())
    {
        i.next();

        QHash<WAbstractBackend::SourceMode, WPrivateMediaMode> & modes = i.value().modes;

        QMutableHashIterator<WAbstractBackend::SourceMode, WPrivateMediaMode> j(modes);

        while (j.hasNext())
        {
            j.next();

            QDateTime expiry = j.value().expiry;

            if (expiry.isValid() && expiry < date)
            {
                qDebug("MEDIA EXPIRED");

                j.remove();
            }
        }

        if (sources.isEmpty())
        {
            qDebug("SOURCE EXPIRED");

            urls.removeOne(i.key());

            i.remove();
        }
    }

    while (urls.count() > CONTROLLERMEDIA_CACHE_MAX)
    {
        QString url = urls.takeFirst();

        sources.remove(url);
    }
}

//-------------------------------------------------------------------------------------------------

void WControllerMediaPrivate::clearReply(WMediaReply * reply)
{
    foreach (WPrivateMediaData * media, medias)
    {
        QList<WMediaReply *> * replies = &(media->replies);

        if (replies->contains(reply) == false) continue;

        replies->removeOne(reply);

        if (replies->isEmpty())
        {
            WRemoteData * data = jobs.key(media);

            if (data) jobs.remove(data);

            WBackendNet * backend = media->backend;

            if (backend) backend->tryDelete();

            QIODevice * networkReply = media->reply;

            if (networkReply)
            {
                queries.remove(networkReply);
            }

            medias.removeOne(media);

            delete media;
            delete data;
        }

        return;
    }
}

//-------------------------------------------------------------------------------------------------

bool WControllerMediaPrivate::resolve(const QString & backendId, WBackendNetQuery & query)
{
    QString id = query.backend;

    if (id.isEmpty() || id == backendId) return query.isValid();

    WBackendNet * backend = wControllerPlaylist->backendFromId(id);

    if (backend) query = backend->getQuerySource(query.url);

    return query.isValid();
}

void WControllerMediaPrivate::getData(WPrivateMediaData * media, WBackendNetQuery * query)
{
    Q_Q(WControllerMedia);

    // NOTE: Media sources should be high priority by default. But maybe we should let the user
    //       configure this.
    query->priority = static_cast<QNetworkRequest::Priority> (QNetworkRequest::HighPriority);

    WRemoteData * data = wControllerPlaylist->getData(loader, *query, q);

    QObject::connect(data, SIGNAL(loaded(WRemoteData *)), q, SLOT(onLoaded(WRemoteData *)));

    jobs.insert(data, media);
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WControllerMediaPrivate::onLoaded(WRemoteData * data)
{
    WPrivateMediaData * media = jobs.take(data);

    WBackendNetQuery * backendQuery = &(media->query);

    WBackendNet * backend;

    QString id = backendQuery->backend;

    if (id.isEmpty())
    {
         backend = wControllerPlaylist->backendFromUrl(backendQuery->url);
    }
    else backend = wControllerPlaylist->backendFromId(id);

    if (data->hasError() && backendQuery->skipError == false)
    {
        qWarning("WControllerMediaPrivate::onLoaded: Failed to load media %s.", data->url().C_STR);

        if (backend)
        {
            backend->queryFailed(*backendQuery);

            backend->tryDelete();
        }

        QString error = data->error();

        foreach (WMediaReply * reply, media->replies)
        {
            reply->_error = error;

            emit reply->loaded(reply);
        }

        medias.removeOne(media);

        delete media;
    }
    else
    {
        Q_Q(WControllerMedia);

        QIODevice * reply = data->takeReply(NULL);

        media->reply = reply;

        queries.insert(reply, media);

        if (backend)
        {
            // NOTE: We apply the backend to the query so we can delete it later.
            media->backend = backend;

            backend->loadSource(reply, *backendQuery,
                                q, SLOT(onSourceLoaded(QIODevice *, WBackendNetSource)));
        }
        else loadUrl(reply, *backendQuery);
    }

    delete data;
}

void WControllerMediaPrivate::onUrl(QIODevice * device, const WControllerMediaData & data)
{
    WPrivateMediaData * media = queries.take(device);

    device->deleteLater();

    if (media == NULL) return;

    QString origin = data.origin;

    WBackendNetQuery & query = media->query;

    WAbstractBackend::SourceMode mode = query.mode;

    if (origin.isEmpty() == false)
    {
        query = WBackendNetQuery(origin);

        if (WControllerPlaylist::urlIsVbmlUri(origin))
        {
            query.type = WBackendNetQuery::TypeVbml;

            query.url = origin;
        }

        // NOTE: We propagate the compatibility mode.
        query.mode = mode;

        media->type = data.type;

        getData(media, &query);

        return;
    }

    QString source = data.source;

    WBackendNet * backend = wControllerPlaylist->backendFromUrl(source);

    if (backend)
    {
        QString backendId = backend->id();

        query = backend->getQuerySource(source);

        backend->tryDelete();

        if (resolve(backendId, query))
        {
            // NOTE: We propagate the compatibility mode.
            query.mode = mode;

            media->type = data.type;

            getData(media, &query);

            return;
        }
    }

    foreach (WMediaReply * reply, media->replies)
    {
        emit reply->loaded(reply);
    }

    medias.removeOne(media);

    delete media;
}

//-------------------------------------------------------------------------------------------------

void WControllerMediaPrivate::onSourceLoaded(QIODevice * device, const WBackendNetSource & source)
{
    WPrivateMediaData * media = queries.take(device);

    device->deleteLater();

    if (media == NULL) return;

    media->reply = NULL;

    const WBackendNetQuery & backendQuery = media->query;

    if (source.reload)
    {
        int indexReload = backendQuery.indexReload;

        if (indexReload < CONTROLLERMEDIA_MAX_RELOAD)
        {
            WBackendNetQuery nextQuery = backendQuery;

            nextQuery.indexReload = indexReload + 1;

            // NOTE: We propagate the compatibility mode.
            nextQuery.mode = backendQuery.mode;

            media->query = nextQuery;

            getData(media, &nextQuery);

            return;
        }
    }

    WBackendNet * backend = media->backend;

    QString backendId;

    if (backend)
    {
        backend->applySource(backendQuery, source);

        backendId = backend->id();

        media->backend = NULL;

        backend->tryDelete();
    }

    if (source.valid)
    {
        const QByteArray & cache = source.cache;

        if (cache.isEmpty() == false)
        {
            wControllerFile->addCache(media->url, cache);
        }
    }

    const QList<WBackendNetQuery> & queries = source.nextQueries;

    if (queries.isEmpty() == false)
    {
        WBackendNetQuery nextQuery = queries.first();

        int indexNext = backendQuery.indexNext;

        if (resolve(backendId, nextQuery) && indexNext < CONTROLLERMEDIA_MAX_QUERY)
        {
            nextQuery.indexNext = indexNext + 1;

            // NOTE: We propagate the compatibility mode.
            nextQuery.mode = backendQuery.mode;

            media->query = nextQuery;

            getData(media, &nextQuery);

            return;
        }
    }

    WTrack::Type type = media->type;

    const QHash<WAbstractBackend::Quality, QString> & medias = source.medias;
    const QHash<WAbstractBackend::Quality, QString> & audios = source.audios;

    if (medias.count())
    {
        WPrivateMediaMode mode;

        mode.medias = source.medias;
        mode.audios = source.audios;

        mode.expiry = source.expiry;

        const QString & url = media->url;

        if (sources.contains(url))
        {
            WPrivateMediaSource * source = &(sources[url]);

            source->type = type;

            source->modes.insert(backendQuery.mode, mode);
        }
        else
        {
            WPrivateMediaSource source;

            source.type = type;

            source.modes.insert(backendQuery.mode, mode);

            sources.insert(url, source);
        }

        foreach (WMediaReply * reply, media->replies)
        {
            reply->_loaded = true;

            reply->_type = type;

            reply->_medias = medias;
            reply->_audios = audios;

            emit reply->loaded(reply);
        }
    }
    else
    {
        foreach (WMediaReply * reply, media->replies)
        {
            emit reply->loaded(reply);
        }
    }

    this->medias.removeOne(media);

    delete media;
}

//=================================================================================================
// WControllerMedia
//=================================================================================================
// Private

WControllerMedia::WControllerMedia() : WController(new WControllerMediaPrivate(this)) {}

//-------------------------------------------------------------------------------------------------
// Initialize
//-------------------------------------------------------------------------------------------------

/* virtual */ void WControllerMedia::initController(const QStringList & options)
{
    Q_D(WControllerMedia);

    if (d->created == false)
    {
        d->created = true;

        d->init(options);
    }
    else qWarning("WControllerMedia::initController: Controller is already initialized.");
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

#ifndef SK_NO_PLAYER

/* Q_INVOKABLE */ WVlcPlayer * WControllerMedia::createVlcPlayer() const
{
    Q_D(const WControllerMedia);

    return new WVlcPlayer(d->engine, d->thread);
}

#endif

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ WMediaReply * WControllerMedia::getMedia(const QString & url,
                                                           WAbstractBackend::SourceMode mode,
                                                           QObject * parent)
{
    if (url.isEmpty()) return NULL;

    Q_D(WControllerMedia);

    WMediaReply * reply;

    if (parent) reply = new WMediaReply(url, mode, parent);
    else        reply = new WMediaReply(url, mode, this);

    d->updateSources();

    if (d->sources.contains(url))
    {
        const WPrivateMediaSource & source = d->sources[url];

        const QHash<WAbstractBackend::SourceMode, WPrivateMediaMode> & sources = source.modes;

        if (sources.contains(mode))
        {
            qDebug("MEDIA CACHED");

            d->urls.removeOne(url);
            d->urls.append   (url);

            reply->_type = source.type;

            const WPrivateMediaMode & source = sources[mode];

            reply->_medias = source.medias;
            reply->_audios = source.audios;

            reply->_loaded = true;

            return reply;
        }
    }

    d->loadSources(reply);

    return reply;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WControllerMedia::clearMedia(const QString & url)
{
    Q_D(WControllerMedia);

    QList<WMediaReply *> replies;

    QHashIterator<WRemoteData *, WPrivateMediaData *> i(d->jobs);

    while (i.hasNext())
    {
        i.next();

        foreach (WMediaReply * reply, i.value()->replies)
        {
            if (reply->_url == url)
            {
                replies.append(reply);
            }
        }
    }

    QHashIterator<QIODevice *, WPrivateMediaData *> j(d->queries);

    while (j.hasNext())
    {
        j.next();

        foreach (WMediaReply * reply, j.value()->replies)
        {
            if (reply->_url == url)
            {
                replies.append(reply);
            }
        }
    }

    foreach (WMediaReply * reply, replies)
    {
        delete reply;
    }

    d->sources.remove(url);
}

/* Q_INVOKABLE */ void WControllerMedia::clearMedias()
{
    Q_D(WControllerMedia);

    QList<WMediaReply *> replies;

    QHashIterator<WRemoteData *, WPrivateMediaData *> i(d->jobs);

    while (i.hasNext())
    {
        i.next();

        foreach (WMediaReply * reply, i.value()->replies)
        {
            replies.append(reply);
        }
    }

    QHashIterator<QIODevice *, WPrivateMediaData *> j(d->queries);

    while (j.hasNext())
    {
        j.next();

        foreach (WMediaReply * reply, j.value()->replies)
        {
            replies.append(reply);
        }
    }

    foreach (WMediaReply * reply, replies)
    {
        delete reply;
    }

    d->sources.clear();
}

//-------------------------------------------------------------------------------------------------
// Static functions
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerMedia::generateSource(const QString & url)
{
#ifdef Q_OS_WIN
    if (url.startsWith('/') || url.startsWith('\\') || (url.length() > 1 && url.at(1) == ':'))
#else
    if (url.startsWith('/') || (url.length() > 1 && url.at(1) == ':'))
#endif
    {
        return url;
    }
    else if (WControllerNetwork::urlIsHttp(url) == false)
    {
        if (QUrl(url).scheme().isEmpty())
        {
            return "https://" + url;
        }
        else return url;
    }

    QString source = WControllerNetwork::removeUrlPrefix(url);

    if (source.startsWith("vbml.", Qt::CaseInsensitive) == false) return url;

    int index = source.lastIndexOf('/');

    source.remove(0, index + 1);

    return "vbml:" + source;
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

#ifndef SK_NO_PLAYER

WVlcEngine * WControllerMedia::engine() const
{
    Q_D(const WControllerMedia); return d->engine;
}

#endif

//-------------------------------------------------------------------------------------------------

WAbstractLoader * WControllerMedia::loader() const
{
    Q_D(const WControllerMedia); return d->loader;
}

void WControllerMedia::setLoader(WAbstractLoader * loader)
{
    Q_D(WControllerMedia);

    if (d->loader == loader) return;

    d->loader = loader;

    emit loaderChanged();
}

#endif // SK_NO_CONTROLLERMEDIA

#include "WControllerMedia.moc"
