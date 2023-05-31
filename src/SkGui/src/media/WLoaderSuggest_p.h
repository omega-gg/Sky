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

#ifndef WLOADERSUGGEST_P_H
#define WLOADERSUGGEST_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

// Private includes
#include <private/WLoaderPlaylist_p>

#ifndef SK_NO_LOADERSUGGEST

class SK_GUI_EXPORT WLoaderSuggestPrivate : public WLoaderPlaylistPrivate
{
public:
    WLoaderSuggestPrivate(WLoaderSuggest * p);

    void init(WPlaylist * history);

public: // Variables
    WPlaylist * history;

protected:
    W_DECLARE_PUBLIC(WLoaderSuggest)
};

#endif // SK_NO_LOADERSUGGEST
#endif // WLOADERSUGGEST_P_H
