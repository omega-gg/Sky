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

#ifndef WDECLARATIVEPLAYER_H
#define WDECLARATIVEPLAYER_H

// Sk includes
#ifdef SK_SOFTWARE
#include <WDeclarativeItemPaint>
#else
#include <WDeclarativeItem>
#endif
#include <WAbstractBackend>
#include <WPlaylist>
#include <WTrack>

#ifndef SK_NO_DECLARATIVEPLAYER

// Forward declarations
class WDeclarativePlayerPrivate;
class WBroadcastServer;
class WPlayer;
class WAbstractHook;
class WTabsTrack;

#ifdef QT_6
Q_MOC_INCLUDE("WPlayer")
Q_MOC_INCLUDE("WAbstractHook")
Q_MOC_INCLUDE("WBroadcastServer")
Q_MOC_INCLUDE("WTabsTrack")
Q_MOC_INCLUDE("WTabTrack")
#endif

#ifdef SK_SOFTWARE
class SK_GUI_EXPORT WDeclarativePlayer : public WDeclarativeItemPaint, public WPlaylistWatcher
#else
class SK_GUI_EXPORT WDeclarativePlayer : public WDeclarativeItem, public WPlaylistWatcher
#endif
{
    Q_OBJECT

    Q_ENUMS(Repeat)

    Q_PROPERTY(WPlayer * player READ player CONSTANT)

    Q_PROPERTY(WAbstractBackend * backend READ backend WRITE setBackend NOTIFY backendChanged)

    Q_PROPERTY(QList<WAbstractHook *> hooks READ hooks WRITE setHooks NOTIFY hooksChanged)

    Q_PROPERTY(WBroadcastServer * server READ server WRITE setServer NOTIFY serverChanged)

    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)

    Q_PROPERTY(WPlaylist * playlist READ playlist WRITE setPlaylist NOTIFY playlistChanged)

    Q_PROPERTY(WAbstractBackend::State     currentState READ state     NOTIFY stateChanged)
    Q_PROPERTY(WAbstractBackend::StateLoad stateLoad    READ stateLoad NOTIFY stateLoadChanged)

    Q_PROPERTY(bool isLoading READ isLoading NOTIFY stateLoadChanged)

    Q_PROPERTY(bool isDefault   READ isDefault   NOTIFY stateLoadChanged)
    Q_PROPERTY(bool isStarting  READ isStarting  NOTIFY stateLoadChanged)
    Q_PROPERTY(bool isResuming  READ isResuming  NOTIFY stateLoadChanged)
    Q_PROPERTY(bool isBuffering READ isBuffering NOTIFY stateLoadChanged)

    Q_PROPERTY(bool isPlaying READ isPlaying NOTIFY stateChanged)
    Q_PROPERTY(bool isPaused  READ isPaused  NOTIFY stateChanged)
    Q_PROPERTY(bool isStopped READ isStopped NOTIFY stateChanged)

    Q_PROPERTY(bool isVideo READ isVideo NOTIFY sourceChanged)
    Q_PROPERTY(bool isAudio READ isAudio NOTIFY sourceChanged)

    Q_PROPERTY(bool isVbml READ isVbml NOTIFY vbmlChanged)
    Q_PROPERTY(bool isLive READ isLive NOTIFY liveChanged)

    Q_PROPERTY(bool hasStarted READ hasStarted NOTIFY startedChanged)
    Q_PROPERTY(bool hasEnded   READ hasEnded   NOTIFY endedChanged)

    Q_PROPERTY(bool hasOutput READ hasOutput NOTIFY currentOutputChanged)

    Q_PROPERTY(int currentTime READ currentTime NOTIFY currentTimeChanged)
    Q_PROPERTY(int duration    READ duration    NOTIFY durationChanged)

    Q_PROPERTY(qreal progress READ progress NOTIFY progressChanged)

    Q_PROPERTY(qreal speed READ speed WRITE setSpeed NOTIFY speedChanged)

    Q_PROPERTY(qreal volume READ volume WRITE setVolume NOTIFY volumeChanged)

    Q_PROPERTY(bool autoPlay READ autoPlay WRITE setAutoPlay NOTIFY autoPlayChanged)

    Q_PROPERTY(bool shuffle READ shuffle WRITE setShuffle NOTIFY shuffleChanged)

    Q_PROPERTY(Repeat repeat READ repeat WRITE setRepeat NOTIFY repeatChanged)

    Q_PROPERTY(WAbstractBackend::Output output READ output WRITE setOutput NOTIFY outputChanged)

    Q_PROPERTY(WAbstractBackend::Quality quality READ quality WRITE setQuality
               NOTIFY qualityChanged)

    Q_PROPERTY(WAbstractBackend::SourceMode sourceMode READ sourceMode WRITE setSourceMode
               NOTIFY sourceModeChanged)

    Q_PROPERTY(WAbstractBackend::Output outputActive READ outputActive NOTIFY outputActiveChanged)

    Q_PROPERTY(WAbstractBackend::Quality qualityActive READ qualityActive
               NOTIFY qualityActiveChanged)

    Q_PROPERTY(WAbstractBackend::FillMode fillMode READ fillMode WRITE setFillMode
               NOTIFY fillModeChanged)

    Q_PROPERTY(int trackVideo READ trackVideo WRITE setTrackVideo NOTIFY trackVideoChanged)
    Q_PROPERTY(int trackAudio READ trackAudio WRITE setTrackAudio NOTIFY trackAudioChanged)

    Q_PROPERTY(int countVideos READ countVideos NOTIFY videosChanged)
    Q_PROPERTY(int countAudios READ countAudios NOTIFY audiosChanged)

    Q_PROPERTY(bool scanOutput READ scanOutput WRITE setScanOutput NOTIFY scanOutputChanged)

    Q_PROPERTY(int currentOutput READ currentOutput WRITE setCurrentOutput
               NOTIFY currentOutputChanged)

    Q_PROPERTY(QString                      outputName READ outputName NOTIFY currentOutputChanged)
    Q_PROPERTY(WAbstractBackend::OutputType outputType READ outputType NOTIFY currentOutputChanged)

    Q_PROPERTY(int countOutputs READ countOutputs NOTIFY outputsChanged)

    Q_PROPERTY(QString subtitle READ subtitle WRITE setSubtitle NOTIFY subtitleChanged)

    Q_PROPERTY(QString context   READ context   NOTIFY contextChanged)
    Q_PROPERTY(QString contextId READ contextId NOTIFY contextChanged)

    Q_PROPERTY(QList<WChapter> chapters READ chapters NOTIFY chaptersChanged)

    Q_PROPERTY(QString ambient READ ambient NOTIFY ambientChanged)

    Q_PROPERTY(QStringList subtitles READ subtitles NOTIFY subtitlesChanged)

    Q_PROPERTY(int pauseTimeout READ pauseTimeout WRITE setPauseTimeout NOTIFY pauseTimeoutChanged)

    Q_PROPERTY(int count READ count NOTIFY countChanged)

    Q_PROPERTY(bool hasPreviousTrack READ hasPreviousTrack NOTIFY playlistUpdated)
    Q_PROPERTY(bool hasNextTrack     READ hasNextTrack     NOTIFY playlistUpdated)

    Q_PROPERTY(int trackType READ trackType NOTIFY currentTrackUpdated)

    Q_PROPERTY(bool trackIsLive        READ trackIsLive        NOTIFY currentTrackUpdated)
    Q_PROPERTY(bool trackIsHub         READ trackIsHub         NOTIFY currentTrackUpdated)
    Q_PROPERTY(bool trackIsChannel     READ trackIsChannel     NOTIFY currentTrackUpdated)
    Q_PROPERTY(bool trackIsInteractive READ trackIsInteractive NOTIFY currentTrackUpdated)
    Q_PROPERTY(bool trackIsLite        READ trackIsLite        NOTIFY currentTrackUpdated)

    Q_PROPERTY(int trackState READ trackState NOTIFY currentTrackUpdated)

    Q_PROPERTY(bool trackIsDefault READ trackIsDefault NOTIFY currentTrackUpdated)
    Q_PROPERTY(bool trackIsLoading READ trackIsLoading NOTIFY currentTrackUpdated)
    Q_PROPERTY(bool trackIsLoaded  READ trackIsLoaded  NOTIFY currentTrackUpdated)

    Q_PROPERTY(QString trackTitle READ trackTitle NOTIFY currentTrackUpdated)
    Q_PROPERTY(QString trackCover READ trackCover NOTIFY currentTrackUpdated)

    Q_PROPERTY(int trackCurrentTime READ trackCurrentTime NOTIFY currentTrackUpdated)
    Q_PROPERTY(int trackDuration    READ trackDuration    NOTIFY currentTrackUpdated)

    Q_PROPERTY(QDateTime trackDate READ trackDate NOTIFY currentTrackUpdated)

    Q_PROPERTY(int trackIndex READ trackIndex NOTIFY currentTrackUpdated)

    Q_PROPERTY(WTabsTrack * tabs READ tabs WRITE setTabs NOTIFY tabsChanged)

    Q_PROPERTY(WTabTrack * tab READ tab NOTIFY tabChanged)

    Q_PROPERTY(int tabIndex READ tabIndex NOTIFY tabIndexChanged)

    Q_PROPERTY(bool videoTag READ videoTag WRITE setVideoTag NOTIFY videoTagChanged)

