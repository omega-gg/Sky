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

#if defined(QT_LATEST) && defined(Q_OS_ANDROID)
// Qt includes
#include <QtAndroid>
#endif

// Sk includes
#include <WControllerApplication>
#include <WControllerFile>
#include <WView>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include "WWindow_p.h"

WWindowPrivate::WWindowPrivate(WWindow * p) : WDeclarativeMouseAreaPrivate(p) {}

/* virtual */ WWindowPrivate::~WWindowPrivate()
{
    view->deleteLater();
}

//-------------------------------------------------------------------------------------------------

void WWindowPrivate::init()
{
    Q_Q(WWindow);

    icon = sk->icon();

    visible = true;

    hoverItem = NULL;

    q->setAcceptHoverEvents(true);

#ifdef QT_LATEST
    q->setFlag(QQuickItem::ItemAcceptsDrops);
#endif

    //---------------------------------------------------------------------------------------------
    // View

#ifdef SK_WIN_NATIVE
    view = new WView(q, NULL);
#elif defined(SK_MOBILE)
    // NOTE: This makes sure the full screen occupies the entire screen area including notches.
    view = new WView(q, NULL, Qt::FramelessWindowHint | Qt::MaximizeUsingFullscreenGeometryHint);
#elif defined(Q_OS_WIN)
    view = new WView(q, NULL, Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);
#else
    view = new WView(q, NULL, Qt::FramelessWindowHint);
#endif

#ifdef QT_4
    view->setWindowTitle(sk->name());
#else
    view->setTitle(sk->name());
#endif

    if (icon.isEmpty() == false)
    {
#ifdef QT_4
        view->setWindowIcon(QIcon(icon));
#else
        view->setIcon(QIcon(icon));
#endif
    }

    view->setVisible(true);

    //---------------------------------------------------------------------------------------------
    // Signals

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

#ifdef QT_4
    QObject::connect(view, SIGNAL(openglChanged()), q, SIGNAL(openglChanged()));
#endif

    QObject::connect(view, SIGNAL(antialiasChanged()), q, SIGNAL(antialiasChanged()));
    QObject::connect(view, SIGNAL(vsyncChanged    ()), q, SIGNAL(vsyncChanged    ()));

    QObject::connect(view, SIGNAL(hoverEnabledChanged()), q, SIGNAL(hoverEnabledChanged()));

    QObject::connect(view, SIGNAL(fadeEnabledChanged ()), q, SIGNAL(fadeEnabledChanged ()));
    QObject::connect(view, SIGNAL(fadeDurationChanged()), q, SIGNAL(fadeDurationChanged()));

    //---------------------------------------------------------------------------------------------

    QObject::connect(view, SIGNAL(idleCheckChanged()), q, SIGNAL(idleCheckChanged()));
    QObject::connect(view, SIGNAL(idleChanged     ()), q, SIGNAL(idleChanged     ()));
    QObject::connect(view, SIGNAL(idleDelayChanged()), q, SIGNAL(idleDelayChanged()));

    //---------------------------------------------------------------------------------------------

    QObject::connect(view, SIGNAL(mousePressed(WDeclarativeMouseEvent *)),
                     q,    SIGNAL(mousePressed(WDeclarativeMouseEvent *)));

    QObject::connect(view, SIGNAL(mouseReleased(WDeclarativeMouseEvent *)),
                     q,    SIGNAL(mouseReleased(WDeclarativeMouseEvent *)));

    QObject::connect(view, SIGNAL(mouseDoubleClicked(WDeclarativeMouseEvent *)),
                     q,    SIGNAL(mouseDoubleClicked(WDeclarativeMouseEvent *)));

    QObject::connect(view, SIGNAL(keyPressed(WDeclarativeKeyEvent *)),
                     q,    SIGNAL(keyPressed(WDeclarativeKeyEvent *)));

    QObject::connect(view, SIGNAL(keyReleased(WDeclarativeKeyEvent *)),
                     q,    SIGNAL(keyReleased(WDeclarativeKeyEvent *)));

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
}

//=================================================================================================
// WWindow
//=================================================================================================

