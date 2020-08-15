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

#include "WDeclarativeTexture.h"

#ifndef SK_NO_DECLARATIVETEXTURE

// Sk includes
#include <WView>

// Private includes
#include <private/qquickwindow_p.h>
#include <private/qsgadaptationlayer_p.h>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include "WDeclarativeTexture_p.h"

WDeclarativeTexturePrivate::WDeclarativeTexturePrivate(WDeclarativeTexture * p)
    : WDeclarativeItemPrivate(p) {}

/* virtual */ WDeclarativeTexturePrivate::~WDeclarativeTexturePrivate()
{
    if (texture) texture->deleteLater();
}

//-------------------------------------------------------------------------------------------------

void WDeclarativeTexturePrivate::init()
{
    Q_Q(WDeclarativeTexture);

    context = NULL;
    texture = NULL;

    updateTexture  = false;
    updateGeometry = false;
    updateMipmap   = false;

    mipmap = false;

    q->setFlag(QQuickItem::ItemHasContents);
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WDeclarativeTexturePrivate::applyMipmap(QSGInternalImageNode * node)
{
    updateMipmap = false;

    if (mipmap)
    {
         node->setMipmapFiltering(QSGTexture::Linear);
    }
    else node->setMipmapFiltering(QSGTexture::None);
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

/* explicit */ WDeclarativeTexture::WDeclarativeTexture(QQuickItem * parent)
    : WDeclarativeItem(new WDeclarativeTexturePrivate(this), parent)
{
    Q_D(WDeclarativeTexture); d->init();
}

//-------------------------------------------------------------------------------------------------
// Protected

WDeclarativeTexture::WDeclarativeTexture(WDeclarativeTexturePrivate * p, QQuickItem * parent)
    : WDeclarativeItem(p, parent)
{
    Q_D(WDeclarativeTexture); d->init();
}

//-------------------------------------------------------------------------------------------------
// QQuickItem reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ QSGNode * WDeclarativeTexture::updatePaintNode(QSGNode             * oldNode,
                                                             UpdatePaintNodeData *)
{
    const QPixmap & pixmap = getPixmap();

    if (pixmap.isNull() || width() < 1 || height() < 1)
    {
        if (oldNode)
        {
            Q_D(WDeclarativeTexture);

            delete oldNode;

            delete d->texture;

            d->texture = NULL;
        }

        return NULL;
    }

    Q_D(WDeclarativeTexture);

    QSGInternalImageNode * node;

    if (oldNode)
    {
        node = static_cast<QSGInternalImageNode *> (oldNode);

        if (d->updateTexture)
        {
            if (d->updateMipmap) d->applyMipmap(node);

            d->updateTexture  = false;
            d->updateGeometry = false;

            if (d->texture) delete d->texture;

            d->texture = d->view->createTextureFromImage(pixmap.toImage());

            node->setTexture(d->texture);

            applyGeometry(node, pixmap);

            node->update();
        }
        else if (d->updateGeometry)
        {
            if (d->updateMipmap) d->applyMipmap(node);

            d->updateGeometry = false;

            applyGeometry(node, pixmap);

            node->update();
        }
        else if (d->updateMipmap)
        {
            d->applyMipmap(node);

            node->update();
        }
    }
    else
    {
#if QT_VERSION >= QT_VERSION_CHECK(5, 13, 0)
        node = d->context->sceneGraphContext()->createInternalImageNode(d->context);
#else
        node = d->context->sceneGraphContext()->createInternalImageNode();
#endif

        node->setFiltering(QSGTexture::Linear);

        d->applyMipmap(node);

        d->updateTexture  = false;
        d->updateGeometry = false;

        d->texture = d->view->createTextureFromImage(pixmap.toImage());

        node->setTexture(d->texture);

        applyGeometry(node, pixmap);

        node->update();
    }

    return node;
}

//-------------------------------------------------------------------------------------------------
// Protected virtual functions
//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativeTexture::applyGeometry(QSGInternalImageNode * node, const QPixmap &)
{
    QRectF rect = boundingRect();

    node->setTargetRect     (rect);
    node->setInnerTargetRect(rect);
}

//-------------------------------------------------------------------------------------------------
// Protected QQuickItem reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativeTexture::itemChange(ItemChange change, const ItemChangeData & value)
{
    if (change == ItemSceneChange)
    {
        Q_D(WDeclarativeTexture);

        QQuickWindow * window = value.window;

        if (window)
        {
            QQuickWindowPrivate * p
                = static_cast<QQuickWindowPrivate *> (QObjectPrivate::get(window));

            d->context = p->context;
        }
        else d->context = NULL;
    }

    WDeclarativeItem::itemChange(change, value);
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

bool WDeclarativeTexture::mipmap() const
{
    Q_D(const WDeclarativeTexture); return d->mipmap;
}

void WDeclarativeTexture::setMipmap(bool enabled)
{
    Q_D(WDeclarativeTexture);

    if (d->mipmap == enabled) return;

    d->mipmap = enabled;

    d->updateMipmap = true;

    update();

    emit mipmapChanged();
}

#endif // SK_NO_DECLARATIVETEXTURE
