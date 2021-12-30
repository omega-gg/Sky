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

#ifndef WTEXTUREVIDEO_H
#define WTEXTUREVIDEO_H

// Qt includes
#include <QSGTexture>
#include <QSGMaterialShader>

// Sk includes
#include <Sk>

#ifndef SK_NO_TEXTUREVIDEO

// Forward declarations
class  WTextureVideoPrivate;
struct WBackendTexture;

class SK_GUI_EXPORT WTextureVideo : public QSGTexture, public WPrivatable
{
    Q_OBJECT

public:
    WTextureVideo();

public: // Interface
    void create(QSGMaterialShader::RenderState & state, const WBackendTexture & texture);
    void upload(QSGMaterialShader::RenderState & state, const WBackendTexture & texture);

public: // QSGTexture implementation
    /* virtual */ qint64 comparisonKey() const;

    /* virtual */ QSize textureSize() const;

    /*virtual */ bool hasAlphaChannel() const;
    /*virtual */ bool hasMipmaps     () const;

public: // QSGTexture reimplementation
    /* virtual */ QRhiTexture * rhiTexture() const;

private:
    W_DECLARE_PRIVATE(WTextureVideo)
};

#endif // SK_NO_TEXTUREVIDEO
#endif // WTEXTUREVIDEO_H
