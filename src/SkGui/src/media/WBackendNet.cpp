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

#include "WBackendNet.h"

#ifndef SK_NO_BACKENDNET

// Qt includes
#include <QFile>

#ifdef QT_LATEST
#include <QUrlQuery>
#endif

// Sk includes
#include <WControllerApplication>
#include <WControllerNetwork>
#include <WControllerPlaylist>

// Private includes
#include <private/WControllerPlaylist_p>

//=================================================================================================
// WBackendNetQuery
//=================================================================================================

/* explicit */ WBackendNetQuery::WBackendNetQuery(const QUrl & url)
{
    init(WBackendNetQuery::TypeDefault, url);
}

WBackendNetQuery::WBackendNetQuery(Type type, const QUrl & url)
{
    init(type, url);
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

bool WBackendNetQuery::isValid() const
{
    return url.isValid();
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WBackendNetQuery::init(Type type, const QUrl & url)
{
    this->type = type;
    this->url  = url;

    target = TargetDefault;

    id = 0;

    priority = QNetworkRequest::NormalPriority;

    clearItems = true;
    cookies    = false;

    maxHost = 3;
    delay   = 0;
}

//=================================================================================================
// WBackendNetPlaylistInfo
//=================================================================================================

/* explicit */ WBackendNetPlaylistInfo::WBackendNetPlaylistInfo(WLibraryItem::Type type,
                                                                QString            id)
{
    this->type = type;
    this->id   = id;
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

bool WBackendNetPlaylistInfo::isValid() const
{
    if (id.isEmpty())
    {
         return false;
    }
    else return true;
}

//-------------------------------------------------------------------------------------------------

bool WBackendNetPlaylistInfo::isFeed() const
{
    return (type == WLibraryItem::PlaylistFeed);
}

//=================================================================================================
// WBackendNetSource
//=================================================================================================

WBackendNetSource::WBackendNetSource()
{
    valid = true;

    expiry = QDateTime::currentDateTime().addSecs(3600 * 8);
}

//=================================================================================================
// WBackendNetItem
//=================================================================================================

WBackendNetItem::WBackendNetItem()
{
    valid = true;
}

//=================================================================================================
// WBackendNetTrack
//=================================================================================================

WBackendNetTrack::WBackendNetTrack() : WBackendNetItem() {}

//=================================================================================================
// WBackendNetPlaylist
//=================================================================================================

WBackendNetPlaylist::WBackendNetPlaylist() : WBackendNetItem()
{
    currentIndex = -1;

    clearDuplicate = false;
}

//=================================================================================================
// WBackendNetFolder
//=================================================================================================

WBackendNetFolder::WBackendNetFolder() : WBackendNetItem()
{
    currentIndex = -1;

    scanItems = false;
}

//=================================================================================================
// WBackendNetReply
//=================================================================================================
// Protected

WBackendNetReply::WBackendNetReply(QIODevice * device, const WBackendNetQuery & query)
{
    _device = device;
    _query  = query;
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

QIODevice * WBackendNetReply::device() const
{
    return _device;
}

//-------------------------------------------------------------------------------------------------

WBackendNetQuery WBackendNetReply::query() const
{
    return _query;
}

//=================================================================================================
// WNetReplySource
//=================================================================================================

WNetReplySource::WNetReplySource(QIODevice * device, const WBackendNetQuery & query)
    : WBackendNetReply(device, query) {}

//=================================================================================================
// WNetReplyTrack
//=================================================================================================

WNetReplyTrack::WNetReplyTrack(QIODevice * device, const WBackendNetQuery & query)
    : WBackendNetReply(device, query) {}

//=================================================================================================
// WNetReplyPlaylist
//=================================================================================================

WNetReplyPlaylist::WNetReplyPlaylist(QIODevice * device, const WBackendNetQuery & query)
    : WBackendNetReply(device, query) {}

//=================================================================================================
// WNetReplyFolder
//=================================================================================================

WNetReplyFolder::WNetReplyFolder(QIODevice * device, const WBackendNetQuery & query)
    : WBackendNetReply(device, query) {}

//=================================================================================================
// WBackendNetPrivate
//=================================================================================================

WBackendNetPrivate::WBackendNetPrivate(WBackendNet * p) : WPrivate(p) {}

/* virtual */ WBackendNetPrivate::~WBackendNetPrivate()
{
    Q_Q(WBackendNet);

    W_GET_CONTROLLER(WControllerPlaylist, controller);

    if (controller) controller->d_func()->unregisterBackend(q);
}

//-------------------------------------------------------------------------------------------------

void WBackendNetPrivate::init()
{
    Q_Q(WBackendNet);

    const QMetaObject * meta = q->metaObject();

    methodSource   = meta->method(meta->indexOfMethod("onLoadSource(WNetReplySource*)"));
    methodTrack    = meta->method(meta->indexOfMethod("onLoadTrack(WNetReplyTrack*)"));
    methodPlaylist = meta->method(meta->indexOfMethod("onLoadPlaylist(WNetReplyPlaylist*)"));
    methodFolder   = meta->method(meta->indexOfMethod("onLoadFolder(WNetReplyFolder*)"));

    q->moveToThread(wControllerPlaylist->d_func()->thread);

    wControllerPlaylist->d_func()->registerBackend(q);
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WBackendNetPrivate::onLoadSource(WNetReplySource * reply) const
{
    Q_Q(const WBackendNet);

    QByteArray data = reply->_device->readAll();

    WBackendNetSource source;

    if (data.isEmpty() == false)
    {
        source = q->extractSource(data, reply->_query);
    }

    emit reply->loaded(reply->_device, source);

    reply->deleteLater();
}

void WBackendNetPrivate::onLoadTrack(WNetReplyTrack * reply) const
{
    Q_Q(const WBackendNet);

    QByteArray data = reply->_device->readAll();

    WBackendNetTrack track;

    if (data.isEmpty() == false)
    {
        track = q->extractTrack(data, reply->_query);
    }

    emit reply->loaded(reply->_device, track);

    reply->deleteLater();
}

void WBackendNetPrivate::onLoadPlaylist(WNetReplyPlaylist * reply) const
{
    Q_Q(const WBackendNet);

    QByteArray data = reply->_device->readAll();

    WBackendNetPlaylist playlist;

    if (data.isEmpty() == false)
    {
        playlist = q->extractPlaylist(data, reply->_query);
    }

    emit reply->loaded(reply->_device, playlist);

    reply->deleteLater();
}

void WBackendNetPrivate::onLoadFolder(WNetReplyFolder * reply) const
{
    Q_Q(const WBackendNet);

    QByteArray data = reply->_device->readAll();

    WBackendNetFolder folder;

    if (data.isEmpty() == false)
    {
        folder = q->extractFolder(data, reply->_query);
    }

    emit reply->loaded(reply->_device, folder);

    reply->deleteLater();
}

//=================================================================================================
// WBackendNet
//=================================================================================================
// Protected

WBackendNet::WBackendNet(WBackendNetPrivate * p) : WPrivatable(p)
{
    Q_D(WBackendNet); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ WLibraryItem::Type WBackendNet::getPlaylistType(const QUrl & url) const
{
    return getPlaylistInfo(url).type;
}

/* Q_INVOKABLE */ QString WBackendNet::getPlaylistId(const QUrl & url) const
{
    return getPlaylistInfo(url).id;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ QUrl WBackendNet::getSourceTrack(const QUrl & url) const
{
    QString id = getTrackId(url);

    if (id.isEmpty())
    {
         return QUrl();
    }
    else return getUrlTrack(id);
}

/* Q_INVOKABLE */ QUrl WBackendNet::getSourcePlaylist(const QUrl & url) const
{
    WBackendNetPlaylistInfo info = getPlaylistInfo(url);

    if (info.isValid())
    {
         return getUrlPlaylist(info);
    }
    else return QUrl();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ bool WBackendNet::checkQuery(const QUrl & source) const
{
    if (source.host() == sk->applicationUrl().host())
    {
         return true;
    }
    else return false;
}

/* Q_INVOKABLE */ WBackendNetQuery WBackendNet::extractQuery(const QUrl & source) const
{
#ifdef QT_4
    QString method = source.queryItemValue("method");
    QString label  = source.queryItemValue("label");
    QString q      = source.queryItemValue("q");
#else
    QUrlQuery query(source);

    QString method = query.queryItemValue("method");
    QString label  = query.queryItemValue("label");
    QString q      = query.queryItemValue("q");
#endif

    return createQuery(method, label, WControllerNetwork::decodeUrl(q));
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WBackendNet::loadSource(QIODevice              * device,
                                               const WBackendNetQuery & query,
                                               QObject                * receiver,
                                               const char             * method)
{
    Q_D(WBackendNet);

    WNetReplySource * reply = new WNetReplySource(device, query);

    connect(reply, SIGNAL(loaded(QIODevice *, WBackendNetSource)), receiver, method);

    device->moveToThread(thread());

    d->methodSource.invoke(this, Q_ARG(WNetReplySource *, reply));
}

/* Q_INVOKABLE */ void WBackendNet::loadTrack(QIODevice              * device,
                                              const WBackendNetQuery & query,
                                              QObject                * receiver,
                                              const char             * method)
{
    Q_D(WBackendNet);

    WNetReplyTrack * reply = new WNetReplyTrack(device, query);

    connect(reply, SIGNAL(loaded(QIODevice *, WBackendNetTrack)), receiver, method);

    device->moveToThread(thread());

    d->methodTrack.invoke(this, Q_ARG(WNetReplyTrack *, reply));
}

/* Q_INVOKABLE */ void WBackendNet::loadPlaylist(QIODevice              * device,
                                                 const WBackendNetQuery & query,
                                                 QObject                * receiver,
                                                 const char             * method)
{
    Q_D(WBackendNet);

    WNetReplyPlaylist * reply = new WNetReplyPlaylist(device, query);

    connect(reply, SIGNAL(loaded(QIODevice *, WBackendNetPlaylist)), receiver, method);

    device->moveToThread(thread());

    d->methodPlaylist.invoke(this, Q_ARG(WNetReplyPlaylist *, reply));
}

/* Q_INVOKABLE */ void WBackendNet::loadFolder(QIODevice              * device,
                                               const WBackendNetQuery & query,
                                               QObject                * receiver,
                                               const char             * method)
{
    Q_D(WBackendNet);

    WNetReplyFolder * reply = new WNetReplyFolder(device, query);

    connect(reply, SIGNAL(loaded(QIODevice *, WBackendNetFolder)), receiver, method);

    device->moveToThread(thread());

    d->methodFolder.invoke(this, Q_ARG(WNetReplyFolder *, reply));
}

//-------------------------------------------------------------------------------------------------
// Virtual interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ bool WBackendNet::isHub() const
{
    return false;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ bool WBackendNet::isSearchEngine() const
{
    return false;
}

/* Q_INVOKABLE virtual */ bool WBackendNet::isSearchCover() const
{
    return false;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ bool WBackendNet::checkValidUrl(const QUrl &) const
{
    return false;
}

/* Q_INVOKABLE virtual */ bool WBackendNet::checkCover(const QString &, const QString &) const
{
    return false;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QString WBackendNet::getHost() const
{
    return QString();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QList<WLibraryFolderItem> WBackendNet::getLibraryItems() const
{
    return QList<WLibraryFolderItem>();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QString WBackendNet::getTrackId(const QUrl &) const
{
    return QString();
}

/* Q_INVOKABLE virtual */ WAbstractBackend::Output WBackendNet::getTrackOutput(const QUrl &) const
{
    return WAbstractBackend::OutputMedia;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ WBackendNetPlaylistInfo WBackendNet::getPlaylistInfo(const QUrl &) const
{
    return WBackendNetPlaylistInfo();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
QUrl WBackendNet::getUrlTrack(const QString &) const
{
    qWarning("WBackendNet::getUrlTrack: Not supported by backend.");

    return QUrl();
}

/* Q_INVOKABLE virtual */
QUrl WBackendNet::getUrlPlaylist(const WBackendNetPlaylistInfo &) const
{
    qWarning("WBackendNet::getUrlPlaylist: Not supported by backend.");

    return QUrl();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
WBackendNetQuery WBackendNet::getQuerySource(const QUrl &) const
{
    return WBackendNetQuery();
}

/* Q_INVOKABLE virtual */
WBackendNetQuery WBackendNet::getQueryTrack(const QUrl &) const
{
    return WBackendNetQuery();
}

/* Q_INVOKABLE virtual */
WBackendNetQuery WBackendNet::getQueryPlaylist(const QUrl &) const
{
    return WBackendNetQuery();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
WBackendNetQuery WBackendNet::createQuery(const QString &, const QString &, const QString &) const
{
    return WBackendNetQuery();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
WBackendNetSource WBackendNet::extractSource(const QByteArray       &,
                                             const WBackendNetQuery &) const
{
    qWarning("WBackendNet::extractSource: Not supported by backend.");

    return WBackendNetSource();
}

/* Q_INVOKABLE virtual */
WBackendNetTrack WBackendNet::extractTrack(const QByteArray       &,
                                           const WBackendNetQuery &) const
{
    qWarning("WBackendNet::extractTrack: Not supported by backend.");

    return WBackendNetTrack();
}

/* Q_INVOKABLE virtual */
WBackendNetPlaylist WBackendNet::extractPlaylist(const QByteArray       &,
                                                 const WBackendNetQuery &) const
{
    qWarning("WBackendNet::extractPlaylist: Not supported by backend.");

    return WBackendNetPlaylist();
}

/* Q_INVOKABLE virtual */
WBackendNetFolder WBackendNet::extractFolder(const QByteArray       &,
                                             const WBackendNetQuery &) const
{
    qWarning("WBackendNet::extractFolder: Not supported by backend.");

    return WBackendNetFolder();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ void WBackendNet::queryFailed(const WBackendNetQuery &) {}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ void WBackendNet::applySource(const WBackendNetQuery  &,
                                                        const WBackendNetSource &) {}

/* Q_INVOKABLE virtual */ void WBackendNet::applyTrack(const WBackendNetQuery &,
                                                       const WBackendNetTrack &) {}

/* Q_INVOKABLE virtual */ void WBackendNet::applyPlaylist(const WBackendNetQuery    &,
                                                          const WBackendNetPlaylist &) {}

/* Q_INVOKABLE virtual */ void WBackendNet::applyFolder(const WBackendNetQuery  &,
                                                        const WBackendNetFolder &) {}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

QString WBackendNet::id() const
{
    return getId();
}

QString WBackendNet::title() const
{
    return getTitle();
}

#endif // SK_NO_BACKENDNET
