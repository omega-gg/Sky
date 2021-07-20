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

#ifndef WABSTRACTBACKEND_H
#define WABSTRACTBACKEND_H

// Qt includes
#include <QObject>
#include <QImage>
#ifdef QT_LATEST
#include <QSGGeometryNode>
#endif

// Sk includes
#include <Sk>

#ifndef SK_NO_ABSTRACTBACKEND

// Forward declarations
#ifdef QT_4
class QPainter;
class QStyleOptionGraphicsItem;
#endif
class WAbstractBackendPrivate;
class WDeclarativePlayer;
class WBackendFilter;
class WBackendOutput;
#ifdef QT_LATEST
struct WBackendFrame;
struct WBackendTexture;
#endif


#ifdef QT_LATEST

//-------------------------------------------------------------------------------------------------
// WBackendNode
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WBackendNode : public QSGGeometryNode
{
public:
    WBackendNode();

public: // Interface
    void setRect(const QRectF & rect);

public: // Abstract interface
    virtual void setTextures(WBackendTexture * textures) = 0;

private: // Variables
    QSGGeometry _geometry;

    QRectF _source;
};

#endif

//-------------------------------------------------------------------------------------------------
// WBackendInterface
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WBackendInterface
{
public:
    virtual QString source() const = 0;

    virtual bool sourceIsVideo() const = 0;
    virtual bool sourceIsAudio() const = 0;

    virtual void loadSource(const QString & url, int duration = -1, int currentTime = -1) = 0;

    virtual void play  () = 0;
    virtual void replay() = 0;

    virtual void pause() = 0;
    virtual void stop () = 0;
    virtual void clear() = 0;

    virtual void seek(int msec) = 0;
};

//-------------------------------------------------------------------------------------------------
// WBackendWatcher
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WBackendWatcher
{
protected:
    virtual void beginOutputInsert(int first, int last);
    virtual void endOutputInsert  ();

    virtual void beginOutputRemove(int first, int last);
    virtual void endOutputRemove  ();

    virtual void beginOutputClear();
    virtual void endOutputClear  ();

    virtual void currentOutputChanged(int index);

    virtual void backendDestroyed();

private:
    friend class WAbstractBackend;
    friend class WAbstractBackendPrivate;
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
    Q_ENUMS(OutputType)
    Q_ENUMS(Quality)
    Q_ENUMS(FillMode)
#ifdef QT_LATEST
    Q_ENUMS(FrameState)
#endif

    Q_PROPERTY(WDeclarativePlayer * parentItem READ parentItem WRITE setParentItem
               NOTIFY parentItemChanged)

    Q_PROPERTY(WBackendFilter * filter READ filter WRITE setFilter NOTIFY filterChanged)

    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)

    Q_PROPERTY(bool sourceIsVideo READ sourceIsVideo NOTIFY sourceChanged)
    Q_PROPERTY(bool sourceIsAudio READ sourceIsAudio NOTIFY sourceChanged)

    Q_PROPERTY(State     state     READ state     NOTIFY stateChanged)
    Q_PROPERTY(StateLoad stateLoad READ stateLoad NOTIFY stateLoadChanged)

    Q_PROPERTY(bool isLoading READ isLoading NOTIFY stateLoadChanged)

    Q_PROPERTY(bool isDefault   READ isDefault   NOTIFY stateLoadChanged)
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

    Q_PROPERTY(bool scanOutput READ scanOutput WRITE setScanOutput NOTIFY scanOutputChanged)

    Q_PROPERTY(int currentOutput READ currentOutput WRITE setCurrentOutput
               NOTIFY currentOutputChanged)

    Q_PROPERTY(QString    outputName READ outputName NOTIFY currentOutputChanged)
    Q_PROPERTY(OutputType outputType READ outputType NOTIFY currentOutputChanged)

    Q_PROPERTY(int countOutput READ countOutput NOTIFY outputsChanged)

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
        OutputAudio
    };

    enum OutputType
    {
        TypeDefault,
        TypeUnknown,
        TypeChromecast
    };

    enum Quality
    {
        QualityInvalid,
        Quality144,
        Quality240,
        Quality360,
        Quality480,
        Quality720,
        Quality1080,
        Quality1440,
        Quality2160
    };

    enum FillMode
    {
        Stretch,
        PreserveAspectFit,
        PreserveAspectCrop
    };

