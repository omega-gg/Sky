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

#include "WDeclarativeMouseArea.h"

#ifndef SK_NO_DECLARATIVEMOUSEAREA

// System includes
#include <float.h>

// Qt includes
#include <QMimeData>
#include <QGraphicsScene>
#include <QGraphicsSceneWheelEvent>

// Sk includes
#include <WControllerView>
#include <WView>

// Private includes
#include <private/qobject_p.h>

// Private include
#include "WDeclarativeMouseArea_p.h"

//-------------------------------------------------------------------------------------------------
// Static variables

static const int MOUSEAREA_PRESS_HOLD_DELAY = 800;

//=================================================================================================
// WDeclarativeDrag
//=================================================================================================

/* explicit */ WDeclarativeDrag::WDeclarativeDrag(QObject * parent) : QObject(parent)
{
    _target = NULL;

    _axis = XandYAxis;

    _threshold = -1;

    _minimumX = -FLT_MAX;
    _maximumX =  FLT_MAX;

    _minimumY = -FLT_MAX;
    _maximumY =  FLT_MAX;

    _active = false;

    _filterChildren = false;
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
QGraphicsObject * WDeclarativeDrag::target() const
#else
QQuickItem * WDeclarativeDrag::target() const
#endif
{
    return _target;
}

#ifdef QT_4
void WDeclarativeDrag::setTarget(QGraphicsObject * object)
#else
void WDeclarativeDrag::setTarget(QQuickItem * item)
#endif
{
#ifdef QT_4
    if (_target == object) return;

    _target = object;
#else
    if (_target == item) return;

    _target = item;
#endif

    emit targetChanged();
}

void WDeclarativeDrag::resetTarget()
{
    if (_target == NULL) return;

    _target = NULL;

    emit targetChanged();
}

//-------------------------------------------------------------------------------------------------

WDeclarativeDrag::Axis WDeclarativeDrag::axis() const
{
    return _axis;
}

void WDeclarativeDrag::setAxis(WDeclarativeDrag::Axis axis)
{
    if (_axis == axis) return;

    _axis = axis;

    emit axisChanged();
}

//-------------------------------------------------------------------------------------------------

qreal WDeclarativeDrag::threshold() const
{
    return _threshold;
}

void WDeclarativeDrag::setThreshold(qreal threshold)
{
    if (_threshold == threshold) return;

    _threshold = threshold;

    emit thresholdChanged();
}

//-------------------------------------------------------------------------------------------------

qreal WDeclarativeDrag::minimumX() const
{
    return _minimumX;
}

void WDeclarativeDrag::setMinimumX(qreal minimumX)
{
    if (_minimumX == minimumX) return;

    _minimumX = minimumX;

    emit minimumXChanged();
}

qreal WDeclarativeDrag::maximumX() const
{
    return _maximumX;
}

void WDeclarativeDrag::setMaximumX(qreal maximumX)
{
    if (_maximumX == maximumX) return;

    _maximumX = maximumX;

    emit maximumXChanged();
}

//-------------------------------------------------------------------------------------------------

qreal WDeclarativeDrag::minimumY() const
{
    return _minimumY;
}

void WDeclarativeDrag::setMinimumY(qreal minimumY)
{
    if (_minimumY == minimumY) return;

    _minimumY = minimumY;

    emit minimumYChanged();
}

qreal WDeclarativeDrag::maximumY() const
{
    return _maximumY;
}

void WDeclarativeDrag::setMaximumY(qreal maximumY)
{
    if (_maximumY == maximumY) return;

    _maximumY = maximumY;

    emit maximumYChanged();
}

//-------------------------------------------------------------------------------------------------

bool WDeclarativeDrag::active() const
{
    return _active;
}

void WDeclarativeDrag::setActive(bool active)
{
    if (_active == active) return;

    _active = active;

    emit activeChanged();
}

//-------------------------------------------------------------------------------------------------

bool WDeclarativeDrag::filterChildren() const
{
    return _filterChildren;
}

void WDeclarativeDrag::setFilterChildren(bool filter)
{
    if (_filterChildren == filter) return;

    _filterChildren = filter;

    emit filterChildrenChanged();
}

//=================================================================================================
// WDeclarativeMouseEvent
//=================================================================================================

/* virtual */
WDeclarativeMouseEvent::WDeclarativeMouseEvent(int x, int y,
                                               Qt::MouseButton button, Qt::MouseButtons buttons,
                                               Qt::KeyboardModifiers modifiers,
                                               bool isClick, bool wasHeld)
{
    _x = x;
    _y = y;

    _button  = button;
    _buttons = buttons;

    _modifiers = modifiers;

    _wasHeld = wasHeld;

    _isClick = isClick;

    _accepted = true;
}

//=================================================================================================
// WDeclarativeMouseAreaPrivate
//=================================================================================================

WDeclarativeMouseAreaPrivate::WDeclarativeMouseAreaPrivate(WDeclarativeMouseArea * p)
    : WDeclarativeItemPrivate(p) {}

/* virtual */ WDeclarativeMouseAreaPrivate::~WDeclarativeMouseAreaPrivate()
{
    if (drag) delete drag;

    clearView();
}

//-------------------------------------------------------------------------------------------------

void WDeclarativeMouseAreaPrivate::init()
{
    Q_Q(WDeclarativeMouseArea);

    enabled = true;

    wheelEnabled = false;

#ifdef QT_LATEST
    dropEnabled = false;
#endif

    dragAccepted = false;

    preventStealing = false;

    hoverEnabled = false;
    hoverRetain  = false;

    moved = false;

    hovered = false;

    pressed   = false;
    longPress = false;

    doubleClick = false;

    dragX = false;
    dragY = false;

    stealMouse = false;

    drag = NULL;

    startX = 0;
    startY = 0;

    cursor     = Qt::ArrowCursor;
    cursorDrop = Qt::ArrowCursor;

    q->setAcceptedMouseButtons(Qt::LeftButton);

#ifdef QT_LATEST
#ifdef QT_5_LATEST
    q->setAcceptTouchEvents(false);
#endif

    q->setFiltersChildMouseEvents(true);
#endif
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WDeclarativeMouseAreaPrivate::mouseUngrab()
{
    if (pressed == false) return;

    Q_Q(WDeclarativeMouseArea);

    pressed = false;

    stealMouse = false;

    q->release();

    q->setKeepMouseGrab(false);

    emit q->canceled();

    emit q->pressedChanged();
}

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

//-------------------------------------------------------------------------------------------------

void WDeclarativeMouseAreaPrivate::dragMoveEvent(const QPointF       & pos,
                                                 const WViewDragData & data)
{
    Q_Q(WDeclarativeMouseArea);

    lastPos = pos;

    emit q->mousePositionChanged();

    WDeclarativeDropEvent event(pos.x(), pos.y(), data.text, data.actions, data.action);

    emit q->dragMove(&event);
}

//-------------------------------------------------------------------------------------------------

void WDeclarativeMouseAreaPrivate::dropEvent(const QPointF       & pos,
                                             const WViewDragData & data)
{
    Q_Q(WDeclarativeMouseArea);

    WDeclarativeDropEvent event(pos.x(), pos.y(), data.text, data.actions, data.action);

    emit q->drop(&event);

    setDragAccepted(false);
}

//-------------------------------------------------------------------------------------------------

#ifdef QT_4
void WDeclarativeMouseAreaPrivate::saveEvent(QGraphicsSceneMouseEvent * event)
#else
void WDeclarativeMouseAreaPrivate::saveEvent(QMouseEvent * event)
#endif
{
    lastPos = event->pos();

#ifdef QT_4
    lastScenePos = event->scenePos();
#else
    lastScenePos = event->screenPos();
#endif

    lastButton  = event->button ();
    lastButtons = event->buttons();

    lastModifiers = event->modifiers();
}

//-------------------------------------------------------------------------------------------------

bool WDeclarativeMouseAreaPrivate::isDoubleClickConnected()
{
    Q_Q(WDeclarativeMouseArea);

    static int idx = QObjectPrivate::get(q)->signalIndex("doubleClicked(WDeclarativeMouseEvent*)");

    return QObjectPrivate::get(q)->isSignalConnected(idx);
}

//-------------------------------------------------------------------------------------------------

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

//-------------------------------------------------------------------------------------------------

void WDeclarativeMouseAreaPrivate::clearView()
{
    if (view == NULL) return;

    Q_Q(WDeclarativeMouseArea);

    if (hovered)
    {
        hovered = false;

        view->d_func()->itemsHovered.removeOne(q);

        emit q->hoveredChanged();

        emit q->exited();
    }

    if (view->d_func()->areaDrop == q)
    {
        WDeclarativeDropEvent event(-1, -1, QString());

        emit q->dragExited(&event);

        view->d_func()->areaDrop = NULL;
    }
}

//=================================================================================================
// WDeclarativeMouseArea
//=================================================================================================

#ifdef QT_4
/* explicit */ WDeclarativeMouseArea::WDeclarativeMouseArea(QDeclarativeItem * parent)
#else
/* explicit */ WDeclarativeMouseArea::WDeclarativeMouseArea(QQuickItem * parent)
#endif
    : WDeclarativeItem(new WDeclarativeMouseAreaPrivate(this), parent)
{
    Q_D(WDeclarativeMouseArea); d->init();
}

//-------------------------------------------------------------------------------------------------
// Protected

#ifdef QT_4
WDeclarativeMouseArea::WDeclarativeMouseArea(WDeclarativeMouseAreaPrivate * p,
                                             QDeclarativeItem             * parent)
#else
WDeclarativeMouseArea::WDeclarativeMouseArea(WDeclarativeMouseAreaPrivate * p, QQuickItem * parent)
#endif
    : WDeclarativeItem(p, parent)
{
    Q_D(WDeclarativeMouseArea); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WDeclarativeMouseArea::press(Qt::MouseButton button)
{
    Q_D(WDeclarativeMouseArea);

    WDeclarativeMouseEvent event(d->lastPos.x(), d->lastPos.y(), button, d->lastButtons,
                                 d->lastModifiers, false, d->longPress);

    emit pressed(&event);
}

/* Q_INVOKABLE */ void WDeclarativeMouseArea::release(Qt::MouseButton button)
{
    Q_D(WDeclarativeMouseArea);

    WDeclarativeMouseEvent event(d->lastPos.x(), d->lastPos.y(), button, d->lastButtons,
                                 d->lastModifiers, true, d->longPress);

    emit released(&event);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WDeclarativeMouseArea::click(Qt::MouseButton button)
{
    Q_D(WDeclarativeMouseArea);

    WDeclarativeMouseEvent event(d->lastPos.x(), d->lastPos.y(), button, d->lastButtons,
                                 d->lastModifiers, true, d->longPress);

    emit clicked(&event);
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

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WDeclarativeMouseArea::click(int button)
{
    click(static_cast<Qt::MouseButton> (button));
}

//-------------------------------------------------------------------------------------------------
// Protected functions
//-------------------------------------------------------------------------------------------------

void WDeclarativeMouseArea::setHovered(bool hovered)
{
    Q_D(WDeclarativeMouseArea);

    if (d->hovered == hovered) return;

    d->hovered = hovered;

    emit hoveredChanged();

    if (hovered)
    {
#ifdef QT_4
        setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);
#endif

        if (d->view)
        {
            d->lastPos = mapFromScene(d->view->d_func()->mousePos);
        }

        emit entered();
    }
    else
    {
#ifdef QT_4
        setFlag(QGraphicsItem::ItemSendsScenePositionChanges, false);
#endif

        emit exited();
    }
}

bool WDeclarativeMouseArea::setPressed(bool pressed)
{
    Q_D(WDeclarativeMouseArea);

    if (d->pressed == pressed) return false;

    bool dragged = d->drag && d->drag->active();

    bool isClick;

    if (d->pressed && pressed == false && dragged == false
        &&
#ifdef QT_4 // FIXME
        isUnderMouse())
#else
        d->view->d_func()->itemUnderMouse(this))
#endif
    {
         isClick = true;
    }
    else isClick = false;

    d->pressed = pressed;

    WDeclarativeMouseEvent event(d->lastPos.x(), d->lastPos.y(), d->lastButton, d->lastButtons,
                                 d->lastModifiers, isClick, d->longPress);

    if (d->pressed)
    {
        /*if (d->doubleClick == false)*/ emit this->pressed(&event);

        emit mousePositionChanged();

        emit pressedChanged();
    }
    else
    {
        emit released(&event);

        event.setX(d->lastPos.x());
        event.setY(d->lastPos.y());

        emit pressedChanged();

        if (isClick && d->longPress == false /*&& d->doubleClick == false*/)
        {
            emit clicked(&event);
        }
    }

    return event.isAccepted();
}

//-------------------------------------------------------------------------------------------------

#ifdef QT_4
bool WDeclarativeMouseArea::sendMouseEvent(QGraphicsSceneMouseEvent * event)
#else
bool WDeclarativeMouseArea::sendMouseEvent(QMouseEvent * event)
#endif
{
    Q_D(WDeclarativeMouseArea);

#ifdef QT_4
    QGraphicsScene * scene = this->scene();

    QDeclarativeItem * grabber;

    if (scene)
    {
         grabber = qobject_cast<QDeclarativeItem *> (scene->mouseGrabberItem());
    }
    else grabber = NULL;

    QRectF myRect = mapToScene(QRectF(0, 0, width(), height())).boundingRect();
#else
    QQuickItem * grabber = d->view->mouseGrabberItem();

    QPointF localPos = mapFromScene(event->windowPos());
#endif

    bool stealMouse = d->stealMouse;

#ifdef QT_4
    if ((stealMouse || myRect.contains(event->scenePos().toPoint()))
#else
    if ((stealMouse || boundingRect().contains(localPos))
#endif
        &&
        (grabber == NULL || grabber->keepMouseGrab() == false))
    {
#ifdef QT_4
        QGraphicsSceneMouseEvent mouse(event->type());

        for (unsigned int i = Qt::LeftButton; i <= Qt::XButton2; i <<= 1)
        {
            if (event->buttons() & i)
            {
                Qt::MouseButton button = Qt::MouseButton(i);

                mouse.setButtonDownPos(button, mapFromScene(event->buttonDownPos(button)));
            }
        }

        mouse.setScenePos    (event->scenePos    ());
        mouse.setLastScenePos(event->lastScenePos());

        mouse.setPos    (mapFromScene(event->scenePos    ()));
        mouse.setLastPos(mapFromScene(event->lastScenePos()));
#else
        QMouseEvent mouse(event->type(), localPos, event->windowPos(), event->screenPos(),
                          event->button(), event->buttons(), event->modifiers());
#endif

        mouse.setAccepted(false);

        QEvent::Type type = mouse.type();

#ifdef QT_4
        if      (type == QEvent::GraphicsSceneMousePress)   mousePressEvent  (&mouse);
        else if (type == QEvent::GraphicsSceneMouseRelease) mouseReleaseEvent(&mouse);
        else if (type == QEvent::GraphicsSceneMouseMove)    mouseMoveEvent   (&mouse);

        grabber = qobject_cast<QDeclarativeItem *> (scene->mouseGrabberItem());
#else
        if      (type == QEvent::MouseButtonPress)   mousePressEvent  (&mouse);
        else if (type == QEvent::MouseButtonRelease) mouseReleaseEvent(&mouse);
        else if (type == QEvent::MouseMove)          mouseMoveEvent   (&mouse);

        grabber = d->view->mouseGrabberItem();
#endif

        if (grabber && stealMouse && grabber->keepMouseGrab() == false && grabber != this)
        {
            grabMouse();
        }

        return stealMouse;
    }

#ifdef QT_4
    if (event->type() == QEvent::GraphicsSceneMouseRelease && d->pressed)
#else
    if (event->type() == QEvent::MouseButtonRelease && d->pressed)
#endif
    {
        d->pressed = false;

        d->stealMouse = false;

        release();

#ifdef QT_4
        if (scene && scene->mouseGrabberItem() == this)
#else
        if (d->view && d->view->mouseGrabberItem() == this)
#endif
        {
            ungrabMouse();
        }

        emit canceled();

        emit pressedChanged();
    }

    return false;
}

//-------------------------------------------------------------------------------------------------
// Protected QGraphicsItem / QQuickItem reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativeMouseArea::geometryChanged(const QRectF & newGeometry,
                                                          const QRectF & oldGeometry)
{
    Q_D(WDeclarativeMouseArea);

    WDeclarativeItem::geometryChanged(newGeometry, oldGeometry);

    if (d->lastScenePos.isNull())
    {
        d->lastScenePos = mapToScene(d->lastPos);
    }
    else if (newGeometry.x() != oldGeometry.x() || newGeometry.y() != oldGeometry.y())
    {
        d->lastPos = mapFromScene(d->lastScenePos);
    }
}

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

        if (scene == NULL) d->clearView();
#else
        if (value.window == NULL) d->clearView();
#endif
    }

#ifdef QT_4
    return WDeclarativeItem::itemChange(change, value);
#else
    WDeclarativeItem::itemChange(change, value);
#endif
}

//-------------------------------------------------------------------------------------------------
// Events
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
/* virtual */ void WDeclarativeMouseArea::mousePressEvent(QGraphicsSceneMouseEvent * event)
#else
/* virtual */ void WDeclarativeMouseArea::mousePressEvent(QMouseEvent * event)
#endif
{
    Q_D(WDeclarativeMouseArea);

    d->moved = false;

    d->stealMouse = d->preventStealing;

    if (d->enabled == false)
    {
        //WDeclarativeItem::mousePressEvent(event);

        return;
    }

    d->longPress = false;

    d->saveEvent(event);

    if (d->drag)
    {
        WDeclarativeDrag::Axis axis = d->drag->axis();

        d->dragX = axis & WDeclarativeDrag::XAxis;
        d->dragY = axis & WDeclarativeDrag::YAxis;
    }

    if (d->drag) d->drag->setActive(false);

#ifdef QT_4
    d->startScene = event->scenePos();
#else
    d->startScene = event->screenPos();
#endif

    setKeepMouseGrab(d->stealMouse);

    bool accept = setPressed(true);

    event->setAccepted(accept);

    if (accept)
    {
        d->pressAndHoldTimer.start(MOUSEAREA_PRESS_HOLD_DELAY, this);
    }
}

#ifdef QT_4
/* virtual */ void WDeclarativeMouseArea::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
#else
/* virtual */ void WDeclarativeMouseArea::mouseReleaseEvent(QMouseEvent * event)
#endif
{
    Q_D(WDeclarativeMouseArea);

    d->stealMouse = false;

    if (d->enabled == false)
    {
        //WDeclarativeItem::mouseReleaseEvent(event);

        return;
    }

    d->saveEvent(event);

    setPressed(false);

    if (d->drag) d->drag->setActive(false);

#ifdef QT_4
    QGraphicsScene * scene = this->scene();

    if (scene && scene->mouseGrabberItem() == this)
#else
    if (d->view && d->view->mouseGrabberItem() == this)
#endif
    {
        ungrabMouse();
    }

    setKeepMouseGrab(false);
}

//-------------------------------------------------------------------------------------------------

#ifdef QT_4
/* virtual */ void WDeclarativeMouseArea::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event)
#else
/* virtual */ void WDeclarativeMouseArea::mouseDoubleClickEvent(QMouseEvent * event)
#endif
{
    Q_D(WDeclarativeMouseArea);

    if (d->enabled == false)
    {
        //WDeclarativeItem::mouseDoubleClickEvent(event);

        return;
    }

    d->saveEvent(event);

    WDeclarativeMouseEvent mouse(d->lastPos.x(), d->lastPos.y(), d->lastButton, d->lastButtons,
                                 d->lastModifiers, true, false);

    mouse.setAccepted(d->isDoubleClickConnected());

    emit doubleClicked(&mouse);

    WDeclarativeItem::mouseDoubleClickEvent(event);
}

