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

#ifndef WMODELPLAYLISTWEB_P_H
#define WMODELPLAYLISTWEB_P_H

#include <private/Sk_p>

#ifndef SK_NO_MODELPLAYLISTWEB

//=================================================================================================
// WModelPlaylistPrivate
//=================================================================================================

class SK_GUI_EXPORT WModelPlaylistPrivate : public WPrivate
{
public:
    WModelPlaylistPrivate(WModelPlaylist * p);

    /* virtual */ ~WModelPlaylistPrivate();

    void init();

public: // Variables
    WPlaylist * playlist;

    const WTrack * oldTrack;

protected:
    W_DECLARE_PUBLIC(WModelPlaylist)
};

//=================================================================================================
// WModelPlaylistFilteredPrivate
//=================================================================================================

class SK_GUI_EXPORT WModelPlaylistFilteredPrivate : public WPrivate
{
public:
    WModelPlaylistFilteredPrivate(WModelPlaylistFiltered * p);

    void init();

public: // Slots
    void onRoleChanged();

public: // Variables
    WModelPlaylist * model;

    Qt::SortOrder sortOrder;

protected:
    W_DECLARE_PUBLIC(WModelPlaylistFiltered)
};

#endif // SK_NO_MODELPLAYLISTWEB
#endif // WMODELPLAYLISTWEB_P_H
