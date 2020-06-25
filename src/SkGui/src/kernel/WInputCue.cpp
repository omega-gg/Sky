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

#include "WInputCue.h"

#ifndef SK_NO_INPUTCUE

// Sk includes
#include <WView>

//=================================================================================================
// WInputCueAction
//=================================================================================================

WInputCueAction::WInputCueAction()
{
    delay = 0;
}

//-------------------------------------------------------------------------------------------------
// Protected

/* virtual */ WInputCueAction::~WInputCueAction() {}

//=================================================================================================
// WInputCueId
//=================================================================================================

class SK_GUI_EXPORT WInputCueId : public WInputCueAction
{
public: // WInputCueAction implementation
    /* virtual */ void run()
    {
        emit inputCue->run(id);
    }

public: // Variables
    WInputCue * inputCue;

    int id;
};

//=================================================================================================
// WInputCueWait
//=================================================================================================

class SK_GUI_EXPORT WInputCueWait : public WInputCueAction
{
public: // WInputCueAction implementation
    /* virtual */ void run() {}
};

//=================================================================================================
// WInputCueMouseMove
//=================================================================================================

class SK_GUI_EXPORT WInputCueMouseMove : public WInputCueAction
{
public: // WInputCueAction implementation
    /* virtual */ void run()
    {
        view->mouseMove(x, y, button);
    }

public: // Variables
    WView * view;

    int x;
    int y;

    Qt::MouseButton button;
};

//=================================================================================================
// WInputCueMousePress
//=================================================================================================

class SK_GUI_EXPORT WInputCueMousePress : public WInputCueAction
{
public: // WInputCueAction implementation
    /* virtual */ void run()
    {
        view->mousePress(button);
    }

public: // Variables
    WView * view;

    Qt::MouseButton button;
};

//=================================================================================================
// WInputCueMouseRelease
//=================================================================================================

class SK_GUI_EXPORT WInputCueMouseRelease : public WInputCueAction
{
public: // WInputCueAction implementation
    /* virtual */ void run()
    {
        view->mouseRelease(button);
    }

public: // Variables
    WView * view;

    Qt::MouseButton button;
};

//=================================================================================================
// WInputCueMouseWheel
//=================================================================================================

class SK_GUI_EXPORT WInputCueWheel : public WInputCueAction
{
public: // WInputCueAction implementation
    /* virtual */ void run()
    {
        view->wheel(orientation, delta);
    }

public: // Variables
    WView * view;

    Qt::Orientation orientation;
    int             delta;
};

//=================================================================================================
// WInputCueKeyPress
//=================================================================================================

class SK_GUI_EXPORT WInputCueKeyPress : public WInputCueAction
{
public: // WInputCueAction implementation
    /* virtual */ void run()
    {
        view->keyPress(key, modifiers);
    }

public: // Variables
    WView * view;

    int                   key;
    Qt::KeyboardModifiers modifiers;
};

//=================================================================================================
// WInputCueKeyRelease
//=================================================================================================

class SK_GUI_EXPORT WInputCueKeyRelease : public WInputCueAction
{
public: // WInputCueAction implementation
    /* virtual */ void run()
    {
        view->keyRelease(key, modifiers);
    }

public: // Variables
    WView * view;

    int                   key;
    Qt::KeyboardModifiers modifiers;
};

//=================================================================================================
// WInputCuePrivate
//=================================================================================================

WInputCuePrivate::WInputCuePrivate(WInputCue * p) : WPrivate(p) {}

/* virtual */ WInputCuePrivate::~WInputCuePrivate()
{
    pause.stop();

    foreach (WInputCueAction * action, actions)
    {
        delete action;
    }
}

//-------------------------------------------------------------------------------------------------

