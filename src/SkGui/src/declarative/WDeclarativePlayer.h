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

#ifndef WDECLARATIVEPLAYER_H
#define WDECLARATIVEPLAYER_H

// Sk includes
#include <WDeclarativeItem>
#include <WAbstractPlaylist>
#include <WAbstractTrack>

#ifndef SK_NO_DECLARATIVEPLAYER

// Forward declarations
class WDeclarativePlayerPrivate;
class QGraphicsWebView;
class WAbstractHook;
class WPlaylistNet;
class WTabsTrack;
class WTabTrack;

class SK_GUI_EXPORT WDeclarativePlayer : public WDeclarativeItem, public WAbstractPlaylistWatcher
{
    Q_OBJECT

    Q_ENUMS(Repeat)

    Q_PROPERTY(WAbstractBackend * backend READ backend WRITE setBackend NOTIFY backendChanged)
    Q_PROPERTY(WAbstractHook    * hook    READ hook    WRITE setHook    NOTIFY hookChanged)

    Q_PROPERTY(WAbstractBackend::State currentState READ state NOTIFY stateChanged)

    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)

    Q_PROPERTY(WPlaylistNet * playlist READ playlist WRITE setPlaylist NOTIFY playlistChanged)

    Q_PROPERTY(bool hasStarted READ hasStarted NOTIFY startedChanged)

    Q_PROPERTY(bool isLoading READ isLoading NOTIFY stateLoadChanged)

    Q_PROPERTY(bool isStarting  READ isStarting  NOTIFY stateLoadChanged)
    Q_PROPERTY(bool isResuming  READ isResuming  NOTIFY stateLoadChanged)
    Q_PROPERTY(bool isBuffering READ isBuffering NOTIFY stateLoadChanged)

    Q_PROPERTY(bool isPlaying READ isPlaying NOTIFY stateChanged)
    Q_PROPERTY(bool isPaused  READ isPaused  NOTIFY stateChanged)
    Q_PROPERTY(bool isStopped READ isStopped NOTIFY stateChanged)

    Q_PROPERTY(int currentTime READ currentTime NOTIFY currentTimeChanged)
    Q_PROPERTY(int duration    READ duration    NOTIFY durationChanged)

    Q_PROPERTY(qreal speed READ speed WRITE setSpeed NOTIFY speedChanged)

    Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY volumeChanged)

    Q_PROPERTY(bool shuffle READ shuffle WRITE setShuffle NOTIFY shuffleChanged)

    Q_PROPERTY(Repeat repeat READ repeat WRITE setRepeat NOTIFY repeatChanged)

    Q_PROPERTY(WAbstractBackend::Quality quality READ quality WRITE setQuality
               NOTIFY qualityChanged)

    Q_PROPERTY(WAbstractBackend::Quality qualityActive READ qualityActive
               NOTIFY qualityActiveChanged)

    Q_PROPERTY(WAbstractBackend::FillMode fillMode READ fillMode WRITE setFillMode
               NOTIFY fillModeChanged)

    Q_PROPERTY(int count READ count NOTIFY countChanged)

    Q_PROPERTY(bool hasPreviousTrack READ hasPreviousTrack NOTIFY playlistUpdated)
    Q_PROPERTY(bool hasNextTrack     READ hasNextTrack     NOTIFY playlistUpdated)

    Q_PROPERTY(int trackState READ trackState NOTIFY currentTrackUpdated)

    Q_PROPERTY(bool trackIsDefault READ trackIsDefault NOTIFY currentTrackUpdated)
    Q_PROPERTY(bool trackIsLoading READ trackIsLoading NOTIFY currentTrackUpdated)
    Q_PROPERTY(bool trackIsLoaded  READ trackIsLoaded  NOTIFY currentTrackUpdated)

    Q_PROPERTY(QString trackTitle READ trackTitle NOTIFY currentTrackUpdated)
    Q_PROPERTY(QUrl    trackCover READ trackCover NOTIFY currentTrackUpdated)

    Q_PROPERTY(int trackCurrentTime READ trackCurrentTime NOTIFY currentTrackUpdated)
    Q_PROPERTY(int trackDuration    READ trackDuration    NOTIFY currentTrackUpdated)

    Q_PROPERTY(int trackIndex READ trackIndex NOTIFY currentTrackUpdated)

    Q_PROPERTY(WTabsTrack * tabs READ tabs WRITE setTabs NOTIFY tabsChanged)

    Q_PROPERTY(WTabTrack * tab READ tab NOTIFY tabChanged)

    Q_PROPERTY(int tabIndex READ tabIndex NOTIFY tabsUpdated)

    Q_PROPERTY(bool keepState READ keepState WRITE setKeepState NOTIFY keepStateChanged)

public: // Enums
    enum Repeat
    {
        RepeatNone,
        RepeatAll,
        RepeatOne
    };

