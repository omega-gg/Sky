//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkMedia module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#include "WBackendVlc.h"

#ifndef SK_NO_BACKENDVLC

// Qt includes
#include <QCoreApplication>
#include <QGraphicsItem>
#include <QImage>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

// Vlc includes
#include <vlc/vlc.h>

// Sk includes
#include <WControllerMedia>
#include <WControllerDownload>
#include <WControllerPlaylist>

// 3rdparty includes
#include <3rdparty/vlc/mmxRgb.h>
#ifndef Q_OS_MAC
#include <3rdparty/opengl/glext.h>
#endif

// Mac includes
#ifdef Q_OS_MAC
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#endif

// Linux includes
#ifdef Q_OS_LINUX
#include <GL/glx.h>
#endif

// Private includes
#include <private/WVlcPlayer_p>

//=================================================================================================
// Defines

#define RED_MARGIN    178
#define GREEN_MARGIN  135
#define BLUE_MARGIN   224
#define RED_OFFSET   1501
#define GREEN_OFFSET  135
#define BLUE_OFFSET   818

//-------------------------------------------------------------------------------------------------
// Opengl

#ifndef Q_OS_MAC
PFNGLGENPROGRAMSARBPROC              pglGenProgramsARB              = 0;
PFNGLBINDPROGRAMARBPROC              pglBindProgramARB              = 0;
PFNGLPROGRAMSTRINGARBPROC            pglProgramStringARB            = 0;
PFNGLDELETEPROGRAMSARBPROC           pglDeleteProgramsARB           = 0;
PFNGLPROGRAMLOCALPARAMETER4FVARBPROC pglProgramLocalParameter4fvARB = 0;
PFNGLACTIVETEXTUREARBPROC            pglActiveTextureARB            = 0;
PFNGLMULTITEXCOORD2FARBPROC          pglMultiTexCoord2fARB          = 0;

#define glGenProgramsARB              pglGenProgramsARB
#define glBindProgramARB              pglBindProgramARB
#define glProgramStringARB            pglProgramStringARB
#define glDeleteProgramsARB           pglDeleteProgramsARB
#define glProgramLocalParameter4fvARB pglProgramLocalParameter4fvARB
#define glActiveTextureARB            pglActiveTextureARB
#define glMultiTexCoord2fARB          pglMultiTexCoord2fARB
#endif

//-------------------------------------------------------------------------------------------------
// YUV pixel conversions

#define SHIFT 20

#define U_GREEN_COEF ((int) (-0.391 * (1 << SHIFT) / 1.164))
#define U_BLUE_COEF  ((int) ( 2.018 * (1 << SHIFT) / 1.164))
#define V_RED_COEF   ((int) ( 1.596 * (1 << SHIFT) / 1.164))
#define V_GREEN_COEF ((int) (-0.813 * (1 << SHIFT) / 1.164))

#define W_CONVERT_YUV_PIXEL                             \
                                                        \
    u = *(p_u++);                                       \
    v = *(p_v++);                                       \
                                                        \
    r = (V_RED_COEF   * v)                    >> SHIFT; \
    g = (U_GREEN_COEF * u + V_GREEN_COEF * v) >> SHIFT; \
    b = (U_BLUE_COEF  * u)                    >> SHIFT; \
                                                        \
    W_CONVERT_Y_PIXEL;                                  \

#define W_CONVERT_Y_PIXEL                                                                  \
                                                                                           \
    yBase = tableRgb + *(p_y++);                                                           \
                                                                                           \
    *bits++ = yBase[RED_OFFSET   - ((V_RED_COEF                    * 128) >> SHIFT) + r] | \
              yBase[GREEN_OFFSET - (((U_GREEN_COEF + V_GREEN_COEF) * 128) >> SHIFT) + g] | \
              yBase[BLUE_OFFSET  - ((U_BLUE_COEF                   * 128) >> SHIFT) + b];  \

//-------------------------------------------------------------------------------------------------

#define W_SCISSOR                                                                       \
{                                                                                       \
    glEnable(GL_SCISSOR_TEST);                                                          \
                                                                                        \
    const QTransform & transform = painter->transform();                                \
                                                                                        \
    QRectF rect = painter->clipBoundingRect();                                          \
                                                                                        \
    glScissor(transform.dx() + rect.x(),                                                \
              painter->viewport().height() - transform.dy() - rect.y() - rect.height(), \
              rect.width(), rect.height());                                             \
}                                                                                       \

#define W_DRAW_BACK_AND_FRAME       \
{                                   \
    glColor4f(0, 0, 0, d->opacity); \
                                    \
    glBegin(GL_POLYGON);            \
                                    \
    glVertex2i(0,          0);      \
    glVertex2i(0,     height);      \
    glVertex2i(width, height);      \
    glVertex2i(width,      0);      \
                                    \
    glEnd();                        \
                                    \
    W_DRAW_FRAME;                   \
}                                   \

