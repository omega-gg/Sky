//=================================================================================================
/*
    Copyright (C) 2015-2026 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkMedia.

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

#include "WBackendFfmpeg.h"

#ifndef SK_NO_BACKENDFFMPEG

// Qt includes
#include <QCoreApplication>
#include <QPainter>
#include <QtMath>

// Sk includes
#include <WControllerMedia>
#include <WControllerPlaylist>

// FFmpeg includes
extern "C"
{
#include <libswscale/swscale.h>
}

//-------------------------------------------------------------------------------------------------
// Static variables

// NOTE: Aligned planes let swscale write them at full speed.
static const int BACKENDFFMPEG_ALIGN = 32;

//=================================================================================================
// WBackendFfmpegPrivate
//=================================================================================================

WBackendFfmpegPrivate::WBackendFfmpegPrivate(WBackendFfmpeg * p) : WAbstractBackendPrivate(p) {}

/* virtual */ WBackendFfmpegPrivate::~WBackendFfmpegPrivate()
{
    delete player;

    sws_freeContext(scale);
}

//-------------------------------------------------------------------------------------------------

void WBackendFfmpegPrivate::init()
{
    Q_Q(WBackendFfmpeg);

    player = new WFfmpegPlayer;

    width  = -1;
    height = -1;

    frameWidth  = -1;
    frameHeight = -1;

    scale = NULL;

    frameSwap  = false;
    frameIndex = false;

    started = false;
    active  = false;
    playing = false;

    frameReset   = false;
    frameUpdated = false;
    frameFreeze  = false;

    length = 0;

    streamVideo = -1;
    streamAudio = -1;

    loop = false;

    closestOutput  = WAbstractBackend::OutputNone;
    closestQuality = WAbstractBackend::QualityDefault;

    ratio = Qt::KeepAspectRatio;

    reply = NULL;

    const QMetaObject * meta = q->metaObject();

    method = meta->method(meta->indexOfMethod("onFrameUpdated()"));

    player->setBackend(q, setup, lock, unlock);
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

// NOTE: The I420 frame to RGB, with the BT.709 video range of the video shader.
void WBackendFfmpegPrivate::convertFrame()
{
    if (frameReset || frameWidth == -1) return;

    scale = sws_getCachedContext(scale, frameWidth, frameHeight, AV_PIX_FMT_YUV420P,
                                 frameWidth, frameHeight, AV_PIX_FMT_BGRA, SWS_POINT,
                                 NULL, NULL, NULL);

    if (scale == NULL) return;

    const int * coefficients = sws_getCoefficients(SWS_CS_ITU709);

    sws_setColorspaceDetails(scale, coefficients, 0, coefficients, 1, 0, 1 << 16, 1 << 16);

    const uchar * planes[3] = { textures[0].bits, textures[1].bits, textures[2].bits };

    int pitches[3] = { textures[0].pitch, textures[1].pitch, textures[2].pitch };

    uchar * bits = frameSoftware.bits();

    int pitch = frameSoftware.bytesPerLine();

    sws_scale(scale, planes, pitches, 0, frameHeight, &bits, &pitch);
}

//-------------------------------------------------------------------------------------------------

void WBackendFfmpegPrivate::loadSources(bool play)
{
    if (reply) return;

    Q_Q(WBackendFfmpeg);

    reply = wControllerMedia->getMedia(source, q, q->getMode());

    if (reply == NULL) return;

    if (reply->isLoaded())
    {
        applySources(reply, play);

        delete reply;

        reply = NULL;
    }
    else QObject::connect(reply, SIGNAL(loaded(WMediaReply *)), q, SLOT(onLoaded()));
}

void WBackendFfmpegPrivate::applySources(const WMediaReply * reply, bool play)
{
    Q_Q(WBackendFfmpeg);

    medias = reply->medias();
    audios = reply->audios();

    options = reply->options();

    q->setVbml(reply->isVbml());

    loop = (reply->typeSource() == WTrack::Hub);

    if (applyQuality(quality))
    {
        currentMedia = medias.value(closestQuality);
        currentAudio = audios.value(closestQuality);

        closestOutput = getOutput(output);

        player->setOutput(closestOutput);

        if (play) playMedia();
    }
    else
    {
        closestOutput = WAbstractBackend::OutputNone;

        q->stop();
    }

    emit q->loaded();
}

//-------------------------------------------------------------------------------------------------

bool WBackendFfmpegPrivate::applyQuality(WAbstractBackend::Quality quality)
{
    if (medias.value(quality).isEmpty() == false)
    {
        closestQuality = quality;

        return true;
    }

    for (int i = quality - 1; i >= WAbstractBackend::QualityDefault; i--)
    {
        WAbstractBackend::Quality closestQuality = static_cast<WAbstractBackend::Quality> (i);

        if (medias.value(closestQuality).isEmpty()) continue;

        this->closestQuality = closestQuality;

        return true;
    }

    for (int i = quality + 1; i <= WAbstractBackend::Quality2160; i++)
    {
        WAbstractBackend::Quality closestQuality = static_cast<WAbstractBackend::Quality> (i);

        if (medias.value(closestQuality).isEmpty()) continue;

        this->closestQuality = closestQuality;

        return true;
    }

    return false;
}

//-------------------------------------------------------------------------------------------------

// NOTE: A paused player shows the frame at this time, and stays there.
// NOTE: A live source plays from its live point, as VLC's.
void WBackendFfmpegPrivate::playAt(int time)
{
    if (live)
    {
         player->play();
    }
    else player->play(time);

    if (state == WAbstractBackend::StatePaused) player->pause();
}

void WBackendFfmpegPrivate::playMedia()
{
    Q_Q(WBackendFfmpeg);

    player->setSource(currentMedia, currentAudio, options, loop);

    q->setOutputActive (closestOutput);
    q->setQualityActive(closestQuality);

    if (currentTime == -1)
    {
        playAt(0);

        q->setStateLoad(WAbstractBackend::StateLoadStarting);
    }
    else
    {
        playAt(currentTime);

        q->setStateLoad(WAbstractBackend::StateLoadResuming);
    }
}

// NOTE: The source plays again from the current time, its last frame stays on screen meanwhile.
void WBackendFfmpegPrivate::restart()
{
    Q_Q(WBackendFfmpeg);

    if (q->hasStarted() == false) return;

    started = false;

    frameFreeze = true;

    onFrameUpdated();

    player->setSource(currentMedia, currentAudio, options, loop);

    if (state == WAbstractBackend::StatePlaying || state == WAbstractBackend::StatePaused)
    {
        playAt(currentTime);
    }
}

void WBackendFfmpegPrivate::reload()
{
    Q_Q(WBackendFfmpeg);

    if (currentMedia.isEmpty()) return;

    clearReply();

    if (q->hasStarted())
    {
        q->backendStop();

        // NOTE: We clear sources to check their validity when we resume playback.
        clearSources();

        loadSources(q->isPlaying());

        updateLoading();
    }
    // NOTE: We clear sources to check their validity when we resume playback.
    else clearSources();
}

//-------------------------------------------------------------------------------------------------

void WBackendFfmpegPrivate::updateTargetRect()
{
    if (frameWidth == -1) return;

    QSizeF frameSize = QSizeF(frameWidth, frameHeight);

    frameSize.scale(size, ratio);

    qreal width  = frameSize.width ();
    qreal height = frameSize.height();

    qreal x = (size.width () - width)  / 2;
    qreal y = (size.height() - height) / 2;

    targetRect = QRectF(x, y, width, height);
}

void WBackendFfmpegPrivate::updateLoading()
{
    Q_Q(WBackendFfmpeg);

    if (currentTime == -1)
    {
         q->setStateLoad(WAbstractBackend::StateLoadStarting);
    }
    else q->setStateLoad(WAbstractBackend::StateLoadResuming);
}

//-------------------------------------------------------------------------------------------------

void WBackendFfmpegPrivate::clearPlayer()
{
    playing = false;

    length = 0;

    if (started)
    {
        started = false;
        active  = false;

        frameFreeze = false;

        onFrameUpdated();
    }
    else if (frameFreeze)
    {
        active = false;

        frameFreeze = false;

        onFrameUpdated();
    }
}

void WBackendFfmpegPrivate::clearActive()
{
    Q_Q(WBackendFfmpeg);

    q->setOutputActive (WAbstractBackend::OutputNone);
    q->setQualityActive(WAbstractBackend::QualityDefault);
}

//-------------------------------------------------------------------------------------------------

void WBackendFfmpegPrivate::clearReply()
{
    if (reply == NULL) return;

    delete reply;

    reply = NULL;
}

void WBackendFfmpegPrivate::clearMedia()
{
    clearReply();

    currentMedia = QString();
    currentAudio = QString();
}

void WBackendFfmpegPrivate::clearSources()
{
    currentMedia = QString();
    currentAudio = QString();

    medias.clear();
    audios.clear();
}

//-------------------------------------------------------------------------------------------------

WAbstractBackend::Output WBackendFfmpegPrivate::getOutput(WAbstractBackend::Output output)
{
    if (output == WAbstractBackend::OutputNone)
    {
        return WAbstractBackend::OutputNone;
    }

    if (output == WAbstractBackend::OutputAudio
        ||
        (currentAudio.isEmpty() && WControllerPlaylist::urlIsAudio(currentMedia)))
    {
         return WAbstractBackend::OutputAudio;
    }
    else return output;
}

//-------------------------------------------------------------------------------------------------
// Static private functions
//-------------------------------------------------------------------------------------------------

/* static */ bool WBackendFfmpegPrivate::setup(void * data, int * width, int * height,
                                                            int * pitches, int * lines)
{
    WBackendFfmpegPrivate * d = static_cast<WBackendFfmpeg *> (data)->d_func();

    int frameWidth  = *width;
    int frameHeight = *height;

    if (frameWidth == 0 || frameHeight == 0) return false;

    pitches[0] = frameWidth;
    pitches[1] = (frameWidth + 1) / 2;

    while (pitches[0] % BACKENDFFMPEG_ALIGN) pitches[0]++;
    while (pitches[1] % BACKENDFFMPEG_ALIGN) pitches[1]++;

    pitches[2] = pitches[1];

    lines[0] = frameHeight;
    lines[1] = (frameHeight + 1) / 2;

    while (lines[0] % 4) lines[0]++;
    while (lines[1] % 4) lines[1]++;

    lines[2] = lines[1];

    int cursorU = pitches[0] * lines[0];

    int cursorV = cursorU + pitches[1] * lines[1];

    int size = cursorV + pitches[2] * lines[2];

    // NOTE: We make sure we're not writing a new frame while applying it on a texture.
    d->mutex.lock();

    d->width  = frameWidth;
    d->height = frameHeight;

    // NOTE: We need to swap frame(s) in case we're still reading bits from the rendering thread.
    if (d->frameSwap)
    {
        d->frames[0] = QByteArray(size, Qt::Uninitialized);
        d->frames[1] = QByteArray(size, Qt::Uninitialized);

        applyFrames(d, d->frames[0], d->frames[1], cursorU, cursorV);
    }
    else
    {
        d->frames[2] = QByteArray(size, Qt::Uninitialized);
        d->frames[3] = QByteArray(size, Qt::Uninitialized);

        applyFrames(d, d->frames[2], d->frames[3], cursorU, cursorV);
    }

    d->mutex.unlock();

    d->frameIndex = false;

    QCoreApplication::postEvent(d->q_func(),
                                new WBackendFfmpegEventSetup(frameWidth, frameHeight,
                                                             pitches[0], pitches[1], pitches[2],
                                                             cursorU, cursorV));

    return true;
}

//-------------------------------------------------------------------------------------------------

/* static */ void WBackendFfmpegPrivate::lock(void * data, uchar ** planes)
{
    WBackendFfmpegPrivate * d = static_cast<WBackendFfmpeg *> (data)->d_func();

    d->frameIndex = !(d->frameIndex);

    if (d->frameIndex)
    {
        planes[0] = d->textures[0].bitsA;
        planes[1] = d->textures[1].bitsA;
        planes[2] = d->textures[2].bitsA;
    }
    else
    {
        planes[0] = d->textures[0].bitsB;
        planes[1] = d->textures[1].bitsB;
        planes[2] = d->textures[2].bitsB;
    }
}

/* static */ void WBackendFfmpegPrivate::unlock(void * data)
{
    WBackendFfmpeg * backend = static_cast<WBackendFfmpeg *> (data);

    WBackendFfmpegPrivate * d = backend->d_func();

    if (d->frameIndex)
    {
        d->textures[0].bits = d->textures[0].bitsA;
        d->textures[1].bits = d->textures[1].bitsA;
        d->textures[2].bits = d->textures[2].bitsA;
    }
    else
    {
        d->textures[0].bits = d->textures[0].bitsB;
        d->textures[1].bits = d->textures[1].bitsB;
        d->textures[2].bits = d->textures[2].bitsB;
    }

    d->method.invoke(backend);
}

//-------------------------------------------------------------------------------------------------

/* static */ void WBackendFfmpegPrivate::applyFrames(WBackendFfmpegPrivate * d,
                                                     QByteArray & frameA, QByteArray & frameB,
                                                     int cursorU, int cursorV)
{
    uchar * bitsA = reinterpret_cast<uchar *> (frameA.data());
    uchar * bitsB = reinterpret_cast<uchar *> (frameB.data());

    d->textures[0].bitsA = bitsA;
    d->textures[1].bitsA = bitsA + cursorU;
    d->textures[2].bitsA = bitsA + cursorV;

    d->textures[0].bitsB = bitsB;
    d->textures[1].bitsB = bitsB + cursorU;
    d->textures[2].bitsB = bitsB + cursorV;

    d->textures[0].bits = d->textures[0].bitsA;
    d->textures[1].bits = d->textures[1].bitsA;
    d->textures[2].bits = d->textures[2].bitsA;
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WBackendFfmpegPrivate::onLoaded()
{
    Q_Q(WBackendFfmpeg);

    if (reply->hasError())
    {
        q->stop();

        emit q->loaded();
    }
    else applySources(reply, q->isPlaying());

    reply->deleteLater();

    reply = NULL;
}

void WBackendFfmpegPrivate::onFrameUpdated()
{
    Q_Q(WBackendFfmpeg);

    frameUpdated = true;

    q->applyFrame();
}

//=================================================================================================
// WBackendFfmpeg
//=================================================================================================

WBackendFfmpeg::WBackendFfmpeg(QObject * parent)
    : WAbstractBackend(new WBackendFfmpegPrivate(this), parent)
{
    Q_D(WBackendFfmpeg); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WBackendFfmpeg::setProxy(const QString & host,
                                                int             port, const QString & password)
{
    Q_D(WBackendFfmpeg);

    d->clearReply();

    d->player->setProxy(host + ':' + QString::number(port), password);

    d->clearSources();
}

/* Q_INVOKABLE */ void WBackendFfmpeg::clearProxy()
{
    Q_D(WBackendFfmpeg);

    d->clearReply();

    d->player->clearProxy();

    d->clearSources();
}

//-------------------------------------------------------------------------------------------------
// Protected WAbstractBackend implementation
//-------------------------------------------------------------------------------------------------

#ifndef SK_NO_QML

/* virtual */ WBackendNode * WBackendFfmpeg::backendCreateNode() const
{
    return new WBackendYuvNode;
}

#endif

/* virtual */ bool WBackendFfmpeg::backendSetSource(const QString & url,
                                                    const WMediaReply * reply)
{
    Q_D(WBackendFfmpeg);

    d->clearMedia();

    if (url.isEmpty())
    {
        d->clearPlayer();

        d->player->stop();

        d->clearActive();
    }
    else if (isPlaying())
    {
        d->clearPlayer();

        d->player->stop();

        d->clearActive();

        if (reply)
        {
            d->applySources(reply, true);
        }
        else d->loadSources(true);

        d->updateLoading();
    }
    else if (reply)
    {
        d->applySources(reply, false);
    }

    return true;
}

//-------------------------------------------------------------------------------------------------

/* virtual */ bool WBackendFfmpeg::backendPlay()
{
    Q_D(WBackendFfmpeg);

    if (isPaused() == false)
    {
        if (d->currentMedia.isEmpty())
        {
            d->loadSources(true);

            d->updateLoading();
        }
        else d->playMedia();
    }
    else if (d->frameFreeze && d->currentTime != -1)
    {
         d->player->play(d->currentTime);
    }
    else d->player->play();

    return true;
}

// NOTE: The player pauses at once, even while it loads: it then shows its first frame and stays.
/* virtual */ bool WBackendFfmpeg::backendPause()
{
    Q_D(WBackendFfmpeg);

    d->player->pause();

    return true;
}

/* virtual */ bool WBackendFfmpeg::backendStop()
{
    Q_D(WBackendFfmpeg);

    d->clearPlayer();

    d->player->stop();

    d->clearActive();

    return true;
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WBackendFfmpeg::backendSetVolume(qreal volume)
{
    Q_D(WBackendFfmpeg);

    // NOTE: As VLC, we don't want the volume to lower too quickly for human ears.
    d->player->setVolume(qRound(qPow(volume, 0.6) * 100));
}

//-------------------------------------------------------------------------------------------------

/* virtual */ bool WBackendFfmpeg::backendDelete()
{
    Q_D(WBackendFfmpeg);

    d->clearMedia ();
    d->clearPlayer();
    d->clearActive();

    d->player->deletePlayer();

    return false;
}

//-------------------------------------------------------------------------------------------------
// Protected WAbstractBackend reimplementation
//-------------------------------------------------------------------------------------------------

// NOTE: The player keeps the last seek only, so seeks never pile up.
/* virtual */ void WBackendFfmpeg::backendSeek(int msec)
{
    Q_D(WBackendFfmpeg);

    d->player->seek(msec);

    if (d->stateLoad != StateLoadDefault) return;

    setStateLoad(StateLoadBuffering);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WBackendFfmpeg::backendSetSpeed(qreal speed)
{
    Q_D(WBackendFfmpeg);

    d->player->setSpeed(speed);
}

//-------------------------------------------------------------------------------------------------

// NOTE: Another output plays the source again from the current time, as VLC does.
/* virtual */ void WBackendFfmpeg::backendSetOutput(Output output)
{
    Q_D(WBackendFfmpeg);

    if (d->currentMedia.isEmpty()) return;

    output = d->getOutput(output);

    if (d->closestOutput == output) return;

    d->closestOutput = output;

    d->player->setOutput(output);

    setOutputActive(output);

    d->restart();
}

/* virtual */ void WBackendFfmpeg::backendSetQuality(Quality quality)
{
    Q_D(WBackendFfmpeg);

    if (d->closestQuality != quality && d->applyQuality(quality))
    {
        QString media = d->medias.value(d->closestQuality);

        if (d->currentMedia != media)
        {
            d->currentMedia = media;

            d->currentAudio = d->audios.value(d->closestQuality);
        }

        setQualityActive(d->closestQuality);
    }

    d->restart();
}

/* virtual */ void WBackendFfmpeg::backendSetSourceMode(SourceMode)
{
    Q_D(WBackendFfmpeg); d->reload();
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WBackendFfmpeg::backendSetFillMode(FillMode fillMode)
{
    Q_D(WBackendFfmpeg);

    d->ratio = static_cast<Qt::AspectRatioMode> (fillMode);

    d->updateTargetRect();
}

// NOTE: The node adjusts the frames, the current one is sent again so a paused video shows it.
/* virtual */ void WBackendFfmpeg::backendSetAdjust(const WBackendAdjust &)
{
    Q_D(WBackendFfmpeg);

    if (d->started == false) return;

    d->onFrameUpdated();
}

// NOTE: A track is a stream of the source: it plays again from the current time with that stream,
//       its last frame stays on screen meanwhile.
// NOTE: Each opening reports its tracks and the picked ones come back here. We restart only for
//       a stream the player does not play, and once for a pick the source does not have.
/* virtual */ void WBackendFfmpeg::backendSetVideo(int id)
{
    Q_D(WBackendFfmpeg);

    if (d->player->setTracks(id, trackAudio()) && id != d->streamVideo) d->restart();
}

/* virtual */ void WBackendFfmpeg::backendSetAudio(int id)
{
    Q_D(WBackendFfmpeg);

    if (d->player->setTracks(trackVideo(), id) && id != d->streamAudio) d->restart();
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WBackendFfmpeg::backendSetSize(const QSizeF &)
{
    Q_D(WBackendFfmpeg);

    d->updateTargetRect();
}

//-------------------------------------------------------------------------------------------------

#ifndef SK_NO_QML

/* virtual */ void WBackendFfmpeg::backendSynchronize(WBackendFrame * frame)
{
    Q_D(WBackendFfmpeg);

    // NOTE: FFmpeg does not adjust the frames, the node does.
    frame->adjust = adjust();

    d->player->setSwap(frame->swapTime, frame->swapPeriod);

    if (d->active == false)
    {
        frame->state = WAbstractBackend::FrameClear;
    }
    else if (d->frameUpdated && d->frameFreeze == false)
    {
        if (d->frameReset)
        {
            // NOTE: If the frame has already changed in 'setup' we wait for the next bits.
            if (d->width != d->frameWidth || d->height != d->frameHeight) return;

            d->frameUpdated = false;

            d->frameReset = false;

            WBackendTexture * textures = frame->textures;

            d->mutex.lock();

            for (int i = 0; i < 3; i++)
            {
                WBackendTexture       * textureA = &(textures[i]);
                WBackendFfmpegTexture * textureB = &(d->textures[i]);

                textureA->size = QSize(textureB->width, textureB->height);

                textureA->bits = textureB->bits;

                textureA->length = textureB->length;
                textureA->pitch  = textureB->pitch;
            }

            d->frameSwap = !(d->frameSwap);

#ifdef SK_SOFTWARE
            d->convertFrame();
#endif

            d->mutex.unlock();

            frame->state = WAbstractBackend::FrameReset;
        }
        else
        {
            d->frameUpdated = false;

            WBackendTexture * textures = frame->textures;

            textures[0].bits = d->textures[0].bits;
            textures[1].bits = d->textures[1].bits;
            textures[2].bits = d->textures[2].bits;

#ifdef SK_SOFTWARE
            d->convertFrame();
#endif

            // NOTE: When frame reset is pending we keep it that way. Otherwise we won't reset the
            //       texture properly in 'updatePaintNode'.
            if (frame->state != WAbstractBackend::FrameReset)
            {
                frame->state = WAbstractBackend::FrameUpdate;
            }
        }
    }
    else frame->state = WAbstractBackend::FrameDefault;
}

#endif

//-------------------------------------------------------------------------------------------------

/* virtual */ void WBackendFfmpeg::backendDrawFrame(QPainter * painter, const QRect &)
{
    Q_D(WBackendFfmpeg);

    if (d->active == false) return;

    bool smooth = painter->testRenderHint(QPainter::SmoothPixmapTransform);

    painter->setRenderHint(QPainter::SmoothPixmapTransform);

    painter->drawImage(d->targetRect, d->frameSoftware);

    painter->setRenderHint(QPainter::SmoothPixmapTransform, smooth);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WBackendFfmpeg::backendUpdateFrame()
{
    Q_D(WBackendFfmpeg);

    d->convertFrame();
}

/* virtual */ void WBackendFfmpeg::backendNextFrame()
{
    Q_D(WBackendFfmpeg);

    d->player->nextFrame();
}

/* virtual */ QImage WBackendFfmpeg::backendGetFrame() const
{
    Q_D(const WBackendFfmpeg);

    if (d->frameWidth == -1)
    {
        return QImage();
    }
    else return d->frameSoftware;
}

/* virtual */ QImage WBackendFfmpeg::backendGetFrameGray() const
{
    Q_D(const WBackendFfmpeg);

    if (d->frameWidth == -1) return QImage();

    const WBackendFfmpegTexture & texture = d->textures[0];

    return QImage(texture.bits, texture.width, texture.height, texture.pitch,
                  QImage::Format_Grayscale8);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ QRectF WBackendFfmpeg::backendRect() const
{
    Q_D(const WBackendFfmpeg);

    return d->targetRect;
}

//-------------------------------------------------------------------------------------------------
// Events
//-------------------------------------------------------------------------------------------------

/* virtual */ bool WBackendFfmpeg::event(QEvent * event)
{
    QEvent::Type type = event->type();

    if (type == static_cast<QEvent::Type> (WBackendFfmpegPrivate::EventSetup))
    {
        Q_D(WBackendFfmpeg);

        WBackendFfmpegEventSetup * setup = static_cast<WBackendFfmpegEventSetup *> (event);

        int width  = setup->width;
        int height = setup->height;

        d->frameWidth  = width;
        d->frameHeight = height;

        d->frameSoftware = QImage(width, height, QImage::Format_RGB32);

        d->textures[0].width  = width;
        d->textures[0].height = height;

        d->textures[1].width  = (width  + 1) / 2;
        d->textures[1].height = (height + 1) / 2;

        d->textures[2].width  = d->textures[1].width;
        d->textures[2].height = d->textures[1].height;

        d->textures[0].pitch = setup->pitchY;
        d->textures[1].pitch = setup->pitchU;
        d->textures[2].pitch = setup->pitchV;

        d->textures[0].length = setup->cursorU;
        d->textures[1].length = setup->cursorV - setup->cursorU;
        d->textures[2].length = d->textures[1].length;

        d->updateTargetRect();

        d->frameReset = true;

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WFfmpegPlayer::EventPlaying))
    {
        Q_D(WBackendFfmpeg);

        d->playing = true;

        if (d->state == StateStopped) d->player->pause();

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WFfmpegPlayer::EventBuffering))
    {
        Q_D(WBackendFfmpeg);

        // NOTE: When playing a hub, we want to avoid buffering while restarting the video.
        if (d->loop) return true;

        int progress = static_cast<WFfmpegPlayerEvent *> (event)->value.toInt();

        if (progress == 100)
        {
            if (d->stateLoad == StateLoadBuffering)
            {
                setStateLoad(StateLoadDefault);
            }
        }
        else if (d->stateLoad == StateLoadDefault)
        {
            setStateLoad(StateLoadBuffering);
        }

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WFfmpegPlayer::EventLengthChanged))
    {
        Q_D(WBackendFfmpeg);

        int length = static_cast<WFfmpegPlayerEvent *> (event)->value.toInt();

        d->length = length;

        if (length) setDuration(length);

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WFfmpegPlayer::EventTimeChanged))
    {
        Q_D(WBackendFfmpeg);

        if (d->playing == false) return true;

        // NOTE: The frame of this time comes right after it.
        if (d->started == false)
        {
            d->started = true;
            d->active  = true;

            d->frameFreeze = false;
        }

        setStateLoad(StateLoadDefault);

        int time = static_cast<WFfmpegPlayerEvent *> (event)->value.toInt();

        setCurrentTime(time);

        if (time > d->length && d->live == false) setLive(true);

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WFfmpegPlayer::EventEndReached))
    {
        Q_D(WBackendFfmpeg);

        if (d->started)
        {
            // NOTE: We repeat at the lowest level possible, instead of relying on the WPlayer.
            if (d->repeat)
            {
                d->player->seek(0);

                d->player->play();

                return true;
            }

            d->started = false;
            d->active  = false;
            d->playing = false;

            d->frameFreeze = false;

            setEnded(true);
        }
        else stop();

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WFfmpegPlayer::EventTracks))
    {
        Q_D(WBackendFfmpeg);

        WFfmpegTracksEvent * eventTracks = static_cast<WFfmpegTracksEvent *> (event);

        d->streamVideo = eventTracks->trackVideo;
        d->streamAudio = eventTracks->trackAudio;

        applyTracks(eventTracks->tracks, eventTracks->trackVideo, eventTracks->trackAudio);

        setFrameRate(eventTracks->frameRate);

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WFfmpegPlayer::EventError))
    {
        Q_D(WBackendFfmpeg);

        stopError("FFmpeg player error");

        // NOTE: We clear sources to check their validity when we resume playback.
        d->clearSources();

        wControllerMedia->clearMedia(d->source);

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WFfmpegPlayer::EventDelete))
    {
        deleteNow();

        return true;
    }
    else return QObject::event(event);
}

#endif // SK_NO_BACKENDFFMPEG
