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

#include "WControllerTorrent.h"

#ifndef SK_NO_CONTROLLERTORRENT

// Qt includes
#include <QThread>
#include <QDeclarativeComponent>

// Sk includes
#include <WControllerFile>
#include <WTorrentEngine>

W_INIT_CONTROLLER(WControllerTorrent)

//-------------------------------------------------------------------------------------------------
// Static variables

static const QString CONTROLLERTORRENT_PATH_TORRENTS = "/torrents";

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include <private/WControllerTorrent_p>

WControllerTorrentPrivate::WControllerTorrentPrivate(WControllerTorrent * p)
    : WControllerPrivate(p) {}

/* virtual */ WControllerTorrentPrivate::~WControllerTorrentPrivate()
{
    delete engine;

    thread->quit();
    thread->wait();

    W_CLEAR_CONTROLLER(WControllerTorrent);
}

void WControllerTorrentPrivate::init()
{
    Q_Q(WControllerTorrent);

    thread = new QThread(q);

    thread->start();

    engine = new WTorrentEngine(thread);

    QObject::connect(wControllerFile, SIGNAL(pathStorageChanged()),
                     q,               SIGNAL(pathStorageChanged()));
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

WControllerTorrent::WControllerTorrent() : WController(new WControllerTorrentPrivate(this)) {}

//-------------------------------------------------------------------------------------------------
// Initialize
//-------------------------------------------------------------------------------------------------

/* virtual */ void WControllerTorrent::init()
{
    Q_D(WControllerTorrent); d->init();
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

QString WControllerTorrent::pathStorage() const
{
    return wControllerFile->pathStorage() + "/torrents";
}

#endif // SK_NO_CONTROLLERTORRENT