#define W_DRAW_FRAME                                     \
{                                                        \
    glEnable(GL_FRAGMENT_PROGRAM_ARB);                   \
                                                         \
    glBegin(GL_POLYGON);                                 \
                                                         \
    glTexCoord2i(0, 0);                                  \
                                                         \
    for (int i = 0; i < 3; i++)                          \
    {                                                    \
        glMultiTexCoord2fARB(GL_TEXTURE0_ARB + i, 0, 0); \
    }                                                    \
                                                         \
    glVertex2f(d->targetX, d->targetY);                  \
                                                         \
    glTexCoord2i(0, 1);                                  \
                                                         \
    for (int i = 0; i < 3; i++)                          \
    {                                                    \
        glMultiTexCoord2fARB(GL_TEXTURE0_ARB + i, 0, 1); \
    }                                                    \
                                                         \
    glVertex2f(d->targetX, d->targetHeight);             \
                                                         \
    glTexCoord2i(1, 1);                                  \
                                                         \
    for (int i = 0; i < 3; i++)                          \
    {                                                    \
        glMultiTexCoord2fARB(GL_TEXTURE0_ARB + i, 1, 1); \
    }                                                    \
                                                         \
    glVertex2f(d->targetWidth, d->targetHeight);         \
                                                         \
    glTexCoord2i(1, 0);                                  \
                                                         \
    for (int i = 0; i < 3; i++)                          \
    {                                                    \
        glMultiTexCoord2fARB(GL_TEXTURE0_ARB + i, 1, 0); \
    }                                                    \
                                                         \
    glVertex2f(d->targetWidth, d->targetY);              \
                                                         \
    glEnd();                                             \
                                                         \
    glDisable(GL_FRAGMENT_PROGRAM_ARB);                  \
}                                                        \

//-------------------------------------------------------------------------------------------------
// Static variables

static const int PLAYER_FORMAT          = GL_LUMINANCE;
static const int PLAYER_FORMAT_INTERNAL = GL_LUMINANCE;

static const int PLAYER_DATA_TYPE = GL_UNSIGNED_BYTE;

static const int PLAYER_MAX_WIDTH  = 5760;
static const int PLAYER_MAX_HEIGHT = 3240;

//-------------------------------------------------------------------------------------------------

static GLuint shaderId    = 0;
static int    shaderCount = 0;

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

WBackendVlcPrivate::WBackendVlcPrivate(WBackendVlc * p) : WAbstractBackendPrivate(p) {}

/* virtual */ WBackendVlcPrivate::~WBackendVlcPrivate()
{
    delete player;

    deleteShader();

    if (textureIds[0]) glDeleteTextures(3, textureIds);
}

//-------------------------------------------------------------------------------------------------

void WBackendVlcPrivate::init()
{
    Q_Q(WBackendVlc);

    player = wControllerMedia->createVlcPlayer();

    frameWidth  = -1;
    frameHeight = -1;

    frameIndex = false;

    targetX      = 0.f;
    targetY      = 0.f;
    targetWidth  = 0.f;
    targetHeight = 0.f;

    shader = false;

    textureIds[0] = 0;

    opacity = 1.f;

    started = false;
    active  = false;
    playing = false;

    frameReset   = false;
    frameUpdated = false;
    frameFreeze  = false;

    volume = 100;
    mute   = false;

    closestOutput  = WAbstractBackend::OutputInvalid;
    closestQuality = WAbstractBackend::QualityInvalid;

    ratio = Qt::KeepAspectRatio;

    reply = NULL;

    const QMetaObject * meta = q->metaObject();

    method = meta->method(meta->indexOfMethod("onFrameUpdated()"));

    populateTableRgb();

    player->setBackend(q, setup, NULL, lock, unlock, NULL);

    QObject::connect(player, SIGNAL(optionsChanged     ()), q, SIGNAL(optionsChanged     ()));
    QObject::connect(player, SIGNAL(networkCacheChanged()), q, SIGNAL(networkCacheChanged()));
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WBackendVlcPrivate::populateTableRgb()
{
    uchar gamma[256];

    for (int i = 0; i < 256; i++)
    {
        gamma[i] = i;
    }

    for (int i = 0; i < RED_MARGIN; i++)
    {
        tableRgb[RED_OFFSET - RED_MARGIN + i] = qRgb(gamma[0],   0, 0);
        tableRgb[RED_OFFSET + 256        + i] = qRgb(gamma[255], 0, 0);
    }

    for (int i = 0; i < GREEN_MARGIN; i++)
    {
        tableRgb[GREEN_OFFSET - GREEN_MARGIN + i] = qRgb(0, gamma[0],   0);
        tableRgb[GREEN_OFFSET + 256          + i] = qRgb(0, gamma[255], 0);
    }

    for (int i = 0; i < BLUE_MARGIN; i++)
    {
        tableRgb[BLUE_OFFSET - BLUE_MARGIN + i] = qRgb(0, 0, gamma[0]);
        tableRgb[BLUE_OFFSET + BLUE_MARGIN + i] = qRgb(0, 0, gamma[255]);
    }

    for (int i = 0; i < 256; i++)
    {
        tableRgb[RED_OFFSET   + i] = qRgb(gamma[i], 0, 0);
        tableRgb[GREEN_OFFSET + i] = qRgb(0, gamma[i], 0);
        tableRgb[BLUE_OFFSET  + i] = qRgb(0, 0, gamma[i]);
    }
}

//-------------------------------------------------------------------------------------------------

void WBackendVlcPrivate::initShader()
{
    if (shader) return;

    if (shaderCount)
    {
        shader = true;

        shaderCount++;

        return;
    }

#if defined(Q_OS_WIN)
    glGenProgramsARB   = (PFNGLGENPROGRAMSARBPROC)   wglGetProcAddress("glGenProgramsARB");
    glBindProgramARB   = (PFNGLBINDPROGRAMARBPROC)   wglGetProcAddress("glBindProgramARB");
    glProgramStringARB = (PFNGLPROGRAMSTRINGARBPROC) wglGetProcAddress("glProgramStringARB");

    glDeleteProgramsARB = (PFNGLDELETEPROGRAMSARBPROC) wglGetProcAddress("glDeleteProgramsARB");

    glProgramLocalParameter4fvARB = (PFNGLPROGRAMLOCALPARAMETER4FVARBPROC)
                                    wglGetProcAddress("glProgramLocalParameter4fvARB");

    glActiveTextureARB   = (PFNGLACTIVETEXTUREARBPROC)   wglGetProcAddress("glActiveTextureARB");
    glMultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC) wglGetProcAddress("glMultiTexCoord2fARB");
#elif defined(Q_OS_LINUX)
    glGenProgramsARB = (PFNGLGENPROGRAMSARBPROC) glXGetProcAddress((GLubyte *) "glGenProgramsARB");
    glBindProgramARB = (PFNGLBINDPROGRAMARBPROC) glXGetProcAddress((GLubyte *) "glBindProgramARB");

    glProgramStringARB = (PFNGLPROGRAMSTRINGARBPROC)
                         glXGetProcAddress((GLubyte *) "glProgramStringARB");

    glDeleteProgramsARB = (PFNGLDELETEPROGRAMSARBPROC)
                          glXGetProcAddress((GLubyte *) "glDeleteProgramsARB");

    glProgramLocalParameter4fvARB = (PFNGLPROGRAMLOCALPARAMETER4FVARBPROC)
                                    glXGetProcAddress((GLubyte *) "glProgramLocalParameter4fvARB");

    glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)
                         glXGetProcAddress((GLubyte *) "glActiveTextureARB");

    glMultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC)
                           glXGetProcAddress((GLubyte *) "glMultiTexCoord2fARB");
