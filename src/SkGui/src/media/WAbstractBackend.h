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

#ifndef WABSTRACTBACKEND_H
#define WABSTRACTBACKEND_H

// Qt includes
#include <QObject>
#include <QUrl>
#include <QImage>

// Sk includes
#include <Sk>

#ifndef SK_NO_ABSTRACTBACKEND

// Forward declarations
class WAbstractBackendPrivate;
class QGraphicsItem;
class QPainter;
class QStyleOptionGraphicsItem;
class WBackendFilter;

//-------------------------------------------------------------------------------------------------
// WBackendInterface
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WBackendInterface
{
public:
    virtual QUrl source() const = 0;

    virtual bool sourceIsVideo() const = 0;
    virtual bool sourceIsAudio() const = 0;

    virtual void loadSource(const QUrl & url, int duration = -1, int currentTime = -1) = 0;

    virtual void play  () = 0;
    virtual void replay() = 0;

    virtual void pause() = 0;
    virtual void stop () = 0;
    virtual void clear() = 0;

    virtual void seek(int msec) = 0;
};

//-------------------------------------------------------------------------------------------------
// WAbstractBackend
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WAbstractBackend : public QObject, public WBackendInterface, public WPrivatable
{
    Q_OBJECT

    Q_ENUMS(State)
    Q_ENUMS(StateLoad)
    Q_ENUMS(Output)
    Q_ENUMS(Quality)
    Q_ENUMS(FillMode)

    Q_PROPERTY(QGraphicsItem * parentItem READ parentItem WRITE setParentItem
               NOTIFY parentItemChanged)

    Q_PROPERTY(WBackendFilter * filter READ filter WRITE setFilter NOTIFY filterChanged)

    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)

    Q_PROPERTY(bool sourceIsVideo READ sourceIsVideo NOTIFY sourceChanged)
    Q_PROPERTY(bool sourceIsAudio READ sourceIsAudio NOTIFY sourceChanged)

    Q_PROPERTY(State     state     READ state     NOTIFY stateChanged)
    Q_PROPERTY(StateLoad stateLoad READ stateLoad NOTIFY stateLoadChanged)

    Q_PROPERTY(bool isLoading READ isLoading NOTIFY stateLoadChanged)

    Q_PROPERTY(bool isStarting  READ isStarting  NOTIFY stateLoadChanged)
    Q_PROPERTY(bool isResuming  READ isResuming  NOTIFY stateLoadChanged)
    Q_PROPERTY(bool isBuffering READ isBuffering NOTIFY stateLoadChanged)

    Q_PROPERTY(bool isPlaying READ isPlaying NOTIFY stateChanged)
    Q_PROPERTY(bool isPaused  READ isPaused  NOTIFY stateChanged)
    Q_PROPERTY(bool isStopped READ isStopped NOTIFY stateChanged)

    Q_PROPERTY(bool hasStarted READ hasStarted NOTIFY startedChanged)
    Q_PROPERTY(bool hasEnded   READ hasEnded   NOTIFY endedChanged)

    Q_PROPERTY(int currentTime READ currentTime NOTIFY currentTimeChanged)
    Q_PROPERTY(int duration    READ duration    NOTIFY durationChanged)

    Q_PROPERTY(qreal progress READ progress NOTIFY progressChanged)

    Q_PROPERTY(qreal speed READ speed WRITE setSpeed NOTIFY speedChanged)

    Q_PROPERTY(qreal volume READ volume WRITE setVolume NOTIFY volumeChanged)

    Q_PROPERTY(bool repeat READ repeat WRITE setRepeat NOTIFY repeatChanged)

    Q_PROPERTY(Output  output  READ output  WRITE setOutput  NOTIFY outputChanged)
    Q_PROPERTY(Quality quality READ quality WRITE setQuality NOTIFY qualityChanged)

    Q_PROPERTY(Output  outputActive  READ outputActive  NOTIFY outputActiveChanged)
    Q_PROPERTY(Quality qualityActive READ qualityActive NOTIFY qualityActiveChanged)

    Q_PROPERTY(FillMode fillMode READ fillMode WRITE setFillMode NOTIFY fillModeChanged)

public:
    enum State
    {
        StateStopped,
        StatePlaying,
        StatePaused
    };

    enum StateLoad
    {
        StateLoadDefault,
        StateLoadStarting,
        StateLoadResuming,
        StateLoadBuffering
    };

    enum Output
    {
        OutputInvalid,
        OutputMedia,
        OutputVideo,
        OutputAudio,
    };

    enum Quality
    {
        QualityInvalid,
        QualityMinimum,
        QualityLow,
        QualityMedium,
        QualityHigh,
        QualityUltra,
        QualityMaximum
    };

    enum FillMode
    {
        Stretch,
        PreserveAspectFit,
        PreserveAspectCrop
    };

public:
    WAbstractBackend();
protected:
    WAbstractBackend(WAbstractBackendPrivate * p);

