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

#include "WMainViewDrag.h"

#ifndef SK_NO_MAINVIEWDRAG

// Qt includes
#include <QApplication>
#include <QScreen>
#include <QCursor>

// Sk includes
#include <WMainView>

// Private includes
#include <private/WDeclarativeMouseArea_p>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WMainViewDragPrivate : public WDeclarativeMouseAreaPrivate
{
public:
    WMainViewDragPrivate(WMainViewDrag * p);

    void init();

public: // Functions
    void clearDrag();

public: // Variables
    bool dragEnabled;
    bool dragging;

    QPoint dragLastPos;

protected:
    W_DECLARE_PUBLIC(WMainViewDrag)
};

//-------------------------------------------------------------------------------------------------

WMainViewDragPrivate::WMainViewDragPrivate(WMainViewDrag * p) : WDeclarativeMouseAreaPrivate(p) {}

void WMainViewDragPrivate::init()
{
    hoverRetain = true;

    dragEnabled = true;
    dragging    = false;
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WMainViewDragPrivate::clearDrag()
{
    dragging = false;

    view->d_func()->setDragged(false);
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

/* explicit */ WMainViewDrag::WMainViewDrag(QDeclarativeItem * parent)
    : WDeclarativeMouseArea(new WMainViewDragPrivate(this), parent)
{
    Q_D(WMainViewDrag); d->init();
}

//-------------------------------------------------------------------------------------------------
// Events
//-------------------------------------------------------------------------------------------------

/* virtual */ void WMainViewDrag::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    Q_D(WMainViewDrag);

    WDeclarativeMouseArea::mousePressEvent(event);

    if (d->dragEnabled && event->button() == Qt::LeftButton)
    {
        d->dragging = true;

        d->dragLastPos = QPoint(-1, -1);

        d->view->d_func()->setDragged(true);
    }
}

/* virtual */ void WMainViewDrag::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
    Q_D(WMainViewDrag);

    WDeclarativeMouseArea::mouseReleaseEvent(event);

    d->clearDrag();
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WMainViewDrag::mouseMoveEvent(QGraphicsSceneMouseEvent *)
{
    Q_D(WMainViewDrag);

    if (d->dragging == false) return;

#ifdef Q_OS_WIN
    ReleaseCapture();

    PostMessage((HWND) d->view->winId(), WM_NCLBUTTONDOWN, HTCAPTION, 0);
#else
    QRect geometry = d->view->availableGeometry();

    QPoint pos = QCursor::pos();

    if (d->dragLastPos != QPoint(-1, -1))
    {
        int moveX = pos.x() - d->dragLastPos.x();
        int moveY = pos.y() - d->dragLastPos.y();

        int finalX = d->view->x() + moveX;
        int finalY = d->view->y() + moveY;

        int geometryHeight = geometry.height() + geometry.y();

        if (pos.y() < geometry.y() || pos.y() > geometryHeight)
        {
            finalY = d->view->y();

            pos.setY(d->view->y());
        }

        int boxY = d->view->pos().y() + moveY;

        int maxHeight = geometryHeight - 25 - 2;

        if (boxY < geometry.y())
        {
            finalY = geometry.y();
        }
        else if (boxY > maxHeight)
        {
            finalY = maxHeight;
        }

        d->view->move(finalX, finalY);
    }

    d->dragLastPos = pos;
#endif
}

//-------------------------------------------------------------------------------------------------

/* virtual */ bool WMainViewDrag::sceneEvent(QEvent * event)
{
    bool result = WDeclarativeMouseArea::sceneEvent(event);

    if (event->type() == QEvent::UngrabMouse)
    {
        Q_D(WMainViewDrag);

        d->clearDrag();
    }

    return result;
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

bool WMainViewDrag::dragEnabled() const
{
    Q_D(const WMainViewDrag); return d->dragEnabled;
}

void WMainViewDrag::setDragEnabled(bool enabled)
{
    Q_D(WMainViewDrag);

    if (d->dragEnabled == enabled) return;

    d->dragEnabled = enabled;

    if (enabled == false)
    {
        d->clearDrag();
    }

    emit dragEnabledChanged(d->dragEnabled);
}

#endif // SK_NO_MAINVIEWDRAG