//-------------------------------------------------------------------------------------------------

#ifdef QT_4
/* virtual */ void WDeclarativeMouseArea::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
#else
/* virtual */ void WDeclarativeMouseArea::mouseMoveEvent(QMouseEvent * event)
#endif
{
    Q_D(WDeclarativeMouseArea);

    if (d->enabled == false)
    {
        WDeclarativeItem::mouseMoveEvent(event);

        return;
    }

    d->saveEvent(event);

    if (d->drag && d->drag->target())
    {
        if (d->moved == false)
        {
            d->startX = drag()->target()->x();
            d->startY = drag()->target()->y();
        }

        QPointF posStart;
        QPointF posCurrent;

        if (drag()->target()->parentItem())
        {
            posStart = drag()->target()->parentItem()->mapFromScene(d->startScene);

#ifdef QT_4
            posCurrent = drag()->target()->parentItem()->mapFromScene(event->scenePos());
#else
            posCurrent = drag()->target()->parentItem()->mapFromScene(event->screenPos());
#endif
        }
        else
        {
            posStart = d->startScene;

#ifdef QT_4
            posCurrent = event->scenePos();
#else
            posCurrent = event->screenPos();
#endif
        }

        if (d->drag->active() == false)
        {
            if (d->view->testDrag(posStart, posCurrent, d->drag->threshold()))
            {
                d->drag->setActive(true);

                setKeepMouseGrab(true);

                d->stealMouse = true;
            }
            else
            {
                d->moved = true;

                return;
            }
        }

        if (d->dragX)
        {
            qreal x = (posCurrent.x() - posStart.x()) + d->startX;

            if      (x < drag()->minimumX()) x = drag()->minimumX();
            else if (x > drag()->maximumX()) x = drag()->maximumX();

            drag()->target()->setX(x);
        }

        if (d->dragY)
        {
            qreal y = (posCurrent.y() - posStart.y()) + d->startY;

            if      (y < drag()->minimumY()) y = drag()->minimumY();
            else if (y > drag()->maximumY()) y = drag()->maximumY();

            drag()->target()->setY(y);
        }

        d->moved = true;
    }

    emit mousePositionChanged();

    WDeclarativeMouseEvent mouse(d->lastPos.x(), d->lastPos.y(), d->lastButton, d->lastButtons,
                                 d->lastModifiers, false, d->longPress);

    emit positionChanged(&mouse);
}

