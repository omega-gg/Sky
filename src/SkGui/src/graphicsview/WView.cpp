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

#include "WView.h"

#ifndef SK_NO_VIEW

// Qt includes
#include <QApplication>
#ifdef QT_LATEST
#include <QScreen>
#endif
#include <QDesktopWidget>
#include <QGLWidget>
#include <QImageReader>
#include <QDrag>
#include <QMimeData>

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

#ifdef QT_4

//=================================================================================================
// WViewScene
//=================================================================================================

WViewScene::WViewScene(WView * parent) : QGraphicsScene(parent)
{
    view = parent;
}

#endif

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

#ifdef QT_4
void WViewPrivate::init(QDeclarativeItem * item)
#else
void WViewPrivate::init(QQuickItem * item)
#endif
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

    locked  = false;
    scaling = true;
    closed  = false;

    opengl    = wControllerView->opengl();
    antialias = false;
    vsync     = false;

#ifdef QT_4
    //timerLeave.setSingleShot(true);
#endif

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
    dragAccepted  = false;

    resetHover = false;

    button  = Qt::NoButton;
    buttons = Qt::NoButton;

    cursor = Qt::ArrowCursor;

    idleCheck = false;
    idle      = false;
    idleDelay = 3000;

    idleTimer.setInterval(3000);

#ifdef QT_LATEST
    //---------------------------------------------------------------------------------------------
    // Touch

    idTouch = -1;
#endif

    //---------------------------------------------------------------------------------------------
    // Drag

    drag = NULL;
    mime = NULL;

    dragData.actions = Qt::IgnoreAction;
    dragData.action  = Qt::IgnoreAction;

    areaDrop = NULL;

#ifdef QT_LATEST
    dragSkip = false;
#endif

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

#ifdef QT_4
    scene = new WViewScene(q);
#endif

    if (item)
    {
#ifdef QT_4
        scene->addItem(item);
#else
        item->setParentItem(q->contentItem());
#endif

        QObject::connect(item, SIGNAL(widthChanged ()), q, SIGNAL(itemWidthChanged ()));
        QObject::connect(item, SIGNAL(heightChanged()), q, SIGNAL(itemHeightChanged()));

        QObject::connect(item, SIGNAL(xChanged()), q, SIGNAL(originXChanged()));
        QObject::connect(item, SIGNAL(yChanged()), q, SIGNAL(originYChanged()));
    }

#ifdef QT_4
    if (opengl)
    {
        QGLFormat format = QGLFormat::defaultFormat();

        format.setSampleBuffers(true);

        q->setViewport(new QGLWidget(format));

        q->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    }
    else
    {
        q->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);

#ifdef Q_OS_WIN
        q->setWindowClip(true);
#endif
    }
#else
    QSurfaceFormat format;

    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);

    format.setDepthBufferSize(24);

    format.setSamples     (0);
    format.setSwapInterval(0);

    q->setFormat(format);

    //q->setClearBeforeRendering(false);
#endif

#ifdef QT_4
    q->setScene(scene);

#ifdef Q_OS_WIN
    // FIXME Qt4 Windows: Workaround for opengl full screen flicker.
    q->setViewportMargins(0, 0, -1, 0);
#endif
#endif

#ifdef QT_4
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
#endif

    //---------------------------------------------------------------------------------------------
    // Default size

#ifdef QT_4
    q->WAbstractView::setMinimumSize(minimumWidth, minimumHeight);
#else
    q->WAbstractView::setMinimumSize(QSize(minimumWidth, minimumHeight));
#endif

    QRect rect = wControllerView->availableGeometry(sk->defaultScreen());

    geometryNormal = getGeometryDefault(rect);

    q->setGeometry(geometryNormal);

    //---------------------------------------------------------------------------------------------
    // Timer

    q->startTimer(16);

    //---------------------------------------------------------------------------------------------
    // Registering view

    wControllerView->d_func()->registerView(q);

    //---------------------------------------------------------------------------------------------
    // Signals

    if (sk->isSingle())
    {
        QObject::connect(qApp, SIGNAL(messageReceived(const QString &)),
                         q,    SIGNAL(messageReceived(const QString &)));
    }

    QObject::connect(qApp->desktop(), SIGNAL(workAreaResized(int)), q, SLOT(onGeometryChanged()));

    QObject::connect(q, SIGNAL(availableGeometryChanged()), q, SIGNAL(centerXChanged()));
    QObject::connect(q, SIGNAL(availableGeometryChanged()), q, SIGNAL(centerYChanged()));

    QObject::connect(q, SIGNAL(widthChanged ()), q, SIGNAL(centerXChanged()));
    QObject::connect(q, SIGNAL(heightChanged()), q, SIGNAL(centerYChanged()));

