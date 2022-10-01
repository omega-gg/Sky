//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkCore.

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

#include "WControllerDownload.h"

#ifndef SK_NO_CONTROLLERDOWNLOAD

// Qt includes
#include <QTime>

// Sk includes
#include <WLoaderNetwork>

W_INIT_CONTROLLER(WControllerDownload)

//-------------------------------------------------------------------------------------------------
// Static variables

static const int CONTROLLERDOWNLOAD_MAX_JOBS = 16;

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
            _loader->d_func()->clear(_reply);
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

void WRemoteData::abort()
{
    if (_loader && _reply)
    {
        _loader->abort(_reply);
    }
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

QString WRemoteData::urlBase() const
{
    return _urlBase;
}

QString WRemoteData::url() const
{
    return _url;
}

//-------------------------------------------------------------------------------------------------

QString WRemoteData::host() const
{
    return _host;
}

//-------------------------------------------------------------------------------------------------

QNetworkRequest::Priority WRemoteData::priority() const
{
    return _priority;
}

//-------------------------------------------------------------------------------------------------

WRemoteParameters WRemoteData::parameters() const
{
    return _parameters;
}

//-------------------------------------------------------------------------------------------------

QString WRemoteData::header() const
{
    return _parameters.header;
}

QString WRemoteData::body() const
{
    return _parameters.body;
}

//-------------------------------------------------------------------------------------------------

bool WRemoteData::redirect() const
{
    return _parameters.redirect;
}

bool WRemoteData::cookies() const
{
    return _parameters.cookies;
}

//-------------------------------------------------------------------------------------------------

int WRemoteData::maxHost() const
{
    return _parameters.maxHost;
}

int WRemoteData::delay() const
{
    return _parameters.delay;
}

int WRemoteData::timeout() const
{
    return _parameters.timeout;
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
// WRemoteTimeout
//=================================================================================================

class SK_CORE_EXPORT WRemoteTimeout : public QTimer
{
    Q_OBJECT

public:
    WRemoteTimeout(WRemoteData * data);

private slots:
    void onTimeout();
    void onLoaded ();

private: // Variables
    WRemoteData * _data;
};

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

WRemoteTimeout::WRemoteTimeout(WRemoteData * data) : QTimer(data)
{
    _data = data;

    setSingleShot(true);

    connect(this, SIGNAL(timeout()), this, SLOT(onTimeout()));

    connect(_data, SIGNAL(loaded(WRemoteData *)), this, SLOT(onLoaded()));

    start(data->_parameters.timeout);
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WRemoteTimeout::onTimeout()
{
    qDebug("TIMEOUT");

    _data->abort();
}

void WRemoteTimeout::onLoaded()
{
    stop();

    deleteLater();
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
        QString url  = data->_url;
        QString host = data->_host;

        const WRemoteParameters & parameters = data->_parameters;

        if (checkJobs(host, parameters.maxHost))
        {
            int delay = checkDelay(data, host);

            if (delay)
            {
                qDebug("Delayed %s %d", url.C_STR, delay);

                if (interval == 0 || delay < interval)
                {
                    interval = delay;
                }

                continue;
            }
            else qDebug("Get %s", url.C_STR);

            jobsPending.removeOne(data);

            jobs.append(data);

            data->_loader->get(data);

            if (data->_reply && parameters.timeout != -1)
            {
                new WRemoteTimeout(data);
            }

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
        if (data->_host == host) count++;
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
        int delay = data->_parameters.delay;

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

    int delay = data->_parameters.delay;

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

/* Q_INVOKABLE */
WRemoteData * WControllerDownload::getData(WAbstractLoader           * loader,
                                           const QString             & url,
                                           QObject                   * parent,
                                           QNetworkRequest::Priority   priority,
                                           const WRemoteParameters   & parameters)
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

    data->_host = QUrl(url).host();

    data->_priority = priority;

    data->_parameters = parameters;

    data->_redirectCount = 0;

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

/* Q_INVOKABLE */
WRemoteData * WControllerDownload::getData(const QString             & url,
                                           QObject                   * parent,
                                           QNetworkRequest::Priority   priority,
                                           const WRemoteParameters   & parameters)
{
    return getData(NULL, url, parent, priority, parameters);
}

/* Q_INVOKABLE */
WRemoteData * WControllerDownload::getData(const QString & url,
                                           int             timeout,
                                           QObject       * parent)
{
    WRemoteParameters parameters;

    parameters.timeout = timeout;

    return getData(NULL, url, parent, QNetworkRequest::NormalPriority, parameters);
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

#include "WControllerDownload.moc"