#ifdef QT_LATEST
    enum FrameState
    {
        FrameDefault,
        FrameReset,
        FrameUpdate,
        FrameClear
    };
#endif

public:
    WAbstractBackend();
protected:
    WAbstractBackend(WAbstractBackendPrivate * p);

public: // Interface
#ifdef QT_LATEST
    Q_INVOKABLE WBackendNode * createNode() const;
#endif

    Q_INVOKABLE const QSizeF & getSize() const;
    Q_INVOKABLE void           setSize(const QSizeF & size);

#ifdef QT_LATEST
    Q_INVOKABLE void synchronize(WBackendFrame * frame);
#endif

    Q_INVOKABLE void drawFrame(QPainter * painter, const QRect & rect);

    Q_INVOKABLE void   updateFrame();
    Q_INVOKABLE QImage getFrame   () const;

    Q_INVOKABLE QRectF getRect() const;

    Q_INVOKABLE bool deleteBackend();

    //---------------------------------------------------------------------------------------------
    // Output

    Q_INVOKABLE WBackendOutput outputAt(int index) const;

    Q_INVOKABLE const WBackendOutput * outputPointerAt(int index) const;

    Q_INVOKABLE const WBackendOutput * currentOutputPointer() const;

    Q_INVOKABLE int indexOutput(const WBackendOutput * item) const;

    //---------------------------------------------------------------------------------------------
    // Watchers

    Q_INVOKABLE void registerWatcher  (WBackendWatcher * watcher);
    Q_INVOKABLE void unregisterWatcher(WBackendWatcher * watcher);

public: // WBackendInterface implementation
    Q_INVOKABLE /* virtual */ QString source() const;

    Q_INVOKABLE /* virtual */ bool sourceIsVideo() const;
    Q_INVOKABLE /* virtual */ bool sourceIsAudio() const;

    Q_INVOKABLE /* virtual */ void loadSource(const QString & url, int duration    = -1,
                                                                   int currentTime = -1);

    Q_INVOKABLE /* virtual */ void play  ();
    Q_INVOKABLE /* virtual */ void replay();

    Q_INVOKABLE /* virtual */ void pause();
    Q_INVOKABLE /* virtual */ void stop ();
    Q_INVOKABLE /* virtual */ void clear();

    Q_INVOKABLE /* virtual */ void seek(int msec);

protected: // Functions
    void addOutput(const WBackendOutput & output);

    void removeOutput(int index);

    void clearOutputs();

    void setState    (State     state);
    void setStateLoad(StateLoad stateLoad);

    void setEnded(bool ended);

    void setCurrentTime(int msec);
    void setDuration   (int msec);

    void setProgress(qreal progress);

    void setOutputActive (Output  output);
    void setQualityActive(Quality quality);

    void deleteNow();

    //---------------------------------------------------------------------------------------------
    // Watchers

    void beginOutputInsert(int first, int last) const;
    void beginOutputRemove(int first, int last) const;

    void endOutputInsert() const;
    void endOutputRemove() const;

protected: // Abstract functions
#ifdef QT_LATEST
    virtual WBackendNode * backendCreateNode() const = 0;
#endif

    virtual bool backendSetSource(const QString & url) = 0;

    virtual bool backendPlay () = 0;
    virtual bool backendPause() = 0;
    virtual bool backendStop () = 0;

    virtual void backendSetVolume(qreal volume) = 0;

    virtual bool backendDelete() = 0;

protected: // Virtual functions
    virtual void backendSeek(int msec); // {}

    virtual void backendSetSpeed(qreal speed); // {}

    virtual void backendSetOutput (Output  output);  // {}
    virtual void backendSetQuality(Quality quality); // {}

    virtual void backendSetFillMode(FillMode fillMode); // {}

    virtual void backendSetScanOutput(bool enabled); // {}

    virtual void backendSetCurrentOutput(int index); // {}

    virtual void backendSetSize(const QSizeF & size); // {}

