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

#include "WWindow.h"

#ifndef SK_NO_WINDOW

#if defined(Q_OS_ANDROID) && defined(QT_5)
// Qt includes
#include <QtAndroid>
#endif

// Sk includes
#include <WControllerApplication>
#include <WControllerFile>

// Private includes
#include "WWindow_p.h"

//=================================================================================================
// WViewportPrivate
//=================================================================================================

WViewportPrivate::WViewportPrivate(WViewport * p) : WDeclarativeMouseAreaPrivate(p) {}

void WViewportPrivate::init() {}

//=================================================================================================
// WViewport
//=================================================================================================

#ifdef QT_4
/* explicit */ WViewport::WViewport(QDeclarativeItem * parent)
#else
/* explicit */ WViewport::WViewport(QQuickItem * parent)
#endif
    : WDeclarativeMouseArea(new WViewportPrivate(this), parent)
{
    Q_D(WViewport); d->init();
}

//-------------------------------------------------------------------------------------------------
// Protected events
//-------------------------------------------------------------------------------------------------

/* virtual */ void WViewport::keyPressEvent(QKeyEvent * event)
{
    Q_D(WViewport);

    WDeclarativeKeyEvent keyEvent(*event);

    emit d->view->viewportKeyPressed(&keyEvent);

    if (keyEvent.isAccepted() == false)
    {
        WDeclarativeMouseArea::keyPressEvent(event);
    }
}

/* virtual */ void WViewport::keyReleaseEvent(QKeyEvent * event)
{
    Q_D(WViewport);

    WDeclarativeKeyEvent keyEvent(*event);

    emit d->view->viewportKeyReleased(&keyEvent);

    WDeclarativeMouseArea::keyReleaseEvent(event);
}

//=================================================================================================
// WWindowPrivate
//=================================================================================================

WWindowPrivate::WWindowPrivate(WWindow * p) : WViewPrivate(p) {}

/* virtual */ WWindowPrivate::~WWindowPrivate()
{
    deleteItems();
}

//-------------------------------------------------------------------------------------------------

