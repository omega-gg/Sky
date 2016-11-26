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

#include "WDeclarativeMouseWatcher.h"

#ifndef SK_NO_DECLARATIVEMOUSEWATCHER

// Qt includes
#include <QGraphicsSceneMouseEvent>

// Sk includes
#include <WControllerView>
#include <WView>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

WDeclarativeMouseWatcherPrivate::WDeclarativeMouseWatcherPrivate(WDeclarativeMouseWatcher * p)
    : WDeclarativeItemPrivate(p) {}

void WDeclarativeMouseWatcherPrivate::init() {}

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

/* explicit */ WDeclarativeMouseWatcher::WDeclarativeMouseWatcher(QDeclarativeItem * parent)
    : WDeclarativeItem(new WDeclarativeMouseWatcherPrivate(this), parent)
{
    Q_D(WDeclarativeMouseWatcher); d->init();
}

//-------------------------------------------------------------------------------------------------
// Events
//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativeMouseWatcher::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    Q_D(WDeclarativeMouseWatcher);

    event->ignore();

    connect(d->view, SIGNAL(pressedChanged()), this, SLOT(onPressedChanged()));

    emit pressed();
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

Qt::MouseButtons WDeclarativeMouseWatcher::acceptedButtons() const
{
    return acceptedMouseButtons();
}

void WDeclarativeMouseWatcher::setAcceptedButtons(Qt::MouseButtons buttons)
{
    if (buttons != acceptedMouseButtons())
    {
        setAcceptedMouseButtons(buttons);

        emit acceptedButtonsChanged();
    }
}

#endif // SK_NO_DECLARATIVEMOUSEWATCHER
