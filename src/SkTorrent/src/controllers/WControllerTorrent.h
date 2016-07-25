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

#ifndef WCONTROLLERTORRENT_H
#define WCONTROLLERTORRENT_H

// Sk includes
#include <WController>

#ifndef SK_NO_CONTROLLERTORRENT

// Forward declarations
class WControllerTorrentPrivate;

// Defines
#define wControllerTorrent WControllerTorrent::instance()

class SK_TORRENT_EXPORT WControllerTorrent : public WController
{
    Q_OBJECT

    Q_PROPERTY(QString pathStorage READ pathStorage NOTIFY pathStorageChanged)

private:
    WControllerTorrent();

protected: // Initialize
    /* virtual */ void init();

signals:
    void pathStorageChanged();

public: // Properties
    QString pathStorage() const;

private:
    W_DECLARE_PRIVATE   (WControllerTorrent)
    W_DECLARE_CONTROLLER(WControllerTorrent)
};

#endif // SK_NO_CONTROLLERTORRENT
#endif // WCONTROLLERTORRENT_H