public: // Enums
    enum Repeat
    {
        RepeatNone,
        RepeatAll,
        RepeatOne
    };

public:
#ifdef QT_4
    explicit WDeclarativePlayer(QDeclarativeItem * parent = NULL);
#else
    explicit WDeclarativePlayer(QQuickItem * parent = NULL);
#endif

protected:
#ifdef QT_4
    WDeclarativePlayer(WDeclarativePlayerPrivate * p, QDeclarativeItem * parent = NULL);
#else
    WDeclarativePlayer(WDeclarativePlayerPrivate * p, QQuickItem * parent = NULL);
#endif

public: // Interface
    Q_INVOKABLE void play  ();
    Q_INVOKABLE void replay();

    Q_INVOKABLE void pause();
    Q_INVOKABLE void stop ();
    Q_INVOKABLE void clear();

    Q_INVOKABLE void togglePlay();

    Q_INVOKABLE void seek(int msec);

    Q_INVOKABLE void setPreviousTrack();
    Q_INVOKABLE void setNextTrack    ();

    Q_INVOKABLE void reloadSource();

    Q_INVOKABLE void updateFrame();

    Q_INVOKABLE QImage getFrame() const;

    Q_INVOKABLE QRectF getRect() const;

    // NOTE: This functions returns the video frame geometry mapped to the player dimensions.
    Q_INVOKABLE QRectF getGeometry() const;

    Q_INVOKABLE void updateHighlightedTab();

    Q_INVOKABLE QVariantList chaptersData(bool sort = true) const;

    //---------------------------------------------------------------------------------------------
    // Tracks

    Q_INVOKABLE int idVideo(int index) const;
    Q_INVOKABLE int idAudio(int index) const;

    Q_INVOKABLE int indexVideo(int id) const;
    Q_INVOKABLE int indexAudio(int id) const;

    Q_INVOKABLE QString videoName(int id) const;
    Q_INVOKABLE QString audioName(int id) const;

