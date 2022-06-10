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

#ifndef WDECLARATIVETEXTURE_P_H
#define WDECLARATIVETEXTURE_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

#include <private/WDeclarativeItem_p>

#ifndef SK_NO_DECLARATIVETEXTURE

// Forward declarations
class QSGRenderContext;
class QSGTexture;

class SK_GUI_EXPORT WDeclarativeTexturePrivate : public WDeclarativeItemPrivate
{
public:
    WDeclarativeTexturePrivate(WDeclarativeTexture * p);

    /* virtual */ ~WDeclarativeTexturePrivate();

    void init();

public: // Functions
    void applySmooth(QSGInternalImageNode * node, bool smooth);
    void applyMipmap(QSGInternalImageNode * node);

public: // Slots
    void onSmoothChanged();

public: // Variables
    QSGRenderContext * context;
    QSGTexture       * texture;

    bool updateTexture  : 1;
    bool updateGeometry : 1;
    bool updateSmooth   : 1;
    bool updateMipmap   : 1;

    bool mipmap : 1;

protected:
    W_DECLARE_PUBLIC(WDeclarativeTexture)
};

#endif // SK_NO_DECLARATIVETEXTURE
#endif // WDECLARATIVETEXTURE_P_H
