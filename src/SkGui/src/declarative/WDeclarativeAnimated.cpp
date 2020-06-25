//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkGui.

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

#include "WDeclarativeAnimated.h"

#ifndef SK_NO_DECLARATIVEANIMATED

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

WDeclarativeAnimatedPrivate::WDeclarativeAnimatedPrivate(WDeclarativeAnimated * p)
    : WDeclarativeItemPrivate(p) {}

void WDeclarativeAnimatedPrivate::init()
{
    Q_Q(WDeclarativeAnimated);

    running = true;

    stepDirection = WDeclarativeAnimated::StepForward;
    stepMode      = WDeclarativeAnimated::StepAuto;

    stepCount =  0;
    step      = -1;

    loopCount = 1;
    loop      = 0;

#ifdef QT_4
    q->setFlag(QGraphicsItem::ItemHasNoContents, false);
#endif

    QObject::connect(&pause, SIGNAL(finished()), q, SLOT(onFinished()));
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WDeclarativeAnimatedPrivate::start()
{
    if (stepDirection == WDeclarativeAnimated::StepForward)
    {
         setStep(0);
    }
    else setStep(stepCount - 1);

    if (stepMode == WDeclarativeAnimated::StepManual)
    {
        Q_Q(WDeclarativeAnimated);

        q->setRunning(false);
    }
    else pause.start();
}

void WDeclarativeAnimatedPrivate::stop()
{
    pause.stop();

    setStep(-1);
    setLoop( 0);
}

//-------------------------------------------------------------------------------------------------

void WDeclarativeAnimatedPrivate::update()
{
    Q_Q(WDeclarativeAnimated);

    if (q->isComponentComplete() && q->isVisible())
    {
        if (running && stepCount && loopCount)
        {
             start();
        }
        else stop();
    }
}

//-------------------------------------------------------------------------------------------------

void WDeclarativeAnimatedPrivate::setStep(int step)
{
    if (this->step == step) return;

    Q_Q(WDeclarativeAnimated);

    this->step = step;

    emit q->stepChanged();
}

void WDeclarativeAnimatedPrivate::setLoop(int loop)
{
    if (this->loop == loop) return;

    Q_Q(WDeclarativeAnimated);

    this->loop = loop;

    emit q->loopChanged();
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WDeclarativeAnimatedPrivate::onFinished()
{
    Q_Q(WDeclarativeAnimated);

    if (stepDirection == WDeclarativeAnimated::StepForward)
    {
        if (step == stepCount - 1)
        {
            if (loopCount != -1)
            {
                loop++;

                if (loop == loopCount)
                {
                    q->setRunning(false);

                    emit q->loopChanged();

                    return;
                }
                else emit q->loopChanged();
            }

            step = 0;
        }
        else step++;
    }
    else if (step == 0)
    {
        if (loopCount != -1)
        {
            loop++;

            if (loop == loopCount)
            {
                q->setRunning(false);

                emit q->loopChanged();

                return;
            }
            else emit q->loopChanged();
        }

        step = stepCount - 1;
    }
    else step--;

    if (stepMode == WDeclarativeAnimated::StepManual)
    {
        q->setRunning(false);

        emit q->stepChanged();
    }
    else
    {
        emit q->stepChanged();

        if (running) pause.start();
    }
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
/* explicit */ WDeclarativeAnimated::WDeclarativeAnimated(QDeclarativeItem * parent)
#else
/* explicit */ WDeclarativeAnimated::WDeclarativeAnimated(QQuickItem * parent)
#endif
    : WDeclarativeItem(new WDeclarativeAnimatedPrivate(this), parent)
{
    Q_D(WDeclarativeAnimated); d->init();
}

//-------------------------------------------------------------------------------------------------
// Protected

#ifdef QT_4
WDeclarativeAnimated::WDeclarativeAnimated(WDeclarativeAnimatedPrivate * p,
                                           QDeclarativeItem            * parent)
#else
WDeclarativeAnimated::WDeclarativeAnimated(WDeclarativeAnimatedPrivate * p, QQuickItem * parent)
#endif
    : WDeclarativeItem(p, parent)
{
    Q_D(WDeclarativeAnimated); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WDeclarativeAnimated::start()
{
    setRunning(true);
}

/* Q_INVOKABLE */ void WDeclarativeAnimated::restart()
{
    stop();

    setRunning(true);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WDeclarativeAnimated::pause()
{
    setRunning(false);
}

/* Q_INVOKABLE */ void WDeclarativeAnimated::stop()
{
    Q_D(WDeclarativeAnimated);

    setRunning(false);

    d->setStep(-1);
    d->setLoop( 0);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WDeclarativeAnimated::stepBackward()
{
    Q_D(WDeclarativeAnimated);

    if (d->stepDirection == StepForward)
    {
        d->stepDirection = StepBackward;

        setRunning(false);

        emit stepDirectionChanged();
    }
    else setRunning(true);
}

/* Q_INVOKABLE */ void WDeclarativeAnimated::stepForward()
{
    Q_D(WDeclarativeAnimated);

    if (d->stepDirection == StepBackward)
    {
        d->stepDirection = StepForward;

        setRunning(false);

        emit stepDirectionChanged();
    }
    else setRunning(true);
}

//-------------------------------------------------------------------------------------------------
// QDeclarativeItem / QQuickItem reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativeAnimated::componentComplete()
{
    Q_D(WDeclarativeAnimated);

    WDeclarativeItem::componentComplete();

    if (isVisible() && d->running && d->stepCount && d->loopCount)
    {
        d->start();
    }
}

//-------------------------------------------------------------------------------------------------
// Protected QGraphicsItem / QQuickItem reimplementation
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
/* virtual */ QVariant WDeclarativeAnimated::itemChange(GraphicsItemChange change,
                                                        const QVariant &   value)
#else
/* virtual */ void WDeclarativeAnimated::itemChange(ItemChange change, const ItemChangeData & value)
#endif
{
#ifdef QT_4
    if (change == ItemVisibleHasChanged)
    {
        Q_D(WDeclarativeAnimated);

        if (value.toBool())
#else
    Q_D(WDeclarativeAnimated);

    if (d->view && change == ItemVisibleHasChanged)
    {
        if (value.boolValue)
#endif
        {
            if (d->running && d->stepCount && d->loopCount)
            {
                d->start();
            }
        }
        else d->stop();
    }

#ifdef QT_4
    return WDeclarativeItem::itemChange(change, value);
#else
    WDeclarativeItem::itemChange(change, value);
#endif
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

bool WDeclarativeAnimated::running() const
{
    Q_D(const WDeclarativeAnimated); return d->running;
}

void WDeclarativeAnimated::setRunning(bool running)
{
    Q_D(WDeclarativeAnimated);

    if (d->running == running) return;

    if (isComponentComplete() && isVisible())
    {
        if (running)
        {
            if (d->stepCount == 0 || d->loopCount == 0) return;

            if (d->step == -1)
            {
                d->running = true;

                emit runningChanged();

                if (d->running)
                {
                    d->start();
                }
            }
            else if ((d->loopCount == -1 || d->loop != (d->loopCount - 1))
                     ||
                     (d->stepDirection == StepForward  && d->step != (d->stepCount - 1))
                     ||
                     (d->stepDirection == StepBackward && d->step))
            {
                d->running = true;

                emit runningChanged();

                if (d->running)
                {
                    d->onFinished();
                }
            }
        }
        else
        {
            d->running = false;

            d->pause.stop();

            emit runningChanged();
        }
    }
    else
    {
        d->running = running;

        emit runningChanged();
    }
}

//-------------------------------------------------------------------------------------------------

WDeclarativeAnimated::StepDirection WDeclarativeAnimated::stepDirection() const
{
    Q_D(const WDeclarativeAnimated); return d->stepDirection;
}

void WDeclarativeAnimated::setStepDirection(StepDirection direction)
{
    Q_D(WDeclarativeAnimated);

    if (d->stepDirection == direction) return;

    d->stepDirection = direction;

    emit stepDirectionChanged();
}

//-------------------------------------------------------------------------------------------------

bool WDeclarativeAnimated::isBackward() const
{
    Q_D(const WDeclarativeAnimated); return (d->stepDirection == StepBackward);
}

bool WDeclarativeAnimated::isForward() const
{
    Q_D(const WDeclarativeAnimated); return (d->stepDirection == StepForward);
}

//-------------------------------------------------------------------------------------------------

WDeclarativeAnimated::StepMode WDeclarativeAnimated::stepMode() const
{
    Q_D(const WDeclarativeAnimated); return d->stepMode;
}

void WDeclarativeAnimated::setStepMode(StepMode mode)
{
    Q_D(WDeclarativeAnimated);

    if (d->stepMode == mode) return;

    d->stepMode = mode;

    emit stepModeChanged();
}

//-------------------------------------------------------------------------------------------------

int WDeclarativeAnimated::stepCount() const
{
    Q_D(const WDeclarativeAnimated); return d->stepCount;
}

void WDeclarativeAnimated::setStepCount(int count)
{
    Q_D(WDeclarativeAnimated);

    count = qMax(0, count);

    if (d->stepCount == count) return;

    d->stepCount = count;

    if (d->step > count - 1)
    {
        stop();
    }

    d->update();

    emit stepCountChanged();
}

//-------------------------------------------------------------------------------------------------

int WDeclarativeAnimated::step() const
{
    Q_D(const WDeclarativeAnimated); return d->step;
}

void WDeclarativeAnimated::setStep(int step)
{
    Q_D(WDeclarativeAnimated);

    step = qBound(0, step, d->stepCount - 1);

    d->setStep(step);
}

//-------------------------------------------------------------------------------------------------

int WDeclarativeAnimated::interval() const
{
    Q_D(const WDeclarativeAnimated); return d->pause.duration();
}

void WDeclarativeAnimated::setInterval(int interval)
{
    Q_D(WDeclarativeAnimated);

    if (d->loopCount == -1 && interval == 0)
    {
        interval = 1;
    }

    int duration = d->pause.duration();

    d->pause.setDuration(interval);

    if (d->pause.duration() != duration)
    {
        emit intervalChanged();
    }
}

//-------------------------------------------------------------------------------------------------

int WDeclarativeAnimated::loopCount() const
{
    Q_D(const WDeclarativeAnimated); return d->loopCount;
}

void WDeclarativeAnimated::setLoopCount(int count)
{
    Q_D(WDeclarativeAnimated);

    count = qMax(-1, count);

    if (d->loopCount == count) return;

    d->loopCount = count;

    if (count == -1 && d->pause.duration() == 0)
    {
        d->pause.setDuration(1);

        emit intervalChanged();
    }

    d->update();

    emit loopCountChanged();
}

//-------------------------------------------------------------------------------------------------

int WDeclarativeAnimated::loop() const
{
    Q_D(const WDeclarativeAnimated); return d->loop;
}

#endif // SK_NO_DECLARATIVEANIMATED
