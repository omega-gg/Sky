//=================================================================================================
/*
    Copyright (C) 2015-2016 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkGui module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#ifndef WMODELPLAYLISTWEB_P_H
#define WMODELPLAYLISTWEB_P_H

#include <private/Sk_p>

#ifndef SK_NO_MODELPLAYLISTWEB

// Forward declarations
class WPlaylistNet;

class SK_GUI_EXPORT WModelPlaylistNetPrivate : public WPrivate
{
public:
    WModelPlaylistNetPrivate(WModelPlaylistNet * p);

    /* virtual */ ~WModelPlaylistNetPrivate();

    void init();

public: // Variables
    WPlaylistNet * playlist;

    const WAbstractTrack * oldTrack;

protected:
    W_DECLARE_PUBLIC(WModelPlaylistNet)
};

#endif // SK_NO_MODELPLAYLISTWEB
#endif // WMODELPLAYLISTWEB_P_H
