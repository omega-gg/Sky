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

#ifndef WDOWNLOADHOOKTORRENT_H
#define WDOWNLOADHOOKTORRENT_H

// Sk includes
#include <WMediaDownload>

#ifndef SK_NO_DOWNLOADHOOKTORRENT

// Forward declarations
class WDownloadHookTorrentPrivate;
class WTorrentReply;

#ifdef QT_6
Q_MOC_INCLUDE("WControllerTorrent")
#endif

class SK_TORRENT_EXPORT WDownloadHookTorrent : public WAbstractDownloadHook
{
    Q_OBJECT

public:
    explicit WDownloadHookTorrent(QObject * parent = NULL);

public: // WAbstractDownloadHook implementation
    /* virtual */ bool check(const QString & url);

    /* virtual */ void load(WMediaDownload * download, const QString & url);

    /* virtual */ void clear(WMediaDownload * download);

private:
    W_DECLARE_PRIVATE(WDownloadHookTorrent)

    Q_PRIVATE_SLOT(d_func(), void onAdded (WTorrentReply *))
    Q_PRIVATE_SLOT(d_func(), void onLoaded(WTorrentReply *))

    Q_PRIVATE_SLOT(d_func(), void onBuffer(qint64))

    Q_PRIVATE_SLOT(d_func(), void onDestroyed(QObject *))

    Q_PRIVATE_SLOT(d_func(), void onAboutToQuit())
};

#include <private/WDownloadHookTorrent_p>

#endif // SK_NO_DOWNLOADHOOKTORRENT
#endif // WDOWNLOADHOOKTORRENT_H
