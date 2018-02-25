//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkCore module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#include "WControllerDownload.h"

#ifndef SK_NO_CONTROLLERDOWNLOAD

// Qt includes
#include <QTime>

// Sk includes
#include <WLoaderNetwork>

W_INIT_CONTROLLER(WControllerDownload)

//-------------------------------------------------------------------------------------------------
// Static variables

static const int CONTROLLERDOWNLOAD_MAX_JOBS = 10;

//=================================================================================================
// WRemoteData
//=================================================================================================
// Private

/* explicit */ WRemoteData::WRemoteData(QObject * parent) : QObject(parent) {}

//-------------------------------------------------------------------------------------------------
// Public

/* virtual */ WRemoteData::~WRemoteData()
{
    W_GET_CONTROLLER(WControllerDownload, controller);

    if (_reply)
    {
        if (_loader)
        {
            _loader->d_func()->abort(_reply);
        }

        if (controller)
        {
            controller->d_func()->completeJob(this);
        }

        _reply->deleteLater();
    }
    else if (controller)
    {
        controller->d_func()->jobsPending.removeOne(this);
    }
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

QByteArray WRemoteData::readAll()
{
    if (_reply)
    {
         return _reply->readAll();
    }
    else return QByteArray();
}

//-------------------------------------------------------------------------------------------------

QIODevice * WRemoteData::takeReply(QObject * parent)
{
    if (_reply)
    {
        QIODevice * reply = _reply;

        _reply = NULL;

        reply->setParent(parent);

        return reply;
    }
    else return NULL;
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

QIODevice * WRemoteData::reply() const
{
    return _reply;
}

QNetworkReply * WRemoteData::networkReply() const
{
    return qobject_cast<QNetworkReply *> (_reply);
}

//-------------------------------------------------------------------------------------------------

QUrl WRemoteData::urlBase() const
{
    return _urlBase;
}

QUrl WRemoteData::url() const
{
    return _url;
}

//-------------------------------------------------------------------------------------------------

QNetworkRequest::Priority WRemoteData::priority() const
{
    return _priority;
}

//-------------------------------------------------------------------------------------------------

bool WRemoteData::redirect() const
{
    return _redirect;
}

bool WRemoteData::cookies() const
{
    return _cookies;
}

bool WRemoteData::header() const
{
    return _header;
}

//-------------------------------------------------------------------------------------------------

int WRemoteData::maxHost() const
{
    return _maxHost;
}

int WRemoteData::delay() const
{
    return _delay;
}

//-------------------------------------------------------------------------------------------------

bool WRemoteData::hasError() const
{
    return (_error.isEmpty() == false);
}

QString WRemoteData::error() const
{
    return _error;
}

//=================================================================================================
// WControllerDownloadPrivate
//=================================================================================================

WControllerDownloadPrivate::WControllerDownloadPrivate(WControllerDownload * p)
    : WControllerPrivate(p) {}

/* virtual */ WControllerDownloadPrivate::~WControllerDownloadPrivate()
{
    W_CLEAR_CONTROLLER(WControllerDownload);
}

//-------------------------------------------------------------------------------------------------

void WControllerDownloadPrivate::init()
{
    Q_Q(WControllerDownload);

    loader = new WLoaderNetwork(q);

    maxJobs = CONTROLLERDOWNLOAD_MAX_JOBS;

    timer.setSingleShot(true);

    QObject::connect(&timer, SIGNAL(timeout()), q, SLOT(onProcess()));
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WControllerDownloadPrivate::processJobs()
{
    if (jobs.count() == maxJobs) return;

    int interval = 0;

    foreach (WRemoteData * data, jobsPending)
    {
        QUrl url = data->_url;

        QString host = url.host();

        if (checkJobs(host, data->_maxHost))
        {
            int delay = checkDelay(data, host);

            if (delay)
            {
                qDebug("Delayed %s %d", url.C_URL, delay);

                if (interval == 0 || delay < interval)
                {
                    interval = delay;
                }

                continue;
            }
            else qDebug("Get %s", url.C_URL);

            jobsPending.removeOne(data);

            jobs.append(data);

            data->_loader->get(data);

            if (jobs.count() == maxJobs) return;
        }
    }

    if (interval)
    {
        timer.start(interval);
    }
}

//-------------------------------------------------------------------------------------------------

bool WControllerDownloadPrivate::checkJobs(const QString & host, int maxHost) const
{
    if (maxHost < 1) return true;

    int count = 0;

    foreach (WRemoteData * data, jobs)
    {
        if (data->_url.host() == host) count++;
    }

    if (count < maxHost)
    {
         return true;
    }
    else return false;
}

int WControllerDownloadPrivate::checkDelay(WRemoteData * data, const QString & host)
{
    if (delays.contains(host) == false)
    {
        int delay = data->_delay;

        if (delay)
        {
            QTime time = QTime::currentTime().addMSecs(delay);

            delays.insert(host, time);
        }

        return 0;
    }

    QTime time = QTime::currentTime();

    QTime timeDelay = delays.value(host);

    if (time < timeDelay)
    {
        return time.msecsTo(timeDelay);
    }

    int delay = data->_delay;

    if (delay)
    {
         delays.insert(host, time.addMSecs(delay));
    }
    else delays.remove(host);

    return 0;
}

//-------------------------------------------------------------------------------------------------

void WControllerDownloadPrivate::completeJob(WRemoteData * data)
{
    jobs.removeOne(data);

    processJobs();
}

//-------------------------------------------------------------------------------------------------

void WControllerDownloadPrivate::clearLoader(WAbstractLoader * loader)
{
    foreach (WRemoteData * data, jobsPending)
    {
        if (data->_loader == loader)
        {
            data->_loader = this->loader;
        }
    }
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WControllerDownloadPrivate::onProcess()
{
    processJobs();
}

//=================================================================================================
// WControllerDownload
//=================================================================================================
// Private

WControllerDownload::WControllerDownload() : WController(new WControllerDownloadPrivate(this)) {}

//-------------------------------------------------------------------------------------------------
// Initialize
//-------------------------------------------------------------------------------------------------

/* virtual */ void WControllerDownload::init()
{
    Q_D(WControllerDownload); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ WRemoteData * WControllerDownload::getData(WAbstractLoader           * loader,
                                                             const QUrl                & url,
                                                             QObject                   * parent,
                                                             QNetworkRequest::Priority   priority,
                                                             bool                        redirect,
                                                             bool                        cookies,
                                                             bool                        header,
                                                             int                         maxHost,
                                                             int                         delay)
{
    Q_D(WControllerDownload);

    WRemoteData * data;

    if (parent) data = new WRemoteData(parent);
    else        data = new WRemoteData(this);

    if (loader)
    {
         data->_loader = loader;
    }
    else data->_loader = d->loader;

    data->_reply = NULL;

    data->_urlBase = url;
    data->_url     = url;

    data->_priority = priority;

    data->_redirect = redirect;
    data->_cookies  = cookies;
    data->_header   = header;

    data->_redirectCount = 0;

    data->_maxHost = maxHost;
    data->_delay   = delay;

    for (int i = 0; i < d->jobsPending.count(); i++)
    {
        if (d->jobsPending.at(i)->_priority > priority)
        {
            d->jobsPending.insert(i, data);

            d->processJobs();

            return data;
        }
    }

    d->jobsPending.append(data);

    d->processJobs();

    return data;
}

/* Q_INVOKABLE */ WRemoteData * WControllerDownload::getData(const QUrl                & url,
                                                             QObject                   * parent,
                                                             QNetworkRequest::Priority   priority,
                                                             bool                        redirect,
                                                             bool                        cookies,
                                                             bool                        header,
                                                             int                         maxHost,
                                                             int                         delay)
{
    return getData(NULL, url, parent, priority, redirect, cookies, header, maxHost, delay);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WControllerDownload::setProxy(const QString & host,
                                                     int             port,
                                                     const QString & password)
{
    Q_D(WControllerDownload);

    d->loader->setProxy(host, port, password);
}

/* Q_INVOKABLE */ void WControllerDownload::clearProxy()
{
    Q_D(WControllerDownload);

    d->loader->clearProxy();
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

WLoaderNetwork * WControllerDownload::loader() const
{
    Q_D(const WControllerDownload); return d->loader;
}

//-------------------------------------------------------------------------------------------------

int WControllerDownload::maxJobs() const
{
    Q_D(const WControllerDownload); return d->maxJobs;
}

void WControllerDownload::setMaxJobs(int max)
{
    Q_D(WControllerDownload);

    if (d->maxJobs == max) return;

    d->maxJobs = max;

    emit maxJobsChanged();
}

#endif // SK_NO_CONTROLLERDOWNLOAD
