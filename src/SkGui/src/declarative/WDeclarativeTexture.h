//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of Sky kit.

    - GNU Lesser General Public License Usage:
    This file may be used under the terms of the GNU Lesser General Public License version 3 as
    published by the Free Software Foundation and appearing in the LICENSE.md file included in the
    packaging of this file. Please review the following information to ensure the GNU Lesser
    General Public License requirements will be met: https://www.gnu.org/licenses/lgpl.html.
*/
//=================================================================================================

#ifndef WDECLARATIVETEXTURE_H
#define WDECLARATIVETEXTURE_H

// Sk includes
#include <WDeclarativeItem>

#ifndef SK_NO_DECLARATIVETEXTURE

// Forward declarations
class QSGInternalImageNode;
class WDeclarativeTexturePrivate;

class SK_GUI_EXPORT WDeclarativeTexture : public WDeclarativeItem
{
    Q_OBJECT

    Q_PROPERTY(bool mipmap READ mipmap WRITE setMipmap NOTIFY mipmapChanged)

public:
    explicit WDeclarativeTexture(QQuickItem * parent = NULL);
protected:
    WDeclarativeTexture(WDeclarativeTexturePrivate * p, QQuickItem * parent = NULL);

public: // QQuickItem reimplementation
    /* virtual */ QSGNode * updatePaintNode(QSGNode * oldNode, UpdatePaintNodeData * data);

protected: // Abstract functions
    virtual const QPixmap & getPixmap() = 0;

protected: // Virtual functions
    virtual void applyGeometry(QSGInternalImageNode * node, const QPixmap & pixmap);

protected: // QQuickItem reimplementation
    /* virtual */ void itemChange(ItemChange change, const ItemChangeData & value);

signals:
    void mipmapChanged();

public: // Properties
    bool mipmap() const;
    void setMipmap(bool enabled);

private:
    W_DECLARE_PRIVATE(WDeclarativeTexture)
};

#endif // SK_NO_DECLARATIVETEXTURE
#endif // WDECLARATIVETEXTURE_H
