//=================================================================================================
/*
    Copyright (C) 2015-2026 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkTorrent.

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

#include "WDownloadHookTorrent.h"

#ifndef SK_NO_DOWNLOADHOOKTORRENT

// Sk includes
#include <WControllerApplication>
#include <WControllerNetwork>
#include <WControllerPlaylist>
#include <WControllerTorrent>

//-------------------------------------------------------------------------------------------------
// Static variables

// NOTE: The progress we leave for the copy out of the torrent cache.
static const int DOWNLOADHOOKTORRENT_PROGRESS = 99;

// NOTE: The received bytes go out with every megabyte.
static const int DOWNLOADHOOKTORRENT_STEP = 1048576;

//=================================================================================================
// WDownloadHookTorrentPrivate
//=================================================================================================

WDownloadHookTorrentPrivate::WDownloadHookTorrentPrivate(WDownloadHookTorrent * p)
    : WAbstractDownloadHookPrivate(p) {}

void WDownloadHookTorrentPrivate::init()
{
    Q_Q(WDownloadHookTorrent);

    // NOTE: Our replies have to go while the torrent controller is still there.
    QObject::connect(sk, SIGNAL(aboutToQuit()), q, SLOT(onAboutToQuit()));
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WDownloadHookTorrentPrivate::clearReply(QObject * reply)
{
    Q_Q(WDownloadHookTorrent);

    QObject::disconnect(reply, NULL, q, NULL);

    downloads.remove(reply);

    // NOTE: This can happen during one of the reply's signals.
    reply->deleteLater();
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

// NOTE: Without a file index we get every file of the torrent, so we pick the first media.
void WDownloadHookTorrentPrivate::onAdded(WTorrentReply * reply)
{
    Q_Q(WDownloadHookTorrent);

    WTorrent * torrent = reply->torrent();

    foreach (const QString & path, torrent->paths())
    {
        QString extension = WControllerNetwork::extractUrlExtension(path);

        if (WControllerPlaylist::extensionIsMedia(extension) == false) continue;

        WMediaDownload * download = downloads.value(reply);

        QString url = torrent->url() + '#' + path.mid(path.lastIndexOf('/') + 1);

        clearReply(reply);

        q->load(download, url);

        return;
    }
}

void WDownloadHookTorrentPrivate::onLoaded(WTorrentReply * reply)
{
    WMediaDownload * download = downloads.value(reply);

    WTorrent * torrent = reply->torrent();

    QStringList paths = torrent->paths();

    if (torrent->hasError() || paths.isEmpty())
    {
        qWarning("WDownloadHookTorrentPrivate::onLoaded: Failed to load %s.",
                 torrent->url().C_STR);

        download->applyFile(QString());
    }
    else download->applyFile(paths.first());
}

void WDownloadHookTorrentPrivate::onBuffer(qint64 pieces)
{
    Q_Q(WDownloadHookTorrent);

    WTorrentReply * reply = static_cast<WTorrentReply *> (q->sender());

    WMediaDownload * download = downloads.value(reply);

    qint64 size = reply->torrent()->size();

    // NOTE: This comes with every piece.
    if (size <= 0 || pieces - download->received() < DOWNLOADHOOKTORRENT_STEP) return;

    download->applyProgress(pieces * DOWNLOADHOOKTORRENT_PROGRESS / size, pieces, size);
}

// NOTE: The cache can be cleared while we're waiting for the torrent.
void WDownloadHookTorrentPrivate::onDestroyed(QObject * object)
{
    WMediaDownload * download = downloads.take(object);

    if (download) download->applyFile(QString());
}

void WDownloadHookTorrentPrivate::onAboutToQuit()
{
    Q_Q(WDownloadHookTorrent);

    downloads.clear();

    // NOTE: That includes the replies we cleared, they're still waiting for their deleteLater.
    foreach (WTorrentReply * reply, q->findChildren<WTorrentReply *>())
    {
        QObject::disconnect(reply, NULL, q, NULL);

        delete reply;
    }
}

//=================================================================================================
// WDownloadHookTorrent
//=================================================================================================

/* explicit */ WDownloadHookTorrent::WDownloadHookTorrent(QObject * parent)
    : WAbstractDownloadHook(new WDownloadHookTorrentPrivate(this), parent)
{
    Q_D(WDownloadHookTorrent); d->init();
}

//-------------------------------------------------------------------------------------------------
// WAbstractDownloadHook implementation
//-------------------------------------------------------------------------------------------------

/* virtual */ bool WDownloadHookTorrent::check(const QString & url)
{
    return WControllerPlaylist::urlIsTorrent(url);
}

/* virtual */ void WDownloadHookTorrent::load(WMediaDownload * download, const QString & url)
{
    Q_D(WDownloadHookTorrent);

    // NOTE: The file stays in the torrent cache as long as its reply is there.
    WTorrentReply * reply = wControllerTorrent->getTorrent(url, this, WTorrent::Default);

    d->downloads.insert(reply, download);

    connect(reply, SIGNAL(buffer(qint64, qint64)), this, SLOT(onBuffer(qint64)));

    if (url.contains('#') == false)
    {
        connect(reply, SIGNAL(added(WTorrentReply *)), this, SLOT(onAdded(WTorrentReply *)));
    }

    connect(reply, SIGNAL(loaded(WTorrentReply *)), this, SLOT(onLoaded(WTorrentReply *)));

    connect(reply, SIGNAL(destroyed(QObject *)), this, SLOT(onDestroyed(QObject *)));
}

/* virtual */ void WDownloadHookTorrent::clear(WMediaDownload * download)
{
    Q_D(WDownloadHookTorrent);

    QObject * reply = d->downloads.key(download);

    if (reply) d->clearReply(reply);
}

#endif // SK_NO_DOWNLOADHOOKTORRENT
