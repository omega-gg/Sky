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

#include "WBackendBencode.h"

#ifndef SK_NO_BACKENDBENCODE

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include <private/WBackendNet_p>

class SK_TORRENT_EXPORT WBackendBencodePrivate : public WBackendNetPrivate
{
public:
    WBackendBencodePrivate(WBackendBencode * p);

    void init();

protected:
    W_DECLARE_PUBLIC(WBackendBencode)
};

//-------------------------------------------------------------------------------------------------

WBackendBencodePrivate::WBackendBencodePrivate(WBackendBencode * p)
    : WBackendNetPrivate(p) {}

void WBackendBencodePrivate::init() {}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

WBackendBencode::WBackendBencode() : WBackendNet(new WBackendBencodePrivate(this))
{
    Q_D(WBackendBencode); d->init();
}

//-------------------------------------------------------------------------------------------------
// WBackendNet implementation
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QString WBackendBencode::getId() const
{
    return "bencode";
}

/* Q_INVOKABLE virtual */ QString WBackendBencode::getTitle() const
{
    return "Bencode";
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ bool WBackendBencode::checkValidUrl(const QUrl &) const
{
    return false;
}

#endif // SK_NO_BACKENDBENCODE
