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

#include "WAbstractThreadAction.h"

#ifndef SK_NO_ABSTRACTTHREADACTION

// Sk includes
#include <WThreadActions>
#include <WAbstractThreadReply>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include "WAbstractThreadAction_p.h"

WAbstractThreadActionPrivate::WAbstractThreadActionPrivate(WAbstractThreadAction * p)
    : WPrivate(p) {}

/* virtual */ WAbstractThreadActionPrivate::~WAbstractThreadActionPrivate()
{
    if (reply) delete reply;
}

//-------------------------------------------------------------------------------------------------

void WAbstractThreadActionPrivate::init()
{
    thread = NULL;

    reply = NULL;

    abort = false;
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

WAbstractThreadReply * WAbstractThreadActionPrivate::generateReply()
{
    Q_Q(WAbstractThreadAction);

    reply = q->createReply();

    emit q->replyChanged();

    return reply;
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

WAbstractThreadAction::WAbstractThreadAction()
    : QObject(), WPrivatable(new WAbstractThreadActionPrivate(this))
{
    Q_D(WAbstractThreadAction); d->init();
}

//-------------------------------------------------------------------------------------------------
// Protected

/* virtual */ WAbstractThreadAction::~WAbstractThreadAction() {}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

void WAbstractThreadAction::start()
{
    Q_D(WAbstractThreadAction);

    if (d->reply)
    {
        qWarning("WAbstractThreadAction::start: Action is already started.");

        return;
    }

    d->reply = createReply();

    bool ok = run();

    if (d->reply)
    {
        d->reply->onCompleted(ok);

        delete d->reply;

        d->reply = NULL;
    }
}

//-------------------------------------------------------------------------------------------------

void WAbstractThreadAction::abortAndDelete()
{
    Q_D(WAbstractThreadAction);

    if (d->thread)
    {
        d->abort = true;
    }
    else delete this;
}

//-------------------------------------------------------------------------------------------------
// Virtual functions
//-------------------------------------------------------------------------------------------------

/* virtual */ WAbstractThreadReply * WAbstractThreadAction::createReply() const
{
    return NULL;
}

//-------------------------------------------------------------------------------------------------
// Protected properties
//-------------------------------------------------------------------------------------------------

WAbstractThreadReply * WAbstractThreadAction::reply() const
{
    Q_D(const WAbstractThreadAction); return d->reply;
}

#endif // SK_NO_ABSTRACTTHREADACTION
