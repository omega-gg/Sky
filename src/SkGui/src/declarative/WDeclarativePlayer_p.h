//=================================================================================================
/*
    Copyright (C) 2015-2016 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkGui module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
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
#include <private/WDeclarativeItem_p>

#ifndef SK_NO_DECLARATIVEPLAYER

class SK_GUI_EXPORT WDeclarativePlayerPrivate : public WDeclarativeItemPrivate
{
protected:
    WDeclarativePlayerPrivate(WDeclarativePlayer * p);

    /* virtual */ ~WDeclarativePlayerPrivate();

    void init();

public: // Functions
    void setPlaylist(WPlaylistNet * playlist);

    void setTab(WTabTrack * tab);

    void loadSource(const QUrl & source, int duration, int currentTime);

    void updateRepeat();

    void resetShuffle();
    void clearShuffle();

    void setShuffleTrack(const WAbstractTrack * track);

    void clearPlaylistAndTabs();

public: // Slots
    void onEnded();

    void onStateChanged   ();
    void onDurationChanged();

    void onCurrentTrackChanged();

    void onPlaylistDestroyed();
    void onFolderDestroyed  ();

    void onCurrentTabChanged    ();
    void onHighlightedTabChanged();

    void onCurrentBookmarkChanged();
    void onCurrentBookmarkUpdated();

    void onTabsDestroyed();
    void onTabDestroyed ();

public: // Variables
    WAbstractBackend * backend;
    WAbstractHook    * hook;

    WAbstractBackend::State state;

    WLibraryFolder * folder;
    WPlaylistNet   * playlist;

    qreal speed;

    int volume;

    bool shuffle;

    QList<const WAbstractTrack *> shuffleTracks;
    QList<const WAbstractTrack *> shuffleHistory;
    int                           shuffleIndex;
    bool                          shuffleLock;

    WDeclarativePlayer::Repeat repeat;

    WAbstractBackend::Quality quality;

    WAbstractBackend::FillMode fillMode;

    WTabsTrack * tabs;
    WTabTrack  * tab;

    bool keepState;

protected:
    W_DECLARE_PUBLIC(WDeclarativePlayer)
};

#endif // SK_NO_DECLARATIVEPLAYER
#endif // WDECLARATIVEPLAYER_P_H
