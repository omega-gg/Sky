//=================================================================================================
/*
    Copyright (C) 2015-2016 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkGui module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#include "WActionCue.h"

#ifndef SK_NO_ACTIONCUE

//-------------------------------------------------------------------------------------------------
// Static variables

static const int ACTIONCUE_MAX_CUED = 2;

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

WActionCuePrivate::WActionCuePrivate(WActionCue * p) : WPrivate(p) {}

void WActionCuePrivate::init()
{
    Q_Q(WActionCue);

    enabled = true;

    active = false;

    maxCued = ACTIONCUE_MAX_CUED;

    QObject::connect(&pause, SIGNAL(finished()), q, SLOT(onFinished()));
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WActionCuePrivate::setActive(bool active)
{
    if (this->active == active) return;

    Q_Q(WActionCue);

    this->active = active;

    emit q->activeChanged();
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WActionCuePrivate::onFinished()
{
    if (actions.isEmpty())
    {
        setActive(false);

        return;
    }

    Q_Q(WActionCue);

    int id = actions.takeFirst();

    q->processAction(id);

    if (pause.state() != QPauseAnimation::Running)
    {
        actions.clear();

        setActive(false);
    }
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

/* explicit */ WActionCue::WActionCue(QObject * parent)
    : QObject(parent), WPrivatable(new WActionCuePrivate(this))
{
    Q_D(WActionCue); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ bool WActionCue::tryPush(int id)
{
    Q_D(WActionCue);

    if (d->enabled && d->pause.state() == QPauseAnimation::Running)
    {
        if (d->actions.count() < d->maxCued)
        {
            d->actions.append(id);
        }

        return true;
    }
    else return false;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ bool WActionCue::start(int msec)
{
    Q_D(WActionCue);

    if (d->enabled == false || d->pause.state() == QPauseAnimation::Running) return false;

    d->pause.setDuration(msec);

    d->pause.start();

    d->setActive(true);

    return true;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WActionCue::clear()
{
    Q_D(WActionCue);

    d->actions.clear();

    if (d->active == false) return;

    d->pause.stop();

    d->active = false;

    emit activeChanged();
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

bool WActionCue::isEnabled() const
{
    Q_D(const WActionCue); return d->enabled;
}

void WActionCue::setEnabled(bool enabled)
{
    Q_D(WActionCue);

    if (d->enabled == enabled) return;

    d->enabled = enabled;

    if (d->enabled == false) clear();

    emit enabledChanged();
}

//-------------------------------------------------------------------------------------------------

bool WActionCue::isActive() const
{
    Q_D(const WActionCue); return d->active;
}

//-------------------------------------------------------------------------------------------------

int WActionCue::maxCued() const
{
    Q_D(const WActionCue); return d->maxCued;
}

void WActionCue::setMaxCued(int max)
{
    Q_D(WActionCue);

    if (d->maxCued == max) return;

    d->maxCued = max;

    emit maxCuedChanged();
}

#endif // SK_NO_ACTIONCUE
