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
#include <QQmlEngine>
#include <QUrlQuery>
#endif

// Sk includes
#include <WControllerApplication>
#ifdef QT_LATEST
#include <WControllerDeclarative>
#endif
#include <WControllerNetwork>
#include <WControllerPlaylist>

// Private includes
#include <private/WControllerPlaylist_p>

//=================================================================================================
// WBackendNetQuery
//=================================================================================================

/* explicit */ WBackendNetQuery::WBackendNetQuery(const QString & url)
{
    init(WBackendNetQuery::TypeDefault, url);
}

WBackendNetQuery::WBackendNetQuery(Type type, const QString & url)
{
    init(type, url);
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

bool WBackendNetQuery::isValid() const
{
    return (url.isEmpty() == false);
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WBackendNetQuery::init(Type type, const QString & url)
{
    this->type = type;
    this->url  = url;

    target = TargetDefault;

    id    = 0;
    index = 0;

    priority = QNetworkRequest::NormalPriority;

    clearItems = true;
    cookies    = false;
    header     = false;
    skipError  = false;

    maxHost =  3;
    delay   =  0;
    timeout = -1;
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
// WBackendNetBase
//=================================================================================================

WBackendNetBase::WBackendNetBase()
{
    valid = true;
}

//=================================================================================================
// WBackendNetTrack
//=================================================================================================

WBackendNetTrack::WBackendNetTrack() : WBackendNetBase() {}

//=================================================================================================
// WBackendNetPlaylist
//=================================================================================================

WBackendNetPlaylist::WBackendNetPlaylist() : WBackendNetBase()
{
    currentIndex = -1;

    clearDuplicate = false;
}

//=================================================================================================
// WBackendNetFolder
//=================================================================================================

WBackendNetFolder::WBackendNetFolder() : WBackendNetBase()
{
    currentIndex = -1;

    clearDuplicate = false;
    scanItems      = false;
}

//=================================================================================================
// WBackendNetItem
//=================================================================================================

WBackendNetItem::WBackendNetItem() : WBackendNetBase() {}

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
// WNetReplyItem
//=================================================================================================

WNetReplyItem::WNetReplyItem(QIODevice * device, const WBackendNetQuery & query)
    : WBackendNetReply(device, query) {}

//=================================================================================================
// WBackendNetInterface
//=================================================================================================

class WBackendNetInterface : public QObject
{
    Q_OBJECT

public:
    WBackendNetInterface();

public: // Functions
    Q_INVOKABLE void loadSource  (WBackendNet * backend, WNetReplySource   * reply);
    Q_INVOKABLE void loadTrack   (WBackendNet * backend, WNetReplyTrack    * reply);
    Q_INVOKABLE void loadPlaylist(WBackendNet * backend, WNetReplyPlaylist * reply);
    Q_INVOKABLE void loadFolder  (WBackendNet * backend, WNetReplyFolder   * reply);
    Q_INVOKABLE void loadItem    (WBackendNet * backend, WNetReplyItem     * reply);

private: // Slots
    Q_INVOKABLE void onSource  (WBackendNet * backend, WNetReplySource   * reply) const;
    Q_INVOKABLE void onTrack   (WBackendNet * backend, WNetReplyTrack    * reply) const;
    Q_INVOKABLE void onPlaylist(WBackendNet * backend, WNetReplyPlaylist * reply) const;
    Q_INVOKABLE void onFolder  (WBackendNet * backend, WNetReplyFolder   * reply) const;
    Q_INVOKABLE void onItem    (WBackendNet * backend, WNetReplyItem     * reply) const;

public: // Variables
    QMetaMethod source;
    QMetaMethod track;
    QMetaMethod playlist;
    QMetaMethod folder;
    QMetaMethod item;
};

Q_GLOBAL_STATIC(WBackendNetInterface, backendInterface)

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

WBackendNetInterface::WBackendNetInterface()
{
    const QMetaObject * meta = metaObject();

    source   = meta->method(meta->indexOfMethod("onSource(WBackendNet*,WNetReplySource*)"));
    track    = meta->method(meta->indexOfMethod("onTrack(WBackendNet*,WNetReplyTrack*)"));
    playlist = meta->method(meta->indexOfMethod("onPlaylist(WBackendNet*,WNetReplyPlaylist*)"));
    folder   = meta->method(meta->indexOfMethod("onFolder(WBackendNet*,WNetReplyFolder*)"));
    item     = meta->method(meta->indexOfMethod("onItem(WBackendNet*,WNetReplyItem*)"));

    moveToThread(wControllerPlaylist->d_func()->thread);
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WBackendNetInterface::loadSource(WBackendNet     * backend,
                                                        WNetReplySource * reply)
{
    source.invoke(this, Q_ARG(WBackendNet *, backend), Q_ARG(WNetReplySource *, reply));
}

/* Q_INVOKABLE */ void WBackendNetInterface::loadTrack(WBackendNet    * backend,
                                                       WNetReplyTrack * reply)
{
    track.invoke(this, Q_ARG(WBackendNet *, backend), Q_ARG(WNetReplyTrack *, reply));
}

/* Q_INVOKABLE */ void WBackendNetInterface::loadPlaylist(WBackendNet       * backend,
                                                          WNetReplyPlaylist * reply)
{
    playlist.invoke(this, Q_ARG(WBackendNet *, backend), Q_ARG(WNetReplyPlaylist *, reply));
}

/* Q_INVOKABLE */ void WBackendNetInterface::loadFolder(WBackendNet     * backend,
                                                        WNetReplyFolder * reply)
{
    folder.invoke(this, Q_ARG(WBackendNet *, backend), Q_ARG(WNetReplyFolder *, reply));
}

/* Q_INVOKABLE */ void WBackendNetInterface::loadItem(WBackendNet   * backend,
                                                      WNetReplyItem * reply)
{
    item.invoke(this, Q_ARG(WBackendNet *, backend), Q_ARG(WNetReplyItem *, reply));
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WBackendNetInterface::onSource(WBackendNet     * backend,
                                                      WNetReplySource * reply) const
{
    QByteArray data = reply->_device->readAll();

    WBackendNetSource source = backend->extractSource(data, reply->_query);

    emit reply->loaded(reply->_device, source);

    reply->deleteLater();
}

/* Q_INVOKABLE */ void WBackendNetInterface::onTrack(WBackendNet    * backend,
                                                     WNetReplyTrack * reply) const
{
    QByteArray data = reply->_device->readAll();

    WBackendNetTrack track = backend->extractTrack(data, reply->_query);

    emit reply->loaded(reply->_device, track);

    reply->deleteLater();
}

/* Q_INVOKABLE */ void WBackendNetInterface::onPlaylist(WBackendNet       * backend,
                                                        WNetReplyPlaylist * reply) const
{
    QByteArray data = reply->_device->readAll();

    WBackendNetPlaylist playlist = backend->extractPlaylist(data, reply->_query);

    emit reply->loaded(reply->_device, playlist);

    reply->deleteLater();
}

/* Q_INVOKABLE */ void WBackendNetInterface::onFolder(WBackendNet     * backend,
                                                      WNetReplyFolder * reply) const
{
    QByteArray data = reply->_device->readAll();

    WBackendNetFolder folder = backend->extractFolder(data, reply->_query);

    emit reply->loaded(reply->_device, folder);

    reply->deleteLater();
}

/* Q_INVOKABLE */ void WBackendNetInterface::onItem(WBackendNet   * backend,
                                                    WNetReplyItem * reply) const
{
    QByteArray data = reply->_device->readAll();

    WBackendNetItem item = backend->extractItem(data, reply->_query);

    emit reply->loaded(reply->_device, item);

    reply->deleteLater();
}

//=================================================================================================
// WBackendNetPrivate
//=================================================================================================

WBackendNetPrivate::WBackendNetPrivate(WBackendNet * p) : WPrivate(p) {}

//-------------------------------------------------------------------------------------------------

void WBackendNetPrivate::init()
{
    Q_Q(WBackendNet);

    lockCount = 0;

#ifdef QT_LATEST
    wControllerDeclarative->engine()->setObjectOwnership(q, QQmlEngine::CppOwnership);
#endif
}

//=================================================================================================
// WBackendNet
//=================================================================================================
// Protected

WBackendNet::WBackendNet(WBackendNetPrivate * p) : QObject(), WPrivatable(p)
{
    Q_D(WBackendNet); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ WLibraryItem::Type WBackendNet::getPlaylistType(const QString & url) const
{
    return getPlaylistInfo(url).type;
}

/* Q_INVOKABLE */ QString WBackendNet::getPlaylistId(const QString & url) const
{
    return getPlaylistInfo(url).id;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ QString WBackendNet::getSourceTrack(const QString & url) const
{
    QString id = getTrackId(url);

    if (id.isEmpty())
    {
         return QString();
    }
    else return getUrlTrack(id);
}

/* Q_INVOKABLE */ QString WBackendNet::getSourcePlaylist(const QString & url) const
{
    WBackendNetPlaylistInfo info = getPlaylistInfo(url);

    if (info.isValid())
    {
         return getUrlPlaylist(info);
    }
    else return QString();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ bool WBackendNet::checkQuery(const QString & url) const
{
    return url.startsWith(sk->applicationUrl());
}

/* Q_INVOKABLE */ WBackendNetQuery WBackendNet::extractQuery(const QString & url) const
{    
    QUrl source(url);

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
    WNetReplySource * reply = new WNetReplySource(device, query);

    connect(reply, SIGNAL(loaded(QIODevice *, WBackendNetSource)), receiver, method);

    device->moveToThread(thread());

    backendInterface()->loadSource(this, reply);
}

/* Q_INVOKABLE */ void WBackendNet::loadTrack(QIODevice              * device,
                                              const WBackendNetQuery & query,
                                              QObject                * receiver,
                                              const char             * method)
{
    WNetReplyTrack * reply = new WNetReplyTrack(device, query);

    connect(reply, SIGNAL(loaded(QIODevice *, WBackendNetTrack)), receiver, method);

    device->moveToThread(thread());

    backendInterface()->loadTrack(this, reply);
}

/* Q_INVOKABLE */ void WBackendNet::loadPlaylist(QIODevice              * device,
                                                 const WBackendNetQuery & query,
                                                 QObject                * receiver,
                                                 const char             * method)
{
    WNetReplyPlaylist * reply = new WNetReplyPlaylist(device, query);

    connect(reply, SIGNAL(loaded(QIODevice *, WBackendNetPlaylist)), receiver, method);

    device->moveToThread(thread());

    backendInterface()->loadPlaylist(this, reply);
}

/* Q_INVOKABLE */ void WBackendNet::loadFolder(QIODevice              * device,
                                               const WBackendNetQuery & query,
                                               QObject                * receiver,
                                               const char             * method)
{
    WNetReplyFolder * reply = new WNetReplyFolder(device, query);

    connect(reply, SIGNAL(loaded(QIODevice *, WBackendNetFolder)), receiver, method);

    device->moveToThread(thread());

    backendInterface()->loadFolder(this, reply);
}

/* Q_INVOKABLE */ void WBackendNet::loadItem(QIODevice              * device,
                                             const WBackendNetQuery & query,
                                             QObject                * receiver,
                                             const char             * method)
{
    WNetReplyItem * reply = new WNetReplyItem(device, query);

    connect(reply, SIGNAL(loaded(QIODevice *, WBackendNetItem)), receiver, method);

    device->moveToThread(thread());

    backendInterface()->loadItem(this, reply);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WBackendNet::tryDelete()
{
    Q_D(WBackendNet);

    if (d->lockCount) d->lockCount--;
}

//-------------------------------------------------------------------------------------------------
// Virtual interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ void WBackendNet::reload() {}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ bool WBackendNet::hasSearch() const
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

/* Q_INVOKABLE virtual */ QString WBackendNet::validate() const
{
    return QString();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ bool WBackendNet::checkCover(const QString &, const QString &) const
{
    return false;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QString WBackendNet::getHost() const
{
    return QString();
}

/* Q_INVOKABLE virtual */ QString WBackendNet::getCover() const
{
    return QString();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QList<WLibraryFolderItem> WBackendNet::getLibraryItems() const
{
    return QList<WLibraryFolderItem>();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QString WBackendNet::getTrackId(const QString &) const
{
    return QString();
}

/* Q_INVOKABLE virtual */
WAbstractBackend::Output WBackendNet::getTrackOutput(const QString &) const
{
    return WAbstractBackend::OutputMedia;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
WBackendNetPlaylistInfo WBackendNet::getPlaylistInfo(const QString &) const
{
    return WBackendNetPlaylistInfo();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
QString WBackendNet::getUrlTrack(const QString &) const
{
    qWarning("WBackendNet::getUrlTrack: Not supported.");

    return QString();
}

/* Q_INVOKABLE virtual */
QString WBackendNet::getUrlPlaylist(const WBackendNetPlaylistInfo &) const
{
    qWarning("WBackendNet::getUrlPlaylist: Not supported.");

    return QString();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
WBackendNetQuery WBackendNet::getQuerySource(const QString &) const
{
    return WBackendNetQuery();
}

/* Q_INVOKABLE virtual */
WBackendNetQuery WBackendNet::getQueryTrack(const QString &) const
{
    return WBackendNetQuery();
}

/* Q_INVOKABLE virtual */
WBackendNetQuery WBackendNet::getQueryPlaylist(const QString &) const
{
    return WBackendNetQuery();
}

/* Q_INVOKABLE virtual */
WBackendNetQuery WBackendNet::getQueryFolder(const QString &) const
{
    return WBackendNetQuery();
}

/* Q_INVOKABLE virtual */
WBackendNetQuery WBackendNet::getQueryItem(const QString &) const
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
    qWarning("WBackendNet::extractSource: Not supported.");

    return WBackendNetSource();
}

/* Q_INVOKABLE virtual */
WBackendNetTrack WBackendNet::extractTrack(const QByteArray       &,
                                           const WBackendNetQuery &) const
{
    qWarning("WBackendNet::extractTrack: Not supported.");

    return WBackendNetTrack();
}

/* Q_INVOKABLE virtual */
WBackendNetPlaylist WBackendNet::extractPlaylist(const QByteArray       &,
                                                 const WBackendNetQuery &) const
{
    qWarning("WBackendNet::extractPlaylist: Not supported.");

    return WBackendNetPlaylist();
}

/* Q_INVOKABLE virtual */
WBackendNetFolder WBackendNet::extractFolder(const QByteArray       &,
                                             const WBackendNetQuery &) const
{
    qWarning("WBackendNet::extractFolder: Not supported.");

    return WBackendNetFolder();
}

/* Q_INVOKABLE virtual */
WBackendNetItem WBackendNet::extractItem(const QByteArray       &,
                                         const WBackendNetQuery &) const
{
    qWarning("WBackendNet::extractItem: Not supported.");

    return WBackendNetItem();
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

/* Q_INVOKABLE virtual */ void WBackendNet::applyItem(const WBackendNetQuery &,
                                                      const WBackendNetItem  &) {}

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

#include "WBackendNet.moc"