public:
    explicit WDeclarativePlayer(QDeclarativeItem * parent = NULL);

public: // Interface
    Q_INVOKABLE void play  ();
    Q_INVOKABLE void replay();

    Q_INVOKABLE void pause();

    Q_INVOKABLE void stop();

    Q_INVOKABLE void togglePlay();

    Q_INVOKABLE void seekTo(int msec);

    Q_INVOKABLE void setPreviousTrack();
    Q_INVOKABLE void setNextTrack    ();

    Q_INVOKABLE QImage getFrame() const;

public: // QGraphicsItem reimplementation
    /* virtual */ void paint(QPainter * painter, const QStyleOptionGraphicsItem * option,
                                                 QWidget                        * widget);

signals:
    void backendChanged();
    void hookChanged   ();

    void stateChanged    ();
    void stateLoadChanged();

    void sourceChanged();

    void playlistChanged();
    void playlistUpdated();

    void countChanged();

    void startedChanged();

    void currentTimeChanged();
    void durationChanged   ();

    void speedChanged();

    void volumeChanged();

    void shuffleChanged();

    void repeatChanged();

    void qualityChanged      ();
    void qualityActiveChanged();

    void fillModeChanged();

    void currentTrackUpdated();

    void tabsChanged();

    void tabChanged();

    void tabsUpdated();

    void keepStateChanged();

protected: // QDeclarativeItem reimplementation
    /* virtual */ void geometryChanged(const QRectF & newGeometry, const QRectF & oldGeometry);

protected: // WAbstractPlaylistWatcher implementation
    /* virtual */ void beginTracksInsert(int first, int last);
    /* virtual */ void endTracksInsert  ();

    /* virtual */ void beginTracksRemove(int first, int last);

    /* virtual */ void beginTracksClear();

public: // Properties
    WAbstractBackend * backend() const;
    void               setBackend(WAbstractBackend * backend);

    WAbstractHook * hook() const;
    void            setHook(WAbstractHook * hook);

    WAbstractBackend::State state() const;

    QUrl source() const;
    void setSource(const QUrl & url);

    WPlaylistNet * playlist() const;
    void           setPlaylist(WPlaylistNet * playlist);

    bool hasStarted() const;

    bool isLoading() const;

    bool isStarting () const;
    bool isResuming () const;
    bool isBuffering() const;

    bool isPlaying() const;
    bool isPaused () const;
    bool isStopped() const;

    int currentTime() const;
    int duration   () const;

    qreal speed() const;
    void  setSpeed(qreal speed);

    int  volume() const;
    void setVolume(int percent);

    bool shuffle() const;
    void setShuffle(bool shuffle);

    Repeat repeat() const;
    void   setRepeat(Repeat repeat);

    WAbstractBackend::Quality quality() const;
    void                      setQuality(WAbstractBackend::Quality quality);

    WAbstractBackend::Quality qualityActive() const;

    WAbstractBackend::FillMode fillMode() const;
    void                       setFillMode(WAbstractBackend::FillMode fillMode);

    int count() const;

    bool hasPreviousTrack() const;
    bool hasNextTrack    () const;

    WAbstractTrack::State trackState() const;

    bool trackIsDefault() const;
    bool trackIsLoading() const;
    bool trackIsLoaded () const;

    QString trackTitle() const;
    QUrl    trackCover() const;

    int trackCurrentTime() const;
    int trackDuration   () const;

    int trackIndex() const;

    WTabsTrack * tabs() const;
    void         setTabs(WTabsTrack * tabs);

    WTabTrack * tab() const;

    int tabIndex() const;

    bool keepState() const;
    void setKeepState(bool keepState);

private:
    W_DECLARE_PRIVATE(WDeclarativePlayer)

    Q_PRIVATE_SLOT(d_func(), void onEnded())

    Q_PRIVATE_SLOT(d_func(), void onStateChanged   ())
    Q_PRIVATE_SLOT(d_func(), void onDurationChanged())

    Q_PRIVATE_SLOT(d_func(), void onCurrentTrackChanged())

    Q_PRIVATE_SLOT(d_func(), void onPlaylistDestroyed())
    Q_PRIVATE_SLOT(d_func(), void onFolderDestroyed  ())

    Q_PRIVATE_SLOT(d_func(), void onCurrentTabChanged    ())
    Q_PRIVATE_SLOT(d_func(), void onHighlightedTabChanged())

    Q_PRIVATE_SLOT(d_func(), void onCurrentBookmarkChanged())
    Q_PRIVATE_SLOT(d_func(), void onCurrentBookmarkUpdated())

    Q_PRIVATE_SLOT(d_func(), void onTabsDestroyed())
    Q_PRIVATE_SLOT(d_func(), void onTabDestroyed ())
};

#include <private/WDeclarativePlayer_p>

#endif // SK_NO_DECLARATIVEPLAYER
#endif // WDECLARATIVEPLAYER_H
