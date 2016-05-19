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

#ifndef WBACKENDBENCODE_H
#define WBACKENDBENCODE_H

// Sk includes
#include <WBackendNet>

#ifndef SK_NO_BACKENDBENCODE

// Forward declarations
class WBackendBencodePrivate;

class SK_TORRENT_EXPORT WBackendBencode : public WBackendNet
{
    Q_OBJECT

public:
    WBackendBencode();

public: // WBackendNet implementation
    /* Q_INVOKABLE virtual */ QString getId   () const;
    /* Q_INVOKABLE virtual */ QString getTitle() const;

    /* Q_INVOKABLE virtual */ bool checkValidUrl(const QUrl & url) const;

private:
    W_DECLARE_PRIVATE(WBackendBencode)
};

#endif // SK_NO_BACKENDBENCODE
#endif // WBACKENDBENCODE_H
