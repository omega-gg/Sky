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

#include "WBackendVlc.h"

#ifndef SK_NO_BACKENDVLC

// Qt includes
#include <QCoreApplication>
#if defined(QT_6) || defined(SK_NO_QML)
#include <QPainter>
#endif
#ifdef QT_4
#include <qmath.h>
#elif defined(QT_5)
#include <QtMath>
#endif

// Sk includes
#ifdef QT_6
#include <WControllerApplication>
#endif
#include <WControllerMedia>
#include <WControllerPlaylist>

// 3rdparty includes
#include <3rdparty/vlc/mmxRgb.h>
#if defined(QT_6) == false && defined(SK_NO_QML) == false \
    && \
    defined(Q_OS_MAC) == false && defined(Q_OS_ANDROID) == false
#include <3rdparty/opengl/glext.h>
#endif

#ifndef SK_NO_QML

// Mac includes
#if defined(QT_6) == false && defined(Q_OS_MACOS)
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#endif

// Linux includes
#if defined(QT_6) == false && defined(Q_OS_LINUX) && defined(Q_OS_ANDROID) == false
#include <GL/glx.h>
#endif

#endif // SK_NO_QML

// Android includes
#ifdef __ARM_NEON
#include <arm_neon.h>
#endif

// Private includes
#include <private/WVlcPlayer_p>

//-------------------------------------------------------------------------------------------------
// Defines
//-------------------------------------------------------------------------------------------------
// RGB

#define RED_MARGIN    200
#define GREEN_MARGIN  135
#define BLUE_MARGIN   236
#define RED_OFFSET   1454
#define GREEN_OFFSET  135
#define BLUE_OFFSET   762

//-------------------------------------------------------------------------------------------------
// YUV pixel conversions

#define SHIFT 20

// NOTE: These are the BT.709 coefficients of the video shader, the table scales Y.
#define U_GREEN_COEF ((int) (-0.213249 * (1 << SHIFT) / 1.164384))
#define U_BLUE_COEF  ((int) ( 2.112402 * (1 << SHIFT) / 1.164384))
#define V_RED_COEF   ((int) ( 1.792741 * (1 << SHIFT) / 1.164384))
#define V_GREEN_COEF ((int) (-0.532909 * (1 << SHIFT) / 1.164384))

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
// SSE2

#ifdef CAN_COMPILE_SSE2

// NOTE: This is the SSE2_YUV_MUL of mmxRgb.h with the BT.709 coefficients of the video shader,
//       VLC has BT.601 ones.
#undef SSE2_YUV_MUL

#define SSE2_YUV_MUL "                                                      \n\
# convert the chroma part                                                   \n\
punpcklbw %%xmm4, %%xmm0        # scatter 8 Cb    00 u3 00 u2 00 u1 00 u0   \n\
punpcklbw %%xmm4, %%xmm1        # scatter 8 Cr    00 v3 00 v2 00 v1 00 v0   \n\
movl      $0x00800080, %%eax    #                                           \n\
movd      %%eax, %%xmm5         #                                           \n\
pshufd    $0, %%xmm5, %%xmm5    # Set xmm5 to     0080 0080 ... 0080 0080   \n\
psubsw    %%xmm5, %%xmm0        # Cb -= 128                                 \n\
psubsw    %%xmm5, %%xmm1        # Cr -= 128                                 \n\
psllw     $3, %%xmm0            # Promote precision                         \n\
psllw     $3, %%xmm1            # Promote precision                         \n\
movdqa    %%xmm0, %%xmm2        # Copy 8 Cb       00 u3 00 u2 00 u1 00 u0   \n\
movdqa    %%xmm1, %%xmm3        # Copy 8 Cr       00 v3 00 v2 00 v1 00 v0   \n\
movl      $0xf92df92d, %%eax    #                                           \n\
movd      %%eax, %%xmm5         #                                           \n\
pshufd    $0, %%xmm5, %%xmm5    # Set xmm5 to     f92d f92d ... f92d f92d   \n\
pmulhw    %%xmm5, %%xmm2        # Mul Cb with green coeff -> Cb green       \n\
movl      $0xeef2eef2, %%eax    #                                           \n\
movd      %%eax, %%xmm5         #                                           \n\
pshufd    $0, %%xmm5, %%xmm5    # Set xmm5 to     eef2 eef2 ... eef2 eef2   \n\
pmulhw    %%xmm5, %%xmm3        # Mul Cr with green coeff -> Cr green       \n\
movl      $0x43994399, %%eax    #                                           \n\
movd      %%eax, %%xmm5         #                                           \n\
pshufd    $0, %%xmm5, %%xmm5    # Set xmm5 to     4399 4399 ... 4399 4399   \n\
pmulhw    %%xmm5, %%xmm0        # Mul Cb -> Cblue 00 b3 00 b2 00 b1 00 b0   \n\
movl      $0x395e395e, %%eax    #                                           \n\
movd      %%eax, %%xmm5         #                                           \n\
pshufd    $0, %%xmm5, %%xmm5    # Set xmm5 to     395e 395e ... 395e 395e   \n\
pmulhw    %%xmm5, %%xmm1        # Mul Cr -> Cred  00 r3 00 r2 00 r1 00 r0   \n\
paddsw    %%xmm3, %%xmm2        # Cb green + Cr green -> Cgreen             \n\
                                                                            \n\
