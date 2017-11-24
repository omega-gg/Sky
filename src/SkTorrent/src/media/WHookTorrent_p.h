//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkTorrent module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#ifndef WHOOKTORRENT_P_H
#define WHOOKTORRENT_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

// Qt includes
#include <QMetaMethod>

// Private includes
#include <private/WAbstractHook_p>

#ifndef SK_NO_HOOKTORRENT

// Forward declarations
class WTorrentThread;
class WTorrent;
class WTorrentReply;

class SK_TORRENT_EXPORT WHookTorrentPrivate : public WAbstractHookPrivate
{
public: // Enums
    enum State
    {
        StateDefault,
        StateLoading,
        StateStarting,
        StatePlaying,
        StatePaused
    };

public:
    WHookTorrentPrivate(WHookTorrent * p);

    /* virtual */ ~WHookTorrentPrivate();

    void init();

public: // Functions
    void load ();
    void start();

    void play();
    void stop();

    void applyBuffer(qint64 bufferBlocks);

    void clearReply();
    void clearData ();

public: // Slots
    void onAdded ();
    void onLoaded();

    void onBuffer(qint64 bufferPieces, qint64 bufferBlocks);
    void onSeek  (qint64 bufferPieces, qint64 bufferBlocks);

    void onDestroyed();

public: // Variables
    WTorrentThread * thread;

    WTorrent      * torrent;
    WTorrentReply * reply;

    QString fileName;

    State state;

    int port;

    QUrl url;

    int currentTime;

    QMetaMethod methodFile;
    QMetaMethod methodBuffer;
    QMetaMethod methodSeek;
    QMetaMethod methodStart;
    QMetaMethod methodSkip;
    QMetaMethod methodClear;

protected:
    W_DECLARE_PUBLIC(WHookTorrent)
};

#endif // SK_NO_HOOKTORRENT
#endif // WHOOKTORRENT_P_H