//-------------------------------------------------------------------------------------------------

#ifdef QT_LATEST

/* virtual */ void WDeclarativeMouseArea::mouseUngrabEvent()
{
    Q_D(WDeclarativeMouseArea); d->mouseUngrab();
}

#endif

//-------------------------------------------------------------------------------------------------

#ifdef QT_4
/* virtual */ void WDeclarativeMouseArea::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
#else
/* virtual */ void WDeclarativeMouseArea::hoverEnterEvent(QHoverEvent * event)
#endif
{
    Q_D(WDeclarativeMouseArea);

    if (d->enabled)
    {
        d->lastPos = event->pos();

        emit mousePositionChanged();
    }
    else WDeclarativeItem::hoverEnterEvent(event);
}

#ifdef QT_4
/* virtual */ void WDeclarativeMouseArea::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
#else
/* virtual */ void WDeclarativeMouseArea::hoverLeaveEvent(QHoverEvent * event)
#endif
{
    Q_D(WDeclarativeMouseArea);

    if (d->enabled == false)
    {
        WDeclarativeItem::hoverLeaveEvent(event);
    }
}

#ifdef QT_4
/* virtual */ void WDeclarativeMouseArea::hoverMoveEvent(QGraphicsSceneHoverEvent * event)
#else
/* virtual */ void WDeclarativeMouseArea::hoverMoveEvent(QHoverEvent * event)
#endif
{
    Q_D(WDeclarativeMouseArea);

    if (d->enabled == false)
    {
        WDeclarativeItem::hoverMoveEvent(event);

        return;
    }

    d->lastPos = event->pos();

    emit mousePositionChanged();

    WDeclarativeMouseEvent mouse(d->lastPos.x(), d->lastPos.y(), Qt::NoButton, Qt::NoButton,
                                 event->modifiers(), false, false);

    emit positionChanged(&mouse);
}