void WWindowPrivate::init()
{
    Q_Q(WWindow);

    viewport = static_cast<WViewport *> (item);

    icon = sk->icon();

    visible = true;

    viewport->setAcceptHoverEvents(true);

#ifdef QT_NEW
    viewport->setFlag(QQuickItem::ItemAcceptsDrops);
#endif

#ifdef QT_4
    q->setWindowTitle(sk->name());
#else
    q->setTitle(sk->name());
#endif

    if (icon.isEmpty() == false)
    {
#ifdef QT_4
        q->setWindowIcon(QIcon(icon));
#else
        q->WView::setIcon(QIcon(icon));
#endif
    }

    q->WView::setVisible(true);

    //---------------------------------------------------------------------------------------------
    // WDeclarativeMouseArea

    QObject::connect(viewport, SIGNAL(scaleChanged()), q, SIGNAL(scaleChanged()));

    QObject::connect(viewport, SIGNAL(hoveredChanged()), q, SIGNAL(hoveredChanged     ()));
    QObject::connect(viewport, SIGNAL(pressedChanged()), q, SIGNAL(mousePressedChanged()));

    QObject::connect(viewport, SIGNAL(enabledChanged()), q, SIGNAL(enabledChanged()));

    QObject::connect(viewport, SIGNAL(acceptedButtonsChanged()),
                     q,        SIGNAL(acceptedButtonsChanged()));

    //QObject::connect(viewport, SIGNAL(hoverEnabledChanged()), q, SIGNAL(hoverEnabledChanged()));
    QObject::connect(viewport, SIGNAL(hoverRetainChanged ()), q, SIGNAL(hoverRetainChanged ()));

    QObject::connect(viewport, SIGNAL(wheelEnabledChanged()), q, SIGNAL(wheelEnabledChanged()));

    QObject::connect(viewport, SIGNAL(dropEnabledChanged()), q, SIGNAL(dropEnabledChanged()));

    QObject::connect(viewport, SIGNAL(dragAcceptedChanged()), q, SIGNAL(dragAcceptedChanged()));

    QObject::connect(viewport, SIGNAL(preventStealingChanged()),
                     q,        SIGNAL(preventStealingChanged()));

    QObject::connect(viewport, SIGNAL(cursorChanged    ()), q, SIGNAL(cursorChanged    ()));
    QObject::connect(viewport, SIGNAL(cursorDropChanged()), q, SIGNAL(cursorDropChanged()));

    QObject::connect(viewport, SIGNAL(positionChanged(WDeclarativeMouseEvent *)),
                     q,        SIGNAL(positionChanged(WDeclarativeMouseEvent *)));

    //QObject::connect(viewport, SIGNAL(mousePositionChanged()), q, SIGNAL(mousePositionChanged()));

    QObject::connect(viewport, SIGNAL(pressed(WDeclarativeMouseEvent *)),
                     q,        SIGNAL(pressed(WDeclarativeMouseEvent *)));

    QObject::connect(viewport, SIGNAL(pressAndHold(WDeclarativeMouseEvent *)),
                     q,        SIGNAL(pressAndHold(WDeclarativeMouseEvent *)));

    QObject::connect(viewport, SIGNAL(released(WDeclarativeMouseEvent *)),
                     q,        SIGNAL(released(WDeclarativeMouseEvent *)));

    QObject::connect(viewport, SIGNAL(clicked(WDeclarativeMouseEvent *)),
                     q,        SIGNAL(clicked(WDeclarativeMouseEvent *)));

    QObject::connect(viewport, SIGNAL(doubleClicked(WDeclarativeMouseEvent *)),
                     q,        SIGNAL(doubleClicked(WDeclarativeMouseEvent *)));

    QObject::connect(viewport, SIGNAL(entered()), q, SIGNAL(entered()));
    QObject::connect(viewport, SIGNAL(exited ()), q, SIGNAL(exited ()));

    QObject::connect(viewport, SIGNAL(dragEntered(WDeclarativeDropEvent *)),
                     q,        SIGNAL(dragEntered(WDeclarativeDropEvent *)));

    QObject::connect(viewport, SIGNAL(dragExited(WDeclarativeDropEvent *)),
                     q,        SIGNAL(dragExited(WDeclarativeDropEvent *)));

    QObject::connect(viewport, SIGNAL(dragMove(WDeclarativeDropEvent *)),
                     q,        SIGNAL(dragMove(WDeclarativeDropEvent *)));

    QObject::connect(viewport, SIGNAL(drop(WDeclarativeDropEvent *)),
                     q,        SIGNAL(drop(WDeclarativeDropEvent *)));

    QObject::connect(viewport, SIGNAL(canceled()), q, SIGNAL(canceled()));

    QObject::connect(viewport, SIGNAL(wheeled(qreal)), q, SIGNAL(wheeled(qreal)));
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WWindowPrivate::deleteItems()
{
    foreach (QObject * item, items) delete item;

    items.clear();
}

//=================================================================================================
// WWindow
//=================================================================================================

#ifdef QT_4
/* explicit */ WWindow::WWindow(QWidget * parent)
#else
/* explicit */ WWindow::WWindow(QWindow * parent)
#endif
// NOTE: We need to new WViewport here to avoid a crash in 'setParentItem' from the
//       WViewPrivate::init function.
#ifdef SK_WIN_NATIVE
    : WView(new WWindowPrivate(this), new WViewport, parent)
#elif defined(Q_OS_WIN)
    : WView(new WWindowPrivate(this), new WViewport, parent,
            Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);
#else
    : WView(new WWindowPrivate(this), new WViewport, parent, Qt::FramelessWindowHint)
#endif
{
    Q_D(WWindow); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------
// Focus

/* Q_INVOKABLE */ bool WWindow::getFocus() const
{
#ifdef QT_4
    Q_D(const WWindow);

    return (d->scene->focusItem() != NULL);
#else
    return (focusObject() != NULL);
#endif
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WWindow::setFocus(bool focus)
{
    Q_D(WWindow); d->viewport->setFocus(focus);
}

/* Q_INVOKABLE */ void WWindow::clearFocus()
{
    setFocus(true);
}

//-------------------------------------------------------------------------------------------------

#ifdef QT_4
/* Q_INVOKABLE */ void WWindow::clearFocusItem(QDeclarativeItem * item)
#else
/* Q_INVOKABLE */ void WWindow::clearFocusItem(QQuickItem * item)
#endif
{
    Q_ASSERT(item);

#ifdef QT_4
    if (item->focusItem())
    {
        setFocus(true);
    }
#else
    QQuickItem * parent = activeFocusItem();

    while (parent)
    {
        parent = parent->parentItem();

        if (parent == item)
        {
            setFocus(true);

            return;
        }
    }
#endif
}

//-------------------------------------------------------------------------------------------------
// WDeclarativeMouseArea

/* Q_INVOKABLE */ void WWindow::press(Qt::MouseButton button)
{
    Q_D(WWindow); d->viewport->press(button);
}

/* Q_INVOKABLE */ void WWindow::release(Qt::MouseButton button)
{
    Q_D(WWindow); d->viewport->release(button);
}

/* Q_INVOKABLE */ void WWindow::click(Qt::MouseButton button)
{
    Q_D(WWindow); d->viewport->click(button);
}

//---------------------------------------------------------------------------------------------
// QML

/* Q_INVOKABLE */ void WWindow::press(int button)
{
    Q_D(WWindow); d->viewport->press(button);
}

/* Q_INVOKABLE */ void WWindow::release(int button)
{
    Q_D(WWindow); d->viewport->release(button);
}

/* Q_INVOKABLE */ void WWindow::click(int button)
{
    Q_D(WWindow); d->viewport->click(button);
}

//-------------------------------------------------------------------------------------------------
// Static functions
//-------------------------------------------------------------------------------------------------

#if defined(Q_OS_ANDROID) && defined(QT_NEW)

/* Q_INVOKABLE static */ void WWindow::hideSplash(int duration)
{
#ifdef QT_5
    QtAndroid::hideSplashScreen(duration);
#else
    QNativeInterface::QAndroidApplication::hideSplashScreen(duration);
#endif
}

#endif

//-------------------------------------------------------------------------------------------------
// Protected events
//-------------------------------------------------------------------------------------------------

//#ifdef QT_4

///* virtual */ void WWindow::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
//{
//    Q_D(WWindow);

//    d->view->hoverEnter();

//    WDeclarativeMouseArea::hoverEnterEvent(event);
//}

///* virtual */ void WWindow::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)

//{
//    Q_D(WWindow);

//    d->view->hoverLeave();

//    WDeclarativeMouseArea::hoverLeaveEvent(event);
//}

//#endif

//-------------------------------------------------------------------------------------------------

//#ifdef QT_NEW

///* virtual */ void WWindow::dragEnterEvent(QDragEnterEvent * event)
//{
//    Q_D(WWindow); d->view->dragEnterEvent(event);
//}

///* virtual */ void WWindow::dragLeaveEvent(QDragLeaveEvent * event)
//{
//    Q_D(WWindow); d->view->dragLeaveEvent(event);
//}

///* virtual */ void WWindow::dragMoveEvent(QDragMoveEvent * event)
//{
//    Q_D(WWindow); d->view->dragMoveEvent(event);
//}

////-------------------------------------------------------------------------------------------------

///* virtual */ void WWindow::dropEvent(QDropEvent * event)
//{
//    Q_D(WWindow); d->view->dropEvent(event);
//}

//#endif

//-------------------------------------------------------------------------------------------------
// Private declarative
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
/* static */ void WWindow::childrenAppend(QDeclarativeListProperty<QObject> * property,
                                          QObject                           * object)
#else
/* static */ void WWindow::childrenAppend(QQmlListProperty<QObject> * property,
                                          QObject                   * object)
#endif
{
    WWindowPrivate * p = static_cast<WWindow *> (property->object)->d_func();

#ifdef QT_4
    QDeclarativeItem * item = qobject_cast<QDeclarativeItem *> (object);
#else
    QQuickItem * item = qobject_cast<QQuickItem *> (object);
#endif

    if (item) item->setParentItem(p->viewport);

    p->items.append(object);
}

#ifdef QT_4
/* static */ void WWindow::childrenClear(QDeclarativeListProperty<QObject> * property)
#else
/* static */ void WWindow::childrenClear(QQmlListProperty<QObject> * property)
#endif
{
    static_cast<WWindow *> (property->object)->d_func()->deleteItems();
}

#ifdef QT_4
/* static */ int WWindow::childrenCount(QDeclarativeListProperty<QObject> * property)
#elif defined(QT_5)
/* static */ int WWindow::childrenCount(QQmlListProperty<QObject> * property)
#else // QT_6
/* static */ qsizetype WWindow::childrenCount(QQmlListProperty<QObject> * property)
#endif
{
    return static_cast<WWindow *> (property->object)->d_func()->items.count();
}

#ifdef QT_4
/* static */ QObject * WWindow::childrenAt(QDeclarativeListProperty<QObject> * property, int index)
#elif defined(QT_5)
/* static */ QObject * WWindow::childrenAt(QQmlListProperty<QObject> * property, int index)
#else // QT_6
/* static */ QObject * WWindow::childrenAt(QQmlListProperty<QObject> * property, qsizetype index)
#endif
{
    return static_cast<WWindow *> (property->object)->d_func()->items.at(index);
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

WDeclarativeMouseArea * WWindow::viewport() const
{
    Q_D(const WWindow); return d->viewport;
}

//-------------------------------------------------------------------------------------------------

#ifdef QT_4
QDeclarativeListProperty<QObject> WWindow::children()
#else
QQmlListProperty<QObject> WWindow::children()
#endif
{
#ifdef QT_4
    return QDeclarativeListProperty<QObject>(this, 0, childrenAppend, childrenCount, childrenAt,
                                             childrenClear);
#else
    return QQmlListProperty<QObject>(this, 0, childrenAppend, childrenCount, childrenAt,
                                     childrenClear);
#endif
}

//-------------------------------------------------------------------------------------------------

QString WWindow::icon() const
{
    Q_D(const WWindow); return d->icon;
}

void WWindow::setIcon(const QString & icon)
{
    Q_D(WWindow);

    if (d->icon == icon) return;

    d->icon = icon;

#ifdef QT_4
    setWindowIcon(QIcon(icon));
#else
    WView::setIcon(QIcon(icon));
#endif

    emit iconChanged();
}

//-------------------------------------------------------------------------------------------------

bool WWindow::isVisible() const
{
    Q_D(const WWindow); return d->visible;
}

void WWindow::setVisible(bool visible)
{
    Q_D(WWindow);

    if (d->visible == visible) return;

    d->visible = visible;

    WView::setVisible(visible);

    emit visibleChanged();
}

//-------------------------------------------------------------------------------------------------

qreal WWindow::opacity() const
{
#ifdef QT_4
    return windowOpacity();
#else
    return WView::opacity();
#endif
}

void WWindow::setOpacity(qreal opacity)
{
#ifdef QT_4
    if (windowOpacity() == opacity) return;

    setWindowOpacity(opacity);
#else
    if (WView::opacity() == opacity) return;

    WView::setOpacity(opacity);
#endif

    emit opacityChanged();
}

//-------------------------------------------------------------------------------------------------

void WWindow::setLocked(bool locked)
{
    Q_D(WWindow);

    WView::setLocked(locked);

    bool visible = WView::isVisible();

    if (d->visible != visible)
    {
        d->visible = visible;

        emit visibleChanged();
    }
}

//-------------------------------------------------------------------------------------------------
// WDeclarativeMouseArea

qreal WWindow::scale() const
{
    Q_D(const WWindow); return d->viewport->scale();
}

void WWindow::setScale(qreal scale)
{
    Q_D(WWindow);

    d->viewport->setScale(scale);
}

//-------------------------------------------------------------------------------------------------

//qreal WWindow::mouseX() const
//{
//    Q_D(const WWindow); return d->viewport->mouseX();
//}

//qreal WWindow::mouseY() const
//{
//    Q_D(const WWindow); return d->viewport->mouseY();
//}

//-------------------------------------------------------------------------------------------------

bool WWindow::isEnabled() const
{
    Q_D(const WWindow); return d->viewport->isEnabled();
}

void WWindow::setEnabled(bool enabled)
{
    Q_D(WWindow);

    d->viewport->setEnabled(enabled);
}

//-------------------------------------------------------------------------------------------------

bool WWindow::hovered() const
{
    Q_D(const WWindow); return d->viewport->hovered();
}

bool WWindow::pressed() const
{
    Q_D(const WWindow); return d->viewport->pressed();
}

//-------------------------------------------------------------------------------------------------

Qt::MouseButtons WWindow::pressedButtons() const
{
    Q_D(const WWindow); return d->viewport->pressedButtons();
}

Qt::MouseButtons WWindow::acceptedButtons() const
{
    Q_D(const WWindow); return d->viewport->acceptedButtons();
}

void WWindow::setAcceptedButtons(Qt::MouseButtons buttons)
{
    Q_D(WWindow);

    d->viewport->setAcceptedButtons(buttons);
}

//-------------------------------------------------------------------------------------------------

//bool WWindow::hoverEnabled() const
//{
//    Q_D(const WWindow); return d->viewport->hoverEnabled();
//}

//void WWindow::setHoverEnabled(bool enabled)
//{
//    Q_D(WWindow);

//    d->viewport->setHoverEnabled(enabled);
//}

bool WWindow::hoverRetain() const
{
    Q_D(const WWindow); return d->viewport->hoverRetain();
}

void WWindow::setHoverRetain(bool retain)
{
    Q_D(WWindow);

    d->viewport->setHoverRetain(retain);
}

//-------------------------------------------------------------------------------------------------

bool WWindow::wheelEnabled() const
{
    Q_D(const WWindow); return d->viewport->wheelEnabled();
}

void WWindow::setWheelEnabled(bool enabled)
{
    Q_D(WWindow);

    d->viewport->setWheelEnabled(enabled);
}

//-------------------------------------------------------------------------------------------------

bool WWindow::dropEnabled() const
{
    Q_D(const WWindow); return d->viewport->dropEnabled();
}

void WWindow::setDropEnabled(bool enabled)
{
    Q_D(WWindow);

    d->viewport->setDropEnabled(enabled);
}

//-------------------------------------------------------------------------------------------------

bool WWindow::dragAccepted() const
{
    Q_D(const WWindow); return d->viewport->dragAccepted();
}

WDeclarativeDrag * WWindow::drag()
{
    Q_D(const WWindow); return d->viewport->drag();
}

//-------------------------------------------------------------------------------------------------

bool WWindow::preventStealing() const
{
    Q_D(const WWindow); return d->viewport->preventStealing();
}

void WWindow::setPreventStealing(bool prevent)
{
    Q_D(WWindow);

    d->viewport->setPreventStealing(prevent);
}

//-------------------------------------------------------------------------------------------------

Qt::CursorShape WWindow::cursor() const
{
    Q_D(const WWindow); return d->viewport->cursor();
}

void WWindow::setCursor(Qt::CursorShape shape)
{
    Q_D(WWindow);

    d->viewport->setCursor(shape);
}

Qt::CursorShape WWindow::cursorDrop() const
{
    Q_D(const WWindow); return d->viewport->cursorDrop();
}

void WWindow::setCursorDrop(Qt::CursorShape shape)
{
    Q_D(WWindow);

    d->viewport->setCursorDrop(shape);
}

#endif // SK_NO_WINDOW