#ifdef QT_LATEST
    virtual void backendSynchronize(WBackendFrame * frame); // {}
#endif

    virtual void backendDrawFrame(QPainter * painter, const QRect & rect); // {}

    virtual void   backendUpdateFrame();       // {}
    virtual QImage backendGetFrame   () const; // {}

    virtual QRectF backendRect() const; // {}

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

    void scanOutputChanged();

    void currentOutputChanged();

    void outputsChanged();

public: // Properties
    WDeclarativePlayer * parentItem() const;
    void                 setParentItem(WDeclarativePlayer * parent);

    WBackendFilter * filter() const;
    void             setFilter(WBackendFilter * filter);

    void setSource(const QString & url);

    State     state    () const;
    StateLoad stateLoad() const;

    bool isLoading() const;

    bool isDefault  () const;
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

    bool scanOutput() const;
    void setScanOutput(bool enabled);

    int  currentOutput() const;
    void setCurrentOutput(int index);

    QString    outputName() const;
    OutputType outputType() const;

    int countOutput() const;

private:
    W_DECLARE_PRIVATE(WAbstractBackend)

    friend class WAbstractHook;
    friend class WAbstractHookPrivate;
};

//-------------------------------------------------------------------------------------------------
// WBackendOutput
//-------------------------------------------------------------------------------------------------

class WBackendOutput
{
public:
    WBackendOutput(const QString & name,
                   WAbstractBackend::OutputType type = WAbstractBackend::TypeDefault);

    WBackendOutput();

public: // Operators
    WBackendOutput(const WBackendOutput & other);

    bool operator==(const WBackendOutput & other) const;

    WBackendOutput & operator=(const WBackendOutput & other);

public: // Variables
    QString name;

    WAbstractBackend::OutputType type;
};

//-------------------------------------------------------------------------------------------------
// WBackendFilter
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WBackendFilter
{
public:
    virtual void filterAddOutput(WBackendOutput * output); // {}

    virtual void filterRemoveOutput(int * index); // {}

    virtual void filterState    (WAbstractBackend::State     * state);     // {}
    virtual void filterStateLoad(WAbstractBackend::StateLoad * stateLoad); // {}

    virtual void filterEnded(bool * ended); // {}

    virtual void filterCurrentTime(int * msec); // {}
    virtual void filterDuration   (int * msec); // {}

    virtual void filterProgress(qreal * progress); // {}

    virtual void filterOutput      (WAbstractBackend::Output  * output);  // {}
    virtual void filterOutputActive(WAbstractBackend::Output  * output);  // {}

    virtual void filterQuality      (WAbstractBackend::Quality * quality); // {}
    virtual void filterQualityActive(WAbstractBackend::Quality * quality); // {}

    virtual void filterSpeed(qreal * speed); // {}

    virtual void filterVolume(qreal * volume); // {}

    virtual void filterFillMode(WAbstractBackend::FillMode * fillMode); // {}

    virtual void filterScanOutput(bool * enabled); // {}

    virtual void filterCurrentOutput(int * index); // {}
};

#ifdef QT_LATEST

//-------------------------------------------------------------------------------------------------
// WBackendTexture
//-------------------------------------------------------------------------------------------------

struct WBackendTexture
{
    WBackendTexture()
    {
        width  = 0;
        height = 0;

        bits = NULL;
    }

    int width;
    int height;

    uchar * bits;
};

//-------------------------------------------------------------------------------------------------
// WBackendFrame
//-------------------------------------------------------------------------------------------------

struct WBackendFrame
{
    WBackendFrame()
    {
        state = WAbstractBackend::FrameDefault;
    }

    WBackendTexture textures[3];

    WAbstractBackend::FrameState state;
};

#endif

#endif // SK_NO_ABSTRACTBACKEND
#endif // WABSTRACTBACKEND_H