//-------------------------------------------------------------------------------------------------

#ifdef QT_4
/* virtual */ void WDeclarativeMouseArea::wheelEvent(QGraphicsSceneWheelEvent * event)
#else
/* virtual */ void WDeclarativeMouseArea::wheelEvent(QWheelEvent * event)
#endif
{
    Q_D(WDeclarativeMouseArea);

    if (d->wheelEnabled)
    {
        int steps = event->delta() / 120;

        emit wheeled(steps);
    }
    else WDeclarativeItem::wheelEvent(event);
}

//-------------------------------------------------------------------------------------------------

#if defined(QT_4) && defined(QT_NO_CONTEXTMENU) == false

/* virtual */ void WDeclarativeMouseArea::contextMenuEvent(QGraphicsSceneContextMenuEvent * event)
{
    bool acceptsContextMenuButton;

#if defined(Q_OS_SYMBIAN)
    acceptsContextMenuButton = acceptedButtons() & Qt::LeftButton;
#elif defined(Q_OS_WINCE)
    acceptsContextMenuButton = acceptedButtons() & (Qt::LeftButton | Qt::RightButton);
#else
    acceptsContextMenuButton = acceptedButtons() & Qt::RightButton;
#endif

    if (isEnabled() && event->reason() == QGraphicsSceneContextMenuEvent::Mouse
        &&
        acceptsContextMenuButton)
    {
        return;
    }

    WDeclarativeItem::contextMenuEvent(event);
}

