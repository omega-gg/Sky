//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

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
#if defined(QT_5) && defined(SK_NO_QML) == false
#include <QOpenGLFunctions>
#endif
#if defined(QT_6) || defined(SK_NO_QML)
#include <QPainter>
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
#if defined(QT_6) == false && defined(Q_OS_MACX)
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#endif

// Linux includes
#if defined(QT_6) == false && defined(Q_OS_LINUX) && defined(Q_OS_ANDROID) == false
#include <GL/glx.h>
#endif

#endif // SK_NO_QML

// Private includes
#include <private/WVlcPlayer_p>

//-------------------------------------------------------------------------------------------------
// Defines
//-------------------------------------------------------------------------------------------------
// RGB

#define RED_MARGIN    178
#define GREEN_MARGIN  135
#define BLUE_MARGIN   224
#define RED_OFFSET   1501
#define GREEN_OFFSET  135
#define BLUE_OFFSET   818

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

#ifndef SK_NO_QML

//-------------------------------------------------------------------------------------------------
// Opengl

#if defined(QT_4) && defined(Q_OS_MACX) == false
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

//-------------------------------------------------------------------------------------------------

#define W_CREATE_TEXTURE(Unit, Id, Width, Height, Bits)                                        \
{                                                                                              \
    gl->glActiveTexture(Unit);                                                                 \
                                                                                               \
    gl->glBindTexture(GL_TEXTURE_2D, Id);                                                      \
                                                                                               \
    gl->glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, Width, Height, 0, GL_LUMINANCE,           \
                     GL_UNSIGNED_BYTE, 0);                                                     \
                                                                                               \
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);                      \
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);                      \
                                                                                               \
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);                   \
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);                   \
                                                                                               \
    gl->glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, Width, Height, GL_LUMINANCE, GL_UNSIGNED_BYTE, \
                        Bits);                                                                 \
}                                                                                              \

#define W_UPDATE_TEXTURE(Unit, Id, Width, Height, Bits)                                        \
{                                                                                              \
    gl->glActiveTexture(Unit);                                                                 \
                                                                                               \
    gl->glBindTexture(GL_TEXTURE_2D, Id);                                                      \
                                                                                               \
    gl->glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, Width, Height, GL_LUMINANCE, GL_UNSIGNED_BYTE, \
                        Bits);                                                                 \
}                                                                                              \

#define W_BIND_TEXTURE(Unit, Id)          \
{                                         \
    gl->glActiveTexture(Unit);            \
                                          \
    gl->glBindTexture(GL_TEXTURE_2D, Id); \
}                                         \

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

#else

static QSGMaterialType materialType;

#ifdef QT_5
static const char * shaderNames[] = { "vertex", "fragment", 0 };
#endif

static const QMatrix4x4 matrix
(
    1.164383561643836,  0.000000000000000,  1.792741071428571, -0.972945075016308,
    1.164383561643836, -0.213248614273730, -0.532909328559444,  0.301482665475862,
    1.164383561643836,  2.112401785714286,  0.000000000000000, -1.133402217873451,
    0.000000000000000,  0.000000000000000,  0.000000000000000,  1.000000000000000
);

#endif

#ifdef QT_4

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

#else

//=================================================================================================
// WBackendVlcShader
//=================================================================================================

#ifdef QT_6

WBackendVlcShader::WBackendVlcShader()
{
    isNew = true;

    if (sk->isQrc())
    {
        setShaderFileName(VertexStage,   ":/shaders/video.vert.qsb");
        setShaderFileName(FragmentStage, ":/shaders/video.frag.qsb");
    }
    else
    {
#ifdef Q_OS_MACX
        QString path = QCoreApplication::applicationDirPath() + "/../../../";
#else
        QString path = QCoreApplication::applicationDirPath() + '/';
#endif

        setShaderFileName(VertexStage,   path + "shaders/video.vert.qsb");
        setShaderFileName(FragmentStage, path + "shaders/video.frag.qsb");
    }
}

#endif

#ifdef QT_5

//-------------------------------------------------------------------------------------------------
// QSGMaterialShader implementation
//-------------------------------------------------------------------------------------------------

/* virtual */ char const * const * WBackendVlcShader::attributeNames() const
{
    return shaderNames;
}

#endif

//-------------------------------------------------------------------------------------------------
// QSGMaterialShader reimplementation
//-------------------------------------------------------------------------------------------------

#ifdef QT_5

/* virtual */ void WBackendVlcShader::updateState(const RenderState & state,
                                                  QSGMaterial       * newMaterial, QSGMaterial *)
{
    QOpenGLShaderProgram * program = this->program();

    if (state.isMatrixDirty())
    {
        program->setUniformValue(idPosition, state.combinedMatrix());
    }

    if (state.isOpacityDirty())
    {
        program->setUniformValue(idOpacity, state.opacity());
    }

    WBackendVlcMaterial * material = static_cast<WBackendVlcMaterial *> (newMaterial);

    material->updateTextures();
}

