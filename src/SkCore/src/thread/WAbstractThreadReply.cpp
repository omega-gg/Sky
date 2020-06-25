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

#include "WAbstractThreadReply.h"

#ifndef SK_NO_ABSTRACTTHREADREPLY

// Qt includes
#include <QCoreApplication>

// Sk includes
#include <WThreadActions>
#include <WAbstractThreadAction>

// Private includes
#include <private/WAbstractThreadAction_p>

//=================================================================================================
// WAbstractThreadReplyEvent
//=================================================================================================

class WAbstractThreadReplyEvent : public QEvent
{
public:
    WAbstractThreadReplyEvent(WAbstractThreadAction * action, bool ok) : QEvent(QEvent::User)
    {
        this->action = action;

        this->ok = ok;
    }

public: // Variables
    WAbstractThreadAction * action;

    bool ok;
};

//=================================================================================================
// WAbstractThreadReplyPrivate
//=================================================================================================

#include "WAbstractThreadReply_p.h"

WAbstractThreadReplyPrivate::WAbstractThreadReplyPrivate(WAbstractThreadReply * p) : WPrivate(p) {}

void WAbstractThreadReplyPrivate::init() {}

//-------------------------------------------------------------------------------------------------
// Protected ctor / dtor
//-------------------------------------------------------------------------------------------------

WAbstractThreadReply::WAbstractThreadReply() : WPrivatable(new WAbstractThreadReplyPrivate(this))
{
    Q_D(WAbstractThreadReply); d->init();
}

/* virtual */ WAbstractThreadReply::~WAbstractThreadReply() {}

//-------------------------------------------------------------------------------------------------
// Virtual interface
//-------------------------------------------------------------------------------------------------

/* virtual */ void WAbstractThreadReply::postReply(WAbstractThreadAction * action, bool ok)
{
    QCoreApplication::postEvent(this, new WAbstractThreadReplyEvent(action, ok));
}

//-------------------------------------------------------------------------------------------------
// Protected virtual functions
//-------------------------------------------------------------------------------------------------

/* virtual */ void WAbstractThreadReply::onCompleted(bool) {}

//-------------------------------------------------------------------------------------------------
// Events
//-------------------------------------------------------------------------------------------------

/* virtual */ bool WAbstractThreadReply::event(QEvent * event)
{
    if (event->type() == QEvent::User)
    {
        WAbstractThreadReplyEvent * eventReply = static_cast<WAbstractThreadReplyEvent *> (event);

        WAbstractThreadAction * action = eventReply->action;

        if (action->d_func()->abort == false)
        {
            onCompleted(eventReply->ok);
        }

        delete action;

        return true;
    }
    else return QObject::event(event);
}

#endif // SK_NO_ABSTRACTTHREADREPLY