#endif

//-------------------------------------------------------------------------------------------------

#ifdef QT_4

/* virtual */ bool WDeclarativeMouseArea::sceneEvent(QEvent * event)
{
    bool result = WDeclarativeItem::sceneEvent(event);

    if (event->type() == QEvent::UngrabMouse)
    {
        Q_D(WDeclarativeMouseArea);

        d->mouseUngrab();
    }

    return result;
}

#endif

#ifdef QT_4
/* virtual */ bool WDeclarativeMouseArea::sceneEventFilter(QGraphicsItem * item, QEvent * event)
#else
/* virtual */ bool WDeclarativeMouseArea::childMouseEventFilter(QQuickItem * item, QEvent * event)
#endif
{
    Q_D(WDeclarativeMouseArea);

    if (d->enabled == false || isVisible() == false
        ||
        d->drag == NULL || d->drag->filterChildren() == false)
    {
#ifdef QT_4
        return WDeclarativeItem::sceneEventFilter(item, event);
#else
        return WDeclarativeItem::childMouseEventFilter(item, event);
#endif
    }

    QEvent::Type type = event->type();

#ifdef QT_4
    if (type == QEvent::GraphicsSceneMousePress
        ||
        type == QEvent::GraphicsSceneMouseRelease
        ||
        type == QEvent::GraphicsSceneMouseMove)
    {
         return sendMouseEvent(static_cast<QGraphicsSceneMouseEvent *> (event));
    }
    else return WDeclarativeItem::sceneEventFilter(item, event);
#else
    if (type == QEvent::MouseButtonPress
        ||
        type == QEvent::MouseButtonRelease
        ||
        type == QEvent::MouseMove)
    {
         return sendMouseEvent(static_cast<QMouseEvent *> (event));
    }
    else return WDeclarativeItem::childMouseEventFilter(item, event);
#endif
}

