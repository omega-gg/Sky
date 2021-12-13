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

#include "WTextureVideo.h"

#ifndef SK_NO_TEXTUREVIDEO

// Sk includes
#include <WAbstractBackend>

// Qt private includes
#include <private/qrhi_p.h>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include "WTextureVideo_p.h"

WTextureVideoPrivate::WTextureVideoPrivate(WTextureVideo * p) : WPrivate(p) {}

void WTextureVideoPrivate::init()
{
    Q_Q(WTextureVideo);

    texture = NULL;

    // NOTE: We want linear filtering by default.
    q->setFiltering(QSGTexture::Linear);
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

WTextureVideo::WTextureVideo()
    : QSGTexture(), WPrivatable(new WTextureVideoPrivate(this))
{
    Q_D(WTextureVideo); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

void WTextureVideo::create(QSGMaterialShader::RenderState & state,
                           const WBackendTexture          & texture)
{
    Q_D(WTextureVideo);

    if (d->texture) delete d->texture;

    d->size = texture.size;

    d->texture = state.rhi()->newTexture(QRhiTexture::R8, d->size);

    d->texture->create();

    d->description.setData(QByteArray(reinterpret_cast<const char *> (texture.bits),
                                      texture.length));

    // NOTE: This is required when the data is padded on each line.
    d->description.setDataStride(texture.pitch);

    // NOTE: The rendering seems to work without this call.
    //d->description.setSourceSize(d->size);

    QRhiTextureUploadDescription description(QRhiTextureUploadEntry(0, 0, d->description));

    state.resourceUpdateBatch()->uploadTexture(d->texture, description);
}

void WTextureVideo::upload(QSGMaterialShader::RenderState & state,
                           const WBackendTexture          & texture)
{
    Q_D(WTextureVideo);

    d->description.setData(QByteArray(reinterpret_cast<const char *> (texture.bits),
                                      texture.length));

    QRhiTextureUploadDescription description(QRhiTextureUploadEntry(0, 0, d->description));

    state.resourceUpdateBatch()->uploadTexture(d->texture, description);
}

//-------------------------------------------------------------------------------------------------
// QSGTexture implementation
//-------------------------------------------------------------------------------------------------

/* virtual */ qint64 WTextureVideo::comparisonKey() const
{
    Q_D(const WTextureVideo);

    if (d->texture) return qintptr(d->texture);
    else            return qintptr(this);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ QSize WTextureVideo::textureSize() const
{
    Q_D(const WTextureVideo); return d->size;
}

//-------------------------------------------------------------------------------------------------

/*virtual */ bool WTextureVideo::hasAlphaChannel() const
{
    return false;
}

/*virtual */ bool WTextureVideo::hasMipmaps() const
{
    return (mipmapFiltering() != QSGTexture::None);
}

//-------------------------------------------------------------------------------------------------
// QSGTexture reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ QRhiTexture * WTextureVideo::rhiTexture() const
{
    Q_D(const WTextureVideo); return d->texture;
}

#endif // SK_NO_TEXTUREVIDEO
