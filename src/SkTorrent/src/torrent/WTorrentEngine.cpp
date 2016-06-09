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

class SK_TORRENT_EXPORT WTorrentEnginePrivate : public WPrivate
{
public: // Enums
    enum EventType
    {
        EventStart = QEvent::User,
        EventStop
    };

public:
    WTorrentEnginePrivate(WTorrentEngine * p);

    /* virtual */ ~WTorrentEnginePrivate();

    void init(QThread * thread);

public: // Variables
    libtorrent::session * session;

protected:
    W_DECLARE_PUBLIC(WTorrentEngine)
};

//-------------------------------------------------------------------------------------------------

WTorrentEnginePrivate::WTorrentEnginePrivate(WTorrentEngine * p) : WPrivate(p) {}

/* virtual */ WTorrentEnginePrivate::~WTorrentEnginePrivate()
{
    Q_Q(WTorrentEngine);

    QCoreApplication::postEvent(q, new QEvent(static_cast<QEvent::Type>
                                              (WTorrentEnginePrivate::EventStop)),
                                Qt::HighEventPriority * 100);
}

//-------------------------------------------------------------------------------------------------

void WTorrentEnginePrivate::init(QThread * thread)
{
    Q_Q(WTorrentEngine);

    session = NULL;

    if (thread) q->moveToThread(thread);

    QCoreApplication::postEvent(q, new QEvent(static_cast<QEvent::Type>
                                              (WTorrentEnginePrivate::EventStart)),
                                Qt::HighEventPriority * 100);
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
// Events
//-------------------------------------------------------------------------------------------------

bool WTorrentEngine::event(QEvent * event)
{
    Q_D(WTorrentEngine);

    QEvent::Type type = event->type();

    if (type == static_cast<QEvent::Type> (WTorrentEnginePrivate::EventStart))
    {
        d->session = new libtorrent::session;

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WTorrentEnginePrivate::EventStop))
    {
        delete d->session;

        d->session = NULL;

        return true;
    }
    else return QObject::event(event);
}

#endif // SK_NO_TORRENTENGINE