#ifdef QT_4
    //QObject::connect(&timerLeave, SIGNAL(timeout()), q, SLOT(onLeaveTimeout()));
#endif

    QObject::connect(&fadeTimer, SIGNAL(timeout()), q, SLOT(onFadeTimeout()));
    QObject::connect(&idleTimer, SIGNAL(timeout()), q, SLOT(onIdleTimeout()));

#ifndef SK_WIN_NATIVE
    QObject::connect(sk, SIGNAL(aboutToQuit()), q, SLOT(onClose()));
#endif

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
#ifdef QT_4
        q->setWindowOpacity(fadeValue);
#else
        q->setOpacity(fadeValue);
#endif
    }
#ifdef QT_4
    else q->setWindowOpacity(1.0 - fadeValue);
#else
    else q->setOpacity(1.0 - fadeValue);
#endif

    fadeTimer.start(16);
}

//-------------------------------------------------------------------------------------------------

void WViewPrivate::fadeIn()
{
    Q_Q(WView);

    fadeTimer.stop();

#ifdef QT_4
    q->setWindowOpacity(1.0);
#else
    q->setOpacity(1.0);
#endif

    emit q->fadeIn();
}

void WViewPrivate::fadeOut()
{
    Q_Q(WView);

    fadeTimer.stop();

#ifdef QT_4
    q->setWindowOpacity(0.0);
#else
    q->setOpacity(0.0);
#endif

    emit q->fadeOut();

    q->WAbstractView::close();
}

//-------------------------------------------------------------------------------------------------

void WViewPrivate::applySize(int width, int height)
{
    item->setSize(QSizeF(width * zoom, height * zoom));
}

//-------------------------------------------------------------------------------------------------

void WViewPrivate::applyDrop()
{
    QPointF pos = areaDrop->mapFromScene(mousePos);

    areaDrop->d_func()->dropEvent(pos, dragData);

    areaDrop = NULL;
}

//-------------------------------------------------------------------------------------------------