#endif

    if (glGenProgramsARB              && glBindProgramARB   && glProgramStringARB &&
        glProgramLocalParameter4fvARB && glActiveTextureARB && glMultiTexCoord2fARB)
    {
        createShader();

        shader = true;

        shaderCount++;
    }
    else qWarning("WBackendVlcPrivate::initShader: Fragment shaders are not supported.");
}

//-------------------------------------------------------------------------------------------------

void WBackendVlcPrivate::createShader()
{
    const char * code =
        "!!ARBfp1.0"
        "OPTION ARB_precision_hint_fastest;"

        "TEMP src;"
        "TEX  src.x, fragment.texcoord[0], texture[0], 2D;"
        "TEX  src.y, fragment.texcoord[1], texture[1], 2D;"
        "TEX  src.z, fragment.texcoord[2], texture[2], 2D;"

        "PARAM coefficient[4] = { program.local[0..3] };"

        "TEMP tmp;"
        "MAD  tmp,          src.xxxx, coefficient[0], coefficient[3];"
        "MAD  tmp,          src.yyyy, coefficient[1], tmp;"
        "MAD  result.color, src.zzzz, coefficient[2], tmp;"
        "END";

    glGenProgramsARB(1, &shaderId);

    glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, shaderId);

    glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB,
                       GL_PROGRAM_FORMAT_ASCII_ARB, strlen(code), (const GLbyte *) code);
}

void WBackendVlcPrivate::deleteShader()
{
    if (shader == false) return;

    shaderCount--;

    if (shaderCount == 0)
    {
        glDeleteProgramsARB(1, &shaderId);
    }
}

//-------------------------------------------------------------------------------------------------

void WBackendVlcPrivate::convertFrameSoftware()
{
    uint32_t * bits = (uint32_t *) frameSoftware.bits();

    uint8_t * p_y = textures[0].bits;
    uint8_t * p_u = textures[1].bits;
    uint8_t * p_v = textures[2].bits;

    uint8_t u, v;

    int r, g, b;

    uint32_t * yBase;

    int rewind = -(textures[0].width) & 7;

    for (int y = 0; y < textures[0].height; y++)
    {
        for (int x = textures[0].width / 8; x--;)
        {
            W_CONVERT_YUV_PIXEL; W_CONVERT_Y_PIXEL;
            W_CONVERT_YUV_PIXEL; W_CONVERT_Y_PIXEL;
            W_CONVERT_YUV_PIXEL; W_CONVERT_Y_PIXEL;
            W_CONVERT_YUV_PIXEL; W_CONVERT_Y_PIXEL;
        }

        if (rewind)
        {
            p_y -= rewind;
            p_u -= rewind >> 1;
            p_v -= rewind >> 1;

            bits -= rewind;

            W_CONVERT_YUV_PIXEL; W_CONVERT_Y_PIXEL;
            W_CONVERT_YUV_PIXEL; W_CONVERT_Y_PIXEL;
            W_CONVERT_YUV_PIXEL; W_CONVERT_Y_PIXEL;
            W_CONVERT_YUV_PIXEL; W_CONVERT_Y_PIXEL;
        }

        p_y += textures[0].pitchMargin;
        p_u += textures[1].pitchMargin;
        p_v += textures[2].pitchMargin;

        if (!(y & 0x1))
        {
            p_u -= textures[1].pitch;
            p_v -= textures[2].pitch;
        }
    }
}