public: // Interface
    Q_INVOKABLE const QSizeF & getSize() const;
    Q_INVOKABLE void           setSize(const QSizeF & size);

    Q_INVOKABLE void drawFrame(QPainter * painter, const QStyleOptionGraphicsItem * option);

    Q_INVOKABLE void   updateFrame();
    Q_INVOKABLE QImage getFrame   () const;

    Q_INVOKABLE bool deleteBackend();

public: // WBackendInterface implementation
    Q_INVOKABLE /* virtual */ QUrl source() const;

    Q_INVOKABLE /* virtual */ bool sourceIsVideo() const;
    Q_INVOKABLE /* virtual */ bool sourceIsAudio() const;

    Q_INVOKABLE /* virtual */ void loadSource(const QUrl & url, int duration    = -1,
                                                                int currentTime = -1);

    Q_INVOKABLE /* virtual */ void play  ();
    Q_INVOKABLE /* virtual */ void replay();

    Q_INVOKABLE /* virtual */ void pause();
    Q_INVOKABLE /* virtual */ void stop ();
    Q_INVOKABLE /* virtual */ void clear();

    Q_INVOKABLE /* virtual */ void seek(int msec);

protected: // Functions
    void setState    (State     state);
    void setStateLoad(StateLoad stateLoad);

    void setEnded(bool ended);

    void setCurrentTime(int msec);
    void setDuration   (int msec);

    void setProgress(qreal progress);

    void setOutputActive (Output  output);
    void setQualityActive(Quality quality);

    void deleteNow();

protected: // Abstract functions
    virtual bool backendSetSource(const QUrl & url) = 0;

    virtual bool backendPlay () = 0;
    virtual bool backendPause() = 0;
    virtual bool backendStop () = 0;

    virtual void backendSetVolume(qreal volume) = 0;

    virtual bool backendDelete() = 0;

protected: // Virtual functions
    virtual void backendSeekTo(int msec); /* {} */

    virtual void backendSetSpeed(qreal speed); /* {} */

    virtual void backendSetOutput (Output  output);  /* {} */
    virtual void backendSetQuality(Quality quality); /* {} */

    virtual void backendSetFillMode(FillMode fillMode); /* {} */

    virtual void backendSetSize(const QSizeF & size); /* {} */

    virtual void backendDrawFrame(QPainter                       * painter,
                                  const QStyleOptionGraphicsItem * option); /* {} */

    virtual void   backendUpdateFrame();       /* {} */
    virtual QImage backendGetFrame   () const; /* {} */

signals:
    void ended();

    void parentItemChanged();

    void filterChanged();

    void sourceChanged();

    void stateChanged    ();
    void stateLoadChanged();

    void startedChanged();
    void endedChanged  ();

    void currentTimeChanged();
    void durationChanged   ();

    void progressChanged();

    void speedChanged();

    void volumeChanged();

    void repeatChanged();

    void outputChanged ();
    void qualityChanged();

    void outputActiveChanged ();
    void qualityActiveChanged();

    void fillModeChanged();

public: // Properties
    QGraphicsItem * parentItem() const;
    void            setParentItem(QGraphicsItem * parent);

    WBackendFilter * filter() const;
    void             setFilter(WBackendFilter * filter);

    void setSource(const QUrl & url);

    State     state    () const;
    StateLoad stateLoad() const;

    bool isLoading() const;

    bool isStarting () const;
    bool isResuming () const;
    bool isBuffering() const;

    bool isPlaying() const;
    bool isPaused () const;
    bool isStopped() const;

    bool hasStarted() const;
    bool hasEnded  () const;

    int currentTime() const;
    int duration   () const;

    qreal progress() const;

    qreal speed() const;
    void  setSpeed(qreal speed);

    qreal volume() const;
    void  setVolume(qreal volume);

    bool repeat() const;
    void setRepeat(bool repeat);

    Output output() const;
    void   setOutput(Output output);

    Quality quality() const;
    void    setQuality(Quality quality);

    Output  outputActive () const;
    Quality qualityActive() const;

    FillMode fillMode() const;
    void     setFillMode(FillMode fillMode);

private:
    W_DECLARE_PRIVATE(WAbstractBackend)

    friend class WAbstractHook;
    friend class WAbstractHookPrivate;
};

//-------------------------------------------------------------------------------------------------
// WBackendFilter
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WBackendFilter
{
public:
    virtual void filterState    (WAbstractBackend::State     * state);     /* {} */
    virtual void filterStateLoad(WAbstractBackend::StateLoad * stateLoad); /* {} */

    virtual void filterEnded(bool * ended); /* {} */

    virtual void filterCurrentTime(int * msec); /* {} */
    virtual void filterDuration   (int * msec); /* {} */

    virtual void filterProgress(qreal * progress); /* {} */

    virtual void filterOutputActive (WAbstractBackend::Output  * output);  /* {} */
    virtual void filterQualityActive(WAbstractBackend::Quality * quality); /* {} */
};

#endif // SK_NO_ABSTRACTBACKEND
#endif // WABSTRACTBACKEND_H
