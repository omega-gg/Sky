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
#ifdef QT_4
#include <QDeclarativeComponent>
#else
#include <QQmlComponent>
#endif

// Sk includes
#include <WControllerApplication>
#include <WControllerFile>
#include <WControllerDownload>
#include <WControllerPlaylist>
#include <WVlcEngine>
#include <WVlcPlayer>

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
// Properties
//-------------------------------------------------------------------------------------------------

QString WMediaReply::url() const
{
    return _url;
}

//-------------------------------------------------------------------------------------------------

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
// WControllerMediaPrivate
//=================================================================================================

WControllerMediaPrivate::WControllerMediaPrivate(WControllerMedia * p) : WControllerPrivate(p) {}

/* virtual */ WControllerMediaPrivate::~WControllerMediaPrivate()
{
    engine->deleteInstance();

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

    delete engine;

    W_CLEAR_CONTROLLER(WControllerMedia);
}

//-------------------------------------------------------------------------------------------------

void WControllerMediaPrivate::init(const QStringList & options)
{
    Q_Q(WControllerMedia);

    loader = NULL;

    thread = new QThread(q);

    thread->start();

    engine = new WVlcEngine(options, thread);
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WControllerMediaPrivate::loadSources(WMediaReply * reply)
{
    Q_Q(WControllerMedia);

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

    WBackendNet * backend = wControllerPlaylist->backendFromUrl(url);

    if (backend == NULL)
    {
        reply->_medias.insert(WAbstractBackend::Quality720, url);

        reply->_loaded = true;

        return;
    }

    WBackendNetQuery query = backend->getQuerySource(url);

    if (query.isValid() == false)
    {
        backend->tryDelete();

        reply->_medias.insert(WAbstractBackend::Quality720, url);

        reply->_loaded = true;

        return;
    }

    query.priority = QNetworkRequest::HighPriority;

    query.mode = reply->_mode;

    WRemoteData * data = wControllerPlaylist->getData(loader, query, q);

    QObject::connect(data, SIGNAL(loaded(WRemoteData *)), q, SLOT(onLoaded(WRemoteData *)));

    WPrivateMediaData * media = new WPrivateMediaData;

    media->url     = url;
    media->backend = backend;
    media->query   = query;
    media->reply   = NULL;

    media->replies.append(reply);

    medias.append(media);

    jobs.insert(data, media);
}

//-------------------------------------------------------------------------------------------------

void WControllerMediaPrivate::updateSources()
{
    QDateTime date = QDateTime::currentDateTime();

    QMutableHashIterator<QString, WPrivateMediaSource> i(sources);

    while (i.hasNext())
    {
        i.next();

        QDateTime expiry = i.value().expiry;

        if (expiry.isValid() && expiry < date)
        {
            qDebug("MEDIA EXPIRED");

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

            QIODevice * networkReply = media->reply;

            if (networkReply)
            {
                queries.remove(networkReply);
            }

            medias.removeOne(media);

            deleteMedia(media);

            delete data;
        }

        return;
    }
}

//-------------------------------------------------------------------------------------------------

void WControllerMediaPrivate::deleteMedia(WPrivateMediaData * media)
{
    media->backend->tryDelete();

    delete media;
}

//-------------------------------------------------------------------------------------------------

void WControllerMediaPrivate::getData(WPrivateMediaData * media, WBackendNetQuery * query)
{
    Q_Q(WControllerMedia);

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

    if (data->hasError())
    {
        qWarning("WControllerMediaPrivate::onLoaded: Failed to load media %s.", data->url().C_STR);

        media->backend->queryFailed(media->query);

        QString error = data->error();

        foreach (WMediaReply * reply, media->replies)
        {
            reply->_error = error;

            emit reply->loaded(reply);
        }

        medias.removeOne(media);

        deleteMedia(media);
    }
    else
    {
        Q_Q(WControllerMedia);

        QIODevice * reply = data->takeReply(NULL);

        media->reply = reply;

        queries.insert(reply, media);

        media->backend->loadSource(reply, media->query,
                                   q, SLOT(onSourceLoaded(QIODevice *, WBackendNetSource)));
    }

    delete data;
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
            WBackendNetQuery nextQuery = source.nextQuery;

            nextQuery = backendQuery;

            nextQuery.indexReload = indexReload + 1;

            // NOTE: We propagate the compatibility mode.
            nextQuery.mode = backendQuery.mode;

            media->query = nextQuery;

            getData(media, &nextQuery);

            return;
        }
    }

    media->backend->applySource(backendQuery, source);

    if (source.valid)
    {
        const QByteArray & cache = source.cache;

        if (cache.isEmpty() == false)
        {
            wControllerFile->addCache(media->url, cache);
        }
    }

    WBackendNetQuery nextQuery = source.nextQuery;

    int indexNext = backendQuery.indexNext;

    if (nextQuery.isValid() && indexNext < CONTROLLERMEDIA_MAX_QUERY)
    {
        nextQuery.indexNext = indexNext + 1;

        // NOTE: We propagate the compatibility mode.
        nextQuery.mode = backendQuery.mode;

        media->query = nextQuery;

        getData(media, &nextQuery);

        return;
    }

    const QHash<WAbstractBackend::Quality, QString> & medias = source.medias;
    const QHash<WAbstractBackend::Quality, QString> & audios = source.audios;

    if (medias.count())
    {
        WPrivateMediaSource mediaSource;

        mediaSource.medias = source.medias;
        mediaSource.audios = source.audios;

        mediaSource.expiry = source.expiry;

        const QString & url = media->url;

        urls.append(url);

        sources.insert(url, mediaSource);

        foreach (WMediaReply * reply, media->replies)
        {
            reply->_loaded = true;

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

    deleteMedia(media);
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

/* Q_INVOKABLE */ WVlcPlayer * WControllerMedia::createVlcPlayer() const
{
    Q_D(const WControllerMedia);

    return new WVlcPlayer(d->engine, d->thread);
}

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
        qDebug("MEDIA CACHED");

        d->urls.removeOne(url);
        d->urls.append   (url);

        WPrivateMediaSource source = d->sources.value(url);

        reply->_medias = source.medias;
        reply->_audios = source.audios;

        reply->_loaded = true;
    }
    else d->loadSources(reply);

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
// Properties
//-------------------------------------------------------------------------------------------------

WVlcEngine * WControllerMedia::engine() const
{
    Q_D(const WControllerMedia); return d->engine;
}

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
