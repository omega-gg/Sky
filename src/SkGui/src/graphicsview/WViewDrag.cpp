//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkGui module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#include "WViewDrag.h"

#ifndef SK_NO_VIEWDRAG

// Qt includes
#ifndef Q_OS_WIN
#include <QCursor>
#endif

// Sk includes
#include <WView>

// Windows includes
#if defined(Q_OS_WIN) && defined(SK_WIN_NATIVE) == false
#include <qt_windows.h>
#endif

// Private includes
#include <private/WDeclarativeMouseArea_p>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WViewDragPrivate : public WDeclarativeMouseAreaPrivate
{
public:
    WViewDragPrivate(WViewDrag * p);

    void init();

public: // Functions
    void clearDrag();

public: // Variables
    bool dragEnabled;
    bool dragging;

#ifndef Q_OS_WIN
    QPoint dragLastPos;
#endif

protected:
    W_DECLARE_PUBLIC(WViewDrag)
};

//-------------------------------------------------------------------------------------------------

WViewDragPrivate::WViewDragPrivate(WViewDrag * p) : WDeclarativeMouseAreaPrivate(p) {}

void WViewDragPrivate::init()
{
    hoverRetain = true;

    dragEnabled = true;
    dragging    = false;
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WViewDragPrivate::clearDrag()
{
    dragging = false;

    view->d_func()->setDragged(false);
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
/* explicit */ WViewDrag::WViewDrag(QDeclarativeItem * parent)
#else
/* explicit */ WViewDrag::WViewDrag(QQuickItem * parent)
#endif
    : WDeclarativeMouseArea(new WViewDragPrivate(this), parent)
{
    Q_D(WViewDrag); d->init();
}

//-------------------------------------------------------------------------------------------------
// Events
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
/* virtual */ void WViewDrag::mousePressEvent(QGraphicsSceneMouseEvent * event)
#else
/* virtual */ void WViewDrag::mousePressEvent(QMouseEvent * event)
#endif
{
    Q_D(WViewDrag);

    WDeclarativeMouseArea::mousePressEvent(event);

    if (d->dragEnabled && event->button() == Qt::LeftButton)
    {
        d->dragging = true;

#ifndef Q_OS_WIN
        d->dragLastPos = QPoint(-1, -1);
#endif

        d->view->d_func()->setDragged(true);
    }
}

#ifdef QT_4
/* virtual */ void WViewDrag::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
#else
/* virtual */ void WViewDrag::mouseReleaseEvent(QMouseEvent * event)
#endif
{
    Q_D(WViewDrag);

    WDeclarativeMouseArea::mouseReleaseEvent(event);

    d->clearDrag();
}

//-------------------------------------------------------------------------------------------------

#ifdef QT_4
/* virtual */ void WViewDrag::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
#else
/* virtual */ void WViewDrag::mouseMoveEvent(QMouseEvent * event)
#endif
{
    Q_D(WViewDrag);

    if (d->dragging == false)
    {
        WDeclarativeMouseArea::mouseMoveEvent(event);

        return;
    }

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

        int boxY = d->view->position().y() + moveY;

        int maxHeight = geometryHeight - 27;

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

#ifdef QT_4

/* virtual */ bool WViewDrag::sceneEvent(QEvent * event)
{
    bool result = WDeclarativeMouseArea::sceneEvent(event);

    if (event->type() == QEvent::UngrabMouse)
    {
        Q_D(WViewDrag);

        d->clearDrag();
    }

    return result;
}

#else

/* virtual */ void WViewDrag::mouseUngrabEvent()
{
    Q_D(WViewDrag);

    d->clearDrag();
}

#endif

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

bool WViewDrag::dragEnabled() const
{
    Q_D(const WViewDrag); return d->dragEnabled;
}

void WViewDrag::setDragEnabled(bool enabled)
{
    Q_D(WViewDrag);

    if (d->dragEnabled == enabled) return;

    d->dragEnabled = enabled;

    if (enabled == false)
    {
        d->clearDrag();
    }

    emit dragEnabledChanged(d->dragEnabled);
}

#endif // SK_NO_VIEWDRAG