public: // Virtual interface
    // NOTE: You can override the source and the currentTime. When passing -2 we skip the
    //       currentTime entirely.
    Q_INVOKABLE virtual QString toVbml(const QString & source      = QString(),
                                       int             currentTime = -1) const;

#if defined(QT_4) || defined(SK_SOFTWARE)
public: // QGraphicsItem / QQuickPaintedItem reimplementation
#ifdef QT_4
    /* virtual */ void paint(QPainter * painter, const QStyleOptionGraphicsItem * option,
                                                 QWidget                        * widget);
#else
    /* virtual */ void paint(QPainter * painter);
#endif
#endif

#if defined(QT_NEW) && defined(SK_SOFTWARE) == false
public: // QQuickItem reimplementation
    /* virtual */ QSGNode * updatePaintNode(QSGNode * oldNode, UpdatePaintNodeData * data);
#endif

protected: // QDeclarativeItem / QQuickItem reimplementation
#ifdef QT_OLD
    /* virtual */ void geometryChanged(const QRectF & newGeometry, const QRectF & oldGeometry);
#else
    /* virtual */ void geometryChange(const QRectF & newGeometry, const QRectF & oldGeometry);
#endif

signals:
    void loaded();
    void ended ();

    void clearCache();

    void backendChanged();
    void hooksChanged  ();

    void serverChanged();

    void sourceChanged();

    void playlistChanged();
    void playlistUpdated();

    void stateChanged    ();
    void stateLoadChanged();

    void vbmlChanged();
    void liveChanged();

    void startedChanged();
    void endedChanged  ();

    void currentTimeChanged();
    void durationChanged   ();

    void progressChanged();

    void speedChanged();

    void volumeChanged();

    void autoPlayChanged();

    void shuffleChanged();

    void repeatChanged();

    void outputChanged    ();
    void qualityChanged   ();
    void sourceModeChanged();

    void outputActiveChanged ();
    void qualityActiveChanged();

    void fillModeChanged();

    void videosChanged();
    void audiosChanged();

    void trackVideoChanged();
    void trackAudioChanged();

    void scanOutputChanged();

    void currentOutputChanged();

    void outputsChanged();

    void subtitleChanged();

    void contextChanged();

    void chaptersChanged();

    void ambientChanged();

    void subtitlesChanged();

    void pauseTimeoutChanged();

    void countChanged();

    void currentTrackUpdated();

    void tabsChanged();

    void tabChanged     ();
    void tabIndexChanged();

    void videoTagChanged();