void WBackendVlcPrivate::convertFrameSse()
{
    uint32_t * p_buffer = (uint32_t *) frameSoftware.bits();

    uint8_t * p_y = textures[0].bits;
    uint8_t * p_u = textures[1].bits;
    uint8_t * p_v = textures[2].bits;

    int rewind = -(textures[0].width) & 15;

    if (0 == (15 & (textures[0].width | ((intptr_t) p_y)  | ((intptr_t) p_buffer))))
    {
        for (int y = 0; y < textures[0].height; y++)
        {
            for (int x = textures[0].width / 16; x--;)
            {
                SSE2_CALL (SSE2_INIT_32_ALIGNED
                           SSE2_YUV_MUL
                           SSE2_YUV_ADD
                           SSE2_UNPACK_32_ARGB_ALIGNED);

                p_y += 16;
                p_u += 8;
                p_v += 8;

                p_buffer += 16;
            }

            if (rewind)
            {
                p_y -= rewind;
                p_u -= rewind >> 1;
                p_v -= rewind >> 1;

                p_buffer -= rewind;

                SSE2_CALL (SSE2_INIT_32_UNALIGNED
                           SSE2_YUV_MUL
                           SSE2_YUV_ADD
                           SSE2_UNPACK_32_ARGB_UNALIGNED);

                p_y += 16;
                p_u += 4;
                p_v += 4;

                p_buffer += 16;
            }

            p_y += textures[0].pitchMargin;
            p_u += textures[1].pitchMargin;
            p_v += textures[2].pitchMargin;

            if (!(y & 0x1))
            {
                p_u -= textures[1].pitch;
                p_v -= textures[2].pitch;
            }
        }
    }
    else
    {
        for (int y = 0; y < textures[0].height; y++)
        {
            for (int x = textures[0].width / 16; x--;)
            {
                SSE2_CALL (SSE2_INIT_32_UNALIGNED
                           SSE2_YUV_MUL
                           SSE2_YUV_ADD
                           SSE2_UNPACK_32_ARGB_UNALIGNED);

                p_y += 16;
                p_u += 8;
                p_v += 8;

                p_buffer += 16;
            }

            if (rewind)
            {
                p_y -= rewind;
                p_u -= rewind >> 1;
                p_v -= rewind >> 1;

                p_buffer -= rewind;

                SSE2_CALL (SSE2_INIT_32_UNALIGNED
                           SSE2_YUV_MUL
                           SSE2_YUV_ADD
                           SSE2_UNPACK_32_ARGB_UNALIGNED);

                p_y += 16;
                p_u += 8;
                p_v += 8;

                p_buffer += 16;
            }

            p_y += textures[0].pitchMargin;
            p_u += textures[1].pitchMargin;
            p_v += textures[2].pitchMargin;

            if (!(y & 0x1))
            {
                p_u -= textures[1].pitch;
                p_v -= textures[2].pitch;
            }
        }
    }

    SSE2_END;
}

//-------------------------------------------------------------------------------------------------

void WBackendVlcPrivate::loadSources(bool play)
{
    if (reply) return;

    Q_Q(WBackendVlc);

    qDebug("Loading Source %s", source.toString().C_STR);

    reply = wControllerMedia->getMedia(source, q);

    if (reply == NULL)
    {
        applyOutput(getClosestOutput(output));

        if (play) playMedia();
    }
    else if (reply->isLoaded())
    {
        applySources(play);

        delete reply;

        reply = NULL;
    }
    else QObject::connect(reply, SIGNAL(loaded(WMediaReply *)), q, SLOT(onLoaded()));
}

void WBackendVlcPrivate::applySources(bool play)
{
    Q_Q(WBackendVlc);

    medias = reply->medias();
    audios = reply->audios();

    closestQuality = getClosestQuality(quality);

    if (closestQuality == WAbstractBackend::QualityInvalid)
    {
        closestOutput = WAbstractBackend::OutputInvalid;

        q->stop();
    }
    else
    {
        currentMedia = medias.value(closestQuality);
        currentAudio = audios.value(closestQuality);

        applyOutput(getClosestOutput(output));

        if (play) playMedia();
    }

    qDebug("Current source [%s] %d %s", currentMedia.C_URL,
                                        reply->medias().count(), reply->error().C_STR);
}

//-------------------------------------------------------------------------------------------------

bool WBackendVlcPrivate::applyOutput(WAbstractBackend::Output output)
{
    if (output == WAbstractBackend::OutputInvalid || closestOutput == output)
    {
        return false;
    }

    closestOutput = output;

    player->setOutput(output);

    return true;
}

//-------------------------------------------------------------------------------------------------

void WBackendVlcPrivate::playMedia()
{
    Q_Q(WBackendVlc);

    player->setSource(currentMedia, currentAudio);

    q->setOutputActive (closestOutput);
    q->setQualityActive(closestQuality);

    if (currentTime == -1)
    {
        player->play();

        q->setStateLoad(WAbstractBackend::StateLoadStarting);
    }
    else
    {
        player->play(currentTime);

        q->setStateLoad(WAbstractBackend::StateLoadResuming);
    }
}

//-------------------------------------------------------------------------------------------------

void WBackendVlcPrivate::updateTargetRect()
{
    if (frameWidth == -1) return;

    QSizeF frameSize = QSizeF(frameWidth, frameHeight);

    frameSize.scale(size, ratio);

    qreal width  = frameSize.width ();
    qreal height = frameSize.height();

    int x = (size.width () - width)  / 2;
    int y = (size.height() - height) / 2;

    targetRect = QRect(x, y, width, height);

    targetX = x;
    targetY = y;

    targetWidth  = x + width;
    targetHeight = y + height;
}

//-------------------------------------------------------------------------------------------------

void WBackendVlcPrivate::updateLoading()
{
    Q_Q(WBackendVlc);

    if (currentTime == -1)
    {
         q->setStateLoad(WAbstractBackend::StateLoadStarting);
    }
    else q->setStateLoad(WAbstractBackend::StateLoadResuming);
}

//-------------------------------------------------------------------------------------------------

void WBackendVlcPrivate::clearPlayer()
{
    if (started)
    {
        started = false;
        active  = false;

        frameFreeze = false;

        if (parentItem) onFrameUpdated();
    }
    else if (frameFreeze)
    {
        frameFreeze = false;

        if (parentItem) onFrameUpdated();
    }

    playing = false;
}