#else // QT_6

/* virtual */ bool WBackendVlcShader::updateUniformData(RenderState & state,
                                                        QSGMaterial * newMaterial,
                                                        QSGMaterial *)
{
    WBackendVlcMaterial * material = static_cast<WBackendVlcMaterial *> (newMaterial);

    if (material->update)
    {
        WTextureVideo & dataA = material->data[0];
        WTextureVideo & dataB = material->data[1];
        WTextureVideo & dataC = material->data[2];

        if (dataA.rhiTexture())
        {
            dataA.upload(state, material->textures[0]);
            dataB.upload(state, material->textures[1]);
            dataC.upload(state, material->textures[2]);
        }
        else
        {
            dataA.create(state, material->textures[0]);
            dataB.create(state, material->textures[1]);
            dataC.create(state, material->textures[2]);
        }
    }

    char * data = state.uniformData()->data();

    bool update = false;

    if (state.isMatrixDirty())
    {
        QMatrix4x4 combined = state.combinedMatrix();

        memcpy(data, &combined, 64);

        update = true;
    }

    // NOTE: The color matrix stays the same at all time, so we set it once.
    if (isNew)
    {
        isNew = false;

        memcpy(data + 64, &matrix, 64);

        update = true;
    }

    if (state.isOpacityDirty())
    {
        float opacity = state.opacity();

        memcpy(data + 128, &opacity, 4);

        update = true;
    }

    return update;
}

/* virtual */ void WBackendVlcShader::updateSampledImage(RenderState &, int binding,
                                                         QSGTexture ** texture,
                                                         QSGMaterial * newMaterial,
                                                         QSGMaterial *)
{
    WBackendVlcMaterial * material = static_cast<WBackendVlcMaterial *> (newMaterial);

    *texture = &(material->data[binding - 1]);
}

#endif // QT_6

//-------------------------------------------------------------------------------------------------
// Protected QSGMaterialShader reimplementation
//-------------------------------------------------------------------------------------------------

#ifdef QT_5

