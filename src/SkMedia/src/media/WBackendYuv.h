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

#ifndef WBACKENDYUV_H
#define WBACKENDYUV_H

// Sk includes
#include <WAbstractBackend>

#if defined(QT_NEW) && defined(SK_NO_QML) == false

// Qt includes
#include <QSGMaterial>
#ifdef QT_5
#include <QOpenGLFunctions>
#endif

// Sk includes
#ifdef QT_6
#include <WTextureVideo>
#endif

#ifndef SK_NO_BACKENDYUV

//-------------------------------------------------------------------------------------------------
// WBackendYuvShader
//-------------------------------------------------------------------------------------------------

class SK_MEDIA_EXPORT WBackendYuvShader : public QSGMaterialShader
{
#ifdef QT_6
public:
    WBackendYuvShader();
#endif

#ifdef QT_5
public: // QSGMaterialShader implementation
    /* virtual */ char const * const * attributeNames() const;
#endif

public: // QSGMaterialShader reimplementation
#ifdef QT_5
    /* virtual */ void updateState(const RenderState & state, QSGMaterial * newMaterial,
                                                              QSGMaterial * oldMaterial);
#else
    /* virtual */ bool updateUniformData(RenderState & state, QSGMaterial * newMaterial,
                                         QSGMaterial * oldMaterial);

    /* virtual */ void updateSampledImage(RenderState & state, int binding, QSGTexture ** texture,
                                          QSGMaterial * newMaterial, QSGMaterial * oldMaterial);
#endif

#ifdef QT_5
protected: // QSGMaterialShader reimplementation
    /* virtual */ void initialize();

    /* virtual */ const char * vertexShader  () const;
    /* virtual */ const char * fragmentShader() const;
#endif

public: // Properties
#ifdef QT_5
    int idPosition;
    int idOpacity;
    int idColor;

    int idY;
    int idU;
    int idV;
#else
    bool isNew;
#endif
};

//-------------------------------------------------------------------------------------------------
// WBackendYuvMaterial
//-------------------------------------------------------------------------------------------------

class SK_MEDIA_EXPORT WBackendYuvMaterial : public QSGMaterial
{
public:
    WBackendYuvMaterial();

    /* virtual */ ~WBackendYuvMaterial();

#ifdef QT_5
public: // Functions
    void updateTextures();
#endif

public: // QSGMaterial implementation
    /* virtual */ QSGMaterialType * type() const;

#ifdef QT_5
    /* virtual */ QSGMaterialShader * createShader() const;
#else
    /* virtual */
    QSGMaterialShader * createShader(QSGRendererInterface::RenderMode renderMode) const;
#endif

public: // Properties
#ifdef QT_5
    QOpenGLFunctions * gl;
#endif

    WBackendTexture * textures;

#ifdef QT_5
    GLuint ids[3];
#else
    WTextureVideo data[3];

    WBackendAdjust adjust;

    QMatrix4x4 matrixAdjust;

    float gamma;

    bool updateAdjust;
#endif

    bool update;
};

//-------------------------------------------------------------------------------------------------
// WBackendYuvNode
//-------------------------------------------------------------------------------------------------

class SK_MEDIA_EXPORT WBackendYuvNode : public WBackendNode
{
public:
    WBackendYuvNode();

public: // WBackendNode reimplementation
    /* virtual */ void setTextures(WBackendTexture * textures);

#ifdef QT_6
    /* virtual */ void setAdjust(const WBackendAdjust & adjust);
#endif

public: // Properties
    WBackendYuvMaterial material;
};

#endif // SK_NO_BACKENDYUV
#endif
#endif // WBACKENDYUV_H
