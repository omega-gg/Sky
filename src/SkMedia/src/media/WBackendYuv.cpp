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

#include "WBackendYuv.h"

#if defined(QT_NEW) && defined(SK_NO_QML) == false

#ifndef SK_NO_BACKENDYUV

// Qt includes
#include <QCoreApplication>
#ifdef QT_6
#include <QtMath>
#endif

// Sk includes
#ifdef QT_6
#include <WControllerApplication>
#endif

#ifdef QT_5

//-------------------------------------------------------------------------------------------------
// Defines
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

#endif

//-------------------------------------------------------------------------------------------------
// Static variables
//-------------------------------------------------------------------------------------------------

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

//=================================================================================================
// WBackendYuvShader
//=================================================================================================

#ifdef QT_6

WBackendYuvShader::WBackendYuvShader()
{
    isNew = true;

    if (sk->isQrc())
    {
        setShaderFileName(VertexStage,   ":/shaders/video.vert.qsb");
        setShaderFileName(FragmentStage, ":/shaders/video.frag.qsb");
    }
    else
    {
#ifdef Q_OS_MACOS
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

/* virtual */ char const * const * WBackendYuvShader::attributeNames() const
{
    return shaderNames;
}

#endif

//-------------------------------------------------------------------------------------------------
// QSGMaterialShader reimplementation
//-------------------------------------------------------------------------------------------------

#ifdef QT_5

/* virtual */ void WBackendYuvShader::updateState(const RenderState & state,
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

    WBackendYuvMaterial * material = static_cast<WBackendYuvMaterial *> (newMaterial);

    material->updateTextures();
}

#else // QT_6

/* virtual */ bool WBackendYuvShader::updateUniformData(RenderState & state,
                                                        QSGMaterial * newMaterial,
                                                        QSGMaterial *)
{
    WBackendYuvMaterial * material = static_cast<WBackendYuvMaterial *> (newMaterial);

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
        memcpy(data + 64, &matrix, 64);

        update = true;
    }

    if (isNew || material->updateAdjust)
    {
        isNew = false;

        material->updateAdjust = false;

        // NOTE: Without a gamma, the color matrix goes into the adjust one, so the shader does a
        //       single product, as without adjust.
        if (material->gamma == 1.0f)
        {
            QMatrix4x4 adjust = matrix * material->matrixAdjust;

            memcpy(data + 128, &adjust, 64);
        }
        else memcpy(data + 128, &(material->matrixAdjust), 64);

        memcpy(data + 196, &(material->gamma), 4);

        update = true;
    }

    if (state.isOpacityDirty())
    {
        float opacity = state.opacity();

        memcpy(data + 192, &opacity, 4);

        update = true;
    }

    return update;
}

/* virtual */ void WBackendYuvShader::updateSampledImage(RenderState &, int binding,
                                                         QSGTexture ** texture,
                                                         QSGMaterial * newMaterial,
                                                         QSGMaterial *)
{
    WBackendYuvMaterial * material = static_cast<WBackendYuvMaterial *> (newMaterial);

    *texture = &(material->data[binding - 1]);
}

#endif // QT_6

//-------------------------------------------------------------------------------------------------
// Protected QSGMaterialShader reimplementation
//-------------------------------------------------------------------------------------------------

#ifdef QT_5

/* virtual */ void WBackendYuvShader::initialize()
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

/* virtual */ const char * WBackendYuvShader::vertexShader() const
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

/* virtual */ const char * WBackendYuvShader::fragmentShader() const
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
// WBackendYuvMaterial
//=================================================================================================

WBackendYuvMaterial::WBackendYuvMaterial()
{
#ifdef QT_5
    gl = QOpenGLContext::currentContext()->functions();
#endif

    textures = NULL;

#ifdef QT_5
    ids[0] = 0;
#else
    gamma = 1.0f;

    updateAdjust = false;
#endif

    update = false;

    setFlag(Blending, false);
}

/* virtual */ WBackendYuvMaterial::~WBackendYuvMaterial()
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

void WBackendYuvMaterial::updateTextures()
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

/* virtual */ QSGMaterialType * WBackendYuvMaterial::type() const
{
    return &materialType;
}

#ifdef QT_5
/* virtual */ QSGMaterialShader * WBackendYuvMaterial::createShader() const
#else
/* virtual */
QSGMaterialShader * WBackendYuvMaterial::createShader(QSGRendererInterface::RenderMode) const
#endif
{
    return new WBackendYuvShader;
}

//=================================================================================================
// WBackendYuvNode
//=================================================================================================

WBackendYuvNode::WBackendYuvNode() : WBackendNode()
{
    setMaterial(&material);
}

//-------------------------------------------------------------------------------------------------
// WBackendNode reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WBackendYuvNode::setTextures(WBackendTexture * textures)
{
    if (textures)
    {
        material.textures = textures;

        material.update = true;

        markDirty(QSGNode::DirtyMaterial);
    }
    else material.update = false;
}

#ifdef QT_6

// NOTE: The adjust of VLC's filter, on the GPU: contrast and brightness on the luma around its
//       middle, then gamma, then hue and saturation on the chroma. The hue is in degrees.
/* virtual */ void WBackendYuvNode::setAdjust(const WBackendAdjust & adjust)
{
    if (material.adjust == adjust) return;

    material.adjust = adjust;

    if (adjust.enable)
    {
        float contrast   = adjust.contrast;
        float brightness = adjust.brightness - 1.0f;
        float saturation = adjust.saturation;

        float angle = qDegreesToRadians(adjust.hue);

        float cosine = qCos(angle) * saturation;
        float sine   = qSin(angle) * saturation;

        material.matrixAdjust = QMatrix4x4(contrast, 0.0f,   0.0f,
                                           0.5f - 0.5f * contrast + brightness,
                                           0.0f,     cosine, sine,
                                           0.5f - 0.5f * (cosine + sine),
                                           0.0f,    -sine,   cosine,
                                           0.5f - 0.5f * (cosine - sine),
                                           0.0f,     0.0f,   0.0f, 1.0f);

        material.gamma = 1.0f / adjust.gamma;
    }
    else
    {
        material.matrixAdjust.setToIdentity();

        material.gamma = 1.0f;
    }

    material.updateAdjust = true;

    markDirty(QSGNode::DirtyMaterial);
}

#endif

#endif // SK_NO_BACKENDYUV
#endif
