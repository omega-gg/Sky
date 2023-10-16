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
#ifdef QT_4
#include <WView>
#endif

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
// Protected

#ifdef QT_4
WViewport::WViewport(WViewportPrivate * p, QDeclarativeItem * parent)
#else
WViewport::WViewport(WViewportPrivate * p, QQuickItem * parent)
#endif
    : WDeclarativeMouseArea(p, parent)
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

#ifdef QT_4
WWindowPrivate::WWindowPrivate(WWindow * p) : WViewportPrivate(p) {}
#else
WWindowPrivate::WWindowPrivate(WWindow * p) : WViewPrivate(p) {}
#endif

/* virtual */ WWindowPrivate::~WWindowPrivate()
{
#ifdef QT_4
    view->deleteLater();
#else
    deleteItems();
#endif
}

//-------------------------------------------------------------------------------------------------

void WWindowPrivate::init()
{
    Q_Q(WWindow);

#ifdef QT_4
    viewport = q;
#else
    viewport = static_cast<WViewport *> (item);
#endif

    icon = sk->icon();

    visible = true;

    viewport->setAcceptHoverEvents(true);

#ifdef QT_NEW
    viewport->setFlag(QQuickItem::ItemAcceptsDrops);
#endif

#ifdef QT_4
#ifdef SK_WIN_NATIVE
    view = new WView(q, NULL);
#elif defined(Q_OS_WIN)
    view = new WView(q, NULL, Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);
#else
    view = new WView(q, NULL, Qt::FramelessWindowHint);
#endif

    view->setWindowTitle(sk->name());
#else
    q->setTitle(sk->name());
#endif

    if (icon.isEmpty() == false)
    {
#ifdef QT_4
        view->setWindowIcon(QIcon(icon));
#else
        q->WView::setIcon(QIcon(icon));
#endif
    }

#ifdef QT_4
    view->setVisible(true);
#else
    q->WView::setVisible(true);
#endif

#ifdef QT_4
    //---------------------------------------------------------------------------------------------
    // WView

    QObject::connect(view, SIGNAL(messageReceived(const QString &)),
                     q,    SIGNAL(messageReceived(const QString &)));

    QObject::connect(view, SIGNAL(stateChanged(Qt::WindowState)),
                     q,    SIGNAL(stateChanged(Qt::WindowState)));

    QObject::connect(view, SIGNAL(fadeIn ()), q, SIGNAL(fadeIn ()));
    QObject::connect(view, SIGNAL(fadeOut()), q, SIGNAL(fadeOut()));

    QObject::connect(view, SIGNAL(dragEnded()), q, SIGNAL(dragEnded()));

    QObject::connect(view, SIGNAL(beforeClose()), q, SIGNAL(beforeClose()));

    //---------------------------------------------------------------------------------------------

    QObject::connect(view, SIGNAL(itemWidthChanged ()), q, SIGNAL(itemWidthChanged ()));
    QObject::connect(view, SIGNAL(itemHeightChanged()), q, SIGNAL(itemHeightChanged()));

    QObject::connect(view, SIGNAL(xChanged()), q, SIGNAL(viewXChanged()));
    QObject::connect(view, SIGNAL(yChanged()), q, SIGNAL(viewYChanged()));

    QObject::connect(view, SIGNAL(widthChanged ()), q, SIGNAL(viewWidthChanged ()));
    QObject::connect(view, SIGNAL(heightChanged()), q, SIGNAL(viewHeightChanged()));

    QObject::connect(view, SIGNAL(centerXChanged()), q, SIGNAL(centerXChanged()));
    QObject::connect(view, SIGNAL(centerYChanged()), q, SIGNAL(centerYChanged()));

    QObject::connect(view, SIGNAL(originXChanged()), q, SIGNAL(originXChanged()));
    QObject::connect(view, SIGNAL(originYChanged()), q, SIGNAL(originYChanged()));

    QObject::connect(view, SIGNAL(ratioChanged()), q, SIGNAL(ratioChanged()));

    QObject::connect(view, SIGNAL(zoomChanged()), q, SIGNAL(zoomChanged()));

    QObject::connect(view, SIGNAL(minimumWidthChanged ()), q, SIGNAL(minimumWidthChanged ()));
    QObject::connect(view, SIGNAL(minimumHeightChanged()), q, SIGNAL(minimumHeightChanged()));

    QObject::connect(view, SIGNAL(maximumWidthChanged ()), q, SIGNAL(maximumWidthChanged ()));
    QObject::connect(view, SIGNAL(maximumHeightChanged()), q, SIGNAL(maximumHeightChanged()));

    QObject::connect(view, SIGNAL(geometryNormalChanged()), q, SIGNAL(geometryNormalChanged()));

    QObject::connect(view, SIGNAL(minimizedChanged ()), q, SIGNAL(minimizedChanged ()));
    QObject::connect(view, SIGNAL(maximizedChanged ()), q, SIGNAL(maximizedChanged ()));
    QObject::connect(view, SIGNAL(fullScreenChanged()), q, SIGNAL(fullScreenChanged()));
    QObject::connect(view, SIGNAL(lockedChanged    ()), q, SIGNAL(lockedChanged    ()));
    QObject::connect(view, SIGNAL(scalingChanged   ()), q, SIGNAL(scalingChanged   ()));

    QObject::connect(view, SIGNAL(activeChanged  ()), q, SIGNAL(activeChanged  ()));
    QObject::connect(view, SIGNAL(enteredChanged ()), q, SIGNAL(enteredChanged ()));
    QObject::connect(view, SIGNAL(draggingChanged()), q, SIGNAL(draggingChanged()));
    QObject::connect(view, SIGNAL(draggedChanged ()), q, SIGNAL(draggedChanged ()));
    QObject::connect(view, SIGNAL(resizingChanged()), q, SIGNAL(resizingChanged()));
    QObject::connect(view, SIGNAL(touchingChanged()), q, SIGNAL(touchingChanged()));

    QObject::connect(view, SIGNAL(mousePosChanged   ()), q, SIGNAL(mousePosChanged   ()));
    QObject::connect(view, SIGNAL(mouseCursorChanged()), q, SIGNAL(mouseCursorChanged()));

    QObject::connect(view, SIGNAL(openglChanged()), q, SIGNAL(openglChanged()));

    QObject::connect(view, SIGNAL(antialiasChanged()), q, SIGNAL(antialiasChanged()));
    QObject::connect(view, SIGNAL(vsyncChanged    ()), q, SIGNAL(vsyncChanged    ()));

    QObject::connect(view, SIGNAL(hoverEnabledChanged()), q, SIGNAL(hoverEnabledChanged()));

    QObject::connect(view, SIGNAL(fadeEnabledChanged()), q, SIGNAL(fadeEnabledChanged()));

    //---------------------------------------------------------------------------------------------

    QObject::connect(view, SIGNAL(idleCheckChanged()), q, SIGNAL(idleCheckChanged()));
    QObject::connect(view, SIGNAL(idleChanged     ()), q, SIGNAL(idleChanged     ()));
    QObject::connect(view, SIGNAL(idleDelayChanged()), q, SIGNAL(idleDelayChanged()));

    //---------------------------------------------------------------------------------------------

    QObject::connect(view, SIGNAL(mousePressed(QDeclarativeMouseEvent *)),
                     q,    SIGNAL(mousePressed(QDeclarativeMouseEvent *)));

    QObject::connect(view, SIGNAL(mouseReleased(QDeclarativeMouseEvent *)),
                     q,    SIGNAL(mouseReleased(QDeclarativeMouseEvent *)));

    QObject::connect(view, SIGNAL(mouseDoubleClicked(QDeclarativeMouseEvent *)),
                     q,    SIGNAL(mouseDoubleClicked(QDeclarativeMouseEvent *)));

    QObject::connect(view, SIGNAL(keyPressed(WDeclarativeKeyEvent *)),
                     q,    SIGNAL(keyPressed(WDeclarativeKeyEvent *)));

    QObject::connect(view, SIGNAL(keyReleased(WDeclarativeKeyEvent *)),
                     q,    SIGNAL(keyReleased(WDeclarativeKeyEvent *)));

    QObject::connect(view, SIGNAL(viewportKeyPressed(WDeclarativeKeyEvent *)),
                     q,    SIGNAL(viewportKeyPressed(WDeclarativeKeyEvent *)));

    QObject::connect(view, SIGNAL(viewportKeyReleased(WDeclarativeKeyEvent *)),
                     q,    SIGNAL(viewportKeyReleased(WDeclarativeKeyEvent *)));

    //---------------------------------------------------------------------------------------------

    QObject::connect(view, SIGNAL(keyShiftPressedChanged()),
                     q,    SIGNAL(keyShiftPressedChanged()));

    QObject::connect(view, SIGNAL(keyControlPressedChanged()),
                     q,    SIGNAL(keyControlPressedChanged()));

    QObject::connect(view, SIGNAL(keyAltPressedChanged()),
                     q,    SIGNAL(keyAltPressedChanged()));

    //---------------------------------------------------------------------------------------------

    QObject::connect(view, SIGNAL(availableGeometryChanged()),
                     q,    SIGNAL(availableGeometryChanged()));
#else
    //---------------------------------------------------------------------------------------------
    // WDeclarativeMouseArea

    QObject::connect(viewport, SIGNAL(scaleChanged()), q, SIGNAL(scaleChanged()));

    QObject::connect(viewport, SIGNAL(hoveredChanged    ()), q, SIGNAL(hoveredChanged    ()));
    QObject::connect(viewport, SIGNAL(hoverActiveChanged()), q, SIGNAL(hoverActiveChanged()));

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

    QObject::connect(viewport, SIGNAL(positionChanged(QQuickMouseEvent *)),
                     q,        SIGNAL(positionChanged(QQuickMouseEvent *)));

    //QObject::connect(viewport, SIGNAL(mousePositionChanged()), q, SIGNAL(mousePositionChanged()));

    QObject::connect(viewport, SIGNAL(pressed(QQuickMouseEvent *)),
                     q,        SIGNAL(pressed(QQuickMouseEvent *)));

    QObject::connect(viewport, SIGNAL(pressAndHold(QQuickMouseEvent *)),
                     q,        SIGNAL(pressAndHold(QQuickMouseEvent *)));

    QObject::connect(viewport, SIGNAL(released(QQuickMouseEvent *)),
                     q,        SIGNAL(released(QQuickMouseEvent *)));

    QObject::connect(viewport, SIGNAL(clicked(QQuickMouseEvent *)),
                     q,        SIGNAL(clicked(QQuickMouseEvent *)));

    QObject::connect(viewport, SIGNAL(doubleClicked(QQuickMouseEvent *)),
                     q,        SIGNAL(doubleClicked(QQuickMouseEvent *)));

    QObject::connect(viewport, SIGNAL(entered()), q, SIGNAL(entered()));
    QObject::connect(viewport, SIGNAL(exited ()), q, SIGNAL(exited ()));

    QObject::connect(viewport, SIGNAL(hoverEntered()), q, SIGNAL(hoverEntered()));
    QObject::connect(viewport, SIGNAL(hoverExited ()), q, SIGNAL(hoverExited ()));

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
#endif
}