/* virtual */ void WBackendVlcShader::initialize()
{
    QOpenGLShaderProgram * program = this->program();

    idPosition = program->uniformLocation("position");
    idOpacity  = program->uniformLocation("opacity");
    idColor    = program->uniformLocation("matrix");

    idY = program->uniformLocation("y");
    idU = program->uniformLocation("u");
    idV = program->uniformLocation("v");

    program->setUniformValue(idColor, matrix);

    program->setUniformValue(idY, 0);
    program->setUniformValue(idU, 1);
    program->setUniformValue(idV, 2);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ const char * WBackendVlcShader::vertexShader() const
{
    return "attribute highp vec4 vertex;"
           "attribute highp vec2 fragment;"

           "uniform highp mat4 position;"

           "varying highp vec2 vector;"

           "void main()"
           "{"
               "gl_Position = position * vertex;"

               "vector = fragment;"
           "}";
}

/* virtual */ const char * WBackendVlcShader::fragmentShader() const
{
    return "uniform sampler2D y;"
           "uniform sampler2D u;"
           "uniform sampler2D v;"

           "uniform mediump mat4 matrix;"

           "varying highp vec2 vector;"

           "uniform lowp float opacity;"

           "void main()"
           "{"
               "highp vec4 color = vec4(texture2D(y, vector.st).r,"
                                       "texture2D(u, vector.st).r,"
                                       "texture2D(v, vector.st).r, 1.0);"

               "gl_FragColor = matrix * color * opacity;"
           "}";
}

#endif

//=================================================================================================
// WBackendVlcMaterial
//=================================================================================================

WBackendVlcMaterial::WBackendVlcMaterial()
{
#ifdef QT_5
    gl = QOpenGLContext::currentContext()->functions();
#endif

    textures = NULL;

#ifdef QT_5
    ids[0] = 0;
#endif

    update = false;

    setFlag(Blending, false);
}

/* virtual */ WBackendVlcMaterial::~WBackendVlcMaterial()
{
#ifdef QT_5
    if (ids[0] == 0) return;

    gl->glDeleteTextures(3, ids);
#endif
}

#ifdef QT_5

//-------------------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------------------

void WBackendVlcMaterial::updateTextures()
{
    if (update)
    {
        WBackendTexture * dataA = &(textures[0]);
        WBackendTexture * dataB = &(textures[1]);
        WBackendTexture * dataC = &(textures[2]);

        if (ids[0])
        {
            W_UPDATE_TEXTURE(GL_TEXTURE1, ids[1], dataB->width, dataB->height, dataB->bits);
            W_UPDATE_TEXTURE(GL_TEXTURE2, ids[2], dataC->width, dataC->height, dataC->bits);
            W_UPDATE_TEXTURE(GL_TEXTURE0, ids[0], dataA->width, dataA->height, dataA->bits);
        }
        else
        {
            gl->glGenTextures(3, ids);

            W_CREATE_TEXTURE(GL_TEXTURE1, ids[1], dataB->width, dataB->height, dataB->bits);
            W_CREATE_TEXTURE(GL_TEXTURE2, ids[2], dataC->width, dataC->height, dataC->bits);
            W_CREATE_TEXTURE(GL_TEXTURE0, ids[0], dataA->width, dataA->height, dataA->bits);
        }
    }
    else if (ids[0])
    {
        W_BIND_TEXTURE(GL_TEXTURE1, ids[1]);
        W_BIND_TEXTURE(GL_TEXTURE2, ids[2]);
        W_BIND_TEXTURE(GL_TEXTURE0, ids[0]);
    }
}

#endif

//-------------------------------------------------------------------------------------------------
// QSGMaterial implementation
//-------------------------------------------------------------------------------------------------

/* virtual */ QSGMaterialType * WBackendVlcMaterial::type() const
{
    return &materialType;
}

#ifdef QT_5
/* virtual */ QSGMaterialShader * WBackendVlcMaterial::createShader() const
#else
/* virtual */
QSGMaterialShader * WBackendVlcMaterial::createShader(QSGRendererInterface::RenderMode) const
#endif
{
    return new WBackendVlcShader;
}

//=================================================================================================
// WBackendVlcNode
//=================================================================================================

WBackendVlcNode::WBackendVlcNode() : WBackendNode()
{
    setMaterial(&material);
}

//-------------------------------------------------------------------------------------------------
// WBackendNode reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WBackendVlcNode::setTextures(WBackendTexture * textures)
{
    if (textures)
    {
        material.textures = textures;

        material.update = true;

        markDirty(QSGNode::DirtyMaterial);
    }
    else material.update = false;
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
    WAbstractBackendPrivate::volume = -1.0;

    volume = 100;
    mute   = false;

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

    player->setSource(currentMedia, currentAudio, loop);

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

    int index = libvlc_video_get_track(d->player->d_func()->player);

    libvlc_video_get_size(d->player->d_func()->player, index, vlcWidth, vlcHeight);

    *vlcWidth  = qMin((int) (*vlcWidth),  PLAYER_MAX_WIDTH);
    *vlcHeight = qMin((int) (*vlcHeight), PLAYER_MAX_HEIGHT);

    int width  = *vlcWidth;
    int height = *vlcHeight;

    if (width == 0 || height == 0) return 0;

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

    // NOTE: Sometimes the same size is requested several times.
    if (d->width == width && d->height == height) return 1;

    d->width  = width;
    d->height = height;

    int cursorU = pitches[0] * lines[0];

    int cursorV = cursorU + pitches[1] * lines[1];

    // NOTE: We make sure we're not writing a new frame while applying it on a texture.
    //d->mutex.lock();

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

    //d->mutex.unlock();

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

    d->player->setProxy(host, port, password);

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
    return new WBackendVlcNode;
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
    else d->setMute(true);

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

    d->player->setSource(d->currentMedia, d->currentAudio, d->loop);

    if (d->state == StatePlaying)
    {
        d->playAt(d->currentTime);
    }
    else if (d->state == StatePaused)
    {
        d->setMute(true);

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

            //d->mutex.lock();

            for (int i = 0; i < 3; i++)
            {
                WBackendTexture    * textureA = &(textures[i]);
                WBackendVlcTexture * textureB = &(d->textures[i]);

#ifdef QT_OLD
                textureA->width  = textureB->width;
                textureA->height = textureB->height;
#else
                int width  = textureB->width;
                int height = textureB->height;

                textureA->size = QSize(width, height);
#endif

                textureA->bits = textureB->bits;

#ifdef QT_6
                textureA->length = textureB->length;
                textureA->pitch  = textureB->pitch;
#endif
            }

            d->frameSwap = !(d->frameSwap);

            //d->mutex.unlock();

            frame->state = WAbstractBackend::FrameReset;
        }
        else
        {
            d->frameUpdated = false;

            WBackendTexture * textures = frame->textures;

            textures[0].bits = d->textures[0].bits;
            textures[1].bits = d->textures[1].bits;
            textures[2].bits = d->textures[2].bits;

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
#else
        // FIXME Qt5: SSE does not seem to work.
        if (d->frameFreeze == false) d->convertFrameSoftware();
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
#else
    d->convertFrameSoftware();
#endif
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

        // NOTE: When playing a hub, we want to avoid buffering while restarting the video.
        if (d->stateLoad == StateLoadDefault && d->loop == false)
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
        if (length != 0) setDuration(length);

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
                d->player->setSource(d->currentMedia, d->currentAudio, d->loop);

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
