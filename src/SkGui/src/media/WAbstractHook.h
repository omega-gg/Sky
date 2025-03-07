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

#ifndef WABSTRACTHOOK_H
#define WABSTRACTHOOK_H

// Sk includes
#include <WAbstractBackend>

#ifndef SK_NO_ABSTRACTHOOK

class WAbstractHookPrivate;

class SK_GUI_EXPORT WAbstractHook : public QObject,
                                    public WBackendInterface, public WBackendFilter,
                                    public WPrivatable
{
    Q_OBJECT

    Q_PROPERTY(WAbstractBackend * backend READ backend CONSTANT)

    Q_PROPERTY(bool filterActive READ filterActive WRITE setFilterActive
               NOTIFY filterActiveChanged)

protected:
    WAbstractHook(WAbstractBackend * backend);

    WAbstractHook(WAbstractHookPrivate * p, WAbstractBackend * backend);

public: // Interface
    Q_INVOKABLE bool check(const QString & url);

public: // WBackendInterface implementation
    Q_INVOKABLE /* virtual */ QString source() const;

    Q_INVOKABLE /* virtual */ bool sourceIsVideo() const;
    Q_INVOKABLE /* virtual */ bool sourceIsAudio() const;

    Q_INVOKABLE /* virtual */ void loadSource(const QString     & url,
                                              int                 duration    = -1,
                                              int                 currentTime = -1,
                                              const WMediaReply * reply       = NULL);

    Q_INVOKABLE /* virtual */ void play  ();
    Q_INVOKABLE /* virtual */ void replay();

    Q_INVOKABLE /* virtual */ void pause();
    Q_INVOKABLE /* virtual */ void stop ();
    Q_INVOKABLE /* virtual */ void clear();

    Q_INVOKABLE /* virtual */ void seek(int msec);

protected: // Functions
    void applySource(const QString & url);

    void applyState    (WAbstractBackend::State     state);
    void applyStateLoad(WAbstractBackend::StateLoad stateLoad);

    void applyCurrentTime(int msec);

    //---------------------------------------------------------------------------------------------
    // Backend functions

    // NOTE: Resets and applies all the tracks at once (video / audio).
    void applyTracks(const QList<WBackendTrack> & tracks, int trackVideo = -1,
                                                          int trackAudio = -1);

    void applyVideos(const QList<WBackendTrack> & videos, int trackVideo = -1);
    void applyAudios(const QList<WBackendTrack> & audios, int trackAudio = -1);

    const WBackendOutput * addOutput(const WBackendOutput & output);

    bool removeOutput(const WBackendOutput * output);

    void setState    (WAbstractBackend::State     state);
    void setStateLoad(WAbstractBackend::StateLoad stateLoad);

    void setVbml(bool vbml);
    void setLive(bool live);

    void setStarted(bool started);
    void setEnded  (bool ended);

    void setCurrentTime(int msec);
    void setDuration   (int msec);

    void setProgress(qreal progress);

    void setOutputActive (WAbstractBackend::Output  output);
    void setQualityActive(WAbstractBackend::Quality quality);

    void setContext(const QString & context, const QString & contextId);

    void setChapters(const QList<WChapter> & chapters);

    void setAmbient(const QString & ambient);

    void setSubtitles(const QList<WSubtitle> & subtitles);

    //---------------------------------------------------------------------------------------------
    // Backend abstract functions

    bool backendSetSource(const QString & url, const WMediaReply * reply);

    bool backendPlay ();
    bool backendPause();
    bool backendStop ();

    void backendSetVolume(qreal volume);

    bool backendDelete();

    //---------------------------------------------------------------------------------------------
    // Backend virtual functions

    void backendSeek(int msec);

    void backendSetSpeed(qreal speed);

    void backendSetOutput    (WAbstractBackend::Output     output);
    void backendSetQuality   (WAbstractBackend::Quality    quality);
    void backendSetSourceMode(WAbstractBackend::SourceMode mode);

    void backendSetFillMode(WAbstractBackend::FillMode fillMode);

    void backendSetVideo(int id);
    void backendSetAudio(int id);

    void backendSetScanOutput(bool enabled);

    void backendSetCurrentOutput(const WBackendOutput * output);

    void backendSetSize(const QSizeF & size);

#ifndef SK_NO_PLAYER
#if defined(QT_NEW) && defined(SK_NO_QML) == false
    void backendSynchronize(WBackendFrame * frame);
#endif

    void backendDrawFrame(QPainter * painter, const QRect & rect);

    void backendUpdateFrame();

    QImage backendGetFrame    () const;
    QImage backendGetFrameGray() const;

    QRectF backendRect() const;
#endif

protected: // Abstract functions
    virtual bool hookCheck(const QString & url) = 0;

signals:
    // NOTE: This means the 'checked' function changed its conditions.
    void hookUpdated();

    void sourceChanged();

    void filterActiveChanged();

public: // Properties
    WAbstractBackend * backend() const;

    bool filterActive() const;
    void setFilterActive(bool active);

private:
    W_DECLARE_PRIVATE(WAbstractHook)

    Q_PRIVATE_SLOT(d_func(), void onBackendDestroyed())
};

#include <private/WAbstractHook_p>

#endif // SK_NO_ABSTRACTHOOK
#endif // WABSTRACTHOOK_H
