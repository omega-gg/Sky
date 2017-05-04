//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkTorrent module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#include "WTorrentEngine.h"

#ifndef SK_NO_TORRENTENGINE

// Qt includes
#include <QCoreApplication>
#include <QEvent>

// libtorrent includes
#include <libtorrent/session.hpp>

// Private includes
#include <private/Sk_p>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include "WTorrentEngine_p.h"

WTorrentEnginePrivate::WTorrentEnginePrivate(WTorrentEngine * p) : WPrivate(p) {}

//-------------------------------------------------------------------------------------------------

void WTorrentEnginePrivate::init(QThread * thread)
{
    Q_Q(WTorrentEngine);

    session = NULL;

    if (thread) q->moveToThread(thread);

    QCoreApplication::postEvent(q, new QEvent(static_cast<QEvent::Type>
                                              (WTorrentEnginePrivate::EventCreate)),
                                Qt::HighEventPriority * 100);
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

boost::function<void()> const WTorrentEnginePrivate::processAlert()
{
    std::vector<libtorrent::alert *> alerts;

    session->pop_alerts(&alerts);

    std::vector<libtorrent::alert * >::iterator i = alerts.begin();

    while (i != alerts.end())
    {
        qDebug("libtorrent: %s", (*i)->message().c_str());

        i++;
    }

    return boost::function<void()>();
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

WTorrentEngine::WTorrentEngine(QThread * thread, QObject * parent)
    : QObject(parent), WPrivatable(new WTorrentEnginePrivate(this))
{
    Q_D(WTorrentEngine); d->init(thread);
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WTorrentEngine::load(WTorrent * torrent, const QByteArray & data)
{

}

/* Q_INVOKABLE */ void WTorrentEngine::remove(WTorrent * torrent, bool deleteFiles)
{

}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WTorrentEngine::deleteInstance()
{
    QCoreApplication::postEvent(this, new QEvent(static_cast<QEvent::Type>
                                                 (WTorrentEnginePrivate::EventClear)),
                                Qt::HighEventPriority * 100);
}

//-------------------------------------------------------------------------------------------------
// Events
//-------------------------------------------------------------------------------------------------

/* virtual */ bool WTorrentEngine::event(QEvent * event)
{
    Q_D(WTorrentEngine);

    QEvent::Type type = event->type();

    if (type == static_cast<QEvent::Type> (WTorrentEnginePrivate::EventCreate))
    {
        d->session = new libtorrent::session;

        d->session->set_alert_notify(d->processAlert());

        return true;
    }
    else if (d->session == NULL)
    {
        return QObject::event(event);
    }
    else if (type == static_cast<QEvent::Type> (WTorrentEnginePrivate::EventClear))
    {
        delete d->session;

        d->session = NULL;

        return true;
    }
    else return QObject::event(event);
}

#endif // SK_NO_TORRENTENGINE