#ifdef QT_NEW

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WWindowPrivate::deleteItems()
{
    foreach (QObject * item, items) delete item;

    items.clear();
}

#endif

//=================================================================================================
// WWindow
//=================================================================================================

#ifdef QT_4
/* explicit */ WWindow::WWindow(QDeclarativeItem * parent)
    : WViewport(new WWindowPrivate(this), parent)
#else
/* explicit */ WWindow::WWindow(QWindow * parent)
// NOTE: We need to new WViewport here to avoid a crash in 'setParentItem' from the
//       WViewPrivate::init function.
#ifdef SK_WIN_NATIVE
    : WView(new WWindowPrivate(this), new WViewport, parent)
#elif defined(Q_OS_WIN)
    : WView(new WWindowPrivate(this), new WViewport, parent,
            Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint)
#else
    : WView(new WWindowPrivate(this), new WViewport, parent, Qt::FramelessWindowHint)
#endif
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

    return (d->view->d_func()->scene->focusItem() != NULL);
#else
    return (focusObject() != NULL);
#endif
}

//-------------------------------------------------------------------------------------------------

#ifdef QT_NEW

/* Q_INVOKABLE */ void WWindow::setFocus(bool focus)
{
    Q_D(WWindow); d->viewport->setFocus(focus);
}

