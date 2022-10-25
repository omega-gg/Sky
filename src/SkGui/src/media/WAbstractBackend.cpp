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

#include "WAbstractBackend.h"

#ifndef SK_NO_ABSTRACTBACKEND

// Sk includes
#include <WControllerPlaylist>
#ifndef SK_NO_QML
#include <WDeclarativePlayer>
#endif

#if defined(QT_NEW) && defined(SK_NO_QML) == false

//=================================================================================================
// WBackendNode
//=================================================================================================

WBackendNode::WBackendNode() : _geometry(QSGGeometry::defaultAttributes_TexturedPoint2D(), 4)
{
    _source = QRectF(0.0, 0.0, 1.0, 1.0);

    setGeometry(&_geometry);
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

void WBackendNode::setRect(const QRectF & rect)
{
    QSGGeometry::updateTexturedRectGeometry(&_geometry, rect, _source);

    markDirty(QSGNode::DirtyGeometry);
}

#endif

//=================================================================================================
// WAbstractBackendPrivate
//=================================================================================================

#include "WAbstractBackend_p.h"

WAbstractBackendPrivate::WAbstractBackendPrivate(WAbstractBackend * p) : WPrivate(p) {}

void WAbstractBackendPrivate::init()
{
#ifndef SK_NO_QML
    player = NULL;
#endif

    filter = NULL;

    state     = WAbstractBackend::StateStopped;
    stateLoad = WAbstractBackend::StateLoadDefault;

    live = false;

    started = false;
    ended   = false;

    currentTime = -1;
    duration    = -1;

    progress = 0.0;

    speed = 1.0;

    volume = 1.0;

    repeat = false;

    output  = WAbstractBackend::OutputMedia;
    quality = WAbstractBackend::QualityDefault;

    outputActive  = WAbstractBackend::OutputNone;
    qualityActive = WAbstractBackend::QualityDefault;

    fillMode = WAbstractBackend::PreserveAspectFit;

    trackVideo = -1;
    trackAudio = -1;

    scanOutput = false;

    currentOutput = 0;

    deleting = false;

    outputData.name = QObject::tr("Default");

    outputs.append(WBackendOutput(outputData.name));
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WAbstractBackendPrivate::clearCurrentTime()
{
    if (currentTime == -1) return;

    Q_Q(WAbstractBackend);

    currentTime = -1;

    emit q->currentTimeChanged();
}

//-------------------------------------------------------------------------------------------------

void WAbstractBackendPrivate::setStarted(bool started)
{
    if (this->started == started) return;

    Q_Q(WAbstractBackend);

    this->started = started;

    emit q->startedChanged();
}

//-------------------------------------------------------------------------------------------------

void WAbstractBackendPrivate::currentOutputChanged()
{
    Q_Q(WAbstractBackend);

    foreach (WBackendWatcher * watcher, watchers)
    {
        watcher->currentOutputChanged(currentOutput);
    }

    emit q->currentOutputChanged();
}

//=================================================================================================
// WBackendWatcher
//=================================================================================================

/* virtual */ void WBackendWatcher::beginOutputInsert(int, int) {}
/* virtual */ void WBackendWatcher::endOutputInsert  ()         {}

/* virtual */ void WBackendWatcher::beginOutputRemove(int, int) {}
/* virtual */ void WBackendWatcher::endOutputRemove  ()         {}

/* virtual */ void WBackendWatcher::currentOutputChanged(int) {}

/* virtual */ void WBackendWatcher::backendDestroyed() {}

//=================================================================================================
// WAbstractBackend
//=================================================================================================

WAbstractBackend::WAbstractBackend()
    : QObject(), WPrivatable(new WAbstractBackendPrivate(this))
{
    Q_D(WAbstractBackend); d->init();
}

//-------------------------------------------------------------------------------------------------
// Protected

WAbstractBackend::WAbstractBackend(WAbstractBackendPrivate * p)
    : QObject(), WPrivatable(p)
{
    Q_D(WAbstractBackend); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

#if defined(QT_NEW) && defined(SK_NO_QML) == false

/* Q_INVOKABLE */ WBackendNode * WAbstractBackend::createNode() const
{
    return backendCreateNode();
}

#endif

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ const QSizeF & WAbstractBackend::getSize() const
{
    Q_D(const WAbstractBackend); return d->size;
}

/* Q_INVOKABLE */ void WAbstractBackend::setSize(const QSizeF & size)
{
    Q_D(WAbstractBackend);

    if (d->size == size) return;

    d->size = size;

    backendSetSize(size);
}

//-------------------------------------------------------------------------------------------------

#ifndef SK_NO_QML

#ifdef QT_NEW

/* Q_INVOKABLE */ void WAbstractBackend::synchronize(WBackendFrame * frame)
{
    backendSynchronize(frame);
}

#endif

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WAbstractBackend::drawFrame(QPainter * painter, const QRect & rect)
{
    backendDrawFrame(painter, rect);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WAbstractBackend::updateFrame()
{
    backendUpdateFrame();
}

/* Q_INVOKABLE */ QImage WAbstractBackend::getFrame() const
{
    return backendGetFrame();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ QRectF WAbstractBackend::getRect() const
{
    return backendRect();
}

#endif

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ WAbstractBackend::SourceMode WAbstractBackend::getMode() const
{
    Q_D(const WAbstractBackend);

    if (d->output == OutputAudio) return WAbstractBackend::SourceAudio;
    else                          return WAbstractBackend::SourceDefault;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ bool WAbstractBackend::deleteBackend()
{
    Q_D(WAbstractBackend);

    if (d->deleting) return false;

    if (backendDelete())
    {
        delete this;

        return true;
    }
    else
    {
        d->deleting = true;

        setParent(NULL);

#ifndef SK_NO_QML
        setPlayer(NULL);
#endif

        setState(StateStopped);

        return false;
    }
}

//-------------------------------------------------------------------------------------------------
// Tracks

/* Q_INVOKABLE */ QList<WBackendTrack> WAbstractBackend::videos() const
{
    Q_D(const WAbstractBackend); return d->videos;
}

/* Q_INVOKABLE */ QList<WBackendTrack> WAbstractBackend::audios() const
{
    Q_D(const WAbstractBackend); return d->audios;
}

/* Q_INVOKABLE */ int WAbstractBackend::idVideo(int index) const
{
    Q_D(const WAbstractBackend);

    if (index < 0 || index >= d->videos.count()) return -1;

    return d->videos.at(index).id;
}

/* Q_INVOKABLE */ int WAbstractBackend::idAudio(int index) const
{
    Q_D(const WAbstractBackend);

    if (index < 0 || index >= d->audios.count()) return -1;

    return d->audios.at(index).id;
}

/* Q_INVOKABLE */ int WAbstractBackend::indexVideo(int id) const
{
    Q_D(const WAbstractBackend);

    for (int i = 0; i < d->videos.count(); i++)
    {
        if (d->videos.at(i).id != id) continue;

        return i;
    }

    return -1;
}

/* Q_INVOKABLE */ int WAbstractBackend::indexAudio(int id) const
{
    Q_D(const WAbstractBackend);

    for (int i = 0; i < d->audios.count(); i++)
    {
        if (d->audios.at(i).id != id) continue;

        return i;
    }

    return -1;
}

/* Q_INVOKABLE */ QString WAbstractBackend::videoName(int id) const
{
    int index = indexVideo(id);

    if (index == -1) return QString();

    Q_D(const WAbstractBackend);

    return d->videos.at(index).name;
}

/* Q_INVOKABLE */ QString WAbstractBackend::audioName(int id) const
{
    int index = indexAudio(id);

    if (index == -1) return QString();

    Q_D(const WAbstractBackend);

    return d->audios.at(index).name;
}

//-------------------------------------------------------------------------------------------------
// Output

/* Q_INVOKABLE */ WBackendOutput WAbstractBackend::outputAt(int index) const
{
    Q_D(const WAbstractBackend);

    if (index < 0 || index >= d->outputs.count())
    {
        return WBackendOutput();
    }
    else return d->outputs.at(index);
}

/* Q_INVOKABLE */ const WBackendOutput * WAbstractBackend::outputPointerAt(int index) const
{
    Q_D(const WAbstractBackend);

    if (index < 0 || index >= d->outputs.count()) return NULL;

    return &(d->outputs.at(index));
}

/* Q_INVOKABLE */ const WBackendOutput * WAbstractBackend::currentOutputPointer() const
{
    Q_D(const WAbstractBackend);

    return outputPointerAt(d->currentOutput);
}

/* Q_INVOKABLE */ int WAbstractBackend::indexOutput(const WBackendOutput * output) const
{
    Q_D(const WAbstractBackend);

    for (int i = 0; i < d->outputs.count(); i++)
    {
        if (&(d->outputs.at(i)) == output) return i;
    }

    return -1;
}

//-------------------------------------------------------------------------------------------------
// Watchers

/* Q_INVOKABLE */ void WAbstractBackend::registerWatcher(WBackendWatcher * watcher)
{
    Q_ASSERT(watcher);

    Q_D(WAbstractBackend);

    if (d->watchers.contains(watcher)) return;

    d->watchers.append(watcher);
}

/* Q_INVOKABLE */ void WAbstractBackend::unregisterWatcher(WBackendWatcher * watcher)
{
    Q_ASSERT(watcher);

    Q_D(WAbstractBackend);

    if (d->watchers.contains(watcher) == false) return;

    d->watchers.removeOne(watcher);
}

//-------------------------------------------------------------------------------------------------
// Static functions
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */
WAbstractBackend::State WAbstractBackend::stateFromString(const QString & string)
{
    if      (string == "playing") return StatePlaying;
    else if (string == "paused")  return StatePaused;
    else                          return StateStopped;
}

/* Q_INVOKABLE static */
WAbstractBackend::StateLoad WAbstractBackend::stateLoadFromString(const QString & string)
{
    if      (string == "starting")  return StateLoadStarting;
    else if (string == "resuming")  return StateLoadResuming;
    else if (string == "buffering") return StateLoadBuffering;
    else                            return StateLoadDefault;
}

/* Q_INVOKABLE static */
WAbstractBackend::Output WAbstractBackend::outputFromString(const QString & string)
{
    if      (string == "none")  return OutputNone;
    else if (string == "video") return OutputVideo;
    else if (string == "audio") return OutputAudio;
    else                        return OutputMedia;
}

/* Q_INVOKABLE static */
WAbstractBackend::Quality WAbstractBackend::qualityFromString(const QString & string)
{
    if      (string ==  "144") return Quality144;
    else if (string ==  "240") return Quality240;
    else if (string ==  "360") return Quality360;
    else if (string ==  "480") return Quality480;
    else if (string ==  "720") return Quality720;
    else if (string == "1080") return Quality1080;
    else if (string == "1440") return Quality1440;
    else if (string == "2160") return Quality2160;
    else                       return QualityDefault;
}

/* Q_INVOKABLE static */
WAbstractBackend::FillMode WAbstractBackend::fillModeFromString(const QString & string)
{
    if      (string == "stretch") return Stretch;
    else if (string == "crop")    return PreserveAspectCrop;
    else                          return PreserveAspectFit;
}

/* Q_INVOKABLE static */
WBackendTrack WAbstractBackend::trackFromString(const QString & string)
{
    QStringList list = string.split('|');

    if (list.count() != 3) return WBackendTrack();

    WAbstractBackend::TrackType type;

    if (list.at(1) == "audio")
    {
         type = WAbstractBackend::TrackAudio;
    }
    else type = WAbstractBackend::TrackVideo;

    return WBackendTrack(list.at(0).toInt(), type, list.at(2));
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WAbstractBackend::stateToString(State state)
{
    if      (state == StatePlaying) return "playing";
    else if (state == StatePaused)  return "paused";
    else                            return "stopped";
}

/* Q_INVOKABLE static */ QString WAbstractBackend::stateLoadToString(StateLoad stateLoad)
{
    if      (stateLoad == StateLoadStarting)  return "starting";
    else if (stateLoad == StateLoadResuming)  return "resuming";
    else if (stateLoad == StateLoadBuffering) return "buffering";
    else                                      return "default";
}

/* Q_INVOKABLE static */ QString WAbstractBackend::outputToString(Output output)
{
    if      (output == OutputNone)  return "none";
    else if (output == OutputVideo) return "video";
    else if (output == OutputAudio) return "audio";
    else                            return "media";
}

/* Q_INVOKABLE static */ QString WAbstractBackend::qualityToString(Quality quality)
{
    if      (quality == Quality144)  return  "144";
    else if (quality == Quality240)  return  "240";
    else if (quality == Quality360)  return  "360";
    else if (quality == Quality480)  return  "480";
    else if (quality == Quality720)  return  "720";
    else if (quality == Quality1080) return "1080";
    else if (quality == Quality1440) return "1440";
    else if (quality == Quality2160) return "2160";
    else                             return "default";
}

/* Q_INVOKABLE static */ QString WAbstractBackend::fillModeToString(FillMode fillMode)
{
    if      (fillMode == Stretch)            return "stretch";
    else if (fillMode == PreserveAspectCrop) return "crop";
    else                                     return "fit";
}

/* Q_INVOKABLE static */ QString WAbstractBackend::trackToString(const WBackendTrack & track)
{
    QString result = QString::number(track.id) + '|';

    if (track.type == WAbstractBackend::TrackAudio)
    {
         result += "audio|";
    }
    else result += "video|";

    return result + track.name;
}

//-------------------------------------------------------------------------------------------------
// WBackendInterface implementation
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QString WAbstractBackend::source() const
{
    Q_D(const WAbstractBackend); return d->source;
}

/* Q_INVOKABLE virtual */ bool WAbstractBackend::sourceIsVideo() const
{
    Q_D(const WAbstractBackend);

    return wControllerPlaylist->sourceIsVideo(d->source);
}

/* Q_INVOKABLE virtual */ bool WAbstractBackend::sourceIsAudio() const
{
    Q_D(const WAbstractBackend);

    return wControllerPlaylist->sourceIsAudio(d->source);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ void WAbstractBackend::loadSource(const QString & url, int duration,
                                                                                 int currentTime)
{
    Q_D(WAbstractBackend);

    if (d->source != url)
    {
        if (d->state == StatePaused || url.isEmpty()) stop();

        d->source = url;

        setDuration   (duration);
        setCurrentTime(currentTime);

        backendSetSource(url);

        //---------------------------------------------------------------------------------------------
        // NOTE: We need to clear these when changing the source.

        if (d->videos.isEmpty() == false)
        {
            d->videos.clear();

            emit videosChanged();
        }

        if (d->audios.isEmpty() == false)
        {
            d->audios.clear();

            emit audiosChanged();
        }

        setLive(false);

        //---------------------------------------------------------------------------------------------

        emit sourceChanged();
    }
    else if (d->state == StatePlaying)
    {
        backendPause();

        setDuration   (duration);
        setCurrentTime(currentTime);

        setStateLoad(StateLoadResuming);

        backendPlay();
    }
    else
    {
        if (d->state == StatePaused) stop();

        setDuration   (duration);
        setCurrentTime(currentTime);
    }
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ void WAbstractBackend::play()
{
    Q_D(WAbstractBackend);

    if (d->state == StatePlaying || d->source.isEmpty()) return;

    if (backendPlay()) setState(StatePlaying);
}

/* Q_INVOKABLE virtual */ void WAbstractBackend::replay()
{
    Q_D(WAbstractBackend);

    if (d->source.isEmpty()) return;

    d->clearCurrentTime();

    if (backendPlay()) setState(StatePlaying);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ void WAbstractBackend::pause()
{
    Q_D(WAbstractBackend);

    if (d->state != StatePlaying) return;

    if (backendPause()) setState(StatePaused);
}

/* Q_INVOKABLE virtual */ void WAbstractBackend::stop()
{
    Q_D(WAbstractBackend);

    if (d->state == StateStopped) return;

    if (backendStop()) setState(StateStopped);
}

/* Q_INVOKABLE virtual */ void WAbstractBackend::clear()
{
    loadSource(QString());
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ void WAbstractBackend::seek(int msec)
{
    Q_D(WAbstractBackend);

    if (d->currentTime == msec) return;

    d->currentTime = msec;

    if (d->started) backendSeek(msec);

    emit currentTimeChanged();
}

//-------------------------------------------------------------------------------------------------
// Protected functions
//-------------------------------------------------------------------------------------------------

#ifndef SK_NO_QML

void WAbstractBackend::applyFrame() const
{
    Q_D(const WAbstractBackend);

#ifdef QT_4
    if (d->player) d->player->update();
#else
    if (d->player) d->player->updateFrame();
#endif
}

#endif

void WAbstractBackend::stopError(const QString & message)
{
    qWarning("WAbstractBackend::stopError: %s.", message.C_STR);

    emit error(message);

    stop();
}

void WAbstractBackend::applyTracks(const QList<WBackendTrack> & tracks,
                                   int trackVideo, int trackAudio)
{
    QList<WBackendTrack> videos;
    QList<WBackendTrack> audios;

    foreach (const WBackendTrack & track, tracks)
    {
        if (track.type == TrackVideo)
        {
            videos.append(track);
        }
        else if (track.type == TrackAudio)
        {
            audios.append(track);
        }
    }

    applyVideos(videos, trackVideo);
    applyAudios(audios, trackAudio);
}

void WAbstractBackend::applyVideos(const QList<WBackendTrack> & videos, int trackVideo)
{
    Q_D(WAbstractBackend);

    d->videos = videos;

    if (d->trackVideo == -1)
    {
        d->trackVideo = trackVideo;
    }
    // NOTE: When we have a valid track id we try to apply it to the player.
    else backendSetVideo(d->trackVideo);

    emit trackVideoChanged();

    emit videosChanged();
}

void WAbstractBackend::applyAudios(const QList<WBackendTrack> & audios, int trackAudio)
{
    Q_D(WAbstractBackend);

    d->audios = audios;

    if (d->trackAudio == -1)
    {
        d->trackAudio = trackAudio;
    }
    // NOTE: When we have a valid track id we try to apply it to the player.
    else backendSetAudio(d->trackAudio);

    emit trackAudioChanged();

    emit audiosChanged();
}

//-------------------------------------------------------------------------------------------------

const WBackendOutput * WAbstractBackend::addOutput(const WBackendOutput & output)
{
    Q_D(WAbstractBackend);

    WBackendOutput data = output;

    if (d->filter) d->filter->filterAddOutput(&data);

    int index = d->outputs.count();

    beginOutputInsert(index, index);

    d->outputs.append(data);

    endOutputInsert();

    emit outputsChanged();

    // NOTE: We are trying to restore the currentOutput if we've lost it before.
    if (d->currentOutput == -1 && d->outputData == data)
    {
        d->currentOutput = index;

        d->currentOutputChanged();
    }

    return &(d->outputs[index]);
}

bool WAbstractBackend::removeOutput(const WBackendOutput * output)
{
    int index = indexOutput(output);

    if (index == -1) return false;

    Q_D(WAbstractBackend);

    if (d->filter) d->filter->filterRemoveOutput(&index);

    beginOutputRemove(index, index);

    d->outputs.removeAt(index);

    endOutputRemove();

    if (index == d->currentOutput)
    {
        d->currentOutput = -1;

        d->currentOutputChanged();
    }
    else if (index < d->currentOutput)
    {
        d->currentOutput--;

        d->currentOutputChanged();
    }

    emit outputsChanged();

    return true;
}

//-------------------------------------------------------------------------------------------------

void WAbstractBackend::setState(State state)
{
    Q_D(WAbstractBackend);

    if (d->filter) d->filter->filterState(&state);

    if (d->state == state) return;

    d->state = state;

    if (state == StateStopped)
    {
        setStateLoad(StateLoadDefault);

        d->setStarted(false);

        d->clearCurrentTime();
    }
    else if (state == StatePlaying)
    {
        d->setStarted(true);
    }
    else setStateLoad(StateLoadDefault);

    emit stateChanged();
}

void WAbstractBackend::setStateLoad(StateLoad stateLoad)
{
    Q_D(WAbstractBackend);

    if (d->filter) d->filter->filterStateLoad(&stateLoad);

    if (d->stateLoad == stateLoad) return;

    d->stateLoad = stateLoad;

    emit stateLoadChanged();
}

//-------------------------------------------------------------------------------------------------

void WAbstractBackend::setLive(bool live)
{
    Q_D(WAbstractBackend);

    if (d->filter) d->filter->filterLive(&live);

    if (d->live == live) return;

    d->live = live;

    emit liveChanged();
}

//-------------------------------------------------------------------------------------------------

void WAbstractBackend::setEnded(bool ended)
{
    Q_D(WAbstractBackend);

    if (d->filter) d->filter->filterEnded(&ended);

    if (d->ended == ended) return;

    if (ended)
    {
        if (d->repeat)
        {
            d->clearCurrentTime();

            backendPlay();
        }
        else
        {
            d->ended = true;

            d->clearCurrentTime();

            emit endedChanged();

            emit this->ended();
        }
    }
    else
    {
        d->ended = false;

        emit endedChanged();
    }
}

//-------------------------------------------------------------------------------------------------

void WAbstractBackend::setCurrentTime(int msec)
{
    Q_D(WAbstractBackend);

    if (d->filter) d->filter->filterCurrentTime(&msec);

    if (d->currentTime == msec) return;

    d->currentTime = msec;

    if (msec != -1) setEnded(false);

    emit currentTimeChanged();
}

void WAbstractBackend::setDuration(int msec)
{
    Q_D(WAbstractBackend);

    if (d->filter) d->filter->filterDuration(&msec);

    if (d->duration == msec) return;

    d->duration = msec;

    emit durationChanged();
}

//-------------------------------------------------------------------------------------------------

void WAbstractBackend::setProgress(qreal progress)
{
    Q_D(WAbstractBackend);

    if (d->filter) d->filter->filterProgress(&progress);

    if (d->progress == progress) return;

    d->progress = progress;

    emit progressChanged();
}

//-------------------------------------------------------------------------------------------------

void WAbstractBackend::setOutputActive(Output output)
{
    Q_D(WAbstractBackend);

    if (d->filter) d->filter->filterOutputActive(&output);

    if (d->outputActive == output) return;

    d->outputActive = output;

    emit outputActiveChanged();
}

void WAbstractBackend::setQualityActive(Quality quality)
{
    Q_D(WAbstractBackend);

    if (d->filter) d->filter->filterQualityActive(&quality);

    if (d->qualityActive == quality) return;

    d->qualityActive = quality;

    emit qualityActiveChanged();
}

//-------------------------------------------------------------------------------------------------

void WAbstractBackend::deleteNow()
{
    Q_D(WAbstractBackend);

    if (d->deleting) delete this;
}

//---------------------------------------------------------------------------------------------
// Watchers

void WAbstractBackend::beginOutputInsert(int first, int last) const
{
    Q_D(const WAbstractBackend);

    foreach (WBackendWatcher * watcher, d->watchers)
    {
        watcher->beginOutputInsert(first, last);
    }
}

void WAbstractBackend::beginOutputRemove(int first, int last) const
{
    Q_D(const WAbstractBackend);

    foreach (WBackendWatcher * watcher, d->watchers)
    {
        watcher->beginOutputRemove(first, last);
    }
}

//---------------------------------------------------------------------------------------------

void WAbstractBackend::endOutputInsert() const
{
    Q_D(const WAbstractBackend);

    foreach (WBackendWatcher * watcher, d->watchers)
    {
        watcher->endOutputInsert();
    }
}

void WAbstractBackend::endOutputRemove() const
{
    Q_D(const WAbstractBackend);

    foreach (WBackendWatcher * watcher, d->watchers)
    {
        watcher->endOutputRemove();
    }
}

//-------------------------------------------------------------------------------------------------
// Protected virtual functions
//-------------------------------------------------------------------------------------------------

/* virtual */ void WAbstractBackend::backendSeek(int)
{
    qWarning("WAbstractBackend::backendSeek: Not supported.");
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WAbstractBackend::backendSetSpeed(qreal)
{
    qWarning("WAbstractBackend::backendSetSpeed: Not supported.");
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WAbstractBackend::backendSetOutput(Output)
{
    qWarning("WAbstractBackend::backendSetOutput: Not supported.");
}

/* virtual */ void WAbstractBackend::backendSetQuality(Quality)
{
    qWarning("WAbstractBackend::backendSetQuality: Not supported.");
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WAbstractBackend::backendSetFillMode(FillMode)
{
    qWarning("WAbstractBackend::backendSetFillMode: Not supported.");
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WAbstractBackend::backendSetVideo(int)
{
    qWarning("WAbstractBackend::backendSetVideo: Not supported.");
}

/* virtual */ void WAbstractBackend::backendSetAudio(int)
{
    qWarning("WAbstractBackend::backendSetAudio: Not supported.");
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WAbstractBackend::backendSetScanOutput(bool)
{
    qWarning("WAbstractBackend::backendSetScanOutput: Not supported.");
}

/* virtual */ void WAbstractBackend::backendSetCurrentOutput(const WBackendOutput *)
{
    qWarning("WAbstractBackend::backendSetCurrentOutput: Not supported.");
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WAbstractBackend::backendSetSize(const QSizeF &)
{
    qWarning("WAbstractBackend::backendSetSize: Not supported.");
}

//-------------------------------------------------------------------------------------------------

#ifndef SK_NO_QML

#ifdef QT_NEW

/* virtual */ void WAbstractBackend::backendSynchronize(WBackendFrame *)
{
    qWarning("WAbstractBackend::backendSynchronize: Not supported.");
}

#endif

//-------------------------------------------------------------------------------------------------

/* virtual */ void WAbstractBackend::backendDrawFrame(QPainter *, const QRect &)
{
    qWarning("WAbstractBackend::backendDrawFrame: Not supported.");
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WAbstractBackend::backendUpdateFrame()
{
    qWarning("WAbstractBackend::backendUpdateFrame: Not supported.");
}

/* virtual */ QImage WAbstractBackend::backendGetFrame() const
{
    qWarning("WAbstractBackend::backendGetFrame: Not supported.");

    return QImage();
}

//-------------------------------------------------------------------------------------------------

/* virtual */ QRectF WAbstractBackend::backendRect() const
{
    qWarning("WAbstractBackend::backendRect: Not supported.");

    return QRectF();
}

#endif

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

#ifndef SK_NO_QML

WDeclarativePlayer * WAbstractBackend::player() const
{
    Q_D(const WAbstractBackend); return d->player;
}

void WAbstractBackend::setPlayer(WDeclarativePlayer * parent)
{
    Q_D(WAbstractBackend);

    if (d->player == parent) return;

    d->player = parent;

    emit playerChanged();
}

#endif

//-------------------------------------------------------------------------------------------------

WBackendFilter * WAbstractBackend::filter() const
{
    Q_D(const WAbstractBackend); return d->filter;
}

void WAbstractBackend::setFilter(WBackendFilter * filter)
{
    Q_D(WAbstractBackend);

    if (d->filter == filter) return;

    d->filter = filter;

    emit filterChanged();
}

//-------------------------------------------------------------------------------------------------

void WAbstractBackend::setSource(const QString & url)
{
    loadSource(url);
}

//-------------------------------------------------------------------------------------------------

WAbstractBackend::State WAbstractBackend::state() const
{
    Q_D(const WAbstractBackend); return d->state;
}

WAbstractBackend::StateLoad WAbstractBackend::stateLoad() const
{
    Q_D(const WAbstractBackend); return d->stateLoad;
}

//-------------------------------------------------------------------------------------------------

bool WAbstractBackend::isLoading() const
{
    Q_D(const WAbstractBackend); return (d->stateLoad != StateLoadDefault);
}

//-------------------------------------------------------------------------------------------------

bool WAbstractBackend::isDefault() const
{
    Q_D(const WAbstractBackend); return (d->stateLoad == StateLoadDefault);
}

bool WAbstractBackend::isStarting() const
{
    Q_D(const WAbstractBackend); return (d->stateLoad == StateLoadStarting);
}

bool WAbstractBackend::isResuming() const
{
    Q_D(const WAbstractBackend); return (d->stateLoad == StateLoadResuming);
}

bool WAbstractBackend::isBuffering() const
{
    Q_D(const WAbstractBackend); return (d->stateLoad == StateLoadBuffering);
}

//-------------------------------------------------------------------------------------------------

bool WAbstractBackend::isPlaying() const
{
    Q_D(const WAbstractBackend); return (d->state == StatePlaying);
}

bool WAbstractBackend::isPaused() const
{
    Q_D(const WAbstractBackend); return (d->state == StatePaused);
}

bool WAbstractBackend::isStopped() const
{
    Q_D(const WAbstractBackend); return (d->state == StateStopped);
}

//-------------------------------------------------------------------------------------------------

bool WAbstractBackend::isLive() const
{
    Q_D(const WAbstractBackend); return d->live;
}

//-------------------------------------------------------------------------------------------------

bool WAbstractBackend::hasStarted() const
{
    Q_D(const WAbstractBackend); return d->started;
}

bool WAbstractBackend::hasEnded() const
{
    Q_D(const WAbstractBackend); return d->ended;
}

//-------------------------------------------------------------------------------------------------

int WAbstractBackend::currentTime() const
{
    Q_D(const WAbstractBackend); return d->currentTime;
}

int WAbstractBackend::duration() const
{
    Q_D(const WAbstractBackend); return d->duration;
}

//-------------------------------------------------------------------------------------------------

qreal WAbstractBackend::progress() const
{
    Q_D(const WAbstractBackend); return d->progress;
}

//-------------------------------------------------------------------------------------------------

qreal WAbstractBackend::speed() const
{
    Q_D(const WAbstractBackend); return d->speed;
}

void WAbstractBackend::setSpeed(qreal speed)
{
    Q_D(WAbstractBackend);

    if (d->filter) d->filter->filterSpeed(&speed);

    if (d->speed == speed) return;

    d->speed = speed;

    backendSetSpeed(speed);

    emit speedChanged();
}

//-------------------------------------------------------------------------------------------------

qreal WAbstractBackend::volume() const
{
    Q_D(const WAbstractBackend); return d->volume;
}

void WAbstractBackend::setVolume(qreal volume)
{
    Q_D(WAbstractBackend);

    if (d->filter) d->filter->filterVolume(&volume);

    if (d->volume == volume) return;

    d->volume = volume;

    backendSetVolume(volume);

    emit volumeChanged();
}

//-------------------------------------------------------------------------------------------------

bool WAbstractBackend::repeat() const
{
    Q_D(const WAbstractBackend); return d->repeat;
}

void WAbstractBackend::setRepeat(bool repeat)
{
    Q_D(WAbstractBackend);

    if (d->repeat == repeat) return;

    d->repeat = repeat;

    emit repeatChanged();
}

//-------------------------------------------------------------------------------------------------

WAbstractBackend::Output WAbstractBackend::output() const
{
    Q_D(const WAbstractBackend); return d->output;
}

void WAbstractBackend::setOutput(Output output)
{
    Q_D(WAbstractBackend);

    if (d->filter) d->filter->filterOutput(&output);

    if (d->output == output) return;

    d->output = output;

    backendSetOutput(output);

    emit outputChanged();
}

WAbstractBackend::Quality WAbstractBackend::quality() const
{
    Q_D(const WAbstractBackend); return d->quality;
}

void WAbstractBackend::setQuality(Quality quality)
{
    Q_D(WAbstractBackend);

    if (d->filter) d->filter->filterQuality(&quality);

    if (d->quality == quality) return;

    d->quality = quality;

    backendSetQuality(quality);

    emit qualityChanged();
}

//-------------------------------------------------------------------------------------------------

WAbstractBackend::Output WAbstractBackend::outputActive() const
{
    Q_D(const WAbstractBackend); return d->outputActive;
}

WAbstractBackend::Quality WAbstractBackend::qualityActive() const
{
    Q_D(const WAbstractBackend); return d->qualityActive;
}

//-------------------------------------------------------------------------------------------------

WAbstractBackend::FillMode WAbstractBackend::fillMode() const
{
    Q_D(const WAbstractBackend); return d->fillMode;
}

void WAbstractBackend::setFillMode(FillMode fillMode)
{
    Q_D(WAbstractBackend);

    if (d->filter) d->filter->filterFillMode(&fillMode);

    if (d->fillMode == fillMode) return;

    d->fillMode = fillMode;

    backendSetFillMode(fillMode);

    emit fillModeChanged();
}

//-------------------------------------------------------------------------------------------------

int WAbstractBackend::trackVideo() const
{
    Q_D(const WAbstractBackend); return d->trackVideo;
}

void WAbstractBackend::setTrackVideo(int id)
{
    Q_D(WAbstractBackend);

    if (d->trackVideo == id) return;

    d->trackVideo = id;

    backendSetVideo(id);

    emit trackVideoChanged();
}

int WAbstractBackend::trackAudio() const
{
    Q_D(const WAbstractBackend); return d->trackAudio;
}

void WAbstractBackend::setTrackAudio(int id)
{
    Q_D(WAbstractBackend);

    if (d->trackAudio == id) return;

    d->trackAudio = id;

    backendSetAudio(id);

    emit trackAudioChanged();
}

//-------------------------------------------------------------------------------------------------

int WAbstractBackend::countVideos() const
{
    Q_D(const WAbstractBackend); return d->videos.count();
}

int WAbstractBackend::countAudios() const
{
    Q_D(const WAbstractBackend); return d->audios.count();
}

//-------------------------------------------------------------------------------------------------

bool WAbstractBackend::scanOutput() const
{
    Q_D(const WAbstractBackend); return d->scanOutput;
}

void WAbstractBackend::setScanOutput(bool enabled)
{
    Q_D(WAbstractBackend);

    if (d->filter) d->filter->filterScanOutput(&enabled);

    if (d->scanOutput == enabled) return;

    d->scanOutput = enabled;

    backendSetScanOutput(enabled);

    emit scanOutputChanged();
}

//-------------------------------------------------------------------------------------------------

int WAbstractBackend::currentOutput() const
{
    Q_D(const WAbstractBackend); return d->currentOutput;
}

void WAbstractBackend::setCurrentOutput(int index)
{
    Q_D(WAbstractBackend);

    if (d->filter) d->filter->filterCurrentOutput(&index);

    if (d->currentOutput == index) return;

    d->currentOutput = index;

    if (index >= 0 && index < d->outputs.count())
    {
        WBackendOutput * output = &(d->outputs[index]);

        // NOTE: This might be useful if the renderer gets removed while still being selected. That
        //       way we can restore the currentOutput later.
        d->outputData = *output;

        // NOTE: The first index is the player itself so we substract one.
        backendSetCurrentOutput(output);
    }
    else d->outputData = WBackendOutput();

    d->currentOutputChanged();
}

QString WAbstractBackend::outputName() const
{
    Q_D(const WAbstractBackend); return d->outputData.name;
}

WAbstractBackend::OutputType WAbstractBackend::outputType() const
{
    Q_D(const WAbstractBackend); return d->outputData.type;
}

int WAbstractBackend::countOutputs() const
{
    Q_D(const WAbstractBackend); return d->outputs.count();
}

//-------------------------------------------------------------------------------------------------

QString WAbstractBackend::subtitle() const
{
    Q_D(const WAbstractBackend); return d->subtitle;
}

void WAbstractBackend::setSubtitle(const QString & subtitle)
{
    Q_D(WAbstractBackend);

    if (d->subtitle == subtitle) return;

    d->subtitle = subtitle;

    emit subtitleChanged();
}

//=================================================================================================
// WBackendTrack
//=================================================================================================

WBackendTrack::WBackendTrack(int id, WAbstractBackend::TrackType type, const QString & name)
{
    this->id = id;

    this->type = type;
    this->name = name;
}

WBackendTrack::WBackendTrack()
{
    id = -1;

    type = WAbstractBackend::TrackVideo;
}

//-------------------------------------------------------------------------------------------------
// Operators
//-------------------------------------------------------------------------------------------------

WBackendTrack::WBackendTrack(const WBackendTrack & other)
{
    *this = other;
}

bool WBackendTrack::operator==(const WBackendTrack & other) const
{
    return (id == other.id && name == other.name && type == other.type);
}

WBackendTrack & WBackendTrack::operator=(const WBackendTrack & other)
{
    id = other.id;

    name = other.name;
    type = other.type;

    return *this;
}

//=================================================================================================
// WBackendOutput
//=================================================================================================

WBackendOutput::WBackendOutput(const QString & name, WAbstractBackend::OutputType type)
{
    this->type = type;
    this->name = name;
}

WBackendOutput::WBackendOutput(const QString & name,
                               const QString & label, WAbstractBackend::OutputType type)
{
    this->type = type;

    this->name  = name;
    this->label = label;
}

WBackendOutput::WBackendOutput()
{
    type = WAbstractBackend::OutputDefault;
}

//-------------------------------------------------------------------------------------------------
// Operators
//-------------------------------------------------------------------------------------------------

WBackendOutput::WBackendOutput(const WBackendOutput & other)
{
    *this = other;
}

bool WBackendOutput::operator==(const WBackendOutput & other) const
{
    return (type == other.type && name == other.name && label == other.label);
}

WBackendOutput & WBackendOutput::operator=(const WBackendOutput & other)
{
    type = other.type;

    name  = other.name;
    label = other.label;

    return *this;
}

//=================================================================================================
// WBackendFilter
//=================================================================================================

/* virtual */ void WBackendFilter::filterAddOutput(WBackendOutput *) {}

/* virtual */ void WBackendFilter::filterRemoveOutput(int *) {}

/* virtual */ void WBackendFilter::filterState    (WAbstractBackend::State     *) {}
/* virtual */ void WBackendFilter::filterStateLoad(WAbstractBackend::StateLoad *) {}

/* virtual */ void WBackendFilter::filterLive(bool *) {}

/* virtual */ void WBackendFilter::filterEnded(bool *) {}

/* virtual */ void WBackendFilter::filterCurrentTime(int *) {}
/* virtual */ void WBackendFilter::filterDuration   (int *) {}

/* virtual */ void WBackendFilter::filterProgress(qreal *) {}

/* virtual */ void WBackendFilter::filterOutput      (WAbstractBackend::Output  *) {}
/* virtual */ void WBackendFilter::filterOutputActive(WAbstractBackend::Output  *) {}

/* virtual */ void WBackendFilter::filterQuality      (WAbstractBackend::Quality *) {}
/* virtual */ void WBackendFilter::filterQualityActive(WAbstractBackend::Quality *) {}

/* virtual */ void WBackendFilter::filterSpeed(qreal *) {}

/* virtual */ void WBackendFilter::filterVolume(qreal *) {}

/* virtual */ void WBackendFilter::filterFillMode(WAbstractBackend::FillMode *) {}

/* virtual */ void WBackendFilter::filterScanOutput(bool *) {}

/* virtual */ void WBackendFilter::filterCurrentOutput(int *) {}

#endif // SK_NO_ABSTRACTBACKEND
