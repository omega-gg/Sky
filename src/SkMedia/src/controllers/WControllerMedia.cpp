//=================================================================================================
/*
    Copyright (C) 2015-2016 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkMedia module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#include "WControllerMedia.h"

#ifndef SK_NO_CONTROLLERMEDIA

// Qt includes
#include <QThread>
#include <QDeclarativeComponent>

// Sk includes
#include <WControllerDownload>
#include <WControllerPlaylist>
#include <WBackendVlc>
#include <WVlcEngine>
#include <WVlcPlayer>

W_INIT_CONTROLLER(WControllerMedia)

//-------------------------------------------------------------------------------------------------
// Static variables

static const int CONTROLLERMEDIA_CACHE_MAX = 1000;

//=================================================================================================
// WMediaReply
//=================================================================================================
// Private

WMediaReply::WMediaReply(const QUrl & url, QObject * parent) : QObject(parent)
{
    _url = url;

    _backend = NULL;

    _loaded = false;
}

//-------------------------------------------------------------------------------------------------
// Public

/* virtual */ WMediaReply::~WMediaReply()
{
    W_GET_CONTROLLER(WControllerMedia, controller);

    if (controller) controller->d_func()->clearMediaReply(this);
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

QUrl WMediaReply::url() const
{
    return _url;
}

//-------------------------------------------------------------------------------------------------

bool WMediaReply::isLoaded() const
{
    return _loaded;
}

//-------------------------------------------------------------------------------------------------

QHash<WAbstractBackend::Quality, QUrl> WMediaReply::medias() const
{
    return _medias;
}

QHash<WAbstractBackend::Quality, QUrl> WMediaReply::audios() const
{
    return _audios;
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
    delete engine;

    QHashIterator<WRemoteData *, WPrivateMediaData *> i(jobs);

    while (i.hasNext())
    {
        i.next();

        delete i.value();
    }

    jobs.clear();

    thread->quit();
    thread->wait();

    W_CLEAR_CONTROLLER(WControllerMedia);
}

//-------------------------------------------------------------------------------------------------

void WControllerMediaPrivate::init()
{
    Q_Q(WControllerMedia);

    loader = NULL;

    thread = new QThread(q);

    thread->start();

    engine = new WVlcEngine(thread);

    qmlRegisterType<WBackendVlc>("Sky", 1,0, "BackendVlc");
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WControllerMediaPrivate::loadSources(WMediaReply * reply)
{
    Q_Q(WControllerMedia);

    const QUrl & url = reply->_url;

    QHashIterator<WRemoteData *, WPrivateMediaData *> i(jobs);

    WRemoteData       * data  = NULL;
    WPrivateMediaData * media = NULL;

    while (i.hasNext())
    {
         i.next();

         if (i.value()->url == url)
         {
             data  = i.key  ();
             media = i.value();

             break;
         }
    }

    if (data == NULL)
    {
        WBackendNet * backend = wControllerPlaylist->backendFromUrl(url);

        if (backend == NULL)
        {
            reply->_medias.insert(WAbstractBackend::QualityMedium, url);

            reply->_loaded = true;

            return;
        }

        WBackendNetQuery query = backend->getQuerySource(url);

        if (query.isValid() == false)
        {
            reply->_medias.insert(WAbstractBackend::QualityMedium, url);

            reply->_loaded = true;

            return;
        }

        query.priority = QNetworkRequest::HighPriority;

        data = WControllerPlaylist::getDataQuery(loader, query, q);

        if (data == NULL)
        {
            qWarning("WControllerMediaPrivate::loadSources: Failed to load media %s.", url.C_URL);

            return;
        }

        QObject::connect(data, SIGNAL(loaded(WRemoteData *)), q, SLOT(onLoaded(WRemoteData *)));

        media = new WPrivateMediaData;

        media->url     = url;
        media->backend = backend;
        media->query   = query;
        media->reply   = NULL;

        medias.append(media);

        jobs.insert(data, media);
    }

    media->replies.append(reply);
}

//-------------------------------------------------------------------------------------------------

void WControllerMediaPrivate::updateSources()
{
    QDateTime date = QDateTime::currentDateTime();

    QMutableHashIterator<QUrl, WPrivateMediaSource> i(sources);

    while (i.hasNext())
    {
         i.next();

         QDateTime expiry = i.value().expiry;

         if (expiry.isValid() && expiry < date)
         {
             i.remove();
         }
    }

    while (sources.count() > CONTROLLERMEDIA_CACHE_MAX)
    {
        sources.erase(sources.begin());
    }
}

//-------------------------------------------------------------------------------------------------

void WControllerMediaPrivate::clearMediaReply(WMediaReply * reply)
{
    foreach (WPrivateMediaData * media, medias)
    {
        QList<WMediaReply *> * replies = &(media->replies);

        if (replies->contains(reply))
        {
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

                delete media;
                delete data;
            }

            return;
        }
    }
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WControllerMediaPrivate::onLoaded(WRemoteData * data)
{
    WPrivateMediaData * media = jobs.take(data);

    if (data->hasError())
    {
        qWarning("WControllerMediaPrivate::onLoaded: Failed to load media %s.", data->url().C_URL);

        QString error = data->error();

        foreach (WMediaReply * reply, media->replies)
        {
            reply->_error = error;

            emit reply->loaded(reply);
        }

        medias.removeOne(media);

        delete media;
        delete data;
    }
    else
    {
        Q_Q(WControllerMedia);

        QIODevice * reply = data->takeReply(NULL);

        media->reply = reply;

        queries.insert(reply, media);

        media->backend->loadSource(reply, media->query,
                                   q, SLOT(onSourceLoaded(QIODevice *, WBackendNetSource)));

        delete data;
    }
}

//-------------------------------------------------------------------------------------------------

void WControllerMediaPrivate::onSourceLoaded(QIODevice * device, const WBackendNetSource & source)
{
    WPrivateMediaData * media = queries.take(device);

    device->deleteLater();

    if (media == NULL) return;

    media->reply = NULL;

    media->backend->applySource(media->query, source);

    WBackendNetQuery query = source.nextQuery;

    if (query.isValid())
    {
        Q_Q(WControllerMedia);

        media->query = query;

        query.priority
            = static_cast<QNetworkRequest::Priority> (QNetworkRequest::HighPriority - 1);

        WRemoteData * data = WControllerPlaylist::getDataQuery(loader, query, q);

        QObject::connect(data, SIGNAL(loaded(WRemoteData *)), q, SLOT(onLoaded(WRemoteData *)));

        jobs.insertMulti(data, media);
    }
    else
    {
        const QHash<WAbstractBackend::Quality, QUrl> & medias = source.medias;
        const QHash<WAbstractBackend::Quality, QUrl> & audios = source.audios;

        if (medias.count())
        {
            WPrivateMediaSource mediaSource;

            mediaSource.medias = source.medias;
            mediaSource.audios = source.audios;

            mediaSource.expiry = source.expiry;

            sources.insert(media->url, mediaSource);

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

        delete media;
    }
}

//=================================================================================================
// WControllerMedia
//=================================================================================================
// Private

WControllerMedia::WControllerMedia()
    : WController(new WControllerMediaPrivate(this), "WControllerMedia") {}

//-------------------------------------------------------------------------------------------------
// Initialize
//-------------------------------------------------------------------------------------------------

void WControllerMedia::init()
{
    Q_D(WControllerMedia); d->init();
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

/* Q_INVOKABLE */ WMediaReply * WControllerMedia::getMedia(const QUrl & url, QObject * parent)
{
    Q_D(WControllerMedia);

    if (url.isValid() == false) return NULL;

    WMediaReply * reply;

    if (parent) reply = new WMediaReply(url, parent);
    else        reply = new WMediaReply(url, this);

    d->updateSources();

    if (d->sources.contains(url))
    {
        WPrivateMediaSource source = d->sources.value(url);

        reply->_medias = source.medias;
        reply->_audios = source.audios;

        reply->_loaded = true;

        qDebug("CACHED !");
    }
    else d->loadSources(reply);

    return reply;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WControllerMedia::clearMedia(const QUrl & url)
{
    Q_D(WControllerMedia);

    QList<WMediaReply *> media;

    QHashIterator<WRemoteData *, WPrivateMediaData *> i(d->jobs);

    while (i.hasNext())
    {
        i.next();

        foreach (WMediaReply * reply, i.value()->replies)
        {
            if (reply->_url == url)
            {
                media.append(reply);
            }
        }
    }

    foreach (WMediaReply * reply, media)
    {
        delete reply;
    }

    d->sources.remove(url);
}

/* Q_INVOKABLE */ void WControllerMedia::clearMedias()
{
    Q_D(WControllerMedia);

    QList<WMediaReply *> media;

    QHashIterator<WRemoteData *, WPrivateMediaData *> i(d->jobs);

    while (i.hasNext())
    {
        i.next();

        foreach (WMediaReply * reply, i.value()->replies)
        {
            media.append(reply);
        }
    }

    foreach (WMediaReply * reply, media)
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