public: // Properties
    WPlayer * player() const;

    WAbstractBackend * backend() const;
    void               setBackend(WAbstractBackend * backend);

    QList<WAbstractHook *> hooks() const;
    void                   setHooks(const QList<WAbstractHook *> & hooks);

    WBroadcastServer * server() const;
    void               setServer(WBroadcastServer * server);

    QString source() const;
    void    setSource(const QString & url);

    WPlaylist * playlist() const;
    void        setPlaylist(WPlaylist * playlist);

    WAbstractBackend::State     state    () const;
    WAbstractBackend::StateLoad stateLoad() const;

    bool isLoading() const;

    bool isDefault  () const;
    bool isStarting () const;
    bool isResuming () const;
    bool isBuffering() const;

    bool isPlaying() const;
    bool isPaused () const;
    bool isStopped() const;

    bool isVideo() const;
    bool isAudio() const;

    bool isVbml() const;
    bool isLive() const;

    bool hasStarted() const;
    bool hasEnded  () const;

    bool hasOutput() const;

    int currentTime() const;
    int duration   () const;

    qreal progress() const;

    qreal speed() const;
    void  setSpeed(qreal speed);

    qreal volume() const;
    void  setVolume(qreal volume);

    bool autoPlay() const;
    void setAutoPlay(bool autoPlay);

    bool shuffle() const;
    void setShuffle(bool shuffle);

    Repeat repeat() const;
    void   setRepeat(Repeat repeat);

    WAbstractBackend::Output output() const;
    void                     setOutput(WAbstractBackend::Output output);

    WAbstractBackend::Quality quality() const;
    void                      setQuality(WAbstractBackend::Quality quality);

    WAbstractBackend::SourceMode sourceMode() const;
    void                         setSourceMode(WAbstractBackend::SourceMode mode);

    WAbstractBackend::Output  outputActive () const;
    WAbstractBackend::Quality qualityActive() const;

    WAbstractBackend::FillMode fillMode() const;
    void                       setFillMode(WAbstractBackend::FillMode fillMode);

    int  trackVideo() const;
    void setTrackVideo(int id);

    int  trackAudio() const;
    void setTrackAudio(int id);

    int countVideos() const;
    int countAudios() const;

    bool scanOutput() const;
    void setScanOutput(bool enabled);

    int  currentOutput() const;
    void setCurrentOutput(int index);

    QString                      outputName() const;
    WAbstractBackend::OutputType outputType() const;

    int countOutputs() const;

    QString subtitle() const;
    void    setSubtitle(const QString & subtitle);

    QString context  () const;
    QString contextId() const;

    QList<WChapter> chapters() const;

    QString ambient() const;

    QStringList subtitles() const;

    int  pauseTimeout() const;
    void setPauseTimeout(int msec);

    int count() const;

    bool hasPreviousTrack() const;
    bool hasNextTrack    () const;

    WTrack::Type trackType() const;

    bool trackIsLive       () const;
    bool trackIsHub        () const;
    bool trackIsChannel    () const;
    bool trackIsInteractive() const;
    bool trackIsLite       () const;

    WTrack::State trackState() const;

    bool trackIsDefault() const;
    bool trackIsLoading() const;
    bool trackIsLoaded () const;

    QString trackTitle() const;
    QString trackCover() const;

    int trackCurrentTime() const;
    int trackDuration   () const;

    QDateTime trackDate() const;

    int trackIndex() const;

    WTabsTrack * tabs() const;
    void         setTabs(WTabsTrack * tabs);

    WTabTrack * tab() const;

    int tabIndex() const;

    bool videoTag() const;
    void setVideoTag(bool enabled);

private:
    W_DECLARE_PRIVATE(WDeclarativePlayer)
};

#include <private/WDeclarativePlayer_p>

#endif // SK_NO_DECLARATIVEPLAYER
#endif // WDECLARATIVEPLAYER_H
