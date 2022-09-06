//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkCore.

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

#include "WBroadcastServer.h"

#ifndef SK_NO_BROADCASTSERVER

// Qt includes
#include <QNetworkInterface>
#include <QHostInfo>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include "WBroadcastServer_p.h"

WBroadcastServerPrivate::WBroadcastServerPrivate(WBroadcastServer * p) : WPrivate(p) {}

//-------------------------------------------------------------------------------------------------

void WBroadcastServerPrivate::init() {}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

/* explicit */ WBroadcastServer::WBroadcastServer(QObject * parent)
    : QObject(parent), WPrivatable(new WBroadcastServerPrivate(this))
{
    Q_D(WBroadcastServer); d->init();
}

//-------------------------------------------------------------------------------------------------
// Static functions
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WBroadcastServer::source(const QString & prefix)
{
    const QHostAddress & local = QHostAddress(QHostAddress::LocalHost);

    QString host;

    foreach (const QHostAddress & address, QNetworkInterface::allAddresses())
    {
        if (local == address
            ||
            address.protocol() != QAbstractSocket::IPv4Protocol || address.isLoopback()) continue;

        host = address.toString();

        break;
    }

    if (host.isEmpty()) return QString();

    QString name = QHostInfo::localHostName();

    if (prefix.isEmpty())
    {
        return host + '/' + name;
    }
    else return prefix + '/' + host + '/' + name;
}

#endif // SK_NO_BROADCASTSERVER