void WViewPrivate::updateFlags()
{
    Q_Q(WView);

#ifdef Q_OS_WIN
    if (locked)
    {
         SetWindowPos((HWND) q->winId(),
                      HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    }
    else SetWindowPos((HWND) q->winId(),
                      HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
#elif defined(QT_4)
    if (locked)
    {
         q->setWindowFlags(flags | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
    }
    else q->setWindowFlags(flags);
#else
    if (locked)
    {
         q->QQuickWindow::setFlags(flags | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
    }
    else q->QQuickWindow::setFlags(flags);
#endif
}

//-------------------------------------------------------------------------------------------------

#ifdef QT_4

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

#endif

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
    if (dragging == false) return;

    Q_Q(WView);

    QPoint pos = QCursor::pos();

#ifdef Q_OS_LINUX
    setMousePos(q->mapFromGlobal(pos));
#else
    // FIXME Windows: Mapping the position from global.
    setMousePos(q->mapFromGlobal(pos));
#endif

    if (isUnderMouse())
    {
#ifdef QT_4
        QList<QGraphicsItem *> items = q->items(mousePos);
#else
        QList<QQuickItem *> items;

        getItems(&items, q->contentItem(), mousePos);
#endif

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

            if (areaPrivate->dragAccepted)
            {
                areaPrivate->dragMoveEvent(posItem, dragData);

                return;
            }
            else
            {
                if (areaDrop)
                {
#ifdef QT_LATEST
                    // FIXME Qt5: Sometimes the drop area goes beyond the parent geometry.
                    QPointF posItem = areaDrop->mapFromScene(mousePos);

                    if (areaDrop->boundingRect().contains(posItem))
                    {
                        areaDrop->d_func()->dragMoveEvent(posItem, dragData);

                        return;
                    }
#endif

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
        }

        if (areaDrop)
        {
            areaDrop->d_func()->dragLeaveEvent();

            areaDrop = NULL;
        }
    }
    else if (drag == NULL && dragAccepted == false)
    {
        clearDrag();

        drag = new QDrag(q);

        QMimeData * mime = new QMimeData;

        mime->setText(dragData.text);

        drag->setMimeData(mime);

        drag->exec(dragData.actions);

#ifdef Q_OS_WIN
        if (drag == NULL) return;
#endif

        drag = NULL;

        setDragging(false);
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
    int width  = rect.width () / 8;
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

#ifdef QT_LATEST

bool WViewPrivate::itemUnderMouse(QQuickItem * item) const
{
    return item->contains(item->mapFromScene(mousePos));
}

#endif

//-------------------------------------------------------------------------------------------------

void WViewPrivate::setCursor(Qt::CursorShape shape)
{
    if (cursor == shape) return;

    applyCursor(shape);
}

void WViewPrivate::applyCursor(Qt::CursorShape shape)
{
    Q_Q(WView);

    cursor = shape;

    if (cursors.contains(shape))
    {
        const QCursor & cursor = cursors.value(shape);

#ifdef QT_4
        if (QApplication::overrideCursor())
        {
             QApplication::changeOverrideCursor(cursor);
        }
        else QApplication::setOverrideCursor(cursor);
    }
    else if (QApplication::overrideCursor())
    {
        QApplication::changeOverrideCursor(shape);
    }
    else if (shape) QApplication::setOverrideCursor(shape);
#else
        if (QGuiApplication::overrideCursor())
        {
             QGuiApplication::changeOverrideCursor(cursor);
        }
        else QGuiApplication::setOverrideCursor(cursor);
    }
    else if (QGuiApplication::overrideCursor())
    {
        QGuiApplication::changeOverrideCursor(shape);
    }
    else if (shape) QGuiApplication::setOverrideCursor(shape);
#endif

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

#ifdef QT_LATEST

void WViewPrivate::getItems(QList<QQuickItem *> * items,
                            QQuickItem          * item, const QPoint & pos) const
{
    if (item->isVisible() == false) return;

    QPoint position = item->mapFromScene(pos).toPoint();

    if (item->boundingRect().toRect().contains(position) == false) return;

    QList<QQuickItem *> childsA = item->childItems();
    QList<QQuickItem *> childsB;

    foreach (QQuickItem * child, childsA)
    {
        int index = 0;

        while (index < childsB.count() && childsB.at(index)->z() <= child->z())
        {
            index++;
        }

        childsB.insert(index, child);
    }

    for (int i = childsB.count() - 1; i > -1; i--)
    {
        getItems(items, childsB.at(i), pos);
    }

    items->append(item);
}

#endif

//-------------------------------------------------------------------------------------------------

#ifdef QT_4
QList<WDeclarativeMouseArea *>
WViewPrivate::getMouseAreas(const QList<QGraphicsItem *> & items) const
#else
QList<WDeclarativeMouseArea *> WViewPrivate::getMouseAreas(const QList<QQuickItem *> & items) const
#endif
{
    QList<WDeclarativeMouseArea *> mouseAreas;

#ifdef QT_4
    foreach (QGraphicsItem * item, items)
    {
        QGraphicsObject * object = item->toGraphicsObject();

        if (object == NULL) continue;

        WDeclarativeMouseArea * area = qobject_cast<WDeclarativeMouseArea *> (object);
#else
    foreach (QQuickItem * item, items)
    {
        WDeclarativeMouseArea * area = qobject_cast<WDeclarativeMouseArea *> (item);
#endif

        if (area == NULL) continue;

        if (area->d_func()->hoverEnabled)
        {
            mouseAreas.append(area);
        }

        if (area->d_func()->hoverRetain)
        {
            return mouseAreas;
        }
    }

    return mouseAreas;
}

#ifdef QT_4
QList<WDeclarativeMouseArea *>
WViewPrivate::getDropAreas(const QList<QGraphicsItem *> & items) const
#else
QList<WDeclarativeMouseArea *> WViewPrivate::getDropAreas(const QList<QQuickItem *> & items) const
#endif
{
    QList<WDeclarativeMouseArea *> dropAreas;

#ifdef QT_4
    foreach (QGraphicsItem * item, items)
    {
        QGraphicsObject * object = item->toGraphicsObject();

        if (object == NULL) continue;

        WDeclarativeMouseArea * area = qobject_cast<WDeclarativeMouseArea *> (object);
#else
    foreach (QQuickItem * item, items)
    {
        WDeclarativeMouseArea * area = qobject_cast<WDeclarativeMouseArea *> (item);
#endif

#ifdef QT_4
        if (area && area->acceptDrops())
#else
        if (area && area->d_func()->dropEnabled)
#endif
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

#ifdef QT_4

/*void WViewPrivate::onLeaveTimeout()
{
    Q_Q(WView);

    q->clearHover ();
    q->updateHover();
}*/

#endif

//-------------------------------------------------------------------------------------------------

void WViewPrivate::onFadeTimeout()
{
    Q_Q(WView);

    if (fadeVisible)
    {
#ifdef QT_4
        qreal opacity = q->windowOpacity() + fadeValue;
#else
        qreal opacity = q->opacity() + fadeValue;
#endif

        if (opacity >= 1.0)
        {
            fadeIn();
        }
#ifdef QT_4
        else q->setWindowOpacity(opacity);
#else
        else q->setOpacity(opacity);
#endif
    }
    else
    {
#ifdef QT_4
        qreal opacity = q->windowOpacity() - fadeValue;
#else
        qreal opacity = q->opacity() - fadeValue;
#endif

        if (opacity <= 0.0)
        {
            fadeOut();
        }
#ifdef QT_4
        else q->setWindowOpacity(opacity);
#else
        else q->setOpacity(opacity);
#endif
    }
}

void WViewPrivate::onIdleTimeout()
{
    Q_Q(WView); q->setIdle(true);
}

//-------------------------------------------------------------------------------------------------

void WViewPrivate::onClose()
{
    if (closed) return;

    Q_Q(WView);

    closed = true;

    emit q->beforeClose();

    if (fade) emit q->fadeOut();
}

//-------------------------------------------------------------------------------------------------

void WViewPrivate::onCursorVisibleChanged()
{
    Q_Q(WView);

    updateHoverable();

    if (sk->cursorVisible() == false)
    {
        cursor = Qt::BlankCursor;

        q->clearHover();
    }
    else q->updateHover();
}

//=================================================================================================
// WView
//=================================================================================================

#ifdef QT_4
WView::WView(QDeclarativeItem * item, QWidget * parent, Qt::WindowFlags flags)
#else
WView::WView(QQuickItem * item, QWindow * parent, Qt::WindowFlags flags)
#endif
    : WAbstractView(new WViewPrivate(this), parent, flags)
{
    Q_D(WView); d->init(item);
}

#ifdef QT_4
WView::WView(QWidget * parent, Qt::WindowFlags flags)
#else
WView::WView(QWindow * parent, Qt::WindowFlags flags)
#endif
    : WAbstractView(new WViewPrivate(this), parent, flags)
{
    Q_D(WView); d->init(NULL);
}

//-------------------------------------------------------------------------------------------------
// Protected

#ifdef QT_4
WView::WView(WViewPrivate * p, QDeclarativeItem * item, QWidget * parent, Qt::WindowFlags flags)
#else
WView::WView(WViewPrivate * p, QQuickItem * item, QWindow * parent, Qt::WindowFlags flags)
#endif
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

#ifdef QT_4
    activateWindow();
#else
    requestActivate();
#endif

    raise();

#ifdef QT_4
    setFocus();
#endif
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

/* Q_INVOKABLE */ bool WView::close()
{
    Q_D(WView);

    if (d->closed) return true;

    d->closed = true;

    emit beforeClose();

    if (d->fade)
    {
        if (d->fadeVisible)
        {
            d->startFade(false);
        }

        return true;
    }
    else return WAbstractView::close();
}

//-------------------------------------------------------------------------------------------------
// Geometry

/* Q_INVOKABLE */ int WView::getScreenNumber() const
{
#ifdef QT_4
    return wControllerView->screenNumber(this);
#else
    return wControllerView->screenNumber(QPoint(x() + width() / 2, y() + height() / 2));
#endif
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WView::setMinimumSize(int width, int height)
{
    Q_D(WView);

    int minimumWidth  = d->minimumWidth;
    int minimumHeight = d->minimumHeight;

    d->minimumWidth  = width;
    d->minimumHeight = height;

#ifdef QT_4
    WAbstractView::setMinimumSize(width, height);
#else
    WAbstractView::setMinimumSize(QSize(width, height));
#endif

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
#ifdef QT_4
             WAbstractView::setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
        }
        else WAbstractView::setMaximumSize(QWIDGETSIZE_MAX, height);
#else
            WAbstractView::setMaximumSize(QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX));
       }
       else WAbstractView::setMaximumSize(QSize(QWIDGETSIZE_MAX, height));
#endif
    }
    else if (height == -1)
    {
        if (width == -1)
        {
#ifdef QT_4
             WAbstractView::setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
        }
        else WAbstractView::setMaximumSize(width, QWIDGETSIZE_MAX);
#else
            WAbstractView::setMaximumSize(QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX));
       }
       else WAbstractView::setMaximumSize(QSize(width, QWIDGETSIZE_MAX));
#endif
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

#ifdef QT_4
    if (update) move(x, y);
#else
    if (update) setPosition(x, y);
#endif
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WView::originTo(qreal x, qreal y)
{
#ifdef QT_4
    Q_D(WView); d->item->setPos(-x, -y);
#else
    Q_D(WView); d->item->setPosition(QPointF(-x, -y));
#endif
}

//-------------------------------------------------------------------------------------------------
// Drag

/* Q_INVOKABLE */ bool WView::testDrag(const QPointF & posA, const QPointF & posB, qreal distance)
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

#ifdef QT_4
    d->scene->mouseGrabberItem()->ungrabMouse();
#else
    mouseGrabberItem()->ungrabMouse();
#endif

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
#ifdef QT_4
        QList<QGraphicsItem *> itemsCursor = items(d->mousePos);
#else
        QList<QQuickItem *> itemsCursor;

        d->getItems(&itemsCursor, contentItem(), d->mousePos);
#endif

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
            d->setCursor(Qt::ArrowCursor);

            return;
        }

        WDeclarativeMouseArea * area = areas.takeFirst();

        Qt::CursorShape cursor = area->d_func()->cursor;

        while (cursor == Qt::BlankCursor)
        {
            if (areas.isEmpty())
            {
                d->setCursor(Qt::ArrowCursor);

                return;
            }

            area = areas.takeFirst();

            cursor = area->d_func()->cursor;
        }

        d->setCursor(cursor);
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
        d->setCursor(Qt::ArrowCursor);
    }
}

//-------------------------------------------------------------------------------------------------

#ifdef QT_4

///* Q_INVOKABLE */ void WView::checkLeave(int msec)
//{
//    Q_D(WView); d->timerLeave.start(msec);
//}

#endif

//-------------------------------------------------------------------------------------------------
// Shot

#ifdef QT_4
/* Q_INVOKABLE */ QPixmap WView::takeShot(int x, int y, int width, int height) const
#else
/* Q_INVOKABLE */ QPixmap WView::takeShot(int x, int y, int width, int height)
#endif
{
#ifdef QT_4
    Q_D(const WView);

    if (d->opengl)
    {
        if (width  == -1) width  = this->width ();
        if (height == -1) height = this->height();

        QGLWidget * viewport = qobject_cast<QGLWidget *> (this->viewport());

#ifdef Q_OS_WIN
        // FIXME Windows: Making sure we grab the front buffer.
        glReadBuffer(GL_FRONT);
#endif

        QImage image = viewport->grabFrameBuffer().copy(x, y, width, height);

        return QPixmap::fromImage(image);
    }
    else return QPixmap::grabWidget(viewport(), x, y, width, height);
#else
#ifdef Q_OS_WIN
    //---------------------------------------------------------------------------------------------
    // FIXME Windows: Workaround for opengl full screen flicker.

    if (width  == -1) width  = this->width ();
    if (height == -1) height = this->height();

    //---------------------------------------------------------------------------------------------
#endif

    QImage image = grabWindow().copy(x, y, width, height);

    return QPixmap::fromImage(image);
#endif
}

#ifdef QT_4
/* Q_INVOKABLE */ bool WView::saveShot(const QString & fileName, int x,     int y,
                                                                 int width, int height) const
#else
/* Q_INVOKABLE */ bool WView::saveShot(const QString & fileName, int x,     int y,
                                                                 int width, int height)
#endif
{
    QImage image = takeShot(x, y, width, height).toImage();

    return image.save(fileName, "png");
}

//-------------------------------------------------------------------------------------------------
// Cursor

/* Q_INVOKABLE */ void WView::registerCursor(Qt::CursorShape shape, const QCursor & cursor)
{
    Q_D(WView);

    d->cursors.insert(shape, cursor);

    if (d->cursor == shape)
    {
        d->applyCursor(shape);
    }
}

/* Q_INVOKABLE */ void WView::unregisterCursor(Qt::CursorShape shape)
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

/* Q_INVOKABLE */ void WView::mouseMove(int x, int y, Qt::MouseButton button)
{
    QPoint point(x, y);

    QMouseEvent event(QEvent::MouseMove, point, mapToGlobal(point), button, Qt::NoButton,
                                                                            Qt::NoModifier);

#ifdef QT_4
    QCoreApplication::sendEvent(viewport(), &event);
#else
    QCoreApplication::sendEvent(this, &event);
#endif
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WView::mousePress(Qt::MouseButton button)
{
    Q_D(WView);

    QMouseEvent event(QEvent::MouseButtonPress,
                      d->mousePos, mapToGlobal(d->mousePos), button, Qt::NoButton, Qt::NoModifier);

#ifdef QT_4
    QCoreApplication::sendEvent(viewport(), &event);
#else
    QCoreApplication::sendEvent(this, &event);
#endif
}

/* Q_INVOKABLE */ void WView::mouseRelease(Qt::MouseButton button)
{
    Q_D(WView);

    QMouseEvent event(QEvent::MouseButtonRelease,
                      d->mousePos, mapToGlobal(d->mousePos), button, Qt::NoButton, Qt::NoModifier);

#ifdef QT_4
    QCoreApplication::sendEvent(viewport(), &event);
#else
    QCoreApplication::sendEvent(this, &event);
#endif
}

/* Q_INVOKABLE */ void WView::mouseClick(Qt::MouseButton button, int msec)
{
    mousePress(button);

    Sk::wait(msec);

    mouseRelease(button);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WView::wheel(Qt::Orientation orientation, int delta)
{
    Q_D(WView);

    QWheelEvent event(d->mousePos, mapToGlobal(d->mousePos), delta, Qt::NoButton,
                                                                    Qt::NoModifier, orientation);

#ifdef QT_4
    QCoreApplication::sendEvent(viewport(), &event);
#else
    QCoreApplication::sendEvent(this, &event);
#endif
}

/* Q_INVOKABLE */ void WView::wheelUp(int delta)
{
    wheel(Qt::Vertical, delta);
}

/* Q_INVOKABLE */ void WView::wheelDown(int delta)
{
    wheel(Qt::Vertical, delta);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WView::keyPress(int key, Qt::KeyboardModifiers modifiers)
{
    QKeyEvent event(QEvent::KeyPress, key, modifiers);

#ifdef QT_4
    QCoreApplication::sendEvent(viewport(), &event);
#else
    QCoreApplication::sendEvent(this, &event);
#endif
}

/* Q_INVOKABLE */ void WView::keyRelease(int key, Qt::KeyboardModifiers modifiers)
{
    QKeyEvent event(QEvent::KeyRelease, key, modifiers);

#ifdef QT_4
    QCoreApplication::sendEvent(viewport(), &event);
#else
    QCoreApplication::sendEvent(this, &event);
#endif
}

/* Q_INVOKABLE */ void WView::keyClick(int key, Qt::KeyboardModifiers modifiers, int msec)
{
    keyPress(key, modifiers);

    Sk::wait(msec);

    keyRelease(key, modifiers);
}

//-------------------------------------------------------------------------------------------------
// QML

/* Q_INVOKABLE */ void WView::mouseMove(int x, int y, int button)
{
    mouseMove(x, y, static_cast<Qt::MouseButton> (button));
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WView::mousePress(int button)
{
    mousePress(static_cast<Qt::MouseButton> (button));
}

/* Q_INVOKABLE */ void WView::mouseRelease(int button)
{
    mouseRelease(static_cast<Qt::MouseButton> (button));
}

/* Q_INVOKABLE */ void WView::mouseClick(int button, int msec)
{
    mouseClick(static_cast<Qt::MouseButton> (button), msec);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WView::wheel(int orientation, int delta)
{
    wheel(static_cast<Qt::Orientation> (orientation), delta);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WView::keyPress(int key, int modifiers)
{
    keyPress(key, static_cast<Qt::KeyboardModifiers> (modifiers));
}

/* Q_INVOKABLE */ void WView::keyRelease(int key, int modifiers)
{
    keyRelease(key, static_cast<Qt::KeyboardModifiers> (modifiers));
}

/* Q_INVOKABLE */ void WView::keyClick(int key, int modifiers, int msec)
{
    keyClick(key, static_cast<Qt::KeyboardModifiers> (modifiers), msec);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WView::registerCursorUrl(int shape, const QUrl & url, const QSize & size)
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

    registerCursor(static_cast<Qt::CursorShape> (shape), cursor);
}

/* Q_INVOKABLE */ void WView::unregisterCursor(int shape)
{
    unregisterCursor(static_cast<Qt::CursorShape> (shape));
}

//-------------------------------------------------------------------------------------------------
// Static functions
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
/* Q_INVOKABLE static */ QPixmap WView::takeItemShot(QGraphicsObject * item,
                                                     const QColor    & background)
#else
/* Q_INVOKABLE static */ QPixmap WView::takeItemShot(QQuickItem * item, const QColor & background)
#endif
{
    return WControllerView::takeItemShot(item, background);
}

#ifdef QT_4
/* Q_INVOKABLE static */ bool WView::saveItemShot(const QString   & fileName,
                                                  QGraphicsObject * item,
                                                  const QColor    & background)
#else
/* Q_INVOKABLE static */ bool WView::saveItemShot(const QString & fileName,
                                                  QQuickItem    * item,
                                                  const QColor  & background)
#endif
{
    return WControllerView::saveItemShot(fileName, item, background);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ bool WView::compressShot(const QString & fileName, int quality)
{
    return WControllerView::compressShot(fileName, quality);
}

/* Q_INVOKABLE static */ bool WView::compressShots(const QString & path, int quality)
{
    return WControllerView::compressShots(path, quality);
}

//-------------------------------------------------------------------------------------------------
// Protected functions
//-------------------------------------------------------------------------------------------------

void WView::hoverEnter()
{
    Q_D(WView);

    d->setEntered(true);

#ifdef QT_LATEST
    // FIXME Qt5: Resetting the cursor by hand.
    if (d->cursors.contains(d->cursor))
    {
        const QCursor & cursor = d->cursors.value(d->cursor);

        if (QGuiApplication::overrideCursor())
        {
             QGuiApplication::changeOverrideCursor(cursor);
        }
        else QGuiApplication::setOverrideCursor(cursor);
    }
    else if (QGuiApplication::overrideCursor())
    {
        QGuiApplication::changeOverrideCursor(d->cursor);
    }
    else if (d->cursor) QGuiApplication::setOverrideCursor(d->cursor);
#endif

    if (d->resetHover == false) return;

    d->resetHover = false;

    if (d->currentResizer)
    {
        d->currentResizer = NULL;

        d->setResizing(false);
    }

    clearHover ();
    updateHover();
}

void WView::hoverLeave()
{
    Q_D(WView);

    if (d->dragged) return;

    if (d->resizing == false)
    {
        QPoint pos = QCursor::pos();

#ifdef Q_OS_LINUX
        d->setMousePos(mapFromGlobal(pos));
#else
        d->setMousePos(QPoint(pos.x() - x(), pos.y() - y()));
#endif
    }

    if (d->active && d->isUnderMouse())
    {
        d->resetHover = true;
    }
    else d->setEntered(false);
}

#ifdef QT_4

/* virtual */ void WView::drawBackground(QPainter *, const QRectF &) {}
/* virtual */ void WView::drawForeground(QPainter *, const QRectF &) {}

#endif

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

//#ifdef QT_4

///* virtual */ void WView::enterEvent(QEvent * event)
//{
//    Q_D(WView);

//    d->setEntered(true);

//    WAbstractView::enterEvent(event);
//}

///* virtual */ void WView::leaveEvent(QEvent * event)
//{
//    Q_D(WView);

//    // FIXME: Sometimes we get a leaveEvent for no reason.
//    if (d->timerLeave.isActive())
//    {
//        if (d->item == NULL || d->item->isUnderMouse() == false)
//        {
//            d->setEntered(false);
//        }
//    }
//    else d->setEntered(false);

//    WAbstractView::leaveEvent(event);
//}

//#endif

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
            d->applyDrop();
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

    if (d->drag)
    {
#ifdef Q_OS_WIN
        d->drag = NULL;

#ifdef QT_LATEST
        d->dragSkip = true;
#endif

        keybd_event(VK_ESCAPE, 0x81,               0, 0);
        keybd_event(VK_ESCAPE, 0x81, KEYEVENTF_KEYUP, 0);
#else
        event->accept();
#endif
    }
    else
    {
#ifdef QT_LATEST
        if (d->dragSkip)
        {
            d->dragSkip = false;

            return;
        }
#endif

        d->dragAccepted = true;

        const QMimeData * mime = event->mimeData();

        if (mime->hasUrls())
        {
             d->dragData.text = mime->urls().first().toString();
        }
        else d->dragData.text = mime->text();

        d->dragData.actions = event->possibleActions();
        d->dragData.action  = event->proposedAction ();

        event->accept();

        d->setDragging(true);
    }

    d->setEntered(true);
}

/* virtual */ void WView::dragLeaveEvent(QDragLeaveEvent *)
{
    Q_D(WView);

    if (d->dragAccepted)
    {
        d->dragAccepted = false;

        d->setDragging(false);
    }
    else d->clearDrag();

    d->setEntered(false);
}

/* virtual */ void WView::dragMoveEvent(QDragMoveEvent *)
{
    Q_D(WView); d->updateDrag();
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WView::dropEvent(QDropEvent *)
{
    Q_D(WView);

    if (d->areaDrop)
    {
#ifdef QT_4
        activateWindow();

        setFocus();
#else
        requestActivate();
#endif

        d->applyDrop();
    }

    if (d->dragAccepted)
    {
        d->dragAccepted = false;

        d->setDragging(false);
    }
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

    WDeclarativeKeyEvent keyEvent(*event);

    emit keyReleased(&keyEvent);

    WAbstractView::keyReleaseEvent(event);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WView::focusInEvent(QFocusEvent * event)
{
    Q_D(WView);

    d->setActive(true);

#ifdef QT_4
    if (d->minimized != WAbstractView::isMinimized())
#else
    if (d->minimized != (windowState() & Qt::WindowMinimized))
#endif
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

#ifdef SK_WIN_NATIVE

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
    else
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

/* virtual */ void WView::onClose()
{
    close();
}

#endif

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
QDeclarativeItem * WView::item() const
#else
QQuickItem * WView::item() const
#endif
{
    Q_D(const WView); return d->item;
}

#ifdef QT_4
void WView::setItem(QDeclarativeItem * item)
#else
void WView::setItem(QQuickItem * item)
#endif
{
    Q_D(WView);

    if (d->item)
    {
#ifdef QT_4
        d->scene->removeItem(d->item);
#endif

        disconnect(d->item, 0, this, 0);

        delete d->item;

        return;
    }

    d->item = item;

    if (item)
    {
#ifdef QT_4
        d->scene->addItem(item);
#else
        d->item->setParentItem(contentItem());
#endif

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

    if (d->maximized || d->fullScreen) return;

#ifdef QT_4
    move(x, y());
#else
    setPosition(x, y());
#endif

    emit xChanged();
}

void WView::setY(int y)
{
    if (this->y() == y) return;

    Q_D(WView);

    if (d->maximized || d->fullScreen) return;

#ifdef QT_4
    move(x(), y);
#else
    setPosition(x(), y);
#endif

    emit yChanged();
}

//-------------------------------------------------------------------------------------------------

void WView::setWidth(int width)
{
    if (this->width() == width || width < 0) return;

    Q_D(WView);

    if (d->maximized || d->fullScreen) return;

    resize(width, height());

    emit widthChanged();
}

void WView::setHeight(int height)
{
    if (this->height() == height || height < 0) return;

    Q_D(WView);

    if (d->maximized || d->fullScreen) return;

    resize(width(), height);

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

    WAbstractView::setMinimumWidth(width);

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

    WAbstractView::setMinimumHeight(height);

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
         WAbstractView::setMaximumWidth(QWIDGETSIZE_MAX);
    }
    else WAbstractView::setMaximumWidth(width);

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
         WAbstractView::setMaximumHeight(QWIDGETSIZE_MAX);
    }
    else WAbstractView::setMaximumHeight(height);

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
        //-----------------------------------------------------------------------------------------
        // FIXME Linux: Workaround to undock the window.

        QRect rect = availableGeometry();

        if (height() == rect.height())
        {
            showMaximized();
            showNormal   ();

            setGeometry(d->getGeometry(rect));

            return;
        }

        //-----------------------------------------------------------------------------------------
#endif

        d->maximized = true;

        if (d->fullScreen == false)
        {
            showMaximized();
        }
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
    // FIXME Windows: Locking during keyPress skips keyRelease.
    if (d->keyVirtual)
    {
        keybd_event(d->keyVirtual, d->keyScan, KEYEVENTF_KEYUP, 0);
    }
#endif

    d->updateFlags();

    emit lockedChanged();
}

//-------------------------------------------------------------------------------------------------

bool WView::isScaling() const
{
    Q_D(const WView); return d->scaling;
}

void WView::setScaling(bool scaling)
{
    Q_D(WView);

    if (d->scaling == scaling) return;

    d->scaling = scaling;

    emit scalingChanged();
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

Qt::CursorShape WView::mouseCursor() const
{
    Q_D(const WView); return d->cursor;
}

//-------------------------------------------------------------------------------------------------

bool WView::opengl() const
{
    Q_D(const WView); return d->opengl;
}

#ifdef QT_4

void WView::setOpengl(bool enabled)
{
    Q_D(WView);

    if (d->opengl == enabled) return;

    d->opengl = enabled;

    if (enabled)
    {
        d->updateViewport();

        setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

#ifdef Q_OS_WIN
        setWindowClip(false);
#endif
    }
    else
    {
        setViewport(NULL);

        setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);

#ifdef Q_OS_WIN
        setWindowClip(true);
#endif
    }

    emit openglChanged();
}

#endif

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

#ifdef QT_4
    setRenderHint(QPainter::Antialiasing, enabled);
#else
    QSurfaceFormat format = this->format();

    if (enabled)
    {
         format.setSamples(1);
    }
    else format.setSamples(0);

    setFormat(format);
#endif

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

#ifdef QT_4
    if (d->opengl) d->updateViewport();
#else
    QSurfaceFormat format = this->format();

    if (enabled)
    {
         format.setSwapInterval(1);
    }
    else format.setSwapInterval(0);

    setFormat(format);
#endif

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
#ifdef QT_4
    return wControllerView->availableGeometry(this);
#else
    return screen()->availableGeometry();
#endif
}

QRect WView::screenGeometry() const
{
#ifdef QT_4
    return wControllerView->screenGeometry(this);
#else
    return screen()->geometry();
#endif
}

#endif // SK_NO_VIEW
