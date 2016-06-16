//=================================================================================================
/*
    Copyright (C) 2015-2016 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkTorrent module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#ifndef WHOOKTORRENT_H
#define WHOOKTORRENT_H

// Sk includes
#include <WAbstractHook>

#ifndef SK_NO_HOOKTORRENT

class WHookTorrentPrivate;

class SK_TORRENT_EXPORT WHookTorrent : public WAbstractHook
{
    Q_OBJECT

public:
    WHookTorrent();

private:
    W_DECLARE_PRIVATE(WHookTorrent)
};

#endif // SK_NO_HOOKTORRENT
#endif // WHOOKTORRENT_H
