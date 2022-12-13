//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkGui.

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

#ifndef WDECLARATIVEPLAYER_P_H
#define WDECLARATIVEPLAYER_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

// Sk includes
#include <WBroadcastClient>

// Private includes
#ifdef SK_SOFTWARE
#include <private/WDeclarativeItemPaint_p>
#else
#include <private/WDeclarativeItem_p>
#endif

#ifndef SK_NO_DECLARATIVEPLAYER

#ifdef SK_SOFTWARE
class SK_GUI_EXPORT WDeclarativePlayerPrivate : public WDeclarativeItemPaintPrivate
#else
class SK_GUI_EXPORT WDeclarativePlayerPrivate : public WDeclarativeItemPrivate
#endif
{
public:
    WDeclarativePlayerPrivate(WDeclarativePlayer * p);

    /* virtual */ ~WDeclarativePlayerPrivate();

    void init();

public: // Functions
#if defined(QT_NEW) && defined(SK_SOFTWARE) == false
    void updateGeometry(WBackendNode * node);
#endif

    void applyPlaylist(WPlaylist * playlist);

    void loadSource(const QString & url, int duration, int currentTime);

    bool updateBackend(const QString & url, bool isPlaying);

    void stop();

    void updateRepeat();

    void updateShuffle();
    void resetShuffle ();
    void clearShuffle ();

    void clearPlaylistAndTabs();

    void setBackendInterface(WBackendInterface * currentBackend, WAbstractHook * currentHook);

    void setPlaylist(WPlaylist * playlist);

    void setTab(WTabTrack * tab);

    void setShuffleTrack(const WTrack * track);

public: // Slots
    void onEnded();
    void onError();

    void onHookUpdated();

    void onStateChanged   ();
    void onDurationChanged();

    void onCurrentTrackChanged();

    void onCurrentTabChanged    ();
    void onHighlightedTabChanged();

    void onCurrentBookmarkChanged();
    void onCurrentBookmarkUpdated();

    void onConnectedChanged();

    void onMessage(const WBroadcastMessage & message);

    void onSource    ();
    void onState     ();
    void onStateLoad ();
    void onLive      ();
    void onStart     ();
    void onEnd       ();
    void onTime      ();
    void onDuration  ();
    void onProgress  ();
    void onOutput    ();
    void onQuality   ();
    void onVideos    ();
    void onAudios    ();
    void onScreen    ();
    void onFullScreen();
#ifdef SK_DESKTOP
    void onStartup   ();
#endif

    void onHookDestroyed    ();
    void onPlaylistDestroyed();
    void onFolderDestroyed  ();
    void onTabsDestroyed    ();
    void onTabDestroyed     ();

public: // Variables
    WAbstractBackend * backend;
    WAbstractHook    * hook;

    QList<WAbstractHook *> hooks;

    WBackendInterface * backendInterface;

    WBroadcastServer * server;

#if defined(QT_NEW) && defined(SK_SOFTWARE) == false
    WBackendFrame frame;

    bool frameUpdate;
#endif

    WLibraryFolder * folder;

    WPlaylist * playlist;
    WPlaylist * playlistServer;

    WTabsTrack * tabs;
    WTabTrack  * tab;

    QString source;

    int currentTime;

    WAbstractBackend::State state;

    qreal speed;

    qreal volume;

    bool autoPlay;

    bool shuffle;

    QList<const WTrack *> shuffleTracks;
    QList<const WTrack *> shuffleHistory;
    int                   shuffleIndex;
    bool                  shuffleLock;

    WDeclarativePlayer::Repeat repeat;

    WAbstractBackend::Output  output;
    WAbstractBackend::Quality quality;

    WAbstractBackend::FillMode fillMode;

    int trackVideo;
    int trackAudio;

    bool scanOutput;

    int currentOutput;

    QString subtitle;

    int pauseTimeout;

    bool keepState;

    bool videoTag;

    QTimer timer;

protected:
    W_DECLARE_PUBLIC(WDeclarativePlayer)
};

#endif // SK_NO_DECLARATIVEPLAYER
#endif // WDECLARATIVEPLAYER_P_H