# convert the luma part                                                     \n\
movl      $0x10101010, %%eax    #                                           \n\
movd      %%eax, %%xmm5         #                                           \n\
pshufd    $0, %%xmm5, %%xmm5    # Set xmm5 to   1010 1010 ... 1010 1010     \n\
psubusb   %%xmm5, %%xmm6        # Y -= 16                                   \n\
movdqa    %%xmm6, %%xmm7        # Copy 16 Y       Y7 Y6 Y5 Y4 Y3 Y2 Y1 Y0   \n\
movl      $0x00ff00ff, %%eax    #                                           \n\
movd      %%eax, %%xmm5         #                                           \n\
pshufd    $0, %%xmm5, %%xmm5    # set xmm5 to     00ff 00ff ... 00ff 00ff   \n\
pand      %%xmm5, %%xmm6        # get Y even      00 Y6 00 Y4 00 Y2 00 Y0   \n\
psrlw     $8, %%xmm7            # get Y odd       00 Y7 00 Y5 00 Y3 00 Y1   \n\
psllw     $3, %%xmm6            # Promote precision                         \n\
psllw     $3, %%xmm7            # Promote precision                         \n\
movl      $0x253f253f, %%eax    #                                           \n\
movd      %%eax, %%xmm5         #                                           \n\
pshufd    $0, %%xmm5, %%xmm5    # set xmm5 to     253f 253f ... 253f 253f   \n\
pmulhw    %%xmm5, %%xmm6        # Mul 8 Y even    00 y6 00 y4 00 y2 00 y0   \n\
pmulhw    %%xmm5, %%xmm7        # Mul 8 Y odd     00 y7 00 y5 00 y3 00 y1   \n\
"

#endif

#ifndef SK_NO_QML

//-------------------------------------------------------------------------------------------------
// Opengl

#if defined(QT_4) && defined(Q_OS_MACOS) == false
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

#endif // SK_NO_QML

//-------------------------------------------------------------------------------------------------
// Static variables

#if defined(QT_4) && defined(SK_NO_QML) == false

static const int PLAYER_FORMAT          = GL_LUMINANCE;
static const int PLAYER_FORMAT_INTERNAL = GL_LUMINANCE;

static const int PLAYER_DATA_TYPE = GL_UNSIGNED_BYTE;

#endif

static const int PLAYER_MAX_WIDTH  = 5760;
static const int PLAYER_MAX_HEIGHT = 3240;

//-------------------------------------------------------------------------------------------------

#ifndef SK_NO_QML

#ifdef QT_4

static GLuint shaderId    = 0;
static int    shaderCount = 0;

static GLfloat shaderValues[16];

static GLfloat shaderOpacity = 1.f;

//=================================================================================================
// Static functions
//=================================================================================================

void createShader()
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

    const qreal matrix[12] =
    {
        1.164383561643836,  0.000000000000000,  1.792741071428571, -0.972945075016308,
        1.164383561643836, -0.213248614273730, -0.532909328559444,  0.301482665475862,
        1.164383561643836,  2.112401785714286,  0.000000000000000, -1.133402217873451,
    };

    glGenProgramsARB(1, &shaderId);

    glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, shaderId);

    glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB,
                       GL_PROGRAM_FORMAT_ASCII_ARB, strlen(code), (const GLbyte *) code);

    GLfloat * values = shaderValues;

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (j == 3)
            {
                 values[j] = shaderOpacity;
            }
            else values[j] = matrix[j * 4 + i];
        }

        glProgramLocalParameter4fvARB(GL_FRAGMENT_PROGRAM_ARB, i, values);

        values += 4;
    }
}

#endif

#endif // SK_NO_QML

//=================================================================================================
// WBackendVlcPrivate
//=================================================================================================

WBackendVlcPrivate::WBackendVlcPrivate(WBackendVlc * p) : WAbstractBackendPrivate(p) {}

/* virtual */ WBackendVlcPrivate::~WBackendVlcPrivate()
{
    delete player;

#if defined(QT_4) && defined(SK_NO_QML) == false
    deleteShader();

    if (textureIds[0]) glDeleteTextures(3, textureIds);
#endif
}

//-------------------------------------------------------------------------------------------------

void WBackendVlcPrivate::init()
{
    Q_Q(WBackendVlc);

    player = wControllerMedia->createVlcPlayer();

    width  = -1;
    height = -1;

    frameWidth  = -1;
    frameHeight = -1;

    frameSwap  = false;
    frameIndex = false;

#if defined(QT_4) && defined(SK_NO_QML) == false
    targetX      = 0.f;
    targetY      = 0.f;
    targetWidth  = 0.f;
    targetHeight = 0.f;

    shader = false;

    textureIds[0] = 0;
#endif

    started = false;
    active  = false;
    playing = false;

    frameReset   = false;
    frameUpdated = false;
    frameFreeze  = false;

    length = 0;

    // FIXME VLC: This forces our default volume instead of the saved one.
    //WAbstractBackendPrivate::volume = -1.0;

    //volume = 100;
    //mute   = false;

    loop = false;

    closestOutput  = WAbstractBackend::OutputNone;
    closestQuality = WAbstractBackend::QualityDefault;

    ratio = Qt::KeepAspectRatio;

    indexOutput = -1;

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

    // NOTE: Video range goes full, black 16 and white 235 becoming 0 and 255 as the video shader.
    for (int i = 0; i < 256; i++)
    {
        gamma[i] = qBound(0, qRound((i - 16) * 255.0 / 219), 255);
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
        tableRgb[BLUE_OFFSET + 256         + i] = qRgb(0, 0, gamma[255]);
    }

    for (int i = 0; i < 256; i++)
    {
        tableRgb[RED_OFFSET   + i] = qRgb(gamma[i], 0, 0);
        tableRgb[GREEN_OFFSET + i] = qRgb(0, gamma[i], 0);
        tableRgb[BLUE_OFFSET  + i] = qRgb(0, 0, gamma[i]);
    }
}

