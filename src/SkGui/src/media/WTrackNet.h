//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkGui module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#ifndef WTRACKNET_H
#define WTRACKNET_H

// Sk includes
#include <WAbstractTrack>

#ifndef SK_NO_TRACKNET

// Forward declarations
class WTrackNetPrivate;
class WPlaylistNet;

class SK_GUI_EXPORT WTrackNet : public WAbstractTrack
{
public:
    explicit WTrackNet(const QUrl & source = QUrl(), State state = Loaded);

public: // Operators
    WTrackNet(const WTrackNet & other);

public: // Properties
    WPlaylistNet * playlistNet() const;

private:
    W_DECLARE_PRIVATE_COPY(WTrackNet)

    friend class WPlaylistNet;
    friend class WPlaylistNetPrivate;
    friend class WPlaylistNetRead;
};

#endif // SK_NO_TRACKNET
#endif // WTRACKNET_H
