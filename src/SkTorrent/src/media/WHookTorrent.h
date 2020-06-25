//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

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

#ifndef WHOOKTORRENT_H
#define WHOOKTORRENT_H

// Sk includes
#include <WAbstractHook>

#ifndef SK_NO_HOOKTORRENT

class WHookTorrentPrivate;

class SK_TORRENT_EXPORT WHookTorrent : public WAbstractHook
{
    Q_OBJECT

public:
    WHookTorrent(WAbstractBackend * backend);

public: // WAbstractHook reimplementation
    /* Q_INVOKABLE virtual */ void loadSource(const QString & url, int duration    = -1,
                                                                   int currentTime = -1);

    /* Q_INVOKABLE virtual */ void play  ();
    /* Q_INVOKABLE virtual */ void replay();

    /* Q_INVOKABLE virtual */ void pause();
    /* Q_INVOKABLE virtual */ void stop ();
    /* Q_INVOKABLE virtual */ void clear();

    /* Q_INVOKABLE virtual */ void seek(int msec);

public: // WBackendFilter reimplementation
    /* virtual */ void filterState    (WAbstractBackend::State     * state);
    /* virtual */ void filterStateLoad(WAbstractBackend::StateLoad * stateLoad);

protected: // WAbstractHook implementation
    /* virtual */ bool hookCheckSource(const QString & url);

private:
    W_DECLARE_PRIVATE(WHookTorrent)

    Q_PRIVATE_SLOT(d_func(), void onAdded ())
    Q_PRIVATE_SLOT(d_func(), void onLoaded())

    Q_PRIVATE_SLOT(d_func(), void onBuffer(qint64, qint64))
    Q_PRIVATE_SLOT(d_func(), void onSeek  (qint64, qint64))

    Q_PRIVATE_SLOT(d_func(), void onUpdateState())

    Q_PRIVATE_SLOT(d_func(), void onDestroyed())
};

#include <private/WHookTorrent_p>

#endif // SK_NO_HOOKTORRENT
#endif // WHOOKTORRENT_H