void WBackendVlcPrivate::clearReply()
{
    if (reply == NULL) return;

    delete reply;

    reply = NULL;
}

void WBackendVlcPrivate::clearActive()
{
    Q_Q(WBackendVlc);

    q->setOutputActive (WAbstractBackend::OutputInvalid);
    q->setQualityActive(WAbstractBackend::QualityInvalid);
}

//-------------------------------------------------------------------------------------------------

void WBackendVlcPrivate::setOpacity(GLfloat opacity)
{
    if (this->opacity == opacity) return;

    this->opacity = opacity;

    GLfloat * values = this->values;

    for (int i = 0; i < 4; i++)
    {
        values[3] = opacity;

        glProgramLocalParameter4fvARB(GL_FRAGMENT_PROGRAM_ARB, i, values);

        values += 4;
    }
}

//-------------------------------------------------------------------------------------------------

void WBackendVlcPrivate::setMute(bool enabled)
{
    if (mute == enabled) return;

    mute = enabled;

    if (enabled)
    {
         player->setVolume(0);
    }
    else player->setVolume(volume);
}

//-------------------------------------------------------------------------------------------------

WAbstractBackend::Output WBackendVlcPrivate::getClosestOutput(WAbstractBackend::Output output)
{
    if (output == WAbstractBackend::OutputInvalid)
    {
        return WAbstractBackend::OutputInvalid;
    }

    if (output == WAbstractBackend::OutputAudio
        ||
        (currentAudio.isEmpty() && WControllerPlaylist::urlIsAudio(currentMedia)))
    {
         return WAbstractBackend::OutputAudio;
    }
    else return output;
}

WAbstractBackend::Quality WBackendVlcPrivate::getClosestQuality(WAbstractBackend::Quality quality)
{
    if (quality == WAbstractBackend::QualityInvalid)
    {
        return WAbstractBackend::QualityInvalid;
    }
    else if (medias.value(quality).isValid())
    {
        return quality;
    }

    for (int i = quality - 1; i > WAbstractBackend::QualityInvalid; i--)
    {
        WAbstractBackend::Quality closestQuality = static_cast<WAbstractBackend::Quality> (i);

        if (medias.value(closestQuality).isValid())
        {
            return closestQuality;
        }
    }

    for (int i = quality + 1; i <= WAbstractBackend::QualityMaximum; i++)
    {
        WAbstractBackend::Quality closestQuality = static_cast<WAbstractBackend::Quality> (i);

        if (medias.value(closestQuality).isValid())
        {
            return closestQuality;
        }
    }

    return WAbstractBackend::QualityInvalid;
}

//-------------------------------------------------------------------------------------------------
// Privae slots
//-------------------------------------------------------------------------------------------------

void WBackendVlcPrivate::onLoaded()
{
    Q_Q(WBackendVlc);

    if (reply->hasError())
    {
        q->stop();
    }
    else applySources(q->isPlaying());

    reply->deleteLater();

    reply = NULL;
}

//-------------------------------------------------------------------------------------------------

void WBackendVlcPrivate::onFrameUpdated()
{
    frameUpdated = true;

    if (parentItem) parentItem->update();
}

//-------------------------------------------------------------------------------------------------
// Static private functions
//-------------------------------------------------------------------------------------------------

/* static */ unsigned WBackendVlcPrivate::setup(void ** data, char * chroma, unsigned * vlcWidth,
                                                                             unsigned * vlcHeight,
                                                                             unsigned * pitches,
                                                                             unsigned * lines)
{
    WBackendVlcPrivate * d = static_cast<WBackendVlc *> (*data)->d_func();

    libvlc_video_get_size(d->player->d_func()->player, 0, vlcWidth, vlcHeight);

    int width  = qMin((int) (*vlcWidth),  PLAYER_MAX_WIDTH);
    int height = qMin((int) (*vlcHeight), PLAYER_MAX_HEIGHT);

    strcpy(chroma, "I420");

    pitches[0] = width;
    pitches[1] = width / 2;

    while (pitches[0] % 4) pitches[0]++;
    while (pitches[1] % 4) pitches[1]++;

    pitches[2] = pitches[1];

    lines[0] = height;
    lines[1] = height / 2;

    while (lines[0] % 4) lines[0]++;
    while (lines[1] % 4) lines[1]++;

    lines[2] = lines[1];

    d->mutex.lock();

    if (d->frameA.width() != width || d->frameA.height() != height)
    {
        d->frameA = QImage(width, height, QImage::Format_RGB16);
        d->frameB = QImage(width, height, QImage::Format_RGB16);

        int cursorU = pitches[0] * lines[0];

        int cursorV = cursorU + pitches[1] * lines[1];

        d->textures[0].bitsA = d->frameA.bits();
        d->textures[1].bitsA = d->frameA.bits() + cursorU;
        d->textures[2].bitsA = d->frameA.bits() + cursorV;

        d->textures[0].bitsB = d->frameB.bits();
        d->textures[1].bitsB = d->frameB.bits() + cursorU;
        d->textures[2].bitsB = d->frameB.bits() + cursorV;

        d->textures[0].bits = d->textures[0].bitsA;
        d->textures[1].bits = d->textures[1].bitsA;
        d->textures[2].bits = d->textures[2].bitsA;

        d->mutex.unlock();

        QCoreApplication::postEvent(d->q_func(),
                                    new WBackendVlcEventSetup(width, height,
                                                              pitches[0], pitches[1], pitches[2]));
    }
    else d->mutex.unlock();

    return 1;
}

//-------------------------------------------------------------------------------------------------

