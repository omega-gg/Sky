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

#include "WDeclarativeMouseArea.h"

#ifndef SK_NO_DECLARATIVEMOUSEAREA

#ifdef QT_4
// Qt includes
#include <QCoreApplication>
#include <QGraphicsScene>
#include <QGraphicsSceneWheelEvent>
#endif

// Sk includes
#include <WView>

//-------------------------------------------------------------------------------------------------
// Static variables

static const int MOUSEAREA_DELAY_TOUCH = 200;

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

WDeclarativeMouseAreaPrivate::WDeclarativeMouseAreaPrivate(WDeclarativeMouseArea * p)
    : WPrivate(p) {}

/* virtual */ WDeclarativeMouseAreaPrivate::~WDeclarativeMouseAreaPrivate()
{
    clearView();
}

//-------------------------------------------------------------------------------------------------

void WDeclarativeMouseAreaPrivate::init()
{
    Q_Q(WDeclarativeMouseArea);

    view = NULL;

    hoverActive = false;
    hoverRetain = false;

    wheelEnabled = false;

#ifdef QT_NEW
    dropEnabled = false;
#endif

    dragAccepted = false;

    cursor     = Qt::ArrowCursor;
    cursorDrop = Qt::ArrowCursor;

    QObject::connect(q, SIGNAL(hoverEnabledChanged()), q, SLOT(onClearHover()));
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

bool WDeclarativeMouseAreaPrivate::dragEnterEvent(const QPointF & pos, WViewDragData * data)
{
    Q_Q(WDeclarativeMouseArea);

    WDeclarativeDropEvent event(pos.x(), pos.y(), data->text, data->actions, data->action);

    emit q->dragEntered(&event);

    if (event.isAccepted())
    {
        data->action = event.action();

        data->text = event.text();

        setDragAccepted(true);

        return true;
    }
    else return false;
}

void WDeclarativeMouseAreaPrivate::dragLeaveEvent()
{
    Q_Q(WDeclarativeMouseArea);

    WDeclarativeDropEvent event(-1, -1, QString());

    emit q->dragExited(&event);

    setDragAccepted(false);
}

void WDeclarativeMouseAreaPrivate::dragMoveEvent(const QPointF       & pos,
                                                 const WViewDragData & data)
{
    Q_Q(WDeclarativeMouseArea);

    WDeclarativeDropEvent event(pos.x(), pos.y(), data.text, data.actions, data.action);

    emit q->dragMove(&event);
}

void WDeclarativeMouseAreaPrivate::dropEvent(const QPointF       & pos,
                                             const WViewDragData & data)
{
    Q_Q(WDeclarativeMouseArea);

    WDeclarativeDropEvent event(pos.x(), pos.y(), data.text, data.actions, data.action);

    emit q->drop(&event);

    setDragAccepted(false);
}

//-------------------------------------------------------------------------------------------------

void WDeclarativeMouseAreaPrivate::setHoverActive(bool active)
{
    if (hoverActive == active) return;

    Q_Q(WDeclarativeMouseArea);

    hoverActive = active;

    emit q->hoverActiveChanged();

    if (active) emit q->hoverEntered();
    else        emit q->hoverExited ();
}

void WDeclarativeMouseAreaPrivate::setDragAccepted(bool accepted)
{
    if (dragAccepted == accepted) return;

    Q_Q(WDeclarativeMouseArea);

    dragAccepted = accepted;

    emit q->dragAcceptedChanged();
}

//-------------------------------------------------------------------------------------------------

void WDeclarativeMouseAreaPrivate::clearHover()
{
    if (view == NULL) return;

    Q_Q(WDeclarativeMouseArea);

#ifdef QT_4
    QList<QGraphicsItem *> * items = &(view->d_func()->itemsCursor);
#else
    QList<QQuickItem *> * items = &(view->d_func()->itemsCursor);
#endif

    if (items->contains(q))
    {
        items->clear();
    }
}

void WDeclarativeMouseAreaPrivate::clearDrop()
{
    if (view == NULL) return;

    Q_Q(WDeclarativeMouseArea);

#ifdef QT_4
    QList<QGraphicsItem *> * items = &(view->d_func()->itemsDrop);
#else
    QList<QQuickItem *> * items = &(view->d_func()->itemsDrop);
#endif

    if (items->contains(q))
    {
        items->clear();
    }
}

void WDeclarativeMouseAreaPrivate::clearView()
{
    if (view == NULL) return;

    Q_Q(WDeclarativeMouseArea);

    if (hoverActive)
    {
        hoverActive = false;

        view->d_func()->itemsHovered.removeOne(q);

        emit q->hoverActiveChanged();

        emit q->hoverExited();
    }

    if (view->d_func()->areaDrop == q)
    {
        WDeclarativeDropEvent event(-1, -1, QString());

        emit q->dragExited(&event);

        view->d_func()->areaDrop = NULL;
    }
}

//-------------------------------------------------------------------------------------------------
// Private events
//-------------------------------------------------------------------------------------------------

void WDeclarativeMouseAreaPrivate::onClearHover()
{
    clearHover();
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
/* explicit */ WDeclarativeMouseArea::WDeclarativeMouseArea(QDeclarativeItem * parent)
    : QDeclarativeMouseArea(parent), WPrivatable(new WDeclarativeMouseAreaPrivate(this))
#else
/* explicit */ WDeclarativeMouseArea::WDeclarativeMouseArea(QQuickItem * parent)
    : QQuickMouseArea(parent), WPrivatable(new WDeclarativeMouseAreaPrivate(this))
#endif
{
    Q_D(WDeclarativeMouseArea); d->init();
}

//-------------------------------------------------------------------------------------------------
// Protected

#ifdef QT_4
WDeclarativeMouseArea::WDeclarativeMouseArea(WDeclarativeMouseAreaPrivate * p,
                                             QDeclarativeItem             * parent)
    : QDeclarativeMouseArea(parent), WPrivatable(p)
#else
WDeclarativeMouseArea::WDeclarativeMouseArea(WDeclarativeMouseAreaPrivate * p, QQuickItem * parent)
    : QQuickMouseArea(parent), WPrivatable(p)
#endif
{
    Q_D(WDeclarativeMouseArea); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WDeclarativeMouseArea::press(Qt::MouseButton button)
{
    Q_D(WDeclarativeMouseArea);

    QPoint position = d->view->d_func()->mousePos;

    QMouseEvent event(QEvent::MouseButtonPress, position, d->view->mapToGlobal(position),
                      button, button, Qt::NoModifier);

    QCoreApplication::sendEvent(this, &event);
}

/* Q_INVOKABLE */ void WDeclarativeMouseArea::release(Qt::MouseButton button)
{
    Q_D(WDeclarativeMouseArea);

    QPoint position = d->view->d_func()->mousePos;

    QMouseEvent event(QEvent::MouseButtonRelease, position, d->view->mapToGlobal(position),
                      button, Qt::NoButton, Qt::NoModifier);

    QCoreApplication::sendEvent(this, &event);
}

/* Q_INVOKABLE */ void WDeclarativeMouseArea::click(Qt::MouseButton button)
{
    press  (button);
    release(button);
}

//-------------------------------------------------------------------------------------------------
// QML

/* Q_INVOKABLE */ void WDeclarativeMouseArea::press(int button)
{
    press(static_cast<Qt::MouseButton> (button));
}

/* Q_INVOKABLE */ void WDeclarativeMouseArea::release(int button)
{
    release(static_cast<Qt::MouseButton> (button));
}

/* Q_INVOKABLE */ void WDeclarativeMouseArea::click(int button)
{
    click(static_cast<Qt::MouseButton> (button));
}

//-------------------------------------------------------------------------------------------------
// Protected QGraphicsItem / QQuickItem reimplementation
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
/* virtual */ QVariant WDeclarativeMouseArea::itemChange(GraphicsItemChange change,
                                                         const QVariant &   value)
#else
/* virtual */ void WDeclarativeMouseArea::itemChange(ItemChange             change,
                                                     const ItemChangeData & value)
#endif
{
#ifdef QT_4
    if (change == ItemSceneHasChanged)
#else
    if (change == ItemSceneChange)
#endif
    {
        Q_D(WDeclarativeMouseArea);

#ifdef QT_4
        QGraphicsScene * scene = qvariant_cast<QGraphicsScene *> (value);

        WViewScene * mainScene = static_cast<WViewScene *> (scene);

        if (mainScene == NULL)
        {
            d->clearView();

            d->view = NULL;
        }
        else d->view = mainScene->view;
#else
        QQuickWindow * window = value.window;

        if (window == NULL)
        {
            d->clearView();

            d->view = NULL;
        }
        else d->view = static_cast<WView *> (window);
#endif

        emit viewChanged();
    }

#ifdef QT_4
    return QDeclarativeMouseArea::itemChange(change, value);
#else
    QQuickMouseArea::itemChange(change, value);
#endif
}

//-------------------------------------------------------------------------------------------------
// Events
//-------------------------------------------------------------------------------------------------

#ifdef QT_NEW

/* virtual */ void WDeclarativeMouseArea::mouseDoubleClickEvent(QMouseEvent * event)
{
    Q_D(WDeclarativeMouseArea);

    // NOTE Qt5.15: We handle double click for touch ourselves because it seems broken.
    if (d->view == NULL || d->view->d_func()->touchId == -1)
    {
        QQuickMouseArea::mouseDoubleClickEvent(event);

        return;
    }
}

/* virtual */ void WDeclarativeMouseArea::touchEvent(QTouchEvent * event)
{
    Q_D(WDeclarativeMouseArea);

    if (d->view == NULL || (acceptedButtons() == Qt::NoButton))
    {
        QQuickMouseArea::touchEvent(event);

        return;
    }

#ifdef QT_5
    const QList<QTouchEvent::TouchPoint> & points = event->touchPoints();
#else
    const QList<QTouchEvent::TouchPoint> & points = event->points();
#endif

    WViewPrivate * p = d->view->d_func();

    int id = p->touchId;

    if (id == -1)
    {
        if (points.isEmpty())
        {
            QQuickMouseArea::touchEvent(event);

            return;
        }

        QTouchEvent::TouchPoint point = points.first();

#ifdef QT_5
        if (point.state() == Qt::TouchPointPressed)
#else
        if (point.state() == QEventPoint::Pressed)
#endif
        {
#ifdef QT_5
            QPointF screenPos = point.screenPos();
#else
            QPointF screenPos = point.globalPosition();
#endif

            // NOTE: This is useful for WView::mouseX and mouseY.
            p->setMousePos(screenPos.toPoint());

            // NOTE: This call requires the mouse position to update hovering.
            p->setEntered(true);

            p->setTouch(point.id());
        }
    }
    else
    {
        foreach (const QTouchEvent::TouchPoint & point, points)
        {
            if (point.id() != id) continue;

#ifdef QT_5
            if (point.state() == Qt::TouchPointMoved)
#else
            if (point.state() == QEventPoint::Updated)
#endif
            {
#ifdef QT_5
                QPointF screenPos = point.screenPos();
#else
                QPointF screenPos = point.globalPosition();
#endif

                // NOTE: This is useful for WView::mouseX and mouseY.
                p->setMousePos(screenPos.toPoint());
            }
#ifdef QT_5
            else if (point.state() == Qt::TouchPointReleased)
#else
            else if (point.state() == QEventPoint::Released)
#endif
            {
                // NOTE Qt5.15: We handle double click for touch ourselves because it seems broken.
                if (p->touchItem == this && p->touchTimer.isActive())
                {
                    p->touchTimer.stop();

                    p->touchItem = NULL;

#ifdef QT_5
                    QPointF screenPos = point.screenPos();
#else
                    QPointF screenPos = point.globalPosition();
#endif

#ifdef QT_5
                    QPointF localPos = d->view->mapFromGlobal(screenPos.toPoint());
#else
                    QPointF localPos = d->view->mapFromGlobal(screenPos);
#endif

                    QMouseEvent eventClick(QEvent::MouseButtonDblClick, localPos, screenPos,
                                           Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);

                    QQuickMouseArea::mouseDoubleClickEvent(&eventClick);
                }
                else
                {

                    p->setTouch(-1);

                    p->setEntered(false);

                    p->touchItem = this;

                    p->touchTimer.start(MOUSEAREA_DELAY_TOUCH);
                }
            }

            break;
        }
    }

    QQuickMouseArea::touchEvent(event);
}

/* virtual */ void WDeclarativeMouseArea::touchUngrabEvent()
{
    Q_D(WDeclarativeMouseArea);

    if (d->view == NULL) return;

    WViewPrivate * p = d->view->d_func();

    if (p->touchId != -1)
    {
        p->setTouch(-1);

        p->setEntered(false);
    }

    QQuickMouseArea::touchUngrabEvent();
}

#endif

#ifdef QT_4
/* virtual */ void WDeclarativeMouseArea::wheelEvent(QGraphicsSceneWheelEvent * event)
#else
/* virtual */ void WDeclarativeMouseArea::wheelEvent(QWheelEvent * event)
#endif
{
    Q_D(WDeclarativeMouseArea);

    if (d->wheelEnabled)
    {
#ifdef QT_4
        qreal steps = (qreal) (event->delta()) / 120;
#else
        qreal steps = (qreal) (event->angleDelta().y()) / 120;
#endif

        emit wheeled(steps);
    }
#ifdef QT_4
    else QDeclarativeMouseArea::wheelEvent(event);
#else
    else QQuickMouseArea::wheelEvent(event);
#endif
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

WView * WDeclarativeMouseArea::view() const
{
    Q_D(const WDeclarativeMouseArea); return d->view;
}

//-------------------------------------------------------------------------------------------------

bool WDeclarativeMouseArea::hoverActive() const
{
    Q_D(const WDeclarativeMouseArea); return d->hoverActive;
}

bool WDeclarativeMouseArea::hoverRetain() const
{
    Q_D(const WDeclarativeMouseArea); return d->hoverRetain;
}

void WDeclarativeMouseArea::setHoverRetain(bool retain)
{
    Q_D(WDeclarativeMouseArea);

    if (d->hoverRetain == retain) return;

    d->hoverRetain = retain;

    d->clearHover();

    emit hoverRetainChanged();
}

//-------------------------------------------------------------------------------------------------

bool WDeclarativeMouseArea::wheelEnabled() const
{
    Q_D(const WDeclarativeMouseArea); return d->wheelEnabled;
}

void WDeclarativeMouseArea::setWheelEnabled(bool enabled)
{
    Q_D(WDeclarativeMouseArea);

    if (d->wheelEnabled == enabled) return;

    d->wheelEnabled = enabled;

    emit wheelEnabledChanged();
}

//-------------------------------------------------------------------------------------------------

bool WDeclarativeMouseArea::dropEnabled() const
{
#ifdef QT_4
    return acceptDrops();
#else
    Q_D(const WDeclarativeMouseArea); return d->dropEnabled;
#endif
}

void WDeclarativeMouseArea::setDropEnabled(bool enabled)
{
#ifdef QT_4
    if (acceptDrops() == enabled) return;
#endif

    Q_D(WDeclarativeMouseArea);

#ifdef QT_4
    setAcceptDrops(enabled);
#else
    if (d->dropEnabled == enabled) return;

    d->dropEnabled = enabled;
#endif

    d->clearDrop();

    emit dropEnabledChanged();
}

bool WDeclarativeMouseArea::dragAccepted() const
{
    Q_D(const WDeclarativeMouseArea); return d->dragAccepted;
}

//-------------------------------------------------------------------------------------------------

Qt::CursorShape WDeclarativeMouseArea::cursor() const
{
    Q_D(const WDeclarativeMouseArea); return d->cursor;
}

void WDeclarativeMouseArea::setCursor(Qt::CursorShape shape)
{
    Q_D(WDeclarativeMouseArea);

    if (d->cursor == shape) return;

    d->cursor = shape;

    d->clearHover();

    emit cursorChanged();
}

Qt::CursorShape WDeclarativeMouseArea::cursorDrop() const
{
    Q_D(const WDeclarativeMouseArea); return d->cursorDrop;
}

void WDeclarativeMouseArea::setCursorDrop(Qt::CursorShape shape)
{
    Q_D(WDeclarativeMouseArea);

    if (d->cursorDrop == shape) return;

    d->cursorDrop = shape;

    d->clearDrop();

    emit cursorDropChanged();
}

#endif // SK_NO_DECLARATIVEMOUSEAREA
