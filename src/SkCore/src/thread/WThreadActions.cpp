//=================================================================================================
/*
    Copyright (C) 2015-2016 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkCore module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#include "WThreadActions.h"

#ifndef SK_NO_THREADACTIONS

// Qt includes
#include <QCoreApplication>

// Sk includes
#include <WAbstractThreadAction>
#include <WAbstractThreadReply>

// Private includes
#include <private/WAbstractThreadAction_p>

//=================================================================================================
// WThreadActionsEvent
//=================================================================================================

class WThreadActionsEvent : public QEvent
{
public:
    WThreadActionsEvent(WAbstractThreadAction * action) : QEvent(QEvent::User)
    {
        this->action = action;
    }

public: // Variables
    WAbstractThreadAction * action;
};

//=================================================================================================
// WThreadActionsThread
//=================================================================================================

class WThreadActionsThread : public QThread
{
    Q_OBJECT

public:
    WThreadActionsThread();

public: // QObject reimplementation
    /* virtual */ bool event(QEvent * event);
};

//-------------------------------------------------------------------------------------------------

WThreadActionsThread::WThreadActionsThread() : QThread()
{
    moveToThread(this);

    start(QThread::IdlePriority);
}

//-------------------------------------------------------------------------------------------------
// QObject reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ bool WThreadActionsThread::event(QEvent * event)
{
    if (event->type() == QEvent::User)
    {
        WThreadActionsEvent * eventThread = static_cast<WThreadActionsEvent *> (event);

        WAbstractThreadAction * action = eventThread->action;

        bool ok = action->run();

        WAbstractThreadReply * reply = action->d_func()->reply;

        if (reply)
        {
            reply->postReply(action, ok);
        }

        return true;
    }
    else return QObject::event(event);
}

//=================================================================================================
// WThreadActionsPrivate
//=================================================================================================

#include "WThreadActions_p.h"

WThreadActionsPrivate::WThreadActionsPrivate(WThreadActions * p) : WPrivate(p) {}

/* virtual */ WThreadActionsPrivate::~WThreadActionsPrivate()
{
    thread->quit();
    thread->wait();

    delete thread;
}

//-------------------------------------------------------------------------------------------------

void WThreadActionsPrivate::init()
{
    thread = new WThreadActionsThread;
}

//=================================================================================================
// WThreadActions
//=================================================================================================

/* explicit */ WThreadActions::WThreadActions(QObject * parent)
    : QObject(parent), WPrivatable(new WThreadActionsPrivate(this))
{
    Q_D(WThreadActions); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

WAbstractThreadReply * WThreadActions::pushAction(WAbstractThreadAction * action)
{
    if (action->d_func()->thread)
    {
        qWarning("WThreadActions::pushAction: Action is already pushed.");

        return NULL;
    }

    Q_D(WThreadActions);

    action->d_func()->thread = this;

    WAbstractThreadReply * reply = action->d_func()->generateReply();

    QCoreApplication::postEvent(d->thread, new WThreadActionsEvent(action));

    return reply;
}

#endif // SK_NO_THREADACTIONS

#include "WThreadActions.moc"