/* static */ void * WBackendVlcPrivate::lock(void * data, void ** buffer)
{
    WBackendVlcPrivate * d = static_cast<WBackendVlc *> (data)->d_func();

    d->mutex.lock();

    d->frameIndex = !(d->frameIndex);

    if (d->frameIndex)
    {
        buffer[0] = d->textures[0].bitsA;
        buffer[1] = d->textures[1].bitsA;
        buffer[2] = d->textures[2].bitsA;
    }
    else
    {
        buffer[0] = d->textures[0].bitsB;
        buffer[1] = d->textures[1].bitsB;
        buffer[2] = d->textures[2].bitsB;
    }

    return NULL;
}

/* static */ void WBackendVlcPrivate::unlock(void * data, void *, void * const *)
{
    WBackendVlc * backend = static_cast<WBackendVlc *> (data);

    WBackendVlcPrivate * d = backend->d_func();

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

    d->mutex.unlock();

    d->method.invoke(backend);
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

WBackendVlc::WBackendVlc() : WAbstractBackend(new WBackendVlcPrivate(this))
{
    Q_D(WBackendVlc); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WBackendVlc::setProxy(const QString & host,
                                             int             port, const QString & password)
{
    Q_D(WBackendVlc);

    d->clearReply();

    d->player->setProxy(host, port, password);

    d->currentMedia = QUrl();
    d->currentAudio = QUrl();

    d->medias.clear();
    d->audios.clear();
}

/* Q_INVOKABLE */ void WBackendVlc::clearProxy()
{
    Q_D(WBackendVlc);

    d->clearReply();

    d->player->clearProxy();

    d->currentMedia = QUrl();
    d->currentAudio = QUrl();

    d->medias.clear();
    d->audios.clear();
}

//-------------------------------------------------------------------------------------------------
// Protected WAbstractBackend implementation
//-------------------------------------------------------------------------------------------------

/* virtual */ bool WBackendVlc::backendSetSource(const QUrl & url)
{
    Q_D(WBackendVlc);

    d->clearReply();

    d->currentMedia = QUrl();
    d->currentAudio = QUrl();

    if (url.isEmpty())
    {
        d->clearPlayer();

        d->player->stop();

        d->clearActive();
    }
    else if (isPlaying())
    {
        d->updateLoading();

        backendStop();

        d->loadSources(true);
    }

    return true;
}

//-------------------------------------------------------------------------------------------------

/* virtual */ bool WBackendVlc::backendPlay()
{
    Q_D(WBackendVlc);

    if (isPaused() == false)
    {
        if (d->currentMedia.isValid() == false)
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

/* virtual */ bool WBackendVlc::backendPause()
{
    Q_D(WBackendVlc);

    if (d->started)
    {
        d->player->pause();
    }
    // FIXME VLC: Muting the sound to avoid the audio glitch.
    else d->setMute(true);

    return true;
}

/* virtual */ bool WBackendVlc::backendStop()
{
    Q_D(WBackendVlc);

    d->clearPlayer();

    d->player->pause();

    d->clearActive();

    return true;
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WBackendVlc::backendSetVolume(qreal volume)
{
    Q_D(WBackendVlc);

    if (volume)
    {
        if (volume < 1.0)
        {
             d->volume = qRound(volume * 80) + 20;
        }
        else d->volume = qRound(volume * 100);
    }
    else d->volume = 0;

    if (d->mute == false)
    {
        d->player->setVolume(d->volume);
    }
}

//-------------------------------------------------------------------------------------------------

/* virtual */ bool WBackendVlc::backendDelete()
{
    Q_D(WBackendVlc);

    d->clearPlayer();
    d->clearActive();

    d->player->deletePlayer();

    return false;
}

//-------------------------------------------------------------------------------------------------
// Protected WAbstractBackend reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WBackendVlc::backendSeek(int msec)
{
    Q_D(WBackendVlc);

    d->player->seek(msec);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WBackendVlc::backendSetSpeed(qreal speed)
{
    Q_D(WBackendVlc);

    d->player->setSpeed(speed);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WBackendVlc::backendSetOutput(Output output)
{
    Q_D(WBackendVlc);

    Output closestOutput = d->getClosestOutput(output);

    if (d->applyOutput(closestOutput) && hasStarted())
    {
        d->started = false;
        d->active  = false;

        d->frameFreeze = true;

        d->player->setSource(d->currentMedia, d->currentAudio);

        setOutputActive(closestOutput);

        if (d->state == StatePlaying)
        {
            d->player->play(d->currentTime);
        }
        else if (d->state == StatePaused)
        {
            d->setMute(true);

            d->player->play(d->currentTime);
        }
    }
}

/* virtual */ void WBackendVlc::backendSetQuality(Quality quality)
{
    Q_D(WBackendVlc);

    Quality closestQuality = d->getClosestQuality(quality);

    if (closestQuality == QualityInvalid || d->closestQuality == closestQuality) return;

    QUrl media = d->medias.value(closestQuality);

    if (d->currentMedia == media) return;

    d->closestQuality = closestQuality;

    d->currentMedia = media;

    d->currentAudio = d->audios.value(closestQuality);

    if (hasStarted())
    {
        d->started = false;

        d->frameFreeze = true;

        d->player->setSource(d->currentMedia, d->currentAudio);

        setQualityActive(closestQuality);

        if (d->state == StatePlaying)
        {
            d->player->play(d->currentTime);
        }
        else if (d->state == StatePaused)
        {
            d->setMute(true);

            d->player->play(d->currentTime);
        }
    }
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WBackendVlc::backendSetFillMode(FillMode fillMode)
{
    Q_D(WBackendVlc);

    d->ratio = static_cast<Qt::AspectRatioMode> (fillMode);

    d->updateTargetRect();
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WBackendVlc::backendSetSize(const QSizeF &)
{
    Q_D(WBackendVlc);

    d->updateTargetRect();
}

/* virtual */ void WBackendVlc::backendDrawFrame(QPainter                       * painter,
                                                 const QStyleOptionGraphicsItem * option)
{
    Q_D(WBackendVlc);

    if (d->active == false)
    {
        painter->setPen  (Qt::NoPen);
        painter->setBrush(Qt::black);

        painter->drawRect(option->rect);
    }
    else if (painter->paintEngine()->type() == QPaintEngine::OpenGL2)
    {
        if (d->frameUpdated && d->frameFreeze == false)
        {
            d->frameUpdated = false;

            if (d->frameReset)
            {
                d->frameReset = false;

                d->initShader();

                if (d->textureIds[0] != 0)
                {
                    glDeleteTextures(3, d->textureIds);
                }

                glGenTextures(3, d->textureIds);

                for (int i = 0; i < 3; i++)
                {
                    glActiveTextureARB(GL_TEXTURE0_ARB + i);

                    glBindTexture(GL_TEXTURE_2D, d->textureIds[i]);

                    glTexImage2D(GL_TEXTURE_2D, 0, PLAYER_FORMAT_INTERNAL,
                                 d->textures[i].width, d->textures[i].height,
                                 0, PLAYER_FORMAT, PLAYER_DATA_TYPE, 0);

                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

                    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                                    d->textures[i].width, d->textures[i].height,
                                    PLAYER_FORMAT, PLAYER_DATA_TYPE, d->textures[i].bits);
                }

                d->opacity = painter->opacity();

                const qreal matrix[12] =
                {
                    1.164383561643836,  0.0000,             1.792741071428571, -0.972945075016308,
                    1.164383561643836, -0.21324861427373,  -0.532909328559444,  0.301482665475862,
                    1.164383561643836,  2.112401785714286,  0.0000,            -1.133402217873451,
                };

                GLfloat * values = d->values;

                for (int i = 0; i < 4; i++)
                {
                    for (int j = 0; j < 4; j++)
                    {
                        if (j == 3)
                        {
                             values[j] = d->opacity;
                        }
                        else values[j] = matrix[j * 4 + i];
                    }

                    glProgramLocalParameter4fvARB(GL_FRAGMENT_PROGRAM_ARB, i, values);

                    values += 4;
                }

                if (glGetError() == GL_INVALID_OPERATION)
                {
                    qWarning("WBackendVlc::backendDrawFrame: Opengl texture setup failed.");
                }
            }
            else
            {
                for (int i = 0; i < 3; i++)
                {
                    glActiveTextureARB(GL_TEXTURE0_ARB + i);

                    glBindTexture(GL_TEXTURE_2D, d->textureIds[i]);

                    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                                    d->textures[i].width, d->textures[i].height,
                                    PLAYER_FORMAT, PLAYER_DATA_TYPE, d->textures[i].bits);
                }

                d->setOpacity(painter->opacity());
            }
        }
        else if (d->textureIds[0])
        {
            for (int i = 0; i < 3; i++)
            {
                glActiveTextureARB(GL_TEXTURE0_ARB + i);

                glBindTexture(GL_TEXTURE_2D, d->textureIds[i]);
            }

            d->setOpacity(painter->opacity());
        }
        else
        {
            painter->setPen  (Qt::NoPen);
            painter->setBrush(Qt::black);

            painter->drawRect(option->rect);

            return;
        }

        qreal width  = option->rect.width ();
        qreal height = option->rect.height();

        painter->beginNativePainting();

        const QTransform & m = painter->transform();

        float matrix[4][4] =
        {
            { float(m.m11()), float(m.m12()), 0, float(m.m13()) },
            { float(m.m21()), float(m.m22()), 0, float(m.m23()) },
            {              0,              0, 1,              0 },
            {  float(m.dx()),  float(m.dy()), 0, float(m.m33()) }
        };

        QPaintDevice * device = painter->device();

        const QSize size = QSize(device->width(), device->height());

        glMatrixMode(GL_PROJECTION);

        glLoadIdentity();

        glOrtho(0, size.width(), size.height(), 0, -999999, 999999);

        glMatrixMode(GL_MODELVIEW);

        glLoadMatrixf(&matrix[0][0]);

        glEnable(GL_BLEND);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        if (d->fillMode == PreserveAspectFit)
        {
            if (painter->hasClipping())
            {
                W_SCISSOR;

                W_DRAW_BACK_AND_FRAME;

                glDisable(GL_SCISSOR_TEST);
            }
            else W_DRAW_BACK_AND_FRAME;
        }
        else if (d->fillMode == PreserveAspectCrop)
        {
            if (painter->hasClipping() == false)
            {
                glEnable(GL_SCISSOR_TEST);

                const QTransform & transform = painter->transform();

                glScissor(transform.dx(),
                          painter->viewport().height() - transform.dy() - height,
                          width, height);
            }
            else W_SCISSOR;

            W_DRAW_FRAME;

            glDisable(GL_SCISSOR_TEST);
        }
        else // if (d->fillMode == Stretch)
        {
            if (painter->hasClipping())
            {
                W_SCISSOR;

                W_DRAW_FRAME;

                glDisable(GL_SCISSOR_TEST);
            }
            else W_DRAW_FRAME;
        }

        glDisable(GL_BLEND);

        painter->endNativePainting();
    }
    else
    {
        painter->setPen  (Qt::NoPen);
        painter->setBrush(Qt::black);

        painter->drawRect(option->rect);

        if (d->frameFreeze == false) d->convertFrameSse();

        bool smooth = painter->testRenderHint(QPainter::SmoothPixmapTransform);

        painter->setRenderHint(QPainter::SmoothPixmapTransform);

        painter->drawImage(d->targetRect, d->frameSoftware);

        painter->setRenderHint(QPainter::SmoothPixmapTransform, smooth);
    }
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WBackendVlc::backendUpdateFrame()
{
    Q_D(WBackendVlc); d->convertFrameSse();
}

/* virtual */ QImage WBackendVlc::backendGetFrame() const
{
    Q_D(const WBackendVlc);

    if (d->frameWidth == -1) return QImage();

    QImage image(d->frameWidth, d->frameHeight, QImage::Format_RGB32);

    QPainter painter(&image);

    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    painter.drawImage(QRect(0, 0, d->frameWidth, d->frameHeight), d->frameSoftware);

    return image;
}

//-------------------------------------------------------------------------------------------------
// Events
//-------------------------------------------------------------------------------------------------

/* virtual */ bool WBackendVlc::event(QEvent * event)
{
    QEvent::Type type = event->type();

    if (type == static_cast<QEvent::Type> (WBackendVlcPrivate::EventSetup))
    {
        Q_D(WBackendVlc);

        WBackendVlcEventSetup * setup = static_cast<WBackendVlcEventSetup *> (event);

        int width  = setup->width;
        int height = setup->height;

        int pitchY = setup->pitchY;
        int pitchU = setup->pitchU;
        int pitchV = setup->pitchV;

        d->frameWidth  = width;
        d->frameHeight = height;

        d->frameSoftware = QImage(width, height, QImage::Format_RGB32);

        d->textures[0].width  = width;
        d->textures[0].height = height;

        d->textures[1].width  = width  / 2;
        d->textures[1].height = height / 2;

        d->textures[2].width  = d->textures[1].width;
        d->textures[2].height = d->textures[1].height;

        d->textures[0].pitch = pitchY;
        d->textures[1].pitch = pitchU;
        d->textures[2].pitch = pitchV;

        d->textures[0].pitchMargin = pitchY - d->textures[0].width;
        d->textures[1].pitchMargin = pitchU - d->textures[1].width;

        d->textures[2].pitchMargin = d->textures[1].pitchMargin;

        d->frameIndex = false;

        d->updateTargetRect();

        d->frameReset = true;

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcPlayer::EventPlaying))
    {
        Q_D(WBackendVlc);

        if (d->state == StatePlaying)
        {
            d->playing = true;

            if (d->started == false)
            {
                d->frameUpdated = false;
            }

            d->setMute(false);
        }
        else if (d->state == StateStopped)
        {
            d->player->pause();
        }
        else d->playing = true;

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcPlayer::EventPaused))
    {
        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcPlayer::EventStopped))
    {
        Q_D(WBackendVlc);

        d->clearPlayer();

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcPlayer::EventBuffering))
    {
        Q_D(WBackendVlc);

        if (d->stateLoad == StateLoadDefault)
        {
            setStateLoad(StateLoadBuffering);
        }

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcPlayer::EventLengthChanged))
    {
        WVlcPlayerEvent * eventPlayer = static_cast<WVlcPlayerEvent *> (event);

        setDuration(eventPlayer->value.toInt());

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcPlayer::EventTimeChanged))
    {
        Q_D(WBackendVlc);

        if (d->playing == false) return true;

        WVlcPlayerEvent * eventPlayer = static_cast<WVlcPlayerEvent *> (event);

        if (d->started == false)
        {
            d->started = true;
            d->active  = true;

            d->frameFreeze = false;

            if (d->state == StatePaused)
            {
                d->player->pause();

                return true;
            }
        }

        int time = eventPlayer->value.toInt();

        setCurrentTime(time);

        if (time > d->duration)
        {
            setDuration(time);
        }

        setStateLoad(StateLoadDefault);

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcPlayer::EventEndReached))
    {
        Q_D(WBackendVlc);

        if (d->started)
        {
            // FIXME: Sometimes the playback ends when seeking.
            if ((d->currentTime + 1000) < d->duration)
            {
                qWarning("WBackendVlc::event: Vlc player ended.");

                if (d->frameFreeze == false)
                {
                    d->started = false;

                    d->frameFreeze = true;

                    d->player->setSource(d->currentMedia, d->currentAudio);

                    d->player->play(d->currentTime);
                }
                else stop();
            }
            else
            {
                if (d->repeat == false)
                {
                    d->clearPlayer();
                }

                setEnded(true);
            }
        }
        else stop();

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcPlayer::EventError))
    {
        Q_D(WBackendVlc);

        qWarning("WBackendVlc::event: Vlc player error.");

        stop();

        wControllerMedia->clearMedia(d->source);

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcPlayer::EventDelete))
    {
        deleteNow();

        return true;
    }
    else return QObject::event(event);
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

QStringList WBackendVlc::options()
{
    Q_D(WBackendVlc); return d->player->options();
}

void WBackendVlc::setOptions(const QStringList & options)
{
    Q_D(WBackendVlc); d->player->setOptions(options);
}

//-------------------------------------------------------------------------------------------------

int WBackendVlc::networkCache()
{
    Q_D(WBackendVlc); return d->player->networkCache();
}

void WBackendVlc::setNetworkCache(int msec)
{
    Q_D(WBackendVlc); d->player->setNetworkCache(msec);
}

#endif // SK_NO_BACKENDVLC