//-------------------------------------------------------------------------------------------------

#if defined(QT_4) && defined(SK_NO_QML) == false

void WBackendVlcPrivate::initShader()
{
    if (shaderCount)
    {
        shader = true;

        shaderCount++;

        return;
    }

#ifdef Q_OS_WIN
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
    else qWarning("WBackendVlc initShader: Fragment shaders are not supported.");
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

#endif

//-------------------------------------------------------------------------------------------------

void WBackendVlcPrivate::convertFrameSoftware()
{
    if (frameReset) return;

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

#ifdef CAN_COMPILE_SSE2

void WBackendVlcPrivate::convertFrameSse()
{
    if (frameReset) return;

    uint32_t * p_buffer = (uint32_t *) frameSoftware.bits();

    uint8_t * p_y = textures[0].bits;
    uint8_t * p_u = textures[1].bits;
    uint8_t * p_v = textures[2].bits;

    int rewind = -(textures[0].width) & 15;

    if (0 == (15 & (textures[0].width | ((intptr_t) p_y) | ((intptr_t) p_buffer))))
    {
        for (int y = 0; y < textures[0].height; y++)
        {
            for (int x = textures[0].width / 16; x--;)
            {
                SSE2_CALL(SSE2_INIT_32_ALIGNED
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

                SSE2_CALL(SSE2_INIT_32_UNALIGNED
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
                SSE2_CALL(SSE2_INIT_32_UNALIGNED
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

                SSE2_CALL(SSE2_INIT_32_UNALIGNED
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

#elif defined(__ARM_NEON)

// NOTE: Inspired from the following: https://github.com/dwy838184016/ImageDecoder
void WBackendVlcPrivate::convertFrameNeon()
{
    if (frameReset) return;

    // Get pointers to the YUV frame data
    uint8_t * p_buffer = (uint8_t *) frameSoftware.bits();

    uint8_t * p_y = textures[0].bits;
    uint8_t * p_u = textures[1].bits;
    uint8_t * p_v = textures[2].bits;

    // Get frame dimensions
    int width  = textures[0].width;
    int height = textures[0].height;

    int row = width * 4;

    // Determine the number of 16-pixel blocks and remaining pixels
    //int count = width % 16; // Remaining pixels that don’t fit into NEON processing
    int n = width / 16; // Number of full 16-pixel blocks

    // Get texture pitch (stride) values
    int pitchY = textures[0].pitch;
    int pitchU = textures[1].pitch;
    int pitchV = textures[2].pitch;

    // NEON constants for YUV to RGB conversion
    uint8x8_t all_yfac = vdup_n_u8  (298 >> 2);
    uint8x8_t t_yfac   = vdup_n_u8  (16);
    // NOTE: These are the BT.709 coefficients of the video shader.
    int16x4_t g_ufac   = vdup_n_s16 (55  >> 2);
    int16x4_t b_ufac   = vdup_n_s16 (541 >> 2);
    int16x4_t r_vfac   = vdup_n_s16 (459 >> 2);
    int16x4_t g_vfac   = vdup_n_s16 (136 >> 2);
    int16x8_t addfac   = vdupq_n_s16(128);

    // Process each row of the image
    for (int i = 0; i < height; i++)
    {
        uint8_t *n_py = p_y;
        uint8_t *n_pu = p_u;
        uint8_t *n_pv = p_v;

        uint8_t *dest_rgb = p_buffer;

        // Process 16 pixels at a time using NEON
        for (int j = 0; j < n; j++)
        {
            uint8x8x2_t  y = vld2_u8(n_py); // Load 16 Y values interleaved
            uint8x8_t uRow = vld1_u8(n_pu); // Load 8 U values
            uint8x8_t vRow = vld1_u8(n_pv); // Load 8 V values

            // Convert U and V to signed 16-bit and subtract 128
            int16x8_t u = vsubq_s16(vreinterpretq_s16_u16(vmovl_u8(uRow)), addfac);
            int16x8_t v = vsubq_s16(vreinterpretq_s16_u16(vmovl_u8(vRow)), addfac);

            // Subtract 16 from Y values
            y.val[0] = vqsub_u8(y.val[0], t_yfac);
            y.val[1] = vqsub_u8(y.val[1], t_yfac);

            // Scale Y values
            int16x8x2_t y_data;

            y_data.val[0] = vreinterpretq_s16_u16(vmull_u8(y.val[0], all_yfac));
            y_data.val[1] = vreinterpretq_s16_u16(vmull_u8(y.val[1], all_yfac));

            uint8x8x2_t r, g, b;

            // Compute R, G, B for both 8-pixel sets
            for (int k = 0; k < 2; k++)
            {
                int16x4_t tmpDataH = vmla_s16(vget_high_s16(y_data.val[k]), vget_high_s16(v), r_vfac);
                int16x4_t tmpDataL = vmla_s16(vget_low_s16(y_data.val[k]), vget_low_s16(v), r_vfac);
                r.val[k] = vqshrun_n_s16(vcombine_s16(tmpDataL, tmpDataH), 6);

                tmpDataH = vmls_s16(vget_high_s16(y_data.val[k]), vget_high_s16(u), g_ufac);
                tmpDataH = vmls_s16(tmpDataH, vget_high_s16(v), g_vfac);
                tmpDataL = vmls_s16(vget_low_s16(y_data.val[k]), vget_low_s16(u), g_ufac);
                tmpDataL = vmls_s16(tmpDataL, vget_low_s16(v), g_vfac);
                g.val[k] = vqshrun_n_s16(vcombine_s16(tmpDataL, tmpDataH), 6);

                tmpDataH = vmla_s16(vget_high_s16(y_data.val[k]), vget_high_s16(u), b_ufac);
                tmpDataL = vmla_s16(vget_low_s16(y_data.val[k]), vget_low_s16(u), b_ufac);
                b.val[k] = vqshrun_n_s16(vcombine_s16(tmpDataL, tmpDataH), 6);
            }

            // Interleave R, G, B and store results
            uint8x8x2_t r_data = vzip_u8(r.val[0], r.val[1]);
            uint8x8x2_t g_data = vzip_u8(g.val[0], g.val[1]);
            uint8x8x2_t b_data = vzip_u8(b.val[0], b.val[1]);

            uint8x8x4_t resultRgb;

            for (int k = 0; k < 2; k++)
            {
                resultRgb.val[0] = b_data.val[k];
                resultRgb.val[1] = g_data.val[k];
                resultRgb.val[2] = r_data.val[k];
                resultRgb.val[3] = vdup_n_u8(255); // Alpha

                vst4_u8(dest_rgb, resultRgb);

                dest_rgb += 32;
            }

            n_py += 16;
            n_pu +=  8;
            n_pv +=  8;
        }

        // // Process remaining pixels (scalar fallback)
        // for (int j = 0; j < count; j += 2)
        // {
        //     int u = n_pu[0] - 128;
        //     int v = n_pv[0] - 128;

        //     for (int k = 0; k < 2; k++)
        //     {
        //         int y = n_py[k] - 16;

        //         // Convert YUV to RGB
        //         int r = (298 * y + 409 * v)           >> 8;
        //         int g = (298 * y - 100 * u - 208 * v) >> 8;
        //         int b = (298 * y + 516 * u)           >> 8;

        //         int index = k * 4;

        //         // Clamp values and store
        //         dest_rgb[index + 0] = (b < 0) ? 0 : (b > 255 ? 255 : b);
        //         dest_rgb[index + 1] = (g < 0) ? 0 : (g > 255 ? 255 : g);
        //         dest_rgb[index + 2] = (r < 0) ? 0 : (r > 255 ? 255 : r);
        //         dest_rgb[index + 3] = 0xFF; // Alpha
        //     }

        //     n_py += 2;
        //     n_pu += 1;
        //     n_pv += 1;

        //     dest_rgb += 8;
        // }

        // Move to the next row
        p_buffer += row;

        p_y += pitchY;

        // Update U/V row pointers every other row (since U and V are subsampled)
        if (i & 1)
        {
            p_u += pitchU;
            p_v += pitchV;
        }
    }
}

#endif

//-------------------------------------------------------------------------------------------------

void WBackendVlcPrivate::loadSources(bool play)
{
    if (reply) return;

    Q_Q(WBackendVlc);

    qDebug("Loading Source %s", source.C_STR);

    WAbstractBackend::SourceMode mode = q->getMode();

    // NOTE: When using Chromecast for video we want to increase source compatibility.
    if (outputData.type == WAbstractBackend::OutputChromecast
        &&
        mode != WAbstractBackend::SourceAudio)
    {
         reply = wControllerMedia->getMedia(source, q, WAbstractBackend::SourceSafe);
    }
    else reply = wControllerMedia->getMedia(source, q, mode);

    if (reply == NULL) return;

    if (reply->isLoaded())
    {
        applySources(reply, play);

        delete reply;

        reply = NULL;
    }
    else QObject::connect(reply, SIGNAL(loaded(WMediaReply *)), q, SLOT(onLoaded()));
}

void WBackendVlcPrivate::applySources(const WMediaReply * reply, bool play)
{
    Q_Q(WBackendVlc);

    medias = reply->medias();
    audios = reply->audios();

    options = reply->options();

    q->setVbml(reply->isVbml());

    loop = (reply->typeSource() == WTrack::Hub);

    if (applyQuality(quality))
    {
        currentMedia = medias.value(closestQuality);
        currentAudio = audios.value(closestQuality);

        applyOutput(getOutput(output));

        if (play) playMedia();

        qDebug("Current source [%s] %d %s", currentMedia.C_STR,
                                            reply->medias().count(), reply->error().C_STR);
    }
    else
    {
        closestOutput = WAbstractBackend::OutputNone;

        q->stop();
    }

    emit q->loaded();
}

//-------------------------------------------------------------------------------------------------

void WBackendVlcPrivate::applyOutput(WAbstractBackend::Output output)
{
    if (output == WAbstractBackend::OutputNone || closestOutput == output) return;

    closestOutput = output;

    player->setOutput(output);
}

bool WBackendVlcPrivate::applyQuality(WAbstractBackend::Quality quality)
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

// FIXME VLC 3.0.20: When the seeking point is incorrect on a live feed the player seems to
//                   bufferize randomly.
void WBackendVlcPrivate::playAt(int time)
{
    if (live)
    {
        player->play();
    }
    else player->play(time);
}

void WBackendVlcPrivate::playMedia()
{
    Q_Q(WBackendVlc);

    player->setSource(currentMedia, currentAudio, options, loop);

    q->setOutputActive (closestOutput);
    q->setQualityActive(closestQuality);

    if (currentTime == -1)
    {
        player->play();

        q->setStateLoad(WAbstractBackend::StateLoadStarting);
    }
    else
    {
        playAt(currentTime);

        q->setStateLoad(WAbstractBackend::StateLoadResuming);
    }
}

void WBackendVlcPrivate::reload()
{
    Q_Q(WBackendVlc);

    // NOTE: When the currentMedia is empty we skip this call. This is useful when using a
    //       WHookOutput.
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

void WBackendVlcPrivate::updateTargetRect()
{
    if (frameWidth == -1) return;

    QSizeF frameSize = QSizeF(frameWidth, frameHeight);

    frameSize.scale(size, ratio);

    qreal width  = frameSize.width ();
    qreal height = frameSize.height();

    qreal x = (size.width () - width)  / 2;
    qreal y = (size.height() - height) / 2;

    targetRect = QRectF(x, y, width, height);

#if defined(QT_4) && defined(SK_NO_QML) == false
    targetX = x;
    targetY = y;

    targetWidth  = x + width;
    targetHeight = y + height;
#endif
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

void WBackendVlcPrivate::clearActive()
{
    Q_Q(WBackendVlc);

    q->setOutputActive (WAbstractBackend::OutputNone);
    q->setQualityActive(WAbstractBackend::QualityDefault);
}

//-------------------------------------------------------------------------------------------------

void WBackendVlcPrivate::clearReply()
{
    if (reply == NULL) return;

    delete reply;

    reply = NULL;
}

void WBackendVlcPrivate::clearMedia()
{
    clearReply();

    currentMedia = QString();
    currentAudio = QString();
}

void WBackendVlcPrivate::clearSources()
{
    currentMedia = QString();
    currentAudio = QString();

    medias.clear();
    audios.clear();
}

//-------------------------------------------------------------------------------------------------

#if defined(QT_4) && defined(SK_NO_QML) == false

void WBackendVlcPrivate::setOpacity(GLfloat opacity)
{
    if (shaderOpacity == opacity) return;

    shaderOpacity = opacity;

    GLfloat * values = shaderValues;

    for (int i = 0; i < 4; i++)
    {
        values[3] = opacity;

        glProgramLocalParameter4fvARB(GL_FRAGMENT_PROGRAM_ARB, i, values);

        values += 4;
    }
}

#endif

//-------------------------------------------------------------------------------------------------

/*void WBackendVlcPrivate::setMute(bool enabled)
{
    if (mute == enabled) return;

    mute = enabled;

    if (enabled)
    {
         player->setVolume(0);
    }
    else player->setVolume(volume);
}*/

//-------------------------------------------------------------------------------------------------

WAbstractBackend::Output WBackendVlcPrivate::getOutput(WAbstractBackend::Output output)
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

/* static */ unsigned WBackendVlcPrivate::setup(void ** data, char * chroma, unsigned * vlcWidth,
                                                                             unsigned * vlcHeight,
                                                                             unsigned * pitches,
                                                                             unsigned * lines)
{
    WBackendVlcPrivate * d = static_cast<WBackendVlc *> (*data)->d_func();

    WVlcPlayerPrivate * p = d->player->d_func();

    p->applySize(vlcWidth, vlcHeight);

    *vlcWidth  = qMin((int) (*vlcWidth),  PLAYER_MAX_WIDTH);
    *vlcHeight = qMin((int) (*vlcHeight), PLAYER_MAX_HEIGHT);

    int width  = *vlcWidth;
    int height = *vlcHeight;

    if (width == 0 || height == 0) return 0;

    // NOTE VLC: When the chroma is set to I0AL it usually means HDR.
    /*if (strncmp(chroma, "I0AL", 4) == 0)
    {
         d->setHdr(true);
    }
    else d->setHdr(false);*/

    memcpy(chroma, "I420", 4);

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

    // NOTE: Sometimes the same size is requested several times.
    if (d->width == width && d->height == height) return 1;

    d->width  = width;
    d->height = height;

    int cursorU = pitches[0] * lines[0];

    int cursorV = cursorU + pitches[1] * lines[1];

    // NOTE: We make sure we're not writing a new frame while applying it on a texture.
    d->mutex.lock();

    // NOTE: We need to swap image(s) in case we're still reading bits from the rendering thread.
    if (d->frameSwap)
    {
        d->frames[0] = QImage(width, height, QImage::Format_RGB16);
        d->frames[1] = QImage(width, height, QImage::Format_RGB16);

        applyFrames(d, d->frames[0], d->frames[1], cursorU, cursorV);
    }
    else
    {
        d->frames[2] = QImage(width, height, QImage::Format_RGB16);
        d->frames[3] = QImage(width, height, QImage::Format_RGB16);

        applyFrames(d, d->frames[2], d->frames[3], cursorU, cursorV);
    }

    d->mutex.unlock();

    d->frameIndex = false;

#ifdef QT_OLD
    QCoreApplication::postEvent(d->q_func(),
                                new WBackendVlcEventSetup(width, height,
                                                          pitches[0], pitches[1], pitches[2]));
#else
    QCoreApplication::postEvent(d->q_func(),
                                new WBackendVlcEventSetup(width, height,
                                                          pitches[0], pitches[1], pitches[2],
                                                          cursorU, cursorV));
#endif

    return 1;
}

//-------------------------------------------------------------------------------------------------

/* static */ void * WBackendVlcPrivate::lock(void * data, void ** buffer)
{
    WBackendVlcPrivate * d = static_cast<WBackendVlc *> (data)->d_func();

    // NOTE: We use a pool of textures, so we should not need this.
    //d->mutex.lock();

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

    //d->mutex.unlock();

    d->method.invoke(backend);
}

//-------------------------------------------------------------------------------------------------

/* static */ void WBackendVlcPrivate::applyFrames(WBackendVlcPrivate * d,
                                                  QImage & frameA, QImage & frameB,
                                                  int cursorU, int cursorV)
{
    d->textures[0].bitsA = frameA.bits();
    d->textures[1].bitsA = frameA.bits() + cursorU;
    d->textures[2].bitsA = frameA.bits() + cursorV;

    d->textures[0].bitsB = frameB.bits();
    d->textures[1].bitsB = frameB.bits() + cursorU;
    d->textures[2].bitsB = frameB.bits() + cursorV;

    d->textures[0].bits = d->textures[0].bitsA;
    d->textures[1].bits = d->textures[1].bitsA;
    d->textures[2].bits = d->textures[2].bitsA;
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

        emit q->loaded();
    }
    else applySources(reply, q->isPlaying());

    reply->deleteLater();

    reply = NULL;
}

//-------------------------------------------------------------------------------------------------

void WBackendVlcPrivate::onFrameUpdated()
{
    Q_Q(WBackendVlc);

    frameUpdated = true;

    q->applyFrame();
}

//-------------------------------------------------------------------------------------------------

#ifdef QT_NEW

void WBackendVlcPrivate::onUpdateState()
{
    if (started == false) return;

    Q_Q(WBackendVlc);

    q->setStateLoad(WAbstractBackend::StateLoadDefault);
}

#endif

//=================================================================================================
// WBackendVlc
//=================================================================================================

WBackendVlc::WBackendVlc(QObject * parent) : WAbstractBackend(new WBackendVlcPrivate(this), parent)
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

    d->player->setProxy(host + ':' + QString::number(port), password);

    d->clearSources();
}

/* Q_INVOKABLE */ void WBackendVlc::clearProxy()
{
    Q_D(WBackendVlc);

    d->clearReply();

    d->player->clearProxy();

    d->clearSources();
}

//-------------------------------------------------------------------------------------------------
// Protected WAbstractBackend implementation
//-------------------------------------------------------------------------------------------------

#if defined(QT_NEW) && defined(SK_NO_QML) == false

/* virtual */ WBackendNode * WBackendVlc::backendCreateNode() const
{
    return new WBackendYuvNode;
}

#endif

/* virtual */ bool WBackendVlc::backendSetSource(const QString & url, const WMediaReply * reply)
{
    Q_D(WBackendVlc);

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

        d->player->pause();

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

/* virtual */ bool WBackendVlc::backendPlay()
{
    Q_D(WBackendVlc);

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
         d->playAt(d->currentTime);
    }
    else d->player->play();

    return true;
}

/* virtual */ bool WBackendVlc::backendPause()
{
    Q_D(WBackendVlc);

    if (d->started)
    {
        // FIXME VLC 3.0.16: Pause does not seem to work on a live stream.
        /*if (d->live)
        {
             d->player->stop();
        }
        else */d->player->pause();
    }
    // FIXME VLC: Muting the sound to avoid the audio glitch.
    //else d->setMute(true);

    return true;
}

/* virtual */ bool WBackendVlc::backendStop()
{
    Q_D(WBackendVlc);

    d->clearPlayer();

    d->player->stop();

    d->clearActive();

    return true;
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WBackendVlc::backendSetVolume(qreal volume)
{
    Q_D(WBackendVlc);

    // NOTE VLC: We don't want the volume to lower too quickly for human ears.
    if (volume)
    {
        volume = qPow(volume, 0.6);

        d->player->setVolume(qRound(volume * 100));
    }
    else d->player->setVolume(0);

    /*if (volume)
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
    }*/
}

//-------------------------------------------------------------------------------------------------

/* virtual */ bool WBackendVlc::backendDelete()
{
    Q_D(WBackendVlc);

    d->clearMedia ();
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

    if (d->stateLoad != StateLoadDefault) return;

    // NOTE: Sometimes the buffering event is not sent or delayed, so we apply it manually.
    setStateLoad(StateLoadBuffering);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WBackendVlc::backendSetSpeed(qreal speed)
{
    Q_D(WBackendVlc);

    d->player->setSpeed(speed);
}

//-------------------------------------------------------------------------------------------------

// NOTE: When switching the output we want to reload sources in case they are different.
/* virtual */ void WBackendVlc::backendSetOutput(Output)
{
    Q_D(WBackendVlc); d->reload();
}

/* virtual */ void WBackendVlc::backendSetQuality(Quality quality)
{
    Q_D(WBackendVlc);

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

    QString string = qualityToString(quality);

    if (string == "default")
    {
         d->player->setQuality("");
    }
    else d->player->setQuality(string);

    if (hasStarted() == false) return;

    d->started = false;

    d->frameFreeze = true;

    d->onFrameUpdated();

    d->player->setSource(d->currentMedia, d->currentAudio, d->options, d->loop);

    if (d->state == StatePlaying)
    {
        d->playAt(d->currentTime);
    }
    else if (d->state == StatePaused)
    {
        //d->setMute(true);

        d->playAt(d->currentTime);
    }
}

/* virtual */ void WBackendVlc::backendSetSourceMode(SourceMode)
{
    Q_D(WBackendVlc); d->reload();
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WBackendVlc::backendSetFillMode(FillMode fillMode)
{
    Q_D(WBackendVlc);

    d->ratio = static_cast<Qt::AspectRatioMode> (fillMode);

    d->updateTargetRect();
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WBackendVlc::backendSetVideo(int id)
{
    Q_D(WBackendVlc);

    if (id == -1) return;

    d->player->setVideo(id);
}

/* virtual */ void WBackendVlc::backendSetAudio(int id)
{
    Q_D(WBackendVlc);

    if (id == -1) return;

    d->player->setAudio(id);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WBackendVlc::backendSetScanOutput(bool enabled)
{
    Q_D(WBackendVlc);

    d->player->setScanOutput(enabled);
}

/* virtual */ void WBackendVlc::backendSetCurrentOutput(const WBackendOutput * output)
{
    Q_D(WBackendVlc);

    int index = d->outputs.indexOf(output);

    if (d->indexOutput == index) return;

    d->indexOutput = index;

    d->player->setOutput(index);

    // NOTE: When the currentMedia is empty we skip this call. This is useful when using a
    //       WHookOutput.
    if (d->currentMedia.isEmpty() || hasStarted() == false) return;

    //---------------------------------------------------------------------------------------------
    // NOTE: We reload sources when switching between outputs. That's required for Chromecast
    //       compatiblity mode.

    d->clearReply  ();
    d->clearSources();

    d->loadSources(isPlaying());
}

/* virtual */ void WBackendVlc::backendSetAdjust(const WBackendAdjust & adjust)
{
    Q_D(WBackendVlc);

    d->player->setAdjust(adjust);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WBackendVlc::backendSetSize(const QSizeF &)
{
    Q_D(WBackendVlc);

    d->updateTargetRect();
}

//-------------------------------------------------------------------------------------------------

#if defined(QT_NEW) && defined(SK_NO_QML) == false

/* virtual */ void WBackendVlc::backendSynchronize(WBackendFrame * frame)
{
    Q_D(WBackendVlc);

    if (d->active == false)
    {
        frame->state = WAbstractBackend::FrameClear;
    }
    else if (d->frameUpdated && d->frameFreeze == false)
    {
        if (d->frameReset)
        {
            // NOTE: If the frame has already changed in 'setup' we wait for the next bits.
            if (d->width != d->frameWidth || d->height != d->frameHeight)
            {
                qDebug("INVALID FRAME");

                return;
            }

            d->frameUpdated = false;

            d->frameReset = false;

            WBackendTexture * textures = frame->textures;

            d->mutex.lock();

            for (int i = 0; i < 3; i++)
            {
                WBackendTexture    * textureA = &(textures[i]);
                WBackendVlcTexture * textureB = &(d->textures[i]);

#ifdef QT_OLD
                textureA->width  = textureB->width;
                textureA->height = textureB->height;
#else
                textureA->size = QSize(textureB->width, textureB->height);
#endif

                textureA->bits = textureB->bits;

#ifdef QT_6
                textureA->length = textureB->length;
                textureA->pitch  = textureB->pitch;
#endif
            }

            d->frameSwap = !(d->frameSwap);

#ifdef SK_SOFTWARE
            // FIXME Qt5: SSE does not seem to work.
            d->convertFrameSoftware();
#endif

            d->mutex.unlock();

            frame->state = WAbstractBackend::FrameReset;
        }
        else
        {
            d->frameUpdated = false;

            WBackendTexture * textures = frame->textures;

            // NOTE: We use a pool of textures, so we should not need this.
            //d->mutex.lock();

            textures[0].bits = d->textures[0].bits;
            textures[1].bits = d->textures[1].bits;
            textures[2].bits = d->textures[2].bits;

#ifdef SK_SOFTWARE
            // FIXME Qt5: SSE does not seem to work.
            d->convertFrameSoftware();
#endif

            //d->mutex.unlock();

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

#ifdef QT_4
/* virtual */ void WBackendVlc::backendDrawFrame(QPainter * painter, const QRect & rect)
#else
/* virtual */ void WBackendVlc::backendDrawFrame(QPainter * painter, const QRect &)
#endif
{
    Q_D(WBackendVlc);

    if (d->active == false) return;

#ifdef QT_4
#ifdef SK_NO_QML
    Q_UNUSED(rect)
#else
    if (painter->paintEngine()->type() == QPaintEngine::OpenGL2)
    {
        if (d->frameUpdated && d->frameFreeze == false)
        {
            d->frameUpdated = false;

            if (d->frameReset
                &&
                // NOTE: If the frame has already changed in 'setup' we wait for the next bits.
                d->width == d->frameWidth && d->height == d->frameHeight)
            {
                d->frameReset = false;

                d->initShader();

                if (d->textureIds[0] != 0)
                {
                    glDeleteTextures(3, d->textureIds);
                }

                glGenTextures(3, d->textureIds);

                //d->mutex.lock();

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

                d->frameSwap = !(d->frameSwap);

                //d->mutex.unlock();

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
            }

            d->setOpacity(painter->opacity());
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
        else return;

        qreal width  = rect.width ();
        qreal height = rect.height();

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

                W_DRAW_FRAME;

                glDisable(GL_SCISSOR_TEST);
            }
            else W_DRAW_FRAME;
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
#endif
#endif // QT_4
    {
#ifdef QT_4
        if (d->frameFreeze == false) d->convertFrameSse();
#endif

        bool smooth = painter->testRenderHint(QPainter::SmoothPixmapTransform);

        painter->setRenderHint(QPainter::SmoothPixmapTransform);

        painter->drawImage(d->targetRect, d->frameSoftware);

        painter->setRenderHint(QPainter::SmoothPixmapTransform, smooth);
    }
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WBackendVlc::backendUpdateFrame()
{
    Q_D(WBackendVlc);

#ifdef CAN_COMPILE_SSE2
    d->convertFrameSse();
#elif defined(__ARM_NEON)
    d->convertFrameNeon();
#else
    d->convertFrameSoftware();
#endif
}

/* virtual */ void WBackendVlc::backendNextFrame()
{
    Q_D(WBackendVlc);

    d->player->nextFrame();

    qreal frameRate = this->frameRate();

    if (frameRate == 0.0) return;

    int msec = currentTime() + qRound(1000 / frameRate);

    int duration = this->duration();

    if (duration > 0) msec = qMin(msec, duration);

    setCurrentTime(msec);
}

/* virtual */ QImage WBackendVlc::backendGetFrame() const
{
    Q_D(const WBackendVlc);

    if (d->frameWidth == -1)
    {
        return QImage();
    }
    else return d->frameSoftware;
}

/* virtual */ QImage WBackendVlc::backendGetFrameGray() const
{
#ifdef QT_4
    return backendGetFrame();
#else
    Q_D(const WBackendVlc);

    if (d->frameWidth == -1) return QImage();

    const WBackendVlcTexture & texture = d->textures[0];

    return QImage(texture.bits, texture.width, texture.height, QImage::Format_Grayscale8);
#endif
}

//-------------------------------------------------------------------------------------------------

/* virtual */ QRectF WBackendVlc::backendRect() const
{
    Q_D(const WBackendVlc);

    return d->targetRect;
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

        qDebug("SETUP %d %d", width, height);

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

#ifdef QT_6
        d->textures[0].length = setup->cursorU;
        d->textures[1].length = setup->cursorV - setup->cursorU;
        d->textures[2].length = d->textures[1].length;
#endif

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

            //d->setMute(false);
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

        WVlcPlayerEvent * eventPlayer = static_cast<WVlcPlayerEvent *> (event);

        int progress = eventPlayer->value.toInt();

        // NOTE: When playing a hub, we want to avoid buffering while restarting the video.
        if (d->loop) return true;

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
    else if (type == static_cast<QEvent::Type> (WVlcPlayer::EventLengthChanged))
    {
        Q_D(WBackendVlc);

        WVlcPlayerEvent * eventPlayer = static_cast<WVlcPlayerEvent *> (event);

        int length = eventPlayer->value.toInt();

        d->length = length;

        // FIXME VLC 3.0.20: When playing m3u(s) we might get a duration of 0.
        if (length) setDuration(length);

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
#ifdef QT_NEW
            // FIXME Qt5: Waiting for the first frame.
            QTimer::singleShot(64, this, SLOT(onUpdateState()));

            return true;
        }
        else setStateLoad(StateLoadDefault);
#else
        }

        setStateLoad(StateLoadDefault);
#endif

        int time = eventPlayer->value.toInt();

        setCurrentTime(time);

        if (time > d->length && d->live == false)
        {
            // FIXME VLC 3.0.20: When the seeking point is incorrect on a live feed the player
            //                   seems to bufferize randomly.
            //d->player->seek(0);

            setLive(true);
        }

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcPlayer::EventEndReached))
    {
        Q_D(WBackendVlc);

        if (d->started)
        {
            // NOTE: We repeat at the lowest level possible, instead of relying on the WPlayer.
            if (d->repeat)
            {
                // NOTE VLC 4.0.0: Setting the source does not seem to be required.
#if LIBVLC_VERSION_MAJOR < 4
                d->player->setSource(d->currentMedia, d->currentAudio, d->options, d->loop);
#endif

                d->player->play(0);

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
    else if (type == static_cast<QEvent::Type> (WVlcPlayer::EventTracks))
    {
        WVlcTracksEvent * eventTracks = static_cast<WVlcTracksEvent *> (event);

        applyTracks(eventTracks->tracks, eventTracks->trackVideo, eventTracks->trackAudio);

        setFrameRate(eventTracks->frameRate);

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcPlayer::EventOutputAdd))
    {
        Q_D(WBackendVlc);

        WVlcOutputsEvent * eventOutputs = static_cast<WVlcOutputsEvent *> (event);

        foreach (const WBackendOutput & output, eventOutputs->outputs)
        {
            d->outputs.append(addOutput(output));
        }

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcPlayer::EventOutputRemove))
    {
        Q_D(WBackendVlc);

        WVlcPlayerEvent * eventPlayer = static_cast<WVlcPlayerEvent *> (event);

        const WBackendOutput * output = d->outputs.takeAt(eventPlayer->value.toInt());

        removeOutput(output);

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcPlayer::EventOutputClear))
    {
        Q_D(WBackendVlc);

        foreach (const WBackendOutput * output, d->outputs)
        {
            removeOutput(output);
        }

        d->outputs.clear();

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcPlayer::EventError))
    {
        Q_D(WBackendVlc);

        stopError("Vlc player error");

        // NOTE: We clear sources to check their validity when we resume playback.
        d->clearSources();

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
