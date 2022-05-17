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

#include "WDeclarativeMouseWatcher.h"

#ifndef SK_NO_DECLARATIVEMOUSEWATCHER

#ifdef QT_4
// Qt includes
#include <QGraphicsSceneMouseEvent>
#endif

// Sk includes
#include <WControllerView>
#include <WView>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

WDeclarativeMouseWatcherPrivate::WDeclarativeMouseWatcherPrivate(WDeclarativeMouseWatcher * p)
    : WDeclarativeItemPrivate(p) {}

void WDeclarativeMouseWatcherPrivate::init()
{
    Q_Q(WDeclarativeMouseWatcher);

#ifdef QT_5
    touch = false;
#endif

#ifdef QT_NEW
    // NOTE: We just want to make sure all the press events have been processed.
    timer.setInterval(1);

    timer.setSingleShot(true);

    QObject::connect(&timer, SIGNAL(timeout()), q, SIGNAL(released()));
#endif

    q->setAcceptedMouseButtons(Qt::LeftButton);

#ifdef QT_6
    q->setAcceptTouchEvents(true);
#endif
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WDeclarativeMouseWatcherPrivate::onPressedChanged()
{
    Q_Q(WDeclarativeMouseWatcher);

    QObject::disconnect(view, SIGNAL(pressedChanged()), q, SLOT(onPressedChanged()));

    emit q->released();
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
/* explicit */ WDeclarativeMouseWatcher::WDeclarativeMouseWatcher(QDeclarativeItem * parent)
#else
/* explicit */ WDeclarativeMouseWatcher::WDeclarativeMouseWatcher(QQuickItem * parent)
#endif
    : WDeclarativeItem(new WDeclarativeMouseWatcherPrivate(this), parent)
{
    Q_D(WDeclarativeMouseWatcher); d->init();
}

//-------------------------------------------------------------------------------------------------
// Events
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
/* virtual */ void WDeclarativeMouseWatcher::mousePressEvent(QGraphicsSceneMouseEvent * event)
#else
/* virtual */ void WDeclarativeMouseWatcher::mousePressEvent(QMouseEvent * event)
#endif
{
    Q_D(WDeclarativeMouseWatcher);

    event->ignore();

#ifdef QT_5
    // NOTE: When we receive a touch event we have to trigger the release signal manually.
    if (d->touch)
    {
        d->touch = false;

        emit pressed();

        d->timer.start();

        return;
    }
#endif

    connect(d->view, SIGNAL(pressedChanged()), this, SLOT(onPressedChanged()));

    emit pressed();
}

#ifdef QT_NEW

/* virtual */ void WDeclarativeMouseWatcher::touchEvent(QTouchEvent * event)
{
    Q_D(WDeclarativeMouseWatcher);

    event->ignore();

#ifdef QT_5
    d->touch = true;
#else
    emit pressed();

    d->timer.start();
#endif
}

#endif

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

Qt::MouseButtons WDeclarativeMouseWatcher::acceptedButtons() const
{
    return acceptedMouseButtons();
}

void WDeclarativeMouseWatcher::setAcceptedButtons(Qt::MouseButtons buttons)
{
    if (buttons == acceptedMouseButtons()) return;

    setAcceptedMouseButtons(buttons);

    emit acceptedButtonsChanged();
}

#endif // SK_NO_DECLARATIVEMOUSEWATCHER
