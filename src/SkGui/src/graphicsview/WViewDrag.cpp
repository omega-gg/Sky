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

#include "WViewDrag.h"

#ifndef SK_NO_VIEWDRAG

// Qt includes
#ifndef Q_OS_WIN
#include <QCursor>
#endif

// Sk includes
#include <WView>

#if defined(QT_5) == false && defined(Q_OS_WIN)
// Windows includes
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
    d->view->d_func()->setDragged(true);

    d->dragging = false;

    ReleaseCapture();

    // FIXME Qt5.12.3: Touch drag seems to be broken, unless we double tap.
    PostMessage((HWND) d->view->winId(), WM_SYSCOMMAND, 0xf012, 0);
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

#ifdef QT_4
        int boxY = d->view->pos().y() + moveY;
#else
        int boxY = d->view->position().y() + moveY;
#endif

#ifdef Q_OS_LINUX
        int maxHeight = geometryHeight - 27;
#else
        int maxHeight = geometryHeight;
#endif

        if (boxY < geometry.y())
        {
            finalY = geometry.y();
        }
        else if (boxY > maxHeight)
        {
            finalY = maxHeight;
        }

#ifdef QT_4
        d->view->move(finalX, finalY);
#else
        d->view->setPosition(finalX, finalY);
#endif
    }
    else d->view->d_func()->setDragged(true);

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
