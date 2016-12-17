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

#include "WView.h"

#ifndef SK_NO_VIEW

// Qt includes
#include <QApplication>
#include <QDesktopWidget>
#include <QGLWidget>
#include <QImageReader>
#include <QDrag>
#include <QMimeData>
#include <QDir>

// Sk includes
#include <WControllerApplication>
#include <WControllerFile>
#include <WControllerView>
#include <WResizer>

// Private includes
#include <private/WControllerView_p>
#include <private/WDeclarativeMouseArea_p>

//=================================================================================================
// WDeclarativeKeyEvent
//=================================================================================================

WDeclarativeKeyEvent::WDeclarativeKeyEvent(QEvent::Type            type,
                                           int                     key,
                                           Qt::KeyboardModifiers   modifiers,
                                           const QString         & text,
                                           bool                    autorep,
                                           ushort                  count)
    : _event(type, key, modifiers, text, autorep, count)
{
    _event.setAccepted(false);
}

WDeclarativeKeyEvent::WDeclarativeKeyEvent(const QKeyEvent & event)
    : _event(event)
{
    _event.setAccepted(false);
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

bool WDeclarativeKeyEvent::isAccepted() const
{
    return _event.isAccepted();
}

void WDeclarativeKeyEvent::setAccepted(bool accepted)
{
    _event.setAccepted(accepted);
}

//-------------------------------------------------------------------------------------------------

int WDeclarativeKeyEvent::key() const
{
    return _event.key();
}

//-------------------------------------------------------------------------------------------------

QString WDeclarativeKeyEvent::text() const
{
    return _event.text();
}

//-------------------------------------------------------------------------------------------------

int WDeclarativeKeyEvent::modifiers() const
{
    return _event.modifiers();
}

//-------------------------------------------------------------------------------------------------

bool WDeclarativeKeyEvent::isAutoRepeat() const
{
    return _event.isAutoRepeat();
}

//-------------------------------------------------------------------------------------------------

int WDeclarativeKeyEvent::count() const
{
    return _event.count();
}

//=================================================================================================
// WDeclarativeDropEvent
//=================================================================================================

WDeclarativeDropEvent::WDeclarativeDropEvent(qreal x, qreal y, const QString & text,
                                             Qt::DropActions actions,
                                             Qt::DropAction  action)
{
    _accepted = false;

    _actions = actions;
    _action  = action;

    _x = x;
    _y = y;

    _text = text;
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

bool WDeclarativeDropEvent::isAccepted() const
{
    return _accepted;
}

void WDeclarativeDropEvent::setAccepted(bool accepted)
{
    _accepted = accepted;
}

//-------------------------------------------------------------------------------------------------

Qt::DropActions WDeclarativeDropEvent::actions() const
{
    return _actions;
}

Qt::DropAction WDeclarativeDropEvent::action() const
{
    return _action;
}

void WDeclarativeDropEvent::setAction(Qt::DropAction action)
{
    _action = action;
}

//-------------------------------------------------------------------------------------------------

qreal WDeclarativeDropEvent::x() const
{
    return _x;
}

qreal WDeclarativeDropEvent::y() const
{
    return _y;
}

//-------------------------------------------------------------------------------------------------

QString WDeclarativeDropEvent::text() const
{
    return _text;
}

void WDeclarativeDropEvent::setText(const QString & text)
{
    _text = text;
}

//=================================================================================================
// WViewScene
//=================================================================================================

WViewScene::WViewScene(WView * parent) : QGraphicsScene(parent)
{
    view = parent;
}

//=================================================================================================
// WViewPrivate
//=================================================================================================

WViewPrivate::WViewPrivate(WView * p) : WAbstractViewPrivate(p) {}

/* virtual */ WViewPrivate::~WViewPrivate()
{
    Q_Q(WView);

    if (mime) delete mime;

    W_GET_CONTROLLER(WControllerView, controller);

    if (controller) controller->d_func()->unregisterView(q);
}

//-------------------------------------------------------------------------------------------------

void WViewPrivate::init(QDeclarativeItem * item)
{
    Q_Q(WView);

    this->item = item;

    currentResizer = NULL;

    zoom = 1.0;

    minimumWidth  = 400;
    minimumHeight = 300;

    maximumWidth  = -1;
    maximumHeight = -1;

    active = false;

    minimized = false;

    Sk::Mode mode = sk->defaultMode();

    if (mode == Sk::Normal)
    {
        maximized  = false;
        fullScreen = false;
    }
    else if (mode == Sk::Maximized)
    {
        maximized  = true;
        fullScreen = false;
    }
    else if (mode == Sk::FullScreen)
    {
        maximized  = false;
        fullScreen = true;
    }
    else // if (mode == Sk::FullScreenMaximized)
    {
        maximized  = true;
        fullScreen = true;
    }

    locked = false;
    closed = false;

    opengl    = true;
    antialias = true;
    vsync     = false;

    //---------------------------------------------------------------------------------------------
    // Fade

    fade = true;

    fadeVisible = false;

    fadeDuration = 150;

    fadeValue = 16.0 / fadeDuration;

    //---------------------------------------------------------------------------------------------
    // Mouse

    entered  = false;
    pressed  = false;
    dragging = false;

    hover     = true;
    hoverable = false;

    dragged  = false;
    resizing = false;

    mouseAccepted = false;

    button  = Qt::NoButton;
    buttons = Qt::NoButton;

    cursor = WDeclarativeMouseArea::ArrowCursor;

    idleCheck = false;
    idle      = false;
    idleDelay = 3000;

    idleTimer.setInterval(3000);

    //---------------------------------------------------------------------------------------------
    // Drag

    drag = NULL;
    mime = NULL;

    dragData.actions = Qt::IgnoreAction;
    dragData.action  = Qt::IgnoreAction;

    areaDrop = NULL;

    //---------------------------------------------------------------------------------------------
    // Keys

#ifdef Q_OS_WIN
    keyVirtual = 0;
    keyScan    = 0;
#endif

    keyShiftPressed   = false;
    keyControlPressed = false;
    keyAltPressed     = false;

    keyAccepted = false;

    //---------------------------------------------------------------------------------------------
    // Scene

    scene = new WViewScene(q);

    if (item)
    {
        scene->addItem(item);

        QObject::connect(item, SIGNAL(widthChanged ()), q, SIGNAL(itemWidthChanged ()));
        QObject::connect(item, SIGNAL(heightChanged()), q, SIGNAL(itemHeightChanged()));

        QObject::connect(item, SIGNAL(xChanged()), q, SIGNAL(originXChanged()));
        QObject::connect(item, SIGNAL(yChanged()), q, SIGNAL(originYChanged()));
    }

    q->setRenderHint(QPainter::Antialiasing, true);

    QGLFormat format = QGLFormat::defaultFormat();

    format.setSampleBuffers(true);

    q->setViewport(new QGLWidget(format));

    q->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    q->setScene(scene);

    // FIXME Windows: Workaround for opengl full screen flicker.
    q->setViewportMargins(0, 0, -1, 0);

    //---------------------------------------------------------------------------------------------
    // For performance

    q->setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing);

    //---------------------------------------------------------------------------------------------
    // Background

    q->setAttribute(Qt::WA_OpaquePaintEvent);
    q->setAttribute(Qt::WA_NoSystemBackground);

    QWidget * viewport = q->viewport();

    viewport->setAttribute(Qt::WA_OpaquePaintEvent);
    viewport->setAttribute(Qt::WA_NoSystemBackground);

    //---------------------------------------------------------------------------------------------
    // Default size

    q->WAbstractView::setMinimumSize(minimumWidth, minimumHeight);

    QRect rect = wControllerView->availableGeometry(sk->defaultScreen());

    geometryNormal = getGeometryDefault(rect);

    q->setGeometry(geometryNormal);

    //---------------------------------------------------------------------------------------------
    // Timers

    q->startTimer(16);

    //---------------------------------------------------------------------------------------------
    // Registering view

    wControllerView->d_func()->registerView(q);

    //---------------------------------------------------------------------------------------------
    // Signals

    QObject::connect(qApp, SIGNAL(messageReceived(const QString &)),
                     q,    SIGNAL(messageReceived(const QString &)));

    QObject::connect(qApp->desktop(), SIGNAL(workAreaResized(int)), q, SLOT(onGeometryChanged()));

    QObject::connect(q, SIGNAL(availableGeometryChanged()), q, SIGNAL(centerXChanged()));
    QObject::connect(q, SIGNAL(availableGeometryChanged()), q, SIGNAL(centerYChanged()));

    QObject::connect(q, SIGNAL(widthChanged ()), q, SIGNAL(centerXChanged()));
    QObject::connect(q, SIGNAL(heightChanged()), q, SIGNAL(centerYChanged()));

    QObject::connect(&fadeTimer, SIGNAL(timeout()), q, SLOT(onFadeTimeout()));
    QObject::connect(&idleTimer, SIGNAL(timeout()), q, SLOT(onIdleTimeout()));

    QObject::connect(sk, SIGNAL(cursorVisibleChanged()), q, SLOT(onCursorVisibleChanged()));
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WViewPrivate::startFade(bool visible)
{
    Q_Q(WView);

    fadeVisible = visible;

    if (visible)
    {
         q->setWindowOpacity(fadeValue);
    }
    else q->setWindowOpacity(1.0 - fadeValue);

    fadeTimer.start(16);
}

//-------------------------------------------------------------------------------------------------

void WViewPrivate::fadeIn()
{
    Q_Q(WView);

    fadeTimer.stop();

    q->setWindowOpacity(1.0);

    emit q->fadeIn();
}

void WViewPrivate::fadeOut()
{
    Q_Q(WView);

    fadeTimer.stop();

    q->setWindowOpacity(0.0);

    emit q->fadeOut();

    q->WAbstractView::close();
}

//-------------------------------------------------------------------------------------------------

void WViewPrivate::applySize(int width, int height)
{
    item->setSize(QSizeF(width * zoom, height * zoom));
}

//-------------------------------------------------------------------------------------------------

void WViewPrivate::updateFlags()
{
    Q_Q(WView);

#ifdef Q_OS_WIN
    if (locked)
    {
         SetWindowPos((HWND) q->winId(), HWND_TOPMOST,   0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    }
    else SetWindowPos((HWND) q->winId(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
#else
    if (locked)
    {
         q->setWindowFlags(flags | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
    }
    else q->setWindowFlags(flags);
#endif
}

//-------------------------------------------------------------------------------------------------

void WViewPrivate::updateViewport()
{
    Q_Q(WView);

    QGLFormat format = QGLFormat::defaultFormat();

    format.setSampleBuffers(true);

    if (vsync)
    {
        format.setSwapInterval(1);
    }

    q->setViewport(new QGLWidget(format));
}

//-------------------------------------------------------------------------------------------------

void WViewPrivate::updateHoverable()
{
    if (hover && entered && pressed == false && dragging == false && dragged == false
        &&
        resizing == false && sk->cursorVisible())
    {
         hoverable = true;
    }
    else hoverable = false;
}

//-------------------------------------------------------------------------------------------------

void WViewPrivate::updateMouse()
{
    if (dragged || resizing) return;

    Q_Q(WView);

    QPoint pos = QCursor::pos();

#ifdef Q_OS_LINUX
    setMousePos(q->mapFromGlobal(pos));
#else
    setMousePos(QPoint(pos.x() - q->x(), pos.y() - q->y()));
#endif
}

//-------------------------------------------------------------------------------------------------

void WViewPrivate::updateDrag()
{
    if (dragging == false || drag) return;

    Q_Q(WView);

    QPoint pos = QCursor::pos();

#ifdef Q_OS_LINUX
    setMousePos(q->mapFromGlobal(pos));
#else
    setMousePos(QPoint(pos.x() - q->x(), pos.y() - q->y()));
#endif

    if (isUnderMouse())
    {
        QList<QGraphicsItem *> items = q->items(mousePos);

        if (itemsDrop == items)
        {
            if (areaDrop)
            {
                QPointF posItem = areaDrop->mapFromScene(mousePos);

                areaDrop->d_func()->dragMoveEvent(posItem, dragData);
            }

            return;
        }

        itemsDrop = items;

        QList<WDeclarativeMouseArea *> areas = getDropAreas(itemsDrop);

        foreach (WDeclarativeMouseArea * area, areas)
        {
            WDeclarativeMouseAreaPrivate * areaPrivate = area->d_func();

            QPointF posItem = area->mapFromScene(mousePos);

            if (areaPrivate->dragAccepted == false)
            {
                if (areaDrop)
                {
                    areaDrop->d_func()->dragLeaveEvent();
                }

                if (areaPrivate->dragEnterEvent(posItem, &dragData))
                {
                    areaDrop = area;

                    areaPrivate->dragMoveEvent(posItem, dragData);

                    setCursor(areaPrivate->cursorDrop);

                    return;
                }
                else areaDrop = NULL;
            }
            else
            {
                areaPrivate->dragMoveEvent(posItem, dragData);

                return;
            }
        }

        if (areaDrop)
        {
            areaDrop->d_func()->dragLeaveEvent();

            areaDrop = NULL;
        }
    }
    else
    {
        clearDrag();

        drag = new QDrag(q);

        if (mime)
        {
            drag->setMimeData(mime);

            mime = NULL;
        }
        else
        {
            QMimeData * mime = new QMimeData;

            mime->setText(dragData.text);

            drag->setMimeData(mime);
        }

        drag->exec(dragData.actions);

        if (drag)
        {
            drag = NULL;

            setDragging(false);
        }
    }
}

void WViewPrivate::clearDrag()
{
    itemsDrop.clear();

    if (areaDrop)
    {
        areaDrop->d_func()->dragLeaveEvent();

        areaDrop = NULL;
    }
}

//-------------------------------------------------------------------------------------------------

void WViewPrivate::setActive(bool active)
{
    if (this->active == active) return;

    Q_Q(WView);

    this->active = active;

    emit q->activeChanged();
}

//-------------------------------------------------------------------------------------------------

void WViewPrivate::setEntered(bool entered)
{
    if (this->entered == entered) return;

    Q_Q(WView);

    this->entered = entered;

    updateHoverable();

    if (entered == false)
    {
        setMousePos(QPoint(-1, -1));

        q->clearHover();
    }
    else q->updateHover();

    emit q->enteredChanged();
}

void WViewPrivate::setPressed(bool pressed)
{
    if (this->pressed == pressed) return;

    Q_Q(WView);

    this->pressed = pressed;

    updateHoverable();

    if (pressed == false)
    {
        q->updateHover();
    }

    emit q->pressedChanged();
}

void WViewPrivate::setDragging(bool dragging)
{
    if (this->dragging == dragging) return;

    Q_Q(WView);

    this->dragging = dragging;

    updateHoverable();

    if (dragging == false)
    {
        clearDrag();

        setPressed(false);

        if (entered)
        {
            itemsCursor.clear();

            q->updateHover();
        }

        emit q->draggingChanged();

        emit q->dragEnded();
    }
    else emit q->draggingChanged();
}

//-------------------------------------------------------------------------------------------------

void WViewPrivate::setDragged(bool dragged)
{
    if (this->dragged == dragged) return;

    Q_Q(WView);

    this->dragged = dragged;

    updateHoverable();

    updateMouse();

    emit q->draggedChanged();
}

void WViewPrivate::setResizing(bool resizing)
{
    if (this->resizing == resizing) return;

    Q_Q(WView);

    this->resizing = resizing;

    updateHoverable();

    updateMouse();

    emit q->resizingChanged();
}

//-------------------------------------------------------------------------------------------------

QRect WViewPrivate::getGeometryDefault(const QRect & rect) const
{
    int width  = sk->defaultWidth ();
    int height = sk->defaultHeight();

    int left;
    int right;
    int top;
    int bottom;

    if (width > rect.width() || height > rect.height())
    {
        left = rect.width() / 8;

        right = left;

        top = rect.height() / 8;

        bottom = top;
    }
    else
    {
        if (width == -1)
        {
            left = rect.width() / 8;

            right = left;
        }
        else
        {
            int size = rect.width() - width;

            left = qMax(0, size / 2);

            right = size - left;
        }

        if (height == -1)
        {
            top = rect.height() / 8;

            bottom = top;
        }
        else
        {
            int size = rect.height() - height;

            top = qMax(0, size / 2);

            bottom = size - top;
        }
    }

    return rect.adjusted(left, top, -right, -bottom);
}

QRect WViewPrivate::getGeometry(const QRect & rect) const
{
    int width  = rect.width()  / 8;
    int height = rect.height() / 8;

    return rect.adjusted(width, height, -width, -height);
}

//-------------------------------------------------------------------------------------------------

void WViewPrivate::setGeometryNormal(const QRect & rect)
{
    if (geometryNormal == rect) return;

    Q_Q(WView);

    geometryNormal = rect;

    emit q->geometryNormalChanged();
}

//-------------------------------------------------------------------------------------------------

void WViewPrivate::setMousePos(const QPoint & pos)
{
    if (mousePos == pos) return;

    Q_Q(WView);

    mousePos = pos;

    emit q->mousePosChanged();
}

//-------------------------------------------------------------------------------------------------

bool WViewPrivate::isUnderMouse() const
{
    Q_Q(const WView);

    int x = mousePos.x();
    int y = mousePos.y();

    if (x >= 0 && y >= 0 && x < q->width() && y < q->height())
    {
         return true;
    }
    else return false;
}

//-------------------------------------------------------------------------------------------------

void WViewPrivate::setCursor(WDeclarativeMouseArea::CursorShape shape)
{
    if (cursor != shape)
    {
        applyCursor(shape);
    }
}

void WViewPrivate::applyCursor(WDeclarativeMouseArea::CursorShape shape)
{
    Q_Q(WView);

    cursor = shape;

    if (cursors.contains(shape))
    {
        const QCursor & cursor = cursors.value(shape);

        if (QApplication::overrideCursor())
        {
             QApplication::changeOverrideCursor(cursor);
        }
        else QApplication::setOverrideCursor(cursor);
    }
    else
    {
        Qt::CursorShape shape = static_cast<Qt::CursorShape> (cursor);

        if (QApplication::overrideCursor())
        {
            if (shape)
            {
                 QApplication::changeOverrideCursor(shape);
            }
            else QApplication::restoreOverrideCursor();
        }
        else if (shape) QApplication::setOverrideCursor(shape);
    }

    emit q->mouseCursorChanged();
}

//-------------------------------------------------------------------------------------------------

void WViewPrivate::setKeyShiftPressed(bool pressed)
{
    if (keyShiftPressed == pressed) return;

    Q_Q(WView);

    keyShiftPressed = pressed;

    emit q->keyShiftPressedChanged();
}

void WViewPrivate::setKeyControlPressed(bool pressed)
{
    if (keyControlPressed == pressed) return;

    Q_Q(WView);

    keyControlPressed = pressed;

    emit q->keyControlPressedChanged();
}

void WViewPrivate::setKeyAltPressed(bool pressed)
{
    if (keyAltPressed == pressed) return;

    Q_Q(WView);

    keyAltPressed = pressed;

    emit q->keyAltPressedChanged();
}

//-------------------------------------------------------------------------------------------------

QList<WDeclarativeMouseArea *>
WViewPrivate::getMouseAreas(const QList<QGraphicsItem *> & items) const
{
    QList<WDeclarativeMouseArea *> mouseAreas;

    foreach (QGraphicsItem * item, items)
    {
        QGraphicsObject * graphicsObject = item->toGraphicsObject();

        if (graphicsObject == NULL) continue;

        WDeclarativeMouseArea * area = qobject_cast<WDeclarativeMouseArea *> (graphicsObject);

        if (area)
        {
            if (area->d_func()->hoverEnabled)
            {
                mouseAreas.append(area);
            }

            if (area->d_func()->hoverRetain)
            {
                return mouseAreas;
            }
        }
    }

    return mouseAreas;
}

QList<WDeclarativeMouseArea *>
WViewPrivate::getDropAreas(const QList<QGraphicsItem *> & items) const
{
    QList<WDeclarativeMouseArea *> dropAreas;

    foreach (QGraphicsItem * item, items)
    {
        QGraphicsObject * graphicsObject = item->toGraphicsObject();

        if (graphicsObject == NULL) continue;

        WDeclarativeMouseArea * area = qobject_cast<WDeclarativeMouseArea *> (graphicsObject);

        if (area && area->acceptDrops())
        {
            dropAreas.append(area);
        }
    }

    return dropAreas;
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WViewPrivate::onGeometryChanged()
{
    Q_Q(WView);

    if (maximized == false && fullScreen == false)
    {
        q->checkPosition();
    }

    emit q->availableGeometryChanged();
}

//-------------------------------------------------------------------------------------------------

void WViewPrivate::onFadeTimeout()
{
    Q_Q(WView);

    if (fadeVisible)
    {
        qreal opacity = q->windowOpacity() + fadeValue;

        if (opacity >= 1.0)
        {
            fadeIn();
        }
        else q->setWindowOpacity(opacity);
    }
    else
    {
        qreal opacity = q->windowOpacity() - fadeValue;

        if (opacity <= 0.0)
        {
            fadeOut();
        }
        else q->setWindowOpacity(opacity);
    }
}

void WViewPrivate::onIdleTimeout()
{
    Q_Q(WView); q->setIdle(true);
}

//-------------------------------------------------------------------------------------------------

void WViewPrivate::onCursorVisibleChanged()
{
    Q_Q(WView);

    updateHoverable();

    if (sk->cursorVisible() == false)
    {
        cursor = WDeclarativeMouseArea::BlankCursor;

        q->clearHover();
    }
    else q->updateHover();
}

//=================================================================================================
// WView
//=================================================================================================

WView::WView(QDeclarativeItem * item, QWidget * parent, Qt::WindowFlags flags)
    : WAbstractView(new WViewPrivate(this), parent, flags)
{
    Q_D(WView); d->init(item);
}

WView::WView(QWidget * parent, Qt::WindowFlags flags)
    : WAbstractView(new WViewPrivate(this), parent, flags)
{
    Q_D(WView); d->init(NULL);
}

//-------------------------------------------------------------------------------------------------
// Protected

WView::WView(WViewPrivate * p, QDeclarativeItem * item, QWidget * parent, Qt::WindowFlags flags)
    : WAbstractView(p, parent, flags)
{
    Q_D(WView); d->init(item);
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WView::activate()
{
    setMinimized(false);

    activateWindow();

    raise();

    setFocus();
}

/* Q_INVOKABLE */ void WView::raise()
{
    Q_D(WView);

    if (d->locked) return;

#ifdef Q_OS_WIN
    HWND id = (HWND) winId();

    SetWindowPos(id, HWND_TOPMOST,   0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    SetWindowPos(id, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
#else
    WAbstractView::raise();
#endif
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WView::close()
{
    Q_D(WView);

    if (d->closed) return;

    d->closed = true;

    emit beforeClose();

    if (d->fade)
    {
        if (d->fadeVisible)
        {
            d->startFade(false);
        }
    }
    else WAbstractView::close();
}

//-------------------------------------------------------------------------------------------------
// Geometry

/* Q_INVOKABLE */ int WView::getScreenNumber() const
{
    return wControllerView->screenNumber(this);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WView::setMinimumSize(int width, int height)
{
    Q_D(WView);

    int minimumWidth  = d->minimumWidth;
    int minimumHeight = d->minimumHeight;

    d->minimumWidth  = width;
    d->minimumHeight = height;

    WAbstractView::setMinimumSize(width, height);

    if (d->minimumWidth != minimumWidth)
    {
        emit minimumWidthChanged();
    }

    if (d->minimumHeight != minimumHeight)
    {
        emit minimumHeightChanged();
    }
}

/* Q_INVOKABLE */ void WView::setMaximumSize(int width, int height)
{
    Q_D(WView);

    int maximumWidth  = d->maximumWidth;
    int maximumHeight = d->maximumHeight;

    d->maximumWidth  = width;
    d->maximumHeight = height;

    if (width == -1)
    {
        if (height == -1)
        {
             WAbstractView::setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
        }
        else WAbstractView::setMaximumSize(QWIDGETSIZE_MAX, height);
    }
    else if (height == -1)
    {
        if (width == -1)
        {
             WAbstractView::setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
        }
        else WAbstractView::setMaximumSize(width, QWIDGETSIZE_MAX);
    }

    if (d->maximumWidth != maximumWidth)
    {
        emit maximumWidthChanged();
    }

    if (d->maximumHeight != maximumHeight)
    {
        emit maximumHeightChanged();
    }
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ QRect WView::getDefaultGeometry() const
{
    Q_D(const WView);

    QRect rect = wControllerView->availableGeometry(sk->defaultScreen());

    return d->getGeometryDefault(rect);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WView::setDefaultGeometry()
{
    Q_D(WView);

    if (d->maximized || d->fullScreen)
    {
        d->geometryNormal = getDefaultGeometry();
    }
    else setGeometry(getDefaultGeometry());
}

/* Q_INVOKABLE */ void WView::saveGeometry()
{
    Q_D(WView);

    d->setGeometryNormal(geometry());
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WView::checkPosition()
{
    QRect geometry = availableGeometry();

    int x = this->x();
    int y = this->y();

    int geometryX = geometry.x();
    int geometryY = geometry.y();

    int geometryWidth  = geometryX + geometry.width () - width ();
    int geometryHeight = geometryY + geometry.height() - height();

    bool update = false;

    if (x < geometryX)
    {
        x = geometryX;

        update = true;
    }
    else if (x > geometryWidth)
    {
        x = qMax(geometryX, geometryWidth);

        update = true;
    }

    if (y < geometryY)
    {
        y = geometryY;

        update = true;
    }
    else if (y > geometryHeight)
    {
        y = qMax(geometryY, geometryHeight);

        update = true;
    }

    if (update) move(x, y);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WView::originTo(qreal x, qreal y)
{
    Q_D(WView); d->item->setPos(-x, -y);
}

//-------------------------------------------------------------------------------------------------
// Drag

/* Q_INVOKABLE */ bool WView::testDrag(const QPointF & posA,
                                           const QPointF & posB, qreal distance)
{
    if (distance == -1) distance = QApplication::startDragDistance();

    if ((posA - posB).manhattanLength() >= distance)
    {
         return true;
    }
    else return false;
}

/* Q_INVOKABLE */ void WView::startDrag(const QString & text, int actions)
{
    Q_D(WView);

    if (d->dragging) return;

    d->scene->mouseGrabberItem()->ungrabMouse();

    clearHover();

    d->dragData.text = text;

    Qt::DropActions dropActions = static_cast<Qt::DropActions> (actions);

    d->dragData.actions = dropActions;

    if (dropActions.testFlag(Qt::MoveAction))
    {
        d->dragData.action = Qt::MoveAction;
    }
    else if (dropActions.testFlag(Qt::CopyAction))
    {
        d->dragData.action = Qt::CopyAction;
    }
    else if (dropActions.testFlag(Qt::LinkAction))
    {
        d->dragData.action = Qt::LinkAction;
    }
    else d->dragData.action = Qt::IgnoreAction;

    d->setDragging(true);
}

//-------------------------------------------------------------------------------------------------
// Hover

/* Q_INVOKABLE */ int WView::hoverCount() const
{
    Q_D(const WView); return d->itemsHovered.count();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WView::updateHover()
{
    Q_D(WView);

    if (d->hoverable)
    {
        QList<QGraphicsItem *> itemsCursor = items(d->mousePos);

        if (d->itemsCursor == itemsCursor) return;

        d->itemsCursor = itemsCursor;

        QList<WDeclarativeMouseArea *> areas = d->getMouseAreas(itemsCursor);

        foreach (WDeclarativeMouseArea * item, areas)
        {
            d->itemsHovered.removeOne(item);
        }

        foreach (WDeclarativeMouseArea * item, d->itemsHovered)
        {
            item->setHovered(false);
        }

        foreach (WDeclarativeMouseArea * item, areas)
        {
            item->setHovered(true);
        }

        d->itemsHovered = areas;

        if (areas.isEmpty())
        {
             d->setCursor(WDeclarativeMouseArea::ArrowCursor);
        }
        else d->setCursor(areas.first()->d_func()->cursor);
    }
    else d->updateDrag();
}

/* Q_INVOKABLE */ void WView::clearHover()
{
    Q_D(WView);

    if (d->resizing) return;

    d->itemsCursor.clear();

    foreach (WDeclarativeMouseArea * item, d->itemsHovered)
    {
        item->setHovered(false);
    }

    d->itemsHovered.clear();

    if (sk->cursorVisible())
    {
        d->setCursor(WDeclarativeMouseArea::ArrowCursor);
    }
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WView::checkLeave(int msec)
{
    Q_D(WView); d->timerLeave.start(msec);
}

//-------------------------------------------------------------------------------------------------
// Shot

/* Q_INVOKABLE */ QPixmap WView::takeShot(int x, int y, int width, int height) const
{
#ifdef QT_4
    Q_D(const WView);

    if (d->opengl)
    {
        if (width  == -1) { width  = this->width (); }
        if (height == -1) { height = this->height(); }

        QGLWidget * viewport = qobject_cast<QGLWidget *> (this->viewport());

        // FIXME Windows: Making sure we grab the front buffer.
#ifdef Q_OS_WIN
        glReadBuffer(GL_FRONT);
#endif

        QImage image = viewport->grabFrameBuffer().copy(x, y, width, height);

        return QPixmap::fromImage(image);
    }
    else return QPixmap::grabWidget(viewport(), x, y, width, height);
#else
    //---------------------------------------------------------------------------------------------
    // FIXME Windows: Workaround for opengl full screen flicker.

    if (width  == -1) { width  = this->width (); }
    if (height == -1) { height = this->height(); }

    //---------------------------------------------------------------------------------------------

    return viewport()->grab(QRect(x, y, width, height));
#endif
}

/* Q_INVOKABLE */ bool WView::saveShot(const QString & fileName, int x,     int y,
                                                                     int width, int height) const
{
    QImage image = takeShot(x, y, width, height).toImage();

    return image.save(fileName, "png");
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ QPixmap WView::takeItemShot(QGraphicsObject * item,
                                              bool              recursive,
                                              const QColor    & background,
                                              bool              forceVisible) const
{
    return wControllerView->takeItemShot(item, recursive, background, forceVisible);
}

/* Q_INVOKABLE */ bool WView::saveItemShot(const QString   & fileName,
                                           QGraphicsObject * item,
                                           bool              recursive,
                                           const QColor    & background,
                                           bool              forceVisible) const
{
    return wControllerView->saveItemShot(fileName, item, recursive, background, forceVisible);
}

//-------------------------------------------------------------------------------------------------
// Cursor

/* Q_INVOKABLE */ void WView::registerCursor(WDeclarativeMouseArea::CursorShape shape,
                                             const QCursor                    & cursor)
{
    Q_D(WView);

    d->cursors.insert(shape, cursor);

    if (d->cursor == shape)
    {
        d->applyCursor(shape);
    }
}

/* Q_INVOKABLE */ void WView::unregisterCursor(WDeclarativeMouseArea::CursorShape shape)
{
    Q_D(WView);

    d->cursors.remove(shape);

    if (d->cursor == shape)
    {
        d->applyCursor(shape);
    }
}

/* Q_INVOKABLE */ void WView::unregisterCursors()
{
    Q_D(WView);

    d->cursors.clear();

    d->applyCursor(d->cursor);
}

//-------------------------------------------------------------------------------------------------
// Input

/* Q_INVOKABLE */ void WView::mouseMove(int x, int y, Qt::MouseButton button) const
{
    QPoint point(x, y);

    QMouseEvent event(QEvent::MouseMove, point, mapToGlobal(point), button, Qt::NoButton,
                                                                            Qt::NoModifier);

    QCoreApplication::sendEvent(viewport(), &event);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WView::mousePress(Qt::MouseButton button) const
{
    Q_D(const WView);

    QMouseEvent event(QEvent::MouseButtonPress,
                      d->mousePos, mapToGlobal(d->mousePos), button, Qt::NoButton, Qt::NoModifier);

    QCoreApplication::sendEvent(viewport(), &event);
}

/* Q_INVOKABLE */ void WView::mouseRelease(Qt::MouseButton button) const
{
    Q_D(const WView);

    QMouseEvent event(QEvent::MouseButtonRelease,
                      d->mousePos, mapToGlobal(d->mousePos), button, Qt::NoButton, Qt::NoModifier);

    QCoreApplication::sendEvent(viewport(), &event);
}

/* Q_INVOKABLE */ void WView::mouseClick(Qt::MouseButton button, int msec) const
{
    mousePress(button);

    Sk::wait(msec);

    mouseRelease(button);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WView::wheel(Qt::Orientation orientation, int delta) const
{
    Q_D(const WView);

    QWheelEvent event(d->mousePos, mapToGlobal(d->mousePos), delta, Qt::NoButton,
                                                                    Qt::NoModifier, orientation);

    QCoreApplication::sendEvent(viewport(), &event);
}

/* Q_INVOKABLE */ void WView::wheelUp(int delta) const
{
    wheel(Qt::Vertical, delta);
}

/* Q_INVOKABLE */ void WView::wheelDown(int delta) const
{
    wheel(Qt::Vertical, delta);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WView::keyPress(int key, Qt::KeyboardModifiers modifiers) const
{
    QKeyEvent event(QEvent::KeyPress, key, modifiers);

    QCoreApplication::sendEvent(viewport(), &event);
}

/* Q_INVOKABLE */ void WView::keyRelease(int key, Qt::KeyboardModifiers modifiers) const
{
    QKeyEvent event(QEvent::KeyRelease, key, modifiers);

    QCoreApplication::sendEvent(viewport(), &event);
}

/* Q_INVOKABLE */ void WView::keyClick(int key, Qt::KeyboardModifiers modifiers, int msec) const
{
    keyPress(key, modifiers);

    Sk::wait(msec);

    keyRelease(key, modifiers);
}

//-------------------------------------------------------------------------------------------------
// QML

/* Q_INVOKABLE */ void WView::mouseMove(int x, int y, int button) const
{
    mouseMove(x, y, static_cast<Qt::MouseButton> (button));
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WView::mousePress(int button) const
{
    mousePress(static_cast<Qt::MouseButton> (button));
}

/* Q_INVOKABLE */ void WView::mouseRelease(int button) const
{
    mouseRelease(static_cast<Qt::MouseButton> (button));
}

/* Q_INVOKABLE */ void WView::mouseClick(int button, int msec) const
{
    mouseClick(static_cast<Qt::MouseButton> (button), msec);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WView::wheel(int orientation, int delta) const
{
    wheel(static_cast<Qt::Orientation> (orientation), delta);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WView::keyPress(int key, int modifiers) const
{
    keyPress(key, static_cast<Qt::KeyboardModifiers> (modifiers));
}

/* Q_INVOKABLE */ void WView::keyRelease(int key, int modifiers) const
{
    keyRelease(key, static_cast<Qt::KeyboardModifiers> (modifiers));
}

/* Q_INVOKABLE */ void WView::keyClick(int key, int modifiers, int msec) const
{
    keyClick(key, static_cast<Qt::KeyboardModifiers> (modifiers), msec);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WView::registerCursorUrl(int shape, const QUrl  & url, const QSize & size)
{
    QPixmap pixmap;

    if (size.isValid())
    {
        QImageReader reader(WControllerFile::toLocalFile(url));

        reader.setScaledSize(size);

        pixmap = QPixmap::fromImageReader(&reader);
    }
    else pixmap = QPixmap(WControllerFile::toLocalFile(url));

    QCursor cursor(pixmap);

    registerCursor(static_cast<WDeclarativeMouseArea::CursorShape> (shape), cursor);
}

/* Q_INVOKABLE */ void WView::unregisterCursor(int shape)
{
    unregisterCursor(static_cast<WDeclarativeMouseArea::CursorShape> (shape));
}

//-------------------------------------------------------------------------------------------------
// Static interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ bool WView::compressShots(const QString & path, int quality)
{
    return WControllerView::compressShots(path, quality);
}

//-------------------------------------------------------------------------------------------------
// Protected functions
//-------------------------------------------------------------------------------------------------

/* virtual */ void WView::drawBackground(QPainter *, const QRectF &) {}
/* virtual */ void WView::drawForeground(QPainter *, const QRectF &) {}

//-------------------------------------------------------------------------------------------------
// Events
//-------------------------------------------------------------------------------------------------

/* virtual */ void WView::showEvent(QShowEvent * event)
{
    Q_D(WView);

    if (d->fade && d->fadeVisible == false)
    {
        d->startFade(true);
    }

    WAbstractView::showEvent(event);

    if (d->maximized)
    {
        showMaximized();
    }
    else if (d->fullScreen)
    {
        showFullScreen();
    }
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WView::moveEvent(QMoveEvent * event)
{
    Q_D(WView);

    d->updateMouse();

    WAbstractView::moveEvent(event);
}

/* virtual */ void WView::resizeEvent(QResizeEvent * event)
{
    Q_D(WView);

    d->updateMouse();

    WAbstractView::resizeEvent(event);

    int width  = this->width ();
    int height = this->height();

    if (d->item)
    {
        d->applySize(width, height);
    }
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WView::enterEvent(QEvent * event)
{
    Q_D(WView);

    d->setEntered(true);

    WAbstractView::enterEvent(event);
}

/* virtual */ void WView::leaveEvent(QEvent * event)
{
    Q_D(WView);

    // FIXME: Sometimes we get a leaveEvent for no reason.
    if (d->timerLeave.isActive())
    {
        if (d->item == NULL || d->item->isUnderMouse() == false)
        {
            d->setEntered(false);
        }
    }
    else d->setEntered(false);

    WAbstractView::leaveEvent(event);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WView::mousePressEvent(QMouseEvent * event)
{
    Q_D(WView);

    if (d->dragging)
    {
        if (event->button() == Qt::RightButton)
        {
            d->setDragging(false);
        }

        return;
    }

    d->setPressed(true);

    d->button  = event->button ();
    d->buttons = event->buttons();

    if (d->idleCheck)
    {
        setIdle(false);

        d->idleTimer.start();
    }

    WDeclarativeMouseEvent mouse(event->x(), event->y(), event->button(), event->buttons(),
                                 event->modifiers(), false, false);

    mouse.setAccepted(false);

    emit mousePressed(&mouse);

    d->mouseAccepted = mouse.isAccepted();

    if (d->mouseAccepted == false)
    {
        WAbstractView::mousePressEvent(event);
    }
}

/* virtual */ void WView::mouseReleaseEvent(QMouseEvent * event)
{
    Q_D(WView);

    if (d->dragging && event->button() == Qt::LeftButton)
    {
        if (d->areaDrop)
        {
            QPointF pos = d->areaDrop->mapFromScene(d->mousePos);

            d->areaDrop->d_func()->dropEvent(pos, d->dragData);

            d->areaDrop = NULL;
        }

        d->setDragging(false);
    }
    else if (event->buttons() == Qt::NoButton)
    {
        d->setPressed(false);
    }

#ifdef QT_LATEST
    if (d->currentResizer)
    {
        d->currentResizer = NULL;

        d->setResizing(false);
    }
#endif

    if (d->mouseAccepted)
    {
        WDeclarativeMouseEvent mouse(event->x(), event->y(), event->button(), event->buttons(),
                                     event->modifiers(), true, false);

        mouse.setAccepted(false);

        emit mouseReleased(&mouse);
    }

    WAbstractView::mouseReleaseEvent(event);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WView::mouseDoubleClickEvent(QMouseEvent * event)
{
    Q_D(WView);

    if (d->dragging) return;

    // FIXME: mousePressEvent is not called before mouseDoubleClickEvent.
    d->setPressed(true);

    WDeclarativeMouseEvent mouse(event->x(), event->y(), event->button(), event->buttons(),
                                 event->modifiers(), true, false);

    mouse.setAccepted(false);

    emit mouseDoubleClicked(&mouse);

    if (mouse.isAccepted() == false)
    {
        WAbstractView::mouseDoubleClickEvent(event);
    }
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WView::mouseMoveEvent(QMouseEvent * event)
{
    Q_D(WView);

    d->setEntered(true);

    if (d->pressed && event->buttons() == Qt::NoButton)
    {
        d->pressed = false;

        emit pressedChanged();
    }

    d->setMousePos(event->pos());

#ifdef QT_4
    if (d->currentResizer)
    {
        d->currentResizer = NULL;

        d->setResizing(false);
    }
#endif

    if (d->idleCheck)
    {
        setIdle(false);

        d->idleTimer.start();
    }

    WAbstractView::mouseMoveEvent(event);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WView::dragEnterEvent(QDragEnterEvent * event)
{
    Q_D(WView);

    activate();

    if (d->mime) delete d->mime;

    const QMimeData * mime = event->mimeData();

    d->mime = Sk::duplicateMime(mime);

    if (mime->hasUrls())
    {
         d->dragData.text = mime->urls().first().toString();
    }
    else d->dragData.text = mime->text();

    d->dragData.actions = event->possibleActions();
    d->dragData.action  = event->proposedAction ();

    if (d->drag)
    {
        d->drag = NULL;

#ifdef Q_OS_WIN
        keybd_event(VK_ESCAPE, 0x81,               0, 0);
        keybd_event(VK_ESCAPE, 0x81, KEYEVENTF_KEYUP, 0);
#endif
    }
    else
    {
        d->setDragging(true);

#ifdef Q_OS_WIN
        mouse_event(MOUSEEVENTF_LEFTUP,   0, 0, 0, 0);
        mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
#endif
    }

    d->setEntered(true);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WView::keyPressEvent(QKeyEvent * event)
{
    Q_D(WView);

#ifdef Q_OS_WIN
    d->keyVirtual = event->nativeVirtualKey();
    d->keyScan    = event->nativeScanCode  ();
#endif

    int key = event->key();

    if (key == Qt::Key_Shift)
    {
        d->setKeyShiftPressed(true);
    }
    else if (key == Qt::Key_Control)
    {
        d->setKeyControlPressed(true);
    }
    else if (key == Qt::Key_Alt)
    {
        d->setKeyAltPressed(true);
    }
    else if (key == Qt::Key_Escape && d->dragging)
    {
        d->setDragging(false);

        return;
    }

    WDeclarativeKeyEvent keyEvent(*event);

    emit keyPressed(&keyEvent);

    d->keyAccepted = keyEvent.isAccepted();

    if (d->keyAccepted == false)
    {
        WAbstractView::keyPressEvent(event);
    }
}

/* virtual */ void WView::keyReleaseEvent(QKeyEvent * event)
{
    Q_D(WView);

#ifdef Q_OS_WIN
    d->keyVirtual = 0;
#endif

    int key = event->key();

    if (key == Qt::Key_Shift)
    {
        d->setKeyShiftPressed(false);
    }
    else if (key == Qt::Key_Control)
    {
        d->setKeyControlPressed(false);
    }
    else if (key == Qt::Key_Alt)
    {
        d->setKeyAltPressed(false);
    }

    if (d->keyAccepted)
    {
        WDeclarativeKeyEvent keyEvent(*event);

        emit keyReleased(&keyEvent);
    }

    WAbstractView::keyReleaseEvent(event);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WView::focusInEvent(QFocusEvent * event)
{
    Q_D(WView);

    d->setActive(true);

    if (d->minimized != WAbstractView::isMinimized())
    {
        d->minimized = !(d->minimized);

        emit minimizedChanged();
    }

    WAbstractView::focusInEvent(event);
}

/* virtual */ void WView::focusOutEvent(QFocusEvent * event)
{
    Q_D(WView);

    d->setActive  (false);
    d->setDragging(false);

#ifdef Q_OS_WIN
    d->keyVirtual = 0;
#endif

    d->setKeyShiftPressed  (false);
    d->setKeyControlPressed(false);
    d->setKeyAltPressed    (false);

    if (d->keyAccepted)
    {
        WDeclarativeKeyEvent keyEvent(QEvent::KeyRelease, 0);

        emit keyReleased(&keyEvent);
    }

    WAbstractView::focusOutEvent(event);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WView::timerEvent(QTimerEvent *)
{
    updateHover();
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WView::closeEvent(QCloseEvent * event)
{
    Q_D(WView);

    if (d->closed) return;

    event->ignore();

    if (d->item) d->item->setFocus(true);

    close();
}

//-------------------------------------------------------------------------------------------------
// WAbstractView reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WView::onStateChanged(Qt::WindowState state)
{
    if (state == Qt::WindowMaximized)
    {
        Q_D(WView);

        if (d->maximized == false)
        {
            d->maximized = true;

            emit maximizedChanged();
        }
    }
    else // if (state == Qt::WindowNoState)
    {
        Q_D(WView);

        if (d->maximized)
        {
            d->maximized = false;

            emit maximizedChanged();
        }
    }

    emit stateChanged(state);
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

QDeclarativeItem * WView::item() const
{
    Q_D(const WView); return d->item;
}

void WView::setItem(QDeclarativeItem * item)
{
    Q_D(WView);

    if (d->item)
    {
        d->scene->removeItem(d->item);

        disconnect(d->item, 0, this, 0);

        delete d->item;

        return;
    }

    d->item = item;

    if (item)
    {
        d->scene->addItem(item);

        d->applySize(width(), height());

        connect(item, SIGNAL(widthChanged ()), this, SIGNAL(itemWidthChanged ()));
        connect(item, SIGNAL(heightChanged()), this, SIGNAL(itemHeightChanged()));

        connect(item, SIGNAL(xChanged()), this, SIGNAL(originXChanged()));
        connect(item, SIGNAL(yChanged()), this, SIGNAL(originYChanged()));
    }

    emit itemChanged();
}

//-------------------------------------------------------------------------------------------------

qreal WView::itemWidth() const
{
    Q_D(const WView);

    if (d->item)
    {
         return d->item->width();
    }
    else return 0;
}

qreal WView::itemHeight() const
{
    Q_D(const WView);

    if (d->item)
    {
         return d->item->height();
    }
    else return 0;
}

//-------------------------------------------------------------------------------------------------

Qt::WindowFlags WView::flags() const
{
    Q_D(const WView); return d->flags;
}

void WView::setFlags(Qt::WindowFlags flags)
{
    Q_D(WView);

    if (d->flags == flags) return;

    d->flags = flags;

    d->updateFlags();

    emit flagsChanged();
}

//-------------------------------------------------------------------------------------------------

void WView::setX(int x)
{
    if (this->x() == x) return;

    Q_D(WView);

    if (d->maximized || d->fullScreen)
    {
        if (d->geometryNormal.left() != x)
        {
            d->geometryNormal.setLeft(x);

            emit geometryNormalChanged();
        }
    }
    else move(x, y());

    emit xChanged();
}

void WView::setY(int y)
{
    if (this->y() == y) return;

    Q_D(WView);

    if (d->maximized || d->fullScreen)
    {
        if (d->geometryNormal.top() != y)
        {
            d->geometryNormal.setTop(y);

            emit geometryNormalChanged();
        }
    }
    else move(x(), y);

    emit yChanged();
}

//-------------------------------------------------------------------------------------------------

void WView::setWidth(int width)
{
    if (this->width() == width || width < 0) return;

    Q_D(WView);

    if (d->maximized || d->fullScreen)
    {
        if (d->geometryNormal.right() != width)
        {
            d->geometryNormal.setRight(width);

            emit geometryNormalChanged();
        }
    }
    else resize(width, height());

    emit widthChanged();
}

void WView::setHeight(int height)
{
    if (this->height() == height || height < 0) return;

    Q_D(WView);

    if (d->maximized || d->fullScreen)
    {
        if (d->geometryNormal.bottom() != height)
        {
            d->geometryNormal.setBottom(height);

            emit geometryNormalChanged();
        }
    }
    else resize(width(), height);

    emit heightChanged();
}

//-------------------------------------------------------------------------------------------------

int WView::centerX() const
{
    return (availableGeometry().width() - width()) / 2;
}

int WView::centerY() const
{
    return (availableGeometry().height() - height()) / 2;
}

//-------------------------------------------------------------------------------------------------

qreal WView::originX() const
{
    Q_D(const WView);

    if (d->item)
    {
         return -(d->item->x());
    }
    else return 0;
}

void WView::setOriginX(qreal x)
{
    Q_D(WView);

    if (d->item == NULL || d->item->x() == -x) return;

    d->item->setX(-x);

    emit originXChanged();
}

qreal WView::originY() const
{
    Q_D(const WView);

    if (d->item)
    {
         return -(d->item->y());
    }
    else return 0;
}

void WView::setOriginY(qreal y)
{
    Q_D(WView);

    if (d->item == NULL || d->item->y() == -y) return;

    d->item->setY(-y);

    emit originYChanged();
}

//-------------------------------------------------------------------------------------------------

qreal WView::zoom() const
{
    Q_D(const WView); return d->zoom;
}

void WView::setZoom(qreal zoom)
{
    Q_D(WView);

    d->zoom = zoom;

    if (d->item)
    {
        d->applySize(width(), height());
    }

    emit zoomChanged();
}

//-------------------------------------------------------------------------------------------------

int WView::minimumWidth() const
{
    Q_D(const WView); return d->minimumWidth;
}

void WView::setMinimumWidth(int width)
{
    Q_D(WView);

    if (d->minimumWidth == width) return;

    d->minimumWidth = width;

    WAbstractView::setMinimumSize(width, d->minimumHeight);

    minimumWidthChanged();
}

int WView::minimumHeight() const
{
    Q_D(const WView); return d->minimumHeight;
}

void WView::setMinimumHeight(int height)
{
    Q_D(WView);

    if (d->minimumHeight == height) return;

    d->minimumHeight = height;

    WAbstractView::setMinimumSize(d->minimumWidth, height);

    minimumHeightChanged();
}

//-------------------------------------------------------------------------------------------------

int WView::maximumWidth() const
{
    Q_D(const WView); return d->maximumWidth;
}

void WView::setMaximumWidth(int width)
{
    Q_D(WView);

    if (d->maximumWidth == width) return;

    d->maximumWidth = width;

    if (width == -1)
    {
         WAbstractView::setMaximumSize(QWIDGETSIZE_MAX, WAbstractView::maximumHeight());
    }
    else WAbstractView::setMaximumSize(width, WAbstractView::maximumHeight());

    maximumWidthChanged();
}

int WView::maximumHeight() const
{
    Q_D(const WView); return d->maximumHeight;
}

void WView::setMaximumHeight(int height)
{
    Q_D(WView);

    if (d->maximumHeight == height) return;

    d->maximumHeight = height;

    if (height == -1)
    {
         WAbstractView::setMaximumSize(WAbstractView::maximumWidth(), QWIDGETSIZE_MAX);
    }
    else WAbstractView::setMaximumSize(WAbstractView::maximumWidth(), height);

    maximumHeightChanged();
}

//-------------------------------------------------------------------------------------------------

QRect WView::geometryNormal() const
{
    Q_D(const WView); return d->geometryNormal;
}

//-------------------------------------------------------------------------------------------------

bool WView::isMinimized() const
{
    Q_D(const WView); return d->minimized;
}

void WView::setMinimized(bool minimized)
{
    Q_D(WView);

    if (d->minimized == minimized) return;

    d->minimized = minimized;

    if (minimized) showMinimized();
    else           showNormal   ();

    emit minimizedChanged();
}

//-------------------------------------------------------------------------------------------------

bool WView::isMaximized() const
{
    Q_D(const WView); return d->maximized;
}

void WView::setMaximized(bool maximized)
{
    Q_D(WView);

    if (d->maximized == maximized) return;

    if (maximized)
    {
#ifdef Q_OS_LINUX
        // FIXME: Workaround to undock the window.
        QRect rect = availableGeometry();

        if (height() == rect.height())
        {
            showMaximized();
            showNormal   ();

            setGeometry(d->getGeometry(rect));

            return;
        }
#endif

        d->maximized = true;

        showMaximized();
    }
    else
    {
        d->maximized = false;

        if (d->fullScreen == false)
        {
            showNormal();
        }
    }

    emit maximizedChanged();
}

//-------------------------------------------------------------------------------------------------

bool WView::isFullScreen() const
{
    Q_D(const WView); return d->fullScreen;
}

void WView::setFullScreen(bool fullScreen)
{
    Q_D(WView);

    if (d->fullScreen == fullScreen) return;

    if (fullScreen)
    {
        d->fullScreen = true;

        showFullScreen();
    }
    else
    {
        d->fullScreen = false;

        if (d->maximized)
        {
             showMaximized();
        }
        else showNormal();
    }

    emit fullScreenChanged();
}

//-------------------------------------------------------------------------------------------------

bool WView::isLocked() const
{
    Q_D(const WView); return d->locked;
}

void WView::setLocked(bool locked)
{
    Q_D(WView);

    if (d->locked == locked) return;

    d->locked = locked;

    d->setPressed(false);

#ifdef Q_OS_WIN
    // FIXME: Locking during keyPress skips keyRelease.
    if (d->keyVirtual)
    {
        keybd_event(d->keyVirtual, d->keyScan, KEYEVENTF_KEYUP, 0);
    }
#endif

    d->updateFlags();

    emit lockedChanged();
}

//-------------------------------------------------------------------------------------------------

bool WView::isActive() const
{
    Q_D(const WView); return d->active;
}

bool WView::isEntered() const
{
    Q_D(const WView); return d->entered;
}

bool WView::isPressed() const
{
    Q_D(const WView); return d->pressed;
}

bool WView::isDragging() const
{
    Q_D(const WView); return d->dragging;
}

bool WView::isDragged() const
{
    Q_D(const WView); return d->dragged;
}

bool WView::isResizing() const
{
    Q_D(const WView); return d->resizing;
}

//-------------------------------------------------------------------------------------------------

QPoint WView::mousePos() const
{
    Q_D(const WView); return d->mousePos;
}

//-------------------------------------------------------------------------------------------------

int WView::mouseX() const
{
    Q_D(const WView); return d->mousePos.x();
}

int WView::mouseY() const
{
    Q_D(const WView); return d->mousePos.y();
}

//-------------------------------------------------------------------------------------------------

WDeclarativeMouseArea::CursorShape WView::mouseCursor() const
{
    Q_D(const WView); return d->cursor;
}

//-------------------------------------------------------------------------------------------------

bool WView::opengl() const
{
    Q_D(const WView); return d->opengl;
}

void WView::setOpengl(bool enabled)
{
    Q_D(WView);

    if (d->opengl == enabled) return;

    d->opengl = enabled;

    if (enabled)
    {
        d->updateViewport();

        setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    }
    else
    {
        setViewport(NULL);

        setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    }

    emit openglChanged();
}

//-------------------------------------------------------------------------------------------------

bool WView::antialias() const
{
    Q_D(const WView); return d->antialias;
}

void WView::setAntialias(bool enabled)
{
    Q_D(WView);

    if (d->antialias == enabled) return;

    d->antialias = enabled;

    setRenderHint(QPainter::Antialiasing, enabled);

    emit antialiasChanged();
}

//-------------------------------------------------------------------------------------------------

bool WView::vsync() const
{
    Q_D(const WView); return d->vsync;
}

void WView::setVsync(bool enabled)
{
    Q_D(WView);

    if (d->vsync == enabled) return;

    d->vsync = enabled;

    if (d->opengl) d->updateViewport();

    emit vsyncChanged();
}

//-------------------------------------------------------------------------------------------------

bool WView::hoverEnabled() const
{
    Q_D(const WView); return d->hover;
}

void WView::setHoverEnabled(bool enabled)
{
    Q_D(WView);

    if (d->hover == enabled) return;

    d->hover = enabled;

    d->updateHoverable();

    emit hoverEnabledChanged();
}

//-------------------------------------------------------------------------------------------------

bool WView::fadeEnabled() const
{
    Q_D(const WView); return d->fade;
}

void WView::setFadeEnabled(bool enabled)
{
    Q_D(WView);

    if (d->fade == enabled) return;

    d->fade = enabled;

    if (enabled == false && d->fadeTimer.isActive())
    {
        if (d->fadeVisible)
        {
             d->fadeIn();
        }
        else d->fadeOut();
    }

    emit fadeEnabledChanged();
}

int WView::fadeDuration() const
{
    Q_D(const WView); return d->fadeDuration;
}

void WView::setFadeDuration(int msec)
{
    Q_D(WView);

    if (d->fadeDuration == msec) return;

    d->fadeDuration = msec;

    d->fadeValue = 16.0 / d->fadeDuration;

    emit fadeDurationChanged();
}

//-------------------------------------------------------------------------------------------------
// Mouse idle

bool WView::idleCheck() const
{
    Q_D(const WView); return d->idleCheck;
}

void WView::setIdleCheck(bool check)
{
    Q_D(WView);

    if (d->idleCheck == check) return;

    d->idleCheck = check;

    if (d->idleCheck == false)
    {
        d->idleTimer.stop();

        setIdle(false);
    }
    else d->idleTimer.start();

    emit idleCheckChanged();
}

//-------------------------------------------------------------------------------------------------

bool WView::idle() const
{
    Q_D(const WView); return d->idle;
}

void WView::setIdle(bool idle)
{
    Q_D(WView);

    if (d->idle == idle) return;

    d->idle = idle;

    d->idleTimer.stop();

    emit idleChanged();
}

//-------------------------------------------------------------------------------------------------

int WView::idleDelay() const
{
    Q_D(const WView); return d->idleDelay;
}

void WView::setIdleDelay(int msec)
{
    Q_D(WView);

    if (d->idleDelay == msec) return;

    d->idleDelay = msec;

    d->idleTimer.setInterval(msec);

    if (d->idleCheck)
    {
        d->idleTimer.start();
    }

    emit idleDelayChanged();
}

//-------------------------------------------------------------------------------------------------
// Keys

bool WView::keyShiftPressed() const
{
    Q_D(const WView); return d->keyShiftPressed;
}

bool WView::keyControlPressed() const
{
    Q_D(const WView); return d->keyControlPressed;
}

bool WView::keyAltPressed() const
{
    Q_D(const WView); return d->keyAltPressed;
}

//-------------------------------------------------------------------------------------------------
// Screen

QRect WView::availableGeometry() const
{
    return wControllerView->availableGeometry(this);
}

QRect WView::screenGeometry() const
{
    return wControllerView->screenGeometry(this);
}

#endif // SK_NO_VIEW