void WInputCuePrivate::init()
{
    Q_Q(WInputCue);

    view = NULL;

    QObject::connect(&pause, SIGNAL(finished()), q, SLOT(onFinished()));
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WInputCuePrivate::processActions()
{
    if (pause.state() == QPauseAnimation::Running) return;

    WInputCueAction * action = actions.first();

    pause.setDuration(action->delay);

    pause.start();
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WInputCuePrivate::onFinished()
{
    WInputCueAction * action = actions.takeFirst();

    action->run();

    delete action;

    if (actions.isEmpty()) return;

    action = actions.first();

    pause.setDuration(action->delay);

    pause.start();
}

//=================================================================================================
// WInputCue
//=================================================================================================

/* explicit */ WInputCue::WInputCue(QObject * parent)
    : QObject(parent), WPrivatable(new WInputCuePrivate(this))
{
    Q_D(WInputCue); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WInputCue::push(WInputCueAction * action)
{
    Q_D(WInputCue);

    if (d->actions.contains(action)) return;

    d->actions.append(action);

    d->processActions();
}

/* Q_INVOKABLE */ void WInputCue::remove(WInputCueAction * action)
{
    Q_D(WInputCue);

    if (d->actions.contains(action) == false) return;

    d->actions.removeOne(action);

    if (d->actions.isEmpty())
    {
        d->pause.stop();
    }
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WInputCue::pushId(int id, int delay)
{
    WInputCueId * action = new WInputCueId;

    action->delay = delay;

    action->inputCue = this;

    action->id = id;

    push(action);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WInputCue::wait(int delay)
{
    WInputCueWait * action = new WInputCueWait;

    action->delay = delay;

    push(action);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WInputCue::clear()
{
    Q_D(WInputCue);

    d->pause.stop();

    foreach (WInputCueAction * action, d->actions)
    {
        delete action;
    }

    d->actions.clear();
}

//-------------------------------------------------------------------------------------------------
// View

/* Q_INVOKABLE */ void WInputCue::mouseMove(int delay, int x, int y, Qt::MouseButton button)
{
    Q_D(WInputCue);

    if (d->view == NULL) return;

    WInputCueMouseMove * action = new WInputCueMouseMove;

    action->delay = delay;

    action->view = d->view;

    action->x = x;
    action->y = y;

    action->button = button;

    push(action);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WInputCue::mousePress(int delay, Qt::MouseButton button)
{
    Q_D(WInputCue);

    if (d->view == NULL) return;

    WInputCueMousePress * action = new WInputCueMousePress;

    action->delay = delay;

    action->view = d->view;

    action->button = button;

    push(action);
}

/* Q_INVOKABLE */ void WInputCue::mouseRelease(int delay, Qt::MouseButton button)
{
    Q_D(WInputCue);

    if (d->view == NULL) return;

    WInputCueMouseRelease * action = new WInputCueMouseRelease;

    action->delay = delay;

    action->view = d->view;

    action->button = button;

    push(action);
}

/* Q_INVOKABLE */ void WInputCue::mouseClick(int delay, Qt::MouseButton button, int msec)
{
    mousePress  (delay, button);
    mouseRelease(msec,  button);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WInputCue::wheel(int delay, Qt::Orientation orientation, int delta)
{
    Q_D(WInputCue);

    if (d->view == NULL) return;

    WInputCueWheel * action = new WInputCueWheel;

    action->delay = delay;

    action->view = d->view;

    action->orientation = orientation;
    action->delta       = delta;

    push(action);
}

/* Q_INVOKABLE */ void WInputCue::wheelUp(int delay, int delta)
{
    wheel(delay, Qt::Vertical, delta);
}

/* Q_INVOKABLE */ void WInputCue::wheelDown(int delay, int delta)
{
    wheel(delay, Qt::Vertical, delta);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WInputCue::keyPress(int delay, int key, Qt::KeyboardModifiers modifiers)
{
    Q_D(WInputCue);

    if (d->view == NULL) return;

    WInputCueKeyPress * action = new WInputCueKeyPress;

    action->delay = delay;

    action->view = d->view;

    action->key       = key;
    action->modifiers = modifiers;

    push(action);
}

/* Q_INVOKABLE */ void WInputCue::keyRelease(int delay, int key, Qt::KeyboardModifiers modifiers)
{
    Q_D(WInputCue);

    if (d->view == NULL) return;

    WInputCueKeyRelease * action = new WInputCueKeyRelease;

    action->delay = delay;

    action->view = d->view;

    action->key       = key;
    action->modifiers = modifiers;

    push(action);
}

/* Q_INVOKABLE */ void WInputCue::keyClick(int delay, int key, Qt::KeyboardModifiers modifiers,
                                                               int                   msec)
{
    keyPress  (delay, key, modifiers);
    keyRelease(msec,  key, modifiers);
}

//-------------------------------------------------------------------------------------------------
// QML

/* Q_INVOKABLE */ void WInputCue::mouseMove(int delay, int x, int y, int button)
{
    mouseMove(delay, x, y, static_cast<Qt::MouseButton> (button));
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WInputCue::mousePress(int delay, int button)
{
    mousePress(delay, static_cast<Qt::MouseButton> (button));
}

/* Q_INVOKABLE */ void WInputCue::mouseRelease(int delay, int button)
{
    mouseRelease(delay, static_cast<Qt::MouseButton> (button));
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WInputCue::mouseClick(int delay, int button, int msec)
{
    mouseClick(delay, static_cast<Qt::MouseButton> (button), msec);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WInputCue::wheel(int delay, int orientation, int delta)
{
    wheel(delay, static_cast<Qt::Orientation> (orientation), delta);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WInputCue::keyPress(int delay, int key, int modifiers)
{
    keyPress(delay, key, static_cast<Qt::KeyboardModifiers> (modifiers));
}

/* Q_INVOKABLE */ void WInputCue::keyRelease(int delay, int key, int modifiers)
{
    keyRelease(delay, key, static_cast<Qt::KeyboardModifiers> (modifiers));
}

/* Q_INVOKABLE */ void WInputCue::keyClick(int delay, int key, int modifiers, int msec)
{
    keyClick(delay, key, static_cast<Qt::KeyboardModifiers> (modifiers), msec);
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

WView * WInputCue::view() const
{
    Q_D(const WInputCue); return d->view;
}

void WInputCue::setView(WView * view)
{
    Q_D(WInputCue);

    if (d->view == view) return;

    d->view = view;

    emit viewChanged();
}

#endif // SK_NO_INPUTCUE