#endif

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
    QQuickItem * itemFocus = activeFocusItem();

    while (itemFocus)
    {
        if (itemFocus == item)
        {
            setFocus(true);

            return;
        }

        itemFocus = itemFocus->parentItem();
    }
#endif
}

#ifdef QT_4

//-------------------------------------------------------------------------------------------------
// WView
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WWindow::activate()
{
    Q_D(WWindow); d->view->activate();
}

/* Q_INVOKABLE */ void WWindow::raise()
{
    Q_D(WWindow); d->view->raise();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WWindow::centerWindow()
{
    Q_D(WWindow); d->view->centerWindow();
}

/* Q_INVOKABLE */ void WWindow::moveToScreen(int number)
{
    Q_D(WWindow); d->view->moveToScreen(number);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ bool WWindow::close()
{
    Q_D(WWindow);

    return d->view->close();
}

//-------------------------------------------------------------------------------------------------
// Geometry

/* Q_INVOKABLE */ int WWindow::screenCount() const
{
    Q_D(const WWindow); return d->view->screenCount();
}

/* Q_INVOKABLE */ int WWindow::screenNumber() const
{
    Q_D(const WWindow); return d->view->screenNumber();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WWindow::setMinimumSize(int width, int height)
{
    Q_D(WWindow); d->view->setMinimumSize(width, height);
}

/* Q_INVOKABLE */ void WWindow::setMaximumSize(int width, int height)
{
    Q_D(WWindow); d->view->setMaximumSize(width, height);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ QRect WWindow::getDefaultGeometry() const
{
    Q_D(const WWindow); return d->view->getDefaultGeometry();
}

/* Q_INVOKABLE */ void WWindow::setDefaultGeometry()
{
    Q_D(WWindow); d->view->setDefaultGeometry();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WWindow::saveGeometry()
{
    Q_D(WWindow); d->view->saveGeometry();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WWindow::checkPosition()
{
    Q_D(WWindow); return d->view->checkPosition();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WWindow::originTo(qreal x, qreal y)
{
    Q_D(WWindow); return d->view->originTo(x, y);
}

//-------------------------------------------------------------------------------------------------
// Drag

/* Q_INVOKABLE */ bool WWindow::testDrag(const QPointF & posA,
                                         const QPointF & posB, qreal distance)
{
    Q_D(WWindow); return d->view->testDrag(posA, posB, distance);
}

/* Q_INVOKABLE */ void WWindow::startDrag(const QString & text, int actions)
{
    Q_D(WWindow); d->view->startDrag(text, actions);
}

//-------------------------------------------------------------------------------------------------
// Hover

/* Q_INVOKABLE */ int WWindow::hoverCount() const
{
    Q_D(const WWindow); return d->view->hoverCount();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WWindow::updateHover()
{
    Q_D(WWindow); d->view->updateHover();
}

/* Q_INVOKABLE */ void WWindow::clearHover()
{
    Q_D(WWindow); d->view->clearHover();
}

//-------------------------------------------------------------------------------------------------
// Shot

/* Q_INVOKABLE */ QImage WWindow::takeShot(int x, int y, int width, int height) const
{
    Q_D(const WWindow);

    return d->view->takeShot(x, y, width, height);
}

/* Q_INVOKABLE */ bool WWindow::saveShot(const QString & fileName,
                                         int x, int y, int width, int height,
                                         const QString & format, int quality) const
{
    Q_D(const WWindow);

    return d->view->saveShot(fileName, x, y, width, height, format, quality);
}

/* Q_INVOKABLE */ bool WWindow::saveShot(const QString & fileName,
                                         const QString & format, int quality) const
{
    Q_D(const WWindow);

    return d->view->saveShot(fileName, format, quality);
}

/* Q_INVOKABLE */ void WWindow::writeShot(const QString & path,
                                          const QString & format, int quality) const
{
    Q_D(const WWindow);

    return d->view->writeShot(path, format, quality);
}

//-------------------------------------------------------------------------------------------------
// Cursor

/* Q_INVOKABLE */ void WWindow::registerCursor(Qt::CursorShape shape, const QCursor & cursor)
{
    Q_D(WWindow); d->view->registerCursor(shape, cursor);
}

/* Q_INVOKABLE */ void WWindow::unregisterCursor(Qt::CursorShape shape)
{
    Q_D(WWindow); d->view->unregisterCursor(shape);
}

/* Q_INVOKABLE */ void WWindow::unregisterCursors()
{
    Q_D(WWindow); d->view->unregisterCursors();
}

//-------------------------------------------------------------------------------------------------
// Input

/* Q_INVOKABLE */ void WWindow::mouseMove(int x, int y, Qt::MouseButton button)
{
    Q_D(WWindow); d->view->mouseMove(x, y, button);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WWindow::mousePress(Qt::MouseButton button)
{
    Q_D(WWindow); d->view->mousePress(button);
}

/* Q_INVOKABLE */ void WWindow::mouseRelease(Qt::MouseButton button)
{
    Q_D(WWindow); d->view->mouseRelease(button);
}

/* Q_INVOKABLE */ void WWindow::mouseClick(Qt::MouseButton button, int msec)
{
    Q_D(WWindow); d->view->mouseClick(button, msec);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WWindow::wheel(Qt::Orientation orientation, int delta)
{
    Q_D(WWindow); d->view->wheel(orientation, delta);
}

/* Q_INVOKABLE */ void WWindow::wheelUp(int delta)
{
    Q_D(WWindow); d->view->wheelUp(delta);
}

/* Q_INVOKABLE */ void WWindow::wheelDown(int delta)
{
    Q_D(WWindow); d->view->wheelDown(delta);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WWindow::keyPress(int key, Qt::KeyboardModifiers modifiers)
{
    Q_D(WWindow); d->view->keyPress(key, modifiers);
}

/* Q_INVOKABLE */ void WWindow::keyRelease(int key, Qt::KeyboardModifiers modifiers)
{
    Q_D(WWindow); d->view->keyRelease(key, modifiers);
}

/* Q_INVOKABLE */ void WWindow::keyClick(int key, Qt::KeyboardModifiers modifiers, int msec)
{
    Q_D(WWindow); d->view->keyClick(key, modifiers, msec);
}

//-------------------------------------------------------------------------------------------------
// QML

/* Q_INVOKABLE */ void WWindow::mouseMove(int x, int y, int button)
{
    Q_D(WWindow); d->view->mouseMove(x, y, button);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WWindow::mousePress(int button)
{
    Q_D(WWindow); d->view->mousePress(button);
}

/* Q_INVOKABLE */ void WWindow::mouseRelease(int button)
{
    Q_D(WWindow); d->view->mouseRelease(button);
}

/* Q_INVOKABLE */ void WWindow::mouseClick(int button, int msec)
{
    Q_D(WWindow); d->view->mouseClick(button, msec);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WWindow::wheel(int orientation, int delta)
{
    Q_D(WWindow); d->view->wheel(orientation, delta);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WWindow::keyPress(int key, int modifiers)
{
    Q_D(WWindow); d->view->keyPress(key, modifiers);
}

/* Q_INVOKABLE */ void WWindow::keyRelease(int key, int modifiers)
{
    Q_D(WWindow); d->view->keyRelease(key, modifiers);
}

/* Q_INVOKABLE */ void WWindow::keyClick(int key, int modifiers, int msec)
{
    Q_D(WWindow); d->view->keyClick(key, modifiers, msec);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WWindow::registerCursorUrl(int shape, const QString & url,
                                                             const QSize   & size)
{
    Q_D(WWindow); d->view->registerCursorUrl(shape, url, size);
}

/* Q_INVOKABLE */ void WWindow::unregisterCursor(int shape)
{
    Q_D(WWindow); d->view->unregisterCursor(shape);
}

//-------------------------------------------------------------------------------------------------

#ifdef Q_OS_WIN

/* Q_INVOKABLE */ void WWindow::setWindowSnap(bool enabled)
{
    Q_D(WWindow); d->view->setWindowSnap(enabled);
}

/* Q_INVOKABLE */ void WWindow::setWindowMaximize(bool enabled)
{
    Q_D(WWindow); d->view->setWindowMaximize(enabled);
}

/* Q_INVOKABLE */ void WWindow::setWindowClip(bool enabled)
{
    Q_D(WWindow); d->view->setWindowClip(enabled);
}

#endif // Q_OS_WIN

#else

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

#endif

//-------------------------------------------------------------------------------------------------
// Static functions
//-------------------------------------------------------------------------------------------------

#ifdef QT_4

/* Q_INVOKABLE static */ QPixmap WWindow::takeItemShot(QGraphicsObject * item,
                                                       const QColor    & background)
{
    return WView::takeItemShot(item, background);
}

/* Q_INVOKABLE static */ bool WWindow::saveItemShot(const QString   & fileName,
                                                    QGraphicsObject * item,
                                                    const QString   & format, int quality,
                                                    const QColor    & background)
{
    return WView::saveItemShot(fileName, item, format, quality, background);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ bool WWindow::compressShot(const QString & fileName, int quality)
{
    return WView::compressShot(fileName, quality);
}

/* Q_INVOKABLE static */ bool WWindow::compressShots(const QString & path, int quality)
{
    return WView::compressShots(path, quality);
}

#else

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
#endif

//-------------------------------------------------------------------------------------------------
// Protected events
//-------------------------------------------------------------------------------------------------

#ifdef QT_4

/* virtual */ void WWindow::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
    Q_D(WWindow);

    d->view->hoverEnter();

    WDeclarativeMouseArea::hoverEnterEvent(event);
}

/* virtual */ void WWindow::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)

{
    Q_D(WWindow);

    d->view->hoverLeave();

    WDeclarativeMouseArea::hoverLeaveEvent(event);
}

#else

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

#endif

#ifdef QT_NEW

//-------------------------------------------------------------------------------------------------
// Private declarative
//-------------------------------------------------------------------------------------------------

/* static */ void WWindow::childrenAppend(QQmlListProperty<QObject> * property,
                                          QObject                   * object)
{
    WWindowPrivate * p = static_cast<WWindow *> (property->object)->d_func();

    QQuickItem * item = qobject_cast<QQuickItem *> (object);

    if (item) item->setParentItem(p->viewport);

    p->items.append(object);
}

/* static */ void WWindow::childrenClear(QQmlListProperty<QObject> * property)
{
    static_cast<WWindow *> (property->object)->d_func()->deleteItems();
}

#ifdef QT_5
/* static */ int WWindow::childrenCount(QQmlListProperty<QObject> * property)
#else // QT_6
/* static */ qsizetype WWindow::childrenCount(QQmlListProperty<QObject> * property)
#endif
{
    return static_cast<WWindow *> (property->object)->d_func()->items.count();
}

#ifdef QT_5
/* static */ QObject * WWindow::childrenAt(QQmlListProperty<QObject> * property, int index)
#else // QT_6
/* static */ QObject * WWindow::childrenAt(QQmlListProperty<QObject> * property, qsizetype index)
#endif
{
    return static_cast<WWindow *> (property->object)->d_func()->items.at(index);
}

#endif

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

WDeclarativeMouseArea * WWindow::viewport() const
{
    Q_D(const WWindow); return d->viewport;
}

//-------------------------------------------------------------------------------------------------

#ifdef QT_NEW

QQmlListProperty<QObject> WWindow::children()
{
    return QQmlListProperty<QObject>(this, 0, childrenAppend, childrenCount, childrenAt,
                                     childrenClear);
}

#endif

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
    d->view->setWindowIcon(QIcon(icon));
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

#ifdef QT_4
    d->view->setVisible(visible);
#else
    WView::setVisible(visible);
#endif

    emit visibleChanged();
}

//-------------------------------------------------------------------------------------------------

qreal WWindow::opacity() const
{
#ifdef QT_4
    Q_D(const WWindow);

    return d->view->windowOpacity();
#else
    return WView::opacity();
#endif
}

void WWindow::setOpacity(qreal opacity)
{
#ifdef QT_4
    Q_D(WWindow);

    if (d->view->windowOpacity() == opacity) return;

    d->view->setWindowOpacity(opacity);
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

#ifdef QT_4
    d->view->setLocked(locked);

    bool visible = d->view->isVisible();
#else
    WView::setLocked(locked);

    bool visible = WView::isVisible();
#endif

    if (d->visible != visible)
    {
        d->visible = visible;

        emit visibleChanged();
    }
}

#ifdef QT_4

//-------------------------------------------------------------------------------------------------
// WView
//-------------------------------------------------------------------------------------------------

qreal WWindow::itemWidth() const
{
    Q_D(const WWindow); return d->view->itemWidth();
}

qreal WWindow::itemHeight() const
{
    Q_D(const WWindow); return d->view->itemHeight();
}

//-------------------------------------------------------------------------------------------------

int WWindow::x() const
{
    Q_D(const WWindow); return d->view->x();
}

void WWindow::setX(int x)
{
    Q_D(WWindow); d->view->setX(x);
}

int WWindow::y() const
{
    Q_D(const WWindow); return d->view->y();
}

void WWindow::setY(int y)
{
    Q_D(WWindow); d->view->setY(y);
}

//-------------------------------------------------------------------------------------------------

int WWindow::width() const
{
    Q_D(const WWindow); return d->view->width();
}

void WWindow::setWidth(int width)
{
    Q_D(WWindow); d->view->setWidth(width);
}

int WWindow::height() const
{
    Q_D(const WWindow); return d->view->height();
}

void WWindow::setHeight(int height)
{
    Q_D(WWindow); d->view->setHeight(height);
}

//-------------------------------------------------------------------------------------------------

int WWindow::centerX() const
{
    Q_D(const WWindow); return d->view->centerX();
}

int WWindow::centerY() const
{
    Q_D(const WWindow); return d->view->centerY();
}

//-------------------------------------------------------------------------------------------------

qreal WWindow::originX() const
{
    Q_D(const WWindow); return d->view->originX();
}

void WWindow::setOriginX(qreal x)
{
    Q_D(WWindow); d->view->setOriginX(x);
}

qreal WWindow::originY() const
{
    Q_D(const WWindow); return d->view->originY();
}

void WWindow::setOriginY(qreal y)
{
    Q_D(WWindow); d->view->setOriginY(y);
}

//-------------------------------------------------------------------------------------------------

qreal WWindow::ratio() const
{
    Q_D(const WWindow); return d->view->ratio();
}

//-------------------------------------------------------------------------------------------------

qreal WWindow::zoom() const
{
    Q_D(const WWindow); return d->view->zoom();
}

void WWindow::setZoom(qreal zoom)
{
    Q_D(WWindow); d->view->setZoom(zoom);
}

//-------------------------------------------------------------------------------------------------

int WWindow::minimumWidth() const
{
    Q_D(const WWindow); return d->view->minimumWidth();
}

void WWindow::setMinimumWidth(int width)
{
    Q_D(WWindow); d->view->setMinimumWidth(width);
}

int WWindow::minimumHeight() const
{
    Q_D(const WWindow); return d->view->minimumHeight();
}

void WWindow::setMinimumHeight(int height)
{
    Q_D(WWindow); d->view->setMinimumHeight(height);
}

//-------------------------------------------------------------------------------------------------

int WWindow::maximumWidth() const
{
    Q_D(const WWindow); return d->view->maximumWidth();
}

void WWindow::setMaximumWidth(int width)
{
    Q_D(WWindow); d->view->setMaximumWidth(width);
}

int WWindow::maximumHeight() const
{
    Q_D(const WWindow); return d->view->maximumHeight();
}

void WWindow::setMaximumHeight(int height)
{
    Q_D(WWindow); d->view->setMaximumHeight(height);
}

//-------------------------------------------------------------------------------------------------

QRect WWindow::geometryNormal() const
{
    Q_D(const WWindow); return d->view->geometryNormal();
}

//-------------------------------------------------------------------------------------------------

bool WWindow::isMinimized() const
{
    Q_D(const WWindow); return d->view->isMinimized();
}

void WWindow::setMinimized(bool minimized)
{
    Q_D(WWindow); d->view->setMinimized(minimized);
}

//-------------------------------------------------------------------------------------------------

bool WWindow::isMaximized() const
{
    Q_D(const WWindow); return d->view->isMaximized();
}

void WWindow::setMaximized(bool maximized)
{
    Q_D(WWindow); d->view->setMaximized(maximized);
}

//-------------------------------------------------------------------------------------------------

bool WWindow::isFullScreen() const
{
    Q_D(const WWindow); return d->view->isFullScreen();
}

void WWindow::setFullScreen(bool fullScreen)
{
    Q_D(WWindow); d->view->setFullScreen(fullScreen);
}

//-------------------------------------------------------------------------------------------------

bool WWindow::isLocked() const
{
    Q_D(const WWindow); return d->view->isLocked();
}

//-------------------------------------------------------------------------------------------------

bool WWindow::isScaling() const
{
    Q_D(const WWindow); return d->view->isScaling();
}

void WWindow::setScaling(bool scaling)
{
    Q_D(WWindow); d->view->setScaling(scaling);
}

//-------------------------------------------------------------------------------------------------

bool WWindow::isActive() const
{
    Q_D(const WWindow); return d->view->isActive();
}

bool WWindow::isEntered() const
{
    Q_D(const WWindow); return d->view->isEntered();
}

bool WWindow::isDragging() const
{
    Q_D(const WWindow); return d->view->isDragging();
}

bool WWindow::isDragged() const
{
    Q_D(const WWindow); return d->view->isDragged();
}

bool WWindow::isResizing() const
{
    Q_D(const WWindow); return d->view->isResizing();
}

bool WWindow::isTouching() const
{
    Q_D(const WWindow); return d->view->isTouching();
}

//-------------------------------------------------------------------------------------------------

QPoint WWindow::mousePos() const
{
    Q_D(const WWindow); return d->view->mousePos();
}

//-------------------------------------------------------------------------------------------------

int WWindow::mouseX() const
{
    Q_D(const WWindow); return d->view->mouseX();
}

int WWindow::mouseY() const
{
    Q_D(const WWindow); return d->view->mouseY();
}

//-------------------------------------------------------------------------------------------------

Qt::CursorShape WWindow::mouseCursor() const
{
    Q_D(const WWindow); return d->view->mouseCursor();
}

//-------------------------------------------------------------------------------------------------

bool WWindow::opengl() const
{
    Q_D(const WWindow); return d->view->opengl();
}

void WWindow::setOpengl(bool enabled)
{
    Q_D(WWindow); d->view->setOpengl(enabled);
}

//-------------------------------------------------------------------------------------------------

bool WWindow::antialias() const
{
    Q_D(const WWindow); return d->view->antialias();
}

void WWindow::setAntialias(bool enabled)
{
    Q_D(WWindow); d->view->setAntialias(enabled);
}

//-------------------------------------------------------------------------------------------------

bool WWindow::vsync() const
{
    Q_D(const WWindow); return d->view->vsync();
}

void WWindow::setVsync(bool enabled)
{
    Q_D(WWindow); d->view->setVsync(enabled);
}

//-------------------------------------------------------------------------------------------------

bool WWindow::hoverEnabled() const
{
    Q_D(const WWindow); return d->view->hoverEnabled();
}

void WWindow::setHoverEnabled(bool enabled)
{
    Q_D(WWindow); d->view->setHoverEnabled(enabled);
}

//-------------------------------------------------------------------------------------------------

bool WWindow::fadeEnabled() const
{
    Q_D(const WWindow); return d->view->fadeEnabled();
}

void WWindow::setFadeEnabled(bool enabled)
{
    Q_D(WWindow); d->view->setFadeEnabled(enabled);
}

//-------------------------------------------------------------------------------------------------
// Mouse idle

bool WWindow::idleCheck() const
{
    Q_D(const WWindow); return d->view->idleCheck();
}

void WWindow::setIdleCheck(bool check)
{
    Q_D(WWindow); d->view->setIdleCheck(check);
}

//-------------------------------------------------------------------------------------------------

bool WWindow::idle() const
{
    Q_D(const WWindow); return d->view->idle();
}

void WWindow::setIdle(bool idle)
{
    Q_D(WWindow); d->view->setIdle(idle);
}

//-------------------------------------------------------------------------------------------------

int WWindow::idleDelay() const
{
    Q_D(const WWindow); return d->view->idleDelay();
}

void WWindow::setIdleDelay(int msec)
{
    Q_D(WWindow); d->view->setIdleDelay(msec);
}

//-------------------------------------------------------------------------------------------------
// Keys

bool WWindow::keyShiftPressed() const
{
    Q_D(const WWindow); return d->view->keyShiftPressed();
}

bool WWindow::keyControlPressed() const
{
    Q_D(const WWindow); return d->view->keyControlPressed();
}

bool WWindow::keyAltPressed() const
{
    Q_D(const WWindow); return d->view->keyAltPressed();
}

//-------------------------------------------------------------------------------------------------
// Screen

QRect WWindow::availableGeometry() const
{
    Q_D(const WWindow); return d->view->availableGeometry();
}

QRect WWindow::screenGeometry() const
{
    Q_D(const WWindow); return d->view->screenGeometry();
}

#else

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

bool WWindow::hoverActive() const
{
    Q_D(const WWindow); return d->viewport->hoverActive();
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

#ifdef QT_4
QDeclarativeDrag * WWindow::drag()
#else
QQuickDrag * WWindow::drag()
#endif
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

#endif

#endif // SK_NO_WINDOW
