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

#include "WMainView.h"

#ifndef SK_NO_MAINVIEW

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
// WMainViewScene
//=================================================================================================

WMainViewScene::WMainViewScene(WMainView * parent) : QGraphicsScene(parent)
{
    mainView = parent;
}

//=================================================================================================
// WMainViewPrivate
//=================================================================================================

WMainViewPrivate::WMainViewPrivate(WMainView * p) : WAbstractViewPrivate(p) {}

/* virtual */ WMainViewPrivate::~WMainViewPrivate()
{
    Q_Q(WMainView);

    if (mime) delete mime;

    W_GET_CONTROLLER(WControllerView, controller);

    if (controller) controller->d_func()->unregisterMainView(q);
}

//-------------------------------------------------------------------------------------------------

void WMainViewPrivate::init(QDeclarativeItem * item)
{
    Q_Q(WMainView);

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

    autoSize = true;

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

    scene = new WMainViewScene(q);

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

    q->viewport()->setAttribute(Qt::WA_OpaquePaintEvent);
    q->viewport()->setAttribute(Qt::WA_NoSystemBackground);

    //---------------------------------------------------------------------------------------------
    // Default size

    updateMinimumSize();

    q->setGeometry(q->getDefaultGeometry());

    //---------------------------------------------------------------------------------------------
    // Timers

    q->startTimer(16);

    //---------------------------------------------------------------------------------------------
    // Registering main view

    wControllerView->d_func()->registerMainView(q);

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

void WMainViewPrivate::startFade(bool visible)
{
    Q_Q(WMainView);

    fadeVisible = visible;

    if (visible)
    {
         q->setWindowOpacity(fadeValue);
    }
    else q->setWindowOpacity(1.0 - fadeValue);

    fadeTimer.start(16);
}

//-------------------------------------------------------------------------------------------------

void WMainViewPrivate::fadeIn()
{
    Q_Q(WMainView);

    fadeTimer.stop();

    q->setWindowOpacity(1.0);

    emit q->fadeIn();
}

void WMainViewPrivate::fadeOut()
{
    Q_Q(WMainView);

    fadeTimer.stop();

    q->setWindowOpacity(0.0);

    emit q->fadeOut();

    q->WAbstractView::close();
}

//-------------------------------------------------------------------------------------------------

void WMainViewPrivate::showNormal()
{
    Q_Q(WMainView);

    if (fullScreen)
    {
        fullScreen = false;

        if (maximized == false)
        {
            q->showNormal();

            q->setGeometry(geometryNormal);
        }
        else q->showMaximized();

        emit q->fullScreenChanged();
    }
    else
    {
        maximized = false;

        q->showNormal();

        q->setGeometry(geometryNormal);

        emit q->maximizedChanged();
    }
}

//-------------------------------------------------------------------------------------------------

void WMainViewPrivate::applySize(int width, int height)
{
    item->setSize(QSizeF(width * zoom, height * zoom));
}

//-------------------------------------------------------------------------------------------------

void WMainViewPrivate::updateFlags()
{
    Q_Q(WMainView);

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

void WMainViewPrivate::updateViewport()
{
    Q_Q(WMainView);

    QGLFormat format = QGLFormat::defaultFormat();

    format.setSampleBuffers(true);

    if (vsync)
    {
        format.setSwapInterval(1);
    }

    q->setViewport(new QGLWidget(format));
}

//-------------------------------------------------------------------------------------------------

void WMainViewPrivate::updateMinimumSize()
{
    Q_Q(WMainView);

    QSize size = q->minimumSizeHint();

//#ifdef Q_OS_MAC
//        size = size.expandedTo(QSize(72, 24));
//#endif

    q->WAbstractView::setMinimumSize(size);
}

//-------------------------------------------------------------------------------------------------

void WMainViewPrivate::updateHoverable()
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

void WMainViewPrivate::updateMouse()
{
    if (dragged || resizing) return;

    Q_Q(WMainView);

    QPoint pos = QCursor::pos();

#ifdef Q_OS_LINUX
    setMousePos(q->mapFromGlobal(pos));
#else
    setMousePos(QPoint(pos.x() - q->x(), pos.y() - q->y()));
#endif
}

//-------------------------------------------------------------------------------------------------

void WMainViewPrivate::updateDrag()
{
    if (dragging == false || drag) return;

    Q_Q(WMainView);

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

void WMainViewPrivate::clearDrag()
{
    itemsDrop.clear();

    if (areaDrop)
    {
        areaDrop->d_func()->dragLeaveEvent();

        areaDrop = NULL;
    }
}

//-------------------------------------------------------------------------------------------------

void WMainViewPrivate::setActive(bool active)
{
    if (this->active == active) return;

    Q_Q(WMainView);

    this->active = active;

    emit q->activeChanged();
}

//-------------------------------------------------------------------------------------------------

void WMainViewPrivate::setEntered(bool entered)
{
    if (this->entered == entered) return;

    Q_Q(WMainView);

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

void WMainViewPrivate::setPressed(bool pressed)
{
    if (this->pressed == pressed) return;

    Q_Q(WMainView);

    this->pressed = pressed;

    updateHoverable();

    if (pressed == false)
    {
        q->updateHover();
    }

    emit q->pressedChanged();
}

void WMainViewPrivate::setDragging(bool dragging)
{
    if (this->dragging == dragging) return;

    Q_Q(WMainView);

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

void WMainViewPrivate::setDragged(bool dragged)
{
    if (this->dragged == dragged) return;

    Q_Q(WMainView);

    this->dragged = dragged;

    updateHoverable();

    updateMouse();

    emit q->draggedChanged();
}

void WMainViewPrivate::setResizing(bool resizing)
{
    if (this->resizing == resizing) return;

    Q_Q(WMainView);

    this->resizing = resizing;

    updateHoverable();

    updateMouse();

    emit q->resizingChanged();
}

//-------------------------------------------------------------------------------------------------

QRect WMainViewPrivate::getGeometry(const QRect & rect) const
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

//-------------------------------------------------------------------------------------------------

void WMainViewPrivate::setGeometryNormal(const QRect & rect)
{
    if (geometryNormal == rect) return;

    Q_Q(WMainView);

    geometryNormal = rect;

    emit q->geometryNormalChanged();
}

//-------------------------------------------------------------------------------------------------

void WMainViewPrivate::setMousePos(const QPoint & pos)
{
    if (mousePos == pos) return;

    Q_Q(WMainView);

    mousePos = pos;

    emit q->mousePosChanged();
}

//-------------------------------------------------------------------------------------------------

bool WMainViewPrivate::isUnderMouse() const
{
    Q_Q(const WMainView);

    int x = mousePos.x();
    int y = mousePos.y();

    if (x >= 0 && y >= 0 && x < q->width() && y < q->height())
    {
         return true;
    }
    else return false;
}

//-------------------------------------------------------------------------------------------------

void WMainViewPrivate::setCursor(WDeclarativeMouseArea::CursorShape shape)
{
    if (cursor != shape)
    {
        applyCursor(shape);
    }
}

void WMainViewPrivate::applyCursor(WDeclarativeMouseArea::CursorShape shape)
{
    Q_Q(WMainView);

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

void WMainViewPrivate::setKeyShiftPressed(bool pressed)
{
    if (keyShiftPressed == pressed) return;

    Q_Q(WMainView);

    keyShiftPressed = pressed;

    emit q->keyShiftPressedChanged();
}

void WMainViewPrivate::setKeyControlPressed(bool pressed)
{
    if (keyControlPressed == pressed) return;

    Q_Q(WMainView);

    keyControlPressed = pressed;

    emit q->keyControlPressedChanged();
}

void WMainViewPrivate::setKeyAltPressed(bool pressed)
{
    if (keyAltPressed == pressed) return;

    Q_Q(WMainView);

    keyAltPressed = pressed;

    emit q->keyAltPressedChanged();
}

//-------------------------------------------------------------------------------------------------

QList<WDeclarativeMouseArea *>
WMainViewPrivate::getMouseAreas(const QList<QGraphicsItem *> & items) const
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
WMainViewPrivate::getDropAreas(const QList<QGraphicsItem *> & items) const
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

void WMainViewPrivate::onGeometryChanged()
{
    Q_Q(WMainView);

    if (maximized || fullScreen)
    {
        setGeometryNormal(q->getDefaultGeometry());
    }
    else q->checkPosition();

    emit q->availableGeometryChanged();
}

//-------------------------------------------------------------------------------------------------

void WMainViewPrivate::onFadeTimeout()
{
    Q_Q(WMainView);

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

void WMainViewPrivate::onIdleTimeout()
{
    Q_Q(WMainView); q->setIdle(true);
}

//-------------------------------------------------------------------------------------------------

void WMainViewPrivate::onCursorVisibleChanged()
{
    Q_Q(WMainView);

    updateHoverable();

    if (sk->cursorVisible() == false)
    {
        cursor = WDeclarativeMouseArea::BlankCursor;

        q->clearHover();
    }
    else q->updateHover();
}

//=================================================================================================
// WMainView
//=================================================================================================

WMainView::WMainView(QDeclarativeItem * item, QWidget * parent, Qt::WindowFlags flags)
    : WAbstractView(new WMainViewPrivate(this), parent, flags)
{
    Q_D(WMainView); d->init(item);
}

WMainView::WMainView(QWidget * parent, Qt::WindowFlags flags)
    : WAbstractView(new WMainViewPrivate(this), parent, flags)
{
    Q_D(WMainView); d->init(NULL);
}

//-------------------------------------------------------------------------------------------------
// Protected

WMainView::WMainView(WMainViewPrivate * p,
                     QDeclarativeItem * item, QWidget * parent, Qt::WindowFlags flags)
    : WAbstractView(p, parent, flags)
{
    Q_D(WMainView); d->init(item);
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WMainView::activate()
{
    setMinimized(false);

    activateWindow();

    raise();

    setFocus();
}

/* Q_INVOKABLE */ void WMainView::raise()
{
    Q_D(WMainView);

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

/* Q_INVOKABLE */ void WMainView::close()
{
    Q_D(WMainView);

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

/* Q_INVOKABLE */ int WMainView::getScreenNumber() const
{
    return wControllerView->screenNumber(this);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WMainView::setMinimumSize(int width, int height)
{
    Q_D(WMainView);

    int minimumWidth  = d->minimumWidth;
    int minimumHeight = d->minimumHeight;

    d->minimumWidth  = width;
    d->minimumHeight = height;

    d->updateMinimumSize();

    if (d->minimumWidth != minimumWidth)
    {
        emit minimumWidthChanged();
    }

    if (d->minimumHeight != minimumHeight)
    {
        emit minimumHeightChanged();
    }
}

/* Q_INVOKABLE */ void WMainView::setMaximumSize(int width, int height)
{
    Q_D(WMainView);

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

/* Q_INVOKABLE */ QRect WMainView::getDefaultGeometry() const
{
    Q_D(const WMainView);

    QRect rect = wControllerView->availableGeometry(sk->defaultScreen());

    return d->getGeometry(rect);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WMainView::setDefaultGeometry()
{
    Q_D(WMainView);

    if (d->maximized || d->fullScreen)
    {
        d->geometryNormal = getDefaultGeometry();
    }
    else setGeometry(getDefaultGeometry());
}

/* Q_INVOKABLE */ void WMainView::saveGeometry()
{
    Q_D(WMainView);

    d->setGeometryNormal(geometry());
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WMainView::checkPosition()
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

/* Q_INVOKABLE */ void WMainView::originTo(qreal x, qreal y)
{
    Q_D(WMainView); d->item->setPos(-x, -y);
}

//-------------------------------------------------------------------------------------------------
// Drag

/* Q_INVOKABLE */ bool WMainView::testDrag(const QPointF & posA,
                                           const QPointF & posB, qreal distance)
{
    if (distance == -1) distance = QApplication::startDragDistance();

    if ((posA - posB).manhattanLength() >= distance)
    {
         return true;
    }
    else return false;
}

/* Q_INVOKABLE */ void WMainView::startDrag(const QString & text, int actions)
{
    Q_D(WMainView);

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

/* Q_INVOKABLE */ int WMainView::hoverCount() const
{
    Q_D(const WMainView); return d->itemsHovered.count();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WMainView::updateHover()
{
    Q_D(WMainView);

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

/* Q_INVOKABLE */ void WMainView::clearHover()
{
    Q_D(WMainView);

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

/* Q_INVOKABLE */ void WMainView::checkLeave(int msec)
{
    Q_D(WMainView); d->timerLeave.start(msec);
}

//-------------------------------------------------------------------------------------------------
// Shot

/* Q_INVOKABLE */ QPixmap WMainView::takeShot(int x, int y, int width, int height) const
{
#ifdef QT_LATEST
    //---------------------------------------------------------------------------------------------
    // FIXME Windows: Workaround for opengl full screen flicker.

    if (width  == -1) { width  = this->width (); }
    if (height == -1) { height = this->height(); }

    //---------------------------------------------------------------------------------------------

    return viewport()->grab(QRect(x, y, width, height));
#else
    Q_D(const WMainView);

    if (d->opengl)
    {
        QGLWidget * viewport = qobject_cast<QGLWidget *> (this->viewport());

        if (width == -1)
        {
            width = this->width();
        }

        if (height == -1)
        {
            height = this->height();
        }

        QImage image = viewport->grabFrameBuffer().copy(x, y, width, height);

        return QPixmap::fromImage(image);
    }
    else return QPixmap::grabWidget(viewport(), x, y, width, height);
#endif
}

/* Q_INVOKABLE */ bool WMainView::saveShot(const QString & fileName, int x,     int y,
                                                                     int width, int height) const
{
    QImage image = takeShot(x, y, width, height).toImage();

    return image.save(fileName, "png");
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ QPixmap WMainView::takeItemShot(QGraphicsObject * item,
                                                  bool              recursive,
                                                  const QColor    & background,
                                                  bool              forceVisible) const
{
    return wControllerView->takeItemShot(item, recursive, background, forceVisible);
}

/* Q_INVOKABLE */ bool WMainView::saveItemShot(const QString   & fileName,
                                               QGraphicsObject * item,
                                               bool              recursive,
                                               const QColor    & background,
                                               bool              forceVisible) const
{
    return wControllerView->saveItemShot(fileName, item, recursive, background, forceVisible);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ bool WMainView::compressShots(const QString & path, int quality)
{
    QDir dir(path);

    if (dir.exists() == false) return false;

    QFileInfoList list = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files);

    foreach (QFileInfo info, list)
    {
        if (info.suffix().toLower() == "png")
        {
            const QString & path = info.filePath();

            qDebug("Compressing: %s", path.C_STR);

            QImage image(path);

            if (image.save(path, "png", quality) == false)
            {
                qWarning("WMainView::compressShots: Failed to save image.");
            }
        }
    }

    return true;
}

//-------------------------------------------------------------------------------------------------
// Cursor

/* Q_INVOKABLE */ void WMainView::registerCursor(WDeclarativeMouseArea::CursorShape shape,
                                                 const QCursor                    & cursor)
{
    Q_D(WMainView);

    d->cursors.insert(shape, cursor);

    if (d->cursor == shape)
    {
        d->applyCursor(shape);
    }
}

/* Q_INVOKABLE */ void WMainView::unregisterCursor(WDeclarativeMouseArea::CursorShape shape)
{
    Q_D(WMainView);

    d->cursors.remove(shape);

    if (d->cursor == shape)
    {
        d->applyCursor(shape);
    }
}

/* Q_INVOKABLE */ void WMainView::unregisterCursors()
{
    Q_D(WMainView);

    d->cursors.clear();

    d->applyCursor(d->cursor);
}

//-------------------------------------------------------------------------------------------------
// Input

/* Q_INVOKABLE */ void WMainView::mouseMove(int x, int y, Qt::MouseButton button) const
{
    QPoint point(x, y);

    QMouseEvent event(QEvent::MouseMove, point, mapToGlobal(point), button, Qt::NoButton,
                                                                            Qt::NoModifier);

    QApplication::sendEvent(viewport(), &event);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WMainView::mousePress(Qt::MouseButton button) const
{
    Q_D(const WMainView);

    QMouseEvent event(QEvent::MouseButtonPress,
                      d->mousePos, mapToGlobal(d->mousePos), button, Qt::NoButton, Qt::NoModifier);

    QApplication::sendEvent(viewport(), &event);
}

/* Q_INVOKABLE */ void WMainView::mouseRelease(Qt::MouseButton button) const
{
    Q_D(const WMainView);

    QMouseEvent event(QEvent::MouseButtonRelease,
                      d->mousePos, mapToGlobal(d->mousePos), button, Qt::NoButton, Qt::NoModifier);

    QApplication::sendEvent(viewport(), &event);
}

/* Q_INVOKABLE */ void WMainView::mouseClick(Qt::MouseButton button, int msec) const
{
    mousePress(button);

    Sk::wait(msec);

    mouseRelease(button);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WMainView::wheel(Qt::Orientation orientation, int delta) const
{
    Q_D(const WMainView);

    QWheelEvent event(d->mousePos, mapToGlobal(d->mousePos), delta, Qt::NoButton,
                                                                    Qt::NoModifier, orientation);

    QApplication::sendEvent(viewport(), &event);
}

/* Q_INVOKABLE */ void WMainView::wheelUp(int delta) const
{
    wheel(Qt::Vertical, delta);
}

/* Q_INVOKABLE */ void WMainView::wheelDown(int delta) const
{
    wheel(Qt::Vertical, delta);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WMainView::keyPress(int key, Qt::KeyboardModifiers modifiers) const
{
    QKeyEvent event(QEvent::KeyPress, key, modifiers);

    QApplication::sendEvent(viewport(), &event);
}

/* Q_INVOKABLE */ void WMainView::keyRelease(int key, Qt::KeyboardModifiers modifiers) const
{
    QKeyEvent event(QEvent::KeyRelease, key, modifiers);

    QApplication::sendEvent(viewport(), &event);
}

/* Q_INVOKABLE */ void WMainView::keyClick(int key, Qt::KeyboardModifiers modifiers,
                                                    int                   msec) const
{
    keyPress(key, modifiers);

    Sk::wait(msec);

    keyRelease(key, modifiers);
}

//-------------------------------------------------------------------------------------------------
// QML

/* Q_INVOKABLE */ void WMainView::mouseMove(int x, int y, int button) const
{
    mouseMove(x, y, static_cast<Qt::MouseButton> (button));
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WMainView::mousePress(int button) const
{
    mousePress(static_cast<Qt::MouseButton> (button));
}

/* Q_INVOKABLE */ void WMainView::mouseRelease(int button) const
{
    mouseRelease(static_cast<Qt::MouseButton> (button));
}

/* Q_INVOKABLE */ void WMainView::mouseClick(int button, int msec) const
{
    mouseClick(static_cast<Qt::MouseButton> (button), msec);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WMainView::wheel(int orientation, int delta) const
{
    wheel(static_cast<Qt::Orientation> (orientation), delta);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WMainView::keyPress(int key, int modifiers) const
{
    keyPress(key, static_cast<Qt::KeyboardModifiers> (modifiers));
}

/* Q_INVOKABLE */ void WMainView::keyRelease(int key, int modifiers) const
{
    keyRelease(key, static_cast<Qt::KeyboardModifiers> (modifiers));
}

/* Q_INVOKABLE */ void WMainView::keyClick(int key, int modifiers, int msec) const
{
    keyClick(key, static_cast<Qt::KeyboardModifiers> (modifiers), msec);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WMainView::registerCursorUrl(int shape, const QUrl  & url,
                                                               const QSize & size)
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

/* Q_INVOKABLE */ void WMainView::unregisterCursor(int shape)
{
    unregisterCursor(static_cast<WDeclarativeMouseArea::CursorShape> (shape));
}

//-------------------------------------------------------------------------------------------------
// Size hints
//-------------------------------------------------------------------------------------------------

QSize WMainView::minimumSizeHint() const
{
    Q_D(const WMainView); return QSize(d->minimumWidth, d->minimumHeight);
}

QSize WMainView::sizeHint() const
{
    return minimumSizeHint();
}

//-------------------------------------------------------------------------------------------------
// Protected functions
//-------------------------------------------------------------------------------------------------

/* virtual */ void WMainView::drawBackground(QPainter *, const QRectF &) {}
/* virtual */ void WMainView::drawForeground(QPainter *, const QRectF &) {}

//-------------------------------------------------------------------------------------------------
// Events
//-------------------------------------------------------------------------------------------------

/* virtual */ void WMainView::showEvent(QShowEvent * event)
{
    Q_D(WMainView);

    if (d->fade && d->fadeVisible == false)
    {
        d->startFade(true);
    }

    WAbstractView::showEvent(event);

    if (d->maximized)
    {
        d->setGeometryNormal(geometry());

        showMaximized();
    }
    else if (d->fullScreen)
    {
        d->setGeometryNormal(geometry());

        showFullScreen();
    }

#if defined(QT_LATEST) == false && defined(Q_OS_WIN)
    activateWindow();

    raise();
#endif
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WMainView::moveEvent(QMoveEvent * event)
{
    Q_D(WMainView);

    d->updateMouse();

    WAbstractView::moveEvent(event);
}

/* virtual */ void WMainView::resizeEvent(QResizeEvent * event)
{
    Q_D(WMainView);

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

/* virtual */ void WMainView::enterEvent(QEvent * event)
{
    Q_D(WMainView);

    d->setEntered(true);

    WAbstractView::enterEvent(event);
}

/* virtual */ void WMainView::leaveEvent(QEvent * event)
{
    Q_D(WMainView);

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

/* virtual */ void WMainView::mousePressEvent(QMouseEvent * event)
{
    Q_D(WMainView);

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

/* virtual */ void WMainView::mouseReleaseEvent(QMouseEvent * event)
{
    Q_D(WMainView);

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

/* virtual */ void WMainView::mouseDoubleClickEvent(QMouseEvent * event)
{
    Q_D(WMainView);

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

/* virtual */ void WMainView::mouseMoveEvent(QMouseEvent * event)
{
    Q_D(WMainView);

    d->setEntered(true);

    if (d->pressed && event->buttons() == Qt::NoButton)
    {
        d->pressed = false;

        emit pressedChanged();
    }

    d->setMousePos(event->pos());

#ifndef QT_LATEST
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

/* virtual */ void WMainView::dragEnterEvent(QDragEnterEvent * event)
{
    Q_D(WMainView);

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

/* virtual */ void WMainView::keyPressEvent(QKeyEvent * event)
{
    Q_D(WMainView);

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

/* virtual */ void WMainView::keyReleaseEvent(QKeyEvent * event)
{
    Q_D(WMainView);

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

/* virtual */ void WMainView::focusInEvent(QFocusEvent * event)
{
    Q_D(WMainView);

    d->setActive(true);

    if (d->minimized != WAbstractView::isMinimized())
    {
        d->minimized = !(d->minimized);

        emit minimizedChanged();
    }

    WAbstractView::focusInEvent(event);
}

/* virtual */ void WMainView::focusOutEvent(QFocusEvent * event)
{
    Q_D(WMainView);

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

/* virtual */ void WMainView::timerEvent(QTimerEvent *)
{
    updateHover();
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WMainView::closeEvent(QCloseEvent * event)
{
    Q_D(WMainView);

    if (d->closed) return;

    event->ignore();

    if (d->item) d->item->setFocus(true);

    close();
}

//-------------------------------------------------------------------------------------------------
// WAbstractView reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WMainView::onStateChanged(Qt::WindowState state)
{
    if (state == Qt::WindowMaximized)
    {
        Q_D(WMainView);

        if (d->maximized == false)
        {
            d->maximized = true;

            emit maximizedChanged();
        }
    }
    else // if (state == Qt::WindowNoState)
    {
        Q_D(WMainView);

        if (d->maximized)
        {
            d->maximized = false;

            emit maximizedChanged();
        }
    }
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

QDeclarativeItem * WMainView::item() const
{
    Q_D(const WMainView); return d->item;
}

void WMainView::setItem(QDeclarativeItem * item)
{
    Q_D(WMainView);

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

qreal WMainView::itemWidth() const
{
    Q_D(const WMainView);

    if (d->item)
    {
         return d->item->width();
    }
    else return 0;
}

qreal WMainView::itemHeight() const
{
    Q_D(const WMainView);

    if (d->item)
    {
         return d->item->height();
    }
    else return 0;
}

//-------------------------------------------------------------------------------------------------

Qt::WindowFlags WMainView::flags() const
{
    Q_D(const WMainView); return d->flags;
}

void WMainView::setFlags(Qt::WindowFlags flags)
{
    Q_D(WMainView);

    if (d->flags == flags) return;

    d->flags = flags;

    d->updateFlags();

    emit flagsChanged();
}

//-------------------------------------------------------------------------------------------------

void WMainView::setX(int x)
{
    if (this->x() == x) return;

    Q_D(WMainView);

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

void WMainView::setY(int y)
{
    if (this->y() == y) return;

    Q_D(WMainView);

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

void WMainView::setWidth(int width)
{
    if (this->width() == width || width < 0) return;

    Q_D(WMainView);

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

void WMainView::setHeight(int height)
{
    if (this->height() == height || height < 0) return;

    Q_D(WMainView);

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

int WMainView::centerX() const
{
    return (availableGeometry().width() - width()) / 2;
}

int WMainView::centerY() const
{
    return (availableGeometry().height() - height()) / 2;
}

//-------------------------------------------------------------------------------------------------

qreal WMainView::originX() const
{
    Q_D(const WMainView);

    if (d->item)
    {
         return -(d->item->x());
    }
    else return 0;
}

void WMainView::setOriginX(qreal x)
{
    Q_D(WMainView);

    if (d->item == NULL || d->item->x() == -x) return;

    d->item->setX(-x);

    emit originXChanged();
}

qreal WMainView::originY() const
{
    Q_D(const WMainView);

    if (d->item)
    {
         return -(d->item->y());
    }
    else return 0;
}

void WMainView::setOriginY(qreal y)
{
    Q_D(WMainView);

    if (d->item == NULL || d->item->y() == -y) return;

    d->item->setY(-y);

    emit originYChanged();
}

//-------------------------------------------------------------------------------------------------

qreal WMainView::zoom() const
{
    Q_D(const WMainView); return d->zoom;
}

void WMainView::setZoom(qreal zoom)
{
    Q_D(WMainView);

    d->zoom = zoom;

    if (d->item)
    {
        d->applySize(width(), height());
    }

    emit zoomChanged();
}

//-------------------------------------------------------------------------------------------------

int WMainView::minimumWidth() const
{
    Q_D(const WMainView); return d->minimumWidth;
}

void WMainView::setMinimumWidth(int width)
{
    Q_D(WMainView);

    if (d->minimumWidth == width) return;

    d->minimumWidth = width;

    d->updateMinimumSize();

    minimumWidthChanged();
}

int WMainView::minimumHeight() const
{
    Q_D(const WMainView); return d->minimumHeight;
}

void WMainView::setMinimumHeight(int height)
{
    Q_D(WMainView);

    if (d->minimumHeight == height) return;

    d->minimumHeight = height;

    d->updateMinimumSize();

    minimumHeightChanged();
}

//-------------------------------------------------------------------------------------------------

int WMainView::maximumWidth() const
{
    Q_D(const WMainView); return d->maximumWidth;
}

void WMainView::setMaximumWidth(int width)
{
    Q_D(WMainView);

    if (d->maximumWidth == width) return;

    d->maximumWidth = width;

    if (width == -1)
    {
         WAbstractView::setMaximumSize(QWIDGETSIZE_MAX, WAbstractView::maximumHeight());
    }
    else WAbstractView::setMaximumSize(width, WAbstractView::maximumHeight());

    maximumWidthChanged();
}

int WMainView::maximumHeight() const
{
    Q_D(const WMainView); return d->maximumHeight;
}

void WMainView::setMaximumHeight(int height)
{
    Q_D(WMainView);

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

QRect WMainView::geometryNormal() const
{
    Q_D(const WMainView); return d->geometryNormal;
}

//-------------------------------------------------------------------------------------------------

bool WMainView::isMinimized() const
{
    Q_D(const WMainView); return d->minimized;
}

void WMainView::setMinimized(bool minimized)
{
    Q_D(WMainView);

    if (d->minimized == minimized) return;

    d->minimized = minimized;

    if (minimized) showMinimized();
    else           showNormal   ();

    emit minimizedChanged();
}

//-------------------------------------------------------------------------------------------------

bool WMainView::isMaximized() const
{
    Q_D(const WMainView); return d->maximized;
}

void WMainView::setMaximized(bool maximized)
{
    Q_D(WMainView);

    if (d->maximized == maximized) return;

    if (maximized)
    {
#ifdef Q_OS_LINUX
        // FIXME: Workaround to undock the window.
        int geometryHeight = availableGeometry().height();

        if (height() == geometryHeight)
        {
            sk->setDefaultHeight(geometryHeight - geometryHeight / 8);

            showMaximized();
            showNormal   ();

            setGeometry(getDefaultGeometry());

            return;
        }
#endif
        if (d->autoSize)
        {
            d->setGeometryNormal(geometry());
        }

        d->maximized = true;

        showMaximized();

        emit maximizedChanged();
    }
    else d->showNormal();
}

//-------------------------------------------------------------------------------------------------

bool WMainView::isFullScreen() const
{
    Q_D(const WMainView); return d->fullScreen;
}

void WMainView::setFullScreen(bool fullScreen)
{
    Q_D(WMainView);

    if (d->fullScreen == fullScreen) return;

    if (fullScreen)
    {
        if (d->autoSize && d->maximized == false)
        {
            d->setGeometryNormal(geometry());
        }

        d->fullScreen = true;

        showFullScreen();

        emit fullScreenChanged();
    }
    else d->showNormal();
}

//-------------------------------------------------------------------------------------------------

bool WMainView::isLocked() const
{
    Q_D(const WMainView); return d->locked;
}

void WMainView::setLocked(bool locked)
{
    Q_D(WMainView);

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

bool WMainView::isActive() const
{
    Q_D(const WMainView); return d->active;
}

bool WMainView::isEntered() const
{
    Q_D(const WMainView); return d->entered;
}

bool WMainView::isPressed() const
{
    Q_D(const WMainView); return d->pressed;
}

bool WMainView::isDragging() const
{
    Q_D(const WMainView); return d->dragging;
}

bool WMainView::isDragged() const
{
    Q_D(const WMainView); return d->dragged;
}

bool WMainView::isResizing() const
{
    Q_D(const WMainView); return d->resizing;
}

//-------------------------------------------------------------------------------------------------

QPoint WMainView::mousePos() const
{
    Q_D(const WMainView); return d->mousePos;
}

//-------------------------------------------------------------------------------------------------

int WMainView::mouseX() const
{
    Q_D(const WMainView); return d->mousePos.x();
}

int WMainView::mouseY() const
{
    Q_D(const WMainView); return d->mousePos.y();
}

//-------------------------------------------------------------------------------------------------

WDeclarativeMouseArea::CursorShape WMainView::mouseCursor() const
{
    Q_D(const WMainView); return d->cursor;
}

//-------------------------------------------------------------------------------------------------

bool WMainView::autoSize() const
{
    Q_D(const WMainView); return d->autoSize;
}

void WMainView::setAutoSize(bool autoSize)
{
    Q_D(WMainView);

    if (d->autoSize == autoSize) return;

    d->autoSize = autoSize;

    emit autoSizeChanged();
}

//-------------------------------------------------------------------------------------------------

bool WMainView::opengl() const
{
    Q_D(const WMainView); return d->opengl;
}

void WMainView::setOpengl(bool enabled)
{
    Q_D(WMainView);

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

bool WMainView::antialias() const
{
    Q_D(const WMainView); return d->antialias;
}

void WMainView::setAntialias(bool enabled)
{
    Q_D(WMainView);

    if (d->antialias == enabled) return;

    d->antialias = enabled;

    setRenderHint(QPainter::Antialiasing, enabled);

    emit antialiasChanged();
}

//-------------------------------------------------------------------------------------------------

bool WMainView::vsync() const
{
    Q_D(const WMainView); return d->vsync;
}

void WMainView::setVsync(bool enabled)
{
    Q_D(WMainView);

    if (d->vsync == enabled) return;

    d->vsync = enabled;

    if (d->opengl) d->updateViewport();

    emit vsyncChanged();
}

//-------------------------------------------------------------------------------------------------

bool WMainView::hoverEnabled() const
{
    Q_D(const WMainView); return d->hover;
}

void WMainView::setHoverEnabled(bool enabled)
{
    Q_D(WMainView);

    if (d->hover == enabled) return;

    d->hover = enabled;

    d->updateHoverable();

    emit hoverEnabledChanged();
}

//-------------------------------------------------------------------------------------------------

bool WMainView::fadeEnabled() const
{
    Q_D(const WMainView); return d->fade;
}

void WMainView::setFadeEnabled(bool enabled)
{
    Q_D(WMainView);

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

int WMainView::fadeDuration() const
{
    Q_D(const WMainView); return d->fadeDuration;
}

void WMainView::setFadeDuration(int msec)
{
    Q_D(WMainView);

    if (d->fadeDuration == msec) return;

    d->fadeDuration = msec;

    d->fadeValue = 16.0 / d->fadeDuration;

    emit fadeDurationChanged();
}

//-------------------------------------------------------------------------------------------------
// Mouse idle

bool WMainView::idleCheck() const
{
    Q_D(const WMainView); return d->idleCheck;
}

void WMainView::setIdleCheck(bool check)
{
    Q_D(WMainView);

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

bool WMainView::idle() const
{
    Q_D(const WMainView); return d->idle;
}

void WMainView::setIdle(bool idle)
{
    Q_D(WMainView);

    if (d->idle == idle) return;

    d->idle = idle;

    d->idleTimer.stop();

    emit idleChanged();
}

//-------------------------------------------------------------------------------------------------

int WMainView::idleDelay() const
{
    Q_D(const WMainView); return d->idleDelay;
}

void WMainView::setIdleDelay(int msec)
{
    Q_D(WMainView);

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

bool WMainView::keyShiftPressed() const
{
    Q_D(const WMainView); return d->keyShiftPressed;
}

bool WMainView::keyControlPressed() const
{
    Q_D(const WMainView); return d->keyControlPressed;
}

bool WMainView::keyAltPressed() const
{
    Q_D(const WMainView); return d->keyAltPressed;
}

//-------------------------------------------------------------------------------------------------
// Screen

QRect WMainView::availableGeometry() const
{
    return wControllerView->availableGeometry(this);
}

QRect WMainView::screenGeometry() const
{
    return wControllerView->screenGeometry(this);
}

#endif // SK_NO_MAINVIEW