#ifdef QT_4
/* explicit */ WWindow::WWindow(QDeclarativeItem * parent)
#else
/* explicit */ WWindow::WWindow(QQuickItem * parent)
#endif
    : WDeclarativeMouseArea(new WWindowPrivate(this), parent)
{
    Q_D(WWindow); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
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

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ bool WWindow::close()
{
    Q_D(WWindow);

    return d->view->close();
}

//-------------------------------------------------------------------------------------------------
// Focus

/* Q_INVOKABLE */ bool WWindow::getFocus() const
{
    Q_D(const WWindow);

#ifdef QT_4
    return (d->view->d_func()->scene->focusItem() != NULL);
#else
    return (d->view->focusObject() != NULL);
#endif
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WWindow::clearFocus()
{
    setFocus(true);
}

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
    Q_D(WWindow);

    QQuickItem * parent = d->view->activeFocusItem();

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
// Geometry

/* Q_INVOKABLE */ int WWindow::getScreenNumber() const
{
    Q_D(const WWindow); return d->view->getScreenNumber();
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

#ifdef QT_4

///* Q_INVOKABLE */ void WWindow::checkLeave(int msec)
//{
//    Q_D(WWindow); d->view->checkLeave(msec);
//}

#endif

//-------------------------------------------------------------------------------------------------
// Shot

#ifdef QT_4
/* Q_INVOKABLE */ QImage WWindow::takeShot(int x, int y, int width, int height) const
#else
/* Q_INVOKABLE */ QImage WWindow::takeShot(int x, int y, int width, int height)
#endif
{
#ifdef QT_4
    Q_D(const WWindow);
#else
    Q_D(WWindow);
#endif

    return d->view->takeShot(x, y, width, height);
}

#ifdef QT_4
/* Q_INVOKABLE */ bool WWindow::saveShot(const QString & fileName,
                                         int x, int y, int width, int height,
                                         const QString & format, int quality) const
#else
/* Q_INVOKABLE */ bool WWindow::saveShot(const QString & fileName,
                                         int x, int y, int width, int height,
                                         const QString & format, int quality)
#endif
{
#ifdef QT_4
    Q_D(const WWindow);
#else
    Q_D(WWindow);
#endif

    return d->view->saveShot(fileName, x, y, width, height, format, quality);
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

//-------------------------------------------------------------------------------------------------
// Static functions
//-------------------------------------------------------------------------------------------------

#if defined(QT_LATEST) && defined(Q_OS_ANDROID)

/* Q_INVOKABLE static */ void WWindow::hideSplash(int duration)
{
    QtAndroid::hideSplashScreen(duration);
}

#endif

//-------------------------------------------------------------------------------------------------

#ifdef QT_4
/* Q_INVOKABLE static */ QPixmap WWindow::takeItemShot(QGraphicsObject * item,
                                                       const QColor    & background)
#else
/* Q_INVOKABLE static */ QPixmap WWindow::takeItemShot(QQuickItem   * item,
                                                       const QColor & background)
#endif
{
    return WView::takeItemShot(item, background);
}

#ifdef QT_4
/* Q_INVOKABLE static */ bool WWindow::saveItemShot(const QString   & fileName,
                                                    QGraphicsObject * item,
                                                    const QString   & format, int quality,
                                                    const QColor    & background)
#else
/* Q_INVOKABLE static */ bool WWindow::saveItemShot(const QString & fileName,
                                                    QQuickItem    * item,
                                                    const QString & format, int quality,
                                                    const QColor  & background)
#endif
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

//-------------------------------------------------------------------------------------------------
// Protected events
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
/* virtual */ void WWindow::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
#else
/* virtual */ void WWindow::hoverEnterEvent(QHoverEvent * event)
#endif
{
    Q_D(WWindow);

    d->view->hoverEnter();

    WDeclarativeMouseArea::hoverEnterEvent(event);
}

#ifdef QT_4
/* virtual */ void WWindow::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
#else
/* virtual */ void WWindow::hoverLeaveEvent(QHoverEvent * event)
#endif
{
    Q_D(WWindow);

    d->view->hoverLeave();

    WDeclarativeMouseArea::hoverLeaveEvent(event);
}

//-------------------------------------------------------------------------------------------------

#ifdef QT_LATEST

/* virtual */ void WWindow::dragEnterEvent(QDragEnterEvent * event)
{
    Q_D(WWindow); d->view->dragEnterEvent(event);
}

/* virtual */ void WWindow::dragLeaveEvent(QDragLeaveEvent * event)
{
    Q_D(WWindow); d->view->dragLeaveEvent(event);
}

/* virtual */ void WWindow::dragMoveEvent(QDragMoveEvent * event)
{
    Q_D(WWindow); d->view->dragMoveEvent(event);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WWindow::dropEvent(QDropEvent * event)
{
    Q_D(WWindow); d->view->dropEvent(event);
}

#endif

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

WView * WWindow::view() const
{
    Q_D(const WWindow); return d->view;
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
    d->view->setWindowIcon(QIcon(icon));
#else
    d->view->setIcon(QIcon(icon));
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

    d->view->setVisible(visible);

    emit visibleChanged();
}

//-------------------------------------------------------------------------------------------------

qreal WWindow::opacity() const
{
    Q_D(const WWindow);

#ifdef QT_4
    return d->view->windowOpacity();
#else
    return d->view->opacity();
#endif
}

void WWindow::setOpacity(qreal opacity)
{
    Q_D(WWindow);

#ifdef QT_4
    if (d->view->windowOpacity() == opacity) return;

    d->view->setWindowOpacity(opacity);
#else
    if (d->view->opacity() == opacity) return;

    d->view->setOpacity(opacity);
#endif

    emit opacityChanged();
}

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

void WWindow::setLocked(bool locked)
{
    Q_D(WWindow);

    d->view->setLocked(locked);

    bool visible = d->view->isVisible();

    if (d->visible != visible)
    {
        d->visible = visible;

        emit visibleChanged();
    }
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

#ifdef QT_4

void WWindow::setOpengl(bool enabled)
{
    Q_D(WWindow); d->view->setOpengl(enabled);
}

#endif

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

int WWindow::fadeDuration() const
{
    Q_D(const WWindow); return d->view->fadeDuration();
}

void WWindow::setFadeDuration(int msec)
{
    Q_D(WWindow); d->view->setFadeDuration(msec);
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

#endif // SK_NO_WINDOW
