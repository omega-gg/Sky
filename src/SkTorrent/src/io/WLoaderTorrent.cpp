//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkTorrent.

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

#include "WLoaderTorrent.h"

#ifndef SK_NO_LOADERTORRENT

// Qt includes
#include <QBuffer>

// Sk includes
#include <WControllerDownload>
#include <WControllerTorrent>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

WLoaderTorrentPrivate::WLoaderTorrentPrivate(WLoaderTorrent * p) : WAbstractLoaderPrivate(p) {}

//-------------------------------------------------------------------------------------------------

void WLoaderTorrentPrivate::init()
{
    Q_ASSERT(WControllerTorrent::instancePointer());
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WLoaderTorrentPrivate::onLoaded(WMagnetReply * reply)
{
    Q_Q(WLoaderTorrent);

    QBuffer * buffer = static_cast<QBuffer *> (replies.key(reply));

    replies.remove(buffer);

    WMagnet * magnet = reply->magnet();

    if (magnet->hasError())
    {
        q->setError(q->getData(buffer), magnet->error());
    }

    buffer->setData(magnet->data());

    buffer->open(QIODevice::ReadOnly);

    q->complete(buffer);

    QObject::disconnect(reply, 0, q, 0);

    reply->deleteLater();
}

void WLoaderTorrentPrivate::onDestroyed()
{
    Q_Q(WLoaderTorrent);

    WMagnetReply * reply = static_cast<WMagnetReply *> (q->sender());

    QBuffer * buffer = static_cast<QBuffer *> (replies.key(reply));

    replies.remove(buffer);

    buffer->open(QIODevice::ReadOnly);

    q->complete(buffer);
}

//=================================================================================================
// WLoaderTorrent
//=================================================================================================

/* explicit */ WLoaderTorrent::WLoaderTorrent(QObject * parent)
    : WAbstractLoader(new WLoaderTorrentPrivate(this), parent)
{
    Q_D(WLoaderTorrent); d->init();
}

//-------------------------------------------------------------------------------------------------
// Protected WAbstractLoader implementation
//-------------------------------------------------------------------------------------------------

/* virtual */ QIODevice * WLoaderTorrent::load(WRemoteData * data)
{
    Q_D(WLoaderTorrent);

    WMagnetReply * reply = wControllerTorrent->getMagnet(data->url(), this);

    QBuffer * buffer = new QBuffer;

    d->replies.insert(buffer, reply);

    connect(reply, SIGNAL(loaded(WMagnetReply *)), this, SLOT(onLoaded(WMagnetReply *)));

    connect(reply, SIGNAL(destroyed()), this, SLOT(onDestroyed()));

    return buffer;
}

//-------------------------------------------------------------------------------------------------
// Protected WAbstractLoader reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WLoaderTorrent::abort(QIODevice * reply)
{
    Q_D(WLoaderTorrent);

    WMagnetReply * magnet = d->replies.take(reply);

    reply->open(QIODevice::ReadOnly);

    complete(reply);

    disconnect(magnet, 0, this, 0);

    delete magnet;
}

#endif // SK_NO_LOADERTORRENT