//-------------------------------------------------------------------------------------------------

#ifdef QT_LATEST

/* virtual */ void WDeclarativeMouseArea::windowDeactivateEvent()
{
    Q_D(WDeclarativeMouseArea);

    WDeclarativeItem::windowDeactivateEvent();

    d->mouseUngrab();
}

#endif

//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativeMouseArea::timerEvent(QTimerEvent * event)
{
    Q_D(WDeclarativeMouseArea);

    if (d->pressAndHoldTimer.timerId() == event->timerId())
    {
        d->pressAndHoldTimer.stop();

        bool dragged = d->drag && d->drag->active();

        if (d->pressed && dragged == false && d->hovered)
        {
            d->longPress = true;

            WDeclarativeMouseEvent mouse(d->lastPos.x(), d->lastPos.y(), d->lastButton,
                                         d->lastButtons, d->lastModifiers, false, d->longPress);

            emit pressAndHold(&mouse);
        }
    }
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

qreal WDeclarativeMouseArea::mouseX() const
{
    Q_D(const WDeclarativeMouseArea); return d->lastPos.x();
}

qreal WDeclarativeMouseArea::mouseY() const
{
    Q_D(const WDeclarativeMouseArea); return d->lastPos.y();
}

//-------------------------------------------------------------------------------------------------

bool WDeclarativeMouseArea::isEnabled() const
{
    Q_D(const WDeclarativeMouseArea); return d->enabled;
}

void WDeclarativeMouseArea::setEnabled(bool enabled)
{
    Q_D(WDeclarativeMouseArea);

    if (d->enabled == enabled) return;

    d->enabled = enabled;

    emit enabledChanged();
}

//-------------------------------------------------------------------------------------------------

bool WDeclarativeMouseArea::hovered() const
{
    Q_D(const WDeclarativeMouseArea); return d->hovered;
}

bool WDeclarativeMouseArea::pressed() const
{
    Q_D(const WDeclarativeMouseArea); return d->pressed;
}

//-------------------------------------------------------------------------------------------------

Qt::MouseButtons WDeclarativeMouseArea::pressedButtons() const
{
    Q_D(const WDeclarativeMouseArea); return d->lastButtons;
}

//-------------------------------------------------------------------------------------------------

Qt::MouseButtons WDeclarativeMouseArea::acceptedButtons() const
{
    return acceptedMouseButtons();
}

void WDeclarativeMouseArea::setAcceptedButtons(Qt::MouseButtons buttons)
{
    if (buttons == acceptedMouseButtons()) return;

    setAcceptedMouseButtons(buttons);

    emit acceptedButtonsChanged();
}

//-------------------------------------------------------------------------------------------------

bool WDeclarativeMouseArea::hoverEnabled() const
{
    Q_D(const WDeclarativeMouseArea); return d->hoverEnabled;
}

void WDeclarativeMouseArea::setHoverEnabled(bool enabled)
{
    Q_D(WDeclarativeMouseArea);

    if (d->hoverEnabled == enabled) return;

    d->hoverEnabled = enabled;

    setAcceptHoverEvents(enabled);

    d->clearHover();

    emit hoverEnabledChanged();
}

//-------------------------------------------------------------------------------------------------

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

//-------------------------------------------------------------------------------------------------

bool WDeclarativeMouseArea::dragAccepted() const
{
    Q_D(const WDeclarativeMouseArea); return d->dragAccepted;
}

//-------------------------------------------------------------------------------------------------

WDeclarativeDrag * WDeclarativeMouseArea::drag()
{
    Q_D(WDeclarativeMouseArea);

    if (d->drag == NULL) d->drag = new WDeclarativeDrag;

    return d->drag;
}

//-------------------------------------------------------------------------------------------------

bool WDeclarativeMouseArea::preventStealing() const
{
    Q_D(const WDeclarativeMouseArea); return d->preventStealing;
}

void WDeclarativeMouseArea::setPreventStealing(bool prevent)
{
    Q_D(WDeclarativeMouseArea);

    if (d->preventStealing == prevent) return;

    d->preventStealing = prevent;

    if (d->preventStealing && d->enabled)
    {
         setKeepMouseGrab(true);
    }
    else setKeepMouseGrab(false);

    emit preventStealingChanged();
}

//-------------------------------------------------------------------------------------------------

Qt::CursorShape WDeclarativeMouseArea::cursor()
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

Qt::CursorShape WDeclarativeMouseArea::cursorDrop()
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
