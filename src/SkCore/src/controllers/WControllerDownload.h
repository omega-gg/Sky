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

#ifndef WCONTROLLERDOWNLOAD_H
#define WCONTROLLERDOWNLOAD_H

// Qt includes
#include <QNetworkReply>

// Sk includes
#include <WController>
#include <WBackendNet>

#ifndef SK_NO_CONTROLLERDOWNLOAD

// Forward declarations
class WControllerDownloadPrivate;
class WAbstractLoader;
class WLoaderNetwork;

#ifdef QT_6
Q_MOC_INCLUDE("WLoaderNetwork")
#endif

// Defines
#define wControllerDownload WControllerDownload::instance()

//-------------------------------------------------------------------------------------------------
// WRemoteParameters
//-------------------------------------------------------------------------------------------------

struct WRemoteParameters
{
    WRemoteParameters()
    {
        scope = WBackendNetQuery::ScopeDefault;

        redirect = true;
        cookies  = true;

        maxHost = -1;
        delay   =  0;
        timeout = -1;
    }

    WBackendNetQuery::Scope scope;

    QString header;
    QString body;

    bool redirect;
    bool cookies;

    int maxHost;
    int delay;
    int timeout;
};

//-------------------------------------------------------------------------------------------------
// WRemoteData
//-------------------------------------------------------------------------------------------------

class SK_CORE_EXPORT WRemoteData : public QObject
{
    Q_OBJECT

private:
    explicit WRemoteData(QObject * parent = NULL);
public:
    /* virtual */ ~WRemoteData();

public: // Interface
    QByteArray readAll();

    QIODevice * takeReply(QObject * parent = NULL);

    void abort();

signals:
    void loaded(WRemoteData * data);

public: // Properties
    QIODevice     * reply       () const;
    QNetworkReply * networkReply() const;

    QString urlBase() const;
    QString url    () const;

    QString host() const;

    QNetworkRequest::Priority priority() const;

    WRemoteParameters parameters() const;

    WBackendNetQuery::Scope scope() const;

    QString header() const;
    QString body  () const;

    bool redirect() const;
    bool cookies () const;

    int maxHost() const;
    int delay  () const;
    int timeout() const;

    bool hasError() const;

    QString error() const;

private: // Variables
    WAbstractLoader * _loader;

    QIODevice * _reply;

    QString _urlBase;
    QString _url;

    QString _host;

    QNetworkRequest::Priority _priority;

    WRemoteParameters _parameters;

    int _redirectCount;

    QString _error;

private:
    friend class WControllerDownload;
    friend class WControllerDownloadPrivate;
    friend class WAbstractLoader;
    friend class WAbstractLoaderPrivate;
    friend class WRemoteTimeout;
};

//-------------------------------------------------------------------------------------------------
// WControllerDownload
//-------------------------------------------------------------------------------------------------

class SK_CORE_EXPORT WControllerDownload : public WController
{
    Q_OBJECT

    Q_PROPERTY(WLoaderNetwork * loader READ loader CONSTANT)

    Q_PROPERTY(int maxJobs READ maxJobs WRITE setMaxJobs NOTIFY maxJobsChanged)

private:
    WControllerDownload();

protected: // Initialize
    /* virtual */ void init();

public: // Interface
    Q_INVOKABLE
    WRemoteData * getData(WAbstractLoader           * loader,
                          const QString             & url,
                          QObject                   * parent     = NULL,
                          QNetworkRequest::Priority   priority   = QNetworkRequest::NormalPriority,
                          const WRemoteParameters   & parameters = WRemoteParameters());

    Q_INVOKABLE
    WRemoteData * getData(const QString             & url,
                          QObject                   * parent     = NULL,
                          QNetworkRequest::Priority   priority   = QNetworkRequest::NormalPriority,
                          const WRemoteParameters   & parameters = WRemoteParameters());

    Q_INVOKABLE
    WRemoteData * getData(const QString & url, int timeout, QObject * parent = NULL);

    Q_INVOKABLE void setProxy(const QString & host,
                              int             port, const QString & password = QString());

    Q_INVOKABLE void clearProxy();

signals:
    void maxJobsChanged();

public: // Properties
    WLoaderNetwork * loader() const;

    int  maxJobs() const;
    void setMaxJobs(int max);

private:
    W_DECLARE_PRIVATE   (WControllerDownload)
    W_DECLARE_CONTROLLER(WControllerDownload)

    Q_PRIVATE_SLOT(d_func(), void onProcess())

    friend class WRemoteData;
    friend class WAbstractLoader;
    friend class WAbstractLoaderPrivate;
};

#include <private/WControllerDownload_p>

#endif // SK_NO_CONTROLLERDOWNLOAD
#endif // WCONTROLLERDOWNLOAD_H
