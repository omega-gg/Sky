//=================================================================================================
/*
    Copyright (C) 2015-2016 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkCore module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#ifndef WCONTROLLERDOWNLOAD_H
#define WCONTROLLERDOWNLOAD_H

// Qt includes
#include <QNetworkReply>

// Sk includes
#include <WController.h>

#ifndef SK_NO_CONTROLLERDOWNLOAD

// Forward declarations
class WControllerDownloadPrivate;
class WAbstractLoader;
class WLoaderNetwork;

// Defines
#define wControllerDownload WControllerDownload::instance()

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

signals:
    void loaded(WRemoteData * data);

public: // Properties
    QIODevice     * reply       () const;
    QNetworkReply * networkReply() const;

    QUrl urlBase() const;
    QUrl url    () const;

    QNetworkRequest::Priority priority() const;

    bool redirect() const;
    bool cookies () const;

    int maxHost() const;
    int delay  () const;

    bool hasError() const;

    QString error() const;

private: // Variables
    WAbstractLoader * _loader;

    QIODevice * _reply;

    QUrl _urlBase;
    QUrl _url;

    QNetworkRequest::Priority _priority;

    bool _redirect;
    bool _cookies;

    int _redirectCount;

    int _maxHost;
    int _delay;

    QString _error;

private:
    friend class WControllerDownload;
    friend class WControllerDownloadPrivate;
    friend class WAbstractLoader;
    friend class WAbstractLoaderPrivate;
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
                          const QUrl                & url,
                          QObject                   * parent   = NULL,
                          QNetworkRequest::Priority   priority = QNetworkRequest::NormalPriority,
                          bool                        redirect = true,
                          bool                        cookies  = true,
                          int                         maxHost  = -1,
                          int                         delay    =  0);

    Q_INVOKABLE
    WRemoteData * getData(const QUrl                & url,
                          QObject                   * parent   = NULL,
                          QNetworkRequest::Priority   priority = QNetworkRequest::NormalPriority,
                          bool                        redirect = true,
                          bool                        cookies  = true,
                          int                         maxHost  = -1,
                          int                         delay    =  0);

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
