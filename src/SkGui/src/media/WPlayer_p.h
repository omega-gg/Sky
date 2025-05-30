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

#ifndef WPLAYER_P_H
#define WPLAYER_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

// Sk includes
#include <WBroadcastClient>

#ifndef SK_NO_PLAYER

// Forward declarations
#ifndef SK_NO_QML
class WView;
#endif

class SK_GUI_EXPORT WPlayerPrivate : public WPrivate
{
public:
    WPlayerPrivate(WPlayer * p);

    /* virtual */ ~WPlayerPrivate();

    void init();

public: // Functions
    void applyPlaylist(WPlaylist * playlist);

    void loadSource(const QString & url, int duration, int currentTime);

    bool updateBackend(const QString & url, bool isPlaying);

    void stop();

    void updateRepeat(WTrack::Type type);

    void updateShuffle();
    void resetShuffle ();
    void clearShuffle ();

    void clearPlaylistAndTabs();

#ifndef SK_NO_QML
    WView * getView() const;
#endif

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

    void onSource     ();
    void onState      ();
    void onStateLoad  ();
    void onVbml       ();
    void onLive       ();
    void onStart      ();
    void onEnd        ();
    void onCurrentTime();
    void onDuration   ();
    void onProgress   ();
    void onOutput     ();
    void onQuality    ();
    void onContext    ();
    void onChapters   ();
    void onVideos     ();
    void onAudios     ();
    void onAmbient    ();
    void onSubtitles  ();
#ifndef SK_NO_QML
    void onScreen     ();
    void onFullScreen ();
#endif
#ifdef SK_DESKTOP
    void onStartup    ();
#endif

    void onHookDestroyed    ();
    void onPlaylistDestroyed();
    void onFolderDestroyed  ();
    void onTabsDestroyed    ();
    void onTabDestroyed     ();

public: // Variables
#ifndef SK_NO_QML
    WDeclarativePlayer * view;
#endif

    WAbstractBackend * backend;
    WAbstractHook    * hook;

    QList<WAbstractHook *> hooks;

    WBackendInterface * backendInterface;

    WBroadcastServer * server;

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

    WPlayer::Repeat repeat;

    WAbstractBackend::Output     output;
    WAbstractBackend::Quality    quality;
    WAbstractBackend::SourceMode mode;

    WAbstractBackend::FillMode fillMode;

    int trackVideo;
    int trackAudio;

    bool scanOutput;

    int currentOutput;

    WBackendAdjust adjust;

    QString subtitle;

    int pauseTimeout;

    bool keepState;

    bool videoTag;

    QTimer timer;

protected:
    W_DECLARE_PUBLIC(WPlayer)
};

#endif // SK_NO_PLAYER
#endif // WPLAYER_P_H
